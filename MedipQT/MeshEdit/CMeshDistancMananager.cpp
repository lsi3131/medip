#include "stdafx.h"
#include "CMeshDistancMananager.h"
#include "CMeshModelViewManager.h"
#include "CMeshCutManager.h"
#include "CMeshWorkManager.h"
#include "CMeshViewBtn3DScene.h"
#include "MeshControl.h"
#include "Main/MainMeshWidget.h"
#include "MEVolumeView.h"
#include "WindowManager.h"
#include "resourceManager.h"
#include "MedipQT.h"
#include "DataContext.h"

/*
@brief
*/
CMeshDistancMananager::CMeshDistancMananager()
{
	m_pPickedMesh = nullptr;
}

/*
@brief
*/
CMeshDistancMananager::~CMeshDistancMananager()
{
	SAFE_DELETE(m_pPickedMesh);
}

void CMeshDistancMananager::Init(DataContext* pDataContext)
{
	Init(pDataContext,
		MESH_WORK_MANAGER,
		MESH_CUT_MANAGER,
		MESH_BTN_SCENE_MANAGER);
}

void CMeshDistancMananager::Init(
	DataContext* pDataContext,
	CMeshWorkManager* pMeshWorkManager,
	CMeshCutManager* pMeshCutManager,
	CMeshViewBtn3DScene* pBtn3DScene)
{
	m_pDataContext = pDataContext;

	m_pMeshWorkManager = pMeshWorkManager;
	m_pMeshCutManager = pMeshCutManager;
	m_pBtn3DScene = pBtn3DScene;

	m_movIndex = m_pckLength = m_pckIndex = -1;

	m_pckColor = mip::COLOR(255, 0, 0, 255);
}

/*
@brief
@return
*/
CMeshDistancMananager* CMeshDistancMananager::getInstance()
{
	static CMeshDistancMananager instance;
	return &instance;
}

/*
@brief
@return
*/
void CMeshDistancMananager::Process()
{
	//auto view = WIN_MANAGER->mainMeshWidget->getMainView();

	int currenMeshIdx = m_pDataContext->m_MeshData.GetCurrentMeshIndex();
	if (currenMeshIdx < 0)
	{
		m_pMeshWorkManager->UpdateWorkMode(MESH_WORK_NONE, true);
		//QMessageBox::warning(view, QString("Action for single mesh"), QString("Pick the mesh to apply this function."));
		return;
	}

	//const mip::MeshTopology * m = m_pDataContext->m_MeshData.GetMesh(*(MESH_MODELVIEW_MANAGER->getMeshpckIDPtr()));
	const mip::MeshTopology* m = m_pDataContext->m_MeshData.GetCurrentMesh();

	//if (m_BtnDistance->isChecked() && m)
	if (m)
	{
		mip::AABB abBox = m->m_boundingBox;
		mip::VECTOR3 mSize = abBox.getSize();

		float midSize = mip::math::Max3(mSize.x, mSize.y, mSize.z) - mip::math::Min3(mSize.x, mSize.y, mSize.z);

		midSize /= 10.0f;

		mSize = mip::VECTOR3(midSize, midSize, midSize);
		//	mSize /= 3.f;

		WIN_MANAGER->makeCurrent();

		SAFE_DELETE(m_pPickedMesh);

		m_pPickedMesh = new mip::MeshTopology(g_Renderer);

		mip::mesh_control::createSphere(mSize, 10, 10, m_pPickedMesh->m_verts, m_pPickedMesh->m_tris, m_pPickedMesh->m_normals);

		m_pPickedMesh->updateVertex();
		m_pPickedMesh->buildRenderBufferTopology();

		WIN_MANAGER->doneCurrent();

		m_pMeshWorkManager->UpdateWorkMode(MESH_WORK_DISTANCE, true);

	}
	else
	{
		m_pMeshWorkManager->UpdateWorkMode(MESH_WORK_NONE, true);
	}


	m_pMeshCutManager->clearPlaneCutParam();
}

/*
@brief
@return
*/
void		CMeshDistancMananager::Update(MESH_WORK_MODE _mode, bool iconRefresh)
{
	int sz_icon = WIN_MANAGER->mainWindow->IconSize;

	static QIcon disIcon[] = { RESOURCE_MANAGER->getIcon(ICON_NON_ANNO_LENGTH, sz_icon, sz_icon),
		RESOURCE_MANAGER->getIcon(ICON_ANNO_LENGTH, sz_icon, sz_icon) };

	auto btn_dist = m_pBtn3DScene->m_BtnDistance;

	bool chkMode = false;
	bool chkRefresh = false;

	if (chkRefresh = ((_mode == MESH_WORK_DISTANCE) != (chkMode = btn_dist->isChecked())))
	{
		chkMode = !chkMode;
		btn_dist->setChecked(chkMode);
		btn_dist->setIcon(disIcon[chkMode]);
	}
	else if (!chkRefresh && iconRefresh)
	{
		btn_dist->setIcon(disIcon[chkMode]);
	}
}

/*
@brief
@return
*/
void CMeshDistancMananager::ClearList()
{
	m_pointList3D.clear();
	m_lineList3D.clear();
}