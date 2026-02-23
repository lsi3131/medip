#pragma once

#include "filemanager/dicom/dicom_defines.h"
#include "filemanager/dicom/Network/SCU/SingleSendSCUCommand.h"
#include "filemanager/dicom/Network/DicomNetworkSCP.h"
#include "filemanager/dicom/DicomDataset.h"

namespace fm
{
	class DicomNetworkSCU;
	class DicomNetworkSCP;
	class DcmNetDownloadData;

	class MoveSCUCommand : public SingleSendSCUCommand, public IDicomNetworkSCPObserver
	{
	public:
		MoveSCUCommand(
			DicomNetworkSCU* pSCU,
			std::wstring AETitle, DicomHostInfo hostInfo,
			std::wstring UID,
			DicomNetworkSCP* pSCP,
			EQueryRetrieveLevel queryRetrieveLevel,
			int maxImageCount,
			DcmNetDownloadData* pDownloadDataset);

		virtual ~MoveSCUCommand();

	public:
		virtual std::vector<DicomPresentationContext> GetPresentationContexts() override;
		virtual std::string GetAbstractSyntax() override;

		virtual EDicomNetworkResult SetupSendNetworkMessage(NetworkSendData * pNetSendData, DicomDataset* pDicomDataset) override;
		virtual EDicomNetworkResult SendAndReceiveMessage(DicomDataset* pDicomDataset) override;

	public:
		virtual void onUpdateStatus_StoreSCP(DicomNetworkStoreSCPStatus status) override;

	private:
		void SetupQueryDataset();

	protected:
		std::wstring m_UID;
		std::unique_ptr<fm::DicomDataset> m_pSCUQueryDataset;
		DicomNetworkSCP* m_pSCP;
		DcmNetDownloadData* m_pDownloadData;
		EQueryRetrieveLevel m_queryRetrieveLevel;
		int m_progressCount;
		int m_maxProgressCount;
	};
}