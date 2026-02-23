#pragma once

#include "filemanager/dicom/Network/SCU/SingleSendSCUCommand.h"

namespace fm
{
	class EmptySCUCommand : public SingleSendSCUCommand
	{
	public:
		EmptySCUCommand(DicomNetworkSCU* pSCU, std::wstring AETitle, DicomHostInfo hostInfo);

	protected:
		virtual std::vector<DicomPresentationContext> GetPresentationContexts() override;
		virtual std::string GetAbstractSyntax() override;

		virtual EDicomNetworkResult SetupSendNetworkMessage(NetworkSendData * pNetSendData, DicomDataset* pDicomDataset) override;
		virtual EDicomNetworkResult SendAndReceiveMessage(DicomDataset* pDicomDataset) override;

	protected:
	};
}
