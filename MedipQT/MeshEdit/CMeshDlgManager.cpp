#include "stdafx.h"
#include "CMeshDlgManager.h"
#include "MedipQT.h"
#include "windowManager.h"
#include "Main/MainMeshWidget.h"
#include "MEVolumeView.h"
#include "CollapseDock.h"
#include "Tabwindow.h"

#include "BrushSculptDlg.h"
#include "BrushSelectDlg.h"
#include "SmoothDlg.h"
#include "ReduceDlg.h"
#include "HollowDlg.h"
#include "CRemeshDlg.h"
#include "CSolidDlg.h"
#include "CHoleFillingDlg.h"
#include "CIslandFilterDlg.h"

#include "CMeshViewBtn3DScene.h"
#include "CMeshWorkManager.h"

#include "DataContext.h"

#include <QmessageBox>

/*
@brief
*/
CMeshDlgManager::CMeshDlgManager()
{
	m_pSculptDlg = nullptr;
	m_pSelectDlg = nullptr;
	m_pSmoothDlg = nullptr;
	m_pReduceDlg = nullptr;
	m_pHollowDlg = nullptr;
	m_pStamp3dDlg = nullptr;
	m_pRemeshDlg = nullptr;
	m_pSolidDlg = nullptr;
	m_pHoleFillDlg = nullptr;
	m_pMeshOffsetDlg = nullptr;
	//m_pPlaneCutDlg = nullptr;
	m_pMeshCutDlg = nullptr;
	m_pManpulateDlg = nullptr;
	m_pIslandFilterDlg = nullptr;

	m_pSubDivisionDlg = nullptr;
	m_oldMode = MESH_WORK_NONE;
}

/*
@brief
*/
CMeshDlgManager::~CMeshDlgManager()
{
	//rejectDialog();
}

CMeshDlgManager* CMeshDlgManager::getInstance()
{
	static CMeshDlgManager instance;
	return &instance;
}

void CMeshDlgManager::Init(DataContext* pDataContext, MEVolumeView* pViewer)
{
	Init(
		pDataContext,
		pViewer,
		MESH_WORK_MANAGER,
		MESH_BTN_SCENE_MANAGER
	);
}

void CMeshDlgManager::Init(
	DataContext* pDataContext,
	MEVolumeView* pViewer,
	CMeshWorkManager* pWorkManager,
	CMeshViewBtn3DScene* pBtn3DScene)
{
	m_pDataContext = pDataContext;
	m_pViewer = pViewer;

	m_pWorkManager = pWorkManager;
	m_pBtn3DScene = pBtn3DScene;
}

bool CMeshDlgManager::makeMeshDialog(int mode, bool _b_force, bool bClearFlag)
{
	if (!ACTION_MANAGER->isActionFinished() || !m_pDataContext || !m_pViewer)
	{
		//QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));		
		return false;
	}

	//MEVolumeView* veiwMesh = WIN_MANAGER->mainMeshWidget->getMainView();

	mip::MeshTopology* p_mesh = nullptr;
	int							meshCnt = m_pDataContext->m_MeshData.GetMeshCount();
	//int							pickCnt = WIN_MANAGER->vt_pckID.size();

	int nSelectedID = -1;
	int nSelectedCnt = 0;
	//for (int ii = 0; ii < WIN_MANAGER->vt_pckID.size(); ii++)
	for (int ii = 0; ii < meshCnt; ii++)
	{
		MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(ii);
		//if (WIN_MANAGER->vt_pckID[ii])
		if (pMeshInfo && pMeshInfo->selected)
		{
			nSelectedCnt++;
			nSelectedID = ii;
		}
	}

	if (!_b_force)
	{
		if (nSelectedID < 0 || (nSelectedCnt != 1 && (mode == MESH_WORK_BRUSH_SELECTION || mode == MESH_WORK_BRUSH_SCULPT)))
		{
			//QMessageBox::warning(veiwMesh, QString("Action for single mesh"), QString("Pick the mesh to apply this function."));

			m_pWorkManager->UpdateWorkMode(MESH_WORK_NONE, true);
			return false;
		}
	}

	QDialog* pDialog = nullptr;
	int nPickMesh = m_pDataContext->m_MeshData.GetCurrentMeshIndex();
	MeshInfo* info = m_pDataContext->m_MeshData.GetMeshInfo(nPickMesh);
	mip::MeshTopology* mesh = m_pDataContext->m_MeshData.GetMesh(nPickMesh);
	//auto view				= WIN_MANAGER->mainMeshWidget->getMainView();

	if (mesh)
	{
		if (!(m_oldMode == MESH_WORK_BRUSH_SELECTION && mip::mesh_control::isTriFlag(mesh, mip::SELECTED))
			&& (mode != MESH_WORK_FREEPOLYLINE_CUT) && (mode != MESH_WORK_POLYGON_CUT) && (mode != MESH_WORK_POLYLINE_CUT)
			&& (mode != MESH_WORK_PLANE_CUT) && bClearFlag
			)
		{
			//WIN_MANAGER->mainMeshWidget->getMainView()->clearSelectFlag(true);
			m_pViewer->clearSelectFlag(true);
		}
	}

	switch (mode)
	{
	case MESH_WORK_BRUSH_SELECTION:
	{
		if (m_pSelectDlg == nullptr)
			m_pSelectDlg = new BrushSelectDlg(m_pDataContext, m_pViewer);

		//m_pSelectDlg->updateColor();		
		m_pSelectDlg->updateSizeSlider(m_pViewer->GetSphereScale());

		m_pSelectDlg->setMesh(mesh);
		//m_pSelectDlg->initUI();

		//mesh->updateColor(mesh->m_baseColor);

		pDialog = dynamic_cast<QDialog*>(m_pSelectDlg);
		m_pViewer->setFocus();
	}
	break;
	case MESH_WORK_BRUSH_SCULPT:
	{
		if (m_pSculptDlg == nullptr)
			m_pSculptDlg = new BrushSculptDlg(m_pDataContext, m_pViewer);


		m_pSculptDlg->updateSizeSlider(m_pViewer->GetSphereScale());

		pDialog = dynamic_cast<QDialog*>(m_pSculptDlg);
	}
	break;
	case MESH_WORK_SMOOTH:
	{
		if (m_pSmoothDlg == nullptr)
			m_pSmoothDlg = new SmoothDlg(m_pDataContext, QString("Smooth"), m_pViewer);

		m_pSmoothDlg->setMesh(mesh);
		m_pSmoothDlg->setMeshIdx(nPickMesh);
		m_pSmoothDlg->updateDialog();

		pDialog = dynamic_cast<QDialog*>(m_pSmoothDlg);
	}
	break;
	case MESH_WORK_REDUCTION:
	{
		if (m_pReduceDlg == nullptr)
			m_pReduceDlg = new ReduceDlg(m_pDataContext, QString("Reduction"), m_pViewer);

		m_pReduceDlg->initTopology();
		//m_pReduceDlg->initOptionValue();
		pDialog = dynamic_cast<QDialog*>(m_pReduceDlg);
	}
	break;
	case MESH_WORK_HOLLOW:
	{
		if (m_pHollowDlg == nullptr)
			m_pHollowDlg = new HollowDlg(m_pDataContext, QString("Hollow"), m_pViewer);

		pDialog = dynamic_cast<QDialog*>(m_pHollowDlg);
	}
	break;
	case MESH_WORK_REMESH:
	{
		if (m_pRemeshDlg == nullptr)
		{
			m_pRemeshDlg = new CRemeshDlg(m_pDataContext, QString("Remesh"), m_pViewer);
		}

		//m_pRemeshDlg->Update(WIN_MANAGER->vt_pckID);
		m_pRemeshDlg->Update();

		pDialog = dynamic_cast<QDialog*>(m_pRemeshDlg);
	}
	break;
	case MESH_WORK_SOLID:
	{
		if (m_pSolidDlg == nullptr)
		{
			m_pSolidDlg = new CSolidDlg(m_pDataContext, QString("Solid"), m_pViewer);
		}

		pDialog = dynamic_cast<QDialog*>(m_pSolidDlg);
	}
	break;
	case MESH_WORK_PLANE_CUT:
	case MESH_WORK_POLYGON_CUT:
	case MESH_WORK_POLYLINE_CUT:
	case MESH_WORK_FREEPOLYLINE_CUT:
	{
		if (m_pMeshCutDlg == nullptr)
		{
			m_pMeshCutDlg = new CMeshCutDlg(m_pDataContext, QString("Mesh Cut"), m_pViewer);
		}

		m_pMeshCutDlg->updateComboboxIndex(int(mode - 5), false);

		pDialog = dynamic_cast<QDialog*>(m_pMeshCutDlg);
	}
	break;
	case MESH_WORK_HOLE_FILL_SELECTED:
	{
		if (m_pHoleFillDlg == nullptr)
		{
			m_pHoleFillDlg = new CHoleFillingDlg(m_pDataContext, QString("Hole-Fill"), m_pViewer);
		}

		pDialog = dynamic_cast<QDialog*>(m_pHoleFillDlg);
	}
	break;
	case MESH_WORK_MANIFULATE:
	{
		if (m_pManpulateDlg == nullptr)
		{
			m_pManpulateDlg = new CManpulateDlg(m_pDataContext, QString("Manipulator"), m_pViewer);
		}

		m_pManpulateDlg->Reset();

		pDialog = dynamic_cast<QDialog*>(m_pManpulateDlg);
	}
	break;
	case MESH_WORK_ISLANDFILTER:
	{
		if (m_pIslandFilterDlg == nullptr)
		{
			m_pIslandFilterDlg = new CIslandFilterDlg(m_pDataContext, QString("Island-Filter"), m_pViewer);
		}

		pDialog = dynamic_cast<QDialog*>(m_pIslandFilterDlg);
	}
	break;
	case MESH_WORK_SUBDIVISION:
	{
		if (m_pSubDivisionDlg == nullptr)
		{
			m_pSubDivisionDlg = new CSubDivisionDlg(m_pDataContext, QString("SubDivision"), m_pViewer);
		}

		pDialog = dynamic_cast<QDialog*>(m_pSubDivisionDlg);
	}
	break;
#if SUPPORT_STAMP3D == 1
	case MESH_WORK_STAMP3D:
	{
		if (m_pStamp3dDlg == nullptr)
		{
			m_pStamp3dDlg = new Stamp3dDlg(m_pViewer);
		}

		pDialog = dynamic_cast<QDialog*>(m_pStamp3dDlg);
	}
	break;
#endif
#if SUPPORT_MESHOFFSET == 1
	case MESH_WORK_MESHOFFSET:
	{
		if (m_pMeshOffsetDlg == nullptr)
			m_pMeshOffsetDlg = new MeshOffsetDlg(m_pViewer);

		pDialog = dynamic_cast<QDialog*>(m_pMeshOffsetDlg);
	}
	break;
#endif

	default:
		break;
	}

	if (pDialog == nullptr)
	{
		return false;
	}

	m_pWorkManager->UpdateWorkMode((MESH_WORK_MODE)mode, true);

	printf_s("\n dialog setVisible ");

	pDialog->setWindowOpacity(1);
	pDialog->setVisible(true);
	pDialog->setMouseTracking(true);
	pDialog->installEventFilter(m_pViewer);

	MoveMeshDialog();

	//Thread TEST
	if (mode == MESH_WORK_BRUSH_SCULPT)
	{
		//int nPickMesh = WIN_MANAGER->volume_data.GetCurrentMeshIndex();
		//MeshInfo* info = WIN_MANAGER->volume_data.GetMeshInfo(nPickMesh);
		//mip::MeshTopology *mesh = WIN_MANAGER->volume_data.GetMesh(nPickMesh);

		//ACTION_MANAGER->action_Create_MatchingData(mesh);
	}
	else if (mode == MESH_WORK_BRUSH_SELECTION) //220121 허 건 과장
	{
		m_pSelectDlg->initUI();
	}

	WIN_MANAGER->mainWindow->activateWindow();
	WIN_MANAGER->mainWindow->setFocus();

	m_oldMode = mode;

	return true;
}

bool CMeshDlgManager::isMeshDialog()
{
	if (m_pSculptDlg != nullptr && m_pSculptDlg->isVisible()) return true;
	if (m_pSelectDlg != nullptr && m_pSelectDlg->isVisible()) return true;
	if (m_pSmoothDlg != nullptr && m_pSmoothDlg->isVisible()) return true;
	if (m_pReduceDlg != nullptr && m_pReduceDlg->isVisible()) return true;
	if (m_pHollowDlg != nullptr && m_pHollowDlg->isVisible()) return true;
	if (m_pRemeshDlg != nullptr && m_pRemeshDlg->isVisible()) return true;
	if (m_pSolidDlg != nullptr && m_pSolidDlg->isVisible()) return true;
	if (m_pHoleFillDlg != nullptr && m_pHoleFillDlg->isVisible()) return true;
	//if (m_pPlaneCutDlg != nullptr && m_pPlaneCutDlg->isVisible()) return true;
	if (m_pMeshCutDlg != nullptr && m_pMeshCutDlg->isVisible()) return true;
	if (m_pManpulateDlg != nullptr && m_pManpulateDlg->isVisible()) return true;
	if (m_pIslandFilterDlg != nullptr && m_pIslandFilterDlg->isVisible()) return true;
	if (m_pSubDivisionDlg != nullptr && m_pSubDivisionDlg->isVisible()) return true;
	if (m_pStamp3dDlg != nullptr && m_pStamp3dDlg->isVisible()) return true;
	if (m_pMeshOffsetDlg != nullptr && m_pMeshOffsetDlg->isVisible()) return true;

	return false;
}

void CMeshDlgManager::rejectDialog(int old_idx, bool bForce)
{
	if (m_pSculptDlg != nullptr && m_pSculptDlg->isVisible()) m_pSculptDlg->reject(bForce);
	if (m_pSelectDlg != nullptr && m_pSelectDlg->isVisible())
	{
		m_pSelectDlg->setOldIdx(old_idx);
		m_pSelectDlg->reject(bForce);
	}
	if (m_pSmoothDlg != nullptr && m_pSmoothDlg->isVisible()) m_pSmoothDlg->reject(bForce);
	if (m_pReduceDlg != nullptr && m_pReduceDlg->isVisible()) m_pReduceDlg->reject(bForce);
	if (m_pHollowDlg != nullptr && m_pHollowDlg->isVisible()) m_pHollowDlg->reject(bForce);
	if (m_pRemeshDlg != nullptr && m_pRemeshDlg->isVisible()) m_pRemeshDlg->reject(bForce);
	if (m_pSolidDlg != nullptr && m_pSolidDlg->isVisible()) m_pSolidDlg->reject(bForce);
	if (m_pHoleFillDlg != nullptr && m_pHoleFillDlg->isVisible()) m_pHoleFillDlg->reject(bForce);
	if (m_pManpulateDlg != nullptr && m_pManpulateDlg->isVisible()) m_pManpulateDlg->reject(bForce);
	//if (m_pPlaneCutDlg != nullptr && m_pPlaneCutDlg->isVisible()) m_pPlaneCutDlg->reject(bForce);
	if (m_pMeshCutDlg != nullptr && m_pMeshCutDlg->isVisible()) m_pMeshCutDlg->reject(bForce);
	if (m_pIslandFilterDlg != nullptr && m_pIslandFilterDlg->isVisible()) m_pIslandFilterDlg->reject(bForce);
	if (m_pSubDivisionDlg != nullptr && m_pSubDivisionDlg->isVisible()) m_pSubDivisionDlg->reject(bForce);
	if (m_pStamp3dDlg != nullptr && m_pStamp3dDlg->isVisible()) m_pStamp3dDlg->reject(bForce);
	if (m_pMeshOffsetDlg != nullptr && m_pMeshOffsetDlg->isVisible()) m_pMeshOffsetDlg->reject(bForce);
}

void		CMeshDlgManager::rejectDialog(MESH_DIALOG_TYPE _type)
{
	switch (_type)
	{
	case MESH_DIALOG_SCULPT:
		if (m_pSculptDlg != nullptr && m_pSculptDlg->isVisible()) m_pSculptDlg->reject();
		break;
	case MESH_DIALOG_SELECT:
		if (m_pSelectDlg != nullptr && m_pSelectDlg->isVisible()) m_pSelectDlg->reject();
		break;
	case MESH_DIALOG_SMOOTH:
		if (m_pSmoothDlg != nullptr && m_pSmoothDlg->isVisible()) m_pSmoothDlg->reject();
		break;
	case MESH_DIALOG_REDUCE:
		if (m_pReduceDlg != nullptr && m_pReduceDlg->isVisible()) m_pReduceDlg->reject();
		break;
	case MESH_DIALOG_HOLLOW:
		if (m_pHollowDlg != nullptr && m_pHollowDlg->isVisible()) m_pHollowDlg->reject();
		break;
	case MESH_DIALOG_REMESH:
		if (m_pRemeshDlg != nullptr && m_pRemeshDlg->isVisible()) m_pRemeshDlg->reject();
		break;
	case MESH_DIALOG_SOLID:
		if (m_pSolidDlg != nullptr && m_pSolidDlg->isVisible()) m_pSolidDlg->reject();
		break;
	case MESH_DIALOG_FILLHOLE:
		if (m_pHoleFillDlg != nullptr && m_pHoleFillDlg->isVisible()) m_pHoleFillDlg->reject();
		break;
		//case MESH_DIALOG_PLANECUT:
		//	if (m_pPlaneCutDlg!= nullptr && m_pPlaneCutDlg->isVisible()) m_pPlaneCutDlg->reject();
		//	break;		
	case MESH_DIALOG_MESHCUT:
		if (m_pMeshCutDlg != nullptr && m_pMeshCutDlg->isVisible()) m_pMeshCutDlg->reject();
		break;
	case MESH_WORK_MANIFULATE:
		if (m_pManpulateDlg != nullptr && m_pManpulateDlg->isVisible()) m_pManpulateDlg->reject();
		break;
	case MESH_WORK_ISLANDFILTER:
		if (m_pIslandFilterDlg != nullptr && m_pIslandFilterDlg->isVisible()) m_pIslandFilterDlg->reject();
		break;
	case MESH_WORK_SUBDIVISION:
		if (m_pSubDivisionDlg != nullptr && m_pSubDivisionDlg->isVisible()) m_pSubDivisionDlg->reject();
		break;
	case MESH_DIALOG_STAMP3D:
		if (m_pStamp3dDlg != nullptr && m_pStamp3dDlg->isVisible()) m_pStamp3dDlg->reject();
		break;
	case MESH_DIALOG_MESHOFFSET:
		if (m_pMeshOffsetDlg != nullptr && m_pMeshOffsetDlg->isVisible()) m_pMeshOffsetDlg->reject();
		break;
	}
}

void		CMeshDlgManager::closeDialog()
{
	if (m_pSculptDlg != nullptr && m_pSculptDlg->isVisible()) m_pSculptDlg->close();
	if (m_pSelectDlg != nullptr && m_pSelectDlg->isVisible()) m_pSelectDlg->close();
	if (m_pSmoothDlg != nullptr && m_pSmoothDlg->isVisible()) m_pSmoothDlg->close();
	if (m_pReduceDlg != nullptr && m_pReduceDlg->isVisible()) m_pReduceDlg->close();
	if (m_pHollowDlg != nullptr && m_pHollowDlg->isVisible()) m_pHollowDlg->close();
	if (m_pRemeshDlg != nullptr && m_pRemeshDlg->isVisible()) m_pRemeshDlg->close();
	if (m_pSolidDlg != nullptr && m_pSolidDlg->isVisible()) m_pSolidDlg->close();
	if (m_pHoleFillDlg != nullptr && m_pHoleFillDlg->isVisible()) m_pHoleFillDlg->close();
	if (m_pManpulateDlg != nullptr && m_pManpulateDlg->isVisible()) m_pManpulateDlg->close();
	//if (m_pPlaneCutDlg != nullptr && m_pPlaneCutDlg->isVisible()) m_pPlaneCutDlg->reject(bForce);
	if (m_pMeshCutDlg != nullptr && m_pMeshCutDlg->isVisible()) m_pMeshCutDlg->close();
	if (m_pIslandFilterDlg != nullptr && m_pIslandFilterDlg->isVisible()) m_pIslandFilterDlg->close();
	if (m_pSubDivisionDlg != nullptr && m_pSubDivisionDlg->isVisible()) m_pSubDivisionDlg->close();
	if (m_pStamp3dDlg != nullptr && m_pStamp3dDlg->isVisible()) m_pStamp3dDlg->close();
}

QDialog* CMeshDlgManager::getMeshDialog()
{
	if (m_pSculptDlg != nullptr && m_pSculptDlg->isVisible()) return m_pSculptDlg;
	if (m_pSelectDlg != nullptr && m_pSelectDlg->isVisible()) return m_pSelectDlg;
	if (m_pSmoothDlg != nullptr && m_pSmoothDlg->isVisible()) return m_pSmoothDlg;
	if (m_pReduceDlg != nullptr && m_pReduceDlg->isVisible()) return m_pReduceDlg;
	if (m_pHollowDlg != nullptr && m_pHollowDlg->isVisible()) return m_pHollowDlg;
	if (m_pRemeshDlg != nullptr && m_pRemeshDlg->isVisible()) return m_pRemeshDlg;
	if (m_pSolidDlg != nullptr && m_pSolidDlg->isVisible()) return m_pSolidDlg;
	if (m_pHoleFillDlg != nullptr && m_pHoleFillDlg->isVisible()) return m_pHoleFillDlg;
	//if (m_pPlaneCutDlg != nullptr && m_pPlaneCutDlg->isVisible()) return m_pPlaneCutDlg;
	if (m_pMeshCutDlg != nullptr && m_pMeshCutDlg->isVisible()) return m_pMeshCutDlg;
	if (m_pManpulateDlg != nullptr && m_pManpulateDlg->isVisible()) return m_pManpulateDlg;
	if (m_pIslandFilterDlg != nullptr && m_pIslandFilterDlg->isVisible()) return m_pIslandFilterDlg;
	if (m_pSubDivisionDlg != nullptr && m_pSubDivisionDlg->isVisible()) return m_pSubDivisionDlg;
	if (m_pStamp3dDlg != nullptr && m_pStamp3dDlg->isVisible()) return m_pStamp3dDlg;
	if (m_pMeshOffsetDlg != nullptr && m_pMeshOffsetDlg->isVisible()) return m_pMeshOffsetDlg;

	return nullptr;
}

void* CMeshDlgManager::getMeshDialog(MESH_DIALOG_TYPE _type)
{
	void* p_dlg = nullptr;

	switch (_type)
	{
	case MESH_DIALOG_SCULPT:
	{
		p_dlg = (void*)m_pSculptDlg;
	}
	break;
	case MESH_DIALOG_SELECT:
	{
		p_dlg = (void*)m_pSelectDlg;
	}
	break;
	case MESH_DIALOG_SMOOTH:
	{
		p_dlg = (void*)m_pSmoothDlg;
	}
	break;
	case MESH_DIALOG_REDUCE:
	{
		p_dlg = (void*)m_pReduceDlg;
	}
	break;
	case MESH_DIALOG_HOLLOW:
	{
		p_dlg = (void*)m_pHollowDlg;
	}
	break;
	case MESH_DIALOG_REMESH:
	{
		p_dlg = (void*)m_pRemeshDlg;
	}
	break;
	case MESH_DIALOG_SOLID:
	{
		p_dlg = (void*)m_pSolidDlg;
	}
	break;
	case MESH_DIALOG_FILLHOLE:
	{
		p_dlg = (void*)m_pHoleFillDlg;
	}
	break;
	//case MESH_DIALOG_PLANECUT:
	//	{
	//		p_dlg = (void*)m_pPlaneCutDlg;
	//	}
	//	break;
	case MESH_DIALOG_MESHCUT:
	{
		p_dlg = (void*)m_pMeshCutDlg;
	}
	break;
	case MESH_DIALOG_MANIPULATE:
	{
		p_dlg = (void*)m_pManpulateDlg;
	}
	break;
	case MESH_DIALOG_ISLANDFILTER:
	{
		p_dlg = (void*)m_pIslandFilterDlg;
	}
	break;
	case MESH_DIALOG_SUBDIVISION:
	{
		p_dlg = (void*)m_pSubDivisionDlg;
	}
	break;
	case MESH_DIALOG_STAMP3D:
	{
		p_dlg = (void*)m_pStamp3dDlg;
	}
	break;
	case MESH_DIALOG_MESHOFFSET:
	{
		p_dlg = (void*)m_pMeshOffsetDlg;
	}
	break;
	}

	return p_dlg;
}

void* CMeshDlgManager::getMeshDialog(MESH_WORK_MODE _type)
{
	MESH_DIALOG_TYPE type;
	switch (_type)
	{
	case MESH_WORK_BRUSH_SCULPT:
		type = MESH_DIALOG_SCULPT;
		break;
	case MESH_WORK_BRUSH_SELECTION:
		type = MESH_DIALOG_SELECT;
		break;
	case MESH_WORK_SMOOTH:
		type = MESH_DIALOG_SMOOTH;
		break;
	case MESH_WORK_REDUCTION:
		type = MESH_DIALOG_REDUCE;
		break;
	case MESH_WORK_HOLLOW:
		type = MESH_DIALOG_HOLLOW;
		break;
	case MESH_WORK_REMESH:
		type = MESH_DIALOG_REMESH;
		break;
	case MESH_WORK_SOLID:
		type = MESH_DIALOG_SOLID;
		break;
	case MESH_WORK_HOLE_FILL_SELECTED:
		type = MESH_DIALOG_FILLHOLE;
		break;
		//case MESH_WORK_PLANE_CUT :
		//	type = MESH_DIALOG_PLANECUT;
		//	break;
	case MESH_WORK_PLANE_CUT:
	case MESH_WORK_POLYGON_CUT:
	case MESH_WORK_POLYLINE_CUT:
	case MESH_WORK_FREEPOLYLINE_CUT:
		type = MESH_DIALOG_MESHCUT;
		break;
	case MESH_WORK_MANIFULATE:
		type = MESH_DIALOG_MANIPULATE;
		break;
	case MESH_WORK_ISLANDFILTER:
		type = MESH_DIALOG_ISLANDFILTER;
		break;
	case MESH_WORK_SUBDIVISION:
		type = MESH_DIALOG_SUBDIVISION;
		break;
	case MESH_WORK_STAMP3D:
		type = MESH_DIALOG_STAMP3D;
		break;
	case MESH_WORK_MESHOFFSET:
		type = MESH_DIALOG_MESHOFFSET;
		break;
	}

	return getMeshDialog(type);
}

void CMeshDlgManager::MoveMeshDialog(int fixed)
{
	int _Right = 1; int _Left = 2; int _Exception = 3;

	//Mesh Dialog
	if (getMeshDialog() != nullptr)
	{
		auto mode = m_pWorkManager->getWorkMode();

		auto dlg = this->getMeshDialog(mode);

		QPushButton* btn = nullptr;

		if (dlg == nullptr)
		{
			return;
		}
		else
		{
			switch (mode)
			{
			case MESH_WORK_BRUSH_SCULPT:
				btn = m_pBtn3DScene->m_BtnSculpt;
				break;
			case MESH_WORK_BRUSH_SELECTION:
				btn = m_pBtn3DScene->m_BtnSelect;
				break;
			case MESH_WORK_STAMP3D:
				btn = m_pBtn3DScene->m_BtnStamp3d;
				break;
			case MESH_WORK_HOLE_FILL_SELECTED:
			case MESH_WORK_PLANE_CUT:
			case MESH_WORK_POLYGON_CUT:
			case MESH_WORK_POLYLINE_CUT:
			case MESH_WORK_FREEPOLYLINE_CUT:
			default:
				btn = m_pBtn3DScene->m_BtnFillHole;
				break;
			}
		}

		QRect			MEViewRect;

		MEVolumeView* veiwMesh = WIN_MANAGER->mainMeshWidget->getMainView();

		if (veiwMesh == nullptr)
		{
			return;
		}

		MeshTab* meshTab = WIN_MANAGER->GetTab()->getMeshTab();
		ROITab2* ROITab = WIN_MANAGER->GetTab()->getROITab();

		MEViewRect = veiwMesh->frameGeometry();
		QPoint MeshTabList_tl = veiwMesh->mapToGlobal(WIN_MANAGER->mainWindow->getTabDock()->geometry().topLeft());
		QPoint MEView_tl = veiwMesh->mapToGlobal(veiwMesh->rect().topLeft());
		QPoint MEView_bR = veiwMesh->mapToGlobal(veiwMesh->rect().bottomRight());

		QPoint BtnSculpt_tl = veiwMesh->mapToGlobal(btn->geometry().topLeft());

		QRect DialogRect = getMeshDialog()->geometry();
		int dlgW = DialogRect.width();
		int dlgH = DialogRect.height();
		int SetX; int SetY;

		if (dlg == nullptr)
		{
			int offsetY = 41;
			SetX = MEView_bR.x() - dlgW;
			SetY = offsetY + BtnSculpt_tl.y() + 5;
		}
		else
		{
			SetX = MEView_bR.x() - dlgW - btn->geometry().width();
			SetY = BtnSculpt_tl.y();
		}

		QString basicVal;
		QString AreaVal = WIN_MANAGER->getConfigValue(ELEMENT_FILE, STRING_MANAGER->config_SegDockArea, basicVal) == true ? basicVal : QString::number(Qt::DockWidgetArea::RightDockWidgetArea);
		Qt::DockWidgetArea area = (Qt::DockWidgetArea)(AreaVal.toInt());

		if (area == Qt::LeftDockWidgetArea || fixed == _Left)
		{
			SetX = MEView_tl.x() + 1;
			SetY = MEView_tl.y();
		}

		if (MEView_tl.x() > SetX)
			getMeshDialog()->setWindowOpacity(0.0);
		else
			getMeshDialog()->setWindowOpacity(1.0);

		DialogRect.setX(SetX);
		DialogRect.setY(SetY);
		DialogRect.setWidth(dlgW);
		DialogRect.setHeight(dlgH);

		getMeshDialog()->setGeometry(DialogRect);
	}
}

int  CMeshDlgManager::getSculptMode()
{
	return m_pSculptDlg->getSculptMode();
}
