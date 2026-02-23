#include "stdafx.h"
#include "EchoSCUCommand.h"
#include "filemanager/dicom/Network/DicomNetworkSCU.h"
#include "../DIMSE_Data.h"

namespace fm
{
	EchoSCUCommand::EchoSCUCommand(DicomNetworkSCU * pSCU, std::wstring AETitle, DicomHostInfo hostInfo) :
		SingleSendSCUCommand(pSCU, AETitle, hostInfo)
	{
	}

	std::vector<DicomPresentationContext> EchoSCUCommand::GetPresentationContexts()
	{
		std::vector<DicomPresentationContext> ctx;
		ctx.push_back(DicomPresentationContext(UID_VerificationSOPClass, ASC_SC_ROLE_DEFAULT));
		return ctx;
	}

	std::string EchoSCUCommand::GetAbstractSyntax()
	{
		return GetPresentationContexts()[0].AbstractSyntaxName.data();
	}

	EDicomNetworkResult EchoSCUCommand::SetupSendNetworkMessage(NetworkSendData * pNetSendData, DicomDataset* pDicomDataset)
	{
		SCUCommand::SetupSendNetworkMessage(pNetSendData, pDicomDataset);
		pNetSendData->Message.CommandField = DIMSE_C_ECHO_RQ;
		return EDicomNetworkResult::SUCCESS;
	}

	EDicomNetworkResult EchoSCUCommand::SendAndReceiveMessage(DicomDataset* pDicomDataset)
	{
		OFCondition ofcond;
		DIC_US status = NULL;
		DcmDataset* statusDetail = NULL;
		T_DIMSE_BlockingMode blockMode = DIMSE_BLOCKING;

		ofcond = DIMSE_echoUser(
			m_pNetSendData->Assoc,
			m_pNetSendData->Message.msg.CEchoRQ.MessageID,
			blockMode, m_hostInfo.Timeout(),
			&status, &statusDetail
		);

		if (ofcond.bad())
		{
			qInfo() << "DoEcho failed. OFCondition : " << ofcond.text();
			return EDicomNetworkResult::FAIL_TO_SCU_SEND_DIMSE_ECHO;
		}

		return EDicomNetworkResult::SUCCESS;
	}
}
