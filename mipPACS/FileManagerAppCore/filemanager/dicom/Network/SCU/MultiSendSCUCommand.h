#pragma once

#include "filemanager/dicom/Network/SCU/SCUCommand.h"
#include "filemanager/dicom/DicomDataset.h"
#include <vector>

namespace fm
{
	class DicomDataset;

	class MultiSendSCUCommand : public SCUCommand
	{
	public:
		MultiSendSCUCommand(DicomNetworkSCU* pSCU, std::wstring AETitle, DicomHostInfo hostInfo,
			std::vector<DicomDataset>& sendDicomDatasetList);
	public:
		virtual EDicomNetworkResult SetupAndSendAndReceiveMessageImp() override;

	protected:
		std::vector<DicomDataset> m_sendDicomDatasetList;
	};
}
