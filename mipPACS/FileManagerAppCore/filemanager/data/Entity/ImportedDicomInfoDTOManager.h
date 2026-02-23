#pragma once

#include "filemanager/data/Entity/ImportedDicomInfoDTO.h"
#include <map>
#include <unordered_map>

namespace fm
{
	class ImportedDicomInfoDTOManager
	{
	public:
		ImportedDicomInfoDTOManager();

	public:
		std::vector<ImportedDicomInfoDTO> GetDTOList();
		void Add(const ImportedDicomInfoDTO& data);
		void Clear();

	private:
		//std::map<ImportedDicomInfoDTO_Key, ImportedDicomInfoDTO> m_map;
		//두개 합쳐서 Key값으로 쓸것 
		std::unordered_map<std::wstring, ImportedDicomInfoDTO> m_map;
	};
}
