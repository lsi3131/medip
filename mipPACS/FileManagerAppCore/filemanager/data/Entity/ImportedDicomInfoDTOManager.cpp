#include "stdafx.h"
#include "ImportedDicomInfoDTOManager.h"
#include <QFileInfo>

namespace fm
{
	ImportedDicomInfoDTOManager::ImportedDicomInfoDTOManager()
	{

	}

	std::vector<ImportedDicomInfoDTO> ImportedDicomInfoDTOManager::GetDTOList()
	{
		std::vector<ImportedDicomInfoDTO> dtoList;
		for (auto& dto_pair : m_map)
		{
			dtoList.push_back(dto_pair.second);
		}
		return dtoList;
	}

	void ImportedDicomInfoDTOManager::Add(const ImportedDicomInfoDTO& data)
	{
		std::wstring key = data.StudyInstanceUID + L"/" + data.SeriesInstanceUID;
		auto it = m_map.find(key);
		QFileInfo fileInfo(QString::fromStdWString(data.DicomFilePath));
		QString filename = fileInfo.fileName();
		if (it != m_map.end())
		{
			it->second.ImageCount++;
			it->second.DicomFileList.push_back(filename.toStdWString());
		}
		else
		{
			ImportedDicomInfoDTO newData = data;
			newData.ImageCount = 1;
			newData.DicomFileList.push_back(filename.toStdWString());
			m_map.insert(std::make_pair(key, newData));
		}
	}

	void ImportedDicomInfoDTOManager::Clear()
	{
		m_map.clear();
	}
}