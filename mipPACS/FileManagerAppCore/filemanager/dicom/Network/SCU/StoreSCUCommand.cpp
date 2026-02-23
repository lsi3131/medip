#include "stdafx.h"
#include "StoreSCUCommand.h"
#include "filemanager/dicom/Network/DicomNetworkSCU.h"
#include "filemanager/dicom/Network/DIMSE_Data.h"
#include "filemanager/dicom/DicomDataset.h"

namespace fm
{
	static void StoreRetrieveCallbak(void* pCallbackData, T_DIMSE_StoreProgress* progress, T_DIMSE_C_StoreRQ* req)
	{
		StoreSCUCommand* p = (StoreSCUCommand*)pCallbackData;
		if (progress->state == DIMSE_StoreEnd)
		{
			p->Update_STORE_Progress();
		}
	}

	StoreSCUCommand::StoreSCUCommand(
		DicomNetworkSCU* pSCU,
		std::wstring AETitle, DicomHostInfo hostInfo,
		std::vector<DicomDataset>& sendDicomDatasetList) :
		MultiSendSCUCommand(
			pSCU,
			AETitle,
			hostInfo,
			sendDicomDatasetList),
		m_progressCount(0)
	{
	}

	void fm::StoreSCUCommand::Update_STORE_Progress()
	{
		DicomNetworkSCUStatus status(fm::EDcmNetworkDIMSEType::STORE_SCU);
		status.MaxProgressCount = m_sendDicomDatasetList.size();
		status.InProgressCount = ++m_progressCount;
		emit m_pSCU->updateSCUStatus(status);
	}

	std::vector<DicomPresentationContext> StoreSCUCommand::GetPresentationContexts()
	{
		std::vector<DicomPresentationContext> ctx;

		for (int i = 0; i < numberOfDcmShortSCUStorageSOPClassUIDs; ++i)
		{
			ctx.push_back(DicomPresentationContext(dcmShortSCUStorageSOPClassUIDs[i], ASC_SC_ROLE_DEFAULT));
		}

		return ctx;
	}

	std::string StoreSCUCommand::GetAbstractSyntax()
	{
		const char* abstractSyntax = UID_CTImageStorage;

		return abstractSyntax;
	}

	EDicomNetworkResult StoreSCUCommand::SetupSendNetworkMessage(NetworkSendData* pNetSendData, DicomDataset* pDicomDataset)
	{
		SCUCommand::SetupSendNetworkMessage(pNetSendData, pDicomDataset);

		OFBool correctUIDPadding = OFFalse;
		DIC_UI sopClass;
		DIC_UI sopInstance;

		/* figure out which SOP class and SOP instance is encapsulated in the file */
		if (!DU_findSOPClassAndInstanceInDataSet(pDicomDataset->Data(),
			sopClass, sizeof(sopClass), sopInstance, sizeof(sopInstance), correctUIDPadding))
		{
			qInfo() << "fail to get SOP class, instance";
			return EDicomNetworkResult::FAIL_TO_SCU_SEND_DIMSE_STORE;
		}

		T_DIMSE_C_StoreRQ* req = &(pNetSendData->Message.msg.CStoreRQ);
		pNetSendData->Message.CommandField = DIMSE_C_STORE_RQ;
		req->MessageID = pNetSendData->Assoc->nextMsgID++;
		req->DataSetType = DIMSE_DATASET_PRESENT;
		req->Priority = DIMSE_PRIORITY_MEDIUM;
		OFStandard::strlcpy(req->AffectedSOPClassUID, sopClass, sizeof(req->AffectedSOPClassUID));
		OFStandard::strlcpy(req->AffectedSOPInstanceUID, sopInstance, sizeof(req->AffectedSOPInstanceUID));

		return EDicomNetworkResult::SUCCESS;
	}

	EDicomNetworkResult StoreSCUCommand::SendAndReceiveMessage(DicomDataset* pDicomDataset)
	{
		T_DIMSE_C_StoreRSP rsp;
		DcmDataset* statusDetail = NULL;
		OFCondition ofcond;
		int filesize = 0;

		ofcond = DIMSE_storeUser(
			m_pNetSendData->Assoc,
			m_pNetSendData->PresentationContextID,
			&m_pNetSendData->Message.msg.CStoreRQ,
			NULL, pDicomDataset->Data(),
			StoreRetrieveCallbak, this,
			m_pNetSendData->BlockMode, m_hostInfo.Timeout(),
			&rsp, &statusDetail, NULL, filesize);

		if (ofcond.bad())
		{
			qInfo() << "DoStore failed. OFCondition : " << ofcond.text();
			return EDicomNetworkResult::FAIL_TO_SCU_SEND_DIMSE_STORE;
		}

		if (DICOM_SUCCESS_STATUS(rsp.DimseStatus))
		{
			return EDicomNetworkResult::SUCCESS;
		}
		else
		{
			qInfo() << "DoStore fail. DIMSE status : " << DU_cfindStatusString(rsp.DimseStatus);
			return EDicomNetworkResult::FAIL_TO_SCU_SEND_DIMSE_STORE;
		}
	}

}
