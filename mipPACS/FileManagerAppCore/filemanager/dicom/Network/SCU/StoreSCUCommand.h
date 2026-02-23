#pragma once

#include "filemanager/dicom/Network/SCU/MultiSendSCUCommand.h"
#include "filemanager/dicom/Network/DicomHostInfo.h"
#include "filemanager/dicom/Network/DicomPresentationContext.h"
#include <vector>
#include <string>

class DcmFileFormat;

namespace fm
{
	class DicomDataset;
	class DicomNetworkSCU;

	class StoreSCUCommand : public MultiSendSCUCommand
	{
	public:
		StoreSCUCommand(
			DicomNetworkSCU* pSCU,
			std::wstring AETitle, 
			DicomHostInfo hostInfo,
			std::vector<DicomDataset>& sendDicomDatasetList
		);

	public:
		void Update_STORE_Progress();

	protected:
		virtual std::vector<DicomPresentationContext> GetPresentationContexts() override;
		virtual std::string GetAbstractSyntax() override;

		virtual EDicomNetworkResult SetupSendNetworkMessage(NetworkSendData * pNetSendData, DicomDataset* pDicomDataset) override;
		virtual EDicomNetworkResult SendAndReceiveMessage(DicomDataset* pDicomDataset) override;

	private:
		int m_progressCount;
	};
}
