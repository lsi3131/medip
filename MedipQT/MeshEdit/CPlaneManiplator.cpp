#include "stdafx.h"
#include "windowManager.h"
#include "Main/MainMeshWidget.h"
#include "MEVolumeView.h"
#include "MeshControl.h"
#include "Renderer/effect.h"
#include "Renderer/Renderer.h"
#include "Renderer/ShaderCommon.h"

#include "Math/Range.h"

#include "ActionManager.h"

#include "CPlaneManiplator.h"
#include "CMeshDlgManager.h"
#include "CMeshWorkManager.h"
#include "CMeshModelViewManager.h"

#include "DataContext.h"

// Start CPlaneManiplator

CPlaneManiplator* CPlaneManiplator::m_pInstance = nullptr;

CPlaneManiplator* CPlaneManiplator::GetInstance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = new CPlaneManiplator(g_Renderer);
	}
	return m_pInstance;
}

CPlaneManiplator::CPlaneManiplator(mip::Renderer* pRenderer) :
	CManipulator(pRenderer),
	m_pPlaneMesh(nullptr)
{
}

CPlaneManiplator::~CPlaneManiplator()
{
	if (m_pPlaneMesh)
	{
		SAFE_DELETE(m_pPlaneMesh);
	}
}

void CPlaneManiplator::Init(DataContext* pDataContext)
{
}

void CPlaneManiplator::Init(
	DataContext* pDataContext,
	CMeshModelViewManager* pModelViewManager,
	CMeshDlgManager* pMeshDlgManager)
{
	CManipulator::Init(pModelViewManager, pMeshDlgManager);
	m_pDataContext = pDataContext;

	m_Axis_Seleted = TRANS_XY_AXIS;

	if (m_pPlaneMesh == nullptr)
	{
		m_pPlaneMesh = new mip::MeshTopology(m_pRenderer);
	}

	m_centerSnapping = m_AxisMesh[(int)SCALE_ALL_AXIS]->m_boundingBox.getCenter();

	CreatePlane(m_pPlaneMesh, TRANS_XY_AXIS, LEN_PLANE_LINE * 3.f);

	m_pPlaneMesh->setAlpha(50.f);

	m_pPlaneMesh->m_boundingBox.min.z = 0.f;
	m_pPlaneMesh->m_boundingBox.max.z = 0.f;

	mip::VECTOR3	offset_center(-m_pDataContext->volume_data.getSizeX() * 0.5f, -m_pDataContext->volume_data.getSizeY() * 0.5f, -m_pDataContext->volume_data.getSizeZ() * 0.5f);
	m_pPlaneMesh->addTranslate(-offset_center);

	m_pPlaneMesh->updateVertex();
	WIN_MANAGER->buildRenderBufferTopology(m_pPlaneMesh);

	// inverse arrow show
	m_bShowFlag[(int)INVERSE_Z_AXIS] = true;
	m_bUseFlag[int(INVERSE_Z_AXIS)] = true;

	m_bShowFlag[(int)SCALE_X_AXIS] = false;
	m_bShowFlag[(int)SCALE_Y_AXIS] = false;
	m_bShowFlag[(int)SCALE_Z_AXIS] = false;
	m_bUseFlag[int(SCALE_X_AXIS)] = false;
	m_bUseFlag[int(SCALE_Y_AXIS)] = false;
	m_bUseFlag[int(SCALE_Z_AXIS)] = false;

	m_bUseFlag[int(SCALE_ALL_AXIS)] = false;

	for (int i = 0; i < NUM_AXIS_MANIPULATOR; ++i)
	{
		m_AxisMesh[i]->addTranslate(-offset_center);
	}

	enableUpdateCtrl(true);
}

void CPlaneManiplator::Reset()
{
	auto mat_init = m_InitTransform.getMatrix();

	mip::VECTOR3	offset_center(-m_pDataContext->volume_data.getSizeX() * 0.5f, -m_pDataContext->volume_data.getSizeY() * 0.5f, -m_pDataContext->volume_data.getSizeZ() * 0.5f);

	for (int i = 0; i < NUM_AXIS_MANIPULATOR; ++i)
	{
		if (m_AxisMesh[i])
		{
			m_AxisMesh[i]->setRotate(mat_init.getQuaternion());
			m_AxisMesh[i]->setTranslate(mat_init.getOrigin());

			m_AxisMesh[i]->addTranslate(-offset_center);
		}
	}

	m_pPlaneMesh->setRotate(mat_init.getQuaternion());
	m_pPlaneMesh->setTranslate(mat_init.getOrigin());

	m_pPlaneMesh->addTranslate(-offset_center);

	m_Transform.zero();

	m_TransAccumulate = mip::VECTOR3();
	m_RotAngleAccumulate = mip::VECTOR3();

	//m_bUpdatePivot = false;
}

/*
@brief
@return
*/
void CPlaneManiplator::render(
	DataContext* pDataContext,
	mip::Renderer* _p_renderer,
	mip::MATRIX44& _mat_view_world,
	mip::MATRIX44& _mat_offset,
	mip::SCAMERA* _p_camera
)
{
	if (_p_camera)
	{
		m_mat_view_world = _mat_view_world;
		m_mat_offset = _mat_offset;
		m_p_camera = _p_camera;

		DrawPlaneMesh();

		DrawPlane();

		DrawArrow(pDataContext, _p_renderer);

		DrawSphere();
	}
}

/*
@brief
@return
*/
void	CPlaneManiplator::DrawPlaneMesh()
{
	auto mat_world = m_pModelViewManager->GetMainTransform();
	auto camera = m_pModelViewManager->GetCameraPtr();
	auto matView = camera->getView();
	auto matProj = camera->getProj();

	if (m_pPlaneMesh)
	{
		mip::MATRIX44				mat_offset;
		mip::VECTOR3				offset_center(-m_pDataContext->volume_data.getSizeX() * 0.5f, -m_pDataContext->volume_data.getSizeY() * 0.5f, -m_pDataContext->volume_data.getSizeZ() * 0.5f);

		mat_offset.identity();
		mat_offset.translation(offset_center);

		if (m_pPlaneMesh->renderBegin(m_pRenderer, 0, false, true))
		{
			auto axis_world = m_AxisMesh[(int)SCALE_ALL_AXIS]->getMatrix();
			//auto plane_world = m_PlaneMesh->getMatrix() * mat_world * mat_offset;
			auto plane_world = axis_world * mat_offset * mat_world;

			mip::VECTOR4 color = mip::VECTOR4(255.f, 255.f, 255.f, m_pPlaneMesh->getAlphaVal());

			mip::ShaderCommon* pSC = m_pPlaneMesh->getShaderCommon();
			m_pPlaneMesh->getShaderCommon()->set(plane_world, matView, matProj, color);
			m_pPlaneMesh->renderPosition(*m_pPlaneMesh->getShaderCommon(), *m_pPlaneMesh->getShaderMeshData(), m_pPlaneMesh->getShaderType());

			m_pPlaneMesh->render(false);
			m_pPlaneMesh->renderEnd(m_pRenderer);
		}
	}
}

/*
@brief
@return
*/
void	CPlaneManiplator::DrawSnapping(QPainter* _p_paint)
{
	DrawSnappingInternal(_p_paint);

	DrawPlaneOutline(_p_paint);
}

/*
@brief
@return
*/
void	CPlaneManiplator::DrawPlaneOutline(QPainter* _p_paint)
{
	auto mat_world = m_pModelViewManager->GetMainTransform();
	auto camera = m_pModelViewManager->GetCameraPtr();
	auto mat_view = camera->getView();
	auto mat_proj = camera->getProj();
	auto sz_screen = camera->getScreenSize();

	_p_paint->setPen(QPen(Qt::gray, 1));

	QPolygon		polygon;


	//mip::MATRIX44	mat_mesh = m_PlaneMesh->getMatrix();
	auto	mat_mesh = m_AxisMesh[(int)SCALE_ALL_AXIS]->getMatrix();

	mip::MATRIX44				mat_offset;
	mip::VECTOR3				offset_center(-m_pDataContext->volume_data.getSizeX() * 0.5f, -m_pDataContext->volume_data.getSizeY() * 0.5f, -m_pDataContext->volume_data.getSizeZ() * 0.5f);

	mat_offset.identity();
	mat_offset.translation(offset_center);

	mat_mesh *= mat_offset;
	mat_mesh *= mat_world;

	auto tverts = m_pPlaneMesh->m_tverts;
	auto n_tverts = (int)tverts.size();

	for (int vi = 0; vi < n_tverts; ++vi)
	{
		mip::VECTOR3 vt_2d = mip::geom::WorldToScreen(tverts[vi].pos, sz_screen.x, sz_screen.y, mat_view, mat_proj, &mat_mesh);

		polygon.push_back(QPoint((int)vt_2d.x, (int)vt_2d.y));
	}

	_p_paint->drawLine(polygon.at(0), polygon.at(1));
	_p_paint->drawLine(polygon.at(1), polygon.at(3));
	_p_paint->drawLine(polygon.at(3), polygon.at(2));
	_p_paint->drawLine(polygon.at(2), polygon.at(0));
}

/*
@brief
@return
*/
mip::MATRIX44	CPlaneManiplator::getWorldMat()
{
	mip::MATRIX44	mat_world = m_pModelViewManager->GetMainTransform();
	mip::MATRIX44	mat_mesh = m_AxisMesh[(int)SCALE_ALL_AXIS]->getMatrix();

	mip::MATRIX44				mat_offset;
	mip::VECTOR3				offset_center(-m_pDataContext->volume_data.getSizeX() * 0.5f, -m_pDataContext->volume_data.getSizeY() * 0.5f, -m_pDataContext->volume_data.getSizeZ() * 0.5f);

	mat_offset.identity();
	mat_offset.translation(offset_center);

	return mat_mesh;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
// End CPlaneManiplator