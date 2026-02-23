#include "stdafx.h"
#include "MEVolumeView.h"
#include "windowManager.h"

#include "Renderer/Renderer.h"
#include "Renderer/ShaderMeshData.h"
#include "Renderer/model.h"

#include "System/resourceManager.h"
#include "System/styleManager.h"
#include "System/stringManager.h"

#include "Windows/Main/MainMeshWidget.h"

#include "Actions/ActionManager.h"

#include "Dialogs/SizeDialog.h"

#include "MedipQT.h"
#include "CollapseDock.h"
#include "Tabwindow.h"

#include "MeshEdit/CMeshWorkManager.h"
#include "MeshEdit/CMeshDlgManager.h"
#include "MeshEdit/CMeshModelViewManager.h"
#include "MeshEdit/CMeshViewBtn3DScene.h"
#include "MeshEdit/CMeshViewRenderManager.h"

#include "MeshEdit/MeshEditScreenSelector.h"

#ifdef MESH_TEST
#endif

#define TEMP_CODE 
#define SUPPORT_SCULPT_MOVE 0
#define SUPPORT_SCULPT_REFINE 1
#define SUPPORT_SCULPT_DRAG 0
#define TEST_byPHS 0
#define TEST_byPHS_Octree 0

#include <ppl.h>

using namespace concurrency;

MEVolumeView::MEVolumeView(DataContext* pDataContext, QWidget* parent) :
	MEVolumeView(
		g_Renderer,
		WIN_MANAGER,
		ACTION_MANAGER,
		pDataContext)
{
}

MEVolumeView::MEVolumeView(
	mip::Renderer* pRenderer,
	WindowManager* pWinManager,
	ActionManager* pActionManager,
	DataContext* pDataContext,
	QWidget* parent) :
	OpenGLWidget(parent),
	m_pRenderer(pRenderer),
	m_pWinManager(pWinManager),
	m_pActionManager(pActionManager),
	m_pDataContext(pDataContext),
	m_pMeshWorkManager(nullptr),
	m_pMeshModelViewManager(nullptr),
	m_pMeshRenderManager(nullptr),
	m_pMeshDialogManager(nullptr),
	m_pMeshBtn3DScene(nullptr)
{
	m_RbuttonDown = m_LbuttonDown = m_MbuttonDown = false;

	m_touchZoomEvent = m_mouseMove = false;

	m_clickBox.setCoords(0, 0, 0, 0);

	m_sphereScale = 0.5f;

	m_vertCnt = 0;
	m_TriCnt = 0;
	m_oldIdx = -1;
	m_TotalSize = 0.0f;
	m_bUpdateGeometryCnt = true;
	m_CtrlToggle = false;
}

MEVolumeView::~MEVolumeView()
{
	if (m_pMeshWorkManager)
	{
		m_pMeshWorkManager->resetList();
	}
}

void MEVolumeView::SetModel(CMeshWorkManager* pMeshWorkManager)
{
	m_pMeshWorkManager = pMeshWorkManager;
	m_pMeshModelViewManager = m_pMeshWorkManager->GetModelView();
	m_pMeshRenderManager = m_pMeshWorkManager->GetRenderer();
	m_pMeshDialogManager = m_pMeshWorkManager->GetMeshDlgManager();
	m_pMeshBtn3DScene = m_pMeshWorkManager->GetMeshBtn3DScene();

#ifdef SUPPORT_MESH_TOGGLE
	m_pMeshWorkManager->setKeyBtnPtr(&m_CtrlToggle, &m_alt, &m_shift);
#else
	m_pMeshWorkManager->setKeyBtnPtr(&m_ctrl, &m_alt, &m_shift);
#endif

	m_pMeshEditSculpt = std::make_shared<MeshEditSculpt>(
		m_pRenderer,
		m_pActionManager,
		m_pWinManager,
		&m_pDataContext->m_MeshData, &m_pDataContext->volume_data,
		m_pMeshWorkManager, m_pMeshModelViewManager, m_pMeshDialogManager
		);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////			초기화 및 업데이트 설정부		////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MEVolumeView::reserveInit(HWND hwnd)
{
	m_hwnd = hwnd;
}

void MEVolumeView::resetUI()
{
	if (m_pMeshWorkManager == nullptr)
	{
		return;
	}

	if (m_pMeshWorkManager->getWorkMode() >= MESH_WORK_FOR_SINGLE)
	{
		m_pMeshWorkManager->UpdateWorkMode(MESH_WORK_NONE, true);
	}

	m_pMeshModelViewManager->Reset();

	m_pMeshRenderManager->Reset();

	renderLater();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////			Event 함수 구현부			////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool MEVolumeView::eventFilter(QObject* target, QEvent* e)
{
	if (m_pMeshBtn3DScene)
	{
		m_pMeshBtn3DScene->eventFilter(target, e);
	}

	return QWidget::eventFilter(target, e);
}

void MEVolumeView::resizeEvent(QResizeEvent* e)
{
	if (e == NULL)
	{
		return;
	}

	int width = e->size().width();
	int height = e->size().height();

	if (m_pMeshBtn3DScene)
	{
		m_pMeshBtn3DScene->updateScreenLeftMenu(width, height);
	}

	renderLater();
}

void MEVolumeView::keyPressEvent(QKeyEvent* e)
{
	checkModifiers(e);
}

void MEVolumeView::keyReleaseEvent(QKeyEvent* e)
{
	if (m_pMeshWorkManager == nullptr)
	{
		return;
	}
	checkModifiers(e, false);

	m_pMeshWorkManager->processkeyRelease(e);

	// brush
	{
		const int _modifier = e->key();

		if ((_modifier == Qt::Key_A) && m_ctrl)
		{
			if (m_pMeshWorkManager->getWorkMode() == MESH_WORK_MODE::MESH_WORK_BRUSH_SELECTION)
			{
				m_pActionManager->action_UndoRedo_update(m_pDataContext, MESH_WORK_BRUSH_SELECTION);
				allSelectFlag();
				updateSelectionUI();
			}
		}

		if ((_modifier == Qt::Key_Delete))
		{
			if (m_pMeshWorkManager->getWorkMode() == MESH_WORK_MODE::MESH_WORK_BRUSH_SELECTION)
			{
				m_pActionManager->action_UndoRedo_update(m_pDataContext, MESH_WORK_BRUSH_SELECTION);
				deleteSelectFlag();

				updateGeometryCount();

				BrushSelectDlg* dlg = static_cast<BrushSelectDlg*>(m_pMeshDialogManager->getMeshDialog());
				if (dlg != nullptr)
				{
					dlg->hideFuncBtn();
				}
			}
		}


		if ((_modifier == Qt::Key_BracketLeft))
		{
			if (m_pMeshDialogManager->isMeshDialog())
			{
				if (m_pMeshWorkManager->getWorkMode() == MESH_WORK_MODE::MESH_WORK_BRUSH_SCULPT || m_pMeshWorkManager->getWorkMode() == MESH_WORK_MODE::MESH_WORK_BRUSH_SELECTION)
				{
					QDialog* pDialog = m_pMeshDialogManager->getMeshDialog();
					if (m_pMeshWorkManager->getWorkMode() == MESH_WORK_MODE::MESH_WORK_BRUSH_SCULPT)
					{
						BrushSculptDlg* dlg = static_cast<BrushSculptDlg*>(m_pMeshDialogManager->getMeshDialog());
						dlg->sliderUpDown(QString("Size"), 0);
						setSphereScale(dlg->getSize() * 0.01);
					}
					else if (m_pMeshWorkManager->getWorkMode() == MESH_WORK_MODE::MESH_WORK_BRUSH_SELECTION)
					{
						BrushSelectDlg* dlg = static_cast<BrushSelectDlg*>(m_pMeshDialogManager->getMeshDialog());
						dlg->sliderUpDown(QString("Size"), 0);
						setSphereScale(dlg->getSize() * 0.01);
					}

				}
			}

		}

		if ((_modifier == Qt::Key_BracketRight))
		{
			if (m_pMeshDialogManager->isMeshDialog())
			{
				if (m_pMeshWorkManager->getWorkMode() == MESH_WORK_MODE::MESH_WORK_BRUSH_SCULPT || m_pMeshWorkManager->getWorkMode() == MESH_WORK_MODE::MESH_WORK_BRUSH_SELECTION)
				{
					QDialog* pDialog = m_pMeshDialogManager->getMeshDialog();
					if (m_pMeshWorkManager->getWorkMode() == MESH_WORK_MODE::MESH_WORK_BRUSH_SCULPT)
					{
						BrushSculptDlg* dlg = static_cast<BrushSculptDlg*>(m_pMeshDialogManager->getMeshDialog());
						dlg->sliderUpDown(QString("Size"), 1);
						setSphereScale(dlg->getSize() * 0.01);
					}
					else if (m_pMeshWorkManager->getWorkMode() == MESH_WORK_MODE::MESH_WORK_BRUSH_SELECTION)
					{
						BrushSelectDlg* dlg = static_cast<BrushSelectDlg*>(m_pMeshDialogManager->getMeshDialog());
						dlg->sliderUpDown(QString("Size"), 1);
						setSphereScale(dlg->getSize() * 0.01);
					}

				}
			}
		}

#if SUPPORT_MESH_TOGGLE
		if ((_modifier) == Qt::Key_Control)
		{
			m_CtrlToggle = !m_CtrlToggle;
		}
#endif
	}

	renderLater();
}

void MEVolumeView::mouseMoveEvent(QMouseEvent* e)
{
	if (m_pMeshWorkManager == nullptr)
	{
		return;
	}
	checkModifiers(e);

	this->setFocus();

	//if (m_pDataContext->m_MeshData.isValidate() == false)
	if (m_pDataContext->m_MeshData.GetMeshCount() < 1)
	{
		return;
	}

	if (m_pMeshWorkManager->processMouseMove(e, m_pMeshModelViewManager->GetCameraPtr()))
	{
		renderLater();
	}

	// brush
	{
		m_preMousePos = m_MousePos;
		m_MousePos = e->pos();

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

		m_pMeshModelViewManager->GetCameraPtr()->setScreenXY(m_MousePos.x(), m_MousePos.y());

		processMouseMove();

		m_pMeshModelViewManager->GetCameraPtr()->setPreScreenXY(m_preMousePos.x(), m_preMousePos.y());
	}
}

void MEVolumeView::mousePressEvent(QMouseEvent* e)
{
	if (m_pMeshWorkManager == nullptr)
	{
		return;
	}

	m_pWinManager->setMoveFocus(true);
	this->setFocus();

	// brush
	{
		m_MousePos = e->pos();
		m_mouseMove = false;

		m_clickBox.setCoords(
			m_MousePos.x() - 5, m_MousePos.y() - 5,
			m_MousePos.x() + 5, m_MousePos.y() + 5);

		processMousePress(e);
	}

	m_pMeshWorkManager->processMousePress(e);
}

void MEVolumeView::mouseReleaseEvent(QMouseEvent* e)
{
	if (m_pMeshWorkManager == nullptr)
	{
		return;
	}

	if (m_pMeshWorkManager->processMouseRelease(e))
	{
		renderLater();
	}

	// brush
	processMouseRelease(e);
	update();
}

void MEVolumeView::mouseDoubleClickEvent(QMouseEvent* event)
{
	if (m_pMeshWorkManager == nullptr)
	{
		return;
	}

	if (m_pMeshWorkManager->processMouseDoubleClicked(event))
	{
		renderLater();
	}
}

void MEVolumeView::wheelEvent(QWheelEvent* event)
{
	if (m_pMeshWorkManager == nullptr)
	{
		return;
	}

	m_pMeshWorkManager->ProcessWheel(event);

	renderLater();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////			마우스/키보드 내부함수		////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MEVolumeView::processMouseMove()
{
	if (m_pMeshWorkManager == nullptr)
	{
		return;
	}

	bool need_update = false;

	MESH_WORK_MODE workMode = m_pMeshWorkManager->getWorkMode();

	switch (workMode)
	{
	case MESH_WORK_BRUSH_SELECTION:
	{
		if (m_pMeshDialogManager->isMeshDialog())
		{
#if SUPPORT_MESH_TOGGLE
			if (m_LbuttonDown && enableCtrlToggleKey())
#else
			if (m_LbuttonDown && m_ctrl)
#endif				
			{
				process_Select();
				need_update = true;
			}
#if SUPPORT_MESH_TOGGLE
			if (m_LbuttonDown && enableCtrlToggleKey() && m_shift)
#else
			if (m_LbuttonDown && m_ctrl && m_shift)
#endif
			{
				process_Select(eMeshSelectMode::Unselect);
				need_update = true;
			}
			else
			{
				moveSphere();
				need_update = true;
			}
		}
	}
	break;
	case MESH_WORK_BRUSH_SCULPT:
	{
		if (m_pMeshDialogManager->isMeshDialog())
		{
#if SUPPORT_MESH_TOGGLE
			if (m_LbuttonDown && enableCtrlToggleKey())
#else
			if (m_LbuttonDown && m_ctrl)
#endif
			{
				BrushSculptDlg* pBrushScuptDlg = static_cast<BrushSculptDlg*>(m_pMeshDialogManager->getMeshDialog(MESH_DIALOG_SCULPT));

				SCULPT_MODE sculptMode = pBrushScuptDlg->getSculptMode();

				if (sculptMode == SM_SMOOTH || sculptMode == SM_INFLATE || sculptMode == SM_DEFLATE || sculptMode == SM_REDUCE || sculptMode == SM_REFINE)
				{
					process_BrushSculpt(m_shift);
				}
				else if (sculptMode == SM_MOVE || sculptMode == SM_DRAG)
				{
					process_Sculpt_Move(eMouseMode::Move);
				}
				need_update = true;
			}
			else
			{
				moveSphere();
				need_update = true;
			}
		}
	}
	break;
	case MESH_WORK_STAMP3D:
	{
		int nMove = 1;
		process_Stamp3D_Controller(nMove);
	}
	break;
	}

	if (need_update)
	{
		renderLater();
	}
}

void MEVolumeView::processMousePress(QMouseEvent* e)
{
	if (m_pMeshWorkManager == nullptr)
	{
		return;
	}

	if (e->buttons() & Qt::RightButton)
	{
		m_RbuttonDown = true;
	}
	else if (e->buttons() & Qt::LeftButton)
	{
		m_LbuttonDown = true;
	}
	else if (e->buttons() & Qt::MidButton)
	{
		m_MbuttonDown = true;
	}

	switch (m_pMeshWorkManager->getWorkMode())
	{
	case MESH_WORK_BRUSH_SCULPT:
	{
#if SUPPORT_MESH_TOGGLE
		if (enableCtrlToggleKey() || m_shift)
#else
		if (m_ctrl || m_shift)
#endif
		{
			mip::MeshTopology* pMesh = getIntersectedMeshByScreenMousePoint();
			if (pMesh != nullptr)
			{
				MESH_WORK_MODE workMode = m_pMeshWorkManager->getWorkMode();
				m_pActionManager->action_UndoRedo_update(m_pDataContext, MESH_WORK_BRUSH_SCULPT, workMode);
			}
			else
			{
				return;
			}
			BrushSculptDlg* pBrushSculptDlg = static_cast<BrushSculptDlg*>(m_pMeshDialogManager->getMeshDialog(MESH_DIALOG_SCULPT));

			SCULPT_MODE sculptMode = pBrushSculptDlg->getSculptMode();
			if (pBrushSculptDlg != nullptr && (sculptMode == SM_MOVE || sculptMode == SM_DRAG))
			{
				process_Sculpt_Move(eMouseMode::Press);
			}
			else
			{
				process_UpdateDisplay();
			}
		}
	}
	break;
	case MESH_WORK_BRUSH_SELECTION:
	{
#if SUPPORT_MESH_TOGGLE
		if (m_LbuttonDown && enableCtrlToggleKey())
#else
		if (m_LbuttonDown && m_ctrl)
#endif		
		{
			mip::MeshTopology* pMesh = getIntersectedMeshByScreenMousePoint();

			if (pMesh != nullptr)
			{
				MESH_WORK_MODE mode = m_pMeshWorkManager->getWorkMode();
				m_pActionManager->action_UndoRedo_update(m_pDataContext, MESH_WORK_BRUSH_SELECTION, mode);
			}
			else
			{
				return;
			}

#if SUPPORT_MESH_TOGGLE
			if (m_LbuttonDown && enableCtrlToggleKey())
#else
			if (m_LbuttonDown && m_ctrl)
#endif
			{
				process_Select();
				//need_update = true;
			}
#if SUPPORT_MESH_TOGGLE
			if (m_LbuttonDown && enableCtrlToggleKey() && m_shift)
#else
			if (m_LbuttonDown && m_ctrl && m_shift)
#endif
			{
				process_Select(eMeshSelectMode::Unselect);
				//need_update = true;
			}
		}
	}
	break;
	case MESH_WORK_STAMP3D:
	{
		if (m_LbuttonDown && m_ctrl)
		{
#if SUPPORT_STAMP3D == 1
			process_Stamp3D();
#endif
		}

		if (m_LbuttonDown && !m_ctrl)
		{
			int nPress = 0;
			process_Stamp3D_Controller(nPress);
		}
	}
	break;
	}

	renderLater();
}

void MEVolumeView::processMouseRelease(QMouseEvent* e)
{
	if (m_pMeshEditSculpt == nullptr)
	{
		return;
	}

	bool need_update = false;
	MESH_WORK_MODE workMode = m_pMeshWorkManager->getWorkMode();
	bool objTrans = (workMode == MESH_WORK_NONE);

	objTrans = true;

	if (objTrans)
	{
#if SUPPORT_MESH_TOGGLE
		if (m_LbuttonDown && enableCtrlToggleKey())
#else
		if (m_LbuttonDown && m_ctrl)
#endif
		{
			switch (workMode)
			{
			case MESH_WORK_BRUSH_SELECTION:
			{
				updateSelectionUI();
			}
			break;
			case MESH_WORK_BRUSH_SCULPT:
			{
				m_pMeshEditSculpt->ProcessRelease(m_preMousePos, m_MousePos, this->size());
				updateGeometryCount();
			}
			break;
			}
		}
		else if (m_LbuttonDown && m_shift)
		{
			switch (workMode)
			{
			case MESH_WORK_BRUSH_SELECTION:
			{
				updateSelectionUI();
			}
			case MESH_WORK_BRUSH_SCULPT:
			{
				updateCurrentMeshBoundingBox();
			}
			break;
			}
		}
		else if (m_MbuttonDown && m_shift)
		{
			switch (workMode)
			{
			case	MESH_WORK_BRUSH_SCULPT:
			{
				if (m_pMeshDialogManager->isMeshDialog())
				{
				}
			}
			break;
			}
		}
		else if (m_LbuttonDown)
		{
			switch (workMode)
			{
			case	MESH_WORK_STAMP3D:
			{
				if (m_pMeshDialogManager->isMeshDialog())
				{
					m_pMeshRenderManager->getFontControl()->setPick(false);
				}
			}
			break;
			}
		}
	}

	m_preMousePos = m_MousePos;
	m_pMeshModelViewManager->GetCameraPtr()->setPreScreenXY(m_preMousePos.x(), m_preMousePos.y());

	if (need_update)
	{
		renderLater();
	}

	m_RbuttonDown = false;
	m_LbuttonDown = false;
	m_MbuttonDown = false;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////			렌더링 함수 구현부			////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MEVolumeView::render(QPainter* p)
{
	m_pMeshRenderManager->render(p);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////                Mesh Edit                /////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
@brief
@return
*/
bool MEVolumeView::process_BrushSculpt(bool bShift)
{
	if (m_pMeshEditSculpt == nullptr)
	{
		return false;
	}

	return m_pMeshEditSculpt->ProcessBrushSculpt(m_MousePos, this->size());
}

/*
@brief
@return
*/
bool MEVolumeView::process_Select(eMeshSelectMode selectMode)
{
	if (m_pMeshEditSculpt == nullptr)
	{
		return false;
	}
	return m_pMeshEditSculpt->ProcessSelect(selectMode, m_MousePos, this->size());
}


/*
@brief
@return
*/
bool MEVolumeView::readyBrush(int _MeshWorkMode, int initial)
{
	if (m_pMeshEditSculpt == nullptr)
	{
		return false;
	}

	return m_pMeshEditSculpt->ReadyBrush((MESH_WORK_MODE)_MeshWorkMode, initial);
}

bool MEVolumeView::updateMeshVertex()
{
	if (m_pMeshWorkManager == nullptr)
	{
		return false;
	}

	mip::MeshTopology* p_mesh = nullptr;
	int meshCnt = m_pDataContext->m_MeshData.GetMeshCount();

	int nSelectedID = -1;
	int nSelectedCnt = 0;
	for (int ii = 0; ii < meshCnt; ii++)
	{
		MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(ii);

		if (pMeshInfo && pMeshInfo->selected)
		{
			nSelectedCnt++;
			nSelectedID = ii;
		}
	}

	if (nSelectedID < 0 || nSelectedCnt != 1)
	{
		return false;
	}

	for (int i = 0; i < meshCnt; ++i)
	{
		MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(i);

		if (!pMeshInfo || !pMeshInfo->selected)
		{
			continue;
		}

		p_mesh = m_pDataContext->m_MeshData.GetMesh(i);
		p_mesh->updateVertex();
	}

	printf_s("\n updateMeshVertex");
	return true;
}

bool MEVolumeView::deleteSelectFlag()
{
	if (m_pMeshWorkManager == nullptr)
	{
		return false;
	}

	mip::MeshTopology* p_mesh = nullptr;
	int meshCnt = m_pDataContext->m_MeshData.GetMeshCount();

	if (m_pMeshWorkManager->getWorkMode() != MESH_WORK_MODE::MESH_WORK_BRUSH_SELECTION)
	{
		return false;
	}

	int nSelectedID = -1;
	for (int ii = 0; ii < meshCnt; ii++)
	{
		MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(ii);

		if (pMeshInfo && pMeshInfo->selected)
		{
			nSelectedID = ii;
		}
	}

	if (nSelectedID < 0)
	{
		return false;
	}

	for (int i = 0; i < meshCnt; ++i)
	{
		MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(i);

		if (!pMeshInfo || !pMeshInfo->selected)
		{
			continue;
		}

		p_mesh = m_pDataContext->m_MeshData.GetMesh(i);
#if 0
		for (int ii = 0; ii < p_mesh->m_ttris.size(); ii++)
		{
			if (!p_mesh->m_ttris[ii].isS()) continue;
			if (p_mesh->m_ttris[ii].isD()) continue;
			p_mesh->m_ttris[ii].setD();

			printf_s("\n [1] Deleted m_ttris - %d", ii);

			int v0 = p_mesh->m_ttris[ii].vi[0];
			int v1 = p_mesh->m_ttris[ii].vi[1];
			int v2 = p_mesh->m_ttris[ii].vi[2];

			int TRI_SIZE = 3;
			for (int kk = 0; kk < TRI_SIZE; kk++)
			{
				int vidx = p_mesh->m_ttris[ii].vi[kk];
				for (int gg = 0; gg < p_mesh->m_tverts[vidx].heis.size(); gg++) {
					int heidx = p_mesh->m_tverts[vidx].heis[gg];
					if (p_mesh->m_tVHedges[heidx].vi == v0 ||
						p_mesh->m_tVHedges[heidx].vi == v1 || p_mesh->m_tVHedges[heidx].vi == v2)
					{
						p_mesh->m_tVHedges[heidx].setD();
						printf_s("\n [2]  Deleted m_tVHedges - %d", heidx);
					}
				}
			}
		}

		for (int ii = 0; ii < p_mesh->m_tverts.size(); ii++)
		{
			if (p_mesh->m_tverts[ii].isD()) continue;

			int Dcnt = 0;
			for (int jj = 0; jj < p_mesh->m_tverts[ii].tis.size(); jj++)
			{
				int fi = p_mesh->m_tverts[ii].tis[jj];
				if (p_mesh->m_ttris[fi].isD()) Dcnt++;
			}

			if (p_mesh->m_tverts[ii].tis.size() == Dcnt)
			{
				p_mesh->m_tverts[ii].setD();
				printf_s("\n [3]  Deleted m_tverts - %d", ii);
			}

		}
#endif
		mip::mesh_control::buildTopologyHEdgeOpposite(p_mesh->m_tverts, p_mesh->m_tVHedges);

		for (int ii = 0; ii < p_mesh->m_ttris.size(); ii++)
		{
			if (!p_mesh->m_ttris[ii].isS())
			{
				continue;
			}

			if (p_mesh->m_ttris[ii].isD())
			{
				continue;
			}

			mip::mesh_control::DeleteTTri(p_mesh, ii);
		}

		//mip::mesh_control::DeleteSelection(p_mesh);
		p_mesh->updateVertex();
		p_mesh->updateColor(p_mesh->m_baseColor);

		m_pRenderer->makeCurrent();
		p_mesh->buildRenderBufferTopology();
		m_pRenderer->doneCurrent();
	}
	return false;
}

bool MEVolumeView::allSelectFlag()
{
	if (m_pMeshWorkManager == nullptr)
	{
		return false;
	}

	mip::MeshTopology* p_mesh = nullptr;
	int meshCnt = m_pDataContext->m_MeshData.GetMeshCount();

	if (m_pMeshWorkManager->getWorkMode() != MESH_WORK_MODE::MESH_WORK_BRUSH_SELECTION)
	{
		return false;
	}

	int nSelectedID = -1;
	for (int ii = 0; ii < meshCnt; ii++)
	{
		MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(ii);

		if (pMeshInfo && pMeshInfo->selected)
		{
			nSelectedID = ii;
		}
	}

	if (nSelectedID < 0)
	{
		return false;
	}

	for (int i = 0; i < meshCnt; ++i)
	{
		MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(i);
		if (!pMeshInfo || !pMeshInfo->selected)
		{
			continue;
		}

		p_mesh = m_pDataContext->m_MeshData.GetMesh(i);

		for (int ii = 0; ii < p_mesh->m_ttris.size(); ii++)
		{
			p_mesh->m_ttris[ii].setS();
		}

		MeshInfo* mInfo = m_pDataContext->m_MeshData.GetMeshInfo(i);
		mip::VECTOR4 color(mip::VECTOR4(mInfo->color.r / 255.0f, mInfo->color.g / 255.0f, mInfo->color.b / 255.0f, p_mesh->getAlphaVal() / 255.0f));
		p_mesh->updateColor(color);

		m_pRenderer->makeCurrent();
		p_mesh->buildRenderBufferTopology();
		m_pRenderer->doneCurrent();
	}

	return true;
}

bool MEVolumeView::moveSphere()
{
	if (m_pMeshEditSculpt == nullptr)
	{
		return false;
	}

	return m_pMeshEditSculpt->MoveSphere(m_MousePos, this->size());
}

void MEVolumeView::updateCurrentMeshBoundingBox()
{
	if (m_pMeshWorkManager == nullptr)
	{
		return;
	}

	if (m_pMeshDialogManager->isMeshDialog())
	{
		//Bounding Box 계산
		int pickedMeshIndex = m_pDataContext->m_MeshData.GetCurrentMeshIndex();
		MeshInfo* pInfo = m_pDataContext->m_MeshData.GetMeshInfo(pickedMeshIndex);
		mip::MeshTopology* pMesh = m_pDataContext->m_MeshData.GetMesh(pickedMeshIndex);

		if (pMesh)
		{
			mip::AABB box;
			mip::mesh_control::getMinMax(pMesh->m_verts, box.min, box.max);
			pMesh->m_boundingBox = box;

#if TEST_byPHS
			//test
			int Non_cnt = mip::mesh_control::getNonManiHEdgeCnt(m);
			printf_s("\n [[ !Non_cnt : %d ]] ", Non_cnt);
#endif
		}
	}
}

mip::MeshTopology* MEVolumeView::getIntersectedMeshByScreenMousePoint()
{
	if (m_pMeshWorkManager == nullptr)
	{
		return nullptr;
	}

	MeshEditScreenSelector selector(
		m_pRenderer,
		&m_pDataContext->m_MeshData,
		&m_pDataContext->volume_data,
		m_pMeshWorkManager,
		m_pMeshModelViewManager);

	MeshLayerData meshLayer;
	QPoint screenPoint = m_preMousePos;
	QSize screenSize = this->size();
	if (selector.TryGetIntersectedMesh(&meshLayer, screenPoint, screenSize) == false)
	{
		return nullptr;
	}

	return meshLayer.Data;
}

bool MEVolumeView::process_Stamp3D()
{
	if (!m_pActionManager->isActionFinished())
	{
		return false;
	}
#if SUPPORT_STAMP3D == 1
	const MESH_WORK_MODE		mode = m_pMeshWorkManager->getWorkMode();

	mip::MeshTopology* p_mesh = nullptr;

	int n_mesh = m_pDataContext->m_MeshData.GetMeshCount();

	int nSelectedID = -1;
	int nSelectedCnt = 0;
	//for (int ii = 0; ii < m_pWinManager->vt_pckID.size(); ii++)
	for (int ii = 0; ii < n_mesh; ii++)
	{
		MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(ii);
		//if (m_pWinManager->vt_pckID[ii])
		if (pMeshInfo && pMeshInfo->selected)
			//if (m_pWinManager->vt_pckID[ii])
		{
			nSelectedCnt++;
			nSelectedID = ii;
		}
	}

	if (nSelectedID < 0 /*|| nSelectedCnt != 1*/)
	{
		m_pMeshWorkManager->UpdateWorkMode(MESH_WORK_NONE, true);
		return false;
	}

	for (int i = 0; i < n_mesh; ++i)
	{
		//if (!m_pWinManager->vt_pckID[i])
		MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(i);
		if (!pMeshInfo || !pMeshInfo->selected)
		{
			continue;
		}

		p_mesh = m_pDataContext->m_MeshData.GetMesh(i);

		int _vertIdx = -1;
		mip::VECTOR3 tracePoint(mip::VECTOR3(0, 0, 0));

		mip::MATRIX44				mat_offset;
		//mip::VECTOR3				offset_center(-m_pDataContext->m_MeshData.getSizeX()*0.5f, -m_pDataContext->m_MeshData.getSizeY()*0.5f, -m_pDataContext->m_MeshData.getSizeZ()*0.5f);
		mip::VECTOR3				offset_center(-m_pDataContext->volume_data.getSizeX() * 0.5f, -m_pDataContext->volume_data.getSizeY() * 0.5f, -m_pDataContext->volume_data.getSizeZ() * 0.5f);


		mat_offset.identity();
		mat_offset.translation(offset_center);

		mip::MATRIX44 view_world = m_pMeshModelViewManager->GetMainTransform();

		mip::MATRIX44 wvp = m_pRenderer->getWorld() * m_pRenderer->getView() * m_pRenderer->getProj();
		mip::RAY ray = mip::geom::ScreenToRay(mip::VECTOR2(m_MousePos.x(), m_MousePos.y()), this->width(), this->height(),
			m_pMeshModelViewManager->GetCameraPtr()->getView(), m_pMeshModelViewManager->GetCameraPtr()->getProj(), &(p_mesh->getMatrix() * mat_offset * view_world));


		if (p_mesh->m_ttris.size() > 0)
			bool bReturn = p_mesh->intersectRay(ray.org, ray.dir, tracePoint, _vertIdx, false);

		//pckSphere 위치
#if PRINT_LOG
		printf_s("\n [_vertIdx - %d]", _vertIdx);
#endif
		int pick_ti = -1;
		mip::VECTOR3 outVec = mip::VECTOR3(0.0f, 0.0f, 0.0f);
		if (_vertIdx != -1)
		{
			//			if ((pick_ti = mip::mesh_control::getRealPickPoint(p_mesh, ray, _vertIdx, outVec)))
			//			{
			//				m_pick3dTextPoint = outVec;
			//#if PRINT_LOG
			//				int v0 = p_mesh->m_ttris[pick_ti].vi[0];
			//				int v1 = p_mesh->m_ttris[pick_ti].vi[1];
			//				int v2 = p_mesh->m_ttris[pick_ti].vi[2];
			//				printf_s("\n pick_ti - %d", pick_ti);
			//				printf_s("\n[Success - ti %d ( %d, %d, %d )]\n", pick_ti, v0, v1, v2);
			//#endif
			//			}
			//			else
			{
				m_pick3dTextPoint = p_mesh->m_tverts[_vertIdx].pos;
			}
		}
		else
		{
			m_pick3dTextPoint = p_mesh->m_tverts[0].pos;
		}

		std::vector<mip::VECTOR3> pList;
		auto dlg = static_cast<Stamp3dDlg*>(m_pMeshDialogManager->getMeshDialog(MESH_DIALOG_STAMP3D));
		if (dlg != nullptr && m_pMeshWorkManager->getWorkMode() == MESH_WORK_STAMP3D)
		{
			dlg->SetTargetMesh(p_mesh);
			dlg->SetPickPos(outVec);
			dlg->SetPickFaceIdx(pick_ti);

			printf_s("\n [process_Stamp3D] fontMesh..GetPickFaceIdx() pick_ti %d", pick_ti);
			printf_s("\n [process_Stamp3D] fontMesh..GetPickPos() x - %f / y - %f / z - %f", outVec.x, outVec.y, outVec.z);
			mip::MeshTopology* pFontMT = dlg->GetFontMesh();

			if (pFontMT != nullptr)
			{
				//초기화
				for (int ii = 0; ii < pFontMT->m_tverts.size(); ii++)
				{
					pFontMT->m_tverts[ii].pos = dlg->GetOrgFontTVerts()[ii].pos;
				}

				pFontMT->m_boundingBox = dlg->GetOrgFontBox();
				//m_DrawLineList.clear();
				bool bAttached = mip::mesh_control::MoveFontPolygon(dlg->GetTargetMesh(), dlg->GetFontMesh(), dlg->GetOrgFontTVerts(),
					dlg->GetPickPos(), dlg->GetPickFaceIdx(), dlg->GetOffsetDist(), pList);

				FontController* pFC = m_pMeshRenderManager->getFontControl();
				if (pFC->BackToOrgPos())
					pFC->MoveCtrlToMeshSurface(dlg->GetTargetMesh(), dlg->GetPickPos(), dlg->GetPickFaceIdx());

				dlg->SetStampEnable(bAttached);
				if (!bAttached || _vertIdx < 0)
				{
					for (int ii = 0; ii < pFontMT->m_tverts.size(); ii++)
					{
						pFontMT->m_tverts[ii].pos = mip::VECTOR3(0, 0, 0);
					}
				}
				else
				{
					//offset에 따른 음/양각 처리
					int faceIdx = dlg->GetPickFaceIdx();
					dlg->GetTargetMesh()->m_ttris[faceIdx];

					mip::VECTOR3	Normal;
					mip::VECTOR3	Xyz[3];

					int v0 = dlg->GetTargetMesh()->m_ttris[faceIdx].vi[0];
					int v1 = dlg->GetTargetMesh()->m_ttris[faceIdx].vi[1];
					int v2 = dlg->GetTargetMesh()->m_ttris[faceIdx].vi[2];

					Xyz[0] = dlg->GetTargetMesh()->m_tverts[v0].pos;
					Xyz[1] = dlg->GetTargetMesh()->m_tverts[v1].pos;
					Xyz[2] = dlg->GetTargetMesh()->m_tverts[v2].pos;



					Normal = (Xyz[1] - Xyz[0]).cross((Xyz[2] - Xyz[0]));
					Normal.normalize();

					float fOffset = STAMP3D_OFFSET_ADJUSTMENT;
					for (int ii = 0; ii < pFontMT->m_tverts.size(); ii++)
					{
#if 1
						pFontMT->m_tverts[ii].pos = dlg->GetFontMesh()->m_tverts[ii].pos + (Normal * fOffset);
#endif
					}

				}

				pFontMT->updateVertex();
				m_pWinManager->makeCurrent();
				pFontMT->buildRenderBufferTopology();
				m_pWinManager->doneCurrent();

				dlg->SetAccQuater(mip::QUATERNION(0, 0, 0, 0));
#if TEST_LINE_RENDER == 1
				for (int ii = 0; ii < pList.size(); ii++)
				{
					m_DrawLineList.push_back(pList[ii]);
				}
#endif
			}
		}
	}
#endif
	return false;
}

bool MEVolumeView::process_Stamp3D_Controller(int mode)
{
	if (m_pMeshWorkManager == nullptr)
	{
		return false;
	}

	if (!m_pActionManager->isActionFinished())
	{
		return false;
	}

	const int _press = 0;
	const int _move = 1;
#if 1	//move
	Stamp3dDlg* dlg = static_cast<Stamp3dDlg*>(m_pMeshDialogManager->getMeshDialog(MESH_DIALOG_STAMP3D));
	if (dlg != nullptr && m_pMeshWorkManager->getWorkMode() == MESH_WORK_STAMP3D)
	{
		mip::MeshTopology* pRotCtrlMesh = m_pMeshRenderManager->getFontControl()->pRotateController;
		FontController* pFontControl = m_pMeshRenderManager->getFontControl();

		muint8 id = m_pDataContext->m_MeshData.GetCurrentMeshIndex();
		mip::MeshTopology* mesh = m_pDataContext->m_MeshData.GetMesh(id);

		mip::MATRIX44 mat_offset;
		mip::VECTOR3 offset_center(-m_pDataContext->volume_data.getSizeX() * 0.5f, -m_pDataContext->volume_data.getSizeY() * 0.5f, -m_pDataContext->volume_data.getSizeZ() * 0.5f);

		mat_offset.identity();
		mat_offset.translation(offset_center);

		mip::MATRIX44 view_world = m_pMeshModelViewManager->GetMainTransform();

		mip::RAY ray = mip::geom::ScreenToRay(mip::VECTOR2((float)m_preMousePos.x(), (float)m_preMousePos.y()), this->width(), this->height(),
			m_pMeshModelViewManager->GetCameraPtr()->getView(), m_pMeshModelViewManager->GetCameraPtr()->getProj(), &(mesh->getMatrix() * mat_offset * view_world));
		mip::VECTOR3 vec3 = m_pMeshModelViewManager->GetCameraPtr()->getPos();

		mip::VECTOR4 col = mip::VECTOR4(0, 0, 0, 0);
		mip::VECTOR3 out = mip::VECTOR3(0, 0, 0);
		if (pFontControl->isPicking(dlg->GetTargetMesh(), dlg->GetPickPos(), ray.org, ray.dir, dlg->GetPickFaceIdx(), out))
		{
			col = mip::VECTOR4(180, 20, 20, 80);

			pRotCtrlMesh->setAlpha(170);
			pFontControl->setHover(true);

			if (mode == _press)
			{
				m_pMeshRenderManager->getFontControl()->setPick(true);
			}
		}
		else
		{
			col = mip::VECTOR4(100, 100, 100, 80);
			pRotCtrlMesh->setAlpha(60);
			pFontControl->setHover(false);
		}

		if (mode == _press)
		{
			return true;
		}

		mip::QUATERNION quat = mip::QUATERNION(0, 0, 0, 0);
		if (out != mip::VECTOR3(0, 0, 0) && m_LbuttonDown)
		{
			//m_DrawLineList.clear();
			//m_DrawLineList.push_back(dlg->GetPickPos());
			//m_DrawLineList.push_back(out);

			mip::VECTOR3 oldPickPT = dlg->GetPickOldPos();
			if (oldPickPT != mip::VECTOR3(0, 0, 0))
			{
				mip::VECTOR3 pivotPt = dlg->GetPickPos();
				mip::VECTOR3 CurPickPT = out;

				mip::VECTOR3 CurDir = (CurPickPT - pivotPt).normalize();
				mip::VECTOR3 OldDir = (oldPickPT - pivotPt).normalize();
				quat = mip::mesh_control::RotationBetween_Vectors(OldDir, CurDir);
				mip::QUATERNION AccumulatedQuat = dlg->GetAccQuater();

				if (AccumulatedQuat.equals(mip::QUATERNION(0, 0, 0, 0), MIP_EPSILON2))
				{
					AccumulatedQuat = quat;
				}
				AccumulatedQuat *= quat;

				dlg->SetAccQuater(AccumulatedQuat);
			}

			dlg->SetPickOldPos(out);
			update();
		}


		if (pRotCtrlMesh->m_baseColor.x != col.x &&
			pRotCtrlMesh->m_baseColor.y != col.y &&
			pRotCtrlMesh->m_baseColor.z != col.z)
		{
			update();
		}
		pRotCtrlMesh->m_baseColor = col;

		//----------------------------
		//Rotate
		//----------------------------
		mip::MeshTopology* pFontMT = dlg->GetFontMesh();
#if 1
		if (pFontMT != nullptr && m_LbuttonDown && pFontControl->getHover())
		{
			//초기화
			for (int ii = 0; ii < pFontMT->m_tverts.size(); ii++)
			{
				pFontMT->m_tverts[ii].pos = dlg->GetOrgFontTVerts()[ii].pos;
			}

			mip::QUATERNION AccumulatedQuat = dlg->GetAccQuater();
			pFontMT->m_boundingBox = dlg->GetOrgFontBox();
			std::vector<mip::VECTOR3> pList;
			bool bAttached = mip::mesh_control::MoveFontPolygon(dlg->GetTargetMesh(), dlg->GetFontMesh(), dlg->GetOrgFontTVerts(),
				dlg->GetPickPos(), dlg->GetPickFaceIdx(), dlg->GetOffsetDist(), pList, AccumulatedQuat);

			dlg->SetStampEnable(bAttached);
			if (!bAttached || dlg->GetPickFaceIdx() < 0)
			{
				for (int ii = 0; ii < pFontMT->m_tverts.size(); ii++)
				{
					pFontMT->m_tverts[ii].pos = mip::VECTOR3(0, 0, 0);
				}
				//printf_s("\n ---------- if (!bAttached || dlg->GetPickFaceIdx() < 0) ------------");
				dlg->SetAccQuater(mip::QUATERNION(0, 0, 0, 0));
			}
			else
			{
				//offset에 따른 음/양각 처리
				int faceIdx = dlg->GetPickFaceIdx();

				mip::VECTOR3	Normal;
				mip::VECTOR3	Xyz[3];

				int v0 = dlg->GetTargetMesh()->m_ttris[faceIdx].vi[0];
				int v1 = dlg->GetTargetMesh()->m_ttris[faceIdx].vi[1];
				int v2 = dlg->GetTargetMesh()->m_ttris[faceIdx].vi[2];

				Xyz[0] = dlg->GetTargetMesh()->m_tverts[v0].pos;
				Xyz[1] = dlg->GetTargetMesh()->m_tverts[v1].pos;
				Xyz[2] = dlg->GetTargetMesh()->m_tverts[v2].pos;

				Normal = (Xyz[1] - Xyz[0]).cross((Xyz[2] - Xyz[0]));
				Normal.normalize();

				float fOffset = STAMP3D_OFFSET_ADJUSTMENT;
				for (int ii = 0; ii < pFontMT->m_tverts.size(); ii++)
				{
#if 1
					pFontMT->m_tverts[ii].pos = dlg->GetFontMesh()->m_tverts[ii].pos + (Normal * fOffset);
#endif
				}

			}

			pFontMT->updateVertex();
			m_pWinManager->makeCurrent();
			pFontMT->buildRenderBufferTopology();
			m_pWinManager->doneCurrent();
		}
#endif		

		//----------------------------
		//Rotate end
		//----------------------------

	}
#endif 

	return false;
}

void MEVolumeView::updateSelectionUI(mip::MeshTopology* pMT)
{
	if (m_pMeshWorkManager == nullptr)
	{
		return;
	}

	BrushSelectDlg* dlg = static_cast<BrushSelectDlg*>(m_pMeshDialogManager->getMeshDialog(MESH_DIALOG_SELECT));
	if (dlg != nullptr && m_pMeshWorkManager->getWorkMode() == MESH_WORK_BRUSH_SELECTION)
	{
		int nPickMesh = m_pDataContext->m_MeshData.GetCurrentMeshIndex();
		MeshInfo* info = m_pDataContext->m_MeshData.GetMeshInfo(nPickMesh);
		mip::MeshTopology* m = m_pDataContext->m_MeshData.GetMesh(nPickMesh);

		bool isSelection = false;
		if (pMT != nullptr)
		{
			m = pMT;
		}

		if (m)
		{
			isSelection = mip::mesh_control::isTriFlag(m, mip::SELECTED);
		}

		if (isSelection)
		{
			dlg->showFuncBtn();
		}
		else
		{
			dlg->hideFuncBtn();
		}
	}
}

bool MEVolumeView::getGeometryCount(mip::MeshTopology* pMT, int& _vCnt, int& _tCnt)
{
	if (m_pMeshWorkManager == nullptr)
	{
		return false;
	}

	if (pMT == nullptr || !m_pActionManager->isActionFinished())
	{
		return false;
	}

	int triCnt = 0;
	int vertCnt = 0;
	for (int ii = 0; ii < pMT->m_ttris.size(); ii++)
	{
		if (pMT->m_ttris[ii].isD() || pMT->m_ttris[ii].isUS1())
		{
			continue;
		}

		triCnt++;
	}

	for (int ii = 0; ii < pMT->m_tverts.size(); ii++)
	{
		if (pMT->m_tverts[ii].isD() || pMT->m_tverts[ii].isUS1())
		{
			continue;
		}
		vertCnt++;
	}

	_tCnt = triCnt;
	_vCnt = vertCnt;

	return true;
}

//mode : 0 - All , 1 - only Display, 2 - only BrushData
bool MEVolumeView::createMatchingData(int Mode)
{
	int nPickMesh = m_pDataContext->m_MeshData.GetCurrentMeshIndex();
	MeshInfo* info = m_pDataContext->m_MeshData.GetMeshInfo(nPickMesh);
	mip::MeshTopology* mesh = m_pDataContext->m_MeshData.GetMesh(nPickMesh);

	if (mesh)
	{
		mip::mesh_control::createMatchingData(mesh);
	}
	else
	{
		return false;
	}

	return true;
}

bool MEVolumeView::clearVertFlag()
{
	int nPickMesh = m_pDataContext->m_MeshData.GetCurrentMeshIndex();
	MeshInfo* info = m_pDataContext->m_MeshData.GetMeshInfo(nPickMesh);
	mip::MeshTopology* mesh = m_pDataContext->m_MeshData.GetMesh(nPickMesh);

	if (mesh)
	{
		for (int ii = 0; ii < mesh->m_tverts.size(); ii++)
		{
			if (mesh->m_tverts[ii].isD())
			{
				continue;
			}
			mesh->m_tverts[ii].clearFlags();
		}

		for (int ii = 0; ii < mesh->m_ttris.size(); ii++)
		{
			if (mesh->m_ttris[ii].isD())
			{
				continue;
			}
			mesh->m_ttris[ii].clearFlags();
		}

		for (int ii = 0; ii < mesh->m_tVHedges.size(); ii++)
		{
			if (mesh->m_tVHedges[ii].isD())
			{
				continue;
			}
			mesh->m_tVHedges[ii].clearFlags();
		}
	}
	else
	{
		return false;
	}

	return true;
}


bool MEVolumeView::setSphereScale(float scale)
{
	if (m_pMeshRenderManager == nullptr)
	{
		return false;
	}

	if (m_pMeshWorkManager->getWorkMode() == MESH_WORK_BRUSH_SCULPT || m_pMeshWorkManager->getWorkMode() == MESH_WORK_BRUSH_SELECTION)
	{
		m_pMeshRenderManager->setSphereScale(scale);
		m_sphereScale = scale;
	}

	return true;
}

bool MEVolumeView::getMEViewGeometryCount(int& _vCnt, int& _tCnt)
{
	_vCnt = m_vertCnt;
	_tCnt = m_TriCnt;
	return true;
}

void MEVolumeView::updateGeometryCnt(bool bUpdate)
{
	m_bUpdateGeometryCnt = bUpdate;
};

bool MEVolumeView::enableCtrlToggleKey()
{
	return m_CtrlToggle;
};

float MEVolumeView::GetSphereScale() const
{
	return m_sphereScale;
}

mip::VECTOR3 MEVolumeView::GetPickPoint() const
{
	if (m_pMeshEditSculpt == nullptr)
	{
		return mip::VECTOR3();
	}

	return m_pMeshEditSculpt->GetPickPoint();
}

mip::VECTOR3 MEVolumeView::GetPickVertPoint() const
{
	if (m_pMeshEditSculpt == nullptr)
	{
		return mip::VECTOR3();
	}

	return m_pMeshEditSculpt->GetPickVertPoint();
}

void MEVolumeView::SetUpdateGeometryCnt(bool value)
{
	m_bUpdateGeometryCnt = value;
}

bool MEVolumeView::IsUpdateGeometryCnt() const
{
	return m_bUpdateGeometryCnt;
}

void MEVolumeView::SetOldIdx(int value)
{
	m_oldIdx = value;
}

int MEVolumeView::GetOldIdx() const
{
	return m_oldIdx;
}

void MEVolumeView::SetTotalSize(int value)
{
	m_TotalSize = value;
}

int MEVolumeView::GetTotalSize() const
{
	return m_TotalSize;
}

int MEVolumeView::GetVertCnt() const
{
	return m_vertCnt;
}

int MEVolumeView::GetTriCnt() const
{
	return m_TriCnt;
}

void MEVolumeView::SetSphereRadius(float value)
{
	if (m_pMeshEditSculpt)
	{
		m_pMeshEditSculpt->SetSphereRadius(value);
	}
}

float MEVolumeView::GetSphereRadius() const
{
	if (m_pMeshEditSculpt == nullptr)
	{
		return 0.0f;
	}
	return m_pMeshEditSculpt->GetSphereRadius();
}

std::vector<mip::VECTOR3> MEVolumeView::GetDrawLineList() const
{
	return m_drawLineList;
}

void MEVolumeView::SetCtrl(bool value)
{
	m_ctrl = value;
}

void MEVolumeView::SetAlt(bool value)
{
	m_alt = value;
}

void MEVolumeView::SetShift(bool value)
{
	m_shift = value;
}

bool MEVolumeView::getGeometryCount(int& _vCnt, int& _tCnt)
{
	MESH_WORK_MODE mode = m_pMeshWorkManager->getWorkMode();
	mip::MeshTopology* p_mesh = nullptr;
	int n_mesh = m_pDataContext->m_MeshData.GetMeshCount();

	int nSelectedID = -1;
	int nSelectedCnt = 0;

	for (int ii = 0; ii < n_mesh; ii++)
	{
		MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(ii);
		if (pMeshInfo && pMeshInfo->selected)
		{
			nSelectedCnt++;
			nSelectedID = ii;
		}
	}

	if (nSelectedID < 0 || nSelectedCnt != 1)
	{
		m_pMeshWorkManager->UpdateWorkMode(MESH_WORK_NONE, true);
		return false;
	}

	bool bSuccess = false;

	BrushSculptDlg* dlg = static_cast<BrushSculptDlg*>(m_pMeshDialogManager->getMeshDialog(MESH_DIALOG_SCULPT));

	for (int i = 0; i < n_mesh; ++i)
	{
		MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(i);
		if (!pMeshInfo || !pMeshInfo->selected)
		{
			continue;
		}

		p_mesh = m_pDataContext->m_MeshData.GetMesh(i);
		getGeometryCount(p_mesh, _vCnt, _tCnt);
		return true;
	}
	return false;
}

bool MEVolumeView::updateGeometryCount(mip::MeshTopology* pMT, int& _vCnt, int& _tCnt)
{
	int vertCnt;
	int triCnt;

	if (!getGeometryCount(pMT, vertCnt, triCnt))
	{
		return false;
	}

	_vCnt = vertCnt;
	_tCnt = triCnt;
	return true;
}

bool MEVolumeView::updateGeometryCount(int& _vCnt, int& _tCnt)
{
	m_vertCnt = _vCnt;
	m_TriCnt = _tCnt;
	return true;
}

bool MEVolumeView::updateGeometryCount(int pick)
{
	m_bUpdateGeometryCnt = true;
#if 0 
	int nPickMesh = m_pDataContext->m_MeshData.GetCurrentMeshIndex();
	if (pick != -1) nPickMesh = pick;
	MeshInfo* info = m_pDataContext->m_MeshData.GetMeshInfo(nPickMesh);
	mip::MeshTopology* mesh = m_pDataContext->m_MeshData.GetMesh(nPickMesh);
	updateGeometryCount(mesh);
#endif
	return true;
}

bool MEVolumeView::process_Sculpt_Move(eMouseMode mode)
{
	if (m_pMeshEditSculpt == nullptr)
	{
		return false;
	}
	return m_pMeshEditSculpt->ProcessMove(mode, m_preMousePos, m_MousePos, this->size());
}

bool MEVolumeView::process_UpdateDisplay()
{
	if (m_pMeshWorkManager == nullptr)
	{
		return false;
	}

	MESH_WORK_MODE workMode = m_pMeshWorkManager->getWorkMode();
	mip::MeshTopology* p_mesh = nullptr;

	int n_mesh = m_pDataContext->m_MeshData.GetMeshCount();

	int nSelectedID = -1;
	int nSelectedCnt = 0;
	for (int ii = 0; ii < n_mesh; ii++)
	{
		MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(ii);
		if (pMeshInfo && pMeshInfo->selected)
		{
			nSelectedCnt++;
			nSelectedID = ii;
		}
	}

	if (nSelectedID < 0 || nSelectedCnt != 1)
	{
		m_pMeshWorkManager->UpdateWorkMode(MESH_WORK_NONE, true);
		return false;
	}

	for (int i = 0; i < n_mesh; ++i)
	{
		MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(i);
		if (!pMeshInfo || !pMeshInfo->selected)
		{
			continue;
		}

		p_mesh = m_pDataContext->m_MeshData.GetMesh(i);

		mip::mesh_control::updateDisplay(p_mesh);
		m_pRenderer->makeCurrent();
		p_mesh->buildRenderBufferTopology();
		m_pRenderer->doneCurrent();
	}

	return true;
}

bool MEVolumeView::clearSelectFlag(bool force)
{
	if (m_pMeshWorkManager == nullptr)
	{
		return false;
	}

	mip::MeshTopology* p_mesh = nullptr;
	int meshCnt = m_pDataContext->m_MeshData.GetMeshCount();

	int nSelectedID = -1;
	int nSelectedCnt = 0;
	for (int ii = 0; ii < meshCnt; ii++)
	{
		MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(ii);

		if (pMeshInfo && pMeshInfo->selected)
		{
			nSelectedCnt++;
			nSelectedID = ii;
		}
	}

	if ((nSelectedID < 0 || nSelectedCnt != 1))
	{
		m_pMeshWorkManager->UpdateWorkMode(MESH_WORK_NONE, true);
		return false;
	}

	if (m_pMeshWorkManager->getWorkMode() == MESH_WORK_BRUSH_SELECTION || force)
	{
		for (int i = 0; i < meshCnt; ++i)
		{
			MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(i);

			if (!pMeshInfo || !pMeshInfo->selected)
			{
				continue;
			}

			p_mesh = m_pDataContext->m_MeshData.GetMesh(i);

			for (int ii = 0; ii < p_mesh->m_ttris.size(); ii++)
			{
				if (!p_mesh->m_ttris[ii].isS())
				{
					continue;
				}
				//if (p_mesh->m_ttris[ii].isD()) continue;
				p_mesh->m_ttris[ii].clearS();
			}

			MeshInfo* mInfo = m_pDataContext->m_MeshData.GetMeshInfo(i);
			mip::VECTOR4 color(mip::VECTOR4(mInfo->color.r / 255.0f, mInfo->color.g / 255.0f, mInfo->color.b / 255.0f, p_mesh->getAlphaVal() / 255.0f));
			p_mesh->updateColor(color);

			m_pRenderer->makeCurrent();
			p_mesh->buildRenderBufferTopology();
			m_pRenderer->doneCurrent();
			update();

		}
		printf_s("\n Clear Select Flag \n");
	}

	//m_pMeshWorkManager->UpdateWorkMode(MESH_WORK_NONE, true);
	printf_s("\n Id:%d brush Mode Release \n", nSelectedID);
	return false;
}

bool MEVolumeView::clearSelectFlag(mip::MeshTopology* pMT)
{
	for (int ii = 0; ii < pMT->m_ttris.size(); ii++)
	{
		pMT->m_ttris[ii].clearS();
	}

	//m_pRenderer->makeCurrent();
	//pMT->buildRenderBufferTopology();
	//m_pRenderer->doneCurrent();
	return true;
}
bool MEVolumeView::clearAllSelectFlag()
{
	mip::MeshTopology* p_mesh = nullptr;
	int meshCnt = m_pDataContext->m_MeshData.GetMeshCount();

	for (int i = 0; i < meshCnt; ++i)
	{
		MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(i);

		if (!pMeshInfo || !pMeshInfo->selected)
		{
			continue;
		}

		p_mesh = m_pDataContext->m_MeshData.GetMesh(i);
		if (p_mesh != nullptr)
		{
			clearSelectFlag(p_mesh);
		}
	}

	return false;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
