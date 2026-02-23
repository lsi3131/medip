#pragma once

#include <string>
#include "Omniverse/OmniverseConnector.h"

class OmniverseFileUtil
{
public:
	static bool UploadFile(OmniverseConnector* pConnector, const std::string& serverResourceDirectoryPath, const std::string& localFilePath);
	static bool CreateDir(OmniverseConnector* pConnector, const std::string& resourceDirectoryPath);
	static bool DeleteServerFile(OmniverseConnector* pConnector, const std::string& resourceFilePath);

};
