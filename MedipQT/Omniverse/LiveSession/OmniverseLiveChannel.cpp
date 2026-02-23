#include "stdafx.h"
#include "OmniverseLiveChannel.h"
#include "Omniverse/OmniverseHeader.h"
#include "Omniverse/LiveSession/OmniverseLiveSession.h"

class OmniverseLiveChannelPrivate
{
public:
	std::vector<OmniverseChannelReceiveMessageCallBackInfo> m_messageHandlerCallBackInfoList;
	std::future<void> m_future;
	std::mutex m_mutex;
	bool m_isReceiveLoopRun;
	std::queue<OmniverseLiveChannelMessagePtr> m_messageQueue;
	OmniClientRequestId m_requestId;
	std::string m_channelUrl;
	std::string m_userName;
	std::string m_appName;
	std::string m_currentSessionName;
	std::set<OmniversePeerUser> m_peerUserList;
};

OmniverseLiveChannel::OmniverseLiveChannel()
{
	m_p = std::make_shared<OmniverseLiveChannelPrivate>();
	m_p->m_requestId = 0;
	m_p->m_isReceiveLoopRun = false;
}

OmniverseLiveChannel::~OmniverseLiveChannel()
{
	CloseChannel_And_Wait();
}

bool OmniverseLiveChannel::AddReceiveMessageHandler(OmniverseChannelReceiveMessageCallBack pCallBack, void* pUserData)
{
	if (pCallBack == nullptr)
	{
		return false;
	}

	m_p->m_messageHandlerCallBackInfoList.push_back(OmniverseChannelReceiveMessageCallBackInfo(pCallBack, pUserData));
	return true;
}

bool OmniverseLiveChannel::OpenChannel(const OmniverseLiveSession& liveSession, const std::string& sessionName, const std::string& appName)
{
	if (liveSession.IsLiveSessionFileExist(sessionName) == false)
	{
		return false;
	}

	m_p->m_channelUrl = liveSession.GetMessageChannelUrl(sessionName);
	m_p->m_userName = liveSession.GetUserName();
	m_p->m_appName = appName;
	m_p->m_currentSessionName = sessionName;

	if (m_p->m_isReceiveLoopRun)
	{
		qWarning() << "already join channel. leave channel first";
		return false;
	}

	m_p->m_isReceiveLoopRun = true;
	m_p->m_future = std::async(std::launch::async, &OmniverseLiveChannel::runMessageReceiveLoop, this);

	auto pChannelMessageCallback =
		[](void* userData, OmniClientResult result, OmniClientChannelEvent eventType, char const* fromUser, struct OmniClientContent* content) noexcept
	{
		std::string fromUserStdStr;
		if (fromUser)
		{
			fromUserStdStr = fromUser;
		}

		OmniverseLiveChannel* pChannel = (OmniverseLiveChannel*)userData;
		OmniverseLiveChannelMessagePtr pMessage = OmniverseLiveChannelMessage::CreateRecvMessage(eventType, fromUserStdStr, content);
		if (pMessage)
		{
			pChannel->PushQueueEvent(pMessage);
		}
	};

	m_p->m_requestId = omniClientJoinChannel(m_p->m_channelUrl.c_str(), this, pChannelMessageCallback);
	omniClientWait(m_p->m_requestId);

	SendChannelMessage(eOmniverseLiveChannelMessageType::Join);

	return true;
}

bool OmniverseLiveChannel::CloseChannel_And_Wait(int waitTime_ms)
{
	if (IsChannelOpen() == false)
	{
		return false;
	}

	SendChannelMessage(eOmniverseLiveChannelMessageType::Left);

	m_p->m_isReceiveLoopRun = false;
	m_p->m_future.wait_for(std::chrono::milliseconds(waitTime_ms));

	omniClientStop(m_p->m_requestId);
	m_p->m_requestId = kInvalidRequestId;

	m_p->m_peerUserList.clear();
	m_p->m_currentSessionName = "";

	return true;
}

bool OmniverseLiveChannel::IsChannelOpen() const
{
	return m_p->m_requestId != kInvalidRequestId;
}

int OmniverseLiveChannel::GetChannelID() const
{
	return m_p->m_requestId;
}

int OmniverseLiveChannel::SendChannelMessage(eOmniverseLiveChannelMessageType type)
{
	OmniverseLiveChannelMessagePtr msg = OmniverseLiveChannelMessage::CreateSendMessage(type, m_p->m_userName.c_str(), m_p->m_appName.c_str());
	OmniClientContent content = msg->GetOmniClientContent();
	OmniClientRequestId sendID= omniClientSendMessage(m_p->m_requestId, &content, nullptr, [](void* userData, OmniClientResult result) noexcept {});
	omniClientWait(sendID);

	return sendID;
}

void OmniverseLiveChannel::PushQueueEvent(OmniverseLiveChannelMessagePtr pMessage)
{
	std::unique_lock<std::mutex> lock(m_p->m_mutex);
	m_p->m_messageQueue.push(pMessage);
}

bool OmniverseLiveChannel::TryPopQueueEvent(OmniverseLiveChannelMessagePtr* ppOut)
{
	std::unique_lock<std::mutex> lock(m_p->m_mutex);

	if (m_p->m_messageQueue.empty())
	{
		return false;
	}

	*ppOut = m_p->m_messageQueue.front();
	m_p->m_messageQueue.pop();

	return true;
}

std::set<OmniversePeerUser> OmniverseLiveChannel::GetUserList() const
{
	return m_p->m_peerUserList;
}

std::string OmniverseLiveChannel::GetSessionName() const
{
	return m_p->m_currentSessionName;
}

void OmniverseLiveChannel::runMessageReceiveLoop()
{
	int waitTime_ms = 300;
	qInfo() << "run message receive loop";

	while (m_p->m_isReceiveLoopRun)
	{
		OmniverseLiveChannelMessagePtr pMessage = nullptr;
		if (TryPopQueueEvent(&pMessage))
		{
			bool sendHello = false;

			if (eOmniverseLiveChannelMessageType::GetUsers == pMessage->GetType() ||
				eOmniverseLiveChannelMessageType::Join == pMessage->GetType())
			{
				sendHello = true;
			}

			if (eOmniverseLiveChannelMessageType::Hello == pMessage->GetType() ||
				eOmniverseLiveChannelMessageType::Join == pMessage->GetType())
			{
				OmniversePeerUser newUser(pMessage->GetID(), pMessage->GetUserName(), pMessage->GetAppName());
				m_p->m_peerUserList.insert(newUser);
			}
			else if (eOmniverseLiveChannelMessageType::Left == pMessage->GetType())
			{
				OmniversePeerUser leaveUser(pMessage->GetID(), pMessage->GetUserName(), pMessage->GetAppName());
				m_p->m_peerUserList.erase(leaveUser);
			}


			if (sendHello)
			{
				SendChannelMessage(eOmniverseLiveChannelMessageType::Hello);
			}

			for (auto& pInfo : m_p->m_messageHandlerCallBackInfoList)
			{
				pInfo.pCallBack(this, pMessage, pInfo.pUserData);
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(waitTime_ms));
	}

	qInfo() << "leave message receive loop";
}

