#include "stdafx.h"
#include "SingleSendSCUCommand.h"

fm::SingleSendSCUCommand::SingleSendSCUCommand(DicomNetworkSCU * pSCU, std::wstring AETitle, DicomHostInfo hostInfo, DicomDataset* pSendDataset) :
	SCUCommand(pSCU, AETitle, hostInfo),
	m_pSendDataset(pSendDataset)
{
}

fm::EDicomNetworkResult fm::SingleSendSCUCommand::SetupAndSendAndReceiveMessageImp()
{
	EDicomNetworkResult result = SetupSendNetworkMessage(m_pNetSendData, m_pSendDataset);
	if (result != EDicomNetworkResult::SUCCESS)
	{
		return result;
	}

	result = SetupPresentationContextID();
	if (result != EDicomNetworkResult::SUCCESS)
	{
		return result;
	}

	result = SendAndReceiveMessage(m_pSendDataset);
	if (result != EDicomNetworkResult::SUCCESS)
	{
		return result;
	}

	return EDicomNetworkResult::SUCCESS;
}
