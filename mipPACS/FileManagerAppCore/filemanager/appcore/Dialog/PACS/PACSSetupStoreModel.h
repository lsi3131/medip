#pragma once

#include "FileManager/config/PACSConfig.h"
#include "filemanager/export.h"
#include <vector>

namespace fm
{
	class FM_CORE_EXPORT PACSSetupStoreModel
	{
	public:
		enum EStatus { Normal, Added, Modified };

		struct Record
		{
			EStatus Status;
			PACSConfig::ExportHost Data;
		};
	public:
		PACSSetupStoreModel();

		void Clear();
		void Init(const std::vector<PACSConfig::ExportHost>& exportHost);
		void AddExportHost(PACSConfig::ExportHost exportHost);
		void EditExportHostByName(PACSConfig::ExportHost editHost);
		void Delete(std::wstring name);
		bool FindExportHostByName(PACSConfig::ExportHost* pOutHost, std::wstring name);

		std::vector<PACSConfig::ExportHost> GetExportHostList();
		std::vector<PACSConfig::ExportHost> GetAddedExportHostList();
		std::vector<PACSConfig::ExportHost> GetModifiedExportHostList();
		std::vector<PACSConfig::ExportHost> GetDeletedExportHostList();

	private:
		std::vector<Record> m_visibleRecords;
		std::vector<PACSConfig::ExportHost> m_deletedList;
	};
}
