#include "stdafx.h"
#include "GetSCUCommand.h"
#include "filemanager/dicom/Network/DicomNetworkSCU.h"
#include "filemanager/dicom/Network/DIMSE_Data.h"
#include "filemanager/dicom/DicomDatasetFactory.h"
#include "filemanager/dicom/DicomInfomationModel.h"
#include "dcmtk/dcmnet/dfindscu.h"
#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/ofstd/offile.h"
#include "dcmtk/ofstd/ofcmdln.h"
#include "dcmtk/dcmnet/dfindscu.h"
#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmtls/tlsopt.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcpath.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include <qdir>

namespace fm
{
	GetSCUCommand::GetSCUCommand(
		DicomNetworkSCU* pSCU,
		std::wstring AETitle,
		DicomHostInfo hostInfo,
		std::wstring UID,
		std::wstring downloadDirectoryPath,
		std::wstring fileExtension,
		EQueryRetrieveLevel queryRetrieveLevel,
		int maxImageCount,
		DcmNetDownloadData* pDownloadInformationModel) :
		SingleSendSCUCommand(pSCU, AETitle, hostInfo),
		m_UID(UID),
		m_repositoryDirectoryPath(downloadDirectoryPath),
		m_fileExtension(fileExtension),
		m_queryRetrieveLevel(queryRetrieveLevel),
		m_maxProgressCount(maxImageCount),
		m_pDownloadData(pDownloadInformationModel),
		m_progressCount(0)
	{
	}

	std::vector<DicomPresentationContext> GetSCUCommand::GetPresentationContexts()
	{
		std::vector<DicomPresentationContext> context;
		/* 반드시 첫번쨰를 읽어온다. */
		EQueryModel queryModel = EQueryModel::StudyRoot;
		context.push_back(DicomPresentationContext(QuerySyntax[(int)queryModel].GetSyntax, ASC_SC_ROLE_DEFAULT));

		for (int i = 0; i < numberOfDcmLongSCUStorageSOPClassUIDs; ++i)
		{
			context.push_back(DicomPresentationContext(dcmLongSCUStorageSOPClassUIDs[i], ASC_SC_ROLE_SCP));
		}

		return context;
	}

	std::string GetSCUCommand::GetAbstractSyntax()
	{
		EQueryModel queryModel = EQueryModel::StudyRoot;

		return QuerySyntax[(int)queryModel].GetSyntax;
	}

	EDicomNetworkResult GetSCUCommand::SetupSendNetworkMessage(NetworkSendData* pNetSendData, DicomDataset* pDicomDataset)
	{
		SCUCommand::SetupSendNetworkMessage(pNetSendData, pDicomDataset);

		T_DIMSE_C_GetRQ* req = &(pNetSendData->Message.msg.CGetRQ);
		pNetSendData->Message.CommandField = DIMSE_C_GET_RQ;
		req->DataSetType = DIMSE_DATASET_PRESENT;
		req->Priority = DIMSE_PRIORITY_MEDIUM;

		return EDicomNetworkResult::SUCCESS;
	}

	EDicomNetworkResult GetSCUCommand::SendAndReceiveMessage(DicomDataset* pDicomDataset)
	{
		SetupQueryDataset();

		OFCondition ofcond = DIMSE_sendMessageUsingMemoryData(m_pNetSendData->Assoc,
			m_pNetSendData->PresentationContextID,
			&m_pNetSendData->Message,
			NULL,
			m_pSCUQueryDataset->Data(),
			NULL,
			NULL,
			NULL);

		if (ofcond.bad())
		{
			qInfo() << "fail to send GET message";
			return EDicomNetworkResult::FAIL_TO_SCU_SEND_DIMSE_GET;
		}

		if (ReceiveAndHandleMessageFromSCP() == false)
		{
			return EDicomNetworkResult::FAIL_TO_SCU_SEND_DIMSE_GET;
		}

		return EDicomNetworkResult::SUCCESS;
	}

	void GetSCUCommand::SetupQueryDataset()
	{
		m_pSCUQueryDataset = DicomDatasetFactory::CreateQueryLevelDataset(m_queryRetrieveLevel);
		if (m_queryRetrieveLevel == QR_LEVEL_STUDY)
		{
			m_pSCUQueryDataset->SetTagValue(DicomTagID::StudyInstanceUID, m_UID);
		}
		else
		{
			m_pSCUQueryDataset->SetTagValue(DicomTagID::SeriesInstanceUID, m_UID);
		}
	}

	bool GetSCUCommand::ReceiveAndHandleMessageFromSCP()
	{
		bool continueSession = true;

		qInfo() << "Download GET Loop Start";

		while (continueSession)
		{
			m_pNetRecvData->ReturnStatus = 0;
			m_pNetRecvData->ReceivedDataset = NULL;

			if (WaitToReceiveCommandFromSCP() == false)
			{
				continueSession = false;
			}
			else
			{
				/* Handle C-GET Response */
				if (IsGETResponseReceived())
				{
					HandleGETResponse(&continueSession);
				}
				/* Handle C-STORE Request */
				else if (IsStoreRequestReceived())
				{
					HandleStoreRequest(&continueSession);
				}
				else
				{
					qInfo() << "Received invalid command field : " << m_pNetRecvData->Message.CommandField;
					continueSession = false;
				}
			}
		}

		qInfo() << "Download GET Loop End";

		return true;
	}

	bool GetSCUCommand::WaitToReceiveCommandFromSCP()
	{
		OFString tempStr;
		DcmDataset* statusDetail = NULL;

		bzero((char*)&m_pNetRecvData->Message, sizeof(m_pNetRecvData->Message));

		OFCondition cond = DIMSE_receiveCommand(
			m_pNetSendData->Assoc, m_pNetSendData->BlockMode,
			m_hostInfo.Timeout(),
			&m_pNetRecvData->PresentationContextID,
			&m_pNetRecvData->Message, &statusDetail, NULL);

		if (cond.bad())
		{
			qInfo() << "Failed receiving DIMSE command: " << DimseCondition::dump(tempStr, cond).c_str();
			return false;
		}
		return true;
	}

	bool GetSCUCommand::IsGETResponseReceived()
	{
		return m_pNetRecvData->Message.CommandField == DIMSE_C_GET_RSP;
	}

	bool GetSCUCommand::IsStoreRequestReceived()
	{
		return m_pNetRecvData->Message.CommandField == DIMSE_C_STORE_RQ;
	}

	void GetSCUCommand::HandleGETResponse(bool* pContinueSession)
	{
		//qInfo() << "Received C-GET Response (" << DU_cgetStatusString(rsp.msg.CGetRSP.DimseStatus) << ")";
		DIC_US status = m_pNetRecvData->Message.msg.CGetRSP.DimseStatus;
		if (DICOM_PENDING_STATUS(status))
		{
			*pContinueSession = true;
		}
		else
		{
			*pContinueSession = false;
		}
	}

	void GetSCUCommand::HandleStoreRequest(bool* pContinueSession)
	{
		//qInfo() << "Received C-STORE Request (MsgID " << rsp.msg.CStoreRQ.MessageID << ")";
		if (m_pNetRecvData->Message.msg.CStoreRQ.DataSetType == DIMSE_DATASET_NULL)
		{
			qWarning() << "Incoming C-STORE with no dataset, trying to receive one anyway";
		}

		if (WaitToReceive_STORE_Request() == false)
		{
			*pContinueSession = false;
			return;
		}

		std::wstring dcmStoreFilePath;
		DicomDataset dcmDataset;
		if (UpdateDataFromReceivedDataset(&dcmStoreFilePath, &dcmDataset))
		{
			if (SaveDicomFileToDisk(dcmStoreFilePath))
			{
				if (m_pDownloadData)
				{
					m_pDownloadData->DicomDatasetList.push_back(dcmDataset);
				}

				/* GET Store 진행 상태 Update*/
				Update_Download_Inprogress(dcmStoreFilePath);
			}
		}

		if (Send_STORE_Response() == false)
		{
			*pContinueSession = false;
			return;
		}
	}

	bool GetSCUCommand::WaitToReceive_STORE_Request()
	{
		/* Block 된다. */
		OFCondition result = DIMSE_receiveDataSetInMemory(
			m_pNetSendData->Assoc,
			m_pNetSendData->BlockMode,
			m_hostInfo.Timeout(),
			&m_pNetRecvData->PresentationContextID,
			&m_pNetRecvData->ReceivedDataset, NULL, NULL);

		if (result.bad())
		{
			qWarning() << "fail to receive store request";
			m_pNetRecvData->ReturnStatus = STATUS_STORE_Error_CannotUnderstand;
			return false;
		}

		return true;
	}

	bool GetSCUCommand::SaveDicomFileToDisk(std::wstring dcmSaveFilePath)
	{
		if (m_pNetRecvData->ReceivedDataset == NULL)
		{
			qWarning() << "incoming object is null";
			m_pNetRecvData->ReturnStatus = STATUS_STORE_Error_CannotUnderstand;
			return false;
		}

		if (IsExist_SOPClassUID_Or_SOPInstanceUID() == false)
		{
			qWarning() << "Cannot store received object: either SOP Instance or SOP Class UID not present";
			m_pNetRecvData->ReturnStatus = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
			delete m_pNetRecvData->ReceivedDataset;
			m_pNetRecvData->ReceivedDataset = NULL;
			return false;
		}

		if (OFStandard::fileExists(dcmSaveFilePath.data()))
		{
			qInfo() << "DICOM file already exists, overwriting: " << QString::fromStdWString(dcmSaveFilePath);
		}

		DcmFileFormat dcmff(m_pNetRecvData->ReceivedDataset, OFFalse);
		OFCondition cond = dcmff.saveFile(dcmSaveFilePath.data());
		if (cond.bad())
		{
			qWarning() << "cannot write DICOM file: " << dcmSaveFilePath.c_str();
			m_pNetRecvData->ReturnStatus = STATUS_STORE_Refused_OutOfResources;
			OFStandard::deleteFile(dcmSaveFilePath.data());

			return false;
		}

		m_pNetRecvData->ReturnStatus = STATUS_Success;
		return true;
	}

	bool GetSCUCommand::IsExist_SOPClassUID_Or_SOPInstanceUID()
	{
		OFString sopClassUID;
		OFString sopInstanceUID;
		OFCondition cond = m_pNetRecvData->ReceivedDataset->findAndGetOFString(DCM_SOPClassUID, sopClassUID);
		if (cond.bad())
		{
			cond = m_pNetRecvData->ReceivedDataset->findAndGetOFString(DCM_SOPInstanceUID, sopInstanceUID);
			if (cond.bad())
			{
				return false;
			}
		}

		return true;
	}

	bool GetSCUCommand::UpdateDataFromReceivedDataset(std::wstring* pDCMSaveFilePath, DicomDataset* pDcmDataset)
	{
		OFString sopClassUID, sopInstanceUID;
		OFCondition cond = GetDatasetInfo(m_pNetRecvData->ReceivedDataset, sopClassUID, sopInstanceUID);
		if (cond.bad())
		{
			qWarning() << "fail to get dataset info from response dataset";
			return false;
		}

		// Create unique filename
		if (sopClassUID.empty() || sopInstanceUID.empty())
		{
			qWarning() << "SOP class UID or SOP Instance UID is empty";
			return false;
		}

		OFString studyInstanceUID;
		m_pNetRecvData->ReceivedDataset->findAndGetOFString(DCM_StudyInstanceUID, studyInstanceUID);
		if (studyInstanceUID.empty())
		{
			qWarning() << "receive dataset study instance UID is empty.";
			return false;
		}

		OFString seriesInstanceUID;
		m_pNetRecvData->ReceivedDataset->findAndGetOFString(DCM_SeriesInstanceUID, seriesInstanceUID);
		if (seriesInstanceUID.empty())
		{
			qWarning() << "receive dataset series instance UID is empty.";
			return false;
		}

		OFString downloadDirpath = StringUtil::WideStringToOFString(m_repositoryDirectoryPath) + "/" + studyInstanceUID + "/" + seriesInstanceUID;
		QDir dir;
		if (!dir.exists(downloadDirpath.c_str()))
		{
			dir.mkpath(downloadDirpath.c_str());
		}

		OFString filename = dcmSOPClassUIDToModality(sopClassUID.c_str());
		filename += ".";
		filename += sopInstanceUID;
		filename += StringUtil::WideStringToOFString(m_fileExtension);
		OFString filepath;
		OFStandard::combineDirAndFilename(filepath, downloadDirpath, filename, OFTrue);

		*pDCMSaveFilePath = StringUtil::OFStringToWideString(filepath);
		qInfo() << "save file path : " << QString::fromStdWString(*pDCMSaveFilePath);

		*pDcmDataset = DicomDataset(m_pNetRecvData->ReceivedDataset);
		pDcmDataset->SetValue_Extension(DCM_EXT_FILE_PATH, *pDCMSaveFilePath);

		return true;
	}

	bool GetSCUCommand::Send_STORE_Response()
	{
		T_DIMSE_Message response;
		bzero((char*)&response, sizeof(response));
		T_DIMSE_C_StoreRSP& storeRsp = response.msg.CStoreRSP;

		response.CommandField = DIMSE_C_STORE_RSP;
		storeRsp.MessageIDBeingRespondedTo = m_pNetRecvData->Message.msg.CStoreRQ.MessageID;
		storeRsp.DimseStatus = m_pNetRecvData->ReturnStatus;
		storeRsp.DataSetType = DIMSE_DATASET_NULL;

		OFStandard::strlcpy(
			storeRsp.AffectedSOPClassUID, m_pNetRecvData->Message.msg.CStoreRQ.AffectedSOPClassUID, sizeof(storeRsp.AffectedSOPClassUID));
		OFStandard::strlcpy(
			storeRsp.AffectedSOPInstanceUID, m_pNetRecvData->Message.msg.CStoreRQ.AffectedSOPInstanceUID, sizeof(storeRsp.AffectedSOPInstanceUID));
		storeRsp.opts = O_STORE_AFFECTEDSOPCLASSUID | O_STORE_AFFECTEDSOPINSTANCEUID;

		OFCondition cond = DIMSE_sendMessageUsingMemoryData(
			m_pNetSendData->Assoc,
			m_pNetRecvData->PresentationContextID,
			&response,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL);

		qInfo() << "Sending C-STORE Response (" << DU_cstoreStatusString(m_pNetRecvData->ReturnStatus) << ")";

		if (cond.bad())
		{
			OFString tempStr;
			qInfo() << "Failed sending C-STORE response: " << DimseCondition::dump(tempStr, cond).data();
			return false;
		}

		return true;
	}

	OFCondition GetSCUCommand::GetDatasetInfo(DcmDataset* dataset, OFString& sopClassUID, OFString& sopInstanceUID)
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

	void GetSCUCommand::Update_Download_Inprogress(std::wstring dcmStoreFilePath)
	{
		fm::DicomNetworkSCUStatus status(EDcmNetworkDIMSEType::GET_SCU);
		status.InProgressCount = ++m_progressCount;
		status.MaxProgressCount = m_maxProgressCount;

		emit m_pSCU->updateSCUStatus(status);
	}
}
