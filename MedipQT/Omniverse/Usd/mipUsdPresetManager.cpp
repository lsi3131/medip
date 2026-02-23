#include "stdafx.h"
#include "mipUsdPresetManager.h"

mipUsdPresetManager::mipUsdPresetManager()
{
}

bool mipUsdPresetManager::Init(const std::string& rootPresetDirectoryPath)
{
	QFileInfo fileInfo(QString::fromStdString(rootPresetDirectoryPath));
	if (fileInfo.isDir() == false)
	{
		return false;
	}

	m_rootPresetDirectoryPath = rootPresetDirectoryPath;
	QDir dir(QString::fromStdString(m_rootPresetDirectoryPath));

	QFileInfoList presetNameInfoList = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
	for (QFileInfo& presetNameInfo : presetNameInfoList)
	{
		mipUsdPresetPtr pNewPreset = std::make_shared<mipUsdPreset>();
		std::string presetDirPath = presetNameInfo.absoluteFilePath().toStdString();
		if (pNewPreset->LoadPreset(presetDirPath))
		{
			if (Add(pNewPreset) == false)
			{
				qWarning() << "fail to load preset. dir path : " << presetDirPath.c_str();
			}
		}
	}

	return true;
}

bool mipUsdPresetManager::Add(mipUsdPresetPtr pNewPreset)
{
	if (pNewPreset == nullptr)
	{
		qWarning() << "preset is null";
		return false;
	}

	if (pNewPreset->IsPresetFileLoaded() == false)
	{
		qWarning() << "preset file not loaded";
		return false;
	}

	m_presetList.push_back(pNewPreset);
	return true;
}

bool mipUsdPresetManager::DeleteByName(const std::string& name)
{
	mipUsdPresetPtr pPreset = GetPresetByName(name);
	if (pPreset == nullptr)
	{
		return false;
	}

	auto it = std::find(m_presetList.begin(), m_presetList.end(), pPreset);

	if (it == m_presetList.end())
	{
		return false;
	}

	if ((*it)->DeletePresetFile() == false)
	{
		qCritical() << "delete preset fail. DeletePresetFile() must be worked. name : " << name.c_str();
		Q_ASSERT(false);
	}

	m_presetList.erase(it);
	return true;
}

std::vector<mipUsdPresetPtr> mipUsdPresetManager::GetPresetList() const
{
	return m_presetList;
}

mipUsdPresetPtr mipUsdPresetManager::GetPresetByName(const std::string& name)
{
	for (auto pPreset : m_presetList)
	{
		if (pPreset->GetPresetName() == name)
		{
			return pPreset;
		}
	}

	return nullptr;
}

bool mipUsdPresetManager::IsPresetExist(const std::string& name)
{
	mipUsdPresetPtr pPreset = GetPresetByName(name);
	return pPreset != nullptr;
}

std::string mipUsdPresetManager::GetRootPresetDirectoryPath() const
{
	return m_rootPresetDirectoryPath;
}

