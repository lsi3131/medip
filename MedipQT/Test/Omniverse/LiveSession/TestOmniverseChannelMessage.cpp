#include "stdafx.h"
#include "Test/test_pch.h"
#include "Omniverse/LiveSession/OmniverseLiveChannelMessage.h"

class TestOmniverseChannelMessage : public ::testing::Test
{
public:
	void SetUp() override
	{
	}
	void TearDown() override
	{
	}

protected:
};

TEST_F(TestOmniverseChannelMessage, Test_CreateRecvMessage)
{
	//OmniClientEvent
	OmniClientChannelEvent omniEvent = OmniClientChannelEvent::eOmniClientChannelEvent_Join;
	std::string fromUser = "user";
	OmniClientContent content;

	//OmniverseLiveChannelMessage::CreateRecvMessage();
}
