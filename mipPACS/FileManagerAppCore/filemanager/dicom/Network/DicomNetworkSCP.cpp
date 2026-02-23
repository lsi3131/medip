#include "stdafx.h"
#include "DicomNetworkSCP.h"
#include "DicomNetworkSCU.h"
#include "DicomHostInfo.h"
#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/ofstd/offile.h"
#include "dcmtk/ofstd/ofcmdln.h"
#include "dcmtk/dcmnet/dfindscu.h"
#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmtls/tlsopt.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcpath.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcmetinf.h"
#include "filemanager/dicom/DicomDataset.h"
#include <QDir>

namespace fm
{
	struct StoreSCPCallBackData
	{
		DicomNetworkSCP* SCP;
		DcmFileFormat* DcmFileFormat;
	};

	static OFCondition GetDatasetInfo(DcmDataset* dataset, OFString& sopClassUID, OFString& sopInstanceUID)
	{
		OFCondition status = EC_IllegalParameter;
		sopClassUID.clear();
		sopInstanceUID.clear();
		E_TransferSyntax transferSyntax = EXS_Unknown;
		if (dataset != NULL)
		{
			dataset->findAndGetOFString(DCM_SOPClassUID, sopClassUID);
			dataset->findAndGetOFString(DCM_SOPInstanceUID, sopInstanceUID);
			transferSyntax = dataset->getOriginalXfer();

			if (sopClassUID.empty())
				status = NET_EC_InvalidSOPClassUID;
			else if (sopInstanceUID.empty())
				status = NET_EC_InvalidSOPInstanceUID;
			else if (transferSyntax == EXS_Unknown)
				status = NET_EC_UnknownTransferSyntax;
			else
				status = EC_Normal;
		}

		return status;
	}

	//=======================================
	//		ThreadDicomNetworkSCP
	//=======================================
	ThreadDicomNetworkSCP::ThreadDicomNetworkSCP(DicomNetworkSCP* p) :
		m_p(p)
	{
	}

	void ThreadDicomNetworkSCP::run()
	{
		if (m_p->StartServer() == false)
		{
			qWarning() << "fail to store scp";
			return;
		}
	}


	//==============================================
	//			CallBack Functions
	//==============================================
	void DicomNetworkSCP::StoreSCPCallback(
		void* pCallBackData,
		T_DIMSE_StoreProgress* progress,
		T_DIMSE_C_StoreRQ* req,
		char* imageFileName,
		DcmDataset** imageDataSet,
		T_DIMSE_C_StoreRSP* rsp,
		DcmDataset** statusDetail)
	{
		/* Store End 되었을 경우 파일 저장 및 상태 Update */
		if (progress->state == DIMSE_StoreEnd)
		{
			StoreSCPCallBackData* p = (StoreSCPCallBackData*)pCallBackData;

			if ((imageDataSet != NULL) && (*imageDataSet != NULL))
			{
				OFString sopClassUID, sopInstanceUID;
				OFCondition cond = GetDatasetInfo(p->DcmFileFormat->getDataset(), sopClassUID, sopInstanceUID);
				if (cond.bad())
				{
					qWarning() << "fail to get dataset info from response dataset";
					return;
				}

				// Create unique filename
				if (sopClassUID.empty() || sopInstanceUID.empty())
				{
					qWarning() << "SOP class UID or SOP Instance UID is empty";
					return;
				}

				OFString studyInstanceUID;
				p->DcmFileFormat->getDataset()->findAndGetOFString(DCM_StudyInstanceUID, studyInstanceUID);
				if (studyInstanceUID.empty())
				{
					qWarning() << "receive dataset study instance UID is empty.";
					return;
				}

				OFString seriesInstanceUID;
				p->DcmFileFormat->getDataset()->findAndGetOFString(DCM_SeriesInstanceUID, seriesInstanceUID);
				if (seriesInstanceUID.empty())
				{
					qWarning() << "receive dataset series instance UID is empty.";
					return;
				}

				OFString downloadDirpath = StringUtil::WideStringToOFString(p->SCP->m_repositoryDirPath) + "/" + studyInstanceUID + "/" + seriesInstanceUID;
				QDir dir;
				if (!dir.exists(downloadDirpath.c_str()))
				{
					dir.mkpath(downloadDirpath.c_str());
				}

				OFString filename = dcmSOPClassUIDToModality(sopClassUID.c_str());
				filename += ".";
				filename += sopInstanceUID;
				filename += StringUtil::WideStringToOFString(p->SCP->m_fileExtenstion);
				OFString filepath;
				OFStandard::combineDirAndFilename(filepath, downloadDirpath, filename, OFTrue);

				std::wstring downloadFilePath = StringUtil::OFStringToWideString(filepath);

				qInfo() << "download file path : " << QString::fromStdWString(downloadFilePath);

				if (p->SCP->SaveFileToDisk(p->DcmFileFormat, downloadFilePath))
				{
					DicomNetworkStoreSCPStatus status;
					status.DcmDataset = DicomDataset(p->DcmFileFormat->getDataset());
					status.DcmDataset.SetValue_Extension(DCM_EXT_FILE_PATH, downloadFilePath);

					p->SCP->UpdateStatusToObserver(status);
				}
			}
		}
	}

	//=======================================
	//		DicomNetworkSCP
	//=======================================
	DicomNetworkSCP::DicomNetworkSCP() :
		m_isAbortServer(false),
		m_thread(this),
		m_net(nullptr),
		m_assoc(nullptr)
	{
		m_pSCUAbort = new DicomNetworkSCU();
		m_pSCUAbort->SetAETitle(L"ABORT_SCU");
		m_SCPRunningStatus = DicomNetworkSCPStatus::NOT_RUNNING;
	}

	DicomNetworkSCP::~DicomNetworkSCP()
	{
		/* Store Thread 대기 & wait 진행 */
		delete m_pSCUAbort;
	}


	EDicomNetworkResult DicomNetworkSCP::InitializeNetwork()
	{
		qInfo() << "SCP network initialize start";
		if (IsNetworkInitialized())
		{
			qWarning() << "SCP network is already initialized";
			return EDicomNetworkResult::FAIL_TO_STORE_SCP_INITALIZE_LISTNER;
		}

		if (m_ListenerInfo.IsEmpty())
		{
			qWarning() << "SCP Listner is empty";
			return EDicomNetworkResult::FAIL_TO_STORE_SCP_INITALIZE_LISTNER;
		}

		OFCondition ofcond;
		ofcond = ASC_initializeNetwork(NET_ACCEPTOR, m_ListenerInfo.Port(), m_ListenerInfo.Timeout(), &m_net);
		if (ofcond.bad())
		{
			qWarning() << "SCP initialize network fail";
			return EDicomNetworkResult::FAIL_TO_STORE_SCP_INITALIZE_LISTNER;
		}

		qInfo() << "SCP network initialize finish";
		return EDicomNetworkResult::SUCCESS;
	}

	EDicomNetworkResult DicomNetworkSCP::AbortWaitAndDropNetwork()
	{
		/* Store Loop가 실행 중이면 종료한다. */
		Abort_Server()->wait();

		return DropNetwork();
	}

	void DicomNetworkSCP::SetListenerInfo(DicomListenerInfo& listener)
	{
		m_ListenerInfo = listener;

		DicomHostInfo localSCPHostInfo = DicomHostInfo::LocalHostInfo(
			listener.AETitle(),
			listener.Port(),
			listener.Timeout()
		);

		m_pSCUAbort->SetHostInfoQueryRetrieve(localSCPHostInfo);

		qInfo() <<
			"set new listner network info. Application Entity : " << QString::fromStdWString(m_ListenerInfo.AETitle()) <<
			", Port :" << m_ListenerInfo.Port() <<
			", Time out :" << m_ListenerInfo.Timeout();
	}

	void DicomNetworkSCP::SetDownloadInfo(std::wstring outputDirpath, std::wstring filenameExtenstion)
	{
		qInfo() <<
			"set output directory path = " << QString::fromStdWString(outputDirpath) <<
			", file name extension :" << QString::fromStdWString(filenameExtenstion);

		m_repositoryDirPath = outputDirpath;
		m_fileExtenstion = filenameExtenstion;
	}

	bool DicomNetworkSCP::IsNetworkInitialized()
	{
		return m_net != nullptr;
	}

	bool DicomNetworkSCP::IsStoreSCPRunning()
	{
		if (IsNetworkInitialized() == false)
		{
			return false;
		}

		if (
			(m_SCPRunningStatus == DicomNetworkSCPStatus::STARTED) ||
			(m_SCPRunningStatus == DicomNetworkSCPStatus::WAIT_ASSOC) ||
			(m_SCPRunningStatus == DicomNetworkSCPStatus::DOWNLOADING) ||
			(m_SCPRunningStatus == DicomNetworkSCPStatus::RELEASE_ASSOC)
			)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool DicomNetworkSCP::StartServer()
	{
		qInfo() << "Store SCP is RUNNING";

		EDicomNetworkResult result = EDicomNetworkResult::SUCCESS;
		m_isAbortServer = false;

		/* StoreSCP 시작 */
		if (IsNetworkInitialized() == false)
		{
			qWarning() << "network is not initialized";
			return false;
		}

		m_SCPRunningStatus = DicomNetworkSCPStatus::STARTED;
		emit updateStatus(m_SCPRunningStatus);
		//==== Store SCP running ==== 
		do
		{
			if (m_isAbortServer)
			{
				break;
			}

			/*
				Blocking Mode로 receiveAssociation 실행
				SCU로부터 연결이 될 때까지 blocking 진행한다.
			*/
			m_SCPRunningStatus = DicomNetworkSCPStatus::WAIT_ASSOC;
			emit updateStatus(m_SCPRunningStatus);
			result = Wait_And_SetupAssoc();
			if (m_isAbortServer || (result != EDicomNetworkResult::SUCCESS))
			{
				ReleaseAssoc();
				break;
			}

			/* Data Download Loop. 실제로 SCU로부터 Download 진행.*/
			m_SCPRunningStatus = DicomNetworkSCPStatus::DOWNLOADING;
			emit updateStatus(m_SCPRunningStatus);
			result = Downloading_Loop();
			if (m_isAbortServer || (result != EDicomNetworkResult::SUCCESS))
			{
				ReleaseAssoc();
				break;
			}

			m_SCPRunningStatus = DicomNetworkSCPStatus::RELEASE_ASSOC;
			emit updateStatus(m_SCPRunningStatus);

			ReleaseAssoc();
		} while (true);
		//==== Store SCP finish ==== 


		m_SCPRunningStatus = DicomNetworkSCPStatus::DROP_NETWORK;
		emit updateStatus(m_SCPRunningStatus);

		DropNetwork();

		m_SCPRunningStatus = DicomNetworkSCPStatus::FINISHED;
		emit updateStatus(m_SCPRunningStatus);

		if (m_isAbortServer)		//Abort
		{
			qInfo() << "Stored SCP is Finished - Abort";
			return true;
		}
		else if (result != EDicomNetworkResult::SUCCESS)
		{
			qWarning() << "Stored SCP is Finished - Error";
			return false;
		}
		else    //Success
		{
			qInfo() << "Stored SCP is Finished - Success";
			return true;
		}
	}

	QThread* DicomNetworkSCP::StartServer_Async()
	{
		if (m_thread.isRunning())
		{
			qWarning() << "thread is already running";
		}
		else
		{
			m_thread.start();
		}

		return &m_thread;
	}

	bool DicomNetworkSCP::InitializeNetworkAndStartServer_Async(QThread** ppWorkingThread)
	{
		EDicomNetworkResult result = InitializeNetwork();
		if (result != EDicomNetworkResult::SUCCESS)
		{
			return false;
		}

		QThread* pThread = StartServer_Async();
		if (ppWorkingThread)
		{
			*ppWorkingThread = pThread;
		}

		return true;
	}

	QThread* DicomNetworkSCP::Abort_Server()
	{
		if (IsStoreSCPRunning() == false)
		{
			qInfo() << "StoreSCP is not running. skip to abort StoreSCP";
			return &m_thread;
		}

		qInfo() << "abort to StoreSCP loop requested";
		m_isAbortServer = true;
		m_pSCUAbort->AssocNegoQueryRetrieve();		/* Wait_And_SetupAssoc의 Wait를 종료시킨다. */

		return &m_thread;
	}

	void DicomNetworkSCP::RegisterObserver(IDicomNetworkSCPObserver* observer)
	{
		auto it = std::find(m_observerList.begin(), m_observerList.end(), observer);
		if (it == m_observerList.end())
		{
			m_observerList.push_back(observer);
		}
	}

	void DicomNetworkSCP::UnregisterObserver(IDicomNetworkSCPObserver* observer)
	{
		auto it = std::find(m_observerList.begin(), m_observerList.end(), observer);
		if (it != m_observerList.end())
		{
			m_observerList.erase(it);
		}
	}

	void DicomNetworkSCP::UpdateStatusToObserver(DicomNetworkStoreSCPStatus& status)
	{
		for (auto& o : m_observerList)
		{
			o->onUpdateStatus_StoreSCP(status);
		}
	}


	EDicomNetworkResult DicomNetworkSCP::Wait_And_SetupAssoc()
	{
		qInfo() << "Wait And Setup Assocation RUNNING";
		OFCondition ofcond;
		char buffer[BUFSIZ];
		OFString temp_str;
		OFCmdUnsignedInt maxPDU = ASC_DEFAULTMAXPDU;
		OFBool secureConnection = false;
		DUL_BLOCKOPTIONS block = DUL_BLOCK;

		ofcond = ASC_receiveAssociation(m_net, &m_assoc, maxPDU, NULL, NULL, secureConnection, block);
		if (m_isAbortServer)
		{
			qInfo() << "Abort - SCP Receive Association";
			return EDicomNetworkResult::SUCCESS;
		}

		if (ofcond.bad())
		{
			qWarning() << "Receiving Association failed: " << ofcond.text();
			return EDicomNetworkResult::FAIL_TO_STORE_SCP_RECEIVE_ASSOCIATION;
		}

		std::vector<DicomPresentationContext> presentationContexts;
		presentationContexts.push_back(DicomPresentationContext(UID_VerificationSOPClass, ASC_SC_ROLE_DEFAULT));
		for (int i = 0; i < numberOfDcmAllStorageSOPClassUIDs; ++i)
		{
			presentationContexts.push_back(DicomPresentationContext(dcmAllStorageSOPClassUIDs[i], ASC_SC_ROLE_DEFAULT));
		}

		for (int i = 0; i < presentationContexts.size(); ++i)
		{
			const char* abstractSyntaxes[] =
			{
				presentationContexts[i].AbstractSyntaxName.c_str()
			};

			ofcond = ASC_acceptContextsWithPreferredTransferSyntaxes(
				m_assoc->params,
				abstractSyntaxes, 1,
				presentationContexts[i].TransferSyntaxes,
				presentationContexts[i].NumTransferSyntaxes
			);
			if (ofcond.bad())
			{
				qWarning() << "fail to accept context with preferred tranfer syntaxes : " << DimseCondition::dump(temp_str, ofcond).c_str();
				return EDicomNetworkResult::FAIL_TO_STORE_SCP_RECEIVE_ASSOCIATION;
			}
		}

		OFString listenerAETitle = StringUtil::WideStringToOFString(m_ListenerInfo.AETitle());
		ofcond = ASC_setAPTitles(m_assoc->params, NULL, NULL, listenerAETitle.c_str()
		);

		ofcond = ASC_getApplicationContextName(m_assoc->params, buffer, sizeof(buffer));
		if ((ofcond.bad()) || strcmp(buffer, UID_StandardApplicationContext))
		{
			T_ASC_RejectParameters rej =
			{
				ASC_RESULT_REJECTEDPERMANENT,
				ASC_SOURCE_SERVICEUSER,
				ASC_REASON_SU_APPCONTEXTNAMENOTSUPPORTED
			};

			qWarning() << "Association Rejected: Bad Application Context Name: " << buffer;
			ofcond = ASC_rejectAssociation(m_assoc, &rej);
			return EDicomNetworkResult::FAIL_TO_STORE_SCP_RECEIVE_ASSOCIATION;
		}

		ofcond = ASC_acknowledgeAssociation(m_assoc);
		if (ofcond.bad())
		{
			qWarning() << DimseCondition::dump(temp_str, ofcond).c_str();
			return EDicomNetworkResult::FAIL_TO_STORE_SCP_RECEIVE_ASSOCIATION;
		}

		if (ASC_countAcceptedPresentationContexts(m_assoc->params) == 0)
		{
			return EDicomNetworkResult::FAIL_TO_STORE_SCP_RECEIVE_ASSOCIATION;
		}

		return EDicomNetworkResult::SUCCESS;
	}

	EDicomNetworkResult DicomNetworkSCP::Downloading_Loop()
	{
		T_DIMSE_Message msg;
		T_ASC_PresentationContextID presentationContextID = 0;
		DcmDataset* statusDetail = NULL;
		T_DIMSE_C_StoreRQ* req;
		OFString temp_str;
		OFBool useMetaheader = OFTrue;
		T_DIMSE_BlockingMode blockMode = DIMSE_BLOCKING;
		EDicomNetworkResult result = EDicomNetworkResult::SUCCESS;
		OFCondition ofcond;

		qInfo() << "store scp download loop will be RUNNING";

		while (
			(result == EDicomNetworkResult::SUCCESS) &&
			(ofcond == EC_Normal || ofcond == DIMSE_NODATAAVAILABLE || ofcond == DIMSE_OUTOFRESOURCES))
		{
			if (m_isAbortServer)
			{
				qInfo() << "Abort - SCP Downloading Loop";
				break;
			}

			ofcond = DIMSE_receiveCommand(m_assoc, DIMSE_BLOCKING, 0, &presentationContextID, &msg, &statusDetail);

			if (statusDetail != NULL)
			{
				delete statusDetail;
			}

			if (ofcond == EC_Normal)
			{
				switch (msg.CommandField)
				{
				case DIMSE_C_STORE_RQ:
				{
					req = &msg.msg.CStoreRQ;

					StoreSCPCallBackData callbackData;
					DcmFileFormat dcmff;
					callbackData.DcmFileFormat = &dcmff;
					callbackData.SCP = this;

					const char* AETitle = m_assoc->params->DULparams.callingAPTitle;
					if (AETitle)
					{
						dcmff.getMetaInfo()->putAndInsertString(DCM_SourceApplicationEntityTitle, AETitle);
					}

					DcmDataset* dset = dcmff.getDataset();
					ofcond = DIMSE_storeProvider(
						m_assoc,
						presentationContextID,
						req,
						nullptr,
						useMetaheader,
						&dset,
						StoreSCPCallback,
						&callbackData,
						blockMode,
						m_ListenerInfo.Timeout());

					if (ofcond.bad())
					{
						qWarning() << "Store SCP Failed: " << DimseCondition::dump(temp_str, ofcond).c_str();
						result = EDicomNetworkResult::FAIL_TO_STORE_SCP_DOWNLOADING;
					}
					break;
				}
				default:
					qWarning() << "invalid command field is comming : " << msg.CommandField;
					result = EDicomNetworkResult::FAIL_TO_STORE_SCP_DOWNLOADING;
					break;
				}
			}
		}

		qInfo() << "store scp download loop is finished";
		return result;
	}

	EDicomNetworkResult DicomNetworkSCP::ReleaseAssoc()
	{
		OFString temp_str;
		OFCondition ofcond;

		ofcond = ASC_abortAssociation(m_assoc);
		if (ofcond.bad())
		{
			qWarning() << "fail to acknoledge release :" << DimseCondition::dump(temp_str, ofcond).c_str();
			return EDicomNetworkResult::FAIL_TO_STORE_SCP_RELEASE_ASSOC;
		}

		ofcond = ASC_dropSCPAssociation(m_assoc);
		if (ofcond.bad())
		{
			qWarning() << "fail to drop SCP association : " << DimseCondition::dump(temp_str, ofcond).c_str();
			return EDicomNetworkResult::FAIL_TO_STORE_SCP_RELEASE_ASSOC;
		}

		ofcond = ASC_destroyAssociation(&m_assoc);
		if (ofcond.bad())
		{
			qWarning() << "fail to destory association : " << DimseCondition::dump(temp_str, ofcond).c_str();
			return EDicomNetworkResult::FAIL_TO_STORE_SCP_RELEASE_ASSOC;
		}

		m_assoc = nullptr;

		return EDicomNetworkResult::SUCCESS;
	}

	EDicomNetworkResult DicomNetworkSCP::DropNetwork()
	{
		OFString temp_str;
		OFCondition ofcond;

		/* Network 오브젝트 Drop*/
		ofcond = ASC_dropNetwork(&m_net);
		if (ofcond.bad())
		{
			qWarning() << "fail to drop network : " << DimseCondition::dump(temp_str, ofcond).c_str();
			return EDicomNetworkResult::FAIL_TO_STORE_SCP_DROP_NETWORK;
		}

		m_net = nullptr;

		return EDicomNetworkResult::SUCCESS;
	}

	bool DicomNetworkSCP::SaveFileToDisk(DcmFileFormat* dcmff, std::wstring filepath)
	{
		OFString filePath = StringUtil::WideStringToOFString(filepath);
		E_TransferSyntax xfer = EXS_LittleEndianExplicit;
		E_EncodingType encodingType = EET_ExplicitLength;
		E_GrpLenEncoding grpLenEncoding = EGL_recalcGL;
		E_PaddingEncoding paddingEncoding = EPD_withoutPadding;
		int padLength = 0;
		int subPadLength = 0;
		E_FileWriteMode writeMode = EWM_fileformat;

		/* 지정된 경로로 파일 저장 */
		qInfo() << "save files from PACS(filepath : " << filePath.c_str() << ")";
		OFCondition cond = dcmff->saveFile(
			filePath.c_str(), xfer, encodingType,
			grpLenEncoding, paddingEncoding,
			padLength, subPadLength,
			writeMode
		);

		if (cond.bad())
		{
			qWarning() << "cannot write DICOM file: " << filePath.c_str() << ":" << cond.text();
			OFStandard::deleteFile(filePath);
			return false;
		}

		return true;
	}


}

