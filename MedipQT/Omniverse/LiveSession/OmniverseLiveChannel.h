#pragma once

#include <vector>
#include <functional>
#include <OmniClient.h>
#include <memory>
#include <thread>
#include <mutex>
#include <future>
#include <set>
#include "Omniverse/LiveSession/OmniverseLiveChannelMessage.h"
#include "Omniverse/LiveSession/OmniversePeerUser.h"

class OmniverseLiveChannel;
class OmniverseLiveSession;

using OmniverseChannelReceiveMessageCallBack = void (*)(OmniverseLiveChannel*, OmniverseLiveChannelMessagePtr, void*);

class OmniverseChannelReceiveMessageCallBackInfo
{
public:
	OmniverseChannelReceiveMessageCallBackInfo() :
		pCallBack(nullptr),
		pUserData(nullptr)
	{
	}

	OmniverseChannelReceiveMessageCallBackInfo(OmniverseChannelReceiveMessageCallBack _pCallBack, void* _pUserData) :
		pCallBack(_pCallBack),
		pUserData(_pUserData)
	{
	}
public:
	OmniverseChannelReceiveMessageCallBack pCallBack;
	void* pUserData;
};

class OmniverseLiveChannelPrivate;

class OmniverseLiveChannel : public QObject
{
public:
	Q_OBJECT

public:
	OmniverseLiveChannel();
	virtual ~OmniverseLiveChannel();

public:
	bool AddReceiveMessageHandler(OmniverseChannelReceiveMessageCallBack pCallBack, void* pUserData = nullptr);
	bool OpenChannel(const OmniverseLiveSession& liveSession, const std::string& sessionName, const std::string& appName);
	bool CloseChannel_And_Wait(int waitTime_ms = 30000);
	bool IsChannelOpen() const;

	int GetChannelID() const;

	int SendChannelMessage(eOmniverseLiveChannelMessageType type);

	void PushQueueEvent(OmniverseLiveChannelMessagePtr pMessage);
	bool TryPopQueueEvent(OmniverseLiveChannelMessagePtr* ppOut);

	std::set<OmniversePeerUser> GetUserList() const;
	std::string GetSessionName() const;

private:
	void runMessageReceiveLoop();

private:
	std::shared_ptr<OmniverseLiveChannelPrivate> m_p;
};
