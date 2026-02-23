#pragma once

#include <memory>
#include <thread>
#include <QObject>

class OmniverseStageReaderWorkerPrivate;
class OmniverseStage;
class MeshData;

class OmniverseStageReaderWorker : public QObject
{
    Q_OBJECT
public:
    OmniverseStageReaderWorker(OmniverseStage* pStage, MeshData* pMeshData);

public:
    void DoWork();
    bool Start();
    std::thread* Stop();

Q_SIGNALS:
    void sig_liveDataChanged();

private:
    std::shared_ptr<OmniverseStageReaderWorkerPrivate> m_p;
};
