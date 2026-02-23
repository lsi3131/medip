#include "stdafx.h"
#include "CMeshManipulator.h"
#include "CManipulator.h"
#include "CMeshModelViewManager.h"
#include "CMeshDlgManager.h"
#include "CMeshWorkManager.h"
#include "windowManager.h"
#include "Main/MainMeshWidget.h"
#include "MEVolumeView.h"
#include "MeshControl.h"
#include "Renderer/effect.h"
#include "Renderer/Renderer.h"
#include "Math/Range.h"
#include "ActionManager.h"
#include "DataContext.h"

// Start CMeshManipulator
///////////////////////////////////////////////////////////////////////////////////////////////////
/*
@brief
*/
CMeshManipulator::CMeshManipulator() :
	CMeshManipulator(g_Renderer)
{
}

CMeshManipulator::CMeshManipulator(mip::Renderer* pRenderer) :
	m_pRenderer(pRenderer),
	m_pDataContext(nullptr),
	m_pMeshWorkManager(nullptr),
	m_pMeshModelViewManager(nullptr),
	m_pMeshDlgManager(nullptr)
{
}

/*
@brief
*/
CMeshManipulator::~CMeshManipulator()
{

}

void CMeshManipulator::Init(DataContext* pDataContext)
{
	Init(pDataContext,
		MESH_WORK_MANAGER,
		MESH_MODELVIEW_MANAGER,
		MESH_DIALOG_MANAGER
	);
}

void CMeshManipulator::Init(
	DataContext* pDataContext,
	CMeshWorkManager* pMeshWorkManager,
	CMeshModelViewManager* pMeshModelViewManager,
	CMeshDlgManager* pMeshDlgManager)
{
	m_pDataContext = pDataContext;
	m_pMeshWorkManager = pMeshWorkManager;
	m_pMeshModelViewManager = pMeshModelViewManager;
	m_pMeshDlgManager = pMeshDlgManager;
}

/*
@brief
@return
*/
CMeshManipulator* CMeshManipulator::getInstance()
{
	static CMeshManipulator instance;
	return &instance;
}

/*
@brief
@return
*/
void CMeshManipulator::Process(
	int   _idx_mesh,
	QPoint  	_prev_mouse_pt,
	QPoint  	_cur_mouse_pt
)
{
	if (IsValidate())
	{
		mip::MeshTopology* mesh = m_pDataContext->m_MeshData.GetMesh(_idx_mesh);
		int pck_id = m_pDataContext->m_MeshData.GetCurrentMeshIndex();
		mip::MeshTopology* pck_mesh = m_pDataContext->m_MeshData.GetMesh(pck_id);

		if (mesh)
		{
			if (pck_id == _idx_mesh)
			{
				m_vt_Manipulator[_idx_mesh].Process(m_pDataContext, mesh, _prev_mouse_pt, _cur_mouse_pt);
			}
			else
			{
				auto axis_dir = m_vt_Manipulator[_idx_mesh].getAxisDirection();

				switch (axis_dir)
				{
				case TRANS_X_AXIS:
				case TRANS_Y_AXIS:
				case TRANS_Z_AXIS:
				{
					auto trans = m_vt_Manipulator[pck_id].getTranslateTempVal();
					m_vt_Manipulator[_idx_mesh].processTranslate(mesh, trans);
				}
				break;
				case TRANS_XY_AXIS:
				case TRANS_YZ_AXIS:
				case TRANS_XZ_AXIS:
					break;
				case ROTATE_X_AXIS:
				case ROTATE_Y_AXIS:
				case ROTATE_Z_AXIS:
				{
					auto rot = m_vt_Manipulator[pck_id].getRotateTempMat();
					auto angle = m_vt_Manipulator[pck_id].getRotateTempAngle();
					auto center = m_vt_Manipulator[pck_id].getCenter();
					auto cross_dir_vec = m_vt_Manipulator[pck_id].m_axis_cross_dir;
					auto dir_vec = m_vt_Manipulator[pck_id].m_axis_dir;
					m_vt_Manipulator[_idx_mesh].processRotate(mesh, rot, angle, center, dir_vec, cross_dir_vec, pck_mesh);
				}
				break;
				case SCALE_X_AXIS:
				case SCALE_Y_AXIS:
				case SCALE_Z_AXIS:
				case SCALE_ALL_AXIS:
				{
					auto scale_vec = m_vt_Manipulator[pck_id].getScaleVec();
					auto t = m_vt_Manipulator[pck_id].getScaleTranslation();

					m_vt_Manipulator[_idx_mesh].processScale(mesh, scale_vec, t);
				}
				break;
				}
			}
		}
	}
}

/*
@brief
@return
*/
void CMeshManipulator::render(
	mip::Renderer* _p_renderer,
	mip::MATRIX44& _mat_view_world,
	mip::MATRIX44& _mat_offset,
	mip::SCAMERA* _p_camera
)
{
	auto mode = m_pMeshWorkManager->getWorkMode();

	if (mode != MESH_WORK_MANIFULATE)
	{
		return;
	}

	if (IsValidate())
	{
		int idx = m_pDataContext->m_MeshData.GetCurrentMeshIndex();

		if (idx > -1)
		{
			m_vt_Manipulator[idx].render(
				m_pDataContext,
				_p_renderer,
				_mat_view_world,
				_mat_offset,
				_p_camera
			);
		}
	}
}

/*
@brief
@return
*/
void CMeshManipulator::DrawSnapping(QPainter* _p_paint)
{
	if (IsValidate())
	{
		int idx = m_pDataContext->m_MeshData.GetCurrentMeshIndex();

		if (idx > -1)
		{
			m_vt_Manipulator[idx].DrawSnapping(_p_paint);
		}
	}
}

/*
@brief
@return
*/
bool CMeshManipulator::CheckClickedArrow(QPoint _pt, mip::SCAMERA* _p_camera)
{
	if (IsValidate())
	{
		int idx = m_pDataContext->m_MeshData.GetCurrentMeshIndex();

		if (idx < 0)
		{
			return false;
		}

		return m_vt_Manipulator[idx].CheckClickedArrow(m_pDataContext, _pt, _p_camera);
	}

	return false;
}

/*
@brief
@return
*/
void CMeshManipulator::ReadyProcess(
	int _idx_mesh,
	QPoint	_prev_mouse_pt,
	QPoint	_cur_mouse_pt,
	mip::MATRIX44& _mat_view_world,
	mip::MATRIX44& _mat_offset,
	mip::SCAMERA* _p_camera
)
{
	if (IsValidate())
	{
		if (_idx_mesh < 0 || _idx_mesh >= m_vt_Manipulator.size())
		{
			return;
		}

		int pck_idx = m_pDataContext->m_MeshData.GetCurrentMeshIndex();

		if (pck_idx != _idx_mesh)
		{
			auto axis_dir = m_vt_Manipulator[pck_idx].getAxisDirection();

			m_vt_Manipulator[_idx_mesh].setAxisDirection(axis_dir);

			m_vt_Manipulator[_idx_mesh].enableUpdateCtrl(false);
		}
		else
		{
			m_vt_Manipulator[_idx_mesh].enableUpdateCtrl(true);
		}

		m_vt_Manipulator[_idx_mesh].ReadyProcess(
			_prev_mouse_pt,
			_cur_mouse_pt,
			_mat_view_world,
			_mat_offset,
			_p_camera
		);
	}
}

/*
@brief
@return
*/
bool CMeshManipulator::FinishProcess(int _idx_mesh)
{
	bool b_check = false;
	if (IsValidate())
	{
		if (_idx_mesh < 0 || _idx_mesh >= m_vt_Manipulator.size())
		{
			return b_check;
		}

		mip::MeshTopology* mesh = m_pDataContext->m_MeshData.GetMesh(_idx_mesh);

		if (mesh)
		{
			b_check = m_vt_Manipulator[_idx_mesh].FinishProcess(mesh);
		}
	}

	return b_check;
}

/*
@brief
@return
*/
void CMeshManipulator::OkProcess(int _idx_mesh)
{
	if (IsValidate())
	{
		if (_idx_mesh < 0 || _idx_mesh >= m_vt_Manipulator.size())
		{
			return;
		}

		mip::MeshTopology* mesh = m_pDataContext->m_MeshData.GetMesh(_idx_mesh);

		if (mesh)
		{
			m_vt_Manipulator[_idx_mesh].OkProcess(mesh);
		}
	}
}

/*
@brief
@return
*/
void CMeshManipulator::CancelProcess(int _idx_mesh)
{
	if (IsValidate())
	{
		if (_idx_mesh < 0 || _idx_mesh >= m_vt_Manipulator.size())
		{
			return;
		}

		mip::MeshTopology* mesh = m_pDataContext->m_MeshData.GetMesh(_idx_mesh);

		if (mesh)
		{
			m_vt_Manipulator[_idx_mesh].CancelProcess(mesh);
		}
	}
}

/*
@brief
@return
*/
void CMeshManipulator::UpdatePrevTransform(int _idx_mesh)
{
	if (IsValidate())
	{
		mip::MeshTopology* mesh = m_pDataContext->m_MeshData.GetMesh(_idx_mesh);

		if (mesh)
		{
			m_vt_Manipulator[_idx_mesh].UpdatePrevTransform(mesh);
		}
	}
}

/*
@brief
@return
*/
void CMeshManipulator::UpdateInitTransform(int _idx_mesh)
{
	if (IsValidate())
	{
		mip::MeshTopology* mesh = m_pDataContext->m_MeshData.GetMesh(_idx_mesh);

		if (mesh)
		{
			m_vt_Manipulator[_idx_mesh].UpdateInitTransform(mesh);
		}
	}
}

/*
@brief
@return
*/
void CMeshManipulator::UpdateTransformAxis(
	int  _idx_mesh,
	int  _idx_axis,
	mip::MATRIX44& _mat
)
{
	if (IsValidate())
	{
		if (_idx_mesh < 0)
		{
			return;
		}

		return m_vt_Manipulator[_idx_mesh].UpdateTransformAxis(_idx_axis, _mat);
	}
}

/*
@brief
@return
*/
void CMeshManipulator::AddTransformAxis(int _idx_mesh, mip::MATRIX44& _mat)
{
	if (IsValidate())
	{
		if (_idx_mesh < 0)
		{
			return;
		}

		return m_vt_Manipulator[_idx_mesh].AddTransformAxis(_mat);
	}
}

/*
@brief
@return
*/
void CMeshManipulator::UpdatePosition(
	mip::MeshTopology* _p_mesh,
	bool _init
)
{
	if (IsValidate())
	{
		int idx = m_pDataContext->m_MeshData.GetCurrentMeshIndex();

		if (idx < 0)
		{
			return;
		}

		return m_vt_Manipulator[idx].UpdatePosition(_p_mesh, _init);
	}
}

/*
@brief
@return
*/
bool CMeshManipulator::isCheckedArrow()
{
	if (IsValidate())
	{
		int idx = m_pDataContext->m_MeshData.GetCurrentMeshIndex();

		if (idx < 0)
		{
			return false;
		}

		return m_vt_Manipulator[idx].isCheckedArrow();
	}

	return false;
}

/*
@brief
@return
*/
void CMeshManipulator::UpdatePivotPoint(
	mip::MeshTopology* _p_mesh,
	mip::MATRIX44& _matrix
)
{
	if (IsValidate())
	{
		int idx = m_pDataContext->m_MeshData.GetCurrentMeshIndex();

		if (idx < 0)
		{
			return;
		}

		m_vt_Manipulator[idx].UpdatePivotPoint(_p_mesh, _matrix);
	}
}

/*
@brief
@return
*/
void CMeshManipulator::Reset()
{
	if (IsValidate())
	{
		for (int i = 0; i < m_vt_Manipulator.size(); ++i)
		{
			m_vt_Manipulator[i].Reset();
		}
	}
}

/*
@brief
@return
*/
void CMeshManipulator::HomePosition()
{
	if (IsValidate())
	{
		for (int i = 0; i < m_vt_Manipulator.size(); ++i)
		{
			m_vt_Manipulator[i].HomePosition();
		}
	}
}

/*
@brief
@return
*/
void CMeshManipulator::SaveUndoRedo(int _idx)
{
	if (IsValidate())
	{
		mip::MeshTopology* p_mesh = m_pDataContext->m_MeshData.GetMesh(_idx);

		if (p_mesh)
		{
			m_vt_Manipulator[_idx].SaveUndoRedo(p_mesh);
		}
	}
}

/*
@brief
@return
*/
void CMeshManipulator::deleteManipulator(int _idx)
{
	if (_idx < 0 || _idx >= m_vt_Manipulator.size())
	{
		return;
	}

	m_vt_Manipulator.erase(m_vt_Manipulator.begin() + _idx);
}

/*
@brief
@return
*/
void CMeshManipulator::clear()
{
	m_vt_Manipulator.clear();
}

/*
@brief
@return
*/
void CMeshManipulator::indexChange(int _idx, int _dest_idx)
{
	CManipulator tmp(m_pRenderer);
	tmp.Init(m_pMeshModelViewManager, m_pMeshDlgManager);

	tmp = m_vt_Manipulator[_idx];

	deleteManipulator(_idx);

	m_vt_Manipulator.insert(m_vt_Manipulator.begin() + _dest_idx, tmp);
}

/*
@brief
@return
*/
bool CMeshManipulator::InsertManipulator(int idx)
{
	if (m_vt_Manipulator.size() < idx)
	{
		return false;
	}

	CManipulator manipulator(m_pRenderer);
	manipulator.Init(m_pMeshModelViewManager, m_pMeshDlgManager);
	if (m_vt_Manipulator.size() == idx)
	{
		m_vt_Manipulator.push_back(manipulator);
	}
	else
	{
		m_vt_Manipulator.insert(m_vt_Manipulator.begin() + idx, manipulator);
	}

	if (m_pMeshModelViewManager != nullptr)
	{
		m_pMeshModelViewManager->UpdatePivotPoint(idx);
	}

	return true;
}

/*
@brief
@return
*/
bool CMeshManipulator::IsValidate()
{
	if (m_pDataContext == nullptr)
	{
		return false;
	}

	int manipulatorCount = m_vt_Manipulator.size();
	int meshCount = m_pDataContext->m_MeshData.GetMeshCount();

	if ((manipulatorCount == 0) || (meshCount != manipulatorCount))
	{
		return false;
	}

	return true;
}

/*
@brief
@return
*/
void 	CMeshManipulator::setTransformMat(
	int  _idx,
	mip::TRANSFORM& _mat
)
{
	if (IsValidate())
	{
		m_vt_Manipulator[_idx].setTransformMat(_mat);
	}
}

/*
@brief
@return
*/
mip::TRANSFORM	CMeshManipulator::getTransformMat(int _idx)
{
	mip::TRANSFORM trans;

	if (IsValidate())
	{
		trans = m_vt_Manipulator[_idx].getTransformMat();
	}

	return trans;
}

/*
@brief
@return
*/
mip::MATRIX44	CMeshManipulator::getTransformMatAxis(
	int _idx_mesh,
	int _idx_axis
)
{
	mip::MATRIX44 mat;

	if (IsValidate())
	{
		mat = m_vt_Manipulator[_idx_mesh].getTransformMatAxis(_idx_axis);
	}

	return mat;
}

/*
@brief
@return
*/
void CMeshManipulator::setPrevTransformMat(
	int  	_idx,
	mip::TRANSFORM& _mat
)
{
	if (IsValidate())
	{
		m_vt_Manipulator[_idx].setPrevTransformMat(_mat);
	}
}

/*
@brief
@return
*/
mip::TRANSFORM CMeshManipulator::getPrevTransformMat(int _idx)
{
	//mip::MATRIX44 mat;
	//if (CheckValiidate())
	//{
	//	mat = m_vt_Manipulator[_idx].getPrevTransformMat();
	//}

	//return mat;

	if (IsValidate())
	{
		return m_vt_Manipulator[_idx].getPrevTransformMat();
	}

	return mip::TRANSFORM();
}

/*
@brief
@return
*/
mip::MATRIX44 CMeshManipulator::getPrevTransformMatAxis(
	int _idx_mesh,
	int _idx_axis
)
{
	mip::MATRIX44 mat;

	if (IsValidate())
	{
		mat = m_vt_Manipulator[_idx_mesh].getPrevTransformMatAxis(_idx_axis);
	}

	return mat;
}

std::vector<CManipulator*> CMeshManipulator::GetManipulatorList()
{
	std::vector<CManipulator*> list;
	for (CManipulator& m : m_vt_Manipulator)
	{
		list.push_back(&m);
	}
	return list;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// End CMeshManipulator

