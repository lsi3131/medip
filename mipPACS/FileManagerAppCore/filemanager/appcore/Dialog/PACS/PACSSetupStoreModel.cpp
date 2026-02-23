#include "stdafx.h"
#include "PACSSetupStoreModel.h"

namespace fm
{
	PACSSetupStoreModel::PACSSetupStoreModel()
	{
	}

	void PACSSetupStoreModel::Clear()
	{
		m_visibleRecords.clear();
	}

	void PACSSetupStoreModel::Init(const std::vector<PACSConfig::ExportHost>& host)
	{
		m_visibleRecords.clear();
		for (auto& host : host)
		{
			Record record;
			record.Data = host;
			record.Status = EStatus::Normal;

			m_visibleRecords.push_back(record);
		}
	}

	void PACSSetupStoreModel::AddExportHost(PACSConfig::ExportHost host)
	{
		PACSConfig::ExportHost temp;
		if (!FindExportHostByName(&temp, host.Name))
		{
			Record record;
			record.Data = host;
			record.Status = EStatus::Added;
			m_visibleRecords.push_back(record);
		}
	}

	void PACSSetupStoreModel::EditExportHostByName(PACSConfig::ExportHost host)
	{
		for (auto& record : m_visibleRecords)
		{
			if (record.Data.Name == host.Name)
			{
				if (record.Status == Normal)
				{
					record.Status = Modified;
				}
				record.Data = host;
				break;
			}
		}
	}

	void PACSSetupStoreModel::Delete(std::wstring name)
	{
		for (auto it = m_visibleRecords.begin(); it != m_visibleRecords.end(); ++it)
		{
			if ((*it).Data.Name == name)
			{
				m_deletedList.push_back((*it).Data);
				m_visibleRecords.erase(it);
				break;
			}
		}
	}

	bool PACSSetupStoreModel::FindExportHostByName(PACSConfig::ExportHost* pOutHost, std::wstring name)
	{
		for (auto& record : m_visibleRecords)
		{
			if (record.Data.Name == name)
			{
				*pOutHost = record.Data;
				return true;
			}
		}
		return false;
	}

	std::vector<PACSConfig::ExportHost> PACSSetupStoreModel::GetExportHostList()
	{
		std::vector<PACSConfig::ExportHost> hostList;
		for (auto& record : m_visibleRecords)
		{
			hostList.push_back(record.Data);
		}
		return hostList;
	}

	std::vector<PACSConfig::ExportHost> PACSSetupStoreModel::GetDeletedExportHostList()
	{
		return m_deletedList;
	}

	std::vector<PACSConfig::ExportHost> PACSSetupStoreModel::GetAddedExportHostList()
	{
		std::vector<PACSConfig::ExportHost> hostList;
		for (auto& record : m_visibleRecords)
		{
			if (record.Status == EStatus::Added)
			{
				hostList.push_back(record.Data);
			}
		}
		return hostList;
	}

	std::vector<PACSConfig::ExportHost> PACSSetupStoreModel::GetModifiedExportHostList()
	{
		std::vector<PACSConfig::ExportHost> hostList;
		for (auto& record : m_visibleRecords)
		{
			if (record.Status == EStatus::Modified)
			{
				hostList.push_back(record.Data);
			}
		}
		return hostList;
	}

}
