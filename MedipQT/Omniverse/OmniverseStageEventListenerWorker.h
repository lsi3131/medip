#pragma once

#include <thread>
#include <memory>
#include <QObject>

class OmniverseContext;
class OmniverseStageEventListenerWorkerPrivate;

enum class eOmniverseEventType
{
	GlobalLayerReload,
	RootOrSubLayerChange,
	ObjectsChanged,
	ClientStatCallback,
	ClientStatSubscribeCallback,
};

using OmniverseStageCallBackFunction = std::function<void(eOmniverseEventType, void*)>;

class OmniverseStageEventListenerWorker : public QObject
{
	Q_OBJECT

public:
	OmniverseStageEventListenerWorker();

public:
	bool Start();
	bool Stop();
	std::thread* GetWorkerThread();

	bool Register(OmniverseContext* pContext);
	bool UnRegister();

	void Subscribe_Event(OmniverseStageCallBackFunction callback, void* userData);

public:
	

private:
	void doWorkLoop();

private:
	std::shared_ptr<OmniverseStageEventListenerWorkerPrivate> m_p;
};
