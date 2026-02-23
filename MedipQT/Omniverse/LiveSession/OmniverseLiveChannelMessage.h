#pragma once

#include <memory>
#include <OmniClient.h>

enum class eOmniverseLiveChannelMessageType
{
	Join,
	Hello,
	GetUsers,
	Left,
	MergeStarted,
	MergeFinished,
	NumMsgTypes,
	Error,
};

class OmniverseLiveChannelMessage;
using OmniverseLiveChannelMessagePtr = std::shared_ptr<OmniverseLiveChannelMessage>;

class OmniverseChannelMessagePrivate;

class OmniverseLiveChannelMessage
{
public:
	static OmniverseLiveChannelMessagePtr CreateSendMessage(eOmniverseLiveChannelMessageType messageType, const std::string& fromUser, const std::string& appName);
	static OmniverseLiveChannelMessagePtr CreateRecvMessage(OmniClientChannelEvent eventType, const std::string& fromUser, OmniClientContent* content);

public:
	OmniverseLiveChannelMessage();
	virtual  ~OmniverseLiveChannelMessage();

	OmniClientContent GetOmniClientContent();
	eOmniverseLiveChannelMessageType GetType() const;

	std::string GetType_Text() const;

	std::string GetID() const;
	std::string GetUserName() const;
	std::string GetAppName() const;

private:
	bool buildSendMessage(eOmniverseLiveChannelMessageType messageType, const std::string& userName, const std::string& appName);
	bool buildRecvMessage(OmniClientChannelEvent eventType, const std::string& fromUserID, OmniClientContent* content);

private:
	std::shared_ptr<OmniverseChannelMessagePrivate> m_p;
};

