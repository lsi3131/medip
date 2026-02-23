#include "stdafx.h"
#include "CMeshModelViewManager.h"
#include "windowManager.h"
#include "Main/MainMeshWidget.h"
#include "MEVolumeView.h"
#include "ActionManager.h"
#include "stringManager.h"
#include "CMeshWorkManager.h"
#include "CMeshViewBtn3DScene.h"
#include "CManipulator.h"
#include "CMeshManipulator.h"
#include "MeshControl.h"

/*
@brief
*/
CMeshModelViewManager::CMeshModelViewManager() :
	m_pDataContext(nullptr),
	m_pMeshViewer(nullptr)
{
}


/*
@brief
*/
CMeshModelViewManager::~CMeshModelViewManager()
{

}

/*
@brief
@return
*/
CMeshModelViewManager* CMeshModelViewManager::getInstance()
{
	static CMeshModelViewManager instance;
	return &instance;
}

/*
@brief
@return
*/
void CMeshModelViewManager::Init(DataContext* pDataContext, MEVolumeView* pViewer)
{
	Init(pDataContext, pViewer,
		MESH_BTN_SCENE_MANAGER,
		MESH_WORK_MANAGER,
		MESH_MANIPULATOR,
		PLANE_MANIPULATOR,
		ACTION_MANAGER,
		WIN_MANAGER
	);
}

void CMeshModelViewManager::Init(
	DataContext* pDataContext,
	MEVolumeView* pViewer,
	CMeshViewBtn3DScene* btn3DScene,
	CMeshWorkManager* pWorkManager,
	CMeshManipulator* pMeshManipulator,
	CPlaneManiplator* pPlaneManipulator,
	ActionManager* pActionManager,
	WindowManager* pWinManager)
{
	m_pDataContext = pDataContext;
	m_btn3DScene = btn3DScene;
	m_pWorkManager = pWorkManager;
	m_pMeshManipulator = pMeshManipulator;
	m_pPlaneManipulator = pPlaneManipulator;
	m_pActionManager = pActionManager;
	m_pWinManager = pWinManager;

	SetView(pViewer);
}

void CMeshModelViewManager::Init(
	DataContext* pDataContext,
	CMeshViewBtn3DScene* btn3DScene,
	CMeshWorkManager* pWorkManager,
	CMeshManipulator* pMeshManipulator,
	CPlaneManiplator* pPlaneManipulator,
	ActionManager* pActionManager,
	WindowManager* pWinManager)
{
	m_pDataContext = pDataContext;
	m_btn3DScene = btn3DScene;
	m_pWorkManager = pWorkManager;
	m_pMeshManipulator = pMeshManipulator;
	m_pPlaneManipulator = pPlaneManipulator;
	m_pActionManager = pActionManager;
	m_pWinManager = pWinManager;
}

void CMeshModelViewManager::SetView(MEVolumeView* pViewer)
{
	m_pMeshViewer = pViewer;
	MeshFrontView();
}

/*
@brief
@return
*/
void CMeshModelViewManager::Reset()
{
	HomePosition();
}

void CMeshModelViewManager::HomePosition()
{
	float size = 10.0f;

	//m_camera.setPos(mip::VECTOR3(-3.2299f, 2.0845f, size * 2));//eye
	m_camera.setPos(mip::VECTOR3(0, 0, size * 2));

	m_camera.setZoomMax(size * 30);
	m_camera.setZoom(size * 7.0f);
	m_camera.setNearFar(0.1f, size * 8);//proj
	m_camera.setAt(mip::VECTOR3(0, 0, 0));
	//up -> 0,1,0

	m_TransformMain.zero();
}

void CMeshModelViewManager::SetPivotPoint(mip::VECTOR3 _point)
{
	m_bPivotPoint = true;
	m_PivotPoint = _point;
}

void CMeshModelViewManager::pckMeshModel(QMouseEvent* e, QPoint _mouse_pt, bool _b_shift)
{
	MESH_WORK_MODE mode = m_pWorkManager->getWorkMode();

	if (!ACTION_MANAGER->isActionFinished() || (mode == MESH_WORK_PLANE_CUT) || (mode == MESH_WORK_HOLE_FILL_SELECTED))
	{
		return;
	}

	int chkFound = -1;
	int prePckID = -1;

	float min_dist = FLT_MAX;

	float mouse_x = (float)_mouse_pt.x();
	float mouse_y = (float)_mouse_pt.y();

	int nMesh = m_pDataContext->m_MeshData.GetMeshCount();
	std::vector<bool> vecCopySelect(nMesh, false);
	for (int i = 0; i < nMesh; ++i)
	{
		MeshInfo* pInfo = m_pDataContext->m_MeshData.GetMeshInfo(i);
		if (pInfo && pInfo->selected)
		{
			vecCopySelect[i] = true;
		}
	}

	m_pDataContext->m_MeshData.ClearMeshInfoSelectMode();

	bool b_update = false;

	MEVolumeView* view = WIN_MANAGER->mainMeshWidget->getMainView();

	mip::MATRIX44 mat_offset;
	mip::VECTOR3 offset_center(-m_pDataContext->volume_data.getSizeX() * 0.5f, -m_pDataContext->volume_data.getSizeY() * 0.5f, -m_pDataContext->volume_data.getSizeZ() * 0.5f);

	mat_offset.identity();
	mat_offset.translation(offset_center);

	for (int i = 0; i < nMesh; i++)
	{
		MeshInfo* info = m_pDataContext->m_MeshData.GetMeshInfo(i);

		if (info)
		{
			if (info->show)
			{
				mip::MeshTopology* p_mesh = m_pDataContext->m_MeshData.GetMesh(i);

				if (p_mesh)
				{
					mip::MATRIX44 world_mat = GetMainTransform();
					mip::MATRIX44 mesh_mat = p_mesh->getMatrix();
					mip::MATRIX44 view_mat = m_camera.getView();
					mip::MATRIX44 proj_mat = m_camera.getProj();

					mesh_mat *= mat_offset;
					mesh_mat *= world_mat;

					mip::RAY ray = mip::geom::ScreenToRay(mip::VECTOR2(mouse_x, mouse_y), (float)view->width(), (float)view->height(),
						view_mat, proj_mat, &mesh_mat);

					int _vertIdx = -1;
					mip::VECTOR3 tracePoint(mip::VECTOR3(0, 0, 0));

					bool b_success = p_mesh->intersectRay(ray.org, ray.dir, tracePoint, _vertIdx, false, false);

					if (_vertIdx != -1)
					{
						chkFound = i;

						prePckID = m_pDataContext->m_MeshData.GetCurrentMeshIndex();

						m_pDataContext->m_MeshData.SetCurrentMeshIndex(i);

						break;
					}
				}
			}
		}
	}

	int pckID = m_pDataContext->m_MeshData.GetCurrentMeshIndex();

	if (chkFound != -1)
	{
		if (pckID >= 0)
		{
			MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(pckID);

			m_pDataContext->m_MeshData.SetMeshInfoModeSelectMode(pckID, pMeshInfo->selected ? false : true);

			// Update Pivot
			if (pMeshInfo->selected)
			{
				UpdatePivotPoint();

				m_btn3DScene->enableOpcityCtrl(true);
			}

			WIN_MANAGER->updateMeshTablist(pckID);
		}
	}

	if (chkFound == -1)
	{
		for (int i = 0; i < nMesh; i++)
		{
			MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(i);
			if (pMeshInfo)
			{
				pMeshInfo->selected = vecCopySelect[i];
			}

		}

		if (pckID >= nMesh)
		{
			m_pDataContext->m_MeshData.ClearMeshInfoSelectMode();

			if (m_pWorkManager->getWorkMode() >= MESH_WORK_FOR_SINGLE)
			{
				m_pWorkManager->UpdateWorkMode(MESH_WORK_NONE, true);
			}
		}
	}
}

void CMeshModelViewManager::updatePckMeshAll()
{
	int count = 0;
	int nMesh = m_pDataContext->m_MeshData.GetMeshCount();

	if (nMesh < 1)
	{
		return;
	}

	for (int i = 0; i < nMesh; ++i)
	{
		MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(i);
		if (pMeshInfo && pMeshInfo->selected)
		{
			count++;
		}
	}

	if (nMesh == count)
	{
		WIN_MANAGER->clearMeshTabSelection();

		m_pDataContext->m_MeshData.ClearMeshInfoSelectMode();

		m_pDataContext->m_MeshData.SetMeshInfoModeSelectMode(nMesh - 1, true);
	}
	else
	{
		for (int i = 0; i < nMesh; ++i)
		{
			m_pDataContext->m_MeshData.SetMeshInfoModeSelectMode(i, true);

			WIN_MANAGER->updateMeshTablist(i, false);
		}
	}

	// update pivot-point
	UpdatePivotPoint();
}

void CMeshModelViewManager::updatePckMesh(int _idx, bool _b_flag)
{
	int nMesh = m_pDataContext->m_MeshData.GetMeshCount();

	if (_idx >= nMesh || _idx < 0)
	{
		return;
	}

	m_pDataContext->m_MeshData.SetMeshInfoModeSelectMode(_idx, _b_flag);
}

void CMeshModelViewManager::MoveScreenCenterMesh(bool _b_all)
{
	if (m_pMeshViewer == nullptr)
	{
		return;
	}

	updatePivotPointGroup();
}

void CMeshModelViewManager::MoveScreenCenterMesh(int _idx)
{
}

void CMeshModelViewManager::UpdatePivotPoint(int _idx)
{
	UpdatePivotPoint();
}

void CMeshModelViewManager::UpdatePivotPoint()
{
	if (m_pDataContext == nullptr)
	{
		return;
	}

	// update pivot point
	mip::MeshTopology* p_mesh = m_pDataContext->m_MeshData.GetCurrentMesh();

	if (p_mesh)
	{
		mip::MATRIX44 mat_offset;
		mip::VECTOR3 offset_center(-m_pDataContext->volume_data.getSizeX() * 0.5f, -m_pDataContext->volume_data.getSizeY() * 0.5f, -m_pDataContext->volume_data.getSizeZ() * 0.5f);

		mat_offset.identity();
		mat_offset.translation(offset_center);

		mip::MATRIX44 mat_mesh = p_mesh->getMatrix() * mat_offset;
		mip::MATRIX44 mat_world = GetMainTransform();

		mip::AABB& bb = p_mesh->m_boundingBox;

		bb.min = mip::VECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);
		bb.max = mip::VECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

		if (p_mesh->m_verts.size() > 0)
		{
			//mip::mesh_control::getMinMax(p_mesh->m_verts, bb.min, bb.max);

			int count = 0;
			for (int i = 0; i < p_mesh->m_verts.size(); ++i)
			{
				if (p_mesh->m_verts[i] == mip::VECTOR3::Zero)
				{
					continue;
				}

				for (int j = 0; j < 3; j++)
				{
					if (p_mesh->m_verts[i][j] < bb.min[j])
					{
						bb.min[j] = p_mesh->m_verts[i][j];
					}

					if (p_mesh->m_verts[i][j] > bb.max[j])
					{
						bb.max[j] = p_mesh->m_verts[i][j];
					}
				}
			}
		}

		if (bb.min == mip::VECTOR3(FLT_MAX, FLT_MAX, FLT_MAX))
		{
			bb.min = mip::VECTOR3::Zero;
		}

		if (bb.max == mip::VECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX))
		{
			bb.max = mip::VECTOR3::Zero;
		}

		mip::VECTOR3 center_3d = bb.getCenter();

		mip::MATRIX44 mat_result;

		mat_result.identity();
		mat_result.translation(center_3d);

		mat_mesh *= mat_result;

		m_pMeshManipulator->UpdatePivotPoint(p_mesh, mat_mesh);

		mat_mesh *= mat_world;
		SetPivotPoint(mat_mesh.getOrigin());
	}
}

void CMeshModelViewManager::updatePivotPointGroup()
{
	int pck_count = 0;
	int n_mesh = m_pDataContext->m_MeshData.GetMeshCount();

	if (1 > n_mesh)
	{
		return;
	}

	// calculate group center
	mip::VECTOR3 center_group;

	for (int i = 0; i < n_mesh; ++i)
	{
		mip::MeshTopology* mesh = m_pDataContext->m_MeshData.GetMesh(i);
		MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(i);

		//if (mesh && WIN_MANAGER->vt_pckID[i])
		if (mesh && pMeshInfo && pMeshInfo->selected)
		{
			mip::VECTOR3 center_3d = mesh->m_boundingBox.getCenter();

			mip::MATRIX44 mat_center;
			mat_center.identity();
			mat_center.translation(center_3d);

			mip::MATRIX44 mat_world = mesh->getMatrix();
			mat_center *= mat_world;

			center_group += mat_center.getOrigin();
			pck_count++;
		}
	}

	center_group /= (float)pck_count;

	// update pivot
	SetPivotPoint(center_group);
}

bool CMeshModelViewManager::translateMeshModel()
{
	mip::VECTOR3 v1 = m_camera.getWorldPoint(0.5f);
	mip::VECTOR3 v2 = m_camera.getPreWorldPoint(0.5f);
	mip::VECTOR3 vd = v1 - v2;

	bool b_need_update = false;

	int n_mesh = m_pDataContext->m_MeshData.GetMeshCount();

	if (n_mesh < 1)
	{
		return false;
	}

	for (int i = 0; i < n_mesh; ++i)
	{
		mip::MeshTopology* mesh = m_pDataContext->m_MeshData.GetMesh(i);

		if (mesh)
		{
			b_need_update = true;
		}
	}

	if (b_need_update)
	{
		m_TransformMain.addTranslate(vd);
	}

	// pivot update 
	if (m_bPivotPoint)
	{
		m_PivotPoint += vd;
	}

	return b_need_update;
}

bool CMeshModelViewManager::rotateMeshModel(bool bShift)
{
	bool b_need_update = false;

	bool b_check_multi = false;

	int n_mesh = m_pDataContext->m_MeshData.GetMeshCount();

	if (n_mesh < 1)
	{
		return false;
	}

	int pck_count = 0;
	int pck_idx = -1;
	for (int i = 0; i < n_mesh; ++i)
	{
		MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(i);

		if (pMeshInfo && pMeshInfo->selected)
		{
			pck_count++;
			pck_idx = i;
		}
	}

	MESH_WORK_MODE mode = m_pWorkManager->getWorkMode();

	if (pck_count > 0)
	{
		for (int i = 0; i < n_mesh; ++i)
		{
			mip::MeshTopology* mesh = m_pDataContext->m_MeshData.GetMesh(i);

			if (mesh)
			{
				b_need_update = true;
			}
		}

		if (b_need_update)
		{
			if (!bShift)
			{
				TransformRotate(&m_TransformMain, &m_camera, &m_PivotPoint);
			}
			else
			{
				mip::VECTOR3 zero = mip::VECTOR3::Zero;
				TransformRotate(&m_TransformMain, &m_camera, &zero);
			}
		}
	}

	return b_need_update;
}

mip::VECTOR3 CMeshModelViewManager::getSceneCenter3d()
{
	if (m_pMeshViewer == nullptr)
	{
		return mip::VECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	}

	mip::VECTOR2 sz_screen = mip::VECTOR2(m_pMeshViewer->width(), m_pMeshViewer->height());
	mip::VECTOR2 center_xy = sz_screen * 0.5f;

	mip::MATRIX44 view_mat = m_camera.getView();
	mip::MATRIX44 proj_mat = m_camera.getProj();

	mip::VECTOR3 center = mip::geom::Screen2World(center_xy.x, center_xy.y, sz_screen.x, sz_screen.y, view_mat, proj_mat);

	return center;
}

mip::VECTOR2 CMeshModelViewManager::WorldToScreen(mip::VECTOR3 _pt, mip::MATRIX44* _world_mat)
{
	mip::MATRIX44 view_mat = m_camera.getView();
	mip::MATRIX44 proj_mat = m_camera.getProj();

	mip::VECTOR2 sz_screen = m_camera.getScreenSize();

	mip::VECTOR3 screen_pt = mip::geom::WorldToScreen(_pt, sz_screen.x, sz_screen.y, view_mat, proj_mat, _world_mat);

	return mip::VECTOR2(screen_pt.x, screen_pt.y);
}

mip::VECTOR3 CMeshModelViewManager::ScreenToWorld(mip::VECTOR2 _pt, mip::MATRIX44* _world_mat)
{
	mip::MATRIX44 view_mat = m_camera.getView();
	mip::MATRIX44 proj_mat = m_camera.getProj();

	mip::VECTOR2 sz_screen = m_camera.getScreenSize();

	mip::VECTOR3 world_pt = mip::geom::Screen2World(_pt.x, _pt.y, sz_screen.x, sz_screen.y, view_mat, proj_mat, _world_mat);

	return world_pt;
}

void CMeshModelViewManager::TransformRotate(
	mip::TRANSFORM* tr,
	const mip::SCAMERA* camera,
	const mip::VECTOR3* pPivot,
	mip::MATRIX44* pTransform)
{
	mip::VECTOR3 v1 = camera->getWorldPoint(0.5f, pTransform);
	mip::VECTOR3 v2 = camera->getPreWorldPoint(0.5f, pTransform);

	mip::VECTOR3 rV = (v1 - v2);
	int cx, cy, px, py;

	camera->getScreenSize(cx, cy);
	camera->getScreenXY(px, py);

	float dy = rV.y / (float)(cy);
	float dx = rV.x / (float)(cx);

	mip::MATRIX44 matInvCamera;
	matInvCamera = pTransform ? (*pTransform) * camera->getView() : camera->getView();
	matInvCamera.inverse();

	mip::QUATERNION q;
#ifdef USE_RIGHT_HAND
	mip::VECTOR3 Z = -matInvCamera.getScaledZaxis().normalize();
	if (pTransform == NULL)
	{
		dy = -dy;
	}
#else
	mip::VECTOR3 Z = matInvCamera.getScaledZaxis().normalize();
#endif
	if (px < (cx * 0.9f) && px > (cx * 0.1f))
	{
		mip::VECTOR3 rotV = rV;
		rotV.normalize();

		rotV = (rotV ^ Z).normalize();

		q.setRotationAxis(rotV, rV.length() * 0.05f);
	}
	else if (px > (cx * 0.9f))
	{
		q.setRotationAxis(Z, dy * 10.0f);
	}
	else if (px < (cx * 0.1f))
	{
		q.setRotationAxis(Z, -dy * 10.0f);
	}

	mip::MATRIX44 matRot;
	if (pPivot)
	{
		matRot = mip::math::MatrixAffineTransformation(1.f, *pPivot, q, mip::VECTOR3::Zero);
	}
	else
	{
		matRot = mip::math::MatrixAffineTransformation(1.f, mip::VECTOR3::Zero, q, mip::VECTOR3::Zero);
	}

	mip::MATRIX44 rot;
	mip::MATRIX44 mat_mesh = tr->getMatrix();
	mip::math::MatrixMultiply(&rot, &mat_mesh, &matRot);

	tr->addRotate(matRot.getQuaternion());
	tr->setTranslate(rot.getOrigin());
}

/*
@brief
@return
*/
mip::MATRIX44 CMeshModelViewManager::calcMatrixRotateFromPoint(
	mip::VECTOR3& _vec,
	mip::VECTOR3& _pt
)
{
	mip::TRANSFORM transform;
	mip::VECTOR3 euler;

	transform.zero();

	euler.x = float((double)_vec.x / 360. * (2.0 * M_PI));
	euler.y = float((double)_vec.y / 360. * (2.0 * M_PI));
	euler.z = float((double)_vec.z / 360. * (2.0 * M_PI));

	transform.addRotateZ(euler.z);
	transform.addRotateY(euler.y);
	transform.addRotateX(euler.x);

	mip::MATRIX44 mat_rot = mip::math::MatrixAffineTransformation(1.f, _pt, transform.getQuaternion(), mip::VECTOR3::Zero);

	return mat_rot;
}

void CMeshModelViewManager::MeshTopView()
{
	HomePosition();

	UpdatePivotPoint();

	if (m_pMeshViewer)
	{
		m_pMeshViewer->renderLater();
	}
}

void CMeshModelViewManager::MeshBottomView()
{
	HomePosition();

	m_TransformMain.addRotateX(M_PI);

	UpdatePivotPoint();

	if (m_pMeshViewer)
	{
		m_pMeshViewer->renderLater();
	}
}

void CMeshModelViewManager::MeshLeftView()
{
	HomePosition();

	m_TransformMain.addRotateZ(-90. * M_PI / 180.);
	m_TransformMain.addRotateX(-90. * M_PI / 180.);

	UpdatePivotPoint();

	if (m_pMeshViewer)
	{
		m_pMeshViewer->renderLater();
	}
}

void CMeshModelViewManager::MeshRightView()
{
	HomePosition();

	m_TransformMain.addRotateZ(90. * M_PI / 180.);
	m_TransformMain.addRotateX(-90. * M_PI / 180.);

	UpdatePivotPoint();

	if (m_pMeshViewer)
	{
		m_pMeshViewer->renderLater();
	}
}

void CMeshModelViewManager::MeshFrontView()
{
	HomePosition();

	m_TransformMain.addRotateX(-90. * M_PI / 180.);

	UpdatePivotPoint();

	if (m_pMeshViewer)
	{
		m_pMeshViewer->renderLater();
	}
}

void CMeshModelViewManager::MeshBackView()
{
	HomePosition();

	m_TransformMain.addRotateZ(M_PI);
	m_TransformMain.addRotateX(-90. * M_PI / 180.);

	UpdatePivotPoint();

	if (m_pMeshViewer)
	{
		m_pMeshViewer->renderLater();
	}
}

/*
@brief
@return
*/
void CMeshModelViewManager::MeshInitView(mip::TRANSFORM* _p_transform)
{
	_p_transform->rotation.x = 0.f;
	_p_transform->rotation.y = 0.f;
	_p_transform->rotation.z = 0.f;
	_p_transform->rotation.w = 1.f;

	_p_transform->translation.x = 0.f;
	_p_transform->translation.y = 0.f;
	_p_transform->translation.z = 0.f;
}

/*
@brief
@return
*/
void CMeshModelViewManager::initMeshTransform(int _idx)
{
	mip::MeshTopology* mesh = m_pDataContext->m_MeshData.GetMesh(_idx);

	if (mesh)
	{
		mesh->zero();
	}
}

/*
@brief
@return
*/
void CMeshModelViewManager::initMeshTransformAll()
{
	int n_mesh = m_pDataContext->m_MeshData.GetMeshCount();
	for (int midx = 0; midx < n_mesh; ++midx)
	{
		initMeshTransform(midx);
	}
}

/*
@brief
@return
*/
mip::VECTOR3 CMeshModelViewManager::getCetnerMeshes()
{
	int n_mesh = m_pDataContext->m_MeshData.GetMeshCount();

	mip::VECTOR3 center;
	for (int idx = 0; idx < n_mesh; ++idx)
	{
		mip::MeshTopology* mesh = m_pDataContext->m_MeshData.GetMesh(idx);
		mip::VECTOR3 bb_center = mesh->m_boundingBox.getCenter();

		center += bb_center;
	}

	center /= (float)n_mesh;

	return center;
}

/*
@brief
@return
*/
mip::VECTOR3 CMeshModelViewManager::getVolumeOffset()
{
	mip::VECTOR3 offset_center(-m_pDataContext->volume_data.getSizeX() * 0.5f, -m_pDataContext->volume_data.getSizeY() * 0.5f, -m_pDataContext->volume_data.getSizeZ() * 0.5f);

	return offset_center;
}

/*
@brief
@return
*/
void CMeshModelViewManager::OnReCalcZero()
{
	InitMainTransform();

	mip::MeshTopology* mesh = m_pDataContext->m_MeshData.GetCurrentMesh();

	m_pPlaneManipulator->UpdatePosition(mesh);

	UpdatePivotPoint();

	if (m_pMeshViewer)
	{
		m_pMeshViewer->renderLater();
	}
}


void CMeshModelViewManager::OnOpacityChange(int val)
{
	int n_mesh = m_pDataContext->m_MeshData.GetMeshCount();

	for (int i = 0; i < n_mesh; ++i)
	{
		mip::MeshTopology* mesh = m_pDataContext->m_MeshData.GetMesh(i);

		MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(i);

		//if (WIN_MANAGER->vt_pckID[i] && mesh)
		if (pMeshInfo && pMeshInfo->selected && mesh)
		{
			float opa_val = (float)val * 0.01f;

			mesh->setAlpha(opa_val * 255.f);
		}
	}

	if (m_pMeshViewer)
	{
		m_pMeshViewer->renderLater();
	}
}

mip::SCAMERA* CMeshModelViewManager::GetCameraPtr()
{
	return &m_camera;
}

mip::VECTOR3 CMeshModelViewManager::GetPivotPoint() const
{
	return m_PivotPoint;
}

bool CMeshModelViewManager::IsPivotPointFlagOn() const
{
	return m_bPivotPoint;
}

void CMeshModelViewManager::InitMainTransform()
{
	m_TransformMain.zero();
}

mip::MATRIX44 CMeshModelViewManager::GetMainTransform()
{
	return m_TransformMain.getMatrix();
}

CMeshWorkManager* CMeshModelViewManager::GetWorkManager() const
{
	return m_pWorkManager;
}

