#include "stdafx.h"
#include "SCUCommand.h"
#include "filemanager/dicom/Network/DIMSE_Data.h"
#include "filemanager/dicom/Network/DicomNetworkSCU.h"

namespace fm
{
	const QUERY_ABSTRACT_SYNTAX QuerySyntax[3] =
	{
		{
			UID_FINDPatientRootQueryRetrieveInformationModel,
			UID_MOVEPatientRootQueryRetrieveInformationModel,
			UID_GETPatientRootQueryRetrieveInformationModel
		},
		{
			UID_FINDStudyRootQueryRetrieveInformationModel,
			UID_MOVEStudyRootQueryRetrieveInformationModel,
			UID_GETStudyRootQueryRetrieveInformationModel
		},
		{
			UID_RETIRED_FINDPatientStudyOnlyQueryRetrieveInformationModel,
			UID_RETIRED_MOVEPatientStudyOnlyQueryRetrieveInformationModel,
			UID_RETIRED_GETPatientStudyOnlyQueryRetrieveInformationModel
		}
	};


	SCUCommand::SCUCommand(DicomNetworkSCU * pSCU, std::wstring AETitle, DicomHostInfo hostInfo) :
		m_pSCU(pSCU),
		m_AETitle(AETitle),
		m_hostInfo(hostInfo)
	{
		m_pNetSendData = new NetworkSendData();
		m_pNetRecvData = new NetworkRecvData();
	}

	SCUCommand::~SCUCommand()
	{
		delete m_pNetSendData;
		delete m_pNetRecvData;
	}

	EDicomNetworkResult SCUCommand::Do()
	{
		EDicomNetworkResult result = AssociationNegociation(GetPresentationContexts());

		if (result != EDicomNetworkResult::SUCCESS)
		{
			return result;
		}

		result = SetupAndSendAndReceiveMessageImp();

		ReleaseAssociationNegociation();

		return result;
	}

	EDicomNetworkResult SCUCommand::AssociationNegociation(std::vector<DicomPresentationContext>& contexts)
	{
		OFString temp_str;
		OFCondition ofcond;
		OFCmdUnsignedInt maxReceivePDULength = ASC_DEFAULTMAXPDU;
		OFBool secureConnection = false;
		DIC_NODENAME peerHost;

		ofcond = ASC_initializeNetwork(NET_REQUESTOR, 0, m_hostInfo.Timeout(), &m_pNetSendData->Net);
		if (ofcond.bad())
		{
			qWarning() << "Fail to Initialize Network Failed: " << DimseCondition::dump(temp_str, ofcond).c_str();
			return EDicomNetworkResult::FAIL_TO_SCU_ASSOCIATION_NETWORK;
		}

		ofcond = ASC_createAssociationParameters(&m_pNetSendData->Params, maxReceivePDULength);
		if (ofcond.bad())
		{
			qWarning() << "Creating Association Parameters Failed: " << DimseCondition::dump(temp_str, ofcond).c_str();
			return EDicomNetworkResult::FAIL_TO_SCU_ASSOCIATION_NETWORK;
		}

		ASC_setAPTitles(m_pNetSendData->Params,
			StringUtil::WideToMulitiByte(m_AETitle).c_str(),
			StringUtil::WideToMulitiByte(m_hostInfo.AETitle()).c_str(),
			NULL);

		ofcond = ASC_setTransportLayerType(m_pNetSendData->Params, secureConnection);
		if (ofcond.bad())
		{
			qWarning() << "Setting Transport Layer Type Failed: " << DimseCondition::dump(temp_str, ofcond).c_str();
			(void)ASC_destroyAssociationParameters(&m_pNetSendData->Params);
			return EDicomNetworkResult::FAIL_TO_SCU_ASSOCIATION_NETWORK;
		}

		sprintf(peerHost, "%s:%d", m_hostInfo.IP().c_str(), m_hostInfo.Port());
		ASC_setPresentationAddresses(m_pNetSendData->Params, OFStandard::getHostName().c_str(), peerHost);

		T_ASC_PresentationContextID presID = 1;
		for (int i = 0; i < contexts.size(); ++i)
		{
			ofcond = ASC_addPresentationContext(
				m_pNetSendData->Params, presID, contexts[i].AbstractSyntaxName.c_str(),
				contexts[i].TransferSyntaxes, contexts[i].NumTransferSyntaxes,
				contexts[i].RoleSelect);

			if (ofcond.bad())
			{
				qWarning() << "Adding Presentation Contexts Failed: " << DimseCondition::dump(temp_str, ofcond).c_str();
				(void)ASC_destroyAssociationParameters(&m_pNetSendData->Params);
				return EDicomNetworkResult::FAIL_TO_SCU_ASSOCIATION_NETWORK;
			}
			/* 1,3,5... 홀수 단위로 증가해야함 */
			presID += 2;
		}

		ofcond = ASC_requestAssociation(m_pNetSendData->Net, m_pNetSendData->Params, &m_pNetSendData->Assoc);

		if (ofcond.bad())
		{
			if (ofcond == DUL_PEERABORTEDASSOCIATION)
			{
				/* Abort 호출 시 진행됨.*/
				qInfo() << "Association Aborted";
				return EDicomNetworkResult::FAIL_TO_SCU_ASSOCIATION_NETWORK;
			}
			else if (ofcond == DUL_ASSOCIATIONREJECTED)
			{
				T_ASC_RejectParameters rej;
				ASC_getRejectParameters(m_pNetSendData->Params, &rej);

				qWarning() << "Association Rejected:" << endl << ASC_printRejectParameters(temp_str, &rej).c_str();
				(void)ASC_destroyAssociation(&m_pNetSendData->Assoc); // this also destroys the T_ASC_Parameters structure
				return EDicomNetworkResult::FAIL_TO_SCU_ASSOCIATION_NETWORK;
			}
			else
			{
				qWarning() << "Association Request Failed: " << DimseCondition::dump(temp_str, ofcond).c_str();
				(void)ASC_destroyAssociation(&m_pNetSendData->Assoc); // this also destroys the T_ASC_Parameters structure
				return EDicomNetworkResult::FAIL_TO_SCU_ASSOCIATION_NETWORK;
			}
		}

		int countAcceptedPresentationContexts = ASC_countAcceptedPresentationContexts(m_pNetSendData->Params);
		if (countAcceptedPresentationContexts == 0)
		{
			qWarning() << "No Acceptable Presentation Contexts";
			(void)ASC_destroyAssociation(&m_pNetSendData->Assoc); // this also destroys the T_ASC_Parameters structure
			return EDicomNetworkResult::FAIL_TO_SCU_ASSOCIATION_NETWORK;
		}

		return EDicomNetworkResult::SUCCESS;
	}

	EDicomNetworkResult SCUCommand::SetupPresentationContextID()
	{
		/* Presentation Context ID 초기화 */
		std::string abstractSyntax = GetAbstractSyntax();
		m_pNetSendData->PresentationContextID = ASC_findAcceptedPresentationContextID(m_pNetSendData->Assoc, abstractSyntax.c_str());
		if (m_pNetSendData->PresentationContextID == 0)
		{
			qWarning() << "no valid presentation context ID";
			return EDicomNetworkResult::FAIL_TO_SCU_SETUP_PRESENTATION_CONTEXT_ID;
		}

		return EDicomNetworkResult::SUCCESS;
	}

	EDicomNetworkResult SCUCommand::SetupSendNetworkMessage(NetworkSendData* pNetSendData, DicomDataset* pDicomDataset)
	{
		/* Echo를 기본으로 적용 */
		T_DIMSE_C_EchoRQ* req = &(m_pNetSendData->Message.msg.CEchoRQ);
		m_pNetSendData->Message.CommandField = DIMSE_C_ECHO_RQ;
		req->MessageID = m_pNetSendData->Assoc->nextMsgID++;
		req->DataSetType = DIMSE_DATASET_PRESENT;

		std::string abstractSyntax = GetAbstractSyntax();
		OFStandard::strlcpy(req->AffectedSOPClassUID, abstractSyntax.c_str(), sizeof(req->AffectedSOPClassUID));

		return EDicomNetworkResult::SUCCESS;
	}

	EDicomNetworkResult SCUCommand::ReleaseAssociationNegociation()
	{
		OFCondition ofcond;
		if (m_pNetSendData->Assoc != nullptr)
		{
			ofcond = ASC_releaseAssociation(m_pNetSendData->Assoc);
			if (ofcond.bad())
			{
				qWarning() << "fail to release association : " << ofcond.text();
				return EDicomNetworkResult::FAIL_TO_SCU_RELEASE_ASSOCIATION_NETWORK;
			}
		}

		ofcond = ASC_destroyAssociation(&m_pNetSendData->Assoc);
		if (ofcond.bad())
		{
			qWarning() << "fail to destroy association : " << ofcond.text();
			return EDicomNetworkResult::FAIL_TO_SCU_RELEASE_ASSOCIATION_NETWORK;
		}

		ofcond = ASC_dropNetwork(&m_pNetSendData->Net);
		if (ofcond.bad())
		{
			qWarning() << "fail to drop network : " << ofcond.text();
			return EDicomNetworkResult::FAIL_TO_SCU_RELEASE_ASSOCIATION_NETWORK;
		}

		return EDicomNetworkResult::SUCCESS;
	}
}

