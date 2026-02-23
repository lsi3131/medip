#include "stdafx.h"
#include "OmniverseLiveSession.h"
#include "Omniverse/OmniverseContext.h"
#include "Omniverse/OmniverseStage.h"

#ifdef _WIN32
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem::v1;
#else
#endif
#include <toml/toml.h>

static const char* OMNIVERSE_CHANNEL_FILE_NAME = "__session__.channel";
static const char* LIVE_SUBFOLDER = "/.live";
static const char* LIVE_SUBFOLDER_SUFFIX = ".live";
static const char* SESSION_TOML_FILE_NAME = "__session__.toml";
static const char* DEFAULT_LIVE_FILE_NAME = "root.live";
static const char* CURRENT_VERSION = "1.0";

static const char* KEY_TOML_VERSION = "version";
static const char* KEY_TOML_ADMIN = "user_name";
static const char* KEY_TOML_STAGE_URL = "stage_url";
static const char* KEY_TOML_DESCRIPTION = "description";
static const char* KEY_TOML_MODE = "mode";
static const char* KEY_TOML_NAME = "name";

class OmniverseLiveSessionPrivate
{
public:
	OmniClientUrl* m_pOmniClientStageUrl;

	std::string m_stageUrl;
	std::string m_stageResourcePath;
	std::string m_stageDirectoryUrl;
	std::string m_liveSessionDirectoryPath;
	std::string m_liveSessionDirectoryUrl;

	std::string m_userName;

	OmniverseStage* pStage = nullptr;
};


OmniverseLiveSessionPtr OmniverseLiveSession::New(OmniverseStage* pStage)
{
	if (pStage->IsOpen() == false)
	{
		qWarning() << "omniverse stage is not online";
		return nullptr;
	}

	std::shared_ptr<OmniverseLiveSession> pLiveSession = std::make_shared<OmniverseLiveSession>(pStage);
	return pLiveSession;
}

OmniverseLiveSession::OmniverseLiveSession()
{
	m_p = std::make_shared<OmniverseLiveSessionPrivate>();
}

OmniverseLiveSession::OmniverseLiveSession(OmniverseStage* pStage)
{
	m_p = std::make_shared<OmniverseLiveSessionPrivate>();

	m_p->pStage = pStage;

	initByStage(m_p->pStage);
}

std::shared_ptr<mipUsdStage> OmniverseLiveSession::CreateSessionStage(const std::string& sessionName)
{
	if (IsLiveSessionFileExist(sessionName))
	{
		qWarning() << "live session file already exist : " << sessionName.c_str();
		return nullptr;
	}
	std::map<std::string, std::string> sessionTomlKeyMap = {
		{KEY_TOML_NAME, m_p->m_userName},
		{KEY_TOML_STAGE_URL, m_p->m_stageUrl},
		{KEY_TOML_MODE, "default"},
		{KEY_TOML_VERSION, CURRENT_VERSION}
	};

	bool success = false;
	toml::Value root((toml::Table()));
	for (const auto& dataPair : sessionTomlKeyMap)
	{
		root.setChild(dataPair.first, dataPair.second);
	}

	std::ostringstream oss;
	root.write(&oss);
	OmniClientContent content = omniClientAllocContent(oss.str().length());
	std::memcpy(content.buffer, oss.str().c_str(), content.size);
	std::string sessionConfigFileUrl = GetLiveSessionTomlUrl(sessionName);

	omniClientWait(omniClientWriteFile(
		sessionConfigFileUrl.c_str(),
		&content,
		&success,
		[](void* userData, OmniClientResult result) noexcept
		{
			bool* success = static_cast<bool*>(userData);
			*success = (result == OmniClientResult::eOmniClientResult_Ok);
		}));

	if (success == false)
	{
		return nullptr;
	}

	std::string liveSessionFileUrl = GetLiveSessionLiveFileUrl(sessionName);

	std::shared_ptr<mipUsdStage> pLiveSessionStage = std::make_shared<mipUsdStage>();
	if (pLiveSessionStage->CreateNew(liveSessionFileUrl) == false)
	{
		qWarning() << "fail to create new live session stage : " << liveSessionFileUrl.c_str();
		return nullptr;
	}

	return pLiveSessionStage;
}

std::shared_ptr<mipUsdStage> OmniverseLiveSession::OpenSessionStage(const std::string& sessionName)
{
	if (IsLiveSessionFileExist(sessionName) == false)
	{
		qWarning() << "live session file is not exist : " << sessionName.c_str();
		return nullptr;
	}

	std::string liveSessionFileUrl = GetLiveSessionLiveFileUrl(sessionName);

	std::shared_ptr<mipUsdStage> pLiveSessionStage = std::make_shared<mipUsdStage>();
	if (pLiveSessionStage->Open(liveSessionFileUrl) == false)
	{
		qWarning() << "fail to open new live session stage : " << liveSessionFileUrl.c_str();
		return nullptr;
	}

	return pLiveSessionStage;
}

bool OmniverseLiveSession::IsLiveSessionFileExist(const std::string& sessionName) const
{
	std::vector<std::string> sessionList = GetSessionList();
	auto it = std::find(sessionList.begin(), sessionList.end(), sessionName);
	return it != sessionList.end();
}

std::vector<std::string> OmniverseLiveSession::GetSessionList() const
{
	std::vector<std::string> folderList;
	omniClientWait(omniClientList(m_p->m_liveSessionDirectoryUrl.c_str(), &folderList,
		[](void* userData, OmniClientResult result, uint32_t numEntries, struct OmniClientListEntry const* entries) noexcept {
			std::vector<std::string>* pFolderList = static_cast<std::vector<std::string>*>(userData);
			for (uint32_t i = 0; i < numEntries; i++)
			{
				pFolderList->push_back(entries[i].relativePath);
			}
		}));

	std::vector<std::string> sessionNames;
	for (int i = 0; i < folderList.size(); i++)
	{
		fs::path p(folderList[i]);
		sessionNames.push_back(p.stem().string());
	}
	return sessionNames;
}

std::string OmniverseLiveSession::GetOmniverseClientUrl() const
{
	return m_p->m_pOmniClientStageUrl->path;
}

std::string OmniverseLiveSession::GetStageDirectoryUrl() const
{
	return m_p->m_stageDirectoryUrl;
}

std::string OmniverseLiveSession::GetSessionDirectoryPath() const
{
	return m_p->m_liveSessionDirectoryPath;
}

std::string OmniverseLiveSession::GetSessionDirectoryUrl() const
{
	return m_p->m_liveSessionDirectoryUrl;
}

std::string OmniverseLiveSession::GetLiveSessionDirectoryPath(const std::string& sessionName) const
{
	return m_p->m_liveSessionDirectoryPath + "/" + sessionName + LIVE_SUBFOLDER_SUFFIX;
}

std::string OmniverseLiveSession::GetLiveSessionLiveFileUrl(const std::string& sessionName) const
{
	std::string liveSessionPath = GetLiveSessionDirectoryPath(sessionName) + "/" + DEFAULT_LIVE_FILE_NAME;
	return buildUrlString(liveSessionPath.c_str());
}

std::string OmniverseLiveSession::GetLiveSessionTomlUrl(const std::string& sessionName) const
{
	std::string liveSessionTomlUrl = GetLiveSessionDirectoryPath(sessionName) + "/" + SESSION_TOML_FILE_NAME;
	return buildUrlString(liveSessionTomlUrl.c_str());
}

std::string OmniverseLiveSession::GetMessageChannelUrl(const std::string& sessionName) const
{
	std::string liveSessionTomlUrl = GetLiveSessionDirectoryPath(sessionName) + "/" + OMNIVERSE_CHANNEL_FILE_NAME;
	return buildUrlString(liveSessionTomlUrl.c_str());
}

std::string OmniverseLiveSession::GetUserName() const
{
	return m_p->m_userName;
}

void OmniverseLiveSession::initByStage(OmniverseStage* pStage)
{
	std::string stageBaseFileName = pStage->GetFileBaseName();
	m_p->m_stageUrl = pStage->GetStageUrl();
	m_p->m_userName = pStage->GetConnector()->GetUserName();

	m_p->m_pOmniClientStageUrl = omniClientBreakUrl(m_p->m_stageUrl.c_str());

	m_p->m_stageResourcePath = m_p->m_pOmniClientStageUrl->path;

	std::string parent_path = fs::path(m_p->m_stageResourcePath).parent_path().string();
	std::replace(parent_path.begin(), parent_path.end(), '\\', '/');
	m_p->m_stageDirectoryUrl = buildUrlString(parent_path.c_str());
	m_p->m_liveSessionDirectoryPath = parent_path + LIVE_SUBFOLDER + "/" + stageBaseFileName + LIVE_SUBFOLDER_SUFFIX;
	m_p->m_liveSessionDirectoryUrl = buildUrlString(m_p->m_liveSessionDirectoryPath.c_str());
}

std::string OmniverseLiveSession::buildUrlString(const char* path) const
{
	std::string urlString;
	OmniClientUrl omniUrl = *m_p->m_pOmniClientStageUrl;
	omniUrl.path = path;

	size_t expectedSize = 0;
	omniClientMakeUrl(&omniUrl, nullptr, &expectedSize);
	char* buf = new char[expectedSize];
	omniClientMakeUrl(&omniUrl, buf, &expectedSize);
	urlString.assign(buf);
	delete[] buf;
	return urlString;
}


