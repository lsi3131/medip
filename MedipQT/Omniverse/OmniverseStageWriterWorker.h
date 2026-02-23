#pragma once

#include <memory>
#include <thread>

class OmniverseStageWriterWorkerPrivate;
class OmniverseStage;
class MeshData;

enum class eWriterWorkerID
{
    None = 0,
    Changed,
};

class OmniverseStageWriterWorker : public QObject
{
    Q_OBJECT
public:
    OmniverseStageWriterWorker(OmniverseStage* pContext, MeshData* pVolumeData);

public:
    bool Start();
    std::thread* Stop();

    void PushQueueEvent(eWriterWorkerID id);

private:
    void doWork();

Q_SIGNALS:
    void sig_liveDataChanged();
    void sig_liveDataUpdate();

private:
    std::shared_ptr<OmniverseStageWriterWorkerPrivate> m_p;
};
