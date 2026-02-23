#include "stdafx.h"
#include "Test/test_pch.h"
#include "Omniverse/OmniverseContext.h"
#include "Omniverse/LiveSession/OmniverseLiveSession.h"
#include "Omniverse/OmniverseFileUtil.h"
#include "Test/Omniverse/OmniverseFileTestResource.h"
#include <thread>
#include <QElapsedTimer>

class Test_OmniverseLiveChannel : public ::testing::Test
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

TEST_F(Test_OmniverseLiveChannel, WhenLiveSessionStageNotExist_OpenChannelReturnFalse)
{
	m_pConnector->Connect(m_ipAddress);
	std::shared_ptr<OmniverseStage> pStage = CreateStage(m_pConnector, m_tempServerUsdPath);
	std::shared_ptr<OmniverseLiveSession> pLiveSession = OmniverseLiveSession::New(pStage.get());

	OmniverseLiveChannel channel;
	EXPECT_FALSE(channel.OpenChannel(*pLiveSession, "session_name", "MEDIP"));
	EXPECT_FALSE(channel.IsChannelOpen());
}

TEST_F(Test_OmniverseLiveChannel, WhenOpenChannel_ChannelFileShouldBeCreated)
{
	m_pConnector->Connect(m_ipAddress);
	std::shared_ptr<OmniverseStage> pStage = CreateStage(m_pConnector, m_tempServerUsdPath);
	std::shared_ptr<OmniverseLiveSession> pLiveSession = OmniverseLiveSession::New(pStage.get());
	pLiveSession->CreateSessionStage("session_name");

	OmniverseLiveChannel channel;
	EXPECT_TRUE(channel.OpenChannel(*pLiveSession, "session_name", "MEDIP"));
	EXPECT_TRUE(channel.IsChannelOpen());

	OmniverseFileInfoPtr pSession_Channel = m_pConnector->FindFileByUrl("Projects/.live/temp_usd.live/session_name.live/__session__.channel");
	EXPECT_NE(nullptr, pSession_Channel);
}
TEST_F(Test_OmniverseLiveChannel, TestOpenMultiChannel)
{
	m_pConnector->Connect(m_ipAddress);

	std::shared_ptr<OmniverseStage> pStage = CreateStage(m_pConnector, m_tempServerUsdPath);
	std::shared_ptr<OmniverseLiveSession> pLiveSession = OmniverseLiveSession::New(pStage.get());
	pLiveSession->CreateSessionStage("session_name");

	OmniverseLiveChannel channel_1;
	OmniverseLiveChannel channel_2;

	channel_1.OpenChannel(*pLiveSession, "session_name", "MEDIP_1");
	channel_2.OpenChannel(*pLiveSession, "session_name", "MEDIP_2");

	EXPECT_NE(0, channel_1.GetChannelID());
	EXPECT_NE(0, channel_2.GetChannelID());
}

TEST_F(Test_OmniverseLiveChannel, TestSendMessageToChannel)
{
	m_pConnector->Connect(m_ipAddress);

	std::shared_ptr<OmniverseStage> pStage = CreateStage(m_pConnector, m_tempServerUsdPath);
	std::shared_ptr<OmniverseLiveSession> pLiveSession = OmniverseLiveSession::New(pStage.get());

	pLiveSession->CreateSessionStage("session_name");

	std::vector<OmniverseLiveChannelMessagePtr> receiveMessageList;
	auto pCallbackLeaveChannel = [](OmniverseLiveChannel* pChannel, OmniverseLiveChannelMessagePtr pMessage, void* pUserData) {
		std::vector<OmniverseLiveChannelMessagePtr>* pReceiveMessageList = nullptr;
		if (pUserData)
		{
			pReceiveMessageList = (std::vector<OmniverseLiveChannelMessagePtr>*)pUserData;
			pReceiveMessageList->push_back(pMessage);
		}
	};

	OmniverseLiveChannel channel_1;
	OmniverseLiveChannel channel_2;

	channel_2.AddReceiveMessageHandler(pCallbackLeaveChannel, &receiveMessageList);

	channel_1.OpenChannel(*pLiveSession, "session_name", "MEDIP_1");
	channel_2.OpenChannel(*pLiveSession, "session_name", "MEDIP_2");


	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	int sendID = channel_1.SendChannelMessage(eOmniverseLiveChannelMessageType::Join);
	EXPECT_NE(0, sendID);

	//std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	//EXPECT_EQ(1, receiveMessageList.size());
}

