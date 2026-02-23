#pragma once

#include <string>
#include <memory>
#include "Omniverse/OmniverseFileInfo.h"

class OmniverseConnectorPrivate;
class OmniverseConnector;

using OmniverseConnectorPtr = std::shared_ptr<OmniverseConnector>;

class OmniverseConnector
{
public:
	OmniverseConnector();

	bool IsConnect() const;
	bool Connect(const std::string& ipAddress);
	bool Disconnect();

	std::string GetRootUrl() const;
	std::string IPAddress() const;
	std::string GetUserName() const;
	OmniverseFileInfoPtr GetRootDirectory() const;
	std::string GetDestinationPath() const;
	OmniverseFileInfoPtr FindFileByUrl(const std::string& resourcePath) const;
	bool IsStageExist(const std::string& filepath);
	std::string GetAbsoluteUrl(const std::string resourcePath) const;
	std::string GetConnectedUserNameByUrl(const std::string& stageUrl);

private:
	std::shared_ptr<OmniverseConnectorPrivate> m_p;
};
