#pragma once

#include "FileManager/config/PACSConfig.h"
#include "filemanager/export.h"
#include <vector>

namespace fm
{
	class FM_CORE_EXPORT PACSSetupQueryRetrieveModel
	{
	public:
		enum EStatus { Normal, Added, Modified };
		
		struct Record
		{
			EStatus Status;
			PACSConfig::Host Data;
		};
	public:
		PACSSetupQueryRetrieveModel();

		void Clear();
		void Init(const std::vector<PACSConfig::Host>& host);
		void AddHost(PACSConfig::Host host);
		void EditHostByName(PACSConfig::Host editHost);
		void Delete(std::wstring name);
		bool FindHostByName(PACSConfig::Host* pOutHost, std::wstring name);

		std::vector<PACSConfig::Host> GetHostList();
		std::vector<PACSConfig::Host> GetAddedHostList();
		std::vector<PACSConfig::Host> GetModifiedHostList();
		std::vector<PACSConfig::Host> GetDeletedHostList();

	private:
		std::vector<Record> m_visibleRecords;
		std::vector<PACSConfig::Host> m_deletedList;
	};
}
