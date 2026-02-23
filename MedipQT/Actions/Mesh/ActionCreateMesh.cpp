#include "stdafx.h"
#include "ActionCreateMesh.h"
#include "MedipQT.h"

#include "MeshControl.h"
#include "Windows/windowManager.h"
#include "Windows/Main/MainMeshWidget.h"
#include "Windows/MEVolumeView.h"
#include "Windows/Tabwindow.h"
#include "graphics/volumedata.h"
#include "System/stringManager.h"
#include "MeshEdit\CMeshCutManager.h"
#include "MeshEdit\CMeshViewRenderManager.h"
#include "MeshEdit\CMeshWorkManager.h"
#include "MeshEdit\CManipulator.h"
#include "MeshEdit\CMeshModelViewManager.h"
#include "MeshEdit\CMeshDlgManager.h"
#include "MeshEdit\CMeshHoleFillManager.h"

#include <ppl.h>
#include <thread>

#include <vtkImageGridSource.h>
#include <vtkPolyDataToImageStencil.h>
#include <vtkImageStencil.h>
#include <vtkImageCast.h>


ActionCreateMesh::ActionCreateMesh(
	DataContext* pDataContext, 
	eMeshPrimitiveType type,
	mip::VECTOR3 vecSize, 
	QString strName,
	int resolution_1, 
	int resolution_2, 
	QUndoCommand* parent) :
	m_type(type),
	m_size(vecSize),
	m_name(strName),
	m_pMesh(NULL),
	m_first(true),
	m_pDataContext(pDataContext)
{
	static int s_id = ACT_ID_MESHLIST_CREATE_MESH;

	for (int i = 0; i < 3; i++)
	{
		m_Points[i] = -1;
	}

	m_resolution_1 = resolution_1;
	m_resolution_2 = resolution_2;

	m_id = s_id++;
}

ActionCreateMesh::~ActionCreateMesh()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString str = QString().sprintf("%d", m_id);

		deleteUndoRedoFile(str, &dir);
	}
}

void ActionCreateMesh::undo()
{
	m_pDataContext->m_MeshData.DeleteMeshLayer(m_UID);

	MeshInfo* pMeshInfo = nullptr;
	for (int i = m_UID; i < m_pDataContext->m_MeshData.GetMeshCount(); i++)
	{
		pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(i);

		if (pMeshInfo)
		{
			//layeruid, mesh map move
			m_pDataContext->m_MeshData.ChangeMeshUID(i, i - 1, pMeshInfo->uid);
		}
	}

	m_pDataContext->m_MeshData.SetCurrentMeshIndex(0);
	m_pDataContext->m_MeshData.ClearMeshInfoSelectMode();
	m_pDataContext->m_MeshData.SetMeshInfoModeSelectMode(0, true);

	MESH_MODELVIEW_MANAGER->UpdatePivotPoint(0);

	WIN_MANAGER->updateMeshUI();
	WIN_MANAGER->renderLater_All();
	WIN_MANAGER->setSaveState(false);
}

void ActionCreateMesh::redo()
{
	if (m_first)
	{
		m_pDataContext->m_MeshData.CreateMeshInfo();
		m_UID = m_pDataContext->m_MeshData.GetCurrentMeshIndex();
		m_pDataContext->m_MeshData.SetMeshName(m_name, m_UID);

		MeshInfo* info = m_pDataContext->m_MeshData.GetMeshInfo(m_UID);

		if (info)
		{
			m_meshInfo = (*info);
		}
	}
	else
	{
		m_pDataContext->m_MeshData.AddMeshInfo(m_UID, m_meshInfo);
	}

	if (!m_first)
	{
		MeshInfo* info = nullptr;
		for (int i = m_pDataContext->m_MeshData.GetMeshCount() - 1; i >= m_UID; i--)
		{
			info = m_pDataContext->m_MeshData.GetMeshInfo(i);

			if (info)
			{
				//layeruid, mesh map move
				m_pDataContext->m_MeshData.ChangeMeshUID(i, i + 1, info->uid);
			}
		}
	}

	if (m_first)
	{
		m_first = false;
	}

	QDir dir(STRING_MANAGER->cacheFilePath);

	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);

		QFile file(filename);

		m_pMesh = new mip::MeshTopology(g_Renderer);

		if ((m_Points[0] > 0) && file.exists() && file.open(QIODevice::ReadOnly))
		{
			loadDataAllRedoFile(filename, m_pMesh);
			file.close();
		}
		else if (file.open(QIODevice::WriteOnly))
		{
#ifdef MESH_TEST
			bool success = false;
			if (eMeshPrimitiveType::Cube == m_type)
			{
				success = m_meshPrimitiveBuilder.BuildCube(g_Renderer, m_pMesh, m_size);
			}
			else if (eMeshPrimitiveType::Cylinder == m_type)
			{
				success = m_meshPrimitiveBuilder.BuildCylinder(g_Renderer, m_pMesh, m_size, m_resolution_1);
			}
			else if (eMeshPrimitiveType::Sphere == m_type)
			{
				success = m_meshPrimitiveBuilder.BuildSphere(g_Renderer, m_pMesh, m_size, m_resolution_1, m_resolution_2);
			}

			if (success)//success
			{
				saveDataAllRedoFile(filename, m_pMesh);
			}
			else
			{
				SAFE_DELETE(m_pMesh);
			}

			file.close();
#endif
		}
		else
		{
			SAFE_DELETE(m_pMesh);
		}
	}

	if (m_pMesh)
	{
		m_meshPrimitiveBuilder.UpdateTopology_And_Subdivision(g_Renderer, m_pMesh, m_meshInfo.color);

		//m_pDataContext->m_MeshData.InsertMesh(m_UID, m_pMesh);
		m_pDataContext->m_MeshData.InsertMeshWithoutBuildBuffer(m_UID, m_pMesh);

		//20210810_byPHS_Manipulator와 원점이 맞지않아 주석처리
		mip::VECTOR3	offset_center(m_pDataContext->volume_data.getSizeX() * 0.5f, m_pDataContext->volume_data.getSizeY() * 0.5f, m_pDataContext->volume_data.getSizeZ() * 0.5f);
		m_pMesh->addTranslate(offset_center);

		int cur_idx = m_pDataContext->m_MeshData.GetCurrentMeshIndex();
		m_pDataContext->m_MeshData.ClearMeshInfoSelectMode();
		m_pDataContext->m_MeshData.SetMeshInfoModeSelectMode(cur_idx, true);

		MESH_MODELVIEW_MANAGER->UpdatePivotPoint(cur_idx);

		m_pMesh = nullptr;
	}

	WIN_MANAGER->updateMeshUI();
	WIN_MANAGER->renderLater_All();
	WIN_MANAGER->setSaveState(false);
}
