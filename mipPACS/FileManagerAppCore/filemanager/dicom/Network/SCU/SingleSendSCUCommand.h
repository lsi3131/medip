#pragma once

#include "filemanager/dicom/Network/SCU/SCUCommand.h"

namespace fm
{
	class DicomDataset;

	class SingleSendSCUCommand : public SCUCommand
	{
	public:
		SingleSendSCUCommand(DicomNetworkSCU* pSCU, std::wstring AETitle, DicomHostInfo hostInfo, DicomDataset* pSendDataset = nullptr);
	public:
		virtual EDicomNetworkResult SetupAndSendAndReceiveMessageImp() override;

	protected:
		DicomDataset* m_pSendDataset;
	};
}
