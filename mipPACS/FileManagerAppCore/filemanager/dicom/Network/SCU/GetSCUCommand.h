#pragma once

#include "filemanager/dicom/dicom_defines.h"
#include "filemanager/dicom/Network/SCU/SingleSendSCUCommand.h"
#include "filemanager/dicom/DicomDataset.h"

class DcmDataset;
class OFString;

namespace fm
{
	class DicomNetworkSCU;
	class DcmNetDownloadData;

	class GetSCUCommand : public SingleSendSCUCommand
	{
	public:
		GetSCUCommand(DicomNetworkSCU* pSCU,
			std::wstring AETitle, DicomHostInfo hostInfo,
			std::wstring UID,
			std::wstring downloadDirectoryPath,
			std::wstring fileExtension,
			EQueryRetrieveLevel queryRetrieveLevel,
			int maxProgressCount,
			DcmNetDownloadData* pDownloadDataset);

	public:
		virtual std::vector<DicomPresentationContext> GetPresentationContexts() override;
		virtual std::string GetAbstractSyntax() override;

		virtual EDicomNetworkResult SetupSendNetworkMessage(NetworkSendData * pNetSendData, DicomDataset* pDicomDataset) override;
		virtual EDicomNetworkResult SendAndReceiveMessage(DicomDataset* pDicomDataset) override;

	private:
		void SetupQueryDataset();

		bool ReceiveAndHandleMessageFromSCP();

		bool WaitToReceiveCommandFromSCP();

		bool IsGETResponseReceived();
		bool IsStoreRequestReceived();

		void HandleGETResponse(bool* pContinueSession);
		void HandleStoreRequest(bool* pContinueSession);

		bool WaitToReceive_STORE_Request();

		bool SaveDicomFileToDisk(std::wstring dcmSaveFilePath);
		bool IsExist_SOPClassUID_Or_SOPInstanceUID();

		bool UpdateDataFromReceivedDataset(std::wstring* pDCMSaveFilePath, DicomDataset* dcmDataset);

		bool Send_STORE_Response();

		OFCondition GetDatasetInfo(DcmDataset* dataset,
			OFString& sopClassUID,
			OFString& sopInstanceUID
			);

		void Update_Download_Inprogress(std::wstring dcmStoreFilePath);

	protected:
		std::wstring m_UID;
		std::wstring m_repositoryDirectoryPath;
		std::wstring m_fileExtension;
		DcmNetDownloadData* m_pDownloadData;

		std::unique_ptr<fm::DicomDataset> m_pSCUQueryDataset;

		EQueryRetrieveLevel m_queryRetrieveLevel;
		int m_progressCount;
		int m_maxProgressCount;
	};
}