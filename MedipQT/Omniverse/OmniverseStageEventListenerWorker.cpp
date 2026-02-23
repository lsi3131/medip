#include "stdafx.h"
#include "OmniverseStageEventListenerWorker.h"
#include "OmniverseContext.h"

#include <OmniClient.h>
#include <pxr/usd/usd/notice.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/primRange.h>
#include <pxr/usd/usdGeom/metrics.h>

using namespace pxr;

struct OmniverseStageFunctionContext
{
    OmniverseStageCallBackFunction Callback;
    void* UserData;

    OmniverseStageFunctionContext(OmniverseStageCallBackFunction callback, void* userData)
    {
        Callback = callback;
        UserData = userData;
    }
};

/*
    OmniverseStageEventListenerWorkerPrivate
*/
class OmniverseStageEventListenerWorkerPrivate : public TfWeakBase
{
public:
    static void OnClientStatCallback(void* userData, OmniClientResult result, struct OmniClientListEntry const* entry) noexcept;
    static void OnClientStatSubscribeCallback(void* userData, OmniClientResult result, OmniClientListEvent listEvent, struct OmniClientListEntry const* entry) noexcept;

public:
    OmniverseStageEventListenerWorkerPrivate(OmniverseStageEventListenerWorker* d);

public:
    void HandleGlobalLayerReload(const SdfNotice::LayerDidReloadContent& content);
    void HandleRootOrSubLayerChange(const class SdfNotice::LayersDidChangeSentPerLayer& layerNotice, const TfWeakPtr<SdfLayer>& sender);
    void HandleObjectsChanged(const class pxr::UsdNotice::ObjectsChanged& ObjectsChanged);

    void NotifyRegisteredCallBack(eOmniverseEventType type);

public:
	OmniverseContext* Context = nullptr;
    OmniverseStageEventListenerWorker* D = nullptr;
    TfNotice::Key LayerReloadKey;
    TfNotice::Key LayerChangeKey;
    TfNotice::Key LayerObjectNoticeKey;
    OmniClientRequestId StatSubscribeRequestId = 0;
    std::shared_ptr<std::thread> WorkerThread;
    bool WorkerThreadStopped = false;
    std::vector<OmniverseStageFunctionContext> CallBackFunctionList;
};


void OmniverseStageEventListenerWorkerPrivate::OnClientStatCallback(void* userData, OmniClientResult result, struct OmniClientListEntry const* entry) noexcept
{
    qInfo() << "OnClientStatCallback. ";
    OmniverseStageEventListenerWorkerPrivate* client = static_cast<OmniverseStageEventListenerWorkerPrivate*>(userData);
    client->NotifyRegisteredCallBack(eOmniverseEventType::ClientStatCallback);

    if (result != OmniClientResult::eOmniClientResult_Ok)
    {
        //qInfo() << "Error: stage not found: " << client->StageUrl.c_str();
    }
}

void OmniverseStageEventListenerWorkerPrivate::OnClientStatSubscribeCallback(void* userData, OmniClientResult result, OmniClientListEvent listEvent, struct OmniClientListEntry const* entry) noexcept
{
    qInfo() << "OnClientStatSubscribeCallback Subscribe : " << listEvent;
    OmniverseStageEventListenerWorkerPrivate* client = static_cast<OmniverseStageEventListenerWorkerPrivate*>(userData);
    client->NotifyRegisteredCallBack(eOmniverseEventType::ClientStatSubscribeCallback);

    switch (listEvent)
    {
    case eOmniClientListEvent_Updated:
    {
        qInfo() << "Updated - user: " << entry->modifiedBy << " version: " << entry->version;
        break;
    }
    case eOmniClientListEvent_Created:
        qInfo() << "Created: " << entry->createdBy;
        break;
    case eOmniClientListEvent_Deleted:
        qInfo() << "Deleted: " << entry->createdBy;
        break;
    case eOmniClientListEvent_Locked:
        qInfo() << "Locked: " << entry->createdBy;
        break;
    default:
        break;
    }
}

OmniverseStageEventListenerWorkerPrivate::OmniverseStageEventListenerWorkerPrivate(OmniverseStageEventListenerWorker* d) :
    D(d)
{
}

void OmniverseStageEventListenerWorkerPrivate::HandleGlobalLayerReload(const SdfNotice::LayerDidReloadContent& content)
{
    qInfo() << "HandleGlobalLayerReload called";
    NotifyRegisteredCallBack(eOmniverseEventType::GlobalLayerReload);
}

void OmniverseStageEventListenerWorkerPrivate::HandleRootOrSubLayerChange(const class SdfNotice::LayersDidChangeSentPerLayer& layerNotice, const TfWeakPtr<SdfLayer>& sender)
{
    qInfo() << "Handle Root Sub Layer Change";

    NotifyRegisteredCallBack(eOmniverseEventType::RootOrSubLayerChange);

    auto iter = layerNotice.find(sender);
    SdfChangeList sdfChangeList = iter->second;

    for (auto& changeEntry : sdfChangeList.GetEntryList())
    {
        SdfPath path = changeEntry.first;
        SdfChangeList::Entry entry = changeEntry.second;
        SdfChangeList::Entry::_Flags flags = entry.flags;

        QString message = QString("ChangeEntry. path : %1")
            .arg(path.GetText());

        for (auto info : entry.infoChanged)
        {
            SdfChangeList::Entry::InfoChange infoChange = info.second;
            VtValue infoChangeDataKey = infoChange.first;
            VtValue infoChangeData = infoChange.second;

            //if (infoChangeData.IsArrayValued() && infoChangeData.GetArraySize() > 4)
            //{
            //    qInfo() << " : " << infoChangeData.GetTypeName() << "[" << infoChangeData.GetArraySize() << "]";
            //}
            //else
            //{
            //    qInfo() << " : " << infoChangeDataKey;
            //    qInfo() << " -> " << infoChangeData;
            //}
        }
        qInfo() << "message : " << message;
    }
}

void OmniverseStageEventListenerWorkerPrivate::HandleObjectsChanged(const class pxr::UsdNotice::ObjectsChanged& ObjectsChanged)
{
    qInfo() << "Handle Event. updated";

    NotifyRegisteredCallBack(eOmniverseEventType::ObjectsChanged);
    
    std::vector<SdfFastUpdateList::FastUpdate> updates = ObjectsChanged.GetFastUpdates();

    UsdNotice::ObjectsChanged::PathRange pathRange = ObjectsChanged.GetResyncedPaths();
    for (auto p : pathRange)
    {
        SdfPath primPath = p.GetPrimPath();
        //std::string name = p.GetName();
        qInfo() << "==== Handle Event name : " << primPath.GetText();
    }
}

void OmniverseStageEventListenerWorkerPrivate::NotifyRegisteredCallBack(eOmniverseEventType type)
{
    for (auto& context : CallBackFunctionList)
    {
        context.Callback(type, context.UserData);
    }
}

/* ===================================
    OmniverseStageEventListenerWorker
=====================================*/
OmniverseStageEventListenerWorker::OmniverseStageEventListenerWorker()
{
    m_p = std::make_shared<OmniverseStageEventListenerWorkerPrivate>(this);
}

void OmniverseStageEventListenerWorker::Subscribe_Event(OmniverseStageCallBackFunction callback, void* userData)
{
    m_p->CallBackFunctionList.push_back(OmniverseStageFunctionContext(callback, userData));
}

bool OmniverseStageEventListenerWorker::Start()
{
    if (m_p->Context == nullptr)
    {
        return false;
    }

    m_p->WorkerThread = std::make_shared<std::thread>(&OmniverseStageEventListenerWorker::doWorkLoop, this);

    return true;
}

bool OmniverseStageEventListenerWorker::Stop()
{
    m_p->WorkerThreadStopped = true;

    return true;
}

std::thread* OmniverseStageEventListenerWorker::GetWorkerThread()
{
    return m_p->WorkerThread.get();
}

bool OmniverseStageEventListenerWorker::Register(OmniverseContext* pContext)
{
    if (pContext->IsConnect() == false)
    {
        return false;
    }
	m_p->Context = pContext;
    OmniverseStage* pStage = m_p->Context->GetStage();

    m_p->LayerReloadKey = TfNotice::Register(TfCreateWeakPtr(m_p.get()), &OmniverseStageEventListenerWorkerPrivate::HandleGlobalLayerReload);
    m_p->LayerChangeKey = TfNotice::Register(TfCreateWeakPtr(m_p.get()), &OmniverseStageEventListenerWorkerPrivate::HandleRootOrSubLayerChange, m_p->Context->GetUsdStage()->GetData()->GetRootLayer());
    m_p->LayerObjectNoticeKey = TfNotice::Register(TfCreateWeakPtr(m_p.get()), &OmniverseStageEventListenerWorkerPrivate::HandleObjectsChanged);
    
    std::string stageUrl = pStage->GetStageUrl();

    qInfo() << "register event listener : " << stageUrl.c_str();

    m_p->StatSubscribeRequestId = omniClientStatSubscribe(
        stageUrl.c_str(),
        m_p.get(),
        OmniverseStageEventListenerWorkerPrivate::OnClientStatCallback,
        OmniverseStageEventListenerWorkerPrivate::OnClientStatSubscribeCallback
    );

	return true;
}

bool OmniverseStageEventListenerWorker::UnRegister()
{
    qInfo() << "unregister event listener";
    omniClientStop(m_p->StatSubscribeRequestId);
    pxr::TfNotice::Revoke(m_p->LayerReloadKey);
    pxr::TfNotice::Revoke(m_p->LayerChangeKey);
    pxr::TfNotice::Revoke(m_p->LayerObjectNoticeKey);

    return true;
}

void OmniverseStageEventListenerWorker::doWorkLoop()
{
    qInfo() << "event listener worker loop start";
    m_p->WorkerThreadStopped = false;
    std::time_t currentTime = std::time(0);
    while (!m_p->WorkerThreadStopped)
    {
        using namespace std::chrono_literals;
        double randomn = std::rand();

        // Setting a frequency of 300ms as a starting point for updates
        std::this_thread::sleep_for(300ms);
    }
    qInfo() << "event listener worker loop finish";
}

