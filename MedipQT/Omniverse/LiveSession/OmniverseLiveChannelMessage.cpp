#include "stdafx.h"
#include "OmniverseLiveChannelMessage.h"
#include "Omniverse/OmniverseHeader.h"

static constexpr const char* MSG_HEADER = "__OVUM__";
static constexpr const size_t MSG_HEADER_LEN = 8;
static constexpr const char* MSG_VERSION = "3.0";
static constexpr const char* CUSTOM_MSG_TYPE = "message";
static constexpr const char* MESSAGE_MSG_TYPE = "MESSAGE";
static constexpr const char* CUSTOM_MSG_GROUP_KEY = "__SESSION_MANAGEMENT__";
static constexpr const char* CUSTOM_MSG_VERSION = "1.0";

static constexpr const char* MSG_VERSION_KEY = "version";
static constexpr const char* MSG_USERNAME_KEY = "from_user_name";
static constexpr const char* MSG_APP_KEY = "app";
static constexpr const char* MSG_TYPE_KEY = "message_type";
static constexpr const char* MSG_CONTENT_KEY = "content";

static eOmniverseLiveChannelMessageType StringTypeToMessageType(const std::string& msgStr)
{
	if ("HELLO" == msgStr)
	{
		return eOmniverseLiveChannelMessageType::Hello;
	}
	else if ("JOIN" == msgStr)
	{
		return eOmniverseLiveChannelMessageType::Join;
	}
	else if ("GET_USERS" == msgStr)
	{
		return eOmniverseLiveChannelMessageType::GetUsers;
	}
	else if ("LEFT" == msgStr)
	{
		return eOmniverseLiveChannelMessageType::Left;
	}
	else if ("MERGE_STARTED" == msgStr)
	{
		return eOmniverseLiveChannelMessageType::MergeStarted;
	}
	else if ("MERGE_FINISHED" == msgStr)
	{
		return eOmniverseLiveChannelMessageType::MergeFinished;
	}
	else
	{
		return eOmniverseLiveChannelMessageType::NumMsgTypes;
	}
}

static const char* MessageTypeToStringType(eOmniverseLiveChannelMessageType msgType)
{
	if (msgType == eOmniverseLiveChannelMessageType::Hello)
	{
		return "HELLO";
	}
	else if (msgType == eOmniverseLiveChannelMessageType::Join) 
	{
		return "JOIN";
	}
	else if (msgType == eOmniverseLiveChannelMessageType::GetUsers)
	{
		return "GET_USERS";
	}
	else if (msgType == eOmniverseLiveChannelMessageType::Left)
	{
		return "LEFT";
	}
	else if (msgType == eOmniverseLiveChannelMessageType::MergeStarted)
	{
		return "MERGE_STARTED";
	}
	else if (msgType == eOmniverseLiveChannelMessageType::MergeFinished)
	{
		return "MERGE_FINISHED";
	}
	else
	{
		return "INVALID";
	}
}

class OmniverseChannelMessagePrivate
{
public:
	nlohmann::json Json;
	std::string FromUser;
};

OmniverseLiveChannelMessagePtr OmniverseLiveChannelMessage::CreateSendMessage(
	eOmniverseLiveChannelMessageType messageType,
	const std::string& fromUser,
	const std::string& appName)
{
	auto pNew = std::make_shared<OmniverseLiveChannelMessage>();
	pNew->buildSendMessage(messageType, fromUser, appName);
	return pNew;
}

OmniverseLiveChannelMessagePtr OmniverseLiveChannelMessage::CreateRecvMessage(
	OmniClientChannelEvent eventType,
	const std::string& fromUser,
	OmniClientContent* content)
{
	auto pNew = std::make_shared<OmniverseLiveChannelMessage>();
	if (pNew->buildRecvMessage(eventType, fromUser, content) == false)
	{
		return nullptr;
	}
	return pNew;
}

OmniverseLiveChannelMessage::OmniverseLiveChannelMessage()
{
	m_p = std::make_shared<OmniverseChannelMessagePrivate>();
}

OmniverseLiveChannelMessage::~OmniverseLiveChannelMessage()
{
}

bool OmniverseLiveChannelMessage::buildSendMessage(eOmniverseLiveChannelMessageType messageType, const std::string& fromUser, const std::string& appName)
{
	m_p->Json[MSG_VERSION_KEY] = MSG_VERSION;
	m_p->Json[MSG_USERNAME_KEY] = fromUser;
	m_p->Json[MSG_APP_KEY] = appName;
	nlohmann::json emptyContent;
	//m_p->Json["content"] = emptyContent;

	if (messageType == eOmniverseLiveChannelMessageType::MergeStarted || messageType == eOmniverseLiveChannelMessageType::MergeFinished)
	{
		m_p->Json[MSG_TYPE_KEY] = MESSAGE_MSG_TYPE;
		nlohmann::json sessionContentMsg = {
			{CUSTOM_MSG_GROUP_KEY, {
					{MSG_VERSION_KEY, CUSTOM_MSG_VERSION}, // Currently not sent by Kit
					{CUSTOM_MSG_TYPE,  MessageTypeToStringType(messageType)}
				},
			}
		};
		m_p->Json[MSG_CONTENT_KEY] = sessionContentMsg;
	}
	else
	{
		m_p->Json[MSG_TYPE_KEY] = MessageTypeToStringType(messageType);
	}

	return true;
}

bool OmniverseLiveChannelMessage::buildRecvMessage(OmniClientChannelEvent eventType, const std::string& fromUser, OmniClientContent* content)
{
	m_p->FromUser = fromUser;

	if (eventType == eOmniClientChannelEvent_Message &&
		content && 
		content->buffer && 
		content->size > MSG_HEADER_LEN)
	{
		// Check the first 8 bytes is "__OVUM__"
		std::string bufferStr((char*)content->buffer, content->size);
		if (MSG_HEADER == bufferStr.substr(0, MSG_HEADER_LEN))
		{
			m_p->Json = nlohmann::json::parse(bufferStr.substr(MSG_HEADER_LEN));
		}
		return true;
	}
	else
	{
		//qWarning() << "invalid message type";
		return false;
	}
}

eOmniverseLiveChannelMessageType OmniverseLiveChannelMessage::GetType() const
{
	return StringTypeToMessageType(GetType_Text());
}

std::string OmniverseLiveChannelMessage::GetType_Text() const
{
	std::string data =  m_p->Json[MSG_TYPE_KEY];
	qInfo() << "msg type key : " << data.c_str();
	if (m_p->Json[MSG_TYPE_KEY] == MESSAGE_MSG_TYPE)
	{
		nlohmann::json customContent = m_p->Json[MSG_CONTENT_KEY];
		nlohmann::json sessionMsg = customContent[CUSTOM_MSG_GROUP_KEY];
		return sessionMsg[CUSTOM_MSG_TYPE].get<std::string>();
	}

	std::string type = m_p->Json.value<std::string>(MSG_TYPE_KEY, "");
	return type;
}

std::string OmniverseLiveChannelMessage::GetID() const
{
	return m_p->FromUser;
}

std::string OmniverseLiveChannelMessage::GetUserName() const
{
	auto it = m_p->Json.find(MSG_USERNAME_KEY);
	if (it == m_p->Json.end())
	{
		return "";
	}
	return (*it);
}

std::string OmniverseLiveChannelMessage::GetAppName() const
{
	auto it = m_p->Json.find(MSG_APP_KEY);
	if (it == m_p->Json.end())
	{
		return "";
	}
	return (*it);
}

OmniClientContent OmniverseLiveChannelMessage::GetOmniClientContent()
{
	std::string msg = MSG_HEADER + m_p->Json.dump();
	OmniClientContent content = omniClientAllocContent(msg.length());
	std::memcpy(content.buffer, msg.c_str(), msg.length());
	return content;
}

