#include "stdafx.h"
#include "EmptySCUCommand.h"
#include "filemanager/dicom/Network/DicomNetworkSCU.h"
#include "filemanager/dicom/Network/DIMSE_Data.h"

namespace fm
{
	EmptySCUCommand::EmptySCUCommand(DicomNetworkSCU * pSCU, std::wstring AETitle, DicomHostInfo hostInfo) :
		SingleSendSCUCommand(pSCU, AETitle, hostInfo)
	{
	}

	std::vector<DicomPresentationContext> EmptySCUCommand::GetPresentationContexts()
	{
		std::vector<DicomPresentationContext> ctx;
		ctx.push_back(DicomPresentationContext(UID_VerificationSOPClass, ASC_SC_ROLE_DEFAULT));
		return ctx;
	}

	std::string EmptySCUCommand::GetAbstractSyntax()
	{
		return "";
	}

	EDicomNetworkResult EmptySCUCommand::SetupSendNetworkMessage(NetworkSendData * pNetSendData, DicomDataset* pDicomDataset)
	{
		return EDicomNetworkResult::SUCCESS;
	}

	EDicomNetworkResult EmptySCUCommand::SendAndReceiveMessage(DicomDataset* pDicomDataset)
	{
		return EDicomNetworkResult::SUCCESS;
	}

}
