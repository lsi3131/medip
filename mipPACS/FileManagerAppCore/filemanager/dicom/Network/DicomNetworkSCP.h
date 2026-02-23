#pragma once

#include <qthread>
#include <vector>
#include "filemanager/dicom/dicom_defines.h"
#include "filemanager/dicom/Network/DicomPresentationContext.h"
#include "filemanager/dicom/Network/DicomListenerInfo.h"
#include "filemanager/dicom/Network/DicomNetworkSCUStatus.h"
#include "filemanager/dicom/Network/DicomNetworkSCPStatus.h"
#include "filemanager/dicom/Network/DicomNetworkStoreSCPStatus.h"
#include "filemanager/export.h"

class DcmFileFormat;
class DcmDataset;
class T_ASC_Network;
class T_ASC_Association;
class T_DIMSE_StoreProgress;
class T_DIMSE_C_StoreRQ;
class T_DIMSE_C_StoreRSP;

namespace fm
{
	class IDicomNetworkSCPObserver
	{
	public:
		virtual void onUpdateStatus_StoreSCP(DicomNetworkStoreSCPStatus status) = 0;
	};

	class DicomNetworkSCP;
	class DicomNetworkSCU;

	class ThreadDicomNetworkSCP : public QThread
	{
		Q_OBJECT

	public:
		ThreadDicomNetworkSCP(DicomNetworkSCP* p);

	protected:
		virtual void run() override;

	private:
		DicomNetworkSCP* m_p;
	};

	class FM_CORE_EXPORT DicomNetworkSCP : public QObject
	{
		Q_OBJECT

	public:
		friend ThreadDicomNetworkSCP;
	public:
		static void StoreSCPCallback(
			void *callbackData,
			T_DIMSE_StoreProgress *progress,
			T_DIMSE_C_StoreRQ *req,
			char * imageFileName, DcmDataset **imageDataSet,
			T_DIMSE_C_StoreRSP *rsp,
			DcmDataset **statusDetail);

	public:
		DicomNetworkSCP();
		~DicomNetworkSCP();

	public:
		EDicomNetworkResult InitializeNetwork();
		EDicomNetworkResult AbortWaitAndDropNetwork();

		void SetListenerInfo(DicomListenerInfo& listener);
		void SetDownloadInfo(std::wstring outputDirpath, std::wstring filenameExtenstion);

		bool IsNetworkInitialized();
		bool IsStoreSCPRunning();

		bool StartServer();
		QThread* StartServer_Async();
		bool InitializeNetworkAndStartServer_Async(QThread** ppWorkingThread = nullptr);
		QThread* Abort_Server();

		std::wstring AETitle() { return m_ListenerInfo.AETitle(); }
	public:
		void RegisterObserver(IDicomNetworkSCPObserver* observer);
		void UnregisterObserver(IDicomNetworkSCPObserver* observer);

		void UpdateStatusToObserver(DicomNetworkStoreSCPStatus& status);

	private:
		EDicomNetworkResult Wait_And_SetupAssoc();
		EDicomNetworkResult Downloading_Loop();
		EDicomNetworkResult ReleaseAssoc();
		EDicomNetworkResult DropNetwork();

		bool SaveFileToDisk(DcmFileFormat* dcmff, std::wstring filepath);

	Q_SIGNALS:
		void updateStatus(DicomNetworkSCPStatus status);

	private:
		ThreadDicomNetworkSCP m_thread;
		DicomNetworkSCU* m_pSCUAbort;
		bool m_isAbortServer;
		DicomListenerInfo m_ListenerInfo;

		T_ASC_Network* m_net;
		T_ASC_Association *m_assoc;

		std::wstring m_repositoryDirPath;
		std::wstring m_fileExtenstion;

		DicomNetworkSCPStatus m_SCPRunningStatus;

		std::vector<IDicomNetworkSCPObserver*> m_observerList;
	};
}
