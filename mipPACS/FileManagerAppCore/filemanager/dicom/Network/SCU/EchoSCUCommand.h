#pragma once

#include "filemanager/dicom/Network/SCU/SingleSendSCUCommand.h"
#include "filemanager/dicom/Network/DicomPresentationContext.h"
#include <vector>
#include <string>

namespace fm
{
	class EchoSCUCommand : public SingleSendSCUCommand
	{
	public:
		EchoSCUCommand(DicomNetworkSCU* pSCU, std::wstring AETitle, DicomHostInfo hostInfo);

	protected:
		virtual std::vector<DicomPresentationContext> GetPresentationContexts() override;
		virtual std::string GetAbstractSyntax() override;

		virtual EDicomNetworkResult SetupSendNetworkMessage(NetworkSendData * pNetSendData, DicomDataset* pDicomDataset) override;
		virtual EDicomNetworkResult SendAndReceiveMessage(DicomDataset* pDicomDataset) override;

	protected:
	};
}
