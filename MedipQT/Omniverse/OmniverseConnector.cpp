#include "stdafx.h"
#include "OmniverseConnector.h"
#include "OmniverseHeader.h"
#include "OmniverseStage.h"

class OmniverseConnectorPrivate
{
public:
	std::string RootUrl;
	std::string ProjectPath;
	std::string UsersPath;
	std::string MyUsersPath;
	std::string UserName;
	std::string DestinationPath;
	std::string IPAddress;
};

/*
	OmniverseConnector
*/
static void OmniClientConnectionStatusCallbackImpl(void* userData, const char* url, OmniClientConnectionStatus status) noexcept
{
}

OmniverseConnector::OmniverseConnector()
{
	m_p = std::make_shared<OmniverseConnectorPrivate>();
}

bool OmniverseConnector::IsConnect() const
{
	return !m_p->DestinationPath.empty();
}

bool OmniverseConnector::Connect(const std::string& ipAddress)
{
	m_p->IPAddress = ipAddress;

	m_p->RootUrl = OMNIVERSER_ROOT_URL + m_p->IPAddress;
	m_p->UsersPath = m_p->RootUrl + "/Users";

	omniClientReconnect(m_p->UsersPath.c_str());
	m_p->UserName = GetConnectedUserNameByUrl(m_p->UsersPath.c_str());
	if (m_p->UserName.empty())
	{
		qWarning() << "fail to omniverse connect : " << m_p->RootUrl.c_str();
		return false;
	}

	m_p->ProjectPath = m_p->RootUrl + "/Projects";
	m_p->MyUsersPath = m_p->UsersPath + "/" + m_p->UserName;

	m_p->DestinationPath = m_p->MyUsersPath;

	qInfo() << "omniverse connect Root : " << m_p->RootUrl.c_str() << ", user name : " << m_p->UserName.c_str();

	return true;
}

bool OmniverseConnector::Disconnect()
{
	if (IsConnect() == false)
	{
		return false;
	}

	omniClientLiveWaitForPendingUpdates();
	omniClientSignOut(m_p->UsersPath.c_str());

	m_p->IPAddress = "";
	m_p->DestinationPath = "";

	m_p->RootUrl = "";
	m_p->ProjectPath = "";
	m_p->UsersPath = "";
	m_p->MyUsersPath = "";
	m_p->UserName = "";

	return true;
}

std::string OmniverseConnector::GetRootUrl() const
{
	return m_p->RootUrl;
}

std::string OmniverseConnector::IPAddress() const
{
	return m_p->IPAddress;
}

std::string OmniverseConnector::GetUserName() const
{
	return m_p->UserName;
}

std::string OmniverseConnector::GetDestinationPath() const
{
	return m_p->DestinationPath;
}

OmniverseFileInfoPtr OmniverseConnector::GetRootDirectory() const
{
	return OmniverseFileInfo::CreateDir(this, "");
}

OmniverseFileInfoPtr OmniverseConnector::FindFileByUrl(const std::string& resourcePath) const
{
	OmniverseFileInfoPtr pRoot = GetRootDirectory();
	if (pRoot == nullptr)
	{
		return nullptr;
	}
	return pRoot->FindByUrl(resourcePath);
}

bool OmniverseConnector::IsStageExist(const std::string& filepath)
{
	if (OmniverseStage::IsSupportFileExtension(filepath) == false)
	{
		return false;
	}

	std::string usdUrl = m_p->RootUrl + "/" + filepath;
	UsdStageRefPtr pStage = UsdStage::Open(usdUrl);
	return pStage != nullptr;
}

std::string OmniverseConnector::GetAbsoluteUrl(const std::string resourcePath) const
{
	if (m_p->IPAddress.empty())
	{
		return "";
	}

	return m_p->RootUrl + "/" + resourcePath;
}

std::string OmniverseConnector::GetConnectedUserNameByUrl(const std::string& stageUrl)
{
	std::string userName("");
	omniClientWait(omniClientGetServerInfo(stageUrl.c_str(), &userName, [](void* userData, OmniClientResult result, struct OmniClientServerInfo const* info) noexcept
		{
			std::string* userName = static_cast<std::string*>(userData);
			if (userData && userName && info && info->username)
			{
				userName->assign(info->username);
			}
		}));

	return userName;
}
