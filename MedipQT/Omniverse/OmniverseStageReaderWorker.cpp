#include "stdafx.h"
#include "OmniverseStageReaderWorker.h"
#include "OmniverseStage.h"
#include <chrono>
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

class OmniverseStageReaderWorkerPrivate
{
public:
	static void OnLiveReadCallback(void* userData, OmniClientResult result, uint64_t objectId, uint64_t sequenceNum, struct OmniClientContent* content)
	{
		//qInfo() << "OnLiveReadCallback. result : " << result << ", object id : " << objectId << ", sequence num : " << sequenceNum << "content buffer size : " << content->size;
		OmniverseStageReaderWorkerPrivate* p = (OmniverseStageReaderWorkerPrivate*)userData;
		if (result == eOmniClientResult_Ok || result == eOmniClientResult_OkLatest)
		{
			//if (p->SequenceNumber != sequenceNum)
			{
				qInfo() << "eOmniClientResult_Ok. result : " << result 
					<< ", prev object id : " << p->ObjectID
					<< ", object id : " << objectId 
					<< ", prev sequence num : " << p->SequenceNumber 
					<< ", new sequence num : " << sequenceNum
					<< ", content buffer size : " << content->size;
				p->ObjectID = objectId;
				p->SequenceNumber = sequenceNum;
			}
		}
	}

	static void OnLiveUpdateCallback(void* userData, OmniClientResult result, uint64_t sequenceNum, uint64_t requestId)
	{
		//qInfo() << "OnLiveUpdateCallback. result : " << result << ", requestId  : " << requestId << ", sequenceNum : " << sequenceNum;
	}


public:
	OmniverseStageReaderWorkerPrivate(OmniverseStageReaderWorker* pData) :
		Data(pData),
		Stopped(true), 
		SequenceNumber(0)
	{
	}
public:
	OmniverseStageReaderWorker* Data;
	OmniverseStage* pStage;
	MeshData* pMeshData;
	std::shared_ptr<std::thread> WorkerThread;
	std::atomic<bool> Stopped;
	uint64_t SequenceNumber;
	uint64_t ObjectID;
};

/**
 * OmniverseStageReaderWorker
 */
OmniverseStageReaderWorker::OmniverseStageReaderWorker(OmniverseStage* pStage, MeshData* pMeshData)
{
	m_p = std::make_shared<OmniverseStageReaderWorkerPrivate>(this);
	m_p->pStage = pStage;
	m_p->pMeshData = pMeshData;
}

bool OmniverseStageReaderWorker::Start()
{
	if (m_p->Stopped == false)
	{
		qWarning() << "worker is running";
		return false;
	}

	m_p->WorkerThread = std::make_shared<std::thread>(&OmniverseStageReaderWorker::DoWork, this);

	return true;
}

void OmniverseStageReaderWorker::DoWork()
{
	qInfo() << "reader worker loop start";
	m_p->Stopped = false;
	std::time_t currentTime = std::time(0);
	while (!m_p->Stopped)
	{
		using namespace std::chrono_literals;

		//std::string url = m_p->pStage->GetUrlCurrentStageUsd();
		//OmniClientRequestId id = omniClientLiveRead(url.c_str(), 0, 0, m_p.get(), OmniverseStageReaderWorkerPrivate::OnLiveReadCallback);
		//omniClientWait(id);

		// Setting a frequency of 300ms as a starting point for updates
		std::this_thread::sleep_for(300ms);
		emit sig_liveDataChanged();
	}
	qInfo() << "reader worker loop finish";
}

std::thread* OmniverseStageReaderWorker::Stop()
{
	qInfo() << "reader worker stop called";
	m_p->Stopped = true;
	return m_p->WorkerThread.get();
}
