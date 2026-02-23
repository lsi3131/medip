#include "stdafx.h"
#include "CMeshHoleFillManager.h"
#include "CMeshModelViewManager.h"
#include "CMeshDlgManager.h"
#include "CMeshViewBtn3DScene.h"
#include "MeshControl.h"
#include "Main/MainMeshWidget.h"
#include "MEVolumeView.h"
#include "Renderer/Renderer.h"
#include "WindowManager.h"
#include "ResourceManager.h"
#include "MedipQT.h"
#include "ActionManager.h"
#include "DataContext.h"

CMeshHoleFillManager::CMeshHoleFillManager()
{
}

CMeshHoleFillManager::~CMeshHoleFillManager()
{

}

void CMeshHoleFillManager::Init(DataContext* pDataContext, MEVolumeView* pViewer)
{
	Init(pDataContext,
		pViewer,
		MESH_BTN_SCENE_MANAGER,
		MESH_MODELVIEW_MANAGER,
		MESH_DIALOG_MANAGER);
}

void CMeshHoleFillManager::Init(
	DataContext* pDataContext,
	MEVolumeView* pViewer,
	CMeshViewBtn3DScene* pBtn3DScene,
	CMeshModelViewManager* pModelViewManager,
	CMeshDlgManager* pDlgManager)
{
	m_pDataContext = pDataContext;
	m_pBtn3DScene = pBtn3DScene;
	m_pModelViewManager = pModelViewManager;
	m_pDlgManager = pDlgManager;

	m_Act_HoleFill = new QAction(QString("Fill hole mode ON"), pViewer);
	m_Act_HoleFill->setCheckable(true);
	m_Act_HoleFill->setChecked(false);

	connect(m_Act_HoleFill, &QAction::triggered, m_pBtn3DScene, &CMeshViewBtn3DScene::OnFillHoleSelected);

	m_PickMeshIdx = -1;
}


/*
@brief
@return
*/
CMeshHoleFillManager* CMeshHoleFillManager::getInstance()
{
	static CMeshHoleFillManager instance;
	return &instance;
}

/*
@brief
@return
*/
void CMeshHoleFillManager::Process(MESH_WORK_MODE _work)
{
	if (ACTION_MANAGER->isActionFinished() && m_pDataContext)
	{
		auto id = m_pDataContext->m_MeshData.GetCurrentMeshIndex();

		m_PickMeshIdx = id;

		switch (_work)
		{
		case MESH_WORK_FIND_HOLE:
		{
			FindHole();
		}
		break;
		case MESH_WORK_HOLE_FILL_SELECTED:
		{
			SelectedHoleFill();
		}
		break;
		}
	}
}

/*
@brief
@return
*/
void CMeshHoleFillManager::Update(MESH_WORK_MODE _mode)
{
	int sz_icon = WIN_MANAGER->mainWindow->IconSize;

	static QIcon fill_hole_icon[] = { 
		RESOURCE_MANAGER->getIcon(ICON_NON_SCULPT_OP_HOLEFILL, sz_icon, sz_icon),
		RESOURCE_MANAGER->getIcon(ICON_SCULPT_OP_HOLEFILL, sz_icon, sz_icon) };

	QPushButton* btn_fill_hole = m_pBtn3DScene->m_BtnFillHole;

	bool chkMode = _mode == MESH_WORK_HOLE_FILL_SELECTED;

	if (m_Act_HoleFill)
	{
		m_Act_HoleFill->setChecked(chkMode);
		btn_fill_hole->setIcon(fill_hole_icon[chkMode]);
		btn_fill_hole->setChecked(chkMode);
	}
	else
	{
		btn_fill_hole->setChecked(chkMode);
		btn_fill_hole->setIcon(fill_hole_icon[chkMode]);
		btn_fill_hole->setChecked(chkMode);
	}
}

/*
@brief
@return
*/
std::vector<std::pair<std::vector<int>, bool>>* CMeshHoleFillManager::getBoundaryIdxPtr()
{
	return &m_vt_boundary_pts_idx;
}

/*
@brief
@return
*/
void CMeshHoleFillManager::FindHole()
{
	//auto pckid = m_pModelViewManager->getMeshpckIDPtr();
	//auto mesh = WIN_MANAGER->volume_data.GetMesh(*pckid);

	//if (!mesh)
	//{
	//	return;
	//}

	//mip::mesh_control::MeshFindboundary(mesh, m_vt_boundary_pts, m_vt_boundary_pts_idx);

	//auto mesh = WIN_MANAGER->volume_data.GetCurrentMesh();
	auto mesh = m_pDataContext->m_MeshData.GetMesh(m_PickMeshIdx);
	ACTION_MANAGER->action_Mesh_FindHole(mesh, m_vt_boundary_pts, m_vt_boundary_pts_idx);
}

/*
@brief
@return
*/
void CMeshHoleFillManager::SelectedHoleFill()
{
	if (ACTION_MANAGER->isActionFinished())
	{
		if (m_PickMeshIdx == -1)
		{
			return;
		}

		//auto pckid = m_pModelViewManager->getMeshpckIDPtr();
		//auto mesh = WIN_MANAGER->volume_data.GetMesh(*pckid);
		auto mesh = m_pDataContext->m_MeshData.GetMesh(m_PickMeshIdx);

		if (!mesh || m_Current_HoleIdx == -1)
		{
			return;
		}

		auto dlg = static_cast<CHoleFillingDlg*>(m_pDlgManager->getMeshDialog(MESH_DIALOG_FILLHOLE));

		if (dlg->isCheckedUse())
		{
			//ACTION_MANAGER->action_UndoRedo_update(MESH_WORK_HOLE_FILL_SELECTED);

			//mip::mesh_control::FillHoleSelected(mesh, m_Current_HoleIdx, dlg->isCheckedRemesh(), dlg->isCheckedSmooth());

			//mesh->updateVertex();

			//WIN_MANAGER->buildRenderBufferTopology(mesh);

			//FindHole();

			//auto view = WIN_MANAGER->mainMeshWidget->getMainView();
			//view->updateGeometryCount(mesh);

			if (m_vt_boundary_pts.size() > 0)
			{

				ACTION_MANAGER->action_Mesh_FillSelectedHole(
					m_pDataContext,
					mesh,
					m_Current_HoleIdx,
					dlg->isCheckedRemesh(),
					dlg->isCheckedSmooth(),
					m_vt_boundary_pts,
					m_vt_boundary_pts_idx
				);
			}
		}
	}
}

void CMeshHoleFillManager::FillHoleAll()
{
	if (ACTION_MANAGER->isActionFinished())
	{
		if (m_PickMeshIdx == -1)
		{
			return;
		}

		auto mesh = m_pDataContext->m_MeshData.GetMesh(m_PickMeshIdx);

		if (!mesh)
		{
			return;
		}

		auto dlg = static_cast<CHoleFillingDlg*>(m_pDlgManager->getMeshDialog(MESH_DIALOG_FILLHOLE));

		if (dlg->isCheckedUse())
		{
			if (m_vt_boundary_pts.size() > 0)
			{
				ACTION_MANAGER->action_Mesh_FillSelectedHole(
					m_pDataContext,
					mesh,
					INT_MIN,
					dlg->isCheckedRemesh(),
					dlg->isCheckedSmooth(),
					m_vt_boundary_pts,
					m_vt_boundary_pts_idx,
					true
				);
			}
		}
	}
}

/*
@brief
@return
*/
void	CMeshHoleFillManager::CalcNearestBoundary(QPoint _pt)
{
	if (ACTION_MANAGER->isActionFinished())
	{
		std::vector<int> vt_idx;

		ContainsPoint(_pt, vt_idx);
	}
}

/*
@brief
@return
*/
bool	CMeshHoleFillManager::ContainsPoint(
	QPoint _pt,
	std::vector<int>& _vt_idx

)
{
	if (m_PickMeshIdx == -1)
	{
		return false;
	}

	// Check mouse point 
	mip::VECTOR3 pt, pt2d;

	auto camera = m_pModelViewManager->GetCameraPtr();

	//auto world_mat = g_Renderer->getWorld();
	auto	p_mesh = m_pDataContext->m_MeshData.GetMesh(m_PickMeshIdx);

	mip::MATRIX44  mat_offset;
	mip::VECTOR3  offset_center(-m_pDataContext->volume_data.getSizeX() * 0.5f, -m_pDataContext->volume_data.getSizeY() * 0.5f, -m_pDataContext->volume_data.getSizeZ() * 0.5f);

	mat_offset.identity();
	mat_offset.translation(offset_center);

	mip::MATRIX44 world_mat = p_mesh->getMatrix() * mat_offset * m_pModelViewManager->GetMainTransform();

	auto view_mat = camera->getView();
	auto proj_mat = camera->getProj();

	auto sz_screen = camera->getScreenSize();

	QPoint	v1;

	bool	b_contain = false;

	float	dist;
	float	min_dist = FLT_MAX;
	int min_idx = INT_MAX;

	m_Current_HoleIdx = -1;

	if (p_mesh)
	{
		for (int i = 0; i < m_vt_boundary_pts_idx.size(); ++i)
		{
			QPolygon polygon;

			m_vt_boundary_pts_idx[i].second = false;

			//if (!m_vt_boundary_idx[i].second)
			{
				for (int j = 0; j < m_vt_boundary_pts_idx[i].first.size(); ++j)
				{
					int& vi = m_vt_boundary_pts_idx[i].first[j];

					if (vi >= p_mesh->m_tverts.size())
					{
						continue;
					}

					auto& pos = p_mesh->m_tverts[vi].pos;

					pt.x = pos.x;
					pt.y = pos.y;
					pt.z = pos.z;

					pt2d = mip::geom::WorldToScreen(pt, sz_screen.x, sz_screen.y, view_mat, proj_mat, &world_mat);

					v1.setX(pt2d.x);
					v1.setY(pt2d.y);

					polygon.push_back(v1);

					dist = std::sqrt(std::pow(pt2d.x - _pt.x(), 2) + std::pow(pt2d.y - _pt.y(), 2));

					if (min_dist > dist)
					{
						min_dist = dist;
						min_idx = i;
					}
				}

				b_contain = (polygon.containsPoint(_pt, Qt::FillRule::OddEvenFill)) ? true : false;

				if (b_contain)
				{
					b_contain = true;

					_vt_idx.push_back(i);
				}
			}
		}
	}

	bool b_success = (_vt_idx.size() > 0) && (min_idx != INT_MAX);

	if (b_success)
	{
		m_Current_HoleIdx = min_idx;

		m_vt_boundary_pts_idx[m_Current_HoleIdx].second = true;
	}

	return b_contain;
}

/*
@brief
@return
*/
void	CMeshHoleFillManager::drawHoleLine(QPainter* _p_paint)
{
	if (ACTION_MANAGER->isActionFinished())
	{
		if (m_PickMeshIdx == -1)
		{
			return;
		}

		mip::MATRIX44  mat_offset;
		mip::VECTOR3  offset_center(-m_pDataContext->volume_data.getSizeX() * 0.5f, -m_pDataContext->volume_data.getSizeY() * 0.5f, -m_pDataContext->volume_data.getSizeZ() * 0.5f);

		mat_offset.identity();
		mat_offset.translation(offset_center);

		auto	p_mesh = m_pDataContext->m_MeshData.GetMesh(m_PickMeshIdx);

		auto camera = m_pModelViewManager->GetCameraPtr();
		auto sz_screen = camera->getScreenSize();
		auto matView = camera->getView();
		auto matProj = camera->getProj();
		//auto world_mat = g_Renderer->getWorld();
		mip::MATRIX44 world_mat = p_mesh->getMatrix() * mat_offset * m_pModelViewManager->GetMainTransform();

		if (p_mesh)
		{
			mip::VECTOR3 pt, pt2d;
			QPoint  v1;
			for (int i = 0; i < m_vt_boundary_pts_idx.size(); ++i)
			{
				if (m_vt_boundary_pts_idx[i].first.empty())
				{
					continue;
				}

				QPolygon polygon;

				polygon.reserve(m_vt_boundary_pts_idx[i].first.size());

				for (int j = 0; j < m_vt_boundary_pts_idx[i].first.size(); ++j)
				{
					int& vi = m_vt_boundary_pts_idx[i].first[j];

					if (vi >= p_mesh->m_tverts.size())
					{
						continue;
					}

					auto& pos = p_mesh->m_tverts[vi].pos;

					pt.x = pos.x;
					pt.y = pos.y;
					pt.z = pos.z;

					pt2d = mip::geom::WorldToScreen(pt, sz_screen.x, sz_screen.y, matView, matProj, &world_mat);

					v1.setX(pt2d.x);
					v1.setY(pt2d.y);

					polygon.push_back(v1);
				}

				if (polygon.size() < 3)
				{
					continue;
				}

				if (m_vt_boundary_pts_idx[i].second)
				{
					_p_paint->setPen(QPen(Qt::blue, 2));
				}
				else
				{
					_p_paint->setPen(QPen(Qt::green, 2));
				}

				for (int i = 0; i < polygon.size() - 1; ++i)
				{
					_p_paint->drawLine(polygon.at(i), polygon.at(i + 1));
				}

				_p_paint->drawLine(polygon.at(polygon.size() - 1), polygon.at(0));
			}
		}
	}
}