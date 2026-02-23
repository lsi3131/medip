#include "stdafx.h"
#include "CMeshCutManager.h"
#include "windowManager.h"
#include "Actions/ActionManager.h"
#include "MeshControl.h"
#include "Renderer/Renderer.h"
#include "Main/MainMeshWidget.h"
#include "MEVolumeView.h"

#include "CMeshDlgManager.h"
#include "CMeshModelViewManager.h"
#include "CMeshViewBtn3DScene.h"
#include "windowManager.h"
#include "resourceManager.h"
#include "MedipQT.h"
#include "CMeshWorkManager.h"
#include "CManipulator.h"
#include "DataContext.h"

#include <cassert>
#include <Qmessagebox>
#include <Qstring>
/*
@brief
*/
CMeshCutManager::CMeshCutManager()
{
}

/*
@brief
*/
CMeshCutManager::~CMeshCutManager()
{

}
void CMeshCutManager::Init(DataContext* pDataContext, MEVolumeView* pViewer)
{
	Init(
		pDataContext,
		pViewer,
		MESH_BTN_SCENE_MANAGER,
		MESH_WORK_MANAGER,
		MESH_MODELVIEW_MANAGER,
		MESH_DIALOG_MANAGER,
		PLANE_MANIPULATOR
	);
}

void CMeshCutManager::Init(
	DataContext* pDataContext,
	MEVolumeView* pViewer,
	CMeshViewBtn3DScene* pBtn3DScene,
	CMeshWorkManager* pWorkManager,
	CMeshModelViewManager* pModelViewManager,
	CMeshDlgManager* pDlgManager,
	CPlaneManiplator* pPlaneManipulator)
{
	m_pDataContext = pDataContext;
	m_pViewer = pViewer;

	m_pBtn3DScene = pBtn3DScene;
	m_pWorkManager = pWorkManager;
	m_pModelViewManager = pModelViewManager;
	m_pDlgManager = pDlgManager;
	m_pPlaneManipulator = pPlaneManipulator;

	m_ActPlane = new QAction(QString("Plane cut mode ON"), m_pViewer);
	m_ActPolygon = new QAction(QString("Inner cut mode ON"), m_pViewer);
	m_ActPolyline = new QAction(QString("Outer cut mode ON"), m_pViewer);
	m_ActPolyFree = new QAction(QString("Free cut mode ON"), m_pViewer);


	m_ActPlane->setCheckable(true);
	m_ActPolygon->setCheckable(true);
	m_ActPolyline->setCheckable(true);
	m_ActPolyFree->setCheckable(true);


	m_ActPlane->setChecked(false);
	m_ActPolygon->setChecked(false);
	m_ActPolyline->setChecked(false);
	m_ActPolyFree->setChecked(false);


	connect(m_ActPlane, &QAction::triggered, m_pBtn3DScene, &CMeshViewBtn3DScene::OnPlanecut);
	connect(m_ActPolygon, &QAction::triggered, m_pBtn3DScene, &CMeshViewBtn3DScene::OnPolyInnercut);
	connect(m_ActPolyline, &QAction::triggered, m_pBtn3DScene, &CMeshViewBtn3DScene::OnPolyLinecut);
	connect(m_ActPolyFree, &QAction::triggered, m_pBtn3DScene, &CMeshViewBtn3DScene::OnPolyFreecut);

}

/*
@brief
@return
*/
CMeshCutManager* CMeshCutManager::getInstance()
{
	static CMeshCutManager instance;
	return &instance;
}

/*
@brief
@return
*/
bool	CMeshCutManager::Process(MESH_WORK_MODE _mode)
{
	bool b_success = false;

	if (ACTION_MANAGER->isActionFinished())
	{
		switch (_mode)
		{
		case MESH_WORK_PLANE_CUT:
			b_success = process_PlaneCut();
			break;
		case MESH_WORK_POLYGON_CUT:
			b_success = process_PolyInnerCut();
			break;
		case MESH_WORK_POLYLINE_CUT:
			b_success = process_PolyOutCut();
			break;
		case MESH_WORK_FREEPOLYLINE_CUT:
			b_success = process_FreePolyCut();
			break;
		}

		printf_s("\n process_Cut");
	}
	//if (b_success)
	//{
	//	auto view = WIN_MANAGER->mainMeshWidget->getMainView();
	//	view->updateGeometryCount();
	//}


	return b_success;
}

/*
@brief
@return
*/
void CMeshCutManager::Update(MESH_WORK_MODE _mode)
{
	int sz_icon = WIN_MANAGER->mainWindow->IconSize;

	static QIcon planeIcon[] = { RESOURCE_MANAGER->getIcon(ICON_NON_PLANE_CUT, sz_icon, sz_icon),
 RESOURCE_MANAGER->getIcon(ICON_PLANE_CUT, sz_icon, sz_icon) };
	static QIcon polyIcon[] = { RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_POLYROI, sz_icon, sz_icon),
 RESOURCE_MANAGER->getIcon(ICON_VIEW_POLYROI, sz_icon, sz_icon) };
	static QIcon polyFreeIcon[] = { RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_POLYFREE, sz_icon, sz_icon),
 RESOURCE_MANAGER->getIcon(ICON_VIEW_POLYFREE, sz_icon, sz_icon) };
	static QIcon lineIcon[] = { RESOURCE_MANAGER->getIcon(ICON_NON_ANNO_PROF_LINE, sz_icon, sz_icon),
 RESOURCE_MANAGER->getIcon(ICON_ANNO_PROF_LINE, sz_icon, sz_icon) };

	bool pre_cut = m_b_Cut;

	bool chkMode = false;
	bool chkRefresh = false;

	auto btn_cut = m_pBtn3DScene->m_BtnCut;
	auto btn_plane = m_pBtn3DScene->m_BtnPlane;
	auto btn_poly = m_pBtn3DScene->m_BtnPoly;
	auto btn_line = m_pBtn3DScene->m_BtnLine;
	auto btn_polyfree = m_pBtn3DScene->m_BtnPolyFree;

	if (m_ActPlane)
	{
		m_ActPlane->setChecked(_mode == MESH_WORK_PLANE_CUT);
		m_ActPlane->setText(QString("  Plane cut mode %1").arg(m_ActPlane->isChecked() ? QString("OFF") : QString("ON")));

		if (m_ActPlane->isChecked())
		{
			setPlaneCutFlag(false);
		}
	}
	else
	{
		chkMode = _mode == MESH_WORK_PLANE_CUT;
		btn_plane->setChecked(chkMode);
		btn_plane->setIcon(planeIcon[chkMode]);
	}

	if (m_ActPolygon)
	{
		m_ActPolygon->setChecked(_mode == MESH_WORK_POLYGON_CUT);
		m_ActPolygon->setText(QString("  Inner cut mode %1").arg(m_ActPolygon->isChecked() ? QString("OFF") : QString("ON")));
	}
	else
	{
		chkMode = _mode == MESH_WORK_POLYGON_CUT;
		btn_poly->setChecked(chkMode);
		btn_poly->setIcon(polyIcon[chkMode]);
	}

	if (m_ActPolyline)
	{
		m_ActPolyline->setChecked(_mode == MESH_WORK_POLYLINE_CUT);
		m_ActPolyline->setText(QString("  Outer cut mode %1").arg(m_ActPolyline->isChecked() ? QString("OFF") : QString("ON")));
	}
	else
	{
		chkMode = _mode == MESH_WORK_POLYLINE_CUT;
		btn_line->setChecked(chkMode);
		btn_line->setIcon(lineIcon[chkMode]);
	}

	if (m_ActPolyFree)
	{
		m_ActPolyFree->setChecked(_mode == MESH_WORK_FREEPOLYLINE_CUT);
		m_ActPolyFree->setText(QString("  Free cut mode %1").arg(m_ActPolyFree->isChecked() ? QString("OFF") : QString("ON")));
	}
	else
	{
		chkMode = _mode == MESH_WORK_FREEPOLYLINE_CUT;
		btn_polyfree->setChecked(chkMode);
		btn_polyfree->setIcon(polyFreeIcon[chkMode]);
	}

	setCutFlag(m_ActPolyline->isChecked() || m_ActPolygon->isChecked() || m_ActPlane->isChecked() || m_ActPolyFree->isChecked());

	if (pre_cut != m_b_Cut)
	{
		auto mode = m_pWorkManager->getWorkMode();

		btn_cut->setIcon(planeIcon[m_b_Cut]);
	}
}

/*
@brief
@return
*/
QAction* CMeshCutManager::getActionPtr(MESH_WORK_MODE _work)
{
	QAction* p_action = nullptr;
	switch (_work)
	{
	case MESH_WORK_PLANE_CUT:
		p_action = m_ActPlane;
		break;
	case MESH_WORK_POLYGON_CUT:
		p_action = m_ActPolygon;
		break;
	case MESH_WORK_POLYLINE_CUT:
		p_action = m_ActPolyline;
		break;
	case MESH_WORK_FREEPOLYLINE_CUT:
		p_action = m_ActPolyFree;
		break;
	}

	return p_action;
}

/*
@brief
@return
*/
void CMeshCutManager::setPlaneCutFlag(bool _b_flag)
{
	m_b_planeCut = _b_flag;
}

/*
@brief
@return
*/
bool* CMeshCutManager::getPlaneCutFlag()
{
	return &m_b_planeCut;
}

/*
@brief
@return
*/
void CMeshCutManager::setCutFlag(bool _b_flag)
{
	m_b_Cut = _b_flag;
}

/*
@brief
@return
*/
bool* CMeshCutManager::getCutFlag()
{
	return &m_b_Cut;
}

/*
@brief
@return
*/
std::vector<std::vector<std::vector<mip::VECTOR3>>>* CMeshCutManager::getHolePtr()
{
	return &m_vt_HolePts;
}

/*
@brief
@return
*/
QPolygon* CMeshCutManager::getPolyLinePtr()
{
	return &m_polyLine;
}



/*
@brief Plane-Cut 수행함수
@return
*/
bool CMeshCutManager::process_PlaneCut()
{
	if (!m_pDataContext || !m_pViewer)
	{
		return false;
	}

	const MESH_WORK_MODE	mode = m_pWorkManager->getWorkMode();

	mip::MeshTopology* p_mesh = nullptr;

	int n_mesh = m_pDataContext->m_MeshData.GetMeshCount();

	//if (n_mesh != WIN_MANAGER->vt_pckID.size())
	if (n_mesh < 1)
	{
		return false;
	}

	m_b_planeCut = true;

	m_vt_HolePts.clear();

	m_vt_HolePts.resize(n_mesh);

	//auto dlg = static_cast<CPlaneCutDlg*>(m_pDlgManager->getMeshDialog(MESH_DIALOG_PLANECUT));
	auto dlg = static_cast<CMeshCutDlg*>(m_pDlgManager->getMeshDialog(MESH_DIALOG_MESHCUT));

	auto type_fill = dlg->getTypeFilling();

	auto b_holefill = ((type_fill != 2) ? true : false);
	auto b_remesh_fill = false;
	auto b_smooth_fill = false;

	if (b_holefill)
	{
		b_remesh_fill = ((type_fill == 0) ? false : true);

		b_smooth_fill = b_remesh_fill;
	}

	std::vector<mip::MeshTopology*> vt_mesh;

	for (int i = 0; i < n_mesh; ++i)
	{
		MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(i);

		if (pMeshInfo && pMeshInfo->selected)
		{
			mip::MeshTopology* p_mesh = m_pDataContext->m_MeshData.GetMesh(i);

			vt_mesh.push_back(p_mesh);
		}
	}

	auto pck_index = m_pDataContext->m_MeshData.GetCurrentMeshIndex();
	p_mesh = m_pDataContext->m_MeshData.GetCurrentMesh();

	mip::MeshTopology* plane_mesh = m_pPlaneManipulator->getPlaneMeshPtr();
	mip::MATRIX44 plane_mat = m_pPlaneManipulator->getWorldMat();

	if (p_mesh)
	{
		const MESH_WORK_MODE mode = m_pWorkManager->getWorkMode();
		const MESH_WORK_MODE prev_mode = m_pWorkManager->getPrevWorkMode();

		ACTION_MANAGER->action_Mesh_Planecut(
			m_pDataContext,
			mode,
			prev_mode,
			plane_mesh,
			plane_mat,
			vt_mesh,
			&m_vt_HolePts[pck_index],
			b_holefill,
			b_remesh_fill,
			b_smooth_fill,
			m_pPlaneManipulator->isInverseZAxis()
		);
	}

	m_b_planeCut = false;

	return true;
}

/*
@brief Free-Poly Cut 수행함수
@return
*/
bool CMeshCutManager::process_FreePolyCut()
{
	if (!m_pDataContext || m_polyLine.size() < 4)
	{
		return  false;
	}

	const MESH_WORK_MODE mode = m_pWorkManager->getWorkMode();
	const MESH_WORK_MODE prev_mode = m_pWorkManager->getPrevWorkMode();

	mip::MeshTopology* p_mesh = nullptr;

	int n_mesh = m_pDataContext->m_MeshData.GetMeshCount();

	if (n_mesh < 1)
	{
		return false;
	}

	m_polyProcessCheck = true;

	int sz_polyline = (int)m_polyLine.size();

	std::vector<mip::VECTOR2>	vt_polygons;

	vt_polygons.reserve(sz_polyline);
	for (int i = 0; i < sz_polyline; ++i)
	{
		vt_polygons.push_back(mip::VECTOR2((float)m_polyLine[i].x(), (float)m_polyLine[i].y()));
	}

	ACTION_MANAGER->action_Mesh_Polycut(
		m_pDataContext,
		mode,
		prev_mode,
		vt_polygons
	);

	m_polyLine.clear();
	m_polyProcessCheck = false;

	return true;
}

/*
@brief 폐곡선 Out Cut 수행함수
@return
*/
bool CMeshCutManager::process_PolyOutCut()
{
	if (!m_pDataContext || m_polyLine.size() < 4)
	{
		return  false;
	}

	const MESH_WORK_MODE mode = m_pWorkManager->getWorkMode();
	const MESH_WORK_MODE prev_mode = m_pWorkManager->getPrevWorkMode();

	mip::MeshTopology* p_mesh = nullptr;

	int n_mesh = m_pDataContext->m_MeshData.GetMeshCount();

	if (n_mesh < 1)
	{
		assert(false);
		return false;
	}

	int sz_polyline = (int)m_polyLine.size();

	QPoint pt1 = m_polyLine[0];
	QPoint pt2 = m_polyLine[sz_polyline - 1];

	float dx = float(pt2.x() - pt1.x());
	float dy = float(pt2.y() - pt1.y());

	int dist = (int)(sqrt(pow(dx, 2) + pow(dy, 2)));

	if (dist > 10)
	{
		return false;
	}

	m_polyProcessCheck = true;

	int sz_line = (int)m_polyLine.size();

	std::vector<mip::VECTOR2> vt_polygons(sz_line);

	for (int i = 0; i < sz_line; ++i)
	{
		vt_polygons.push_back(mip::VECTOR2((float)m_polyLine[i].x(), (float)m_polyLine[i].y()));
	}

	ACTION_MANAGER->action_Mesh_Polycut(
		m_pDataContext,
		mode,
		prev_mode,
		vt_polygons
	);

	m_polyLine.clear();

	m_polyProcessCheck = false;

	return true;
}

/*
@brief 폐곡선 Inner Cut 수행함수
@return
*/
bool CMeshCutManager::process_PolyInnerCut()
{
	if (!m_pDataContext || m_polyLine.size() < 4)
	{
		return  false;
	}

	const MESH_WORK_MODE mode = m_pWorkManager->getWorkMode();
	const MESH_WORK_MODE prev_mode = m_pWorkManager->getPrevWorkMode();

	mip::MeshTopology* p_mesh = nullptr;

	int n_mesh = m_pDataContext->m_MeshData.GetMeshCount();

	if (n_mesh < 1)
	{
		return false;
	}

	int sz_polyline = (int)m_polyLine.size();

	QPoint pt1 = m_polyLine[0];
	QPoint pt2 = m_polyLine[sz_polyline - 1];

	float dx = float(pt2.x() - pt1.x());
	float dy = float(pt2.y() - pt1.y());

	int dist = (int)(sqrt(pow(dx, 2) + pow(dy, 2)));

	if (dist > 10)
	{
		return false;
	}

	m_polyProcessCheck = true;

	std::vector<mip::VECTOR2> vt_polygons;

	int sz_line = (int)m_polyLine.size();

	vt_polygons.reserve(sz_line);

	for (int i = 0; i < sz_line; ++i)
	{
		vt_polygons.push_back(mip::VECTOR2((float)m_polyLine[i].x(), (float)m_polyLine[i].y()));
	}

	ACTION_MANAGER->action_Mesh_Polycut(
		m_pDataContext,
		mode,
		prev_mode,
		vt_polygons
	);

	m_polyLine.clear();

	m_polyProcessCheck = false;

	return true;
}

/*
@brief Cut 종료함수
@return
*/
bool CMeshCutManager::process_FinishCut()
{
	if (!m_pDataContext)
	{
		return false;
	}

	const MESH_WORK_MODE mode = m_pWorkManager->getWorkMode();

	switch (mode)
	{
	case	MESH_WORK_PLANE_CUT:
	case 	MESH_WORK_POLYGON_CUT:
	case 	MESH_WORK_POLYLINE_CUT:
	case 	MESH_WORK_FREEPOLYLINE_CUT:
	{
		mip::MeshTopology* p_mesh = nullptr;

		int n_mesh = m_pDataContext->m_MeshData.GetMeshCount();

		if (n_mesh < 1)
		{
			assert(false);
			return false;
		}

		for (int i = 0; i < n_mesh; ++i)
		{
			MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(i);

			if (!pMeshInfo)
			{
				continue;
			}

			bool& pck_id = pMeshInfo->selected;

			if (pck_id && (mode >= MESH_WORK_FOR_SINGLE))
			{
				p_mesh = m_pDataContext->m_MeshData.GetMesh(i);
			}

			if ((nullptr == p_mesh) || !pck_id)
			{
				continue;
			}

			m_polygonsPlaneTest[0] = mip::VECTOR2(0.f, 0.f);
			m_polygonsPlaneTest[1] = mip::VECTOR2(0.f, 0.f);

			mip::mesh_control::MeshCutOK(p_mesh);

			p_mesh->updateVertex();
			p_mesh->updateColor(p_mesh->m_baseColor);

			m_pModelViewManager->UpdatePivotPoint();

			g_Renderer->makeCurrent();
			p_mesh->buildRenderBufferTopology();
			g_Renderer->doneCurrent();
		}

		m_polyLine.clear();

		m_b_planeCut = false;

		m_vt_HolePts.clear();
	}
	}

	return true;
}

/*
@brief Cut 취소함수
@return
*/
bool CMeshCutManager::process_CancelCut()
{
	if (!m_pDataContext)
	{
		return false;
	}

	const MESH_WORK_MODE mode = m_pWorkManager->getWorkMode();

	int n_mesh = m_pDataContext->m_MeshData.GetMeshCount();

	if (n_mesh < 1)
	{
		return false;
	}

	switch (mode)
	{
	case	MESH_WORK_PLANE_CUT:
	case 	MESH_WORK_POLYGON_CUT:
	case 	MESH_WORK_POLYLINE_CUT:
	case 	MESH_WORK_FREEPOLYLINE_CUT:
	{
		for (int i = 0; i < n_mesh; ++i)
		{
			mip::MeshTopology* p_mesh = nullptr;

			MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(i);

			if (!pMeshInfo)
			{
				continue;
			}

			bool& pck_id = pMeshInfo->selected;

			if (pck_id && (mode >= MESH_WORK_FOR_SINGLE))
			{
				p_mesh = m_pDataContext->m_MeshData.GetMesh(i);
			}

			if ((nullptr == p_mesh) || !pck_id)
			{
				continue;
			}

			if (m_vt_HolePts.size() > i)
			{
				m_vt_HolePts[i].clear();
			}

			mip::mesh_control::MeshCutCancel(p_mesh);

			p_mesh->updateVertex();
			p_mesh->updateColor(p_mesh->m_baseColor);

			g_Renderer->makeCurrent();
			p_mesh->buildRenderBufferTopology();
			g_Renderer->doneCurrent();
		}

		m_b_planeCut = false;

		m_polygonsPlaneTest[0] = mip::VECTOR2(0.f, 0.f);
		m_polygonsPlaneTest[1] = mip::VECTOR2(0.f, 0.f);
	}
	}

	return true;
}

/*
@brief
@return
*/
void CMeshCutManager::clearPlaneCutParam()
{
	if (!m_pDataContext)
	{
		return;
	}

	DWORD start, end;
	start = ::GetTickCount();
	mip::MeshTopology* p_mesh = nullptr;

	int pck_id = m_pDataContext->m_MeshData.GetCurrentMeshIndex();

	if ((-1 != pck_id))
	{
		p_mesh = m_pDataContext->m_MeshData.GetMesh(pck_id);
	}

	if (p_mesh)
	{
		if (m_b_planeCut || m_polyProcessCheck)
		{
			mip::mesh_control::MeshCutOK(p_mesh);

			p_mesh->updateVertex();
			p_mesh->updateColor(p_mesh->m_baseColor);

			g_Renderer->makeCurrent();
			p_mesh->buildRenderBufferTopology();
			g_Renderer->doneCurrent();

			m_b_planeCut = false;
			m_polyProcessCheck = false;
		}
	}

	m_polyLine.clear();

	m_vt_HolePts.clear();

	m_polygonsPlaneTest[0] = mip::VECTOR2(0.f, 0.f);
	m_polygonsPlaneTest[1] = mip::VECTOR2(0.f, 0.f);

	end = ::GetTickCount();
	//printf_s("Finish clearPlaneCutParam() : %lf\n", (double)(end - start) / 1000.);
}

/*
@brief
@return
*/
void CMeshCutManager::getPlane(mip::PLANE& _plane)
{
	if (!m_pDataContext)
	{
		return;
	}

	mip::MeshTopology* plane_mesh = m_pPlaneManipulator->getPlaneMeshPtr();
	mip::MATRIX44 plane_mat = m_pPlaneManipulator->getWorldMat();
	mip::MeshTopology* pick_mesh = m_pDataContext->m_MeshData.GetCurrentMesh();

	if (!pick_mesh)
	{
		return;
	}

	mip::MATRIX44 mesh_mat = pick_mesh->getMatrix();

	std::vector<mip::VECTOR3>  verts = plane_mesh->m_verts;

	for (int vi = 0; vi < verts.size(); ++vi)
	{
		verts[vi] = verts[vi].transform(plane_mat);
	}

	if (m_pPlaneManipulator->isInverseZAxis())
	{
		_plane = mip::PLANE(verts[0], verts[2], verts[1]);
	}
	else
	{
		_plane = mip::PLANE(verts[1], verts[2], verts[0]);
	}
}

/*
@brief
@return
*/
void CMeshCutManager::calcPolyLineFromPlane(std::vector<mip::VECTOR2>& _vt_polygon)
{
	auto plane_mesh = m_pPlaneManipulator->getPlaneMeshPtr();

	m_polyLine.clear();

	if (plane_mesh)
	{
		auto tverts = plane_mesh->m_tverts;
		auto n_tverts = (int)tverts.size();

		if (n_tverts == 4)
		{
			auto camera = m_pModelViewManager->GetCameraPtr();
			auto mat_view_world = m_pModelViewManager->GetMainTransform();

			// Front View로 변환
			mip::TRANSFORM tr;
			{
				tr.setRotate(mat_view_world.getQuaternion());
				tr.setTranslate(mat_view_world.getOrigin());

				m_pModelViewManager->MeshInitView(&tr);
				tr.addRotateX(-45.f * M_PI / 180.f);

				mat_view_world = tr.getMatrix();
			}

			mip::MATRIX44 mat_view = camera->getView();
			mip::MATRIX44 mat_proj = camera->getProj();
			mip::VECTOR2 sz_screen = camera->getScreenSize();
			mip::MeshTopology* plane_mesh = m_pPlaneManipulator->getPlaneMeshPtr();
			mip::MATRIX44 mat_world = plane_mesh->getMatrix();
			mip::QUATERNION q_plane = mat_world.getQuaternion();

			mip::MATRIX44  mat_offset;
			mip::VECTOR3  offset_center(-m_pDataContext->volume_data.getSizeX() * 0.5f, -m_pDataContext->volume_data.getSizeY() * 0.5f, -m_pDataContext->volume_data.getSizeZ() * 0.5f);

			mat_offset.identity();
			mat_offset.translation(offset_center);

			mat_world *= mat_offset;
			mat_world *= mat_view_world;

			mip::VECTOR3 line_2d[2];

			line_2d[0] = (tverts[0].pos + tverts[2].pos) * 0.5f;
			line_2d[1] = (tverts[1].pos + tverts[3].pos) * 0.5f;

			line_2d[0] = mip::geom::WorldToScreen(line_2d[0], sz_screen.x, sz_screen.y, mat_view, mat_proj, &mat_world);
			line_2d[1] = mip::geom::WorldToScreen(line_2d[1], sz_screen.x, sz_screen.y, mat_view, mat_proj, &mat_world);

			_vt_polygon.resize(2);

			m_polyLine.resize(2);

			bool b_invese_dir = m_pPlaneManipulator->isInverseZAxis();

			if (b_invese_dir)
			{
				_vt_polygon[1].x = line_2d[0].x;
				_vt_polygon[1].y = line_2d[0].y;

				_vt_polygon[0].x = line_2d[1].x;
				_vt_polygon[0].y = line_2d[1].y;
			}
			else
			{
				_vt_polygon[0].x = line_2d[0].x;
				_vt_polygon[0].y = line_2d[0].y;

				_vt_polygon[1].x = line_2d[1].x;
				_vt_polygon[1].y = line_2d[1].y;
			}

			float yaw, pitch, roll;
			q_plane.getYawPitchRoll(yaw, pitch, roll);

			if (abs(yaw) >= 90 && abs(yaw) <= 180)
			{
				auto tmp_polygon = _vt_polygon[0];

				_vt_polygon[0] = _vt_polygon[1];
				_vt_polygon[1] = tmp_polygon;
			}
		}
	}
}