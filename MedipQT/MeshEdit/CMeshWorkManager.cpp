#include "stdafx.h"
#include "CMeshViewRenderManager.h"
#include "CMeshModelViewManager.h"
#include "CMeshDistancMananager.h"
#include "CMeshCutManager.h"
#include "CMeshDlgManager.h"
#include "CMeshHoleFillManager.h"
#include "CMeshViewBtn3DScene.h"
#include "CMeshWorkManager.h"
#include "CMeshViewRightClickEvent.h"
#include "CManipulator.h"
#include "CMeshManipulator.h"

#include "windowManager.h"
#include "MedipQT.h"

#include "Main/MainMeshWidget.h"
#include "MEVolumeView.h"
#include "ActionManager.h"

/*
@breif
*/
CMeshWorkManager::CMeshWorkManager() :
	m_pRenderer(nullptr)
{
	m_mouseMove = m_RbuttonDown = m_LbuttonDown = m_MbuttonDown = false;

	m_WorkMode = MESH_WORK_NONE;
	m_PrevWorkMode = MESH_WORK_NONE;

	m_clickBox.setCoords(0, 0, 0, 0);

	m_pDataContext = nullptr;
}

/*
@breif
*/
CMeshWorkManager::~CMeshWorkManager()
{

}

/*
@brief
@return
*/
CMeshWorkManager* CMeshWorkManager::getInstance()
{
	static CMeshWorkManager instance;
	return &instance;
}

/*
@breif
@return
*/
void CMeshWorkManager::setWorkMode(MESH_WORK_MODE _mode)
{
	m_PrevWorkMode = m_WorkMode;

	m_WorkMode = _mode;
}

/*
@breif
@return
*/
void CMeshWorkManager::Init(DataContext* pDataContext, MEVolumeView* pViewer)
{
	Init(
		g_Renderer,
		pDataContext,
		WIN_MANAGER,
		ACTION_MANAGER,
		pViewer,
		MESH_MODELVIEW_MANAGER,
		MESH_RENDER_MANAGER,
		MESH_BTN_SCENE_MANAGER,
		MESH_CUT_MANAGER,
		MESH_HOLE_MANAGER,
		MESH_DIALOG_MANAGER,
		MESH_MANIPULATOR,
		PLANE_MANIPULATOR,
		MESH_DISTANCE_MANAGER,
		MESH_RClick_EVENT_MANAGER
	);
}

void CMeshWorkManager::Init(
	mip::Renderer* pRenderer,
	DataContext* pDataContext,
	WindowManager* pWinManager,
	ActionManager* pActionManager,
	MEVolumeView* pViewer,
	CMeshModelViewManager* pModelViewManager,
	CMeshViewRenderManager* pRenderManager,
	CMeshViewBtn3DScene* pBtn3DScene,
	CMeshCutManager* pMeshCutManager,
	CMeshHoleFillManager* pHoleFillManager,
	CMeshDlgManager* pDlgManager,
	CMeshManipulator* pMeshManipulator,
	CPlaneManiplator* pPlaneManipulator,
	CMeshDistancMananager* pDistanceManager,
	CMeshViewRightClickEvent* pRClickEvent)
{
	m_pRenderer = pRenderer;
	m_pDataContext = pDataContext;

	m_pModelViewManager = pModelViewManager;
	m_pRenderManager = pRenderManager;
	m_pBtn3DScene = pBtn3DScene;
	m_pMeshCutManager = pMeshCutManager;
	m_pHoleFillManager = pHoleFillManager;
	m_pDlgManager = pDlgManager;
	m_pMeshManipulator = pMeshManipulator;
	m_pPlaneManipulator = pPlaneManipulator;
	m_pDistanceManager = pDistanceManager;
	m_pRClickEvent = pRClickEvent;

	m_pModelViewManager->Init(
		m_pDataContext,
		m_pBtn3DScene,
		this,
		m_pMeshManipulator,
		m_pPlaneManipulator,
		pActionManager,
		pWinManager);
	m_pModelViewManager->SetView(pViewer);

	m_pRenderManager->Init(
		m_pRenderer,
		m_pDataContext,
		pViewer,
		this,
		m_pModelViewManager,
		m_pDlgManager,
		m_pMeshCutManager,
		m_pPlaneManipulator,
		m_pHoleFillManager,
		m_pMeshManipulator);

	m_pBtn3DScene->Init(
		m_pDataContext,
		pWinManager,
		pViewer,
		m_pMeshCutManager,
		m_pDlgManager,
		this,
		m_pPlaneManipulator,
		m_pModelViewManager,
		m_pDistanceManager,
		m_pHoleFillManager
	);

	m_pMeshCutManager->Init(
		m_pDataContext,
		pViewer,
		m_pBtn3DScene,
		this,
		m_pModelViewManager,
		m_pDlgManager,
		m_pPlaneManipulator);

	m_pHoleFillManager->Init(
		m_pDataContext,
		pViewer,
		m_pBtn3DScene,
		m_pModelViewManager,
		m_pDlgManager);

	m_pDlgManager->Init(
		m_pDataContext,
		pViewer,
		this,
		m_pBtn3DScene
	);

	m_pMeshManipulator->Init(
		m_pDataContext,
		this,
		m_pModelViewManager,
		m_pDlgManager);

	m_pPlaneManipulator->Init(
		m_pDataContext,
		m_pModelViewManager,
		m_pDlgManager);

	m_pDistanceManager->Init(
		m_pDataContext,
		this,
		m_pMeshCutManager,
		m_pBtn3DScene);
}

void CMeshWorkManager::SetMeshManipulator(CMeshManipulator* pMeshManipulator)
{
	m_pMeshManipulator = pMeshManipulator;
}

CMeshManipulator* CMeshWorkManager::GetMeshManipulator() const
{
	return m_pMeshManipulator;
}

void CMeshWorkManager::SetPlaneManipulator(CPlaneManiplator* pPlaneManipulator)
{
	m_pPlaneManipulator = pPlaneManipulator;
}

CPlaneManiplator* CMeshWorkManager::GetPlaneManipulator() const
{
	return m_pPlaneManipulator;
}

void CMeshWorkManager::SetMeshDlgManager(CMeshDlgManager* pMeshDlgManager)
{
	m_pDlgManager = pMeshDlgManager;
}

CMeshDlgManager* CMeshWorkManager::GetMeshDlgManager() const
{
	return m_pDlgManager;
}

void CMeshWorkManager::SetModelView(CMeshModelViewManager* pMeshModelView)
{
	m_pModelViewManager = pMeshModelView;
}

CMeshModelViewManager* CMeshWorkManager::GetModelView() const
{
	return m_pModelViewManager;
}

void CMeshWorkManager::SetRenderer(CMeshViewRenderManager* pMeshRenderer)
{
	m_pRenderManager = pMeshRenderer;
}

CMeshViewRenderManager* CMeshWorkManager::GetRenderer() const
{
	return m_pRenderManager;
}

void CMeshWorkManager::SetMeshBtn3DScene(CMeshViewBtn3DScene* pBtn3DScene)
{
	m_pBtn3DScene = pBtn3DScene;
}

CMeshViewBtn3DScene* CMeshWorkManager::GetMeshBtn3DScene() const
{
	return m_pBtn3DScene;
}

void CMeshWorkManager::SetMeshHoleFillManager(CMeshHoleFillManager* pMeshHoleFillManager)
{
	m_pHoleFillManager = pMeshHoleFillManager;
}

CMeshHoleFillManager* CMeshWorkManager::GetMeshHoleFillManager() const
{
	return m_pHoleFillManager;
}

void CMeshWorkManager::SetMeshCutManager(CMeshCutManager* pMeshCutManager)
{
	m_pMeshCutManager = pMeshCutManager;
}

CMeshCutManager* CMeshWorkManager::GetMeshCutManager() const
{
	return m_pMeshCutManager;
}

/*
@breif
@return
*/
MESH_WORK_MODE CMeshWorkManager::getWorkMode()
{
	return m_WorkMode;
}

/*
@breif
@return
*/
MESH_WORK_MODE CMeshWorkManager::getPrevWorkMode()
{
	return m_PrevWorkMode;
}


/*
@breif
@return
*/
void CMeshWorkManager::setKeyBtnPtr(
	bool* _p_ctrl,
	bool* _p_alt,
	bool* _p_shift
)
{
	m_p_Ctrl = _p_ctrl;
	m_p_Alt = _p_alt;
	m_p_Shift = _p_shift;
}

/*
@breif
@return
*/
void CMeshWorkManager::resetList()
{
	WIN_MANAGER->makeCurrent();

	//*(m_pModelViewManager->getMeshpckIDPtr()) = -1;

	if (this->getWorkMode() >= MESH_WORK_FOR_SINGLE)
	{
		this->UpdateWorkMode(MESH_WORK_NONE, true);
	}

	WIN_MANAGER->doneCurrent();
}

/*
@breif
@return
*/
void CMeshWorkManager::UpdateWorkMode(MESH_WORK_MODE _mode, bool iconRefresh)
{
	if (m_pDataContext == nullptr)
	{
		return;
	}

	int pckId = m_pDataContext->m_MeshData.GetCurrentMeshIndex();

	//if (*(m_pModelViewManager->getMeshpckIDPtr()) == -1 && m_WorkMode >= MESH_WORK_FOR_SINGLE)
	if (pckId == -1 && m_WorkMode >= MESH_WORK_FOR_SINGLE)
	{
		setWorkMode(MESH_WORK_NONE);
	}

	UpdateBtn(_mode, MESH_WORK_BRUSH_SCULPT);
	UpdateBtn(_mode, MESH_WORK_BRUSH_SELECTION);


	m_PrevWorkMode = m_WorkMode;

	if (m_PrevWorkMode != _mode)
	{
		setWorkMode(_mode);

		//if (_mode != MESH_WORK_PLANE_CUT)
		//{
		// //m_pDlgManager->rejectDialog(MESH_DIALOG_PLANECUT);
		// m_pDlgManager->rejectDialog(MESH_DIALOG_MESHCUT);

		// setWorkMode(_mode);
		//} 
	}

	m_pDistanceManager->Update(_mode, iconRefresh);

	m_pMeshCutManager->Update(_mode);

	m_pHoleFillManager->Update(_mode);
}

/*
@breif
@return
*/
void CMeshWorkManager::UpdateWorkMode(MESH_REMESH_TYPE _mode, bool iconRefresh)
{
	MESH_WORK_MODE mode;

	switch (_mode)
	{
	case MESH_SOLID:
		mode = MESH_WORK_MODE::MESH_WORK_SOLID;
		break;
	case MESH_HOLLOW:
		mode = MESH_WORK_MODE::MESH_WORK_HOLLOW;
		break;
	case MESH_REMESH:
		mode = MESH_WORK_MODE::MESH_WORK_REMESH;
		break;
	case MESH_REDUCTION:
		mode = MESH_WORK_MODE::MESH_WORK_REDUCTION;
		break;
	case MESH_SMOOTH:
		mode = MESH_WORK_MODE::MESH_WORK_SMOOTH;
		break;
	case MESH_SUBDIVISION:
		mode = MESH_WORK_MODE::MESH_WORK_SUBDIVISION;
		break;
	case MESH_ISLAND_FILTER:
		mode = MESH_WORK_MODE::MESH_WORK_ISLANDFILTER;
		break;
	}

	UpdateWorkMode(mode, iconRefresh);
}

/*
@breif
@return
*/
void CMeshWorkManager::UpdateUndoRedo(bool _b_write)
{

	switch (m_WorkMode)
	{
	case MESH_WORK_BRUSH_SCULPT:
	{
		//20210608_byPHS
		//WIN_MANAGER->mainMeshWidget->getMainView()->readyBrush(MESH_WORK_BRUSH_SCULPT);
	}
	break;
	case MESH_WORK_HOLE_FILL_SELECTED:
	{
		if (!_b_write)
		{
			m_pHoleFillManager->Process(MESH_WORK_FIND_HOLE);
		}
	}
	//case MESH_WORK_MANIFULATE:
	//{
	// if (!_b_write)
	// {
	// m_pModelViewManager->OnReCalcZero();
	// }
	//}
	//break;
	}
}

void CMeshWorkManager::UpdateBtn(MESH_WORK_MODE CurMode, MESH_WORK_MODE _mode)
{
	QPushButton* pBtn = nullptr;
	QIcon Icon[2];
	int sz_icon = WIN_MANAGER->mainWindow->IconSize;
	switch (_mode)
	{
	case MESH_WORK_BRUSH_SCULPT:

		Icon[0] = RESOURCE_MANAGER->getIcon(ICON_NON_BRUSH_SCULPT, sz_icon, sz_icon);
		Icon[1] = RESOURCE_MANAGER->getIcon(ICON_BRUSH_SCULPT, sz_icon, sz_icon);
		pBtn = m_pBtn3DScene->m_BtnSculpt;
		break;
	case MESH_WORK_BRUSH_SELECTION:
		Icon[0] = RESOURCE_MANAGER->getIcon(ICON_NON_BRUSH_SELECT, sz_icon, sz_icon);
		Icon[1] = RESOURCE_MANAGER->getIcon(ICON_BRUSH_SELECT, sz_icon, sz_icon);
		pBtn = m_pBtn3DScene->m_BtnSelect;
		break;
	case MESH_WORK_STAMP3D:
		Icon[0] = RESOURCE_MANAGER->getIcon(ICON_NON_BRUSH_SELECT, sz_icon, sz_icon);
		Icon[1] = RESOURCE_MANAGER->getIcon(ICON_BRUSH_SELECT, sz_icon, sz_icon);
		pBtn = m_pBtn3DScene->m_BtnStamp3d;
		break;
	case MESH_WORK_SHADER:
		Icon[0] = RESOURCE_MANAGER->getIcon(ICON_NON_BRUSH_SELECT, sz_icon, sz_icon);
		Icon[1] = RESOURCE_MANAGER->getIcon(ICON_BRUSH_SELECT, sz_icon, sz_icon);
		pBtn = m_pBtn3DScene->m_BtnShader;
		break;
	case MESH_WORK_NONE:
		break;
	default:
		break;
	}

	if ((_mode == CurMode))
	{
		pBtn->setChecked(true);
		pBtn->setIcon(Icon[1]);
	}
	else
	{
		pBtn->setChecked(false);
		pBtn->setIcon(Icon[0]);
	}

}

/*
@breif
@return
*/
void CMeshWorkManager::processkeyPress(QKeyEvent* e)
{

}

/*
@breif
@return
*/
void CMeshWorkManager::processkeyRelease(QKeyEvent* e)
{
	const int _modifier = e->key();

	switch (_modifier)
	{
		// case Qt::Key_A :
		// {
		//#ifdef TEMP_SELECT_CODE
		// if ((_modifier == Qt::Key_A) && *m_p_Ctrl)
		// {
		// if (WIN_MANAGER->getMeshWorkMode() == MESH_WORK_MODE::MESH_WORK_BRUSH_SELECTION && selectMesh != nullptr)
		// {
		// for (int ii = 0; ii < selectMesh->m_ttris.size(); ii++)
		// {
		// selectMesh->m_ttris[ii].setS();
		// }
		//
		// int cnt = 0;
		// for (int ii = 0; ii < selectMesh->m_ttris.size(); ii++)
		// {
		// if (selectMesh->m_ttris[ii].isS())
		// cnt++;
		// }
		//
		// printf_s("\n All Select - %d", cnt);
		// }
		// else
		// {
		// updatePckMeshAll();
		// }
		// }
		//#endif
		// }
		break;
	case Qt::Key_B: m_pBtn3DScene->OnSculpt(); break;
	case Qt::Key_L: m_pModelViewManager->MeshLeftView(); break;
	case Qt::Key_R: m_pModelViewManager->MeshRightView(); break;
	case Qt::Key_S: m_pModelViewManager->MeshTopView(); break;
	case Qt::Key_I: m_pModelViewManager->MeshBottomView(); break;
	case Qt::Key_A:
	{
		if (!(*m_p_Ctrl))
		{
			m_pModelViewManager->MeshFrontView();
		}
	}
	break;
	case Qt::Key_P: m_pModelViewManager->MeshBackView(); break;
	case Qt::Key_W: m_pRenderManager->OnWireFrame(); break;
	case Qt::Key_T:
	{
		// 210324 허건 과장
		//if (m_pModelViewManager->pckMeshModelCount() >= 1)
		if (m_pDataContext->m_MeshData.GetSelectMeshCount() == 1)
		{
			//setWorkMode((m_WorkMode != MESH_WORK_MANIFULATE) ? MESH_WORK_MANIFULATE : MESH_WORK_NONE);

			if (m_WorkMode != MESH_WORK_MANIFULATE)
			{
				//setWorkMode(MESH_WORK_MANIFULATE);
				this->UpdateWorkMode(MESH_WORK_MANIFULATE);

				int n_mesh = m_pDataContext->m_MeshData.GetMeshCount();
				for (int idx = 0; idx < n_mesh; ++idx)
				{
					MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(idx);

					//if (WIN_MANAGER->vt_pckID[idx])
					if (pMeshInfo && pMeshInfo->selected)
					{
						m_pMeshManipulator->UpdatePrevTransform(idx);
						m_pMeshManipulator->UpdateInitTransform(idx);
					}
				}

				ACTION_MANAGER->action_UndoRedo_update(m_pDataContext, m_WorkMode, m_PrevWorkMode);

				//m_pDlgManager->closeDialog();

				//if (WIN_MANAGER->mainTabType == MAINTAB_MESH_EDITING)
				//{
				// m_pDlgManager->makeMeshDialog(m_WorkMode);
				//}

				CManpulateDlg* dlg = static_cast<CManpulateDlg*>(m_pDlgManager->getMeshDialog(MESH_WORK_MANIFULATE));
				for (int idx = 0; idx < n_mesh; ++idx)
				{
					MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(idx);

					//if (WIN_MANAGER->vt_pckID[idx])
					if (pMeshInfo && pMeshInfo->selected)
					{
						auto mesh = m_pDataContext->m_MeshData.GetMesh(idx);

						if (mesh && dlg)
						{
							dlg->UpdateMeshInfo(mesh, NONE_AXIS);
						}
					}
				}

				mip::MeshTopology* mesh = m_pDataContext->m_MeshData.GetCurrentMesh();

				if (mesh)
				{
					m_pMeshManipulator->UpdatePosition(mesh, false);
					int mesh_idx = m_pDataContext->m_MeshData.GetCurrentMeshIndex();

					//WIN_MANAGER->vt_pckID[mesh_idx] = true;
					m_pDataContext->m_MeshData.SetMeshInfoModeSelectMode(mesh_idx, true);
				}
			}
			else
			{
				m_pDlgManager->rejectDialog();
			}
		}
		else
		{
			m_pDlgManager->rejectDialog();

			auto veiwMesh = WIN_MANAGER->mainMeshWidget->getMainView();
			QMessageBox::warning(veiwMesh, QString("Action for mesh"), QString("Pick the mesh to apply this function."));
		}

#ifdef TEMP_SELECT_CODE
		//2020_11_19_byPHS_일정관계로주석
		int nSelectedCnt = 0;
		for (int ii = 0; ii < WIN_MANAGER->vt_pckID.size(); ii++)
		{
			if (WIN_MANAGER->vt_pckID[ii]) nSelectedCnt++;
		}

		if (*(m_pModelViewManager->getMeshpckIDPtr()) == -1)// || (nSelectedCnt != 1 && (WIN_MANAGER->getMeshWorkMode() == MESH_WORK_BRUSH_SELECTION || WIN_MANAGER->getMeshWorkMode() == MESH_WORK_BRUSH_SCULPT)))
		{
			setWorkMode(MESH_WORK_NONE, true);
			QMessageBox::warning(this, QString("Action for single mesh"), QString("Pick the mesh to apply this function."));
			return;
		}

		makeMeshDialog(MESH_WORK_SMOOTH);
		//readyBrush(MESH_WORK_BRUSH_SELECTION);
#endif
	}
	break;
	case Qt::Key_C:
	{
#ifdef TEMP_SELECT_CODE
		if (WIN_MANAGER->getMeshWorkMode() == MESH_WORK_MODE::MESH_WORK_BRUSH_SELECTION && selectMesh != nullptr)
		{
			int cnt = 0;
			for (int ii = 0; ii < selectMesh->m_ttris.size(); ii++)
			{
				if (selectMesh->m_ttris[ii].isS())
					cnt++;
			}

			//printf_s("\n select flag - %d", cnt);

			//int reduceOp = 1; // 0 - percentage, 1 - triangle count, 2 - Max Deviation
			//mip::mesh_control::select_reduce(selectMesh, mip::SELECTED, 100, 0, true);

			//int smoothOp = 3; // 0 - Laplacian, 1 - HC_LAPLACIAN, 2 - taubin, 3 - Inflate
			mip::mesh_control::select_smooth(selectMesh, mip::SELECTED, 0, 1, 100);


			selectMesh->updateVertex();

			g_Renderer->makeCurrent();
			selectMesh->buildRenderBufferTopology();
			g_Renderer->doneCurrent();
		}
#endif
	}
	break;
	case Qt::Key_Delete:
	{
		//if (this->getWorkMode() == MESH_WORK_MODE::MESH_WORK_BRUSH_SELECTION)
		//{
		// auto view = WIN_MANAGER->mainMeshWidget->getMainView();
		// view->deleteSelectFlag();
		// //ACTION_MANAGER->action_UndoRedo_update(MESH_WORK_BRUSH_SELECTION);
		// 
		//}
	}
	break;
	case Qt::Key_Escape: m_pDlgManager->rejectDialog(); break;
	case Qt::Key_Shift:
	{
		m_pModelViewManager->UpdatePivotPoint(m_pDataContext->m_MeshData.GetCurrentMeshIndex());
	}
	break;
	default: break;
	}
}

/*
@breif
@return
*/
bool CMeshWorkManager::processMouseMove(QMouseEvent* e, mip::SCAMERA* pCamera)
{
	m_mouseMove = false;

	m_preMousePos = m_MousePos;
	m_MousePos = e->pos();

	m_clickBox.setCoords(m_MousePos.x() - 5, m_MousePos.y() - 5,
		m_MousePos.x() + 5, m_MousePos.y() + 5
	);

	if (!m_mouseMove)
	{
		if (m_clickBox.width() != 0)
		{
			if (!m_clickBox.contains(m_MousePos))
			{
				m_mouseMove = true;
			}
		}
	}

	pCamera->setScreenXY(m_MousePos.x(), m_MousePos.y());

	bool b_udpate = processMouseMove(pCamera);

	pCamera->setPreScreenXY(m_preMousePos.x(), m_preMousePos.y());

	return b_udpate;
}

/*
@breif
@return
*/
void CMeshWorkManager::processMousePress(QMouseEvent* e)
{
	m_MousePos = e->pos();
	m_mouseMove = false;

	m_clickBox.setCoords(
		m_MousePos.x() - 5, m_MousePos.y() - 5,
		m_MousePos.x() + 5, m_MousePos.y() + 5
	);

	if (e->buttons() & Qt::RightButton) m_RbuttonDown = true;
	else if (e->buttons() & Qt::LeftButton) m_LbuttonDown = true;
	else if (e->buttons() & Qt::MidButton) m_MbuttonDown = true;

	switch (m_WorkMode)
	{
	case MESH_WORK_POLYGON_CUT:
	case MESH_WORK_POLYLINE_CUT:
	case MESH_WORK_FREEPOLYLINE_CUT:
	case MESH_WORK_PLANE_CUT:
	{
		if (ACTION_MANAGER->isActionFinished())
		{
			if (m_LbuttonDown && *m_p_Ctrl && (m_WorkMode != MESH_WORK_PLANE_CUT))
			{
				auto p_polyline = m_pMeshCutManager->getPolyLinePtr();
				p_polyline->push_back(QPoint(m_MousePos.x(), m_MousePos.y()));
			}
			//else if (!*m_p_Ctrl && !*m_p_Shift && m_LbuttonDown) //pckmesh
			//else if (!*m_p_Ctrl && m_LbuttonDown) //pckmesh
			//{
			// m_pModelViewManager->pckMeshModel(e, m_MousePos, *m_p_Shift);
			//}
			if (m_WorkMode == MESH_WORK_PLANE_CUT && m_pPlaneManipulator->CheckClickedArrow(m_pDataContext, m_MousePos, m_pModelViewManager->GetCameraPtr()))
			{

				mip::MATRIX44 mat_offset;
				mip::VECTOR3 offset_center(-m_pDataContext->volume_data.getSizeX() * 0.5f, -m_pDataContext->volume_data.getSizeY() * 0.5f, -m_pDataContext->volume_data.getSizeZ() * 0.5f);

				mat_offset.identity();
				mat_offset.translation(offset_center);

				m_pPlaneManipulator->ReadyProcess(
					m_MousePos,
					m_preMousePos,
					m_pModelViewManager->GetMainTransform(),
					mat_offset,
					m_pModelViewManager->GetCameraPtr()
				);

				MEVolumeView* view = WIN_MANAGER->mainMeshWidget->getMainView();
				view->renderLater();
			}
		}
	}
	break;
	// case MESH_WORK_BRUSH_SCULPT:
	// {
	// if (*m_p_Ctrl || *m_p_Shift)
	// {
	// ACTION_MANAGER->action_UndoRedo_update(MESH_WORK_BRUSH_SCULPT);
	//
	// auto dlg = static_cast<BrushSculptDlg*>(m_pDlgManager->getMeshDialog(MESH_DIALOG_SCULPT));
	//
	// int mode = dlg->getSculptMode();
	//#ifdef TEMP_BRUSH_CODE
	// mode = SM_MOVE;
	//#endif
	// if (dlg != nullptr && mode == SM_MOVE)
	// {
	// int _press = 0;
	// process_Sculpt_Move(_press);
	// }
	// }
	// }
	// break;
	case MESH_WORK_NONE:
		if (!*m_p_Ctrl && m_LbuttonDown) //pckmesh
		{
			m_pModelViewManager->pckMeshModel(e, m_MousePos, *m_p_Shift);
		}
		break;
	case MESH_WORK_MANIFULATE:
	{
		if (m_pMeshManipulator->CheckClickedArrow(m_MousePos, m_pModelViewManager->GetCameraPtr()))
		{
			mip::MATRIX44 mat_offset;
			mip::VECTOR3 offset_center(-m_pDataContext->volume_data.getSizeX() * 0.5f, -m_pDataContext->volume_data.getSizeY() * 0.5f, -m_pDataContext->volume_data.getSizeZ() * 0.5f);

			mat_offset.identity();
			mat_offset.translation(offset_center);

			int n_mesh = m_pDataContext->m_MeshData.GetMeshCount();

			for (int idx = 0; idx < n_mesh; ++idx)
			{
				MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(idx);

				//if (WIN_MANAGER->vt_pckID[idx])
				if (pMeshInfo && pMeshInfo->selected)
				{
					m_pMeshManipulator->ReadyProcess(
						idx,
						m_MousePos,
						m_preMousePos,
						m_pModelViewManager->GetMainTransform(),
						mat_offset,
						m_pModelViewManager->GetCameraPtr()
					);
				}
			}

			MEVolumeView* view = WIN_MANAGER->mainMeshWidget->getMainView();
			view->renderLater();
		}
	}
	break;
	}
}

/*
@breif
@return
*/
bool CMeshWorkManager::processMouseRelease(QMouseEvent* e)
{
	bool b_need_update = false;

	bool b_objTrans = (m_WorkMode == MESH_WORK_NONE);

	switch (m_WorkMode)
	{
	case MESH_WORK_MANIFULATE:
	{
		m_pMeshManipulator->CheckClickedArrow(m_MousePos, m_pModelViewManager->GetCameraPtr());

		m_preMousePos = m_MousePos;

		m_pModelViewManager->GetCameraPtr()->setPreScreenXY(m_preMousePos.x(), m_preMousePos.y());

		m_RbuttonDown = false;
		m_LbuttonDown = false;
		m_MbuttonDown = false;

		int n_mesh = m_pDataContext->m_MeshData.GetMeshCount();
		int cnt = 0;
		for (int i = 0; i < n_mesh; ++i)
		{
			MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(i);

			//if (WIN_MANAGER->vt_pckID[i])
			if (pMeshInfo && pMeshInfo->selected)
			{
				auto mesh = m_pDataContext->m_MeshData.GetMesh(i);

				if (mesh)
				{
					if (m_pMeshManipulator->FinishProcess(i))
					{
						cnt++;
					}
				}
			}
		}

		if (cnt > 0)
		{
			m_pModelViewManager->UpdatePivotPoint();

			ACTION_MANAGER->action_UndoRedo_update(m_pDataContext, MESH_WORK_MANIFULATE, MESH_WORK_MANIFULATE);
		}

		return true;
	}
	break;
	case MESH_WORK_PLANE_CUT:
	{
		m_pPlaneManipulator->CheckClickedArrow(m_pDataContext, m_MousePos, m_pModelViewManager->GetCameraPtr());

		m_preMousePos = m_MousePos;

		m_pModelViewManager->GetCameraPtr()->setPreScreenXY(m_preMousePos.x(), m_preMousePos.y());

		m_RbuttonDown = false;
		m_LbuttonDown = false;
		m_MbuttonDown = false;

		int n_mesh = m_pDataContext->m_MeshData.GetMeshCount();

		if (n_mesh > 0)
		{
			//auto mesh = m_pModelViewManager->getMeshPckPtr();
			mip::MeshTopology* mesh = m_pDataContext->m_MeshData.GetCurrentMesh();

			if (mesh)
			{
				if (m_pPlaneManipulator->FinishProcess(mesh))
				{
					m_pModelViewManager->UpdatePivotPoint();
				}
			}
		}

		return true;
	}
	break;
	}

	b_objTrans = true;

	if (b_objTrans)
	{
		if (m_RbuttonDown)
		{
			m_pRClickEvent->setContextMenu();
			auto context = m_pRClickEvent->getContextMenuPtr();
			context->exec(e->globalPos());
		}

		else if (m_LbuttonDown && *m_p_Ctrl)
		{
			switch (m_WorkMode)
			{
				//case MESH_WORK_PLANE_CUT:
			case MESH_WORK_FREEPOLYLINE_CUT:
			case MESH_WORK_POLYLINE_CUT:
			case MESH_WORK_POLYGON_CUT:
			{
				b_need_update = m_pMeshCutManager->Process(m_WorkMode);
			}
			break;
			// case MESH_WORK_BRUSH_SELECTION:
			// {
			// //
			// }
			// case MESH_WORK_BRUSH_SCULPT:
			// {
			// auto dlg = static_cast<BrushSculptDlg*>(m_pDlgManager->getMeshDialog(MESH_DIALOG_SCULPT));
			//
			// int mode = dlg->getSculptMode();
			//#ifdef TEMP_BRUSH_CODE
			// mode = SM_MOVE;
			//#endif
			// if (dlg != nullptr && mode == SM_MOVE)
			// {
			// int release = 2;
			// process_Sculpt_Move(release);
			// }
			//
			// int vertCnt = 0; int triCnt = 0;
			// getGeometryCount(vertCnt, triCnt);
			// dlg->setVertTriCnt(vertCnt, triCnt);
			// }
			// break;
			}
		}

		else if (m_LbuttonDown && *m_p_Shift)
		{
			//switch (m_WorkMode)
			//{
			//case MESH_WORK_BRUSH_SCULPT:
			//{
			// if (m_pDlgManager->isMeshDialog())
			// {
			// //Bounding Box 계산
			// int nPickMesh = pDataContext->m_MeshData.GetCurrentMeshIndex();
			// MeshInfo* info = pDataContext->m_MeshData.GetMeshInfo(nPickMesh);
			// mip::MeshTopology *m = pDataContext->m_MeshData.GetMesh(nPickMesh);

			// if (m)
			// {
			// mip::AABB box;
			// mip::mesh_control::getMinMax(m->m_verts, box.min, box.max);
			// m->m_boundingBox = box;
			// }
			// }
			//}
			//break;
			//}
		}
		else if (m_LbuttonDown)
		{
			if (m_WorkMode == MESH_WORK_HOLE_FILL_SELECTED)
			{
				m_pHoleFillManager->Process(MESH_WORK_HOLE_FILL_SELECTED);
			}
		}

	}

	m_preMousePos = m_MousePos;

	m_pModelViewManager->GetCameraPtr()->setPreScreenXY(m_preMousePos.x(), m_preMousePos.y());

	m_RbuttonDown = false;
	m_LbuttonDown = false;
	m_MbuttonDown = false;

	return b_need_update;
}

/*
@breif
@return
*/
bool CMeshWorkManager::processMouseDoubleClicked(QMouseEvent* e)
{
	bool b_need_update = false;

	//if (e->buttons() & Qt::LeftButton)
	//{
	// if (ACTION_MANAGER->isActionFinished())
	// {
	// if (*m_p_Ctrl)
	// {
	// auto mode = this->getWorkMode();

	// switch (mode)
	// {
	// case MESH_WORK_POLYLINE_CUT:
	// case MESH_WORK_POLYGON_CUT:
	// case MESH_WORK_FREEPOLYLINE_CUT:
	// case MESH_WORK_PLANE_CUT:
	// {
	// b_need_update = m_pMeshCutManager->process_FinishCut();

	// this->UpdateWorkMode(MESH_WORK_NONE, true);
	// }
	// break;
	// }
	// }
	// else // 메시모델 클릭 시, 화면중심으로 이동(Draw만 해당)
	// {
	// //MoveScreenCenterMesh();

	// //b_need_update = true;
	// }
	// }
	//}
	//else if (e->buttons() && Qt::MiddleButton)
	//{
	// if (ACTION_MANAGER->isActionFinished())
	// {
	// auto p_polyline = m_pMeshCutManager->getPolyLinePtr();
	// p_polyline->clear();

	// if (*m_p_Ctrl)
	// {
	// b_need_update = m_pMeshCutManager->process_CancelCut();
	// }
	// }
	//}

	return b_need_update;
}

/*
@breif
@return
*/
void CMeshWorkManager::ProcessWheel(QWheelEvent* event)
{
	bool planecut = m_WorkMode == MESH_WORK_PLANE_CUT;

	//if (planecut && (!(*m_p_Ctrl)))
	//{
	// mip::MATRIX44 matPlaneImage = m_pRenderManager->m_trPlane.getMatrix();
	// mip::VECTOR3 dir = matPlaneImage.getScaledZaxis();

	// m_pRenderManager->m_trPlane.addTranslate(dir * 0.001f * event->delta());
	//}
	//else
	//{
	// m_pModelViewManager->GetCameraPtr()->wheelZoom(event->delta() * 0.01f);
	//}

	m_pModelViewManager->GetCameraPtr()->wheelZoom(event->delta() * 0.01f);
}

/*
@breif
@return
*/
bool CMeshWorkManager::processMouseMove(mip::SCAMERA* pCamera)
{
	bool b_need_update = false;

	bool b_objTrans = (m_WorkMode == MESH_WORK_NONE);

	b_objTrans = ((m_pDataContext->m_MeshData.GetMeshCount() > 0) ? true : false);

	//#ifndef TEMP_CODE
	// if (!objTrans)
	// if (m_ctrl) objTrans = true;
	//
	// mip::MATRIX44 matVolume = m_trObject.getMatrix();
	// mip::VECTOR3 v1;
	// mip::VECTOR3 v2;
	//
	// mip::VECTOR3 vecOffset = getWorldCenter(false);
	// mip::MATRIX44 matOffset = mip::MATRIX44::Identity;
	// matOffset.translation(vecOffset.x, vecOffset.y, vecOffset.z);
	//
	// mip::MATRIX44 wvp = (matOffset*g_Renderer->getWorld()) * g_Renderer->getView() * g_Renderer->getProj();
	//
	// mip::MeshTopology *m = nullptr;
	//
	// if ((*(m_pModelViewManager->getMeshpckIDPtr()) != -1) && (mode >= MESH_WORK_FOR_SINGLE))
	// m = pDataContext->m_MeshData.GetMesh(*(m_pModelViewManager->getMeshpckIDPtr()));
	//
	// if (objTrans)
	// {
	// if (m_MbuttonDown)
	// {
	// v1 = m_pModelViewManager->GetCameraPtr()->getWorldPoint(0.5f);
	// v2 = m_pModelViewManager->GetCameraPtr()->getPreWorldPoint(0.5f);
	// // m_trObject.addTranslate((v1 - v2));
	// v1 -= v2;
	// v1.z = 0;
	// m_trObject.addTranslate(v1);
	//
	// need_update = true;
	// }
	//
	// else if (m_LbuttonDown)
	// {
	// TransformRotate(&m_trObject, &m_camera);
	// need_update = true;
	// }
	// }
	// else
	// {
	// switch (mode)
	// {
	// case MESH_WORK_PLANE_CUT:
	// {
	// if (m_MbuttonDown)
	// {
	// v1 = m_pModelViewManager->GetCameraPtr()->getWorldPoint(0, &matVolume);
	// v2 = m_pModelViewManager->GetCameraPtr()->getPreWorldPoint(0, &matVolume);
	//
	// m_trPlane.addTranslate((v1 - v2));
	//
	// need_update = true;
	// }
	// else if (m_LbuttonDown)
	// {
	// TransformRotate(&m_trPlane, &m_camera, &(matVolume));
	// need_update = true;
	// }
	// }
	// break;
	// case MESH_WORK_POLYGON_CUT:
	// if (m_polyLine.size() >= 3)
	// {
	// QPoint q = m_polyLine[0] - m_MousePos;
	//
	// if (q.manhattanLength() < 10)
	// {
	// if (!m_polyProcessCheck)
	// {
	// m_polyProcessCheck = true;
	// need_update = true;
	// }
	// }
	// else if (m_polyProcessCheck)
	// {
	// m_polyProcessCheck = false;
	// need_update = true;
	// }
	// }
	// break;
	// case MESH_WORK_POLYLINE_CUT:
	//
	// break;
	// case MESH_WORK_DISTANCE:
	//#ifdef MESH_TEST
	// if (m_pointList3D.size() >= 2)
	// {
	// mip::VECTOR3 point;
	// if (mip::mesh_control::pickMesh(mip::VECTOR2(m_MousePos.x(), m_MousePos.y()), mip::VECTOR2(this->width(), this->height()), wvp,
	// m->m_verts, m->m_tris, 1.0f, point) == 1)
	// {
	// if (pckIndex == -1)
	// {
	// float fDot;
	// for (int i = 0; i < 2; i++)
	// {
	// mip::VECTOR3 tmp = m_pointList3D.at(i) - point;
	//
	// if (abs(tmp.x) <= MIP_EPSILON3 &&
	// abs(tmp.y) <= MIP_EPSILON3 &&
	// abs(tmp.z) <= MIP_EPSILON3)
	// {
	// if (!m_pckProcessCheck)
	// {
	// need_update = m_pckProcessCheck = true;
	//
	// movIndex = i;
	//
	// break;
	// }
	//
	// }
	// else if (m_pckProcessCheck)
	// {
	// m_pckProcessCheck = false;
	// need_update = true;
	// movIndex = -1;
	// }
	// }
	// }
	// else
	// {
	// if (pckIndex == 0)
	// {
	// m_pointList3D.erase(m_pointList3D.begin());
	// m_pointList3D.insert(m_pointList3D.begin(), point);
	// }
	// else
	// {
	// m_pointList3D.pop_back();
	// m_pointList3D.push_back(point);
	// }
	// need_update = true;
	// }
	// }
	// }
	// break;
	//#endif
	// case MESH_WORK_INTERSECT://todo later
	// break;
	// case MESH_WORK_DIFF://todo later
	// break;
	// case MESH_WORK_UNION://todo later
	// break;
	// //case MESH_WORK_NONE:
	// //default:
	// // objTrans = true;
	// // break;
	// }
	// }
	//
	// if (need_update)
	// renderLater();
	//#else

	switch (m_WorkMode)
	{
	case MESH_WORK_MANIFULATE:
		if (!m_MbuttonDown && !m_RbuttonDown && !m_LbuttonDown)
		{
			m_pMeshManipulator->CheckClickedArrow(m_MousePos, m_pModelViewManager->GetCameraPtr());

			return true;
		}
		else if (m_LbuttonDown)
		{
			if (m_pMeshManipulator->isCheckedArrow())
			{
				int n_mesh = m_pDataContext->m_MeshData.GetMeshCount();

				//int pck_id = *(m_pModelViewManager->getMeshpckIDPtr());
				int pck_id = m_pDataContext->m_MeshData.GetCurrentMeshIndex();

				if (pck_id >= 0 && pck_id < n_mesh)
				{
					m_pMeshManipulator->Process(pck_id, m_preMousePos, m_MousePos);

					for (int idx = 0; idx < n_mesh; ++idx)
					{
						MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(idx);

						//if (pck_id == idx)
						if (pck_id == idx || !pMeshInfo)
						{
							continue;
						}

						if (pMeshInfo->selected)
						{
							m_pMeshManipulator->Process(idx, m_preMousePos, m_MousePos);
						}
					}
				}

				return true;
			}
		}
		break;
	case MESH_WORK_PLANE_CUT:
		if (!m_MbuttonDown && !m_RbuttonDown && !m_LbuttonDown)
		{
			m_pPlaneManipulator->CheckClickedArrow(m_pDataContext, m_MousePos, m_pModelViewManager->GetCameraPtr());

			return true;
		}
		else if (m_LbuttonDown)
		{
			if (m_pPlaneManipulator->isCheckedArrow())
			{
				if (0 < m_pDataContext->m_MeshData.GetMeshCount())
				{
					auto mesh = m_pPlaneManipulator->getPlaneMeshPtr();

					if (mesh)
					{
						m_pPlaneManipulator->Process(m_pDataContext, mesh, m_preMousePos, m_MousePos);
					}
				}

				return true;
			}
		}
		break;
	case MESH_WORK_POLYGON_CUT:
	case MESH_WORK_FREEPOLYLINE_CUT:
	case MESH_WORK_POLYLINE_CUT:
		if (!ACTION_MANAGER->isActionFinished())
		{
			return true;
		}
		break;
	}

	if (b_objTrans)
	{
		if (m_MbuttonDown)
		{
			b_need_update = m_pModelViewManager->translateMeshModel();
		}
#if SUPPORT_MESH_TOGGLE
		else if (m_LbuttonDown && /*!*m_p_Ctrl*/ !WIN_MANAGER->mainMeshWidget->getMainView()->enableCtrlToggleKey())
#else
		else if (m_LbuttonDown && !*m_p_Ctrl)
#endif
		{
			if (!m_pRenderManager->getFontControl()->getPick())
				b_need_update = m_pModelViewManager->rotateMeshModel(*m_p_Shift);
		}
	}

	auto p_polyline = m_pMeshCutManager->getPolyLinePtr();

	switch (m_WorkMode)
	{
	case MESH_WORK_PLANE_CUT:
	{
		if (m_LbuttonDown && *m_p_Ctrl)
		{
			//if (p_polyline->size() < 2)
			//{
			// p_polyline->push_back(m_MousePos);

			// b_need_update = true;
			//}

			//if (p_polyline->size() == 2)
			//{
			// p_polyline->back().setX(m_MousePos.x());
			// p_polyline->back().setY(m_MousePos.y());

			// b_need_update = true;
			//}
		}
	}
	break;
	case MESH_WORK_FREEPOLYLINE_CUT:
	{
		if (m_LbuttonDown && *m_p_Ctrl)
		{
			p_polyline->push_back(m_MousePos);
			b_need_update = true;
		}
	}
	break;
	case MESH_WORK_HOLE_FILL_SELECTED:
	{
		m_pHoleFillManager->CalcNearestBoundary(m_MousePos);
		b_need_update = true;
	}
	break;
	//case MESH_WORK_BRUSH_SELECTION:
	//{
	// if (m_pDlgManager->isMeshDialog())
	// {
	// if (m_LbuttonDown && *m_p_Ctrl)
	// {
	// process_Select();
	// need_update = true;
	// }
	// if (m_LbuttonDown && *m_p_Shift)
	// {
	// process_Select(1);
	// need_update = true;
	// }
	// else
	// {
	// moveSphere();
	// need_update = true;
	// }
	// }
	//}
	//break;
 // case MESH_WORK_BRUSH_SCULPT:
 // {
 // if (m_pDlgManager->isMeshDialog())
 // {
 // if (m_LbuttonDown && *m_p_Ctrl)
 // {
 // auto dlg = static_cast<BrushSculptDlg*>(m_pDlgManager->getMeshDialog(MESH_DIALOG_SCULPT));
 //
 // int mode = dlg->getSculptMode();
 //#ifdef TEMP_BRUSH_CODE
 // mode = SM_MOVE;
 //#endif
 // if (mode == SM_SMOOTH || mode == SM_INFLATE || mode == SM_REDUCE)
 // {
 // process_BrushSculpt();
 // }
 // else if (mode == SM_MOVE)
 // {
 // int _move = 1;
 // process_Sculpt_Move(_move);
 // }
 // need_update = true;
 // }
 // else
 // {
 // moveSphere();
 // need_update = true;
 // }
 // }
 // }
 // break;
	}

	return b_need_update;
}
