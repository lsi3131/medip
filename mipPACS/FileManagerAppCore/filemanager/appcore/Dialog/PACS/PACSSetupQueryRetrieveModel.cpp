#include "stdafx.h"
#include "PACSSetupQueryRetrieveModel.h"

namespace fm
{
	PACSSetupQueryRetrieveModel::PACSSetupQueryRetrieveModel()
	{
	}

	void PACSSetupQueryRetrieveModel::Clear()
	{
		m_visibleRecords.clear();
	}

	void PACSSetupQueryRetrieveModel::Init(const std::vector<PACSConfig::Host>& host)
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

	void PACSSetupQueryRetrieveModel::AddHost(PACSConfig::Host host)
	{
		PACSConfig::Host temp;
		if (!FindHostByName(&temp, host.Name))
		{
			Record record;
			record.Data = host;
			record.Status = EStatus::Added;
			m_visibleRecords.push_back(record);
		}
	}

	void PACSSetupQueryRetrieveModel::EditHostByName(PACSConfig::Host host)
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

	void PACSSetupQueryRetrieveModel::Delete(std::wstring name)
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

	bool PACSSetupQueryRetrieveModel::FindHostByName(PACSConfig::Host* pOutHost, std::wstring name)
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

	std::vector<PACSConfig::Host> PACSSetupQueryRetrieveModel::GetHostList()
	{
		std::vector<PACSConfig::Host> hostList;
		for (auto& record : m_visibleRecords)
		{
			hostList.push_back(record.Data);
		}
		return hostList;
	}

	std::vector<PACSConfig::Host> PACSSetupQueryRetrieveModel::GetDeletedHostList()
	{
		return m_deletedList;
	}

	std::vector<PACSConfig::Host> PACSSetupQueryRetrieveModel::GetAddedHostList()
	{
		std::vector<PACSConfig::Host> hostList;
		for (auto& record : m_visibleRecords)
		{
			if (record.Status == EStatus::Added)
			{
				hostList.push_back(record.Data);
			}
		}
		return hostList;
	}

	std::vector<PACSConfig::Host> PACSSetupQueryRetrieveModel::GetModifiedHostList()
	{
		std::vector<PACSConfig::Host> hostList;
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
