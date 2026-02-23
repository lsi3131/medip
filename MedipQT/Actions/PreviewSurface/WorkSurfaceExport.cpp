#include "stdafx.h"
#include "WorkSurfaceExport.h"
#include "windowManager.h"
#include "ActionManager.h"
#include "MeshEdit/CMeshModelViewManager.h"

#include <ppl.h>

#define  USE_PPL

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WorkSurfaceExport Class Member Functions - Start
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
WorkSurfaceExport::WorkSurfaceExport(
	QVector<int>& vecExportList,
	DataContext* pDataContext,
	EXPORT_3D_SURFACE_MESH_METHOD		method,
	bool			bExtract,
	bool			bPatientCoordinate
)
	: VolumeTo3DSurface(&pDataContext->volume_data, method)
{
	m_vecExportList = vecExportList;

	m_bPatientCoordinate = bPatientCoordinate;

	m_bExtract = bExtract;

	m_pDataContext = pDataContext;
}

WorkSurfaceExport::~WorkSurfaceExport()
{

}

void WorkSurfaceExport::singleProcess()
{
	if (m_vecExportList.empty() || !m_pVolumData)
	{
		emit finished();
		return;
	}

	int	nExport = (int)m_vecExportList.size();
	int	interval = 90 / nExport;
	int	progress_val_total = 0;

	std::vector<mint32>					vecLayerUid(nExport, -1);
	std::vector<mip::MeshTopology*>		vecMeshTopology(nExport, NULL);
	std::vector<mip::MeshTopology*>		vecpMesh(nExport, NULL);
	std::vector<QString>				vecFileName(nExport);

	for (int i = 0; i < nExport; ++i)
	{
		MaskInfo* pMaskInfo = m_pVolumData->getMaskInfo(m_vecExportList[i]);

		if (!pMaskInfo)
		{
			continue;
		}

		mint32			layer_uid = pMaskInfo->uid;

		int				maskIdx = pMaskInfo->uid >= MASK_SECOND_MAX ? (pMaskInfo->uid - MASK_SECOND_MAX) / 8 + 1 : 0;

		mask			maskData = ((maskIdx == 0) ? pMaskInfo->mask_id : pMaskInfo->mask_id2);


		QString		file_name = QString::fromStdWString(pMaskInfo->maskName);
		QString		progress_name;

		int			progress_val = 0;
		progress_name = file_name + QString().sprintf("(%d%)", progress_val);

		progress_val_total = i * interval;

		emit sig_progress(progress_val_total, progress_name);

		vecMeshTopology[i] = new mip::MeshTopology(g_Renderer);
		Create3DSurface(vecMeshTopology[i], maskData, maskIdx);

		progress_val_total += interval / 3;
		progress_val += 100 / 6;

		progress_name = file_name + QString().sprintf("(%d%)", progress_val);
		emit sig_progress(progress_val_total, progress_name);

		vecMeshTopology[i]->buildTree();

		progress_val_total += interval / 3;
		progress_val += 100 / 6;

		progress_name = file_name + QString().sprintf("(%d%)", progress_val);
		emit sig_progress(progress_val_total, progress_name);

		// 201102 허 건 대리 수정
		// ACTION_MANAGER->action_MeshList_add 에서 undo redo 분리 위한 내부소스코드
		{
			// create mesh info
			m_pDataContext->m_MeshData.CreateMeshInfo();

			muint32 uid = m_pDataContext->m_MeshData.GetCurrentMeshIndex();

			m_pDataContext->m_MeshData.SetMeshName(file_name, uid);

			MeshInfo* pInfo = m_pDataContext->m_MeshData.GetMeshInfo(uid);

			if (layer_uid != -1)
			{
				QColor col = m_pVolumData->getMaskColor(layer_uid, true);

				pInfo->color = COLOR(col.red(), col.green(), col.blue());
			}

			if (pInfo)
			{
				pInfo->upScale = false;

				pInfo->uid = uid;
			}

			// push pck true 
			//WIN_MANAGER->vt_pckID.push_back(true);
			m_pDataContext->m_MeshData.SetMeshInfoModeSelectMode(uid, true);

			// insert mesh
			m_pDataContext->m_MeshData.InsertMesh(uid, vecMeshTopology[i]);

			vecLayerUid[i] = layer_uid;
			vecFileName[i] = file_name;

			mip::MeshTopology* pMesh = m_pDataContext->m_MeshData.GetMesh(uid);
			if (pMesh)
			{
				vecMeshTopology[i] = pMesh;
			}

			// update mesh-list
			emit sig_updateUI();

			WIN_MANAGER->setSaveState(false);
		}

		progress_val_total = interval * (i + 1);
		progress_val = 100;

		progress_name = file_name + QString().sprintf("(%d%)", progress_val);
		emit sig_progress(progress_val_total, progress_name);
	}

	// push action undo / Redo
	emit sig_progress(90, QString().sprintf("Final Process.."));
	ACTION_MANAGER->action_MeshList_add_Multi(m_pDataContext, vecLayerUid, vecFileName, vecMeshTopology);
	emit sig_progress(100, QString().sprintf("Final Process.."));

	emit finished();
}

void WorkSurfaceExport::parallelProcess()
{
	if (m_vecExportList.empty() || !m_pVolumData)
	{
		emit finished();
		return;
	}

	int	nExport = (int)m_vecExportList.size();

	std::vector<mint32>					vecLayerUid(nExport, -1);
	std::vector<mip::MeshTopology*>		vecMeshTopology(nExport, NULL);
	std::vector<QString>				vecFileName(nExport);

	concurrency::combinable<float>		val_progress;

	float								interval = 90.f / (float)nExport;

	val_progress.local() = 0;

	for (int i = 0; i < nExport; ++i)
	{
		vecMeshTopology[i] = new mip::MeshTopology(g_Renderer);
	}

	for (int i = 0; i < nExport; ++i)
	{
		MaskInfo* pMaskInfo = m_pVolumData->getMaskInfo(m_vecExportList[i]);

		float tmp_progress = 0;

		val_progress.local() += interval * 0.1f;
		tmp_progress = val_progress.combine(plus<float>());
		emit sig_progress((int)tmp_progress, "Generate Mesh...");

		if (pMaskInfo && vecMeshTopology[i])
		{
			mint32			layer_uid = pMaskInfo->uid;

			int				maskIdx = pMaskInfo->uid >= MASK_SECOND_MAX ? (pMaskInfo->uid - MASK_SECOND_MAX) / 8 + 1 : 0;

			mask			maskData = ((maskIdx == 0) ? pMaskInfo->mask_id : pMaskInfo->mask_id2);


			Create3DSurface(vecMeshTopology[i], maskData, maskIdx);

			val_progress.local() += interval * 0.4f;
			tmp_progress = val_progress.combine(plus<float>());
			emit sig_progress((int)tmp_progress, "Generate Mesh...");

			vecMeshTopology[i]->buildTree();
		}

		val_progress.local() += interval * 0.5f;
		tmp_progress = val_progress.combine(plus<float>());
		emit sig_progress((int)tmp_progress, "Generate Mesh...");
	}

	interval = 10.f / (float)nExport;
	float tmp_progress = 0.f;
	for (int i = 0; i < nExport; ++i)
	{
		MaskInfo* pMaskInfo = m_pVolumData->getMaskInfo(m_vecExportList[i]);

		if (!pMaskInfo)
		{
			continue;
		}

		mint32			layer_uid = pMaskInfo->uid;

		QString			file_name = QString::fromStdWString(pMaskInfo->maskName);

		// create mesh info
		m_pDataContext->m_MeshData.CreateMeshInfo();

		muint32		uid = m_pDataContext->m_MeshData.GetCurrentMeshIndex();

		m_pDataContext->m_MeshData.SetMeshName(file_name, uid);

		MeshInfo* info = m_pDataContext->m_MeshData.GetMeshInfo(uid);

		if (!info || !vecMeshTopology[i])
		{
			return;
		}

		if (layer_uid != -1)
		{
			QColor col = m_pVolumData->getMaskColor(layer_uid, true);

			info->color = COLOR(col.red(), col.green(), col.blue());

			vecMeshTopology[i]->m_baseColor = mip::VECTOR4(col.red() / 255.f, col.green() / 255.f, col.blue() / 255.f, 1.f);
		}

		vecMeshTopology[i]->updateColor(vecMeshTopology[i]->m_baseColor);

		info->upScale = false;

		info->uid = uid;


		// push pck true 
		//WIN_MANAGER->vt_pckID.push_back(false);
		m_pDataContext->m_MeshData.SetMeshInfoModeSelectMode(uid, false);

		// insert mesh
		m_pDataContext->m_MeshData.InsertMesh(uid, vecMeshTopology[i]);

		//m_pVolumData->MeshRenderUpdate(uid);

		vecLayerUid[i] = layer_uid;
		vecFileName[i] = file_name;

		mip::MeshTopology* pMesh = m_pDataContext->m_MeshData.GetMesh(uid);
		if (pMesh)
		{
			vecMeshTopology[i] = pMesh;
		}

		// update mesh-list
		emit sig_updateUI();

		WIN_MANAGER->setSaveState(false);

		val_progress.local() += interval;
		tmp_progress = val_progress.combine(plus<float>());
		emit sig_progress((int)tmp_progress, "Generate Mesh...");
	}

	ACTION_MANAGER->action_MeshList_add_Multi(m_pDataContext, vecLayerUid, vecFileName, vecMeshTopology);

	int nMesh = m_pDataContext->m_MeshData.GetMeshCount();

	//WIN_MANAGER->vt_pckID.resize(nMesh, false);

	//for (int i = 0; i < nMesh; ++i)
	//{
	//	WIN_MANAGER->vt_pckID[i] = false;
	//}

	//int pck_idx = WIN_MANAGER->vt_pckID.size() - 1;
	//WIN_MANAGER->vt_pckID[pck_idx] = true;

	int pck_idx = nMesh - 1;
	m_pDataContext->m_MeshData.ClearMeshInfoSelectMode();
	m_pDataContext->m_MeshData.SetMeshInfoModeSelectMode(pck_idx, true);

	m_pDataContext->m_MeshData.SetCurrentMeshIndex(pck_idx);

	MESH_MODELVIEW_MANAGER->UpdatePivotPoint(pck_idx);

	emit sig_progress(100, "Generate Mesh...");

	emit finished();
}

void WorkSurfaceExport::threadRun()
{
#ifdef USE_PPL
	parallelProcess();
#else
	singleProcess();
#endif
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WorkSurfaceExport Class Member Functions - End
//////////////////////////////////////////////////////////////////////////////////////////////////////////////