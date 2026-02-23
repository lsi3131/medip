#pragma once

#include <vector>
#include "Omniverse/Usd/mipUsdPreset.h"

class mipUsdPresetManager
{
public:
	mipUsdPresetManager();

public:
	bool Init(const std::string& rootPresetDirectoryPath);

	bool Add(mipUsdPresetPtr pNewPreset);
	bool DeleteByName(const std::string& name);

	std::vector<mipUsdPresetPtr> GetPresetList() const;

	mipUsdPresetPtr GetPresetByName(const std::string& name);

	bool IsPresetExist(const std::string& name);
	std::string GetRootPresetDirectoryPath() const;


private:
	std::string m_rootPresetDirectoryPath;

	std::vector<mipUsdPresetPtr> m_presetList;
};

