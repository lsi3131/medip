#include "stdafx.h"
#include "ActionAddMeshes.h"
#include "StringManager.h"
#include "MeshEdit/CMeshDlgManager.h"
#include "MeshEdit/CMeshWorkManager.h"
#include "WindowManager.h"
#include "MeshControl.h"

ActionAddMeshes::ActionAddMeshes(
	DataContext* pDataContext,
	const std::vector<ActionAddMeshesData>& dataList,
	bool isUpScale,
	QUndoCommand* _parent) :
	ActionAddMeshes(
		pDataContext,
		WIN_MANAGER,
		MESH_WORK_MANAGER,
		MESH_DIALOG_MANAGER,
		PLANE_MANIPULATOR,
		g_Renderer,
		dataList,
		STRING_MANAGER->cacheFilePath,
		isUpScale,
		_parent)
{
}

ActionAddMeshes::ActionAddMeshes(
	DataContext* pDataContext,
	WindowManager* pWinManager,
	CMeshWorkManager* pMeshWorkManager,
	CMeshDlgManager* pMeshDlgManager,
	CPlaneManiplator* pPlaneManipulator,
	mip::Renderer* pRenderer,
	const std::vector<ActionAddMeshesData>& dataList,
	const QString& cacheDirectory,
	bool isUpScale,
	QUndoCommand* _parent) :
	QUndoCommand(_parent),
	CUndoRedo(pWinManager, pMeshWorkManager),
	m_pWinManager(pWinManager),
	m_pMeshDlgManager(pMeshDlgManager),
	m_pPlaneManipulator(pPlaneManipulator),
	m_pRenderer(pRenderer),
	m_cacheDirectory(cacheDirectory)
{
	static int s_id = ACT_ID_MESHLIST_ADD;

	m_id = s_id++;

	m_pDataContext = pDataContext;

	for (const auto& data : dataList)
	{
		m_newLayerUidList.push_back(data.LayerUID);
		m_newMeshTopologyList.push_back(data.pMeshData);
	}

	m_isFirst = true;
	m_isUpScale = isUpScale;
}

ActionAddMeshes::~ActionAddMeshes()
{
	QDir dir(m_cacheDirectory);
	if (dir.exists())
	{
		QString id_Text = QString().sprintf("%d", m_id);
		deleteUndoRedoFile(id_Text, &dir);
	}
}

void ActionAddMeshes::Run()
{
	// CT Plane
	mip::PLANE axialPlane = getCTPlane(WT_AXIAL);
	mip::PLANE coronalPlane = getCTPlane(WT_CORONAL);
	mip::PLANE sagittalPlane = getCTPlane(WT_SAGITTAL);

	// get offset mat(mesh <->volume)
	mip::VECTOR3 vOffsetCenter = m_pDataContext->volume_data.GetOffsetCenter();
	mip::MATRIX44 matOffsetCenter;
	matOffsetCenter.translation(vOffsetCenter);

	int newMeshCount = (int)m_newLayerUidList.size();

	m_addedUidList.resize(newMeshCount, -1);
	m_addedMeshInfoList.resize(newMeshCount);

	for (int i = 0; i < newMeshCount; ++i)
	{
		m_addedUidList[i] = (m_pDataContext->m_MeshData.GetMeshCount() - newMeshCount) + i;

		MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(m_addedUidList[i]);

		if (pMeshInfo)
		{
			mip::MeshTopology* pMesh = m_pDataContext->m_MeshData.GetMesh(m_addedUidList[i]);

			if (pMesh)
			{
				// ¿Ü°û¼± °è»ê
				mip::MATRIX44 meshMat = pMesh->getMatrix() * matOffsetCenter;

				mip::mesh_control::calcMesh2Outline(pMeshInfo->vecOutline2DAxial, axialPlane, pMesh, &meshMat);
				mip::mesh_control::calcMesh2Outline(pMeshInfo->vecOutline2DSagittal, sagittalPlane, pMesh, &meshMat);
				mip::mesh_control::calcMesh2Outline(pMeshInfo->vecOutline2DCoronal, coronalPlane, pMesh, &meshMat);
			}

			m_addedMeshInfoList[i] = (*pMeshInfo);
		}
	}
}

mip::PLANE ActionAddMeshes::getCTPlane(WINDOW_TYPE winType)
{
	int currentDepth = m_pDataContext->volume_data.getDepth(winType);

	std::vector<mip::VECTOR3>	tempList;
	tempList = m_pDataContext->volume_data.GetMPRPPlanesByDepth(winType, currentDepth);

	double halfSpaceX = m_pDataContext->volume_data.getSpaceX() * 0.25f;
	double halfSpaceY = m_pDataContext->volume_data.getSpaceY() * 0.25f;
	double halfSpaceZ = m_pDataContext->volume_data.getSpaceZ() * 0.25f;

	switch (winType)
	{
	case WT_CORONAL:
	{
		tempList[0].y += halfSpaceY;
		tempList[1].y += halfSpaceY;
		tempList[2].y += halfSpaceY;
	}
	break;
	case WT_SAGITTAL:
	{
		tempList[0].x += halfSpaceX;
		tempList[1].x += halfSpaceX;
		tempList[2].x += halfSpaceX;
	}
	break;
	case WT_AXIAL:
	default:
	{
		tempList[0].z += halfSpaceZ;
		tempList[1].z += halfSpaceZ;
		tempList[2].z += halfSpaceZ;
	}
	break;
	}

	mip::PLANE plane;
	plane.fromTri(tempList[1], tempList[2], tempList[0]);

	return plane;
}

void ActionAddMeshes::undo()
{
	m_pMeshDlgManager->rejectDialog();

	int meshCount = (int)m_addedUidList.size();

	for (int midx = meshCount - 1; midx >= 0; --midx)
	{
		int addedUID = m_addedUidList[midx];
		if (addedUID != -1)
		{
			m_pDataContext->m_MeshData.SetDisconnectMesh(addedUID);
		}

		m_pDataContext->m_MeshData.DeleteMeshLayer(addedUID);

		MeshInfo* pInfo = nullptr;
		for (int i = addedUID; i < m_pDataContext->m_MeshData.GetMeshCount(); i++)
		{
			pInfo = m_pDataContext->m_MeshData.GetMeshInfo(i);

			if (pInfo)
			{
				//layeruid, mesh map move
				m_pDataContext->m_MeshData.ChangeMeshUID(i, i - 1, pInfo->uid);
			}
		}

		int currentMeshIndex = m_pDataContext->m_MeshData.GetCurrentMeshIndex();
		m_pDataContext->m_MeshData.ClearMeshInfoSelectMode();
		m_pDataContext->m_MeshData.SetMeshInfoModeSelectMode(currentMeshIndex, true);

		m_pWinManager->updateMeshUI();
		m_pWinManager->renderLater_All();
		m_pWinManager->setSaveState(false);
	}
}

void ActionAddMeshes::redo()
{
	m_pMeshDlgManager->rejectDialog();

	// CT Plane
	mip::PLANE axialPlane = getCTPlane(WT_AXIAL);
	mip::PLANE coronalPlane = getCTPlane(WT_CORONAL);
	mip::PLANE sagittalPlane = getCTPlane(WT_SAGITTAL);

	// get offset mat(mesh <->volume)
	mip::VECTOR3 offset(mip::VECTOR3(-m_pDataContext->volume_data.getSizeX() * 0.5f, -m_pDataContext->volume_data.getSizeY() * 0.5f, -m_pDataContext->volume_data.getSizeZ() * 0.5f));
	mip::MATRIX44 offsetMat;
	offsetMat.translation(offset);

	int meshCount = (int)m_newLayerUidList.size();

	if (m_isFirst)
	{
		Run();
	}
	else
	{
		for (int i = 0; i < meshCount; ++i)
		{
			m_pDataContext->m_MeshData.AddMeshInfo(m_addedUidList[i], m_addedMeshInfoList[i]);
			m_pWinManager->updateMeshUI();
		}
	}

	if (!m_isFirst)
	{
		for (int midx = 0; midx < meshCount; ++midx)
		{
			MeshInfo* pMeshInfo = nullptr;
			for (int i = m_pDataContext->m_MeshData.GetMeshCount() - 1; i >= m_addedUidList[midx]; i--)
			{
				pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(i);

				if (pMeshInfo)
				{
					//layeruid, mesh map move
					m_pDataContext->m_MeshData.ChangeMeshUID(i, i + 1, pMeshInfo->uid);

					pMeshInfo->m_nReduceLevel = 0;
					pMeshInfo->m_nSmoothLevel = 0;
				}
			}
		}
	}

	if (m_isFirst)
	{
		m_isFirst = false;
	}

	QDir dir(m_cacheDirectory);

	if (dir.exists() == false)
	{
		dir.mkpath(m_cacheDirectory);
	}

	QString filePath = m_cacheDirectory + QString("/redo%1").arg(m_id);
	QFile file(filePath);

	if (file.exists() && file.open(QIODevice::ReadOnly))
	{
		for (int midx = 0; midx < meshCount; ++midx)
		{
			mip::MeshTopology* pNewMeshData = new mip::MeshTopology(m_pRenderer);

			if (pNewMeshData)
			{
				QString redoFilePath = filePath + QString().sprintf("_%d", midx);

				loadDataAllRedoFile(redoFilePath, pNewMeshData);

				m_pDataContext->m_MeshData.InsertMeshToMeshMap(m_addedUidList[midx], pNewMeshData);

				MeshInfo* pInfo = m_pDataContext->m_MeshData.GetMeshInfo(m_addedUidList[midx]);

				if (pNewMeshData && pInfo)
				{
					// ¿Ü°û¼± °è»ê
					mip::MATRIX44 meshMat = pNewMeshData->getMatrix() * offsetMat;

					mip::mesh_control::calcMesh2Outline(pInfo->vecOutline2DAxial, axialPlane, pNewMeshData, &meshMat);
					mip::mesh_control::calcMesh2Outline(pInfo->vecOutline2DSagittal, sagittalPlane, pNewMeshData, &meshMat);
					mip::mesh_control::calcMesh2Outline(pInfo->vecOutline2DCoronal, coronalPlane, pNewMeshData, &meshMat);
				}

				m_pWinManager->updateMeshUI(false, m_addedUidList[midx]);
			}
		}

		mip::MeshTopology* pCurrentMesh = m_pDataContext->m_MeshData.GetCurrentMesh();

		m_pPlaneManipulator->UpdatePosition(pCurrentMesh);
	}
	else if (file.open(QIODevice::WriteOnly))
	{
		for (int midx = 0; midx < meshCount; ++midx)
		{
			QString redoFilePath = filePath + QString().sprintf("_%d", midx);

			saveDataAllRedoFile(redoFilePath, m_newMeshTopologyList[midx]);
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////
