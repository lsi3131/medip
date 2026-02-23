#include "stdafx.h"
#include "Test/test_pch.h"
#include "Omniverse/OmniverseContext.h"
#include "Omniverse/LiveSession/OmniverseLiveSession.h"
#include "Omniverse/OmniverseFileUtil.h"
#include "Test/Omniverse/OmniverseFileTestResource.h"
#include <thread>
#include <QElapsedTimer>

class Test_OmniverseLiveSession : public ::testing::Test
{
public:
	void SetUp() override
	{
		m_ipAddress = m_OmniFileResource.LocalIpAddress;
		m_tempServerUsdPath = m_OmniFileResource.TempUsdPath;
		m_tempServerUsdLiveDirPath = m_OmniFileResource.TempUsdLiveDirPath;

		OmniverseConnector connector;
		connector.Connect(m_ipAddress);
		OmniverseFileUtil::DeleteServerFile(&connector, m_tempServerUsdPath);
		OmniverseFileUtil::DeleteServerFile(&connector, m_tempServerUsdLiveDirPath);

		m_pConnector = new OmniverseConnector();
	}

	void TearDown() override
	{
		delete m_pConnector;
	}

	std::shared_ptr<OmniverseStage> CreateStage(OmniverseConnector* pConnector, const std::string& filepath)
	{
		auto pStage = std::make_shared<OmniverseStage>();
		pStage->Create(pConnector, filepath);
		return pStage;
	}

	std::shared_ptr<OmniverseStage> OpenStage(OmniverseConnector* pConnector, const std::string& filepath)
	{
		auto pStage = std::make_shared<OmniverseStage>();
		pStage->Open(pConnector, filepath);
		return pStage;
	}

protected:
	OmniverseConnector* m_pConnector;
	OmniverseStage* m_pStage;
	std::string m_ipAddress;
	std::string m_tempServerUsdPath;
	std::string m_tempServerUsdLiveDirPath;

	OmniverseFileTestResource m_OmniFileResource;
};

TEST_F(Test_OmniverseLiveSession, WhenStageNotOpen_ReturnNull)
{
	//m_pConnector->Connect(m_OmniFileResource.LocalIpAddress);
	OmniverseStage stage;
	std::shared_ptr<OmniverseLiveSession> pLiveSession = OmniverseLiveSession::New(&stage);
	EXPECT_EQ(nullptr, pLiveSession);
}

TEST_F(Test_OmniverseLiveSession, WhenConnectedAndOnlineStage_CreateLiveSession_Success)
{
	OmniverseConnector connector;
	OmniverseStage stage;
	connector.Connect(m_ipAddress);
	stage.Create(&connector, m_tempServerUsdPath);

	std::shared_ptr<OmniverseLiveSession> pLiveSession = OmniverseLiveSession::New(&stage);
	EXPECT_NE(nullptr, pLiveSession);

	EXPECT_STREQ("/Projects/temp_usd.usd", pLiveSession->GetOmniverseClientUrl().c_str());
	EXPECT_STREQ("omniverse://localhost/Projects", pLiveSession->GetStageDirectoryUrl().c_str());
	EXPECT_STREQ("/Projects/.live/temp_usd.live", pLiveSession->GetSessionDirectoryPath().c_str());
	EXPECT_STREQ("omniverse://localhost/Projects/.live/temp_usd.live", pLiveSession->GetSessionDirectoryUrl().c_str());
}

TEST_F(Test_OmniverseLiveSession, WhenCreateSessionStage_LiveFileShouldBeCreated)
{
	m_pConnector->Connect(m_ipAddress);
	std::shared_ptr<OmniverseStage> pStage = CreateStage(m_pConnector, m_tempServerUsdPath);
	std::shared_ptr<OmniverseLiveSession> pLiveSession = OmniverseLiveSession::New(pStage.get());

	std::shared_ptr<mipUsdStage> pLiveStage = pLiveSession->CreateSessionStage("session_name");
	EXPECT_NE(nullptr, pLiveStage);

	OmniverseFileInfoPtr pDir = m_pConnector->FindFileByUrl("Projects/.live/temp_usd.live/session_name.live");
	OmniverseFileInfoPtr pSession_TOML = m_pConnector->FindFileByUrl("Projects/.live/temp_usd.live/session_name.live/__session__.toml");
	OmniverseFileInfoPtr pSession_RootLive = m_pConnector->FindFileByUrl("Projects/.live/temp_usd.live/session_name.live/root.live");
	EXPECT_NE(nullptr, pDir);
	EXPECT_NE(nullptr, pSession_TOML);
	EXPECT_NE(nullptr, pSession_RootLive);
}

TEST_F(Test_OmniverseLiveSession, TestGetSessionList)
{
	m_pConnector->Connect(m_ipAddress);
	std::shared_ptr<OmniverseStage> pStage = CreateStage(m_pConnector, m_tempServerUsdPath);
	std::shared_ptr<OmniverseLiveSession> pLiveSession = OmniverseLiveSession::New(pStage.get());

	std::vector<std::string> sessionNameList;
	sessionNameList = pLiveSession->GetSessionList();
	EXPECT_EQ(0, sessionNameList.size());

	pLiveSession->CreateSessionStage("session_name_1");
	pLiveSession->CreateSessionStage("session_name_2");

	sessionNameList = pLiveSession->GetSessionList();
	EXPECT_EQ(2, sessionNameList.size());
	EXPECT_STREQ("session_name_1", sessionNameList[0].c_str());
	EXPECT_STREQ("session_name_2", sessionNameList[1].c_str());
}

TEST_F(Test_OmniverseLiveSession, WhenSessionFileExist_CreateSession_ReturnNull)
{
	m_pConnector->Connect(m_ipAddress);
	std::shared_ptr<OmniverseStage> pStage = CreateStage(m_pConnector, m_tempServerUsdPath);
	std::shared_ptr<OmniverseLiveSession> pLiveSession = OmniverseLiveSession::New(pStage.get());

	std::shared_ptr<mipUsdStage> pLiveStage;
	pLiveStage = pLiveSession->CreateSessionStage("session_name");
	EXPECT_NE(nullptr, pLiveStage);

	pLiveStage = pLiveSession->CreateSessionStage("session_name");
	EXPECT_EQ(nullptr, pLiveStage);
}

TEST_F(Test_OmniverseLiveSession, WheJoinNotExistSession_ReturnNull)
{
	m_pConnector->Connect(m_ipAddress);
	std::shared_ptr<OmniverseStage> pStage = CreateStage(m_pConnector, m_tempServerUsdPath);
	std::shared_ptr<OmniverseLiveSession> pLiveSession = OmniverseLiveSession::New(pStage.get());

	std::shared_ptr<mipUsdStage> pLiveStage = pLiveSession->OpenSessionStage("session_name");
	EXPECT_EQ(nullptr, pLiveStage);
}

TEST_F(Test_OmniverseLiveSession, TestCreateAndJoinLiveSessionStage)
{
	m_pConnector->Connect(m_ipAddress);
	std::shared_ptr<OmniverseStage> pStage = CreateStage(m_pConnector, m_tempServerUsdPath);
	std::shared_ptr<OmniverseLiveSession> pLiveSession = OmniverseLiveSession::New(pStage.get());
	std::shared_ptr<mipUsdStage> pLiveStage;

	pLiveSession->CreateSessionStage("session_name");
	pLiveStage = pLiveSession->OpenSessionStage("session_name");
	EXPECT_NE(nullptr, pLiveStage);
}


TEST_F(Test_OmniverseLiveSession, TestCreateAndOpen_MultiLiveStage)
{
	m_pConnector->Connect(m_ipAddress);

	std::shared_ptr<OmniverseStage> pStage_1 = CreateStage(m_pConnector, m_tempServerUsdPath);
	std::shared_ptr<OmniverseStage> pStage_2 = OpenStage(m_pConnector, m_tempServerUsdPath);

	std::shared_ptr<OmniverseLiveSession> pLiveSession_1 = OmniverseLiveSession::New(pStage_1.get());
	std::shared_ptr<OmniverseLiveSession> pLiveSession_2 = OmniverseLiveSession::New(pStage_2.get());

	std::shared_ptr<mipUsdStage> pLiveStage_1 = pLiveSession_1->CreateSessionStage("session_name");
	std::shared_ptr<mipUsdStage> pLiveStage_2 = pLiveSession_2->OpenSessionStage("session_name");

	EXPECT_NE(nullptr, pLiveStage_1);
	EXPECT_NE(nullptr, pLiveStage_2);
}


