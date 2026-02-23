#pragma once

#include <vector>
#include <string>
#include <qobject>
#include <qthread>
#include "filemanager/export.h"
#include "filemanager/dicom/dicom_defines.h"
#include "filemanager/dicom/Network/DicomListenerInfo.h"
#include "filemanager/dicom/Network/DicomHostInfo.h"
#include "filemanager/dicom/Network/DicomNetworkSCUStatus.h"
#include "filemanager/dicom/Network/DicomNetworkSCPStatus.h"
#include "filemanager/dicom/DicomDataset.h"
#include "filemanager/appcore/appcore_defines.h"
#include <QtConcurrent\qtconcurrentrun.h>

namespace fm
{
	class DicomNetworkSCU;
	class DicomNetworkSCP;
	class PACSConfig;
	class DicomFindOption;

	class FM_CORE_EXPORT DicomNetworkManager : public QObject
	{
		Q_OBJECT
	public:
		DicomNetworkManager();
		~DicomNetworkManager();

	public:
		bool SetHostInfoQueryRetrieveByConfig(PACSConfig* pConfig);
		bool SetListenerInfoByConfig(PACSConfig* pConfig);
		bool SetAllByConfig(PACSConfig* pConfig);
		bool Deinitialize();

		void SetHostInfoQueryRetrieve(DicomHostInfo& host);
		void SetHostInfoStore(DicomHostInfo& host);
		void SetListenerInfo(DicomListenerInfo& listener);

		DicomHostInfo HostInfoQueryRetrieve();
		DicomHostInfo HostInfoStore();
		DicomListenerInfo ListenerInfo();

		bool IsHostInfoEnabled();

		/* SCU */
		QFuture<void> Echo_Async();
		QFuture<void> Find_Async(const DicomFindOption& findOption);
		EDicomNetworkResult Find_Sync(const DicomFindOption& findOption, std::vector<DicomDataset>& dcmHeaderInfos);

		QFuture<void> Upload_Async(std::vector<std::string> filepathList);
		QFuture<void> Upload_Async(std::vector<DicomDataset> dcmDatasetList);

		void Upload(std::vector<std::string> filepathList);
		void Upload(std::vector<DicomDataset> dcmDatasetList);

		QFuture<void> Download_Series_Async(
			std::wstring studyInstanceUID,
			std::wstring seriesInstanceUID, 
			std::wstring repositoryDirpath,
			std::wstring fileext, 
			int maxImageCount);

		QFuture<void> Download_Study_Async(
			std::wstring studyInstanceUID,
			std::wstring repositoryDirpath,
			std::wstring fileext,
			int maxImageCount);

		/* SCP */
		EDicomNetworkResult InitializeStoreSCP();
		void StartStoreSCP_Async();
		void StopStoreSCP(QThread* thread = nullptr);
		EDicomNetworkResult ReleaseStoreSCP();
		bool IsStoreSCPInitialized();
		bool IsStoreSCPRunning();

	private slots:
		void onSCUCommunicationInProgress(DicomNetworkSCUStatus status);
		void onSCPCommunicationInProgress(DicomNetworkSCPStatus status);

	private:
		DicomNetworkSCU* m_pSCU;
		DicomNetworkSCP* m_pSCP;

		DicomHostInfo m_hostInfoQueryRetrieve;
		DicomHostInfo m_hostInfoStore;
		DicomListenerInfo m_listenerInfo;
	};

};
