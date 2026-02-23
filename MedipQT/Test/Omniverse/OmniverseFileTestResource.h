#pragma once

#include <string>
#include "test/test_pch.h"

class OmniverseFileTestResource
{
public:
	OmniverseFileTestResource()
	{
		LocalIpAddress = "localhost";
		TempRootDirPath = "Projects";

		TempUsdPath = TempRootDirPath + "/temp_usd.usd";
		TempUsdLiveDirPath = TempRootDirPath + "/.live/temp_usd.live";
		TempLivePath = TempRootDirPath + "/temp_usd.live";
		Server_EmptyUsdFilePath = "Projects/empty.usd";
	}

	void AddTempUsdFileList(int count)
	{
		for (int i = 0; i < count; ++i)
		{
			QString tempUsdPath = QString("%1/temp_usd_%2.usd").arg(TempRootDirPath.c_str()).arg(i);
			TempUsdFilePathList.push_back(tempUsdPath.toStdString());
		}
	}
	std::vector<std::string> GetTempUsdFilePathList() const
	{
		return TempUsdFilePathList;
	}

	void Clear()
	{
		TempUsdFilePathList.clear();
	}

	std::string LocalIpAddress;
	std::string TempRootDirPath;
	std::string TempUsdPath;
	std::string TempUsdLiveDirPath;
	std::string TempLivePath;
	std::string Server_EmptyUsdFilePath;

	std::vector<std::string> TempUsdFilePathList;
};
