#pragma once

#include <QObject>
#include "Omniverse/OmniverseStageEventListenerWorker.h"

class TestOmniverseStageEventListenerWorkerHandlerStub : public QObject
{
	Q_OBJECT

public:
	static void OnEventListen(eOmniverseEventType type, void* userData)
	{
		TestOmniverseStageEventListenerWorkerHandlerStub* d = (TestOmniverseStageEventListenerWorkerHandlerStub*)userData;
		switch (type)
		{
		case eOmniverseEventType::GlobalLayerReload:
			d->GlobalLayerReload_Count++;
			break;
		case eOmniverseEventType::RootOrSubLayerChange:
			d->RootOrSubLayerChange_Count++;
			break;
		case eOmniverseEventType::ObjectsChanged:
			d->ObjectsChanged_Count++;
			break;
		case eOmniverseEventType::ClientStatCallback:
			d->ClientStatCallback_Count++;
			break;
		case eOmniverseEventType::ClientStatSubscribeCallback:
			d->ClientStatSubscribeCallback_Count++;
			break;
		default:
			Q_ASSERT(false);
			break;
		}
	}

public:
	TestOmniverseStageEventListenerWorkerHandlerStub()
	{
	}

	bool ConnectToSlot(OmniverseStageEventListenerWorker* pWorker)
	{
		pWorker->Subscribe_Event(OnEventListen, this);
		return true;
	}

public slots:
	void slot_HandleGlobalLayerReload()
	{
		GlobalLayerReload_Count++;
	}

	void slot_HandleRootOrSubLayerChange()
	{
		RootOrSubLayerChange_Count++;
	}

	void slot_HandleObjectsChanged()
	{
		ObjectsChanged_Count++;
	}

	void slot_ClientStatCallback()
	{
		ClientStatCallback_Count++;
	}

	void slot_ClientStatSubscribeCallback()
	{
		ClientStatSubscribeCallback_Count++;
	}

public:
	int GlobalLayerReload_Count = 0;
	int RootOrSubLayerChange_Count = 0;
	int ObjectsChanged_Count = 0;
	int ClientStatCallback_Count = 0;
	int ClientStatSubscribeCallback_Count = 0;

};
