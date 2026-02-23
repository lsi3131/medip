#include "stdafx.h"
#include "MultiSendSCUCommand.h"

namespace fm
{
	fm::MultiSendSCUCommand::MultiSendSCUCommand(DicomNetworkSCU * pSCU, std::wstring AETitle, DicomHostInfo hostInfo,
		std::vector<DicomDataset>& sendDicomDatasetList) :
		SCUCommand(pSCU, AETitle, hostInfo),
		m_sendDicomDatasetList(sendDicomDatasetList)
	{
	}

	EDicomNetworkResult fm::MultiSendSCUCommand::SetupAndSendAndReceiveMessageImp()
	{
		for (DicomDataset& dicomDataset : m_sendDicomDatasetList)
		{
			EDicomNetworkResult result = SetupSendNetworkMessage(m_pNetSendData, &dicomDataset);
			if (result != EDicomNetworkResult::SUCCESS)
			{
				return result;
			}

			result = SetupPresentationContextID();
			if (result != EDicomNetworkResult::SUCCESS)
			{
				return result;
			}

			result = SendAndReceiveMessage(&dicomDataset);
			if (result != EDicomNetworkResult::SUCCESS)
			{
				return result;
			}
		}
		return EDicomNetworkResult::SUCCESS;
	}
}

