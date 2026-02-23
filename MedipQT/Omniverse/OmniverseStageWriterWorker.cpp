#include "stdafx.h"
#include "OmniverseStageWriterWorker.h"
#include "OmniverseStage.h"
#include <queue>
#include <mutex>

/* Qt slots과 define 충돌 방지*/
#undef slots

#include <OmniClient.h>

#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/usd/usdGeom/metrics.h>
#include <pxr/base/gf/matrix4f.h>
#include <pxr/base/gf/vec2f.h>
#include <pxr/usd/usdUtils/pipeline.h>
#include <pxr/usd/usdUtils/sparseValueWriter.h>
#include <pxr/usd/usdShade/material.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/primRange.h>
#include <pxr/usd/usdGeom/primvar.h>
#include <pxr/usd/usdShade/input.h>
#include <pxr/usd/usdShade/output.h>
#include <pxr/usd/usdGeom/xform.h>
#include <pxr/usd/usdGeom/cube.h>
#include <pxr/usd/usdShade/materialBindingAPI.h>
#include <pxr/usd/usdLux/distantLight.h>
#include <pxr/usd/usdLux/domeLight.h>
#include <pxr/usd/usdShade/shader.h>
#include <pxr/usd/usd/modelAPI.h>

PXR_NAMESPACE_USING_DIRECTIVE

class OmniverseStageWriterWorkerPrivate
{
public:
	OmniverseStageWriterWorkerPrivate() :
		Stopped(true), variance(1.0f), step(0), runLimit(-1)
	{
	}

public:
	void PushQueueEvent(eWriterWorkerID id)
	{
		//qInfo() << "push queue id : " << (int)id;
		std::unique_lock<std::mutex> lock(m_mutex);
		m_queue.push(id);
	}

	bool TryPopQueueEvent(eWriterWorkerID* pOutID)
	{
		//qInfo() << "try pop queue event";
		std::unique_lock<std::mutex> lock(m_mutex);

		if (m_queue.empty())
		{
			return false;
		}

		*pOutID = m_queue.front();
		m_queue.pop();

		return true;
	}

	bool IsQueueEmpty()
	{
		//qInfo() << "is queue empty";
		std::unique_lock<std::mutex> lock(m_mutex);

		return m_queue.empty();
	}

public:
	OmniverseStage* pStage;
	MeshData* pMeshData;
	std::shared_ptr<std::thread> WorkerThread;
	std::atomic<bool> Stopped;
	int zone;
	float variance;
	int step;
	int runLimit;
private:
	std::queue<eWriterWorkerID> m_queue;
	std::mutex m_mutex;
};

OmniverseStageWriterWorker::OmniverseStageWriterWorker(OmniverseStage* pStage, MeshData* pMeshData)
{
	m_p = std::make_shared<OmniverseStageWriterWorkerPrivate>();
	m_p->pStage = pStage;
	m_p->pMeshData = pMeshData;
}

bool OmniverseStageWriterWorker::Start()
{
	if (m_p->Stopped == false)
	{
		qWarning() << "worker is running";
		return false;
	}

	m_p->WorkerThread = std::make_shared<std::thread>(&OmniverseStageWriterWorker::doWork, this);

	return true;
}

void OmniverseStageWriterWorker::doWork()
{
	qInfo() << "write worker loop start";
	m_p->Stopped = false;
	std::time_t currentTime = std::time(0);
	while (!m_p->Stopped)
	{
		using namespace std::chrono_literals;
		double randomn = std::rand();

		// Setting a frequency of 100ms as a starting point for updates
		std::this_thread::sleep_for(100ms);

		if (m_p->IsQueueEmpty() == false)
		{
			eWriterWorkerID id;
			m_p->TryPopQueueEvent(&id);
			qInfo() << "try to pop queue event and send message. id : " << (int)id;
			if (id == eWriterWorkerID::None)
			{
				/* queue에 데이터가 있으면 그 때만 change를 보내는 식으로 변경 */
				emit sig_liveDataChanged();
			}
			else if (id == eWriterWorkerID::Changed)
			{
				emit sig_liveDataChanged();
			}
		}

		emit sig_liveDataUpdate();
	}
	qInfo() << "worker loop finish";
}

std::thread* OmniverseStageWriterWorker::Stop()
{
	qInfo() << "stop called";
	m_p->Stopped = true;
	return m_p->WorkerThread.get();
}

void OmniverseStageWriterWorker::PushQueueEvent(eWriterWorkerID id)
{
	m_p->PushQueueEvent(id);
}
