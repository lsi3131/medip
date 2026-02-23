#include "stdafx.h"
#include "CManipulator.h"
#include "CMeshModelViewManager.h"
#include "CMeshDlgManager.h"
#include "CMeshWorkManager.h"

#include "WindowManager.h"
#include "Main/MainMeshWidget.h"
#include "MEVolumeView.h"
#include "MeshControl.h"
#include "Renderer/effect.h"
#include "Renderer/Renderer.h"
#include "Renderer/MeshTopology.h"
#include "Renderer/ShaderCommon.h"
#include "Renderer/ShaderMeshData.h"

#include "Math/Range.h"
#include "ActionManager.h"
#include "DataContext.h"
#include "Omniverse/OmniverseContext.h"

/*
@brief
*/
CManipulator::CManipulator(mip::Renderer* pRenderer) :
	m_pRenderer(pRenderer),
	m_Label(nullptr),
	m_Label1(nullptr),
	m_Label2(nullptr),
	m_p_camera(nullptr),
	m_pModelViewManager(nullptr),
	m_pMeshDlgManager(nullptr)
{
	for (int i = 0; i < NUM_AXIS_MANIPULATOR; ++i)
	{
		m_AxisMesh[i] = new mip::MeshTopology(m_pRenderer);

		CreateArrow(m_AxisMesh[i], (MANIPULATOR_DIR_TYPE)i);

		m_bShowFlag[i] = true;
		m_bUseFlag[i] = true;
	}

	m_AxisMesh[(int)INVERSE_Z_AXIS]->setAlpha(50.f);

	m_bShowFlag[NUM_AXIS_MANIPULATOR - 1] = false;
	m_bUseFlag[NUM_AXIS_MANIPULATOR - 1] = false;

	m_bShowFlag[int(TRANS_XY_AXIS)] = false;
	m_bShowFlag[int(TRANS_YZ_AXIS)] = false;
	m_bShowFlag[int(TRANS_XZ_AXIS)] = false;
	m_bUseFlag[int(TRANS_XY_AXIS)] = false;
	m_bUseFlag[int(TRANS_YZ_AXIS)] = false;
	m_bUseFlag[int(TRANS_XZ_AXIS)] = false;

	m_SphereCube = new mip::MeshTopology(m_pRenderer);
	CreateSphere(m_SphereCube, mip::VECTOR4(0, 255, 0, 255));

	m_Sphere = new mip::MeshTopology(m_pRenderer);
	CreateSphere(m_Sphere, mip::VECTOR4(255, 0, 0, 255));

	for (int i = 0; i < NUM_Plane_SNAPPING; ++i)
	{
		m_SnappingPlane[i] = new mip::MeshTopology(m_pRenderer);
		CreatePlane(m_SnappingPlane[i], (MANIPULATOR_DIR_TYPE)(i + 3), LEN_PLANE_LINE);
	}

	CreateRotateSnappingPoint(m_vt_RotateSnappingPoint);

	m_bSnapping = true;
	m_bFinished = true;

	m_bUpdatePivot = false;
	m_bBack_Selected = false;

	m_nCount = 0;

	m_Transform.zero();
	m_InitTransform.zero();
	m_TransformTemp.zero();

	m_Axis_Seleted = NONE_AXIS;

	m_bInverseZAxis = false;

	m_bUpdateCtrl = true;

	m_mat_view_world.identity();
	m_mat_offset.identity();
}

/*
@brief
*/
CManipulator::~CManipulator()
{
	//for (int i = 0; i < NUM_AXIS_MANIPULATOR; ++i)
	//{
	//	if (m_AxisMesh[i])
	//	{
	// SAFE_DELETE(m_AxisMesh[i]);
	//	}
	//}

	//for (int i = 0; i < NUM_Plane_SNAPPING; ++i)
	//{
	//	if (m_SnappingPlane[i])
	//	{
	// SAFE_DELETE(m_SnappingPlane[i]);
	//	}
	//}

	//if (m_SphereCube)
	//{
	//	SAFE_DELETE(m_SphereCube);
	//}
	//
	//if (m_SphereCube)
	//{
	//	SAFE_DELETE(m_Sphere);
	//}
}

void CManipulator::Init()
{
	Init(MESH_MODELVIEW_MANAGER, MESH_DIALOG_MANAGER);
}

void CManipulator::Init(CMeshModelViewManager* pModelViewManager, CMeshDlgManager* pMeshDlgManager)
{
	m_pModelViewManager = pModelViewManager;
	m_pMeshDlgManager = pMeshDlgManager;
}

/*
@brief
@return
*/
void CManipulator::Reset()
{
	auto mat_init = m_InitTransform.getMatrix();

	for (int i = 0; i < NUM_AXIS_MANIPULATOR; ++i)
	{
		if (m_AxisMesh[i])
		{
			m_AxisMesh[i]->setRotate(mat_init.getQuaternion());
			m_AxisMesh[i]->setTranslate(mat_init.getOrigin());
		}
	}

	m_Transform.zero();

	m_TransAccumulate = mip::VECTOR3();
	m_RotAngleAccumulate = mip::VECTOR3();

	m_bUpdatePivot = false;
}

/*
@brief
@return
*/
void CManipulator::HomePosition()
{
	for (int i = 0; i < NUM_AXIS_MANIPULATOR; ++i)
	{
		if (m_AxisMesh[i])
		{
			SAFE_DELETE(m_AxisMesh[i]);
		}

		m_AxisMesh[i] = new mip::MeshTopology(m_pRenderer);

		CreateArrow(m_AxisMesh[i], (MANIPULATOR_DIR_TYPE)i);
	}

	m_bUpdatePivot = false;
}

/*
@brief
@return
*/
void CManipulator::Process(
	DataContext* pDataContext,
	mip::MeshTopology* _p_mesh,
	QPoint  _prev_mouse_pt,
	QPoint  _cur_mouse_pt
)
{
	if (!pDataContext)
	{
		return;
	}

	m_bFinished = false;

	if (m_nCount == 0)
	{
		//ACTION_MANAGER->action_UndoRedo_update();

		m_PrevTransform.setRotate(_p_mesh->rotation);
		m_PrevTransform.setTranslate(_p_mesh->translation);
		m_PrevTransform.setScale(_p_mesh->scale);

		for (int i = 0; i < NUM_AXIS_MANIPULATOR; ++i)
		{
			m_PrevAxisTransform[i].setRotate(m_AxisMesh[i]->rotation);
			m_PrevAxisTransform[i].setTranslate(m_AxisMesh[i]->translation);
			m_PrevAxisTransform[i].setScale(m_AxisMesh[i]->scale);
		}
	}

	m_CurrentMousePt = _cur_mouse_pt;
	m_PrevMousePt = _prev_mouse_pt;

	switch (m_Axis_Seleted)
	{
	case TRANS_X_AXIS:
	case TRANS_Y_AXIS:
	case TRANS_Z_AXIS:
	case TRANS_XY_AXIS:
	case TRANS_YZ_AXIS:
	case TRANS_XZ_AXIS:
	{
		processTranslate(_p_mesh);
	}
	break;
	case ROTATE_X_AXIS:
	case ROTATE_Y_AXIS:
	case ROTATE_Z_AXIS:
	{
		processRotate(pDataContext, _p_mesh);
	}
	break;

	case SCALE_X_AXIS:
	case SCALE_Y_AXIS:
	case SCALE_Z_AXIS:
	case SCALE_ALL_AXIS:
	{
		processScale(_p_mesh);
	}
	break;
	}

	m_nCount++;
}

/*
@brief
@return
*/
void CManipulator::ReadyProcess(
	QPoint& _prev_mouse_pt,
	QPoint& _cur_mouse_pt,
	mip::MATRIX44& _mat_view_world,
	mip::MATRIX44& _mat_offset,
	mip::SCAMERA* _p_camera
)
{
	m_InitMousePt = _cur_mouse_pt;
	m_CurrentMousePt = _cur_mouse_pt;
	m_PrevMousePt = _prev_mouse_pt;

	m_Axis_Prev = m_Axis_Seleted;

	m_mat_view_world = _mat_view_world;
	m_mat_offset = _mat_offset;

	m_p_camera = _p_camera;

	initSnapping();

	initSphere();

	initPlane();

	initCtrl();

	updateCtrl();

	initParams();
}

/*
@brief
@return
*/
bool CManipulator::FinishProcess(mip::MeshTopology* _p_mesh)
{
	bool b_check = false;
	if (!m_bFinished)
	{
		if (m_Axis_Seleted == INVERSE_Z_AXIS)
		{
			setInverseZAxis();
		}

		//201102 허 건대리 Undo / Redo
		//SaveUndoRedo(_p_mesh);

		m_bFinished = true;

		m_nCount = 0;

		m_Label->hide();
		m_Label1->hide();
		m_Label2->hide();

		b_check = true;
	}

	return b_check;
}

/*
@brief
@return
*/
void CManipulator::OkProcess(mip::MeshTopology* _p_mesh)
{
	//SaveUndoRedo(_p_mesh);

	FinishProcess(_p_mesh);
}

/*
@brief
@return
*/
void CManipulator::CancelProcess(mip::MeshTopology* _p_mesh)
{
	_p_mesh->setRotate(m_InitTransform.rotation);
	_p_mesh->setTranslate(m_InitTransform.translation);
	_p_mesh->setScale(m_InitTransform.scale);

	_p_mesh->m_boundingBox = m_InitBB;

	//auto org_rot = _p_mesh->rotation;
	//auto org_trans = _p_mesh->translation;
	//auto org_scale = _p_mesh->scale;

	//if (org_rot.x == m_PrevTransform.rotation.x &&
	//	org_rot.y == m_PrevTransform.rotation.y &&
	//	org_rot.z == m_PrevTransform.rotation.z &&
	//	org_trans.x == m_PrevTransform.translation.x &&
	//	org_trans.y == m_PrevTransform.translation.y &&
	//	org_trans.z == m_PrevTransform.translation.z &&
	//	org_scale.x == m_PrevTransform.scale.x &&
	//	org_scale.y == m_PrevTransform.scale.y &&
	//	org_scale.z == m_PrevTransform.scale.z
	//	)
	//{
	//	return;
	//}

	//if (org_rot.x == m_PrevTransform.rotation.x &&
	//	org_rot.y == m_PrevTransform.rotation.y &&
	//	org_rot.z == m_PrevTransform.rotation.z &&
	//	org_trans.x == m_PrevTransform.translation.x &&
	//	org_trans.y == m_PrevTransform.translation.y &&
	//	org_trans.z == m_PrevTransform.translation.z &&
	//	org_scale.x == m_PrevTransform.scale.x &&
	//	org_scale.y == m_PrevTransform.scale.y &&
	//	org_scale.z == m_PrevTransform.scale.z
	//	)
	//{
	//	return;
	//}

	//_p_mesh->setRotate(m_PrevTransform.rotation);
	//_p_mesh->setTranslate(m_PrevTransform.translation);
	//_p_mesh->setScale(m_PrevTransform.scale);

	//_p_mesh->m_boundingBox = m_PrevBB;

	for (int idx = 0; idx < NUM_AXIS_MANIPULATOR; ++idx)
	{
		m_AxisMesh[idx]->setRotate(m_InitAxisTransform[idx].rotation);
		m_AxisMesh[idx]->setTranslate(m_InitAxisTransform[idx].translation);
		m_AxisMesh[idx]->setScale(m_InitAxisTransform[idx].scale);
	}

	m_Transform = m_InitTransform;

	//UpdatePrevTransform(_p_mesh);
}

/*
@brief
@return
*/
void CManipulator::SaveUndoRedo(mip::MeshTopology* _p_mesh)
{
	auto org_rot = _p_mesh->rotation;
	auto org_trans = _p_mesh->translation;
	auto org_scale = _p_mesh->scale;

	if (org_rot.x == m_PrevTransform.rotation.x &&
		org_rot.y == m_PrevTransform.rotation.y &&
		org_rot.z == m_PrevTransform.rotation.z &&
		org_trans.x == m_PrevTransform.translation.x &&
		org_trans.y == m_PrevTransform.translation.y &&
		org_trans.z == m_PrevTransform.translation.z &&
		org_scale.x == m_PrevTransform.scale.x &&
		org_scale.y == m_PrevTransform.scale.y &&
		org_scale.z == m_PrevTransform.scale.z
		)
	{
		return;
	}

	_p_mesh->setRotate(m_PrevTransform.rotation);
	_p_mesh->setTranslate(m_PrevTransform.translation);
	_p_mesh->setScale(m_PrevTransform.scale);

	//201102 허 건대리 Undo / Redo
	//ACTION_MANAGER->action_UndoRedo_update(MESH_WORK_MANIFULATE);

	_p_mesh->setRotate(org_rot);
	_p_mesh->setTranslate(org_trans);
	_p_mesh->setScale(org_scale);

	UpdatePrevTransform(_p_mesh);
}

/*
@brief
@return
*/
void CManipulator::render(
	DataContext* pDataContext,
	mip::Renderer* _p_renderer,
	mip::MATRIX44& _mat_view_world,
	mip::MATRIX44& _mat_offset,
	mip::SCAMERA* _p_camera)
{
	if (_p_camera)
	{
		m_mat_view_world = _mat_view_world;
		m_mat_offset = _mat_offset;
		m_p_camera = _p_camera;

		DrawPlane();

		//DrawCube();
		DrawArrow(pDataContext, _p_renderer);
		//DrawCircle();

		DrawSphere();
	}
}

/*
@brief
@return
*/
bool CManipulator::CheckClickedArrow(DataContext* pDataContext, QPoint _pt, mip::SCAMERA* _p_camera)
{
	bool b_clicked = false;

	m_Axis_Seleted = NONE_AXIS;

	bool b_back = false;

	mip::VECTOR2 sz_screen = _p_camera->getScreenSize();

	mip::MATRIX44 mat_cam_view = _p_camera->getView();
	mip::MATRIX44 mat_cam_proj = _p_camera->getProj();

	float zoomFactor = _p_camera->getZoom();
	float zoomRatio = zoomFactor / DEFAULT_ZOOM_FACTOR;

	// check intersect using ray
	for (int i = 0; i < NUM_AXIS_MANIPULATOR; ++i)
	{
		mip::VECTOR3 scale = m_AxisMesh[i]->scale;

		m_AxisMesh[i]->setScale(mip::VECTOR3(zoomRatio, zoomRatio, zoomRatio));

		mip::MATRIX44 mat_mesh = m_AxisMesh[i]->getMatrix();

		mat_mesh *= m_mat_offset;
		mat_mesh *= m_mat_view_world;

		mip::RAY ray = mip::geom::ScreenToRay(
			mip::VECTOR2((float)_pt.x(), (float)_pt.y()),
			sz_screen[0], sz_screen[1],
			mat_cam_view, mat_cam_proj, &mat_mesh);


		int _vertIdx = -1;
		mip::VECTOR3 tracePoint(mip::VECTOR3(0, 0, 0));

		bool b_success = m_AxisMesh[i]->intersectRay(ray.org, ray.dir, tracePoint, _vertIdx, false, false);

		if (_vertIdx == -1)
		{
			b_success = m_AxisMesh[i]->intersectRay(ray.org, ray.dir, tracePoint, _vertIdx, false, true);

			if (_vertIdx != -1)
			{
				b_back = true;
			}
		}

		m_AxisMesh[i]->setScale(scale);

		if (_vertIdx != -1)
		{
			b_clicked = b_success;

			m_Axis_Seleted = (MANIPULATOR_DIR_TYPE)i;

			break;
		}
	}

	m_bBack_Selected = b_back;

	// 210318 허 건 과장
	// 210318 기능개선전까지 임시코드 삽입
	if (!m_bShowFlag[(int)m_Axis_Seleted] || !m_bUseFlag[(int)m_Axis_Seleted])
	{
		b_clicked = false;
		m_Axis_Seleted = NONE_AXIS;
	}

	if (pDataContext != nullptr)
	{
		OmniverseContext* pOmniverse = pDataContext->GetOmniverseContext();
		if (pOmniverse->GetStage()->IsOpen())
		{
			switch (m_Axis_Seleted)
			{
			case ROTATE_X_AXIS:
			case ROTATE_Y_AXIS:
			case ROTATE_Z_AXIS:
				m_Axis_Seleted = NONE_AXIS;
				b_clicked = false;
				break;
			}
		}
	}

	return b_clicked;
}

/*
@brief
@return
*/
void	CManipulator::CreateArrow(
	mip::MeshTopology* _mesh,
	MANIPULATOR_DIR_TYPE _axis
)
{
	mip::VECTOR4 color;

	switch (_axis)
	{
	case TRANS_X_AXIS:
	case TRANS_Y_AXIS:
	case TRANS_Z_AXIS:
	{
		CreateCommonArrow(_mesh);
	}
	break;
	case TRANS_XY_AXIS:
	case TRANS_YZ_AXIS:
	case TRANS_XZ_AXIS:
	{
		CreateTriArrow(_mesh);
	}
	break;
	case ROTATE_X_AXIS:
	case ROTATE_Y_AXIS:
	case ROTATE_Z_AXIS:
	{
		CreateRotateArrow(_mesh);
	}
	break;
	case SCALE_X_AXIS:
	case SCALE_Y_AXIS:
	case SCALE_Z_AXIS:
	{
		CreateQuadArrow(_mesh);
	}
	break;
	case SCALE_ALL_AXIS:
	{
		CreateCube(_mesh);
	}
	break;
	case INVERSE_Z_AXIS:
	{
		CreateInverseZArrow(_mesh);
	}
	break;
	}

	mip::AABB bbox;
	mip::mesh_control::getMinMax(_mesh->m_verts, bbox.min, bbox.max);

	switch (_axis)
	{
	case TRANS_X_AXIS:
	{
		RotateFromPoint(_mesh, mip::VECTOR3(180.f, 0.f, 0.f), bbox.getCenter());
		RotateFromPoint(_mesh, mip::VECTOR3(0.f, 0.f, 90.f), bbox.getCenter());

		Translate(_mesh, mip::VECTOR3(2.25f, 1.1f, 0.f));
	}
	break;
	case TRANS_Y_AXIS:
	{
		RotateFromPoint(_mesh, mip::VECTOR3(180.f, 0.f, 0.f), bbox.getCenter());
		Translate(_mesh, mip::VECTOR3(0.f, -1.1f, 0.f));
	}
	break;
	case TRANS_Z_AXIS:
	{
		RotateFromPoint(_mesh, mip::VECTOR3(90.f, 0.f, 0.f), bbox.getCenter());
		Translate(_mesh, mip::VECTOR3(0.f, 1.1f, 2.25f));
	}
	break;
	case TRANS_XY_AXIS:
	{
		RotateFromPoint(_mesh, mip::VECTOR3(180.f, 0.f, 0.f), bbox.getCenter());

		Translate(_mesh, mip::VECTOR3(0.4f, -1.4f, 0.f));
	}
	break;
	case TRANS_YZ_AXIS:
	{
		RotateFromPoint(_mesh, mip::VECTOR3(0.f, 270.f, 0.f), bbox.getCenter());
		RotateFromPoint(_mesh, mip::VECTOR3(0.f, 0.f, 180.f), bbox.getCenter());

		Translate(_mesh, mip::VECTOR3(-0.5f, -1.3f, 0.75f));
	}
	break;
	case TRANS_XZ_AXIS:
	{
		RotateFromPoint(_mesh, mip::VECTOR3(0.f, 270.f, 0.f), bbox.getCenter());
		RotateFromPoint(_mesh, mip::VECTOR3(0.f, 0.f, 270.f), bbox.getCenter());

		Translate(_mesh, mip::VECTOR3(0.5f, -0.5f, 0.75f));
	}
	break;
	case SCALE_X_AXIS:
	{
		Translate(_mesh, mip::VECTOR3(4.6f, -0.35f, 0.f));
	}
	break;
	case SCALE_Y_AXIS:
	{
		Translate(_mesh, mip::VECTOR3(0.f, -4.9f, 0.f));
	}
	break;
	case SCALE_Z_AXIS:
	{
		RotateFromPoint(_mesh, mip::VECTOR3(90.f, 0.f, 0.f), bbox.getCenter());
		Translate(_mesh, mip::VECTOR3(0.f, -0.25f, 4.6f));
	}
	break;
	case ROTATE_X_AXIS:
	{
		RotateFromPoint(_mesh, mip::VECTOR3(0.f, 0.f, 315.f), bbox.getCenter());
		RotateFromPoint(_mesh, mip::VECTOR3(0.f, 270.f, 0.f), bbox.getCenter());
		RotateFromPoint(_mesh, mip::VECTOR3(65.f, 0.f, 0.f), bbox.getCenter());

		Translate(_mesh, mip::VECTOR3(-0.15f, -1.6f, 1.2f));
	}
	break;
	case ROTATE_Y_AXIS:
	{
		RotateFromPoint(_mesh, mip::VECTOR3(90.f, 0.f, 0.f), bbox.getCenter());
		RotateFromPoint(_mesh, mip::VECTOR3(0.f, 25.f, 0.f), bbox.getCenter());

		Translate(_mesh, mip::VECTOR3(1.25f, -0.5f, 1.25f));
	}
	break;
	case ROTATE_Z_AXIS:
	{
		RotateFromPoint(_mesh, mip::VECTOR3(0.f, 0.f, 250.f), bbox.getCenter());

		Translate(_mesh, mip::VECTOR3(1.2f, -1.7f, 0.f));
	}
	break;
	case INVERSE_Z_AXIS:
	{
		RotateFromPoint(_mesh, mip::VECTOR3(-90.f, 0.f, 0.f), bbox.getCenter());

		Translate(_mesh, mip::VECTOR3(-10.f, 0.2f, -1.2f));
	}
	break;
	}

	_mesh->mergingVertex();
	_mesh->buildTopologyHEdge();
	_mesh->buildTree();

	//_mesh->setAlpha(125.f); 
	_mesh->setAlpha(200.f);
	_mesh->enableAlpha(true);
	_mesh->initShader(mip::SHADERTYPE::SHADER_BASIC);

	_mesh->updateVertex();
	WIN_MANAGER->buildRenderBufferTopology(_mesh);
}

/*
@brief
@return
*/
void	CManipulator::CreateRotateArrow(mip::MeshTopology* _mesh)
{
	std::vector<mip::VECTOR3> vt_verts;
	std::vector<muint32>	  vt_idx;

	//const double pi2 = M_PI * 2.0;
	const double pi2 = M_PI * 0.75;
	int nside = 32;

	float r = 0.7f;

	double pos0 = 0.0;
	double pos1 = 1.0;

	float x0 = float(cos(pos0 * pi2 / nside));
	float y0 = float(sin(pos0 * pi2 / nside));
	float x1 = float(cos(pos1 * pi2 / nside));
	float y1 = float(sin(pos1 * pi2 / nside));

	float rx0 = x0 * r;
	float ry0 = y0 * r;
	float rx1 = x1 * r;
	float ry1 = y1 * r;

	vt_verts.push_back(mip::VECTOR3(rx0, ry0, 0.f));
	vt_verts.push_back(mip::VECTOR3(x0, y0, 0.f));
	vt_verts.push_back(mip::VECTOR3(rx1, ry1, 0.f));
	vt_verts.push_back(mip::VECTOR3(x1, y1, 0.f));

	vt_idx.push_back(0);
	vt_idx.push_back(1);
	vt_idx.push_back(2);

	vt_idx.push_back(1);
	vt_idx.push_back(3);
	vt_idx.push_back(2);

	for (double i = 2; i < nside; i++)
	{
		pos0 = double(i);

		x0 = float(cos(pos0 * pi2 / nside));
		y0 = float(sin(pos0 * pi2 / nside));

		rx0 = x0 * r;
		ry0 = y0 * r;

		vt_verts.push_back(mip::VECTOR3(rx0, ry0, 0.f));
		vt_verts.push_back(mip::VECTOR3(x0, y0, 0.f));

		int sz_idx = vt_verts.size();

		int idx0 = sz_idx - 4;
		int idx1 = sz_idx - 3;
		int idx2 = sz_idx - 2;
		int idx3 = sz_idx - 1;

		vt_idx.push_back(idx0);
		vt_idx.push_back(idx1);
		vt_idx.push_back(idx2);

		vt_idx.push_back(idx1);
		vt_idx.push_back(idx3);
		vt_idx.push_back(idx2);
	}

	_mesh->m_verts.swap(vt_verts);
	_mesh->m_tris.swap(vt_idx);
}

/*
@brief
@return
*/
void	CManipulator::CreateCommonArrow(mip::MeshTopology* _mesh)
{
	std::vector<mip::VECTOR3> vt_verts;
	std::vector<muint32>	  vt_idx;

	vt_verts.push_back(mip::VECTOR3(10.f, 0.f, 0.f) * 0.08f);
	vt_verts.push_back(mip::VECTOR3(-10.f, 0.f, 0.f) * 0.08f);
	vt_verts.push_back(mip::VECTOR3(0.f, 10.f, 0.f) * 0.08f);

	vt_verts.push_back(mip::VECTOR3(5.f, 0.f, 0.f) * 0.05f);
	vt_verts.push_back(mip::VECTOR3(-5.f, 0.f, 0.f) * 0.05f);
	vt_verts.push_back(mip::VECTOR3(-5.f, -30.f, 0.f) * 0.05f);
	vt_verts.push_back(mip::VECTOR3(5.f, -30.f, 0.f) * 0.05f);

	vt_verts.push_back(mip::VECTOR3(-5.f, -60.f, 0.f) * 0.05f);
	vt_verts.push_back(mip::VECTOR3(5.f, -60.f, 0.f) * 0.05f);

	vt_idx.push_back(0);
	vt_idx.push_back(2);
	vt_idx.push_back(1);

	vt_idx.push_back(3);
	vt_idx.push_back(4);
	vt_idx.push_back(5);

	vt_idx.push_back(3);
	vt_idx.push_back(5);
	vt_idx.push_back(6);

	vt_idx.push_back(6);
	vt_idx.push_back(5);
	vt_idx.push_back(7);

	vt_idx.push_back(6);
	vt_idx.push_back(7);
	vt_idx.push_back(8);

	_mesh->m_verts.swap(vt_verts);
	_mesh->m_tris.swap(vt_idx);
}

/*
@brief
@return
*/
void	CManipulator::CreateInverseZArrow(mip::MeshTopology* _mesh)
{
	std::vector<mip::VECTOR3> vt_verts;
	std::vector<muint32>	  vt_idx;

	vt_verts.push_back(mip::VECTOR3(10.f, 0.f, 0.f) * 0.08f);
	vt_verts.push_back(mip::VECTOR3(-10.f, 0.f, 0.f) * 0.08f);
	vt_verts.push_back(mip::VECTOR3(0.f, 10.f, 0.f) * 0.08f);

	vt_verts.push_back(mip::VECTOR3(5.f, 0.f, 0.f) * 0.05f);
	vt_verts.push_back(mip::VECTOR3(-5.f, 0.f, 0.f) * 0.05f);
	vt_verts.push_back(mip::VECTOR3(-5.f, -30.f, 0.f) * 0.05f);
	vt_verts.push_back(mip::VECTOR3(5.f, -30.f, 0.f) * 0.05f);

	//vt_verts.push_back(mip::VECTOR3(-5.f, -60.f, 0.f) * 0.05f);
	//vt_verts.push_back(mip::VECTOR3(5.f, -60.f, 0.f) * 0.05f);

	vt_idx.push_back(0);
	vt_idx.push_back(2);
	vt_idx.push_back(1);

	vt_idx.push_back(3);
	vt_idx.push_back(4);
	vt_idx.push_back(5);

	vt_idx.push_back(3);
	vt_idx.push_back(5);
	vt_idx.push_back(6);

	//vt_idx.push_back(6);
	//vt_idx.push_back(5);
	//vt_idx.push_back(7);

	//vt_idx.push_back(6);
	//vt_idx.push_back(7);
	//vt_idx.push_back(8);

	_mesh->m_verts.swap(vt_verts);
	_mesh->m_tris.swap(vt_idx);
}

/*
@brief
@return
*/
void	CManipulator::CreateQuadArrow(mip::MeshTopology* _mesh)
{
	std::vector<mip::VECTOR3> vt_verts;
	std::vector<muint32>	  vt_idx;

	vt_verts.push_back(mip::VECTOR3(-0.5f, 0.f, 0.f) * 0.7f);
	vt_verts.push_back(mip::VECTOR3(0.5f, 0.f, 0.f) * 0.7f);
	vt_verts.push_back(mip::VECTOR3(-0.5f, 1.f, 0.f) * 0.7f);
	vt_verts.push_back(mip::VECTOR3(0.5f, 1.f, 0.f) * 0.7f);

	vt_idx.push_back(1);
	vt_idx.push_back(2);
	vt_idx.push_back(0);

	vt_idx.push_back(2);
	vt_idx.push_back(1);
	vt_idx.push_back(3);

	_mesh->m_verts.swap(vt_verts);
	_mesh->m_tris.swap(vt_idx);
}

/*
@brief
@return
*/
void	CManipulator::CreateTriArrow(mip::MeshTopology* _mesh)
{
	std::vector<mip::VECTOR3> vt_verts;
	std::vector<muint32>	  vt_idx;

	vt_verts.push_back(mip::VECTOR3(1.f, 0.f, 0.f));
	vt_verts.push_back(mip::VECTOR3(0.f, 0.f, 0.f));
	vt_verts.push_back(mip::VECTOR3(0.f, 1.f, 0.f));

	vt_idx.push_back(1);
	vt_idx.push_back(2);
	vt_idx.push_back(0);

	_mesh->m_verts.swap(vt_verts);
	_mesh->m_tris.swap(vt_idx);
}

/*
@brief
@return
*/
void	 CManipulator::CreateCube(mip::MeshTopology* _mesh)
{
	mip::VECTOR3	size(0.5f, 0.5f, 0.5f);

	mip::mesh_control::createCube(size, _mesh->m_verts, _mesh->m_tris, _mesh->m_normals);
}

/*
@brief
@return
*/
void	CManipulator::CreateSphere(mip::MeshTopology* _mesh, mip::VECTOR4 _color)
{
	//mip::VECTOR3 mSize(0.125f, 0.125f, 0.125f);
	mip::VECTOR3 mSize(0.4f, 0.4f, 0.4f);
	mip::mesh_control::createSphere(mSize, 50, 50, _mesh->m_verts, _mesh->m_tris, _mesh->m_normals);
	_mesh->mergingVertex();
	_mesh->updateColor(_color);
	_mesh->setAlpha(255); // 0 ~ 255
	_mesh->enableAlpha(true);
	_mesh->initShader(mip::SHADERTYPE::SHADER_BASIC);

	WIN_MANAGER->buildRenderBufferTopology(_mesh);
}

/*
@brief
@return
*/
void	CManipulator::CreatePlane(
	mip::MeshTopology* _mesh,
	MANIPULATOR_DIR_TYPE	_type,
	float  	_length_line
)
{
	std::vector<mip::VECTOR3> vt_verts;
	std::vector<muint32>	  vt_idx;

	switch (_type)
	{
	case TRANS_XY_AXIS:
	{
		vt_verts.push_back(mip::VECTOR3(-_length_line, -_length_line, 0.f));
		vt_verts.push_back(mip::VECTOR3(_length_line, -_length_line, 0.f));
		vt_verts.push_back(mip::VECTOR3(-_length_line, _length_line, 0.f));
		vt_verts.push_back(mip::VECTOR3(_length_line, _length_line, 0.f));
	}
	break;
	case TRANS_YZ_AXIS:
	{
		vt_verts.push_back(mip::VECTOR3(0.f, -_length_line, -_length_line));
		vt_verts.push_back(mip::VECTOR3(0.f, _length_line, -_length_line));
		vt_verts.push_back(mip::VECTOR3(0.f, -_length_line, _length_line));
		vt_verts.push_back(mip::VECTOR3(0.f, _length_line, _length_line));
	}
	break;
	case TRANS_XZ_AXIS:
	{
		vt_verts.push_back(mip::VECTOR3(-_length_line, 0.f, -_length_line));
		vt_verts.push_back(mip::VECTOR3(_length_line, 0.f, -_length_line));
		vt_verts.push_back(mip::VECTOR3(-_length_line, 0.f, _length_line));
		vt_verts.push_back(mip::VECTOR3(_length_line, 0.f, _length_line));
	}
	break;
	}

	vt_idx.push_back(1);
	vt_idx.push_back(2);
	vt_idx.push_back(0);

	vt_idx.push_back(2);
	vt_idx.push_back(1);
	vt_idx.push_back(3);

	_mesh->m_verts.swap(vt_verts);
	_mesh->m_tris.swap(vt_idx);

	mip::VECTOR4 color(255, 255, 255, 255);
	_mesh->mergingVertex();
	_mesh->updateColor(color);
	_mesh->setAlpha(80); // 0 ~ 255
	_mesh->enableAlpha(true);
	_mesh->initShader(mip::SHADERTYPE::SHADER_BASIC);

	_mesh->updateVertex();
	WIN_MANAGER->buildRenderBufferTopology(_mesh);
}

/*
@brief
@return
*/
void	CManipulator::CreateRotateSnappingPoint(std::vector<mip::VECTOR3>& _vt_points)
{
	_vt_points.clear();

	const double pi2 = M_PI * 2.0;
	int nside = 360;

	float r = 5.f;

	_vt_points.reserve(nside);
	for (int i = 0; i < nside; ++i)
	{
		float x = float(cos(i * pi2 / nside)) * r;
		float y = float(sin(i * pi2 / nside)) * r;

		_vt_points.push_back(mip::VECTOR3(x, y, 0.f));
	}
}

/*
@brief
@return
*/
void	CManipulator::Translate(
	mip::MeshTopology* _mesh,
	mip::VECTOR3& _vec,
	bool  _b_tverts
)
{
	if (_b_tverts)
	{
		int n_verts = _mesh->m_tverts.size();
		for (int i = 0; i < n_verts; ++i)
		{
			if (_mesh->m_tverts[i].isD())
			{
				continue;
			}

			_mesh->m_tverts[i].pos += _vec;

		}
	}
	else
	{
		int n_verts = _mesh->m_verts.size();
		for (int i = 0; i < n_verts; ++i)
		{
			_mesh->m_verts[i] += _vec;
		}
	}
}

/*
@brief
@return
*/
void CManipulator::Translate(std::vector<mip::VECTOR3>& _vt_points, mip::VECTOR3& _vec)
{
	int n_verts = _vt_points.size();
	for (int vi = 0; vi < n_verts; ++vi)
	{
		_vt_points[vi] += _vec;
	}
}

/*
@brief
@return
*/
void CManipulator::UpdateOctreeBB(mip::MeshTopology* _p_mesh, mip::OcNode* _node)
{
	if (!_p_mesh || !_p_mesh->m_treeTris)
	{
		return;
	}

	for (int ii = 0; ii < mip::TREETYPE::OCTREE; ii++)
	{
		_node->box.min += m_Trans;
		_node->box.max += m_Trans;

		mip::OcNode* occ_child = _node->pChildren[ii];

		if (!occ_child)
		{
			UpdateOctreeBB(_p_mesh, occ_child);
			continue;
		}

		occ_child->box.min += m_Trans;
		occ_child->box.max += m_Trans;
	}
}

/*
@brief
@return
*/
void CManipulator::UpdatePivotPoint(mip::QUATERNION& _q, mip::VECTOR3& _trans)
{
	for (int i = 0; i < NUM_AXIS_MANIPULATOR; ++i)
	{
		m_AxisMesh[i]->setRotate(_q);
		m_AxisMesh[i]->setTranslate(_trans);
	}

	//m_InitTransform.setRotate(_q);
	//m_InitTransform.setTranslate(_trans);
}

/*
@brief
@return
*/
void CManipulator::UpdatePivotPoint(mip::MeshTopology* _p_mesh, mip::MATRIX44& _matrix)
{
	if (!m_bUpdatePivot)
	{
		mip::MATRIX44 mesh_mat = _p_mesh->getMatrix();

		//mip::QUATERNION q = mip::QUATERNION::Identity;

		for (int i = 0; i < NUM_AXIS_MANIPULATOR; ++i)
		{
			//m_AxisMesh[i]->setRotate(q);
			m_AxisMesh[i]->setRotate(mesh_mat.getQuaternion());
			m_AxisMesh[i]->setTranslate(_matrix.getOrigin());
		}

		m_Transform.setRotate(mesh_mat.getQuaternion());
		m_Transform.setTranslate(_matrix.getOrigin());

		//m_InitTransform.setRotate(q);
		//m_InitTransform.setRotate(mesh_mat.getQuaternion());
		//m_InitTransform.setTranslate(_matrix.getOrigin());

		m_bUpdatePivot = true;
	}
}

/*
@brief
@return
*/
mip::MATRIX44 CManipulator::RotateFromPoint(
	mip::MeshTopology* _mesh,
	mip::VECTOR3& _vec,
	mip::VECTOR3& _pt,
	bool  _b_tverts
)
{
	auto mat_rot = calcMatrixRotateFromPoint(_vec, _pt);

	auto offset = mat_rot.getOrigin();

	if (_b_tverts)
	{
		mip::AABB	box;
		int 	n_verts = _mesh->m_tverts.size();

		box.min.x = FLT_MAX; box.min.y = FLT_MAX; box.min.z = FLT_MAX;
		box.max.x = FLT_MIN; box.max.y = FLT_MIN; box.max.z = FLT_MIN;

		for (int i = 0; i < n_verts; ++i)
		{
			if (_mesh->m_tverts[i].isD())
			{
				continue;
			}

			auto& pos = _mesh->m_tverts[i].pos;

			pos = mat_rot * pos;
			pos += offset;

			if (pos.x > box.max.x)
				box.max.x = pos.x;
			if (pos.y > box.max.y)
				box.max.y = pos.y;
			if (pos.z > box.max.z)
				box.max.z = pos.z;

			if (pos.x < box.min.x)
				box.min.x = pos.x;
			if (pos.y < box.min.y)
				box.min.y = pos.y;
			if (pos.z < box.min.z)
				box.min.z = pos.z;
		}
	}
	else
	{
		int n_verts = _mesh->m_verts.size();
		for (int i = 0; i < n_verts; ++i)
		{
			_mesh->m_verts[i] = mat_rot * _mesh->m_verts[i];
			_mesh->m_verts[i] += offset;
		}

		int n_normals = _mesh->m_normals.size();
		for (int i = 0; i < n_normals; ++i)
		{
			_mesh->m_normals[i] = mat_rot * _mesh->m_normals[i];
		}
	}

	return mat_rot;
}

/*
@brief
@return
*/
void CManipulator::RotateFromPoint(
	std::vector<mip::VECTOR3>& _vt_points,
	mip::TRANSFORM& _tr
)
{
	auto mat = _tr.getMatrix();
	auto offset = mat.getOrigin();

	int size = _vt_points.size();
	for (int i = 0; i < size; ++i)
	{
		_vt_points[i] = mat * _vt_points[i];
		_vt_points[i] += offset;
	}
}

/*
@brief
@return
*/
void CManipulator::RotateFromPoint(
	std::vector<mip::VECTOR3>& _vt_points,
	mip::VECTOR3& _vec,
	mip::VECTOR3& _pt
)
{
	auto rot_mat = calcMatrixRotateFromPoint(_vec, _pt);
	auto offset = rot_mat.getOrigin();

	int size = _vt_points.size();
	for (int i = 0; i < size; ++i)
	{
		_vt_points[i] = rot_mat * _vt_points[i];
		_vt_points[i] += offset;
	}
}

/*
@brief
@return
*/
void CManipulator::RotateFromPoint(mip::VECTOR3& _vt_point, mip::VECTOR3& _vec, mip::VECTOR3& _pt)
{
	//auto mat_rot = calcMatrixRotateFromPoint(_vec, _pt);

	//auto offset = mat_rot.getOrigin();

	//_vt_point = mat_rot * _vt_point;
	//_vt_point += offset;
}

/*
@brief
@return
*/
mip::MATRIX44 CManipulator::calcMatrixRotateFromPoint(mip::VECTOR3& _vec, mip::VECTOR3& _pt, mip::VECTOR3 _translate)
{
	mip::TRANSFORM transform;
	mip::VECTOR3 euler;

	transform.zero();
	//transform.setRotate(m_Transform.getQuaternion());
	//transform.setTranslate(m_Transform.getMatrix().getOrigin());

	euler.x = float((double)_vec.x / 360. * (2.0 * M_PI));
	euler.y = float((double)_vec.y / 360. * (2.0 * M_PI));
	euler.z = float((double)_vec.z / 360. * (2.0 * M_PI));

	transform.addRotateZ(euler.z);
	transform.addRotateY(euler.y);
	transform.addRotateX(euler.x);

	auto mat_rot = mip::math::MatrixAffineTransformation(1.f, _pt, transform.getQuaternion(), _translate);
	return mat_rot;
}

/*
@brief
@return
*/
void CManipulator::TransformRotate(
	mip::MeshTopology* _mesh,
	mip::VECTOR3& _center,
	mip::SCAMERA* _p_camera,
	mip::MATRIX44* trasnform
)
{
	mip::VECTOR3 v1 = _p_camera->getWorldPoint(0.5f, trasnform);
	mip::VECTOR3 v2 = _p_camera->getPreWorldPoint(0.5f, trasnform);

	mip::VECTOR3 rV = (v1 - v2);
	int cx, cy, px, py;

	_p_camera->getScreenSize(cx, cy);
	_p_camera->getScreenXY(px, py);

	float dy = rV.y / (float)(cy);
	float dx = rV.x / (float)(cx);

	mip::MATRIX44 matInvCamera;
	matInvCamera = trasnform ? (*trasnform) * _p_camera->getView() : _p_camera->getView();
	matInvCamera.inverse();

	mip::QUATERNION	q;
#ifdef USE_RIGHT_HAND
	mip::VECTOR3 Z = -matInvCamera.getScaledZaxis().normalize();
	if (trasnform == nullptr)
	{
		dy = -dy;
	}
#else
	mip::VECTOR3 Z = matInvCamera.getScaledZaxis().normalize();
#endif
	if (px < (cx * 0.9f) && px >(cx * 0.1f))
	{
		mip::VECTOR3 rotV = rV;
		rotV.normalize();

		rotV = (rotV ^ Z).normalize();

		q.setRotationAxis(rotV, rV.length() * 0.05f);

		//tr->addRotate(q);
	}
	else if (px > (cx * 0.9f))
	{
		q.setRotationAxis(Z, dy * 10.0f);
		//tr->addRotate(q);
	}
	else if (px < (cx * 0.1f))
	{
		q.setRotationAxis(Z, -dy * 10.0f);
		//tr->addRotate(q);
	}

	mip::MATRIX44 matRot = mip::math::MatrixAffineTransformation(1.f, _center, q, mip::VECTOR3::Zero);

	mip::MATRIX44 rot;
	mip::MATRIX44 mat_mesh = _mesh->getMatrix();
	mip::math::MatrixMultiply(&rot, &mat_mesh, &matRot);

	//_mesh->addRotate(matRot.getQuaternion());
	//_mesh->setTranslate(rot.getOrigin());

	auto& tverts = _mesh->m_tverts;
	auto offset = rot.getOrigin();
	for (int vi = 0; vi < tverts.size(); ++vi)
	{
		tverts[vi].pos = matRot * tverts[vi].pos;
		//tverts[vi].pos += offset;
	}
}

/*
@brief
@return
*/
void	CManipulator::DrawCircle()
{

}

/*
@brief
@return
*/
void CManipulator::DrawArrow(DataContext* pDataContext, mip::Renderer* _p_renderer)
{
	if (!_p_renderer || !m_p_camera)
	{
		return;
	}

	int start = (int)TRANS_X_AXIS;
	int finish = (int)NONE_AXIS;

	mip::MATRIX44 mat_cam_view = m_p_camera->getView();
	mip::MATRIX44 mat_cam_proj = m_p_camera->getProj();

	float zoomFactor = m_p_camera->getZoom();
	float zoomRatio = zoomFactor / DEFAULT_ZOOM_FACTOR;

	for (int axis = start; axis < finish; ++axis)
	{
		if (!m_bShowFlag[axis])
		{
			continue;
		}

		mip::MeshTopology* mesh = m_AxisMesh[axis];

		mip::VECTOR3 scale = mesh->scale;

		if (mesh->renderBegin(_p_renderer, 0, false, false))
		{
			mesh->setScale(mip::VECTOR3(zoomRatio, zoomRatio, zoomRatio));

			mip::MATRIX44 mat_mesh = mesh->getMatrix();

			mat_mesh *= m_mat_offset;
			mat_mesh *= m_mat_view_world;

			mip::VECTOR4 color = mip::VECTOR4(255.f, 0.f, 0.f, mesh->getAlphaVal());

			switch ((MANIPULATOR_DIR_TYPE)axis)
			{
			case TRANS_Y_AXIS:
			case TRANS_XZ_AXIS:
			case ROTATE_Y_AXIS:
			case SCALE_Y_AXIS:
				color = mip::VECTOR4(0.f, 255.f, 0.f, mesh->getAlphaVal());
				break;
			case TRANS_Z_AXIS:
			case TRANS_XY_AXIS:
			case ROTATE_Z_AXIS:
			case SCALE_Z_AXIS:
				color = mip::VECTOR4(0.f, 0.f, 255.f, mesh->getAlphaVal());
				break;
			case SCALE_ALL_AXIS:
				color = mip::VECTOR4(255.f, 255.f, 255.f, mesh->getAlphaVal());
				break;
			case INVERSE_Z_AXIS:
				color = mip::VECTOR4(147.f, 0.f, 219.f, mesh->getAlphaVal());
				break;
			}

			if (pDataContext != nullptr)
			{
				OmniverseContext* pContext = pDataContext->GetOmniverseContext();
				if (pContext->GetStage()->IsOpen())
				{
					switch (axis)
					{
					case ROTATE_X_AXIS:
					case ROTATE_Y_AXIS:
					case ROTATE_Z_AXIS:
						color = mip::VECTOR4(150.f, 150.f, 150.f, mesh->getAlphaVal());
						break;
					}
				}
			}

			if (m_Axis_Seleted == (MANIPULATOR_DIR_TYPE)axis)
			{
				color.w = 255.f;
			}

			mip::ShaderCommon* pSC = mesh->getShaderCommon();
			mesh->getShaderCommon()->set(mat_mesh, mat_cam_view, mat_cam_proj, color);
			mesh->renderPosition(*mesh->getShaderCommon(), *mesh->getShaderMeshData(), mesh->getShaderType());

			mesh->render(false);
			mesh->renderEnd(_p_renderer);

			mesh->setScale(scale);
		}
	}
}

/*
@brief
@return
*/
void	CManipulator::DrawSphere()
{
	if (m_bFinished || !m_p_camera)
	{
		return;
	}

	mip::MeshTopology* mesh = m_AxisMesh[(int)SCALE_ALL_AXIS];

	mip::MATRIX44 mat_cam_view = m_p_camera->getView();
	mip::MATRIX44 mat_cam_proj = m_p_camera->getProj();

	float zoomFactor = m_p_camera->getZoom();
	float zoomRatio = zoomFactor / DEFAULT_ZOOM_FACTOR;

	if (m_SphereCube)
	{
		if (m_SphereCube->renderBegin(m_pRenderer, 0, false, false))
		{
			mip::VECTOR3 scale = m_SphereCube->scale;

			m_SphereCube->setScale(scale * zoomRatio);

			mip::MATRIX44 sphere_world = m_SphereCube->getMatrix() * m_mat_offset * m_mat_view_world;

			mip::VECTOR4 color = mip::VECTOR4(0.f, 0.f, 0.f, m_SphereCube->getAlphaVal());

			mip::ShaderCommon* pSC = m_SphereCube->getShaderCommon();
			m_SphereCube->getShaderCommon()->set(sphere_world, mat_cam_view, mat_cam_proj, color);
			m_SphereCube->renderPosition(*m_SphereCube->getShaderCommon(), *m_SphereCube->getShaderMeshData(), m_SphereCube->getShaderType());

			m_SphereCube->render(false);
			m_SphereCube->renderEnd(m_pRenderer);

			m_SphereCube->setScale(scale);
		}
	}

	switch (m_Axis_Seleted)
	{
	case TRANS_X_AXIS:
	case TRANS_Y_AXIS:
	case TRANS_Z_AXIS:
	case TRANS_XY_AXIS:
	case TRANS_YZ_AXIS:
	case TRANS_XZ_AXIS:
	{
		if (m_Sphere->renderBegin(m_pRenderer, 0, false, false))
		{
			mip::VECTOR3 scale = m_Sphere->scale;

			m_Sphere->setScale(scale * zoomRatio);

			mip::MATRIX44 sphere_world = m_Sphere->getMatrix() * m_mat_offset * m_mat_view_world;

			mip::VECTOR4 color = mip::VECTOR4(255.f, 0.f, 0.f, m_Sphere->getAlphaVal());

			mip::ShaderCommon* pSC = m_Sphere->getShaderCommon();
			m_Sphere->getShaderCommon()->set(sphere_world, mat_cam_view, mat_cam_proj, color);
			m_Sphere->renderPosition(*m_Sphere->getShaderCommon(), *m_Sphere->getShaderMeshData(), m_Sphere->getShaderType());

			m_Sphere->render(false);
			m_Sphere->renderEnd(m_pRenderer);

			m_Sphere->setScale(scale);
		}
	}
	break;
	}
}

/*
@brief
@return
*/
void CManipulator::DrawPlane()
{
	if ((TRANS_XY_AXIS != m_Axis_Seleted) && (TRANS_XZ_AXIS != m_Axis_Seleted) && (TRANS_YZ_AXIS != m_Axis_Seleted))
	{
		return;
	}

	int idx = (int)m_Axis_Seleted - 3;

	if (m_bFinished || idx > 2)
	{
		return;
	}

	mip::MATRIX44 mat_cam_view = m_p_camera->getView();
	mip::MATRIX44 mat_cam_proj = m_p_camera->getProj();

	mip::MeshTopology* mesh = m_SnappingPlane[idx];

	if (mesh)
	{
		if (mesh->renderBegin(m_pRenderer, 0, false, false))
		{
			mip::MATRIX44 mat_world = mesh->getMatrix() * m_mat_offset * m_mat_view_world;

			mip::VECTOR4 color = mip::VECTOR4(255.f, 255.f, 255.f, mesh->getAlphaVal());

			mip::MATRIX44 matView = m_pModelViewManager->GetCameraPtr()->getView();
			mip::MATRIX44 matProj = m_pModelViewManager->GetCameraPtr()->getProj();

			mip::ShaderCommon* pSC = mesh->getShaderCommon();
			mesh->getShaderCommon()->set(mat_world, mat_cam_view, mat_cam_proj, color);
			mesh->renderPosition(*mesh->getShaderCommon(), *mesh->getShaderMeshData(), mesh->getShaderType());

			mesh->render(false);
			mesh->renderEnd(m_pRenderer);
		}
	}
}

/*
@brief
@return
*/
void	CManipulator::DrawSnapping(QPainter* _p_paint)
{
	DrawSnappingInternal(_p_paint);
}

/*
@brief
@return
*/
void CManipulator::DrawSnappingInternal(QPainter* _p_paint)
{
	//if ( !m_bSnapping || (m_vt_OffsetPoint.size() < 1) || m_bFinished )
	if (!m_bSnapping || m_bFinished)
	{
		return;
	}

	_p_paint->setPen(QPen(Qt::black, 1));

	mip::MATRIX44 mat_cam_view = m_p_camera->getView();
	mip::MATRIX44 mat_cam_proj = m_p_camera->getProj();
	mip::VECTOR2 sz_screen = m_p_camera->getScreenSize();

	switch (m_Axis_Seleted)
	{
	case TRANS_X_AXIS:
	case TRANS_Y_AXIS:
	case TRANS_Z_AXIS:
	{
		auto mesh = m_AxisMesh[(int)m_Axis_Seleted];

		if (mesh)
		{
			mip::MATRIX44	mat_mesh = mesh->getMatrix() * m_mat_offset * m_mat_view_world;

			int  n_pt = m_vt_OffsetPoint.size();

			mip::VECTOR3	pt2d, pt[2];

			QPoint 	v1, v2;

			QPolygon polygon;
			QPolygon polygon_cols;

			auto dlg = static_cast<CManpulateDlg*>(m_pMeshDlgManager->getMeshDialog(MESH_DIALOG_MANIPULATE));

			double interval = INTERVAL_TRANSLATE;

			if (dlg)
			{
				interval = dlg->getInterval();
			}

			float 	offset = interval;
			float 	offset2 = interval * 2.f;

			for (int idx = 0; idx < n_pt; ++idx)
			{
				pt2d = mip::geom::WorldToScreen(m_vt_OffsetPoint[idx], sz_screen.x, sz_screen.y, mat_cam_view, mat_cam_proj, &mat_mesh);

				v1.setX(pt2d.x);
				v1.setY(pt2d.y);

				polygon.push_back(v1);

				pt[0] = m_vt_OffsetPoint[idx];
				pt[1] = m_vt_OffsetPoint[idx];

				switch (m_Axis_Seleted)
				{
				case TRANS_X_AXIS:
				{
					if (idx % 5 == 0)
					{
						pt[0].y -= offset2;
						pt[1].y += offset2;
					}
					else
					{
						pt[0].y -= offset;
						pt[1].y += offset;
					}
				}
				break;
				case TRANS_Y_AXIS:
				{
					if (idx % 5 == 0)
					{
						pt[0].x -= offset2;
						pt[1].x += offset2;
					}
					else
					{
						pt[0].x -= offset;
						pt[1].x += offset;
					}
				}
				break;
				case TRANS_Z_AXIS:
				{
					if (idx % 5 == 0)
					{
						pt[0].x -= offset2;
						pt[1].x += offset2;
					}
					else
					{
						pt[0].x -= offset;
						pt[1].x += offset;
					}
				}
				break;
				}

				for (int i = 0; i < 2; ++i)
				{
					pt2d = mip::geom::WorldToScreen(pt[i], sz_screen.x, sz_screen.y, mat_cam_view, mat_cam_proj, &mat_mesh);

					v1.setX(pt2d.x);
					v1.setY(pt2d.y);
					polygon_cols.push_back(v1);
				}
			}

			for (int i = 0; i < polygon.size() - 1; ++i)
			{
				_p_paint->drawLine(polygon.at(i), polygon.at(i + 1));
			}

			_p_paint->drawLine(polygon.at(polygon.size() - 1), polygon.at(0));

			for (int i = 0; i < polygon_cols.size() - 1; i += 2)
			{
				_p_paint->drawLine(polygon_cols.at(i), polygon_cols.at(i + 1));
			}
		}
	}
	break;
	case TRANS_XY_AXIS:
	case TRANS_YZ_AXIS:
	case TRANS_XZ_AXIS:
	{
		//auto mesh = m_AxisMesh[(int)m_Axis_Seleted];
		auto mesh = m_SnappingPlane[int(m_Axis_Seleted) - 3];

		if (mesh)
		{
			mip::MATRIX44 mat_mesh = mesh->getMatrix() * m_mat_offset * m_mat_view_world;

			int n_pt = m_vt_OffsetPoint.size();

			mip::VECTOR3	pt2d;

			QPoint 	v1;

			QPolygon polygon;

			for (int idx = 0; idx < n_pt; ++idx)
			{
				pt2d = mip::geom::WorldToScreen(m_vt_OffsetPoint[idx], sz_screen.x, sz_screen.y, mat_cam_view, mat_cam_proj, &mat_mesh);

				v1.setX(pt2d.x);
				v1.setY(pt2d.y);

				polygon.push_back(v1);
			}

			// row line
			int remain = NUMBER_OF_ROWS - 1;
			for (int i = 0; i < polygon.size() - 1; ++i)
			{
				if ((i > 0) && (i % NUMBER_OF_ROWS == remain))
				{
					continue;
				}

				_p_paint->drawLine(polygon.at(i), polygon.at(i + 1));
			}

			// col line
			int cols = n_pt / NUMBER_OF_ROWS;
			for (int i = 0; i < NUMBER_OF_ROWS; ++i)
			{
				for (int j = 1; j < cols; ++j)
				{
					_p_paint->drawLine(polygon.at(i), polygon.at(i + NUMBER_OF_ROWS * j));
				}

			}
		}
	}
	break;
	case ROTATE_X_AXIS:
	case ROTATE_Y_AXIS:
	case ROTATE_Z_AXIS:
	{
		auto mesh = m_AxisMesh[(int)SCALE_ALL_AXIS];

		if (mesh)
		{
			float zoomFactor = m_p_camera->getZoom();
			float zoomRatio = zoomFactor / DEFAULT_ZOOM_FACTOR;

			mip::VECTOR3 scale = mesh->scale;

			mesh->setScale(scale * zoomRatio);

			mip::MATRIX44 mat_mesh = mesh->getMatrix() * m_mat_offset * m_mat_view_world;

			mip::AABB bb = mesh->m_boundingBox;
			mip::VECTOR3 bb_center = bb.getCenter();

			mip::VECTOR3 bb_center_2d;

			bb_center_2d = mip::geom::WorldToScreen(bb_center, sz_screen.x, sz_screen.y, mat_cam_view, mat_cam_proj, &mat_mesh);

			QPoint center((int)bb_center_2d.x, (int)bb_center_2d.y);
			_p_paint->drawLine(center, m_CurrentMousePt);

			if (m_vt_RotateSnappingPoint.size() > 0)
			{
				QPolygon polygon;
				QPoint 	v1;

				for (int i = 0; i < m_vt_RotateSnappingPoint.size(); ++i)
				{
					mip::VECTOR3 pt2d = mip::geom::WorldToScreen(m_vt_RotateSnappingPoint[i], sz_screen.x, sz_screen.y, mat_cam_view, mat_cam_proj, &mat_mesh);

					v1.setX(pt2d.x);
					v1.setY(pt2d.y);

					polygon.push_back(v1);
				}

				int n_loop = polygon.size() - 1;

				for (int i = 0; i < n_loop; ++i)
				{
					_p_paint->drawLine(polygon.at(i), polygon.at(i + 1));
				}

				_p_paint->drawLine(polygon.at(n_loop), polygon.at(0));
			}

			mesh->setScale(scale);
		}
	}
	break;
	}
}

/*
@brief
@return
*/
void CManipulator::MoveCoordinate(mip::VECTOR3& _trans)
{
	for (int i = 0; i < NUM_AXIS_MANIPULATOR; ++i)
	{
		m_AxisMesh[i]->addTranslate(_trans);

		m_PrevAxisTransform[i].setRotate(m_AxisMesh[i]->rotation);
		m_PrevAxisTransform[i].setTranslate(m_AxisMesh[i]->translation - m_TransformTemp.translation);
	}

	m_SphereCube->addTranslate(_trans);
}

/*
@brief
@return
*/
void CManipulator::RotateCoordinate(
	DataContext* pDataContext,
	mip::MeshTopology* _p_mesh,
	mip::VECTOR3& _vec_angle,
	mip::VECTOR3* _center
)
{
	if (!pDataContext)
	{
		return;
	}

	mip::MATRIX44 rot_mat;

	if (!_center)
	{
		auto axis_mesh = m_AxisMesh[(int)SCALE_ALL_AXIS];
		auto bb_center = axis_mesh->getMatrix() * axis_mesh->m_boundingBox.getCenter();

		rot_mat = calcMatrixRotateFromPoint(_vec_angle, bb_center);

		for (int i = 0; i < NUM_AXIS_MANIPULATOR; ++i)
		{
			m_AxisMesh[i]->addRotate(rot_mat.getQuaternion());
			m_AxisMesh[i]->addTranslate(rot_mat.getOrigin());
		}
	}
	else
	{
		rot_mat = calcMatrixRotateFromPoint(_vec_angle, *_center);

		mip::MATRIX44 rot;
		auto pick_mesh = pDataContext->m_MeshData.GetCurrentMesh();
		auto pick_mat = pick_mesh->getMatrix();
		mip::math::MatrixMultiply(&rot, &pick_mat, &rot_mat);

		for (int i = 0; i < NUM_AXIS_MANIPULATOR; ++i)
		{
			m_AxisMesh[i]->addRotate(rot_mat.getQuaternion());
			//m_AxisMesh[i]->setTranslate(rot.getOrigin());
		}
	}
}

/*
@brief
@return
*/
void CManipulator::RotateCoordinateAxis(
	mip::MATRIX44& _mat
)
{
	for (int i = 0; i < NUM_AXIS_MANIPULATOR; ++i)
	{
		m_AxisMesh[i]->addRotate(_mat.getQuaternion());
		m_AxisMesh[i]->addTranslate(_mat.getOrigin());
	}
}

/*
@brief
@return
*/
void	CManipulator::RotateMesh(
	mip::MeshTopology* _p_mesh,
	mip::VECTOR3& _vec_angle,
	mip::VECTOR3& _center
)
{
	RotateFromPoint(_p_mesh, _vec_angle, _center, true);

	_p_mesh->buildTree();
	_p_mesh->updateVertex();
	WIN_MANAGER->buildRenderBufferTopology(_p_mesh);
}

/*
@brief
@return
*/
void	CManipulator::MovePlane(mip::VECTOR3& _trans)
{
	int idx = (int)m_Axis_Seleted - 3;

	if (idx > 2)
	{
		return;
	}

	m_SnappingPlane[idx]->addTranslate(_trans);
}

/*
@brief
@return
*/
void	CManipulator::processTranslate(mip::MeshTopology* _p_mesh)
{
	mip::MATRIX44 mesh_mat = _p_mesh->getMatrix();

	mesh_mat *= m_mat_offset;
	mesh_mat *= m_mat_view_world;

	//auto	q = _p_mesh->getQuaternion();
	auto	q = mesh_mat.getQuaternion();

	if (m_Axis_Seleted == TRANS_YZ_AXIS)
	{
		mip::TRANSFORM tr(q);

		tr.addRotateY(90.f);

		q = tr.getQuaternion();
	}

	mip::VECTOR3 cur_mouse_2d, prev_mouse_2d, dir;
	convertNewMousePt(m_PrevMousePt, m_CurrentMousePt, prev_mouse_2d, cur_mouse_2d, dir);

	mip::VECTOR3 init_mouse_2d, dir_init;
	convertNewMousePt(m_InitMousePt, m_CurrentMousePt, prev_mouse_2d, cur_mouse_2d, dir_init);

	if (dir == dir.Zero)
	{
		return;
	}

	//int dx = int(cur_mouse_2d.x - prev_mouse_2d.x);
	//int dy = int(cur_mouse_2d.y - prev_mouse_2d.y);

	//if (dx == 0 && dy == 0)
	//{
	//	return;
	//}

	//printf_s("dir[%f %f %f] ", dir.x, dir.y, dir.z);
	//printf_s("dir_init[%f %f %f] ", dir_init.x, dir_init.y, dir_init.z);
	//printf_s("m_TransAccumulate[%f %f %f]\n", m_TransAccumulate.x, m_TransAccumulate.y, m_TransAccumulate.z);

	auto trans_inv_mat = m_Transform.getMatrix().inverse();
	auto inv_q = trans_inv_mat.getQuaternion();

	auto rot_trans = inv_q.rotateVector(m_TransAccumulate);

	m_Trans = mip::VECTOR3(0.f, 0.f, 0.f);

	auto dlg = static_cast<CManpulateDlg*>(m_pMeshDlgManager->getMeshDialog(MESH_DIALOG_MANIPULATE));

	double interval = INTERVAL_TRANSLATE;

	if (dlg)
	{
		interval = dlg->getInterval();
	}

	switch (m_Axis_Seleted)
	{
	case TRANS_X_AXIS:
	{
		//if (dx != 0)
		//{
		//	m_Trans.x = INTERVAL_TRANSLATE * (float)dx;
		//}

		if (dir.x != 0)
		{
			auto diff = dir_init.x - rot_trans.x;

			if (std::abs(diff) >= interval)
			{
				int ratio = int(diff / interval);

				m_Trans.x = interval * (float)ratio;
			}
		}
	}
	break;
	case TRANS_Y_AXIS:
	{
		//if (dy != 0)
		//{
		//	m_Trans.y = INTERVAL_TRANSLATE * (float)dy * -1.f;
		//}

		if (dir.y != 0)
		{
			auto diff = (dir_init.y + dir_init.z) - rot_trans.y;

			if (std::abs(diff) >= interval)
			{
				int ratio = int(diff / interval);

				m_Trans.y = interval * (float)ratio;
			}
		}
	}
	break;
	case TRANS_Z_AXIS:
	{
		//if (dy != 0)
		//{
		//	m_Trans.z = INTERVAL_TRANSLATE * (float)dy * -1.f;
		//}

		if (dir.z != 0)
		{
			auto diff = (dir_init.y + dir_init.z) - rot_trans.z;

			if (std::abs(diff) >= interval)
			{
				int ratio = int(diff / interval);

				m_Trans.z = interval * (float)ratio;
			}
		}
	}
	break;
	case TRANS_XY_AXIS:
	case TRANS_YZ_AXIS:
	case TRANS_XZ_AXIS:
	{
		//checkCloseOffset(dx, dy, q);
	}
	break;
	}

	if (m_Trans != m_Trans.Zero)
	{
		auto tr_q = m_Transform.getQuaternion();

		m_Trans = tr_q.rotateVector(m_Trans);

		_p_mesh->addTranslate(m_Trans);

		//_p_mesh->m_boundingBox.min += m_Trans;
		//_p_mesh->m_boundingBox.max += m_Trans;

		// update
		MoveCoordinate(m_Trans);

		updateTranslateInfo(m_Trans);

		updateCtrl();

		auto dlg = static_cast<CManpulateDlg*>(m_pMeshDlgManager->getMeshDialog(MESH_WORK_MANIFULATE));
		if (dlg)
		{
			dlg->UpdateMeshInfo(_p_mesh, m_Axis_Seleted);
		}
	}

	//qInfo() << "processTranslate dir init : " << QString("%1,%2,%3").arg(dir_init.x).arg(dir_init.y).arg(dir_init.z)
	//	<< ",rotate : " << QString("%1,%2,%3").arg(rot_trans.x).arg(rot_trans.y).arg(rot_trans.z)
	//	<< ",trans : " << QString("%1,%2,%3").arg(m_Trans.x).arg(m_Trans.y).arg(m_Trans.z)
	//	<< ",mesh trans : " << QString("%1,%2,%3").arg(_p_mesh->translation.x).arg(_p_mesh->translation.y).arg(_p_mesh->translation.z)
	//	<< ",mesh vert : " << QString("%1,%2,%3").arg(_p_mesh->m_verts[0].x).arg(_p_mesh->m_verts[0].y).arg(_p_mesh->m_verts[0].z);
}

/*
@brief
@return
*/
void CManipulator::processTranslate(
	mip::MeshTopology* _p_mesh,
	mip::VECTOR3& _trans
)
{
	qInfo() << "translate value = " << QString("%1, %2, %3").arg(_trans[0], _trans[1], _trans[2]);

	m_Trans = _trans;

	_p_mesh->addTranslate(_trans);

	//_p_mesh->m_boundingBox.min += _trans;
	//_p_mesh->m_boundingBox.max += _trans;


	// update
	MoveCoordinate(_trans);

	updateTranslateInfo(_trans);
}

/*
@brief
@return
*/
void CManipulator::processRotate(
	mip::MeshTopology* _p_mesh,
	mip::MATRIX44& _rot,
	mip::VECTOR3& _angle,
	mip::VECTOR3& _center,
	mip::RAY& _dir_vec,
	mip::RAY& _cross_dir_vec,
	mip::MeshTopology* _p_pck_mesh
)
{
	mip::AABB 	bbox = _p_mesh->m_boundingBox;
	mip::MATRIX44	mat_mesh = _p_mesh->getMatrix();

	mat_mesh *= m_mat_offset;

	auto 	center = mat_mesh * bbox.getCenter();

	//auto 	q_mesh = mat_mesh.getQuaternion();

	//auto mat_sel = m_AxisMesh[(int)m_Axis_Seleted]->getMatrix();
	//auto mat_sel_inv = mat_sel.inverse();
	//auto q = mat_sel.getQuaternion();

	//auto rot_vec = q.rotateVector(_angle);

	//auto rot_mat = calcMatrixRotateFromPoint(_angle, _center);

	//_p_mesh->addRotate(_rot.getQuaternion());
	//_p_mesh->addTranslate(_rot.getOrigin());

	//auto intersect_pt = calcIntersectPoint(ray, _cross_dir_vec);
	//auto rot_mat = calcMatrixRotateFromPoint(_angle, _center);

	//mip::MATRIX44 rot;
	//mip::math::MatrixMultiply(&rot, &mat_mesh, &rot_mat);

	auto org_t = _p_mesh->translation;

	mip::QUATERNION q;
	_p_mesh->setTranslate(-_center);
	_p_mesh->addRotate(_rot.getQuaternion());
	//_p_mesh->addTranslate(-org_t);

	//_p_mesh->setTranslate(_center);
	//_p_mesh->addRotate(_rot.getQuaternion());
	//_p_mesh->addTranslate(_rot.getOrigin());
	//_p_mesh->addTranslate(_rot.getOrigin());

	//RotateCoordinate(_p_mesh, _angle, &_center);
	RotateCoordinateAxis(_rot);

	updateRotateInfo(_angle, _rot);
}

/*
@brief
@return
*/
void CManipulator::processScale(
	mip::MeshTopology* _p_mesh,
	mip::VECTOR3& _scale_vec,
	mip::VECTOR3& _t
)
{
	if (_scale_vec == _scale_vec.Zero)
	{
		return;
	}

	auto bbox = _p_mesh->m_boundingBox;

	auto prev_center = bbox.getCenter() * _p_mesh->scale;

	//_p_mesh->setScale(_scale_vec);
	//m_Transform.setScale(_scale_vec);
	//m_TransformTemp.setScale(_scale_vec);

	_p_mesh->addScale(_scale_vec);
	m_Transform.addScale(_scale_vec);
	m_TransformTemp.addScale(_scale_vec);

	m_ScaleVec = _scale_vec;
	m_ScaleTranslate = _t;

	auto next_center = bbox.getCenter() * _p_mesh->scale;
	auto diff = prev_center - next_center;

	if (diff != diff.Zero)
	{
		//auto q = _p_mesh->getQuaternion();
		//diff = q.rotateVector(diff);

		_p_mesh->addTranslate(_t);
		m_Transform.addTranslate(_t);
		m_TransformTemp.addTranslate(_t);

		_p_mesh->m_boundingBox.min += _t;
		_p_mesh->m_boundingBox.max += _t;
	}
}

/*
@brief
@return
*/
void CManipulator::UpdateTransformAxis(
	int  	_idx_axis,
	mip::MATRIX44& _mat
)
{
	if (0 <= _idx_axis && _idx_axis < NUM_AXIS_MANIPULATOR)
	{
		if (m_AxisMesh[_idx_axis])
		{
			m_AxisMesh[_idx_axis]->setTranslate(_mat.getOrigin());
			m_AxisMesh[_idx_axis]->setRotate(_mat.getQuaternion());
		}
	}
}

void CManipulator::AddTransformAxis(mip::MATRIX44& _mat)
{
	for (int idx_axis = 0; idx_axis < NUM_AXIS_MANIPULATOR; ++idx_axis)
	{
		if (m_AxisMesh[idx_axis])
		{
			m_AxisMesh[idx_axis]->addRotate(_mat.getQuaternion());
			m_AxisMesh[idx_axis]->addTranslate(_mat.getOrigin());
		}
	}
}

/*
@brief
@return
*/
void CManipulator::UpdatePosition(
	mip::MeshTopology* _p_mesh,
	bool  _b_init
)
{
	if (_p_mesh)
	{
		mip::VECTOR3 min, max;

		min = mip::VECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);
		max = mip::VECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

		if (_p_mesh->m_verts.size() > 0)
		{
			int count = 0;
			for (int i = 0; i < _p_mesh->m_verts.size(); ++i)
			{
				if (_p_mesh->m_verts[i] == mip::VECTOR3::Zero)
				{
					continue;
				}

				for (int j = 0; j < 3; j++)
				{
					if (_p_mesh->m_verts[i][j] < min[j])
					{
						min[j] = _p_mesh->m_verts[i][j];
					}

					if (_p_mesh->m_verts[i][j] > max[j])
					{
						max[j] = _p_mesh->m_verts[i][j];
					}
				}
			}
		}

		if (min == mip::VECTOR3(FLT_MAX, FLT_MAX, FLT_MAX))
		{
			min = mip::VECTOR3::Zero;
		}

		if (max == mip::VECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX))
		{
			max = mip::VECTOR3::Zero;
		}

		auto mat_mesh = _p_mesh->getMatrix();
		auto center_mesh = (max + min) * 0.5f;

		center_mesh = mip::math::VectorTransform(center_mesh, mat_mesh);

		auto axis_mesh = m_AxisMesh[(int)SCALE_ALL_AXIS];
		auto bb_axis_mesh = axis_mesh->m_boundingBox;
		auto center_axis_mesh = bb_axis_mesh.getCenter();

		auto diff = center_mesh - center_axis_mesh;

		//diff += pck_mesh->translation;

		for (int i = 0; i < NUM_AXIS_MANIPULATOR; ++i)
		{
			if (m_AxisMesh[i])
			{
				if (_b_init)
				{
					m_AxisMesh[i]->zero();
				}
				else
				{
					m_AxisMesh[i]->setTranslate(mip::VECTOR3());
				}

				m_AxisMesh[i]->addTranslate(diff);
			}

			//m_AxisMesh[i]->addRotate(mat_mesh.getQuaternion());
			//m_AxisMesh[i]->addTranslate(mat_mesh.getOrigin());
		}
	}

	if (_b_init)
	{
		m_Transform.zero();
	}
}

/*
@brief
@return
*/
void CManipulator::processRotate(DataContext* pDataContext, mip::MeshTopology* _p_mesh)
{
	auto 	angle = calcAngleFromMousePt();
	auto 	bbox = _p_mesh->m_boundingBox;
	auto 	mat_mesh = _p_mesh->getMatrix();

	mat_mesh *= m_mat_offset;

	auto 	q_mesh = mat_mesh.getQuaternion();
	auto 	center = mat_mesh * bbox.getCenter();
	auto 	_center = bbox.getCenter() + m_mat_offset.getOrigin();
	//auto 	center  = q_mesh.rotateVector(bbox.getCenter()) * _p_mesh->scale;

	if (m_bBack_Selected)
	{
		angle *= -1.0;
	}

	float radian = float((double)angle / 360. * (2.0 * M_PI));

	mip::VECTOR3 angle_vec;
	switch (m_Axis_Seleted)
	{
	case ROTATE_X_AXIS:
	{
		angle_vec = mip::VECTOR3(angle, 0.f, 0.f);
	}
	break;
	case ROTATE_Y_AXIS:
	{
		angle_vec = mip::VECTOR3(0.f, angle, 0.f);
	}
	break;
	case ROTATE_Z_AXIS:
	{
		angle_vec = mip::VECTOR3(0.f, 0.f, -angle);
	}
	break;
	}

	auto mat_sel = m_AxisMesh[(int)m_Axis_Seleted]->getMatrix();
	auto q = mat_sel.getQuaternion();

	auto rot_vec = q.rotateVector(angle_vec);

	m_Angle = rot_vec;

	m_Center = _center;

	auto rot_mat = calcMatrixRotateFromPoint(rot_vec, center);

	_p_mesh->addRotate(rot_mat.getQuaternion());
	_p_mesh->addTranslate(rot_mat.getOrigin());

	RotateCoordinate(pDataContext, _p_mesh, rot_vec);

	updateRotateInfo(rot_vec, rot_mat);

	updateCtrl();

	calcAxisDir(m_axis_dir, m_axis_cross_dir);

	auto dlg = static_cast<CManpulateDlg*>(m_pMeshDlgManager->getMeshDialog(MESH_WORK_MANIFULATE));
	if (dlg)
	{
		dlg->UpdateMeshInfo(_p_mesh, m_Axis_Seleted);
	}
}

/*
@brief
@return
*/
void	CManipulator::processScale(mip::MeshTopology* _p_mesh)
{
	bool	b_update = false;

	float	roll, pitch, yaw;

	float	interval = 0.f;

	auto	vec = m_CurrentMousePt - m_PrevMousePt;

	auto	q = _p_mesh->getQuaternion();

	q.getYawPitchRoll(yaw, pitch, roll);

	mip::VECTOR3 cur_mouse_2d, prev_mouse_2d;
	convertNewMousePt(m_PrevMousePt, m_CurrentMousePt, prev_mouse_2d, cur_mouse_2d);

	//int dx = int(cur_mouse_2d.x - prev_mouse_2d.x) / 2;
	//int dy = int(cur_mouse_2d.y - prev_mouse_2d.y) / 2;
	int dx = int(cur_mouse_2d.x - prev_mouse_2d.x);
	int dy = int(cur_mouse_2d.y - prev_mouse_2d.y);

	if (dx == 0 && dy == 0)
	{
		m_ScaleVec = mip::VECTOR3();
		m_ScaleTranslate = mip::VECTOR3();

		return;
	}

	switch (m_Axis_Seleted)
	{
	case SCALE_X_AXIS:
	{
		if (dx != 0)
		{
			if (dx > 0)
			{
				interval += INTERVAL_SCALE;
			}
			else
			{
				interval -= INTERVAL_SCALE;
			}

			b_update = true;
		}
	}
	break;
	case SCALE_Y_AXIS:
	{
		if (dy != 0)
		{
			if (dy < 0)
			{
				interval -= INTERVAL_SCALE;
			}
			else
			{
				interval += INTERVAL_SCALE;
			}

			b_update = true;
		}
	}
	break;
	case SCALE_Z_AXIS:
	{
		if (dy != 0)
		{
			if (dy < 0)
			{
				interval += INTERVAL_SCALE;
			}
			else
			{
				interval -= INTERVAL_SCALE;
			}

			b_update = true;
		}
	}
	break;
	case SCALE_ALL_AXIS:
	{
		if (vec.x() != 0 && vec.y() != 0)
		{
			b_update = false;
		}
		else if (vec.x() != 0)
		{
			if (vec.x() > 0)
			{
				interval += INTERVAL_SCALE;
			}
			else
			{
				interval -= INTERVAL_SCALE;
			}

			b_update = true;
		}
		else if (vec.y() != 0)
		{
			if (vec.y() < 0)
			{
				interval += INTERVAL_SCALE;
			}
			else
			{
				interval -= INTERVAL_SCALE;
			}

			b_update = true;
		}
	}
	break;
	}

	if (b_update)
	{
		float 	ratio = 1.f;
		mip::VECTOR3	vec_ratio = _p_mesh->scale;

		mip::AABB bb = _p_mesh->m_boundingBox;
		//mip::mesh_control::getMinMax(_p_mesh->m_verts, bb.min, bb.max);

		bb.min.x = std::abs(bb.min.x);
		bb.min.y = std::abs(bb.min.y);
		bb.min.z = std::abs(bb.min.z);

		bb.max.x = std::abs(bb.max.x);
		bb.max.y = std::abs(bb.max.y);
		bb.max.z = std::abs(bb.max.z);

		switch (m_Axis_Seleted)
		{
		case SCALE_X_AXIS:
		{
			ratio = bb.max.x + interval;
			ratio /= bb.max.x;

			vec_ratio.x += ratio - 1.f;

			if (vec_ratio.x < MIN_RATIO_SCALE)
			{
				vec_ratio.x = MIN_RATIO_SCALE;
			}
		}
		break;
		case SCALE_Y_AXIS:
		{
			ratio = bb.max.y + interval;
			ratio /= bb.max.y;

			vec_ratio.y += ratio - 1.f;

			if (vec_ratio.y < MIN_RATIO_SCALE)
			{
				vec_ratio.y = MIN_RATIO_SCALE;
			}
		}
		break;
		case SCALE_Z_AXIS:
		{
			ratio = bb.max.z + interval;
			ratio /= bb.max.z;

			vec_ratio.z += ratio - 1.f;

			if (vec_ratio.z < MIN_RATIO_SCALE)
			{
				vec_ratio.z = MIN_RATIO_SCALE;
			}
		}
		break;
		case SCALE_ALL_AXIS:
		{
			ratio = bb.max.x + interval;
			ratio /= bb.max.x;
			vec_ratio.x += ratio - 1.f;

			ratio = bb.max.y + interval;
			ratio /= bb.max.y;
			vec_ratio.y += ratio - 1.f;

			ratio = bb.max.z + interval;
			ratio /= bb.max.z;
			vec_ratio.z += ratio - 1.f;

			if (vec_ratio.x < MIN_RATIO_SCALE)
			{
				vec_ratio.x = MIN_RATIO_SCALE;
			}

			if (vec_ratio.y < MIN_RATIO_SCALE)
			{
				vec_ratio.y = MIN_RATIO_SCALE;
			}

			if (vec_ratio.z < MIN_RATIO_SCALE)
			{
				vec_ratio.z = MIN_RATIO_SCALE;
			}
		}
		break;
		}

		mip::AABB bbox = _p_mesh->m_boundingBox;
		//mip::mesh_control::getMinMax(_p_mesh->m_verts, bbox.min, bbox.max);

		auto prev_center = bbox.getCenter() * _p_mesh->scale;
		auto prev_scale = _p_mesh->scale;

		m_ScaleVec = vec_ratio - prev_scale;

		//_p_mesh->setScale(vec_ratio);
		//m_Transform.setScale(vec_ratio);
		//m_TransformTemp.setScale(vec_ratio);

		_p_mesh->addScale(m_ScaleVec);
		m_Transform.addScale(m_ScaleVec);
		m_TransformTemp.addScale(m_ScaleVec);

		auto next_center = bbox.getCenter() * _p_mesh->scale;
		auto diff = prev_center - next_center;

		m_ScaleTranslate = mip::VECTOR3();

		if (diff != diff.Zero)
		{
			auto q = _p_mesh->getQuaternion();

			diff = q.rotateVector(diff);

			_p_mesh->addTranslate(diff);
			m_Transform.addTranslate(diff);
			m_TransformTemp.addTranslate(diff);

			m_ScaleTranslate = diff;
		}

		updateCtrl();

		auto dlg = static_cast<CManpulateDlg*>(m_pMeshDlgManager->getMeshDialog(MESH_WORK_MANIFULATE));
		if (dlg)
		{
			dlg->UpdateMeshInfo(_p_mesh, m_Axis_Seleted);
		}
	}
}

/*
@brief
@return
*/
void CManipulator::updateTranslateInfo(mip::VECTOR3& _trans)
{
	m_MoveSnapInterval += _trans;
	m_TransAccumulate += _trans;

	m_Transform.addTranslate(_trans);
	m_TransformTemp.addTranslate(_trans);

	m_centerSnapping += _trans;
}

/*
@brief
@return
*/
void CManipulator::updateRotateInfo(
	mip::VECTOR3& _angle,
	mip::MATRIX44& _mat
)
{
	m_RotAngleAccumulateTemp += _angle;

	m_RotAngleAccumulate += _angle;

	checkAngleLimit(m_RotAngleAccumulate);
	checkAngleLimit(m_RotAngleAccumulateTemp);

	m_Transform.addRotate(_mat.getQuaternion());
	m_Transform.addTranslate(_mat.getOrigin());

	m_TransformTemp.addRotate(_mat.getQuaternion());
	m_TransformTemp.addTranslate(_mat.getOrigin());

	m_RotMatTemp = _mat;

	//m_Transform.setRotate(_mat.getQuaternion());
	//m_Transform.setTranslate(_mat.getOrigin());

	//m_TransformTemp.setRotate(_mat.getQuaternion());
	//m_TransformTemp.setTranslate(_mat.getOrigin());
}

/*
@brief
@return
*/
void CManipulator::calcOffsetPoint(
	std::vector<mip::VECTOR3>& _vt_pt,
	mip::VECTOR3& _center,
	int   	_n_row,
	int   	_n_cols,
	float   _offset
)
{
	switch (m_Axis_Seleted)
	{
	case TRANS_X_AXIS:
	case TRANS_Y_AXIS:
	case TRANS_Z_AXIS:
	{
		int n_row = (_n_row - 1) * 3 + 1;

		int start = ((n_row - 1) / 2) * -1;
		int finish = ((n_row - 1) / 2) + 1;

		_vt_pt.reserve(n_row);

		mip::VECTOR3 pos;

		auto arrow_mesh = m_AxisMesh[(int)m_Axis_Seleted];
		auto center_arrow = arrow_mesh->m_boundingBox.getCenter();
		auto dir = _center - center_arrow;

		for (int rows = start; rows < finish; rows++)
		{
			float row_offset = _offset * (float)rows;

			switch (m_Axis_Seleted)
			{
			case TRANS_X_AXIS:
				pos = _center + mip::VECTOR3(row_offset, 0.f, 0.f);
				break;
			case TRANS_Y_AXIS:
				pos = _center + mip::VECTOR3(0.f, row_offset, 0.f);
				break;
			case TRANS_Z_AXIS:
				pos = _center + mip::VECTOR3(0.f, 0.f, row_offset);
				break;
			}

			_vt_pt.push_back(pos);
		}
	}
	break;
	case TRANS_XY_AXIS:
	case TRANS_YZ_AXIS:
	case TRANS_XZ_AXIS:
	{
		int start = ((_n_row - 1) / 2) * -1;
		int finish = ((_n_row - 1) / 2) + 1;

		_vt_pt.reserve(_n_row * _n_cols);

		mip::VECTOR3 pos;
		for (int cols = start; cols < finish; cols++)
		{
			float col_offset = _offset * (float)cols;

			for (int rows = start; rows < finish; rows++)
			{
				float row_offset = _offset * (float)rows;

				switch (m_Axis_Seleted)
				{
				case TRANS_XY_AXIS:
					pos = _center + mip::VECTOR3(row_offset, col_offset, 0.f);
					break;
				case TRANS_YZ_AXIS:
					pos = _center + mip::VECTOR3(0.f, row_offset, col_offset);
					break;
				case TRANS_XZ_AXIS:
					pos = _center + mip::VECTOR3(row_offset, 0.f, col_offset);
					break;
				}

				_vt_pt.push_back(pos);
			}
		}
	}
	break;
	}
}

/*
@brief
@return
*/
void	CManipulator::convertNewMousePt(
	QPoint& _prev_mouse_pt,
	QPoint& _cur_mouse_pt,
	mip::VECTOR3& _new_prev_mouse_pt,
	mip::VECTOR3& _new_cur_mouse_pt,
	mip::VECTOR3& _direction
)
{
	mip::VECTOR3 cur_mouse_3d, prev_mouse_3d;

	auto mesh = m_AxisMesh[(int)m_Axis_Seleted];

	mip::MATRIX44 view_mat = m_p_camera->getView();
	mip::MATRIX44 proj_mat = m_p_camera->getProj();
	mip::VECTOR2 sz_screen = m_p_camera->getScreenSize();

	mip::MATRIX44 trans_mat = m_Transform.getMatrix();

	trans_mat *= m_mat_offset;
	trans_mat *= m_mat_view_world;

	cur_mouse_3d = mip::geom::Screen2World(
		_cur_mouse_pt.x(),
		_cur_mouse_pt.y(),
		sz_screen.x,
		sz_screen.y,
		view_mat,
		proj_mat
		//&mesh_mat
	);

	prev_mouse_3d = mip::geom::Screen2World(
		_prev_mouse_pt.x(),
		_prev_mouse_pt.y(),
		sz_screen.x,
		sz_screen.y,
		view_mat,
		proj_mat
		//&mesh_mat
	);

	auto mat_inverse = trans_mat.inverse();

	//float yaw, pitch, roll;
	//mat_inverse.getYawPitchRoll(yaw, pitch, roll);

	//mip::TRANSFORM tr(pitch, yaw, roll);
	//auto q = tr.getQuaternion();
	auto q = mat_inverse.getQuaternion();

	//cur_mouse_3d = q.rotateVector(cur_mouse_3d);
	//prev_mouse_3d = q.rotateVector(prev_mouse_3d);
	cur_mouse_3d = mip::math::VectorTransform(cur_mouse_3d, mat_inverse);
	prev_mouse_3d = mip::math::VectorTransform(prev_mouse_3d, mat_inverse);

	_new_cur_mouse_pt = mip::geom::WorldToScreen(
		cur_mouse_3d,
		sz_screen.x,
		sz_screen.y,
		view_mat,
		proj_mat
		//&mesh_mat
	);

	_new_prev_mouse_pt = mip::geom::WorldToScreen(
		prev_mouse_3d,
		sz_screen.x,
		sz_screen.y,
		view_mat,
		proj_mat
		//&mesh_mat
	);

	_direction = cur_mouse_3d - prev_mouse_3d;
}

/*
@brief
@return
*/
void CManipulator::checkCloseOffset(
	int& _dx,
	int& _dy,
	mip::QUATERNION& _q
)
{
	if ((m_vt_OffsetPoint.size() < 1) || ((_dx == 0) && (_dy == 0)))
	{
		return;
	}

	auto dlg = static_cast<CManpulateDlg*>(m_pMeshDlgManager->getMeshDialog(MESH_DIALOG_MANIPULATE));

	double interval = INTERVAL_TRANSLATE;

	if (dlg)
	{
		interval = dlg->getInterval();
	}

	switch (m_Axis_Seleted)
	{
	case TRANS_XY_AXIS:
	{
		m_Trans = mip::VECTOR3(interval * (float)_dx, -interval * (float)_dy, 0.f);
	}
	break;
	case TRANS_YZ_AXIS:
	{
		m_Trans = mip::VECTOR3(0.f, -interval * (float)_dy, -interval * (float)_dx);
	}
	break;
	case TRANS_XZ_AXIS:
	{
		m_Trans = mip::VECTOR3(interval * (float)_dx, 0.f, -interval * (float)_dy);
	}
	break;
	}
}

/*
@brief
@return
*/
float CManipulator::calcAngleFromMousePt()
{
	mip::MATRIX44 matView = m_p_camera->getView();
	mip::MATRIX44 matProj = m_p_camera->getProj();
	mip::VECTOR2 sz_screen = m_p_camera->getScreenSize();

	auto mesh_cube = m_AxisMesh[int(SCALE_ALL_AXIS)];
	auto cube_bb = mesh_cube->m_boundingBox;
	auto cube_center = cube_bb.getCenter();

	auto mat_cube = mesh_cube->getMatrix() * m_mat_offset * m_mat_view_world;
	auto cube_center_2d = mip::geom::WorldToScreen(cube_center, sz_screen.x, sz_screen.y, matView, matProj, &mat_cube);

	mip::VECTOR2 prev_2d(m_PrevMousePt.x(), m_PrevMousePt.y());
	mip::VECTOR2 cur_2d(m_CurrentMousePt.x(), m_CurrentMousePt.y());
	mip::VECTOR2 cube_2d(cube_center_2d.x, cube_center_2d.y);

	return calcAngle(prev_2d, cur_2d, cube_2d);
}

/*
@brief
@return
*/
float CManipulator::calcAngleOffsetPointAxisTrans()
{
	if (m_vt_OffsetPoint.size() == 0)
	{
		return FLT_MIN;
	}

	mip::MeshTopology* mesh = m_AxisMesh[(int)m_Axis_Seleted];
	mip::AABB 	bb = mesh->m_boundingBox;
	mip::VECTOR3 center_mesh = bb.getCenter();

	mip::MATRIX44 mat_view = m_p_camera->getView();
	mip::MATRIX44 mat_proj = m_p_camera->getProj();
	mip::VECTOR2 sz_screen = m_p_camera->getScreenSize();

	auto mesh_cube = m_AxisMesh[(int)SCALE_ALL_AXIS];
	auto mat_world = mesh_cube->getMatrix();

	auto mesh_center_2d = mip::geom::WorldToScreen(center_mesh, sz_screen.x, sz_screen.y, mat_view, mat_proj, &mat_world);
	auto center_2d = mip::geom::WorldToScreen(m_centerSnapping, sz_screen.x, sz_screen.y, mat_view, mat_proj, &mat_world);
	auto pt_2d = mip::geom::WorldToScreen(m_vt_OffsetPoint[m_vt_OffsetPoint.size() - 1], sz_screen.x, sz_screen.y, mat_view, mat_proj, &mat_world);

	return calcAngle(mip::VECTOR2(pt_2d.x, pt_2d.y), mip::VECTOR2(center_2d.x, center_2d.y), mip::VECTOR2(mesh_center_2d.x, mesh_center_2d.y));
}

/*
@brief
@return
*/
float CManipulator::calcAngle(mip::VECTOR2& _pt1, mip::VECTOR2& _pt2, mip::VECTOR2& _pt3)
{
	auto ccw = calculateCCW(
		_pt1,
		_pt3,
		_pt2
	);

	double d2y = 0.0;
	double d2x = 0.0;
	double d1y = 0.0;
	double d1x = 0.0;
	double angle = 0.0;


	d2y = (double)_pt2.y - (double)_pt3.y;
	d2x = (double)_pt2.x - (double)_pt3.x;

	d1y = (double)_pt1.y - (double)_pt3.y;
	d1x = (double)_pt1.x - (double)_pt3.x;

	if (ccw < 0)
	{
		angle = (atan(d2y / d2x) - atan(d1y / d1x)) * 180. / M_PI;
	}
	else
	{
		angle = (atan(d1y / d1x) - atan(d2y / d2x)) * 180. / M_PI;
	}


	if (angle < 0.0)
	{
		angle += 180.;
	}

	if (ccw > 0)
	{
		angle *= -1.0;
	}

	return (float)angle;
}

/*
@brief
@return
*/
void CManipulator::checkAngleLimit(mip::VECTOR3& _angle)
{
	if (_angle.x > 360.f)
	{
		_angle.x -= 360.f;
	}

	if (_angle.x < -360.f)
	{
		_angle.x += 360.f;
	}

	if (_angle.y > 360.f)
	{
		_angle.y -= 360.f;
	}

	if (_angle.y < -360.f)
	{
		_angle.y += 360.f;
	}

	if (_angle.z > 360.f)
	{
		_angle.z -= 360.f;
	}

	if (_angle.z < -360.f)
	{
		_angle.z += 360.f;
	}
}

/*
@brief
@return
*/
void	CManipulator::calcAxisDir(mip::RAY& _axis_dir, mip::RAY& _axis_cross_dir)
{
	auto center_axis = m_AxisMesh[(int)SCALE_ALL_AXIS];
	auto center_axis_center = center_axis->getMatrix() * center_axis->m_boundingBox.getCenter();

	switch (m_Axis_Seleted)
	{
	case ROTATE_X_AXIS:
	{
		auto sel_axis = m_AxisMesh[(int)TRANS_X_AXIS];
		auto sel_axis_center = sel_axis->getMatrix() * sel_axis->m_boundingBox.getCenter();

		_axis_dir.org = sel_axis_center;
		_axis_dir.dir = (center_axis_center - sel_axis_center).normalize();
	}
	break;
	case ROTATE_Y_AXIS:
	{	auto sel_axis = m_AxisMesh[(int)TRANS_Y_AXIS];
	auto sel_axis_center = sel_axis->getMatrix() * sel_axis->m_boundingBox.getCenter();

	_axis_dir.org = sel_axis_center;
	_axis_dir.dir = (center_axis_center - sel_axis_center).normalize();
	}
	break;
	case ROTATE_Z_AXIS:
	{
		auto sel_axis = m_AxisMesh[(int)TRANS_Z_AXIS];
		auto sel_axis_center = sel_axis->getMatrix() * sel_axis->m_boundingBox.getCenter();

		_axis_dir.org = sel_axis_center;
		_axis_dir.dir = (center_axis_center - sel_axis_center).normalize();
	}
	break;
	}

	auto sel_axis = m_AxisMesh[(int)m_Axis_Seleted];
	auto sel_axis_center = sel_axis->getMatrix() * sel_axis->m_boundingBox.getCenter();

	_axis_cross_dir.org = sel_axis_center;
	_axis_cross_dir.dir = (center_axis_center - sel_axis_center).normalize();
}

/*
@brief
@return
*/
mip::VECTOR3	CManipulator::calcIntersectPoint(
	mip::RAY _ray1,
	mip::RAY _ray2
)
{
	return _ray1.org.cross(_ray2.dir);
}

/*
@brief
@return
*/
float CManipulator::calculateCCW(mip::VECTOR2& _pt1, mip::VECTOR2& _pt2, mip::VECTOR2& _pt3)
{
	float dir = _pt1.x * _pt2.y + _pt2.x * _pt3.y + _pt3.x * _pt1.y;

	dir = dir - _pt1.y * _pt2.x - _pt2.y * _pt3.x - _pt3.y * _pt1.x;

	return dir;
}

/*
@brief
@return
*/
void CManipulator::initParams()
{
	m_bFinished = false;

	m_nCount = 0;

	m_TransAccumulate = mip::VECTOR3();
	m_RotAngleAccumulateTemp = mip::VECTOR3();

	m_TransformTemp = mip::TRANSFORM();

	m_RotMatTemp = mip::MATRIX44();
}

/*
@brief
@return
*/
void CManipulator::initSnapping()
{
	m_MoveSnapInterval = mip::VECTOR3();

	// Translation
	{
		auto dlg = static_cast<CManpulateDlg*>(m_pMeshDlgManager->getMeshDialog(MESH_DIALOG_MANIPULATE));

		double interval = INTERVAL_TRANSLATE;

		if (dlg)
		{
			interval = dlg->getInterval();
		}

		m_centerSnapping = m_AxisMesh[(int)SCALE_ALL_AXIS]->m_boundingBox.getCenter();
		m_vt_OffsetPoint.clear();
		calcOffsetPoint(m_vt_OffsetPoint, m_centerSnapping, NUMBER_OF_ROWS, NUMBER_OF_COLS, interval);
	}

	// Rotation
	{
		CreateRotateSnappingPoint(m_vt_RotateSnappingPoint);

		mip::VECTOR3 center;

		for (int i = 0; i < m_vt_RotateSnappingPoint.size(); ++i)
		{
			center = center + m_vt_RotateSnappingPoint[i];
		}

		center /= (float)m_vt_RotateSnappingPoint.size();

		auto mesh = m_AxisMesh[(int)SCALE_ALL_AXIS];

		auto mat_world = mesh->getMatrix();

		auto bb = mesh->m_boundingBox;
		auto bb_center = mat_world * bb.getCenter();

		auto diff = bb_center - center;

		Translate(m_vt_RotateSnappingPoint, diff);

		auto q = mesh->getQuaternion();

		mip::VECTOR3 vec;
		if (m_Axis_Seleted == ROTATE_Y_AXIS)
		{
			vec = mip::VECTOR3(90.f, 0.f, 0.f);
		}
		else if (m_Axis_Seleted == ROTATE_X_AXIS)
		{
			vec = mip::VECTOR3(0.f, -90.f, 0.f);
		}

		//vec = q.rotateVector(vec);

		if (m_bBack_Selected)
		{
			vec *= -1.f;
		}

		if (vec != vec.Zero)
		{
			RotateFromPoint(m_vt_RotateSnappingPoint, vec, bb_center);
		}
	}
}

/*
@brief
@return
*/
void	CManipulator::initSphere()
{
	auto mesh = m_AxisMesh[int(SCALE_ALL_AXIS)];
	auto mat = mesh->getMatrix();
	auto& mesh_bb = mesh->m_boundingBox;

	if (m_SphereCube)
	{
		SAFE_DELETE(m_SphereCube);

		m_SphereCube = new mip::MeshTopology(m_pRenderer);
		CreateSphere(m_SphereCube, mip::VECTOR4(0, 255, 0, 255));

		auto& bb = m_SphereCube->m_boundingBox;

		auto diff = mesh_bb.getCenter() - bb.getCenter();

		Translate(m_SphereCube, diff);
		m_SphereCube->mergingVertex();
		m_SphereCube->buildTree();
		m_SphereCube->updateVertex();
		WIN_MANAGER->buildRenderBufferTopology(m_SphereCube);

		m_SphereCube->setRotate(mat.getQuaternion());
		m_SphereCube->setTranslate(mat.getOrigin());
	}

	if (m_Sphere)
	{
		SAFE_DELETE(m_Sphere);

		m_Sphere = new mip::MeshTopology(m_pRenderer);

		CreateSphere(m_Sphere, mip::VECTOR4(0, 255, 0, 255));

		auto& bb = m_Sphere->m_boundingBox;

		auto diff = mesh_bb.getCenter() - bb.getCenter();

		Translate(m_Sphere, diff);
		m_Sphere->mergingVertex();
		m_Sphere->buildTree();
		m_Sphere->updateVertex();
		WIN_MANAGER->buildRenderBufferTopology(m_Sphere);

		m_Sphere->setRotate(mat.getQuaternion());
		m_Sphere->setTranslate(mat.getOrigin());
	}
}

/*
@brief
@return
*/
void	CManipulator::initPlane()
{
	if ((TRANS_XY_AXIS != m_Axis_Seleted) && (TRANS_XZ_AXIS != m_Axis_Seleted) && (TRANS_YZ_AXIS != m_Axis_Seleted))
	{
		return;
	}

	int idx = (int)m_Axis_Seleted - 3;

	CreatePlane(m_SnappingPlane[idx], m_Axis_Seleted, LEN_PLANE_LINE);

	auto& bb = m_SnappingPlane[idx]->m_boundingBox;

	auto mesh = m_AxisMesh[int(SCALE_ALL_AXIS)];
	auto mat = mesh->getMatrix();
	auto& mesh_bb = mesh->m_boundingBox;
	auto diff = mesh_bb.getCenter() - bb.getCenter();

	Translate(m_SnappingPlane[idx], diff, true);
	m_SnappingPlane[idx]->updateVertex();
	WIN_MANAGER->buildRenderBufferTopology(m_SnappingPlane[idx]);

	m_SnappingPlane[idx]->setTranslate(mat.getOrigin());
	m_SnappingPlane[idx]->setRotate(mat.getQuaternion());
	//mat = m_AxisMesh[int(m_Axis_Seleted)]->getMatrix();
}

/*
@brief
@return
*/
void	CManipulator::initCtrl()
{
	auto view = WIN_MANAGER->mainMeshWidget->getMainView();
	if (!m_Label)
	{
		m_Label = new QLabel("0.0", view);
		m_Label->setAlignment(Qt::AlignCenter);
		m_Label->setStyleSheet("background-color: rgba(255, 255, 255, 50%) ; color : back; font: 20pt;");
		m_Label->resize(QSize(150, 50));
	}

	if (!m_Label1)
	{
		m_Label1 = new QLabel("0.0", view);
		m_Label1->setAlignment(Qt::AlignCenter);
		m_Label1->setStyleSheet("background-color: rgba(255, 255, 255, 50%) ; color : back; font: 20pt;");
		m_Label1->resize(QSize(150, 50));
	}

	if (!m_Label2)
	{
		m_Label2 = new QLabel("0.0", view);
		m_Label2->setAlignment(Qt::AlignCenter);
		m_Label2->setStyleSheet("background-color: rgba(255, 255, 255, 50%) ; color : back; font: 20pt;");
		m_Label2->resize(QSize(150, 50));
	}

	auto mesh = m_AxisMesh[int(SCALE_ALL_AXIS)];
	auto mat = mesh->getMatrix();

	auto trans = mat.getOrigin();

	mip::TRANSFORM tr(mat.getQuaternion());
	trans += tr.getMatrix() * m_TransAccumulate;

	mip::MATRIX44 view_mat = m_p_camera->getView();
	mip::MATRIX44 proj_mat = m_p_camera->getProj();
	mip::MATRIX44 world_mat = mesh->getMatrix();
	mip::VECTOR2 sz_screen = m_p_camera->getScreenSize();

	auto pt_2d = mip::geom::WorldToScreen(
		trans,
		sz_screen.x,
		sz_screen.y,
		view_mat,
		proj_mat
	);

	m_Label->move(QPoint((int)pt_2d.x, (int)pt_2d.y));

	//m_Label->show();
	m_Label->hide();

	switch (m_Axis_Seleted)
	{
	case TRANS_X_AXIS:
	case TRANS_Y_AXIS:
	case TRANS_Z_AXIS:
	case ROTATE_X_AXIS:
	case ROTATE_Y_AXIS:
	case ROTATE_Z_AXIS:
	case SCALE_X_AXIS:
	case SCALE_Y_AXIS:
	case SCALE_Z_AXIS:
		m_Label->resize(QSize(75, 50));
		break;
	case TRANS_XY_AXIS:
	case TRANS_YZ_AXIS:
	case TRANS_XZ_AXIS:
		m_Label->resize(QSize(150, 50));
		break;
	case SCALE_ALL_AXIS:
		m_Label->resize(QSize(75, 50));
		m_Label1->resize(QSize(75, 50));
		m_Label2->resize(QSize(75, 50));
		break;
	}
}

/*
@brief
@return
*/
void	CManipulator::updateCtrl()
{
	if (!m_bUpdateCtrl)
	{
		return;
	}

	//auto view = WIN_MANAGER->mainMeshWidget->getMainView();
	if (!m_Label)
	{
		m_Label = new QLabel("0.0");
		m_Label->setAlignment(Qt::AlignCenter);
		m_Label->setStyleSheet("background-color: rgba(255, 255, 255, 50%) ; color : back; font: 20pt;");
		m_Label->resize(QSize(150, 50));
	}

	if (!m_Label1)
	{
		m_Label1 = new QLabel("0.0");
		m_Label1->setAlignment(Qt::AlignCenter);
		m_Label1->setStyleSheet("background-color: rgba(255, 255, 255, 50%) ; color : back; font: 20pt;");
		m_Label1->resize(QSize(150, 50));
	}

	if (!m_Label2)
	{
		m_Label2 = new QLabel("0.0");
		m_Label2->setAlignment(Qt::AlignCenter);
		m_Label2->setStyleSheet("background-color: rgba(255, 255, 255, 50%) ; color : back; font: 20pt;");
		m_Label2->resize(QSize(150, 50));
	}

	auto trans_inv_mat = m_Transform.getMatrix().inverse();
	auto inv_q = trans_inv_mat.getQuaternion();

	auto rot_trans = inv_q.rotateVector(m_TransAccumulate);

	switch (m_Axis_Seleted)
	{
	case TRANS_X_AXIS:
		m_Label->setText(QString().sprintf("%.2f", rot_trans.x));
		break;
	case TRANS_Y_AXIS:
		m_Label->setText(QString().sprintf("%.2f", rot_trans.y));
		break;
	case TRANS_Z_AXIS:
		m_Label->setText(QString().sprintf("%.2f", rot_trans.z));
		break;
	case ROTATE_X_AXIS:
		m_Label->setText(QString().sprintf("%.2f", m_RotAngleAccumulateTemp.x));
		break;
	case ROTATE_Y_AXIS:
		m_Label->setText(QString().sprintf("%.2f", m_RotAngleAccumulateTemp.y));
		break;
	case ROTATE_Z_AXIS:
		m_Label->setText(QString().sprintf("%.2f", m_RotAngleAccumulateTemp.z));
		break;
	case TRANS_XY_AXIS:
		m_Label->setText(QString().sprintf("%.2f, %.2f", m_TransAccumulate.x, m_TransAccumulate.y));
		break;
	case TRANS_YZ_AXIS:
		m_Label->setText(QString().sprintf("%.2f, %.2f", m_TransAccumulate.y, m_TransAccumulate.z));
		break;
	case TRANS_XZ_AXIS:
		m_Label->setText(QString().sprintf("%.2f, %.2f", m_TransAccumulate.x, m_TransAccumulate.z));
		break;
	case SCALE_X_AXIS:
		m_Label->setText(QString().sprintf("%.2f", m_Transform.scale.x));
		break;
	case SCALE_Y_AXIS:
		m_Label->setText(QString().sprintf("%.2f", m_Transform.scale.y));
		break;
	case SCALE_Z_AXIS:
		m_Label->setText(QString().sprintf("%.2f", m_Transform.scale.z));
		break;
	case SCALE_ALL_AXIS:
		m_Label->setText(QString().sprintf("%.2f", m_Transform.scale.x));
		m_Label1->setText(QString().sprintf("%.2f", m_Transform.scale.y));
		m_Label2->setText(QString().sprintf("%.2f", m_Transform.scale.z));
		break;
	}

	mip::MATRIX44 mat_view = m_p_camera->getView();
	mip::MATRIX44 mat_proj = m_p_camera->getProj();
	mip::VECTOR2 sz_screen = m_p_camera->getScreenSize();

	mip::MATRIX44 mat_world = m_mat_offset * m_mat_view_world;

	switch (m_Axis_Seleted)
	{
	case ROTATE_X_AXIS:
	case ROTATE_Y_AXIS:
	case ROTATE_Z_AXIS:
	{
		m_Label->move(QPoint(m_CurrentMousePt.x() + 5, m_CurrentMousePt.y() - 5));
	}
	break;
	case TRANS_X_AXIS:
	case TRANS_Y_AXIS:
	case TRANS_Z_AXIS:
	case TRANS_XY_AXIS:
	case TRANS_YZ_AXIS:
	case TRANS_XZ_AXIS:
	{
		auto mesh_cube = m_AxisMesh[(int)SCALE_ALL_AXIS];
		auto center_cube = mesh_cube->m_boundingBox.getCenter();
		auto mat_cube = mesh_cube->getMatrix() * mat_world;

		auto center_2d = mip::geom::WorldToScreen(center_cube, sz_screen.x, sz_screen.y, mat_view, mat_proj, &mat_cube);

		m_Label->move(QPoint(int(center_2d.x) + 50, int(center_2d.y) - 50));
	}
	break;
	case SCALE_X_AXIS:
	case SCALE_Y_AXIS:
	case SCALE_Z_AXIS:
	{
		auto mesh_scale = m_AxisMesh[(int)m_Axis_Seleted];
		auto center_scale = mesh_scale->m_boundingBox.getCenter();
		auto mat_mesh = mesh_scale->getMatrix() * mat_world;

		auto center_2d = mip::geom::WorldToScreen(center_scale, sz_screen.x, sz_screen.y, mat_view, mat_proj, &mat_mesh);

		if (m_Axis_Seleted == SCALE_X_AXIS)
		{
			m_Label->move(QPoint(int(center_2d.x) + 25, int(center_2d.y)));
		}
		else if (m_Axis_Seleted == SCALE_Y_AXIS)
		{
			m_Label->move(QPoint(int(center_2d.x), int(center_2d.y) + 25));
		}
		else
		{
			m_Label->move(QPoint(int(center_2d.x), int(center_2d.y) - 25));
		}
	}
	break;
	case SCALE_ALL_AXIS:
	{
		int start = (int)SCALE_X_AXIS;
		int finish = (int)SCALE_ALL_AXIS;

		for (int idx = start; idx < finish; ++idx)
		{
			auto mesh_scale = m_AxisMesh[idx];
			auto center_scale = mesh_scale->m_boundingBox.getCenter();
			auto mat_mesh = mesh_scale->getMatrix() * mat_world;

			auto center_2d = mip::geom::WorldToScreen(center_scale, sz_screen.x, sz_screen.y, mat_view, mat_proj, &mat_mesh);

			if (idx == SCALE_X_AXIS)
			{
				m_Label->move(QPoint(int(center_2d.x) + 25, int(center_2d.y)));
			}
			else if (idx == SCALE_Y_AXIS)
			{
				m_Label1->move(QPoint(int(center_2d.x), int(center_2d.y) + 25));
			}
			else
			{
				m_Label2->move(QPoint(int(center_2d.x), int(center_2d.y) - 25));
			}
		}

		m_Label1->show();
		m_Label2->show();
	}
	break;
	}

	if (m_Axis_Seleted != INVERSE_Z_AXIS)
	{
		m_Label->show();
	}

	if (abs(m_MoveSnapInterval.x) >= 5.f || abs(m_MoveSnapInterval.y) >= 5.f || abs(m_MoveSnapInterval.z) >= 5.f)
	{
		switch (m_Axis_Seleted)
		{
		case TRANS_XY_AXIS:
		case TRANS_YZ_AXIS:
		case TRANS_XZ_AXIS:
		{
			MovePlane(m_MoveSnapInterval);
		}
		break;
		}

		switch (m_Axis_Seleted)
		{
		case TRANS_XY_AXIS:
		case TRANS_YZ_AXIS:
		case TRANS_XZ_AXIS:
		case TRANS_X_AXIS:
		case TRANS_Y_AXIS:
		case TRANS_Z_AXIS:
			m_MoveSnapInterval = mip::VECTOR3();
			break;
		}
	}
}

/*
@brief
@return
*/
void CManipulator::resetPrevTransform()
{
	m_PrevTransform.zero();

	for (int idx = 0; idx < NUM_AXIS_MANIPULATOR; ++idx)
	{
		m_PrevAxisTransform[idx].zero();
	}
}

/*
@brief
@return
*/
void CManipulator::UpdatePrevTransform(mip::MeshTopology* _p_mesh)
{
	m_PrevTransform.setRotate(_p_mesh->rotation);
	m_PrevTransform.setTranslate(_p_mesh->translation);
	m_PrevTransform.setScale(_p_mesh->scale);

	for (int idx = 0; idx < NUM_AXIS_MANIPULATOR; ++idx)
	{
		m_PrevAxisTransform[idx].setRotate(m_AxisMesh[idx]->rotation);
		m_PrevAxisTransform[idx].setTranslate(m_AxisMesh[idx]->translation);
		//m_PrevAxisTransform[idx].setScale(m_AxisMesh[idx]->scale);
	}
}

/*
@brief
@return
*/
void CManipulator::UpdateInitTransform(mip::MeshTopology* _p_mesh)
{
	//auto mat = _p_mesh->getMatrix();

	m_InitTransform.setRotate(_p_mesh->rotation);
	m_InitTransform.setTranslate(_p_mesh->translation);
	m_InitTransform.setScale(_p_mesh->scale);
	//m_InitTransform.setScale(mat.getScaleVector());

	m_InitBB = _p_mesh->m_boundingBox;

	for (int i = 0; i < NUM_AXIS_MANIPULATOR; ++i)
	{
		//mat = m_AxisMesh[i]->getMatrix();

		//m_InitAxisTransform[i].setRotate(mat.getQuaternion());
		//m_InitAxisTransform[i].setTranslate(mat.getOrigin());

		m_InitAxisTransform[i].setRotate(m_AxisMesh[i]->rotation);
		m_InitAxisTransform[i].setTranslate(m_AxisMesh[i]->translation);
	}
}

void CManipulator::setTransformMat(mip::TRANSFORM& _trans)
{
	m_Transform = _trans;
}

//mip::MATRIX44 CManipulator::getTransformMat() 
//{
//	return m_Transform.getMatrix(); 
//}

mip::TRANSFORM CManipulator::getTransformMat()
{
	return m_Transform;
}

mip::MATRIX44 CManipulator::getTransformMatAxis(int _idx)
{
	return m_AxisMesh[_idx]->getMatrix();
}

void CManipulator::setPrevTransformMat(mip::TRANSFORM& _trans)
{
	m_PrevTransform = _trans;
}

mip::TRANSFORM CManipulator::getPrevTransformMat()
{
	//return m_PrevTransform.getMatrix();
	return m_PrevTransform;
}

mip::MATRIX44 CManipulator::getPrevTransformMatAxis(int _idx)
{
	return m_PrevAxisTransform[_idx].getMatrix();
}

mip::MATRIX44 CManipulator::getRotateTempMat() 
{
	return m_RotMatTemp; 
}

mip::VECTOR3 CManipulator::getTranslateTempVal() 
{
	return m_Trans; 
}

mip::VECTOR3 CManipulator::getRotateTempAngle() 
{
	return m_Angle; 
}
mip::VECTOR3 CManipulator::getCenter()
{
	return m_Center; 
}

mip::VECTOR3 CManipulator::getScaleVec() 
{
	return m_ScaleVec; 
}
mip::VECTOR3 CManipulator::getScaleTranslation() 
{
	return m_ScaleTranslate; 
}

void CManipulator::setAxisDirection(MANIPULATOR_DIR_TYPE _axis)
{
	m_Axis_Seleted = _axis; 
}
MANIPULATOR_DIR_TYPE CManipulator::getAxisDirection() 
{
	return m_Axis_Seleted; 
}

void CManipulator::enableUpdateCtrl(bool _enable) 
{
	m_bUpdateCtrl = _enable;
}

/*
@brief
@return
*/
void CManipulator::UpdatePrevTransform(mip::MATRIX44& _matrix)
{
	auto q = _matrix.getQuaternion();
	auto offset = _matrix.getOrigin();
	auto scale = _matrix.getScaleVector();

	m_PrevTransform.setRotate(q);
	m_PrevTransform.setTranslate(offset);
	m_PrevTransform.setScale(scale);

	for (int idx = 0; idx < NUM_AXIS_MANIPULATOR; ++idx)
	{
		m_PrevAxisTransform[idx].setRotate(q);
		m_PrevAxisTransform[idx].setTranslate(offset);
	}
}

/*
@brief
@return
*/
void CManipulator::showAxisArrow(MANIPULATOR_DIR_TYPE _type, bool _b_show)
{
	int idx = (int)_type;

	if (idx > -1 && idx < NUM_AXIS_MANIPULATOR)
	{
		m_bShowFlag[idx] = _b_show;
	}
}

/*
@brief
@return
*/
bool	CManipulator::isCheckedArrow()
{
	return m_Axis_Seleted < NONE_AXIS;
}

/*
@brief
@return
*/
void	CManipulator::setInverseZAxis()
{
	m_bInverseZAxis = (m_bInverseZAxis ? false : true);

	auto mesh = m_AxisMesh[(int)INVERSE_Z_AXIS];
	auto cube_mesh = m_AxisMesh[(int)SCALE_ALL_AXIS];
	auto bbox = cube_mesh->m_boundingBox;
	auto mesh_center = mesh->m_boundingBox;

	auto org_mat = mesh->getMatrix();
	auto mat_trans = m_Transform.getMatrix();

	auto vec = m_bInverseZAxis ? mip::VECTOR3(180.f, 0.f, 0.f) : mip::VECTOR3(0.f, 0.f, 0.f);

	auto mat_rot = calcMatrixRotateFromPoint(vec, bbox.getCenter());
	mesh->setRotate(mat_rot.getQuaternion());
	mesh->setTranslate(mat_rot.getOrigin());

	mesh->addRotate(mat_trans.getQuaternion());

	mesh->addTranslate(org_mat.getOrigin());

	m_Axis_Seleted = NONE_AXIS;
}

/*
@brief
@return
*/
bool	CManipulator::isInverseZAxis()
{
	return m_bInverseZAxis;
}

