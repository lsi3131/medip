#pragma once

#include <memory>
#include <vector>
#include <string>
#include "Omniverse/LiveSession/OmniverseLiveChannel.h"

class OmniverseStage;
class mipUsdStage;

class OmniverseLiveSessionPrivate;

class OmniverseLiveSession;
using OmniverseLiveSessionPtr = std::shared_ptr<OmniverseLiveSession>;

class OmniverseLiveSession
{
public:
	enum class Key
	{
		Version,
		Admin,
		StageUrl,
		Description,
		Mode,
		Name,
		Invalid
	};

public:
	static OmniverseLiveSessionPtr New(OmniverseStage* pStage);

public:
	OmniverseLiveSession();
	OmniverseLiveSession(OmniverseStage* pStage);

public:
	std::shared_ptr<mipUsdStage> CreateSessionStage(const std::string& sessionName);
	std::shared_ptr<mipUsdStage> OpenSessionStage(const std::string& sessionName);

	bool IsLiveSessionFileExist(const std::string& sessionName) const;
	std::vector<std::string> GetSessionList() const;

	std::string GetOmniverseClientUrl() const;
	std::string GetStageDirectoryUrl() const;
	std::string GetSessionDirectoryPath() const;
	std::string GetSessionDirectoryUrl() const;
	std::string GetLiveSessionDirectoryPath(const std::string& sessionName) const;
	std::string GetLiveSessionLiveFileUrl(const std::string& sessionName) const;
	std::string GetLiveSessionTomlUrl(const std::string& sessionName) const;
	std::string GetMessageChannelUrl(const std::string& sessionName) const;

	std::string GetUserName() const;

private:
	void initByStage(OmniverseStage* pStage);
	std::string buildUrlString(const char* path) const;
	
private:
	std::shared_ptr<OmniverseLiveSessionPrivate> m_p;
};
