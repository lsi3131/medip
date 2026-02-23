#include "stdafx.h"

#include "DicomNetworkManager.h"
#include "filemanager/appcore/Event/EventManager.h"
#include "filemanager/dicom/DicomDataset.h"
#include "filemanager/dicom/Network/DicomNetworkSCU.h"
#include "filemanager/dicom/Network/DicomNetworkSCP.h"
#include "FileManager/dicom/Convert/DicomConverter.h"
#include "FileManager/dicom/DicomInfomationModel.h"
#include "filemanager/config/PACSConfig.h"

#include <qfileinfo>
#include <qdir>

namespace fm
{
	DicomNetworkManager::DicomNetworkManager()
	{
		/* Initialize Network가 반드시 실행되어 있어야함*/
		OFStandard::initializeNetwork();

		m_pSCU = new DicomNetworkSCU();
		m_pSCP = new DicomNetworkSCP();

		connect(m_pSCU, &DicomNetworkSCU::updateSCUStatus, this, &DicomNetworkManager::onSCUCommunicationInProgress);
		connect(m_pSCP, &DicomNetworkSCP::updateStatus, this, &DicomNetworkManager::onSCPCommunicationInProgress);
	}

	DicomNetworkManager::~DicomNetworkManager()
	{
		delete m_pSCU;
		delete m_pSCP;
	}

	bool DicomNetworkManager::SetHostInfoQueryRetrieveByConfig(PACSConfig* pConfig)
	{
		if (pConfig == nullptr)
		{
			return false;
		}

		int selectedHostIndex = pConfig->GetSelectedQueryRetrieveHost();
		std::vector<PACSConfig::Host> hostListQueryRetrieve = pConfig->GetQueryRetrieveHosts();

		/* Host 정보 초기화 */
		if (hostListQueryRetrieve.size() > selectedHostIndex)
		{
			PACSConfig::Host hostQueryRetrieve = hostListQueryRetrieve[selectedHostIndex];
			DicomHostInfo hostInfo(
				hostQueryRetrieve.ApplicationEntity,
				hostQueryRetrieve.IP,
				hostQueryRetrieve.TransferSyntax,
				hostQueryRetrieve.Port,
				hostQueryRetrieve.MaxAssoc,
				hostQueryRetrieve.Protocol,
				pConfig->GetTimeout()
			);

			SetHostInfoQueryRetrieve(hostInfo);
		}
		else
		{
			m_hostInfoQueryRetrieve.Clear();
			SetHostInfoQueryRetrieve(m_hostInfoQueryRetrieve);
		}
		return true;
	}

	bool DicomNetworkManager::SetListenerInfoByConfig(PACSConfig* pConfig)
	{
		if (pConfig == nullptr)
		{
			return false;
		}

		/* Listener 정보 초기화 */
		PACSConfig::Listener listener = pConfig->GetListener();
		DicomListenerInfo listenerInfo(
			listener.ApplicationEntity,
			listener.Port,
			pConfig->GetTimeout()
		);

		SetListenerInfo(listenerInfo);
		return true;
	}

	bool DicomNetworkManager::SetAllByConfig(PACSConfig* pConfig)
	{
		if (SetHostInfoQueryRetrieveByConfig(pConfig) == false)
		{
			return false;
		}

		if (SetListenerInfoByConfig(pConfig) == false)
		{
			return false;
		}

		return true;
	}

	bool DicomNetworkManager::Deinitialize()
	{
		m_pSCP->Abort_Server()->wait();
		return true;
	}

	void DicomNetworkManager::SetHostInfoQueryRetrieve(DicomHostInfo& host)
	{
		m_hostInfoQueryRetrieve = host;
		m_pSCU->SetHostInfoQueryRetrieve(host);
	}

	void DicomNetworkManager::SetHostInfoStore(DicomHostInfo& host)
	{
		m_hostInfoStore = host;
		m_pSCU->SetHostInfoStore(host);
	}

	void DicomNetworkManager::SetListenerInfo(DicomListenerInfo& listener)
	{
		m_listenerInfo = listener;
		m_pSCU->SetAETitle(listener.AETitle());
		m_pSCP->SetListenerInfo(listener);
	}

	DicomHostInfo DicomNetworkManager::HostInfoQueryRetrieve()
	{
		return m_hostInfoQueryRetrieve;
	}

	DicomHostInfo DicomNetworkManager::HostInfoStore()
	{
		return m_hostInfoStore;
	}

	DicomListenerInfo DicomNetworkManager::ListenerInfo()
	{
		return m_listenerInfo;
	}

	bool DicomNetworkManager::IsHostInfoEnabled()
	{
		return !m_hostInfoQueryRetrieve.IsEmpty();
	}

	QFuture<void> DicomNetworkManager::Echo_Async()
	{
		QFuture<void> future = QtConcurrent::run([=]() {
			m_pSCU->Echo();
			});
		return future;
	}

	QFuture<void> DicomNetworkManager::Find_Async(const DicomFindOption& findOption)
	{
		QFuture<void> future = QtConcurrent::run([=]() {
			emit g_EventManager.dcmNet_Find_Started();
			std::vector<DicomDataset> resultDcmDatasetList;
			EDicomNetworkResult result = Find_Sync(findOption, resultDcmDatasetList);

			if (result == EDicomNetworkResult::SUCCESS)
			{
				DcmNetFindData findData;
				findData.DicomDatasetList = resultDcmDatasetList;
				findData.FindOption = findOption;
				emit g_EventManager.dcmNet_Find_Finished(findData);
			}
			else
			{
				emit g_EventManager.dcmNet_Error("error occured in finding");
			}
			});

		return future;
	}

	EDicomNetworkResult DicomNetworkManager::Find_Sync(const DicomFindOption& findOption, std::vector<DicomDataset>& dcmDatasetList)
	{
		EDicomNetworkResult result = m_pSCU->Find(
			findOption, &dcmDatasetList
		);
		return result;
	}

	QFuture<void> DicomNetworkManager::Upload_Async(std::vector<std::string> filepathList)
	{
		QFuture<void> future = QtConcurrent::run([=]() {
			Upload(filepathList);
			});

		return future;
	}

	QFuture<void> DicomNetworkManager::Upload_Async(std::vector<DicomDataset> dcmDatasetList)
	{
		QFuture<void> future = QtConcurrent::run([=]() {
			Upload(dcmDatasetList);
			});

		return future;
	}

	void DicomNetworkManager::Upload(std::vector<std::string> filepathList)
	{
		emit g_EventManager.dcmNet_Upload_Started();

		std::vector<DicomDataset> datasetList;

		for (std::string& filepath : filepathList)
		{
			QFileInfo info(filepath.c_str());
			QString suffix = info.suffix().toLower();

			if (suffix == "dcm")
			{
				DicomDataset dataset;
				if (dataset.LoadFromFile(filepath.c_str()))
				{
					datasetList.push_back(dataset);
				}

			}
		}

		EDicomNetworkResult result = m_pSCU->Store(datasetList);
		if (result == EDicomNetworkResult::SUCCESS)
		{
			emit g_EventManager.dcmNet_Upload_Finished();
		}
		else
		{
			emit g_EventManager.dcmNet_Error("error ocurred in upload");
		}
	}

	void DicomNetworkManager::Upload(std::vector<DicomDataset> dcmDatasetList)
	{
		emit g_EventManager.dcmNet_Upload_Started();

		EDicomNetworkResult result = m_pSCU->Store(dcmDatasetList);
		if (result == EDicomNetworkResult::SUCCESS)
		{
			emit g_EventManager.dcmNet_Upload_Finished();
		}
		else
		{
			emit g_EventManager.dcmNet_Error("error ocurred in upload");
		}
	}

	QFuture<void> DicomNetworkManager::Download_Series_Async(
		std::wstring studyInstanceUID,
		std::wstring seriesInstanceUID,
		std::wstring repositoryDirpath,
		std::wstring fileext,
		int maxImageCount)
	{
		QFuture<void> future = QtConcurrent::run([=]() {
			emit g_EventManager.dcmNet_Donwload_Started();

			DicomHostInfo hostInfo = m_pSCU->GetHostInfoQueryRetrieve();
			EDicomNetworkResult result = EDicomNetworkResult::SUCCESS;
			DcmNetDownloadData downloadData;

			if (hostInfo.Protocol() == C_GET)
			{
				result = m_pSCU->GetBySeriesUID(
					seriesInstanceUID,
					repositoryDirpath,
					fileext,
					maxImageCount,
					&downloadData
				);
			}
			else if (hostInfo.Protocol() == C_MOVE)
			{
				m_pSCP->SetDownloadInfo(
					repositoryDirpath, fileext
				);

				result = m_pSCU->MoveBySeriesUID(
					seriesInstanceUID,
					m_pSCP,
					maxImageCount,
					&downloadData
				);
			}
			else
			{
				result = EDicomNetworkResult::INVALID_DOWNLOAD_PROTOCOL;
				qWarning() << "invalid protocol type(=" << hostInfo.Protocol() << ")";
			}

			if (result == EDicomNetworkResult::SUCCESS)
			{
				emit g_EventManager.dcmNet_Donwload_Finished(downloadData);
			}
			else
			{
				emit g_EventManager.dcmNet_Error("error occured in downloading");
			}
			});

		return future;
	}

	QFuture<void> DicomNetworkManager::Download_Study_Async(
		std::wstring studyInstanceUID,
		std::wstring repositoryDirpath,
		std::wstring fileext,
		int maxImageCount)
	{
		QFuture<void> future = QtConcurrent::run([=]() {
			emit g_EventManager.dcmNet_Donwload_Started();

			DicomHostInfo hostInfo = m_pSCU->GetHostInfoQueryRetrieve();
			EDicomNetworkResult result = EDicomNetworkResult::SUCCESS;
			DcmNetDownloadData downloadData;

			if (hostInfo.Protocol() == C_GET)
			{
				result = m_pSCU->GetByStudyUID(
					studyInstanceUID,
					repositoryDirpath,
					fileext,
					maxImageCount,
					&downloadData
				);
			}
			else if (hostInfo.Protocol() == C_MOVE)
			{
				m_pSCP->SetDownloadInfo(
					repositoryDirpath, fileext
				);

				result = m_pSCU->MoveByStudyUID(
					studyInstanceUID,
					m_pSCP,
					maxImageCount,
					&downloadData
				);

			}
			else
			{
				result = EDicomNetworkResult::INVALID_DOWNLOAD_PROTOCOL;
				qWarning() << "invalid protocol type(=" << hostInfo.Protocol() << ")";
			}

			if (result == EDicomNetworkResult::SUCCESS)
			{
				emit g_EventManager.dcmNet_Donwload_Finished(downloadData);
			}
			else
			{
				emit g_EventManager.dcmNet_Error("error occured in downloading");
			}
			});

		return future;
	}

	EDicomNetworkResult DicomNetworkManager::InitializeStoreSCP()
	{
		return m_pSCP->InitializeNetwork();
	}

	void DicomNetworkManager::StartStoreSCP_Async()
	{
		m_pSCP->StartServer_Async();
	}

	void DicomNetworkManager::StopStoreSCP(QThread* thread)
	{
		m_pSCP->Abort_Server();
	}

	EDicomNetworkResult DicomNetworkManager::ReleaseStoreSCP()
	{
		return m_pSCP->AbortWaitAndDropNetwork();
	}

	bool DicomNetworkManager::IsStoreSCPInitialized()
	{
		return m_pSCP->IsNetworkInitialized();
	}

	bool DicomNetworkManager::IsStoreSCPRunning()
	{
		return m_pSCP->IsStoreSCPRunning();
	}

	void DicomNetworkManager::onSCUCommunicationInProgress(DicomNetworkSCUStatus status)
	{
		if (status.DIMSE_Type == EDcmNetworkDIMSEType::FIND_SCU)
		{
			emit g_EventManager.dcmNet_Find_InProgress(status);
		}
		else if (
			status.DIMSE_Type == EDcmNetworkDIMSEType::GET_SCU ||
			status.DIMSE_Type == EDcmNetworkDIMSEType::MOVE_SCU
			)
		{
			emit g_EventManager.dcmNet_Download_InProgress(status);
		}
		else if (status.DIMSE_Type == EDcmNetworkDIMSEType::STORE_SCU)
		{
			emit g_EventManager.dcmNet_Upload_InProgress(status);
		}
	}

	void DicomNetworkManager::onSCPCommunicationInProgress(DicomNetworkSCPStatus status)
	{
		emit g_EventManager.dcmNet_SCPServerStatusChanged(status);
	}
}

