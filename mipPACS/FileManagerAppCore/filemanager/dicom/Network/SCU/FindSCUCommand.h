#pragma once

#include "filemanager/dicom/dicom_defines.h"
#include "filemanager/dicom/Network/SCU/SingleSendSCUCommand.h"
#include "filemanager/dicom/DicomDataset.h"
#include "filemanager/dicom/Network//DicomFindOption.h"

namespace fm
{
	class DicomNetworkSCU;

	class FindSCUCommand : public SingleSendSCUCommand
	{
	public:
		FindSCUCommand(DicomNetworkSCU* pSCU, std::wstring AETitle, DicomHostInfo hostInfo,
			DicomFindOption findOption, std::vector<DicomDataset>* pDcmDatasetList);

	public:
		virtual std::vector<DicomPresentationContext> GetPresentationContexts() override;
		virtual std::string GetAbstractSyntax() override;

		virtual EDicomNetworkResult SetupSendNetworkMessage(NetworkSendData * pNetSendData, DicomDataset* pDicomDataset) override;
		virtual EDicomNetworkResult SendAndReceiveMessage(DicomDataset* pDicomDataset) override;

	public:
		void Update_FIND_InProgress(DicomDataset& dcmDataset);

	private:
		void SetupQueryDataset();

	protected:
		DicomFindOption m_findOption;

		std::vector<DicomDataset>* m_pOutDcmDatasetList;

		std::unique_ptr<fm::DicomDataset> m_pSCUQueryDataset;
		int m_progressCount = 0;
	};
}