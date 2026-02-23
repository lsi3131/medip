#pragma once

#include <string>
#include <vector>
#include <memory>

class OmniverseConfigPrivate;

class OmniverseConfig
{
public:
	OmniverseConfig();
public:
	bool Init(const std::string& configFilePath);

	std::vector<std::string> GetServerList() const;
	void AddServer(const std::string& serverName);
	void EditServer(const std::string& from, const std::string& to);
	void DeleteServer(const std::string& serverName);

	std::string GetCurrentServer() const;
	void SetCurrentServer(const std::string& serverName);

private:
	std::shared_ptr<OmniverseConfigPrivate> m_p;
};
