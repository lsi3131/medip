#include "stdafx.h"
#include "MoveSCUCommand.h"
#include "filemanager/dicom/Network/DicomNetworkSCU.h"
#include "filemanager/dicom/Network/DicomNetworkSCP.h"
#include "filemanager/dicom/Network/DIMSE_Data.h"
#include "filemanager/dicom/DicomDatasetFactory.h"
#include "dcmtk/dcmnet/dfindscu.h"
#include "dcmtk/dcmnet/diutil.h"

namespace fm
{
	static void MoveRetrieveCallback(void* pCallbackData,
		T_DIMSE_C_MoveRQ* request,
		int responseCount,
		T_DIMSE_C_MoveRSP* response)
	{
		/* Debug 용 */
		//qInfo() << "Received Move Response : " << DU_cmoveStatusString(response->DimseStatus);
	}


	MoveSCUCommand::MoveSCUCommand(DicomNetworkSCU* pSCU,
		std::wstring AETitle, DicomHostInfo hostInfo,
		std::wstring studyInstanceUID,
		DicomNetworkSCP* pSCP,
		EQueryRetrieveLevel queryRetrieveLevel,
		int maxImageCount,
		DcmNetDownloadData* pDownloadData
	) :
		SingleSendSCUCommand(pSCU, AETitle, hostInfo),
		m_pSCP(pSCP),
		m_UID(studyInstanceUID),
		m_pDownloadData(pDownloadData),
		m_progressCount(0),
		m_maxProgressCount(maxImageCount),
		m_queryRetrieveLevel(queryRetrieveLevel)
	{
		m_pSCP->RegisterObserver(this);
	}

	MoveSCUCommand::~MoveSCUCommand()
	{
		m_pSCP->UnregisterObserver(this);
	}

	std::vector<DicomPresentationContext> MoveSCUCommand::GetPresentationContexts()
	{
		std::vector<DicomPresentationContext> context;

		EQueryModel queryModel = EQueryModel::StudyRoot;

		context.push_back(DicomPresentationContext(QuerySyntax[(int)queryModel].FindSyntax, ASC_SC_ROLE_DEFAULT));
		context.push_back(DicomPresentationContext(QuerySyntax[(int)queryModel].MoveSyntax, ASC_SC_ROLE_DEFAULT));

		return context;
	}

	std::string MoveSCUCommand::GetAbstractSyntax()
	{
		EQueryModel queryModel = EQueryModel::StudyRoot;

		return QuerySyntax[(int)queryModel].MoveSyntax;
	}

	EDicomNetworkResult MoveSCUCommand::SetupSendNetworkMessage(NetworkSendData* pNetSendData, DicomDataset* pDicomDataset)
	{
		SCUCommand::SetupSendNetworkMessage(pNetSendData, pDicomDataset);

		T_DIMSE_C_MoveRQ* req = &(pNetSendData->Message.msg.CMoveRQ);
		pNetSendData->Message.CommandField = DIMSE_C_MOVE_RQ;
		req->DataSetType = DIMSE_DATASET_PRESENT;
		req->Priority = DIMSE_PRIORITY_MEDIUM;

		/* STORESCP 저장소 설정 */
		std::string AETitle = StringUtil::WideToMulitiByte(m_pSCP->AETitle());
		OFStandard::strlcpy(req->MoveDestination, AETitle.c_str(), sizeof(req->MoveDestination));

		return EDicomNetworkResult::SUCCESS;
	}

	EDicomNetworkResult MoveSCUCommand::SendAndReceiveMessage(DicomDataset* pDicomDataset)
	{
		SetupQueryDataset();

		OFCondition ofcond;
		T_DIMSE_C_MoveRSP rsp;
		DcmDataset* statusDetail = NULL;

		ofcond = DIMSE_moveUser(m_pNetSendData->Assoc,
			m_pNetSendData->PresentationContextID,
			&m_pNetSendData->Message.msg.CMoveRQ,
			m_pSCUQueryDataset->Data(),
			MoveRetrieveCallback, this,
			m_pNetSendData->BlockMode, m_hostInfo.Timeout(),
			m_pNetSendData->Net, nullptr,
			NULL, &rsp, &statusDetail, NULL, OFTrue);

		if (ofcond.bad())
		{
			qInfo() << "DoMove failed. OFCondition : " << ofcond.text();
			return EDicomNetworkResult::FAIL_TO_SCU_SEND_DIMSE_MOVE;
		}

		if (DICOM_SUCCESS_STATUS(rsp.DimseStatus))
		{
			return EDicomNetworkResult::SUCCESS;
		}
		else
		{
			qInfo() << "DoMove fail. DIMSE status : " << DU_cmoveStatusString(rsp.DimseStatus);
			return EDicomNetworkResult::FAIL_TO_SCU_SEND_DIMSE_MOVE;
		}
	}

	void MoveSCUCommand::onUpdateStatus_StoreSCP(DicomNetworkStoreSCPStatus statusSCP)
	{
		/* StoreSCP 결과 다운로드 */
		if (m_pDownloadData)
		{
			m_pDownloadData->DicomDatasetList.push_back(statusSCP.DcmDataset);
		}

		DicomNetworkSCUStatus statusSCU(EDcmNetworkDIMSEType::MOVE_SCU);
		statusSCU.InProgressCount = ++m_progressCount;
		statusSCU.MaxProgressCount = m_maxProgressCount;

		m_pSCU->updateSCUStatus(statusSCU);
	}

	void MoveSCUCommand::SetupQueryDataset()
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
}

