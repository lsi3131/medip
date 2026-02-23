#include "stdafx.h"
#include "WorkBrushThread.h"
#include "ActionManager.h"
#include "MeshControl.h"
#include "MeshEdit/CMeshWorkManager.h"

WorkBrushThread::WorkBrushThread(DataContext* pDataContext)
	: _mesh(nullptr),
	_position(mip::VECTOR3(0, 0, 0)),
	m_pDataContext(pDataContext)
{

}

WorkBrushThread::WorkBrushThread(DataContext* pDataContext, mip::MeshTopology* pMesh, mip::VECTOR3 pos)
	: _mesh(pMesh),
	_position(pos),
	m_pDataContext(pDataContext)
{

}

void WorkBrushThread::setProgressValue(int value, bool init /*= false*/)
{
	static int val = 0;

	if (init)
	{
		val = 0;
		emit progress(val);
		return;
	}

	if (100 <= val)
		return;

	if ((val + 10) <= value)
	{
		val = value;
		emit progress(val);
	}
}

void WorkBrushThread::threadRun()
{
	if (_mesh)
	{
		emit sig_updateProgress(30, "preprocessing..");

		DWORD start, end;
		double time, tot_time = 0.;
		start = ::GetTickCount();

		//_mesh->m_tverts.clear();
		//_mesh->m_ttris.clear();
		//_mesh->m_tVHedges.clear();

		//_mesh->setTopologyed(false);
		//_mesh->mergingVertex();
		//_mesh->buildTopologyHEdge();

		mip::mesh_control::createMatchingData(_mesh);

		end = ::GetTickCount();
		time = (double)(end - start) / 1000.;
		printf_s(" process createMatchingData : %lf\n", time);

		//mip::mesh_control::updateDisplay(_mesh);
	}

	m_pDataContext->volume_data.threadStop = true;

	MESH_WORK_MODE mode = MESH_WORK_MANAGER->getWorkMode();
	ACTION_MANAGER->action_UndoRedo_update(m_pDataContext, MESH_WORK_BRUSH_SCULPT, mode);
	//MESH_WORK_MANAGER->setWorkMode(MESH_WORK_BRUSH_SCULPT);

	emit sig_updateProgress(90, "preprocessing..");
	emit finished();

}