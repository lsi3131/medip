#include "stdafx.h"
#include "ActionMesh.h"

#include "MedipQT.h"

#include "MeshControl.h"
#include "Windows/windowManager.h"
#include "Windows/Main/MainMeshWidget.h"
#include "Windows/MEVolumeView.h"
#include "Windows/Tabwindow.h"
#include "Windows/Tab/MeshTab.h"

#include "graphics/volumedata.h"
#include "System/stringManager.h"
#include "MeshEdit/CMeshCutManager.h"
#include "MeshEdit/CMeshViewRenderManager.h"
#include "MeshEdit/CMeshWorkManager.h"
#include "MeshEdit/CManipulator.h"
#include "MeshEdit/CMeshManipulator.h"
#include "MeshEdit/CMeshModelViewManager.h"
#include "MeshEdit/CMeshDlgManager.h"
#include "MeshEdit/CMeshHoleFillManager.h"

#include <ppl.h>
#include <thread>

#include <vtkImageGridSource.h>
#include <vtkPolyDataToImageStencil.h>
#include <vtkImageStencil.h>
#include <vtkImageCast.h>

void* WorkMeshBoolean::_dt = nullptr;


// 1. smooth 
// 6-a. selectionSmooth
// 6-b. selectionPull 
// 6-c. selectionMoveStart / selectionMoveEnd / selectionMoveUpdate
// 6-d. beginBrush / endBrush / clearBrush / getVertexBuffer / updateVertexBuffer / getMeshDatas

WorkMeshPlanecut::WorkMeshPlanecut(
	DataContext* pDataContext,
	MESH_WORK_MODE _mode,
	MESH_WORK_MODE _prev_mode,
	mip::MeshTopology* _plane_mesh,
	mip::MATRIX44& _plane_mat,
	std::vector<mip::MeshTopology*>& _vt_mesh,
	std::vector<std::vector<mip::VECTOR3>>* _vt_pt_holes,
	bool _b_fill_hole,
	bool _b_remesh,
	bool _b_smooth,
	bool _b_zaxis_inverse
)
{
	m_vt_mesh = _vt_mesh;
	m_PlaneMesh = _plane_mesh;
	m_PlaneMat = _plane_mat;
	m_p_vt_pt_holes = _vt_pt_holes;

	m_b_remesh = _b_remesh;
	m_b_smooth = _b_smooth;
	m_b_fill_hole = _b_fill_hole;
	m_b_InverseZAxis = _b_zaxis_inverse;

	m_mode = _mode;
	m_prev_mode = _prev_mode;

	m_pDataContext = pDataContext;
}

void WorkMeshPlanecut::threadRun()
{
	if (!m_pDataContext)
	{
		emit progress(100);

		emit finished();

		return;
	}

	int progreess_val = 5;

	emit progress(progreess_val);

	ACTION_MANAGER->action_UndoRedo_update(m_pDataContext, m_mode, m_prev_mode);

	progreess_val += 20;

	emit progress(progreess_val);

	std::vector<std::vector<mip::VECTOR3>>	vt_pt_holes;

	int	interval = 70 / m_vt_mesh.size();

	for (int i = 0; i < m_vt_mesh.size(); ++i)
	{
		if (!m_vt_mesh[i])
		{
			progreess_val += interval;

			emit progress(progreess_val);

			continue;
		}

		mip::PLANE plane;

		std::vector<mip::VECTOR3>  verts = m_PlaneMesh->m_verts;

		mip::MATRIX44 mesh_mat = m_vt_mesh[i]->getMatrix();

		mip::MATRIX44 trans_mat = m_PlaneMat * mesh_mat.inverse();

		for (int vi = 0; vi < verts.size(); ++vi)
		{
			verts[vi] = verts[vi].transform(trans_mat);
		}

		if (m_b_InverseZAxis)
		{
			plane = mip::PLANE(verts[0], verts[2], verts[1]);
		}
		else
		{
			plane = mip::PLANE(verts[1], verts[2], verts[0]);
		}

		vt_pt_holes = mip::mesh_control::MeshPlaneCut(
			&plane,
			m_vt_mesh[i],
			m_b_fill_hole,
			m_b_remesh,
			m_b_smooth
		);

		progreess_val += interval / 2;

		mip::mesh_control::MeshCutOK(m_vt_mesh[i]);

		m_p_vt_pt_holes->swap(vt_pt_holes);

		m_vt_mesh[i]->updateVertex();
		m_vt_mesh[i]->updateColor(m_vt_mesh[i]->m_baseColor);

		emit sig_buildRenderBufferTopology(m_vt_mesh[i]);

		progreess_val += interval / 2;

		emit progress(progreess_val);
	}

	emit progress(progreess_val);

	emit finished();
}

WorkMeshPolycut::WorkMeshPolycut(
	DataContext* pDataContext,
	MESH_WORK_MODE _mode,
	MESH_WORK_MODE _prev_mode,
	std::vector<mip::VECTOR2>& _vt_polygons
)
{
	m_pDataContext = pDataContext;
	m_mode = _mode;
	m_prev_mode = _prev_mode;
	m_vt_polygons = _vt_polygons;
}

void WorkMeshPolycut::threadRun()
{
	if (!m_pDataContext)
	{
		emit progress(100);
		emit finished();

		return;
	}

	emit progress(0);

	int n_mesh = m_pDataContext->m_MeshData.GetMeshCount();

	auto  p_camera = MESH_MODELVIEW_MANAGER->GetCameraPtr();
	auto  matView = p_camera->getView();
	auto  matProj = p_camera->getProj();
	auto  sz_screen = p_camera->getScreenSize();
	auto  mat_view_world = MESH_MODELVIEW_MANAGER->GetMainTransform();

	mip::MATRIX44				mat_offset;
	mip::VECTOR3				offset_center(-m_pDataContext->volume_data.getSizeX() * 0.5f, -m_pDataContext->volume_data.getSizeY() * 0.5f, -m_pDataContext->volume_data.getSizeZ() * 0.5f);

	mat_offset.identity();
	mat_offset.translation(offset_center);

	std::vector<int> vt_pick_id;
	vt_pick_id.reserve(n_mesh);
	for (int i = 0; i < n_mesh; ++i)
	{
		//auto  pck_id = WIN_MANAGER->vt_pckID[i];
		MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(i);

		//if (pck_id && (m_mode >= MESH_WORK_FOR_SINGLE))
		if (pMeshInfo && pMeshInfo->selected && (m_mode >= MESH_WORK_FOR_SINGLE))
		{
			vt_pick_id.push_back(i);
		}
	}

	emit progress(10);

	MESH_WORK_MODE prev_mode = m_mode;

	if ((m_prev_mode == MESH_WORK_POLYGON_CUT)
		|| (m_prev_mode == MESH_WORK_POLYLINE_CUT)
		|| (m_prev_mode == MESH_WORK_FREEPOLYLINE_CUT)
		)
	{
		prev_mode = m_prev_mode;
	}

	ACTION_MANAGER->action_UndoRedo_update(m_pDataContext, m_mode, prev_mode);

	emit progress(50);

	int interval = 50 / vt_pick_id.size();
	for (int i = 0; i < vt_pick_id.size(); ++i)
	{
		auto  pck_id = vt_pick_id[i];

		m_p_mesh = m_pDataContext->m_MeshData.GetMesh(pck_id);

		if (nullptr == m_p_mesh)
		{
			continue;
		}

		auto  matWorld = m_p_mesh->getMatrix();

		matWorld *= mat_offset;
		matWorld *= mat_view_world;


		switch (m_mode)
		{
		case MESH_WORK_POLYGON_CUT:
			mip::mesh_control::MeshPolyCutInner(
				m_vt_polygons,
				matWorld,
				matView,
				matProj,
				sz_screen,
				m_p_mesh
			);
			break;
		case MESH_WORK_POLYLINE_CUT:
			mip::mesh_control::MeshPolyCutOut(
				m_vt_polygons,
				matWorld,
				matView,
				matProj,
				sz_screen,
				m_p_mesh
			);
			break;
		case MESH_WORK_FREEPOLYLINE_CUT:
			mip::mesh_control::MeshFreePolyCut(
				m_vt_polygons,
				matWorld,
				matView,
				matProj,
				sz_screen,
				m_p_mesh
			);
			break;
		}

		m_p_mesh->updateVertex();
		m_p_mesh->updateColor(m_p_mesh->m_baseColor);

		emit sig_buildRenderBufferTopology(m_p_mesh);
		emit sig_renderLater();

		emit progress(50 + interval * i);
	}

	emit progress(100);
	emit finished();
}

WorkMeshHoleDetect::WorkMeshHoleDetect(
	mip::MeshTopology* _p_mesh,
	std::vector<std::vector<mip::VECTOR3>>& _vt_boundary_pts,
	std::vector<std::pair<std::vector<int>, bool>>& _vt_boundary_pts_idx
)
{
	m_p_mesh = _p_mesh;
	m_vt_boundary_pts = &_vt_boundary_pts;
	m_vt_boundary_pts_idx = &_vt_boundary_pts_idx;
}

void WorkMeshHoleDetect::threadRun()
{
	emit progress(10);

	if (m_p_mesh)
	{
		mip::mesh_control::MeshFindboundary(m_p_mesh, *m_vt_boundary_pts, *m_vt_boundary_pts_idx);

		emit progress(100);
	}

	emit finished();
}

WorkMeshFillSeltedHole::WorkMeshFillSeltedHole(
	DataContext* pDataContext,
	mip::MeshTopology* _p_mesh,
	int _hole_idx,
	bool _b_remesh,
	bool _b_smooth,
	std::vector<std::vector<mip::VECTOR3>>& _vt_boundary_pts,
	std::vector<std::pair<std::vector<int>, bool>>& _vt_boundary_pts_idx,
	bool _b_FillHoleAll
)
{
	m_p_mesh = _p_mesh;
	m_hole_idx = _hole_idx;
	m_b_remesh = _b_remesh;
	m_b_smooth = _b_smooth;

	m_vt_boundary_pts = &_vt_boundary_pts;
	m_vt_boundary_pts_idx = &_vt_boundary_pts_idx;

	m_b_FillHoleAll = _b_FillHoleAll;

	m_pDataContext = pDataContext;
}

void WorkMeshFillSeltedHole::selectedHoleFill()
{
	mip::mesh_control::FillHoleSelected(m_p_mesh, m_hole_idx, m_b_remesh, m_b_smooth);

	emit progress(60);
}

void WorkMeshFillSeltedHole::HoleFillAll()
{
	mip::mesh_control::FillHoleAll(m_p_mesh);
	emit progress(60);
}

void WorkMeshFillSeltedHole::threadRun()
{
	emit progress(5);

	if (m_pDataContext && m_p_mesh)
	{
		ACTION_MANAGER->action_UndoRedo_update(m_pDataContext, MESH_WORK_HOLE_FILL_SELECTED, MESH_WORK_HOLE_FILL_SELECTED);

		emit progress(30);

		if (m_b_FillHoleAll)
		{
			HoleFillAll();
		}
		else
		{
			selectedHoleFill();
		}

		m_p_mesh->updateVertex();
		m_p_mesh->updateColor(m_p_mesh->m_baseColor);

		emit progress(70);

		auto hole_detector = new WorkMeshHoleDetect(m_p_mesh, *m_vt_boundary_pts, *m_vt_boundary_pts_idx);
		hole_detector->threadRun();

		emit progress(90);

		emit sig_buildRenderBufferTopology(m_p_mesh);

		emit progress(100);

		emit sig_renderLater();
	}

	if (WIN_MANAGER->mainMeshWidget != nullptr && WIN_MANAGER->mainMeshWidget->getMainView() != nullptr)
		WIN_MANAGER->mainMeshWidget->getMainView()->updateGeometryCount();

	emit finished();
}

WorkMeshBoolean::WorkMeshBoolean(DataContext* pDataContext, MESH_WORK_MODE type, mip::MeshTopology* res, muint8 fUID, muint8 sUID)
{
	m_pDataContext = pDataContext;

	this->type = type;
	_mesh = res;
	this->fUID = fUID;
	this->sUID = sUID;
	_dt = (void*)this;
}

void WorkMeshBoolean::updateProgress(float val, void* dt)
{
	WorkMeshBoolean* worker = (WorkMeshBoolean*)_dt;
	worker->setProgressValue(val);
}

void WorkMeshBoolean::setProgressValue(int value, bool init)
{
	static int val = 0;

	if (init)
	{
		val = 0;
		emit progress(val);
		return;
	}

	if (100 <= val)
		return;

	if ((val + 10) <= value)
	{
		val = value;
		emit progress(val);
	}
}

void WorkMeshBoolean::threadRun()
{
	if (!m_pDataContext)
	{
		setProgressValue(100);

		emit finished();

		return;
	}

	setProgressValue(0, true);
	//#ifdef MESH_TEST
	//	mip::MeshTopology *_meshD = m_pDataContext->m_MeshData.GetMesh(sUID);
	//	m_pDataContext->m_MeshData.threadResult = mip::mesh_control::booleanMesh(type, _mesh->m_verts, _mesh->m_tris, _mesh->m_normals,
	//		_mesh->getMatrix(), _meshD->m_verts, _meshD->m_tris, _meshD->m_normals, _meshD->getMatrix(), false, 1e-6, updateProgress);
	//	setProgressValue(100);
	//#endif

	mip::MeshTopology* mesh1 = m_pDataContext->m_MeshData.GetMesh(fUID);
	mip::MeshTopology* mesh2 = m_pDataContext->m_MeshData.GetMesh(sUID);

	if (mesh1 && mesh2)
	{

		mip::MeshTopology* copy_mesh1 = new mip::MeshTopology(g_Renderer);
		mip::MeshTopology* copy_mesh2 = new mip::MeshTopology(g_Renderer);

		// adapt manipulator value
		concurrency::parallel_for(0, 2, [&](int i)
			{
				if (i == 0)
				{
					copy_mesh1->m_tverts.resize(mesh1->m_tverts.size());
					std::copy(mesh1->m_tverts.begin(), mesh1->m_tverts.end(), copy_mesh1->m_tverts.begin());

					copy_mesh1->m_ttris.resize(mesh1->m_ttris.size());
					std::copy(mesh1->m_ttris.begin(), mesh1->m_ttris.end(), copy_mesh1->m_ttris.begin());

					auto trans_f = mesh1->getMatrix();

					auto trans = trans_f.getOrigin();
					auto q = trans_f.getQuaternion();

					auto check1 = (q.x == 0.f && q.y == 0 && q.z == 0.f && q.w == 1.f);
					auto check2 = (trans.x == 0.f && trans.y == 0.f && trans.z == 0.f);

					if (!check1 || !check2)
					{
						int n_verts = copy_mesh1->m_tverts.size();
						for (int it = 0; it < n_verts; ++it)
						{
							auto& pos = copy_mesh1->m_tverts[it].pos;

							pos = mip::math::VectorTransform(pos, trans_f);
						}
					}
				}
				else
				{
					copy_mesh2->m_tverts.resize(mesh2->m_tverts.size());
					std::copy(mesh2->m_tverts.begin(), mesh2->m_tverts.end(), copy_mesh2->m_tverts.begin());

					copy_mesh2->m_ttris.resize(mesh2->m_ttris.size());
					std::copy(mesh2->m_ttris.begin(), mesh2->m_ttris.end(), copy_mesh2->m_ttris.begin());

					auto trans_s = mesh2->getMatrix();

					auto trans = trans_s.getOrigin();
					auto q = trans_s.getQuaternion();

					auto check1 = (q.x == 0.f && q.y == 0 && q.z == 0.f && q.w == 1.f);
					auto check2 = (trans.x == 0.f && trans.y == 0.f && trans.z == 0.f);

					if (!check1 || !check2)
					{
						int n_verts = copy_mesh2->m_tverts.size();
						for (int it = 0; it < n_verts; ++it)
						{
							auto& pos = copy_mesh2->m_tverts[it].pos;

							pos = mip::math::VectorTransform(pos, trans_s);
						}
					}
				}
			});

		mip::MeshTopology* res_mesh = new mip::MeshTopology(g_Renderer);

		mip::mesh_control::boolean(copy_mesh1, copy_mesh2, res_mesh, type, g_Renderer, updateProgress, this);

		res_mesh->buildTree();

		// Update Result
		{
			QString		name;

			switch (type)
			{
			case MESH_WORK_INTERSECT:
				name = "boolean_result_inter";
				break;
			case MESH_WORK_DIFF:
				name = "boolean_result_diff";
				break;
			case MESH_WORK_UNION:
				name = "boolean_result_union";
				break;

			}

			m_pDataContext->m_MeshData.CreateMeshInfo();

			muint32		uid = m_pDataContext->m_MeshData.GetCurrentMeshIndex();

			m_pDataContext->m_MeshData.SetMeshName(name, uid);

			MeshInfo* info = m_pDataContext->m_MeshData.GetMeshInfo(uid);

			if (info)
			{
				info->upScale = false;

				info->uid = uid;
			}

			// push pck true 
			//WIN_MANAGER->vt_pckID.push_back(true);
			m_pDataContext->m_MeshData.SetMeshInfoModeSelectMode(uid, true);

			// insert mesh
			m_pDataContext->m_MeshData.InsertMesh(uid, res_mesh);

			// update mesh-list
			emit sig_updateUI();

			WIN_MANAGER->setSaveState(false);

			// color 변경
			auto mesh = m_pDataContext->m_MeshData.GetMesh(uid);
			auto mesh_info = m_pDataContext->m_MeshData.GetMeshInfo(uid);
			mesh->m_baseColor = mip::VECTOR4(mesh_info->color.r / 255.f, mesh_info->color.g / 255.f, mesh_info->color.b / 255.f, 255.f);
			mesh->updateColor(mesh->m_baseColor);

			m_pDataContext->m_MeshData.MeshRenderUpdate(uid);
			//210510 허 건대리 Undo / Redo
			ACTION_MANAGER->action_MeshList_add(m_pDataContext, -1, name, mesh);
		}

		m_pDataContext->volume_data.threadResult = 1;
	}

	setProgressValue(100);

	emit finished();
}

ActionAddMesh::ActionAddMesh(DataContext* pDataContext, mint32 layerUID, QString strName,
	mip::MeshTopology* m, bool upScale, QUndoCommand* parent)
	:QUndoCommand(parent),
	m_layerUID(layerUID),
	m_strName(strName),
	m_mesh(m),
	m_upScale(upScale),
	m_first(true),
	m_pDataContext(pDataContext)
{
	static int s_id = ACT_ID_MESHLIST_ADD;

	for (int i = 0; i < 3; i++)
		m_Points[i] = -1;

	m_id = s_id++;
}

ActionAddMesh::~ActionAddMesh()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString str = QString().sprintf("%d", m_id);

		deleteUndoRedoFile(str, &dir);
	}
}

void ActionAddMesh::undo()
{
	MESH_DIALOG_MANAGER->rejectDialog();

	if (!m_pDataContext)
	{
		return;
	}

	if (m_layerUID != -1)
		m_pDataContext->m_MeshData.SetDisconnectMesh(m_UID);

	m_pDataContext->m_MeshData.DeleteMeshLayer(m_UID);

	MeshInfo* info = nullptr;
	for (int i = m_UID; i < m_pDataContext->m_MeshData.GetMeshCount(); i++)
	{
		info = m_pDataContext->m_MeshData.GetMeshInfo(i);

		if (info)
		{
			//layeruid, mesh map move
			m_pDataContext->m_MeshData.ChangeMeshUID(i, i - 1, info->uid);
		}
	}

	//WIN_MANAGER->vt_pckID.pop_back();

	WIN_MANAGER->updateMeshUI();
	WIN_MANAGER->renderLater_All();
	WIN_MANAGER->setSaveState(false);
}

void ActionAddMesh::redo()
{
	MESH_DIALOG_MANAGER->rejectDialog();

	if (m_first)
	{
		m_pDataContext->m_MeshData.CreateMeshInfo();
		m_UID = m_pDataContext->m_MeshData.GetCurrentMeshIndex();
		m_pDataContext->m_MeshData.SetMeshName(m_strName, m_UID);

		MeshInfo* info = m_pDataContext->m_MeshData.GetMeshInfo(m_UID);
		if (m_layerUID != -1)
		{
			QColor col = m_pDataContext->volume_data.getMaskColor(m_layerUID, true);

			info->color = COLOR(col.red(), col.green(), col.blue());
		}

		if (info)
		{
			info->upScale = m_upScale;
			m_info = (*info);
		}
	}
	else
	{
		m_pDataContext->m_MeshData.AddMeshInfo(m_UID, m_info);
	}

	if (!m_first)
	{
		MeshInfo* info = nullptr;
		for (int i = m_pDataContext->m_MeshData.GetMeshCount() - 1; i >= m_UID; i--)
		{
			info = m_pDataContext->m_MeshData.GetMeshInfo(i);

			if (info)
			{
				//layeruid, mesh map move
				m_pDataContext->m_MeshData.ChangeMeshUID(i, i + 1, info->uid);

				info->m_nReduceLevel = 0;
				info->m_nSmoothLevel = 0;
			}

		}
	}

	if (m_first)
		m_first = false;

	// 200901 허건 대리
	//WIN_MANAGER->vt_pckID.push_back(true);
	m_pDataContext->m_MeshData.SetMeshInfoModeSelectMode(m_UID, true);

	QDir dir(STRING_MANAGER->cacheFilePath);

	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);

		QFile file(filename);

		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			m_mesh = new mip::MeshTopology(g_Renderer);

			if (m_mesh)
			{
				// 201015 허 건 대리
				{
					loadDataAllRedoFile(filename, m_mesh);

					m_pDataContext->m_MeshData.InsertMeshToMeshMap(m_UID, m_mesh);

					WIN_MANAGER->updateMeshUI(false, m_UID);

					PLANE_MANIPULATOR->UpdatePosition(m_mesh);
				}
			}
		}
		else if (file.open(QIODevice::WriteOnly))
		{
			if (m_mesh)
			{
				m_pDataContext->m_MeshData.InsertMesh(m_UID, m_mesh);
			}

			// 201015 허 건 대리
			saveDataAllRedoFile(filename, m_mesh);
		}
	}

	// 201023 허 건 대리
	// mesh id가 roi에 종속되지 않도록 수정
	MeshInfo* info = m_pDataContext->m_MeshData.GetMeshInfo(m_UID);

	if (info)
	{
		info->uid = m_UID;
	}

	//WIN_MANAGER->updateMeshUI(false, m_UID);
	WIN_MANAGER->renderLater_All();
	WIN_MANAGER->setSaveState(false);
}


///////////////////////////////////////////////////////////////////////////////////////
// ActionAddMeshVisualPrint Class Member Functions - Start
///////////////////////////////////////////////////////////////////////////////////////
ActionAddMeshVisualPrint::ActionAddMeshVisualPrint(DataContext* pDataContext, mint32 layerUID, QString strName,
	mip::MeshTopology* m, bool upScale, QUndoCommand* parent)
	:QUndoCommand(parent),
	m_layerUID(layerUID),
	m_strName(strName),
	m_mesh(m),
	m_upScale(upScale),
	m_first(true),
	m_pDataContext(pDataContext)
{
	static int s_id = ACT_ID_MESHLIST_ADD;

	for (int i = 0; i < 3; i++)
		m_Points[i] = -1;

	m_id = s_id++;
}

ActionAddMeshVisualPrint::~ActionAddMeshVisualPrint()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString str = QString().sprintf("%d", m_id);

		deleteUndoRedoFile(str, &dir);
	}
}

void ActionAddMeshVisualPrint::undo()
{
	if (!m_pDataContext)
	{
		return;
	}

	if (m_layerUID != -1)
		m_pDataContext->m_MeshData.SetDisconnectMesh(m_UID);

	m_pDataContext->m_MeshData.DeleteMeshLayer(m_UID);

	MeshInfo* info = nullptr;
	for (int i = m_UID; i < m_pDataContext->m_MeshData.GetMeshCount(); i++)
	{
		info = m_pDataContext->m_MeshData.GetMeshInfo(i);

		if (info)
		{
			//layeruid, mesh map move
			m_pDataContext->m_MeshData.ChangeMeshUID(i, i - 1, info->uid);
		}
	}

	//WIN_MANAGER->updateMeshUI();
	WIN_MANAGER->renderLater_All();
	WIN_MANAGER->setSaveState(false);
}

void ActionAddMeshVisualPrint::redo()
{
	if (!m_pDataContext)
	{
		return;
	}

	if (m_first)
	{
		m_pDataContext->m_VisualPrinting_MeshData.CreateMeshInfo();
		m_UID = m_pDataContext->m_VisualPrinting_MeshData.GetCurrentMeshIndex();
		m_pDataContext->m_VisualPrinting_MeshData.SetMeshName(m_strName, m_UID);

		MeshInfo* info = m_pDataContext->m_VisualPrinting_MeshData.GetMeshInfo(m_UID);

		if (m_layerUID != -1)
		{
			QColor col = m_pDataContext->volume_data.getMaskColor(m_layerUID, true);

			info->color = COLOR(col.red(), col.green(), col.blue());
		}

		if (info)
		{
			info->upScale = m_upScale;
			m_info = (*info);
		}
	}
	else
	{
		m_pDataContext->m_VisualPrinting_MeshData.AddMeshInfo(m_UID, m_info);
	}

	if (!m_first)
	{
		MeshInfo* info = nullptr;
		for (int i = m_pDataContext->m_VisualPrinting_MeshData.GetMeshCount() - 1; i >= m_UID; i--)
		{
			info = m_pDataContext->m_VisualPrinting_MeshData.GetMeshInfo(i);

			if (info)
			{
				//layeruid, mesh map move
				m_pDataContext->m_VisualPrinting_MeshData.ChangeMeshUID(i, i + 1, info->uid);

				info->m_nReduceLevel = 0;
				info->m_nSmoothLevel = 0;
			}

		}
	}

	if (m_first)
		m_first = false;

	// 200901 허건 대리
	//WIN_MANAGER->vt_pckIDVisualPrint.push_back(true);
	m_pDataContext->m_VisualPrinting_MeshData.SetMeshInfoModeSelectMode(m_UID, true);

	QDir dir(STRING_MANAGER->cacheFilePath);

	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);

		QFile file(filename);

		if (m_Points[0] > 0 && file.exists() && file.open(QIODevice::ReadOnly))
		{
			m_mesh = new mip::MeshTopology(g_Renderer);

			if (m_mesh)
			{
				m_mesh->m_verts.reserve(m_Points[0]);
				m_mesh->m_tris.reserve(m_Points[1]);
				m_mesh->m_normals.reserve(m_Points[2]);

				m_mesh->m_verts.assign(m_Points[0], mip::VECTOR3());
				m_mesh->m_tris.assign(m_Points[1], 0);
				m_mesh->m_normals.assign(m_Points[2], mip::VECTOR3());

				mip::VECTOR3* dt = m_mesh->m_verts.data();
				muint32* dtTri = m_mesh->m_tris.data();
				mip::VECTOR3* dtNor = m_mesh->m_normals.data();

				file.read((char*)dt, sizeof(mip::VECTOR3) * m_Points[0]);
				file.read((char*)dtTri, sizeof(muint32) * m_Points[1]);
				file.read((char*)dtNor, sizeof(mip::VECTOR3) * m_Points[2]);

				file.close();
			}
		}
		else if (file.open(QIODevice::WriteOnly))
		{
			m_Points[0] = m_mesh->m_verts.size();
			m_Points[1] = m_mesh->m_tris.size();
			m_Points[2] = m_mesh->m_normals.size();

			mip::VECTOR3* dt = m_mesh->m_verts.data();
			muint32* dtTri = m_mesh->m_tris.data();
			mip::VECTOR3* dtNor = m_mesh->m_normals.data();

			file.write((char*)dt, sizeof(mip::VECTOR3) * m_Points[0]);
			file.write((char*)dtTri, sizeof(muint32) * m_Points[1]);
			file.write((char*)dtNor, sizeof(mip::VECTOR3) * m_Points[2]);

			file.close();
		}
		if (m_mesh)
		{
			m_pDataContext->m_VisualPrinting_MeshData.InsertMesh(m_UID, m_mesh);

			m_mesh = nullptr;
		}
	}

	if (m_layerUID != -1)
	{
		m_pDataContext->m_VisualPrinting_MeshData.SetConnectMesh(m_UID, m_layerUID);
	}

	WIN_MANAGER->renderLater_All();
	WIN_MANAGER->setSaveState(false);
}

///////////////////////////////////////////////////////////////////////////////////////
// ActionAddMeshVisualPrint Class Member Functions - End
///////////////////////////////////////////////////////////////////////////////////////

ActionDelMesh::ActionDelMesh(DataContext* pDataContext, muint8 mUID, mint32 lUID, QUndoCommand* parent)
	:QUndoCommand(parent),
	m_UID(mUID),
	m_layerUID(lUID),
	m_first(true),
	m_pDataContext(pDataContext)
{
	static int s_id = ACT_ID_MESHLIST_DEL;

	m_id = s_id++;

	m_WorkMode = MESH_WORK_MANAGER->getWorkMode();
}

ActionDelMesh::~ActionDelMesh()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString str = QString().sprintf("%d", m_id);

		deleteUndoRedoFile(str, &dir);
	}
}

void ActionDelMesh::undo()
{
	if (m_WorkMode != NONE)
	{
		MESH_DIALOG_MANAGER->makeMeshDialog(m_WorkMode, true);
	}
	else
	{
		MESH_DIALOG_MANAGER->rejectDialog();
	}

	if (!m_pDataContext)
	{
		return;
	}


	MeshInfo* info = nullptr;

	for (int i = m_pDataContext->m_MeshData.GetMeshCount() - 1; i >= m_UID; i--)
	{
		info = m_pDataContext->m_MeshData.GetMeshInfo(i);

		if (info)
		{
			//layeruid, mesh map move
			m_pDataContext->m_MeshData.ChangeMeshUID(i, i + 1, info->uid);
		}
	}

	if (m_info)
	{
		m_pDataContext->m_MeshData.AddMeshInfo(m_UID, *m_info);
	}

	//for (int idx = 0; idx < WIN_MANAGER->vt_pckID.size(); ++idx)
	//{
	//	WIN_MANAGER->vt_pckID[idx] = false;
	//}

	//WIN_MANAGER->vt_pckID.insert(WIN_MANAGER->vt_pckID.begin() + m_UID, true);

	m_pDataContext->m_MeshData.ClearMeshInfoSelectMode();
	m_pDataContext->m_MeshData.SetMeshInfoModeSelectMode(m_UID, true);

	QDir dir(STRING_MANAGER->cacheFilePath);
	mip::MeshTopology* m = nullptr;
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);

		QFile file(filename);

		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			m = new mip::MeshTopology(g_Renderer);

			if (m)
			{
				loadDataAllRedoFile(filename, m);

				m_pDataContext->m_MeshData.InsertMeshToMeshMap(m_UID, m);

				PLANE_MANIPULATOR->UpdatePosition(m);
			}
		}
	}
	else
	{
		if (m)
		{
			m_pDataContext->m_MeshData.InsertMesh(m_UID, m);
		}
	}

	if (m_layerUID != -1)
		m_pDataContext->m_MeshData.SetConnectMesh(m_UID, m_layerUID);

	WIN_MANAGER->updateMeshUI();
	WIN_MANAGER->renderLater_All();
	WIN_MANAGER->setSaveState(false);

}

void ActionDelMesh::redo()
{
	if (!m_pDataContext)
	{
		return;
	}

	if (m_first)
	{
		m_info = m_pDataContext->m_MeshData.GetMeshInfo(m_UID);
		m_first = false;

		QDir dir(STRING_MANAGER->cacheFilePath);

		if (dir.exists())
		{
			QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);

			QFile file(filename);

			if (!file.exists() && file.open(QIODevice::WriteOnly))
			{
				mip::MeshTopology* m = m_pDataContext->m_MeshData.GetMesh(m_UID);
				if (m)
				{
					saveDataAllRedoFile(filename, m);
				}
			}
		}
	}

	if (m_layerUID != -1)
		m_pDataContext->m_MeshData.SetDisconnectMesh(m_layerUID);

	m_pDataContext->m_MeshData.DeleteMeshLayer(m_UID);

	MeshInfo* info = nullptr;

	for (int i = m_UID; i < m_pDataContext->m_MeshData.GetMeshCount(); i++)
	{
		info = m_pDataContext->m_MeshData.GetMeshInfo(i);

		if (info)
		{
			//layeruid, mesh map move
			m_pDataContext->m_MeshData.ChangeMeshUID(i + 1, i, info->uid);
		}
	}

	int cur_idx = m_pDataContext->m_MeshData.GetCurrentMeshIndex();
	m_pDataContext->m_MeshData.ClearMeshInfoSelectMode();
	m_pDataContext->m_MeshData.SetMeshInfoModeSelectMode(cur_idx, true);

	MESH_MODELVIEW_MANAGER->UpdatePivotPoint(cur_idx);

	MESH_WORK_MANAGER->UpdateWorkMode(MESH_WORK_NONE, true);

	WIN_MANAGER->updateMeshUI();
	WIN_MANAGER->renderLater_All();
	WIN_MANAGER->setSaveState(false);
}

ActionDelMeshes::ActionDelMeshes(DataContext* pDataContext, std::vector<mint8> delUIDs, QUndoCommand* parent)
	:QUndoCommand(parent),
	m_first(true),
	m_pDataContext(pDataContext)
{
	static int s_id = ACT_ID_MESHLIST_DEL_LIST;

	std::sort(delUIDs.begin(), delUIDs.end(), std::less<mint8>());//오름차순(0,1,2,,,,)

	if (m_pDataContext)
	{
		m_BeforeCnt = m_pDataContext->m_MeshData.GetMeshCount();
		m_DelCnt = delUIDs.size();

		m_Points = new double[m_DelCnt];
		memset(m_Points, 0, sizeof(double) * m_DelCnt);
		m_layerUID = new mint32[m_BeforeCnt];
		memset(m_layerUID, 0, sizeof(mint32) * m_BeforeCnt);
		m_UID = new mint8[m_BeforeCnt];
		memset(m_UID, 0, sizeof(mint8) * m_BeforeCnt);
		m_info = new MeshInfo[m_DelCnt];
		memset(m_info, 0, sizeof(MeshInfo) * m_DelCnt);

		int index = 0;

		for (int i = 0, j = 0; i < m_BeforeCnt; i++)
		{
			int UID = 0;
			MeshInfo* info = m_pDataContext->m_MeshData.GetMeshInfo(i);
			if (j < delUIDs.size() && i == delUIDs[j])
			{
				m_info[j] = *(info);
				j++;
				UID = -1;
			}
			else
				UID = index++;

			m_UID[i] = UID;
			m_layerUID[i] = info->uid;
		}

		m_id = s_id++;

		m_WorkMode = MESH_WORK_MANAGER->getWorkMode();
	}
}

ActionDelMeshes::~ActionDelMeshes()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString str = QString().sprintf("%d", m_id);

		deleteUndoRedoFile(str, &dir);
	}

	SAFE_DELETES(m_Points);
	SAFE_DELETES(m_layerUID);
	SAFE_DELETES(m_UID);
	SAFE_DELETES(m_info);
}

void ActionDelMeshes::undo()
{
	if (m_WorkMode != NONE)
	{
		MESH_DIALOG_MANAGER->makeMeshDialog(m_WorkMode, true);
	}
	else
	{
		MESH_DIALOG_MANAGER->rejectDialog();
	}

	if (!m_pDataContext)
	{
		return;
	}

	//	for (int i = 0, j = 0; i < m_BeforeCnt; i++)
	for (int i = m_BeforeCnt - 1, j = 0; i >= 0; i--)
	{
		/*if (m_UID[i] == -1)
		{
			m_pDataContext->m_MeshData.AddMeshInfo(i, m_info[j]);
			if (m_layerUID[i] != -1)
				m_pDataContext->m_MeshData.SetConnectMesh(i, m_layerUID[i]);
			j++;
		}
		else*/
		if (m_UID[i] != -1)
			m_pDataContext->m_MeshData.ChangeMeshUID(m_UID[i], i, m_layerUID[i]);
	}

	for (int i = 0, j = 0, index = -1; i < m_DelCnt; i++)
	{
		for (index = index + 1; index < m_BeforeCnt; index++)
		{
			if (m_UID[index] == -1)
				break;
		}

		m_pDataContext->m_MeshData.AddMeshInfo(index, m_info[j]);
		if (m_layerUID[index] != -1)
			m_pDataContext->m_MeshData.SetConnectMesh(index, m_layerUID[index]);
		j++;
	}


	QDir dir(STRING_MANAGER->cacheFilePath);
	mip::MeshTopology* m = nullptr;
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);

		QFile file(filename);

		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			for (int i = 0, index = -1; i < m_DelCnt; i++)
			{
				m = new mip::MeshTopology(g_Renderer);

				for (index = index + 1; index < m_BeforeCnt; index++)
				{
					if (m_UID[index] == -1)
						break;
				}

				if (m)
				{
					QString _file_name = filename + QString().sprintf("_%d", index);

					loadDataAllRedoFile(_file_name, m);

					m_pDataContext->m_MeshData.InsertMeshToMeshMap(index, m);
					//m_pDataContext->m_MeshData.InsertMesh(index, m);

					//WIN_MANAGER->vt_pckID.insert(WIN_MANAGER->vt_pckID.begin() + index, true);
					m_pDataContext->m_MeshData.SetMeshInfoModeSelectMode(index, true);
				}
			}

			file.close();

			mip::MeshTopology* mesh = m_pDataContext->m_MeshData.GetCurrentMesh();

			PLANE_MANIPULATOR->UpdatePosition(mesh);
		}
	}

	int cur_idx = m_pDataContext->m_MeshData.GetCurrentMeshIndex();
	m_pDataContext->m_MeshData.ClearMeshInfoSelectMode();
	m_pDataContext->m_MeshData.SetMeshInfoModeSelectMode(cur_idx, true);

	MESH_MODELVIEW_MANAGER->UpdatePivotPoint(cur_idx);

	WIN_MANAGER->updateMeshUI();


	WIN_MANAGER->updateMeshUI(false, cur_idx, false, false);

	WIN_MANAGER->renderLater_All();
	WIN_MANAGER->setSaveState(false);
}

void ActionDelMeshes::redo()
{
	if (!m_pDataContext)
	{
		return;
	}

	if (m_first)
	{
		m_first = false;

		QDir dir(STRING_MANAGER->cacheFilePath);

		if (dir.exists())
		{
			QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);

			QFile file(filename);

			if ((!file.exists()) && file.open(QIODevice::WriteOnly))
			{
				double* v1, * v2, * v3;
				mip::MeshTopology* m = nullptr;
				for (int i = 0, index = -1; i < m_DelCnt; i++)
				{
					for (index = index + 1; index < m_BeforeCnt; index++)
					{
						if (m_UID[index] == -1)
							break;
					}

					m = m_pDataContext->m_MeshData.GetMesh(index);
					if (m)
					{
						QString _file_name = filename + QString().sprintf("_%d", index);

						saveDataAllRedoFile(_file_name, m);
					}
				}

				file.close();
			}
		}
	}

	for (int i = 0, j = 0, index = m_BeforeCnt; i < m_DelCnt; i++)
	{
		for (index = index - 1; index >= 0; index--)
		{
			if (m_UID[index] == -1)
				break;
		}

		if (m_layerUID[index] != -1)
			m_pDataContext->m_MeshData.SetDisconnectMesh(m_layerUID[index]);

		m_pDataContext->m_MeshData.DeleteMeshLayer(index);
		j++;
	}

	for (int i = 0; i < m_BeforeCnt; i++)
	{
		if (m_UID[i] != -1)
		{
			m_pDataContext->m_MeshData.ChangeMeshUID(i, m_UID[i], m_layerUID[i]);
		}
	}

	int cur_idx = m_pDataContext->m_MeshData.GetCurrentMeshIndex();
	m_pDataContext->m_MeshData.ClearMeshInfoSelectMode();
	m_pDataContext->m_MeshData.SetMeshInfoModeSelectMode(cur_idx, true);

	MESH_MODELVIEW_MANAGER->UpdatePivotPoint(cur_idx);

	if (!m_first)
	{
		WIN_MANAGER->updateMeshUI();
		for (int i = 0; i < m_pDataContext->m_MeshData.GetMeshCount(); ++i)
		{
			MeshInfo* pInfo = m_pDataContext->m_MeshData.GetMeshInfo(i);

			if (pInfo && pInfo->selected)
			{
				WIN_MANAGER->updateMeshUI(false, i, false, false);
			}
		}
	}

	MESH_DIALOG_MANAGER->rejectDialog();

	MESH_WORK_MANAGER->UpdateWorkMode(MESH_WORK_NONE, true);

	WIN_MANAGER->updateMeshUI();
	WIN_MANAGER->renderLater_All();
	WIN_MANAGER->setSaveState(false);
}

ActionUpdateMesh::ActionUpdateMesh(DataContext* pDataContext, mint8 mUID, mip::MeshTopology* m, QUndoCommand* parent) :
	QUndoCommand(parent),
	m_UID(mUID),
	m_mesh(m),
	m_pDataContext(pDataContext)
{
	static int s_id = ACT_ID_MESHLIST_UPDATE;

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 3; j++)
			m_Points[i][j] = -1;
	}

	m_id = s_id++;
}


ActionUpdateMesh::~ActionUpdateMesh()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString str = QString().sprintf("%d", m_id);

		deleteUndoRedoFile(str, &dir);
	}
}

void ActionUpdateMesh::undo()
{
	if (!m_pDataContext)
	{
		return;
	}

	QDir dir(STRING_MANAGER->cacheFilePath);

	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
		QFile file(filename);

		if (!file.exists() && file.open(QIODevice::WriteOnly))
		{
			auto p_mesh = m_pDataContext->m_MeshData.GetMesh(m_UID);

			if (p_mesh)
			{
				QString _file_name = filename + QString().sprintf("_%d", m_UID);

				saveDataAllRedoFile(_file_name, p_mesh);
			}
		}

		file.close();

		filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);
		file.setFileName(filename);

		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			m_mesh = new mip::MeshTopology(g_Renderer);
			if (m_mesh)
			{
				QString _file_name = filename + QString().sprintf("_%d", m_UID);

				loadDataAllRedoFile(_file_name, m_mesh);

				m_pDataContext->m_MeshData.DeleteMeshTopologyData(m_UID);
				m_pDataContext->m_MeshData.InsertMeshToMeshMap(m_UID, m_mesh);
				m_mesh = NULL;

				PLANE_MANIPULATOR->UpdatePosition(m_mesh);
			}

			file.close();


		}
	}

	// 200827 허건 대리 주석처리
	//WIN_MANAGER->updateMeshUI();
	WIN_MANAGER->renderLater_All();
	WIN_MANAGER->setSaveState(false);
}

void ActionUpdateMesh::redo()
{
	if (!m_pDataContext)
	{
		return;
	}

	QDir dir(STRING_MANAGER->cacheFilePath);

	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);

		QFile file(filename);

		if (!file.exists() && file.open(QIODevice::WriteOnly))
		{
			mip::MeshTopology* m = nullptr;

			m = m_pDataContext->m_MeshData.GetMesh(m_UID);
			if (m)
			{
				QString _file_name = filename + QString().sprintf("_%d", m_UID);

				saveDataAllRedoFile(_file_name, m);
			}

			file.close();
		}

		filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
		file.setFileName(filename);

		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			m_mesh = new mip::MeshTopology(g_Renderer);
			if (m_mesh)
			{
				QString _file_name = filename + QString().sprintf("_%d", m_UID);

				loadDataAllRedoFile(_file_name, m_mesh);

				m_pDataContext->m_MeshData.DeleteMeshTopologyData(m_UID);
				m_pDataContext->m_MeshData.InsertMeshToMeshMap(m_UID, m_mesh);

				PLANE_MANIPULATOR->UpdatePosition(m_mesh);

				m_mesh = NULL;
			}
		}

		file.close();
	}

	// 200827 허건 대리 주석처리
	//WIN_MANAGER->updateMeshUI(); 
	WIN_MANAGER->renderLater_All();
	WIN_MANAGER->setSaveState(false);
}

// 201102 허 건 대리
// Undo Redo Action 
//////////////////////////////////////////////////////////////////////////////////////////
ActionUndoRedoMesh::ActionUndoRedoMesh(
	DataContext* pDataContext,
	MESH_WORK_MODE _work_mode,
	MESH_WORK_MODE _work_prev_mode,
	QUndoCommand* parent
) : QUndoCommand(parent)
{
	m_pDataContext = pDataContext;

	static int s_id = ACT_ID_MESHLIST_UPDATE;

	m_id = s_id++;

	m_WorkMode = _work_mode;

	m_PrevWorkMode = _work_prev_mode;

	m_b_PlaneFill = false;

	m_b_first = true;

	m_SculptMode = SCULPT_MODE::SM_SMOOTH;

	//m_vt_pckID = WIN_MANAGER->vt_pckID;

	if (m_pDataContext)
	{
		m_CurrentMeshIndex = m_pDataContext->m_MeshData.GetCurrentMeshIndex();
	}
	else
	{
		m_CurrentMeshIndex = -1;
	}
}


ActionUndoRedoMesh::~ActionUndoRedoMesh()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString str = QString().sprintf("%d", m_id);

		deleteUndoRedoFile(str, &dir);
	}
}

void ActionUndoRedoMesh::undo()
{
	if (!m_pDataContext)
	{
		return;
	}

	// Mesh Editing Tab 체크
	CheckMeshEditingTab();

	if (!m_b_first)
	{
		// Mesh list Picking 상태 업데이트
		UpdateMeshList();
	}

	// 다이얼로그 상태 변경
	{
		switch (m_WorkMode)
		{
		case MESH_WORK_BRUSH_SELECTION:
		{
			MESH_DIALOG_MANAGER->closeDialog();
		}
		break;
		case MESH_WORK_BRUSH_SCULPT:
		{
			MESH_DIALOG_MANAGER->closeDialog();
			MESH_DIALOG_MANAGER->makeMeshDialog(m_PrevWorkMode);
		}
		break;
		case MESH_WORK_MANIFULATE:
		{
			if (m_PrevWorkMode != MESH_WORK_MANIFULATE)
			{
				MESH_DIALOG_MANAGER->closeDialog();

				MESH_DIALOG_MANAGER->makeMeshDialog(m_PrevWorkMode);
			}
		}
		break;
		case MESH_WORK_POLYGON_CUT:
		case MESH_WORK_POLYLINE_CUT:
		case MESH_WORK_FREEPOLYLINE_CUT:
		case MESH_WORK_PLANE_CUT:
		{
			if ((m_PrevWorkMode != MESH_WORK_POLYGON_CUT)
				&& (m_PrevWorkMode != MESH_WORK_POLYLINE_CUT)
				&& (m_PrevWorkMode != MESH_WORK_FREEPOLYLINE_CUT)
				&& (m_PrevWorkMode != MESH_WORK_PLANE_CUT)
				)
			{
				MESH_DIALOG_MANAGER->closeDialog();

				MESH_DIALOG_MANAGER->makeMeshDialog(m_PrevWorkMode);
			}
			else
			{
				if (m_WorkMode != m_PrevWorkMode)
				{
					CMeshCutDlg* dlg = static_cast<CMeshCutDlg*>(MESH_DIALOG_MANAGER->getMeshDialog(m_WorkMode));
					if (dlg && dlg->isVisible())
					{
						dlg->updateComboboxIndex(int(m_PrevWorkMode - MESH_WORK_PLANE_CUT), false);
					}
				}
			}
		}
		break;
		case MESH_WORK_HOLE_FILL_SELECTED:
		{
			if (m_PrevWorkMode != MESH_WORK_HOLE_FILL_SELECTED)
			{
				MESH_DIALOG_MANAGER->closeDialog();

				MESH_DIALOG_MANAGER->makeMeshDialog(m_PrevWorkMode);
			}
		}
		break;
		case MESH_WORK_NONE:
		{
			MESH_DIALOG_MANAGER->closeDialog();

			if (m_PrevWorkMode != MESH_WORK_NONE)
			{
				switch (m_PrevWorkMode)
				{
				case MESH_WORK_MANIFULATE:
				case MESH_WORK_POLYGON_CUT:
				case MESH_WORK_POLYLINE_CUT:
				case MESH_WORK_FREEPOLYLINE_CUT:
				case MESH_WORK_PLANE_CUT:
				case MESH_WORK_HOLE_FILL_SELECTED:
				case MESH_WORK_SMOOTH:
				case MESH_WORK_REDUCTION:
				case MESH_WORK_SOLID:
				case MESH_WORK_REMESH:
				case MESH_WORK_SUBDIVISION:
				case MESH_WORK_ISLANDFILTER:
				{
					MESH_DIALOG_MANAGER->makeMeshDialog(m_PrevWorkMode);
				}
				break;
				default: break;
				}
			}
		}
		break;
		default: break;
		}
	}

	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		// Redo File 저장
		SaveRedoFiles();

		// Undo File 로딩
		LoadUndoFiles();
	}

	// Mesh Work 상태변경
	MESH_WORK_MANAGER->UpdateWorkMode(
		(m_WorkMode != MESH_WORK_TRANSFORM) ? m_WorkMode : MESH_WORK_NONE
	);

	switch (m_PrevWorkMode)
	{
	case MESH_WORK_HOLE_FILL_SELECTED:
	{
		MESH_HOLE_MANAGER->setMeshIdx(m_CurrentMeshIndex);
		MESH_DIALOG_MANAGER->makeMeshDialog(m_PrevWorkMode, false, false);
	}
	break;
	case MESH_WORK_BRUSH_SCULPT:
	{
		MESH_DIALOG_MANAGER->makeMeshDialog(m_PrevWorkMode, false, false);
	}
	break;
	case MESH_WORK_BRUSH_SELECTION:
	{
		MESH_DIALOG_MANAGER->closeDialog();

		if (m_PrevWorkMode != MESH_WORK_BRUSH_SCULPT && m_PrevWorkMode != MESH_WORK_NONE)
		{
			BrushSelectDlg* dlg = static_cast<BrushSelectDlg*>(MESH_DIALOG_MANAGER->getMeshDialog(MESH_DIALOG_SELECT));

			if (!dlg)
			{
				MESH_DIALOG_MANAGER->makeMeshDialog(m_PrevWorkMode, false, false);
			}
			else
			{
				if (!dlg->isVisible())
				{
					dlg->show();
				}
			}

			mip::MeshTopology* p_mesh = m_pDataContext->m_MeshData.GetMesh(m_CurrentMeshIndex);

			if (p_mesh && mip::mesh_control::isTriFlag(p_mesh, mip::SELECTED))
			{
				if (dlg && dlg->isVisible())
				{
					dlg->showFuncBtn();
				}
			}
			else
			{
				if (dlg && dlg->isVisible())
				{
					dlg->hideFuncBtn();
				}
			}
		}
	}
	break;
	case MESH_WORK_PLANE_CUT:
	case MESH_WORK_POLYGON_CUT:
	case MESH_WORK_FREEPOLYLINE_CUT:
	case MESH_WORK_POLYLINE_CUT:
	{
		CMeshCutDlg* dlg = static_cast<CMeshCutDlg*>(MESH_DIALOG_MANAGER->getMeshDialog(MESH_DIALOG_MESHCUT));
		if (dlg && !dlg->isVisible())
		{
			MESH_DIALOG_MANAGER->makeMeshDialog(m_PrevWorkMode, false, false);
		}
	}
	break;
	case MESH_WORK_NONE:
	{
		if (!m_b_first)
		{
			if (MESH_DIALOG_MANAGER->isMeshDialog())
			{
				MESH_DIALOG_MANAGER->closeDialog();
			}
		}
	}
	break;
	default:
	{
		MESH_DIALOG_MANAGER->makeMeshDialog(m_PrevWorkMode, false, false);
	}
	break;
	}

	m_pDataContext->m_MeshData.SetCurrentMeshIndex(m_CurrentMeshIndex);
	MESH_MODELVIEW_MANAGER->UpdatePivotPoint(m_CurrentMeshIndex);

	WIN_MANAGER->renderLater_All();
	WIN_MANAGER->setSaveState(false);
	ACTION_MANAGER->SendActionFinished(ACTION_PROCESSING::ACTP_MESH_DATA_CHANGED);
}

void ActionUndoRedoMesh::redo()
{
	if (!m_pDataContext)
	{
		return;
	}

	// Mesh Editing Tab 체크
	CheckMeshEditingTab();

	// Mesh list Picking 상태 업데이트
	if (!m_b_first)
	{
		UpdateMeshList();
	}

	// 다이얼로그 상태 변경
	{
		switch (m_WorkMode)
		{
		case MESH_WORK_SMOOTH:
		case MESH_WORK_REDUCTION:
		case MESH_WORK_SOLID:
		case MESH_WORK_REMESH:
		case MESH_WORK_SUBDIVISION:
		case MESH_WORK_ISLANDFILTER:
		{
			if (m_PrevWorkMode != m_WorkMode && !m_b_first)
			{
				MESH_DIALOG_MANAGER->closeDialog();
				MESH_DIALOG_MANAGER->makeMeshDialog(m_WorkMode, false, false);
			}
		}
		break;
		case MESH_WORK_BRUSH_SELECTION:
		{
			if (m_PrevWorkMode != MESH_WORK_BRUSH_SELECTION)
			{
				MESH_DIALOG_MANAGER->closeDialog();

				mip::MeshTopology* p_mesh = m_pDataContext->m_MeshData.GetCurrentMesh();
				if (p_mesh && mip::mesh_control::isTriFlag(p_mesh, mip::SELECTED))
				{
					MESH_DIALOG_MANAGER->makeMeshDialog(m_WorkMode, false, false);

					BrushSelectDlg* dlg = static_cast<BrushSelectDlg*>(MESH_DIALOG_MANAGER->getMeshDialog(MESH_DIALOG_SELECT));
					if (dlg && dlg->isVisible() && !m_b_first)
					{
						dlg->showFuncBtn();
					}
				}
				else
				{
					MESH_DIALOG_MANAGER->makeMeshDialog(m_WorkMode);
				}
			}
			else
			{
				mip::MeshTopology* p_mesh = m_pDataContext->m_MeshData.GetMesh(m_CurrentMeshIndex);
				if (p_mesh)
				{
					BrushSelectDlg* dlg = static_cast<BrushSelectDlg*>(MESH_DIALOG_MANAGER->getMeshDialog(MESH_DIALOG_SELECT));
					if (dlg && dlg->isVisible() && !m_b_first)
					{
						if (mip::mesh_control::isTriFlag(p_mesh, mip::SELECTED))
						{
							dlg->showFuncBtn();
						}
						else
						{
							dlg->hideFuncBtn();
						}
					}
				}
			}
		}
		break;
		case MESH_WORK_POLYGON_CUT:
		case MESH_WORK_POLYLINE_CUT:
		case MESH_WORK_FREEPOLYLINE_CUT:
		case MESH_WORK_PLANE_CUT:
		{
			if ((m_PrevWorkMode != MESH_WORK_POLYGON_CUT)
				&& (m_PrevWorkMode != MESH_WORK_POLYLINE_CUT)
				&& (m_PrevWorkMode != MESH_WORK_FREEPOLYLINE_CUT)
				&& (m_PrevWorkMode != MESH_WORK_PLANE_CUT)
				&& !m_b_first
				)
			{
				MESH_DIALOG_MANAGER->closeDialog();

				MESH_DIALOG_MANAGER->makeMeshDialog(m_WorkMode);
			}
			else
			{
				if (m_WorkMode != m_PrevWorkMode && !m_b_first)
				{
					CMeshCutDlg* dlg = static_cast<CMeshCutDlg*>(MESH_DIALOG_MANAGER->getMeshDialog(m_WorkMode));
					if (dlg && dlg->isVisible())
					{
						dlg->updateComboboxIndex(int(m_WorkMode - MESH_WORK_PLANE_CUT), false);
					}
				}
			}
		}
		break;
		case MESH_WORK_NONE:
		{
			if (m_PrevWorkMode != MESH_WORK_NONE && m_PrevWorkMode != MESH_WORK_HOLLOW)
			{
				MESH_DIALOG_MANAGER->closeDialog();
			}


			if (m_PrevWorkMode == MESH_WORK_HOLLOW)
			{
				HollowDlg* dlg = static_cast<HollowDlg*>(MESH_DIALOG_MANAGER->getMeshDialog(MESH_WORK_HOLLOW));
				if (dlg && dlg->isVisible())
				{
					if (dlg->getState() == 0) //apply 
					{
						MESH_DIALOG_MANAGER->closeDialog();
					}
					else
					{
						dlg->setState(0);
					}
				}
			}

		}
		break;
		default: break;
		}
	}

	QDir dir(STRING_MANAGER->cacheFilePath);

	// 저장할 폴더 위치 존재 체크
	if (dir.exists())
	{
		int n_mesh = m_pDataContext->m_MeshData.GetMeshCount();

		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);

		QFile file(filename);

		SaveUndoFiles();

		LoadRedoFiles();
	}

	// Mesh Work 상태변경
	MESH_WORK_MANAGER->UpdateWorkMode(
		(m_WorkMode != MESH_WORK_TRANSFORM) ? m_WorkMode : MESH_WORK_NONE
	);

	switch (m_WorkMode)
	{
	case MESH_WORK_BRUSH_SCULPT:
	{
		if (m_PrevWorkMode != MESH_WORK_BRUSH_SCULPT)
		{
			MESH_DIALOG_MANAGER->makeMeshDialog(m_WorkMode);
		}
	}
	break;
	case MESH_WORK_MANIFULATE:
	{
		if (m_PrevWorkMode != MESH_WORK_MANIFULATE)
		{
			MESH_DIALOG_MANAGER->closeDialog();

			MESH_DIALOG_MANAGER->makeMeshDialog(m_WorkMode);
		}
	}
	break;
	case MESH_WORK_HOLE_FILL_SELECTED:
	{
		if (m_PrevWorkMode != MESH_WORK_HOLE_FILL_SELECTED)
		{
			MESH_DIALOG_MANAGER->closeDialog();

			MESH_DIALOG_MANAGER->makeMeshDialog(m_WorkMode);

			MESH_HOLE_MANAGER->setMeshIdx(m_CurrentMeshIndex);
		}
	}
	break;
	case MESH_WORK_TRANSFORM:
	case MESH_WORK_NONE:
	{
		if (!m_b_first)
		{
			if (MESH_DIALOG_MANAGER->isMeshDialog())
			{
				MESH_DIALOG_MANAGER->closeDialog();
			}
		}
	}
	break;
	default:
	{
		if (!m_b_first)
		{
			if (MESH_DIALOG_MANAGER->isMeshDialog())
			{
				MESH_DIALOG_MANAGER->closeDialog();
			}

			MESH_DIALOG_MANAGER->makeMeshDialog(m_WorkMode);
		}
	}
	break;
	}

	m_b_first = false;

	m_pDataContext->m_MeshData.SetCurrentMeshIndex(m_CurrentMeshIndex);
	MESH_MODELVIEW_MANAGER->UpdatePivotPoint(m_CurrentMeshIndex);

	WIN_MANAGER->renderLater_All();
	WIN_MANAGER->setSaveState(false);
	ACTION_MANAGER->SendActionFinished(ACTION_PROCESSING::ACTP_MESH_DATA_CHANGED);
}

void ActionUndoRedoMesh::SaveUndoFiles()
{
	if (!m_pDataContext)
	{
		return;
	}

	int n_mesh = m_pDataContext->m_MeshData.GetMeshCount();

	QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);

	QFile file(filename);

	// Undo 파일 생성여부 체크 및 저장
	if (!file.exists() && file.open(QIODevice::WriteOnly))
	{
		saveHoleInfo(filename);

		std::vector<int> vt_pck_id;
		for (int i = 0; i < n_mesh; ++i)
		{
			MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(i);
			if (pMeshInfo && pMeshInfo->selected)
			{
				vt_pck_id.push_back(i);
			}
		}

		for (int i = 0; i < vt_pck_id.size(); ++i)
		{
			mip::MeshTopology* p_mesh = m_pDataContext->m_MeshData.GetMesh(vt_pck_id[i]);

			if (p_mesh)
			{
				QString _file_name = filename + QString().sprintf("_%d", vt_pck_id[i]);

				// 현재 Mesh work mode 체크
				switch (m_WorkMode)
				{
				case MESH_WORK_SMOOTH:
				case MESH_WORK_REDUCTION:
				case MESH_WORK_BRUSH_SCULPT:
				{
					if (m_PrevWorkMode != MESH_WORK_BRUSH_SELECTION)
					{
						saveDataAllRedoFile(_file_name, p_mesh, m_WorkMode);

						if (m_PrevWorkMode == MESH_WORK_MANIFULATE)
						{
							saveManipulatorInfo(m_pDataContext, _file_name, vt_pck_id[i], true);
						}
					}
				}
				break;
				case MESH_WORK_POLYGON_CUT:
				case MESH_WORK_POLYLINE_CUT:
				case MESH_WORK_FREEPOLYLINE_CUT:
				{
					if (p_mesh)
					{
						switch (m_PrevWorkMode)
						{
						case MESH_WORK_POLYGON_CUT:
						case MESH_WORK_POLYLINE_CUT:
						case MESH_WORK_FREEPOLYLINE_CUT:
						{
							saveTVertexRedoFile(_file_name, p_mesh);
							saveTTrisRedoFile(_file_name, p_mesh);
							saveTHedgeRedoFile(_file_name, p_mesh);
						}
						break;
						default:
						{
							saveDataAllRedoFile(_file_name, p_mesh, m_WorkMode);

							if (m_PrevWorkMode == MESH_WORK_MANIFULATE)
							{
								saveManipulatorInfo(m_pDataContext, _file_name, vt_pck_id[i], true);
							}
						}
						break;
						}
					}
				}
				break;
				case MESH_WORK_MANIFULATE:
				{
					if (m_PrevWorkMode != MESH_WORK_MANIFULATE)
					{
						switch (m_PrevWorkMode)
						{
						case MESH_WORK_POLYGON_CUT:
						case MESH_WORK_POLYLINE_CUT:
						case MESH_WORK_FREEPOLYLINE_CUT:
						{
							if (p_mesh)
							{
								saveTVertexRedoFile(_file_name, p_mesh);
								saveTTrisRedoFile(_file_name, p_mesh);
								saveTHedgeRedoFile(_file_name, p_mesh);
							}

							saveManipulatorInfo(m_pDataContext, _file_name, vt_pck_id[i], true);
						}
						break;
						default:
						{
							saveDataAllRedoFile(_file_name, p_mesh, m_WorkMode);
							saveManipulatorInfo(m_pDataContext, _file_name, vt_pck_id[i], true);
						}
						break;
						}
					}
					else
					{
						saveManipulatorInfo(m_pDataContext, _file_name, vt_pck_id[i], true);
					}
				}
				break;
				case MESH_WORK_HOLE_FILL_SELECTED:
				{
					if (m_PrevWorkMode != MESH_WORK_HOLE_FILL_SELECTED)
					{
						switch (m_PrevWorkMode)
						{
						case MESH_WORK_POLYGON_CUT:
						case MESH_WORK_POLYLINE_CUT:
						case MESH_WORK_FREEPOLYLINE_CUT:
						{
							if (p_mesh)
							{
								saveTVertexRedoFile(_file_name, p_mesh);
								saveTTrisRedoFile(_file_name, p_mesh);
								saveTHedgeRedoFile(_file_name, p_mesh);
							}
						}
						break;
						case MESH_WORK_MANIFULATE:
						{
							saveManipulatorInfo(m_pDataContext, _file_name, vt_pck_id[i], false);
						}
						break;
						default:
						{
							saveDataAllRedoFile(_file_name, p_mesh, m_WorkMode);

							if (m_PrevWorkMode == MESH_WORK_MANIFULATE)
							{
								saveManipulatorInfo(m_pDataContext, _file_name, vt_pck_id[i], true);
							}
						}
						break;
						}
					}
					else
					{
						saveDataAllRedoFile(_file_name, p_mesh, m_WorkMode);

						if (m_PrevWorkMode == MESH_WORK_MANIFULATE)
						{
							saveManipulatorInfo(m_pDataContext, _file_name, vt_pck_id[i], true);
						}
					}
				}
				break;
				case MESH_WORK_TRANSFORM:
				{
					saveManipulatorInfo(m_pDataContext, _file_name, vt_pck_id[i], false);
				}
				break;
				case MESH_WORK_NONE:
				{
					if (m_PrevWorkMode != MESH_WORK_NONE)
					{
						switch (m_PrevWorkMode)
						{
						case MESH_WORK_POLYGON_CUT:
						case MESH_WORK_POLYLINE_CUT:
						case MESH_WORK_FREEPOLYLINE_CUT:
						{
							if (p_mesh)
							{
								saveTVertexRedoFile(_file_name, p_mesh);
								saveTTrisRedoFile(_file_name, p_mesh);
								saveTHedgeRedoFile(_file_name, p_mesh);
							}
						}
						break;
						case MESH_WORK_MANIFULATE:
						{
							saveManipulatorInfo(m_pDataContext, _file_name, vt_pck_id[i], false);
						}
						break;
						default:
						{
							saveDataAllRedoFile(_file_name, p_mesh, m_WorkMode);
						}
						break;
						}
					}
				}
				break;
				default:
				{
					saveDataAllRedoFile(_file_name, p_mesh, m_WorkMode);

					if (m_PrevWorkMode == MESH_WORK_MANIFULATE)
					{
						saveManipulatorInfo(m_pDataContext, _file_name, vt_pck_id[i], true);
					}
				}
				break;
				}
			}

		}
	}

	file.close();
}

void ActionUndoRedoMesh::SaveRedoFiles()
{
	if (!m_pDataContext) return;

	int n_mesh = m_pDataContext->m_MeshData.GetMeshCount();

	QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
	QFile file(filename);

	if (!file.exists() && file.open(QIODevice::WriteOnly))
	{
		QString filename_undo = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);
		QFile file_undo(filename_undo);
		file_undo.setFileName(filename_undo);

		std::vector<int> vt_idxs;

		if (file_undo.exists())
		{
			for (int i = 0; i < n_mesh; ++i)
			{
				QString _file_name = filename_undo + QString().sprintf("_%d", i);

				if (m_WorkMode != MESH_WORK_MANIFULATE)
				{
					switch (m_PrevWorkMode)
					{
					case MESH_WORK_MANIFULATE:
					case MESH_WORK_TRANSFORM:
					{
						QFile _file(_file_name + "_info");

						if (_file.exists())
						{
							vt_idxs.push_back(i);
						}
					}
					break;
					default:
					{
						QFile _file(_file_name + "_tvertex");

						if (_file.exists())
						{
							vt_idxs.push_back(i);
						}
					}
					break;
					}

				}
				else
				{
					QFile _file(_file_name + "_info");

					if (_file.exists())
					{
						vt_idxs.push_back(i);
					}
				}
			}

			saveHoleInfo(filename);

			for (int i = 0; i < vt_idxs.size(); ++i)
			{
				mip::MeshTopology* mesh = m_pDataContext->m_MeshData.GetMesh(vt_idxs[i]);

				if (mesh)
				{
					QString _file_name = filename + QString().sprintf("_%d", vt_idxs[i]);

					switch (m_WorkMode)
					{
					case MESH_WORK_SMOOTH:
					case MESH_WORK_REDUCTION:
					case MESH_WORK_BRUSH_SCULPT:
					{
						if (m_PrevWorkMode != MESH_WORK_BRUSH_SELECTION)
						{
							saveDataAllRedoFile(_file_name, mesh, m_WorkMode);
						}
					}
					break;
					case MESH_WORK_POLYGON_CUT:
					case MESH_WORK_POLYLINE_CUT:
					case MESH_WORK_FREEPOLYLINE_CUT:
					{
						saveTVertexRedoFile(_file_name, mesh);
						saveTTrisRedoFile(_file_name, mesh);
						saveTHedgeRedoFile(_file_name, mesh);
					}
					break;
					case MESH_WORK_MANIFULATE:
					case MESH_WORK_TRANSFORM:
					{
						saveManipulatorInfo(m_pDataContext, _file_name, vt_idxs[i], false);
					}
					break;
					case MESH_WORK_NONE:
					{
						switch (m_PrevWorkMode)
						{
						case MESH_WORK_POLYGON_CUT:
						case MESH_WORK_POLYLINE_CUT:
						case MESH_WORK_FREEPOLYLINE_CUT:
						{
							saveTVertexRedoFile(_file_name, mesh);
							saveTTrisRedoFile(_file_name, mesh);
							saveTHedgeRedoFile(_file_name, mesh);
						}
						break;
						case MESH_WORK_MANIFULATE:
						{
							saveManipulatorInfo(m_pDataContext, _file_name, vt_idxs[i], false);
						}
						break;
						default:
						{
							saveDataAllRedoFile(_file_name, mesh, m_WorkMode);

							if (m_PrevWorkMode == MESH_WORK_MANIFULATE)
							{
								saveManipulatorInfo(m_pDataContext, _file_name, vt_idxs[i], true);
							}
						}
						break;
						}
					}
					break;
					default:
					{
						saveDataAllRedoFile(_file_name, mesh, m_WorkMode);
					}
					break;
					}
				}
			}
		}
	}

	file.close();
}

void ActionUndoRedoMesh::LoadUndoFiles()
{
	if (!m_pDataContext)
	{
		return;
	}

	QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);
	QFile file(filename);

	if (file.exists() && file.open(QIODevice::ReadOnly))
	{
		loadHoleInfo(filename);

		int n_mesh = m_pDataContext->m_MeshData.GetMeshCount();

		for (int i = 0; i < n_mesh; ++i)
		{
			mip::MeshTopology* p_mesh = m_pDataContext->m_MeshData.GetMesh(i);

			QString _file_name = filename + QString().sprintf("_%d", i);

			QFile _file(_file_name + "_tvertex");

			switch (m_WorkMode)
			{
			case MESH_WORK_SMOOTH:
			case MESH_WORK_REDUCTION:
			case MESH_WORK_BRUSH_SCULPT:
			{
				if (m_PrevWorkMode != MESH_WORK_BRUSH_SELECTION)
				{
					if (_file.exists())
					{
						loadDataAllRedoFile(_file_name, p_mesh, m_WorkMode);
					}
				}
			}
			break;
			case MESH_WORK_POLYGON_CUT:
			case MESH_WORK_POLYLINE_CUT:
			case MESH_WORK_FREEPOLYLINE_CUT:
			{
				if (p_mesh)
				{
					switch (m_PrevWorkMode)
					{
					case MESH_WORK_MANIFULATE:
					{
						if (loadManipulatorInfo(m_pDataContext, _file_name, i))
						{
							MESH_MANIPULATOR->UpdatePosition(p_mesh, false);
						}
					}
					break;
					case MESH_WORK_POLYGON_CUT:
					case MESH_WORK_POLYLINE_CUT:
					case MESH_WORK_FREEPOLYLINE_CUT:
					{
						if (_file.exists())
						{
							p_mesh->m_tverts.clear();
							p_mesh->m_ttris.clear();
							p_mesh->m_tVHedges.clear();

							loadTVertexRedoFile(_file_name, p_mesh);
							loadTTrisRedoFile(_file_name, p_mesh);
							loadTHedgeRedoFile(_file_name, p_mesh);

							updateBB(p_mesh);

							p_mesh->updateVertex();
							p_mesh->updateColor(p_mesh->m_baseColor);

							auto poly_line = MESH_CUT_MANAGER->getPolyLinePtr();
							poly_line->clear();

							auto hole_ptr = MESH_CUT_MANAGER->getHolePtr();
							hole_ptr->clear();

							auto view = WIN_MANAGER->mainMeshWidget->getMainView();
							view->updateGeometryCount();

							WIN_MANAGER->buildRenderBufferTopology(p_mesh);
						}

						_file.close();
					}
					default:
					{
						if (m_PrevWorkMode != MESH_WORK_MANIFULATE)
						{
							if (_file.exists())
							{
								loadDataAllRedoFile(_file_name, p_mesh, m_WorkMode);
							}
						}
						else
						{
							if (loadManipulatorInfo(m_pDataContext, _file_name, i))
							{
								MESH_MANIPULATOR->UpdatePosition(p_mesh, false);
							}
						}
					}
					break;
					}
				}
			}
			break;
			case MESH_WORK_MANIFULATE:
			{
				if (m_PrevWorkMode != MESH_WORK_MANIFULATE)
				{
					switch (m_PrevWorkMode)
					{
					case MESH_WORK_POLYGON_CUT:
					case MESH_WORK_POLYLINE_CUT:
					case MESH_WORK_FREEPOLYLINE_CUT:
					{
						if (p_mesh)
						{
							if (_file.exists())
							{
								p_mesh->m_tverts.clear();
								p_mesh->m_ttris.clear();
								p_mesh->m_tVHedges.clear();

								loadTVertexRedoFile(_file_name, p_mesh);
								loadTTrisRedoFile(_file_name, p_mesh);
								loadTHedgeRedoFile(_file_name, p_mesh);

								updateBB(p_mesh);

								p_mesh->updateVertex();
								p_mesh->updateColor(p_mesh->m_baseColor);

								auto poly_line = MESH_CUT_MANAGER->getPolyLinePtr();
								poly_line->clear();

								auto hole_ptr = MESH_CUT_MANAGER->getHolePtr();
								hole_ptr->clear();

								auto view = WIN_MANAGER->mainMeshWidget->getMainView();
								view->updateGeometryCount();

								WIN_MANAGER->buildRenderBufferTopology(p_mesh);
							}

							_file.close();
						}
					}
					break;
					default:	break;
					}
				}
				else
				{
					if (loadManipulatorInfo(m_pDataContext, _file_name, i))
					{
						MESH_MANIPULATOR->UpdatePosition(p_mesh, false);
					}
				}
			}
			break;
			case MESH_WORK_TRANSFORM:
			{
				if (loadManipulatorInfo(m_pDataContext, _file_name, i))
				{
					MESH_MANIPULATOR->UpdatePosition(p_mesh, false);
				}
			}
			break;
			case MESH_WORK_NONE:
			{
				if (m_PrevWorkMode != MESH_WORK_NONE)
				{
					switch (m_PrevWorkMode)
					{
					case MESH_WORK_POLYGON_CUT:
					case MESH_WORK_POLYLINE_CUT:
					case MESH_WORK_FREEPOLYLINE_CUT:
					{
						if (p_mesh)
						{
							if (_file.exists())
							{
								p_mesh->m_tverts.clear();
								p_mesh->m_ttris.clear();
								p_mesh->m_tVHedges.clear();

								loadTVertexRedoFile(_file_name, p_mesh);
								loadTTrisRedoFile(_file_name, p_mesh);
								loadTHedgeRedoFile(_file_name, p_mesh);

								updateBB(p_mesh);

								p_mesh->updateVertex();
								p_mesh->updateColor(p_mesh->m_baseColor);

								auto poly_line = MESH_CUT_MANAGER->getPolyLinePtr();
								poly_line->clear();

								auto hole_ptr = MESH_CUT_MANAGER->getHolePtr();
								hole_ptr->clear();

								auto view = WIN_MANAGER->mainMeshWidget->getMainView();
								view->updateGeometryCount();

								WIN_MANAGER->buildRenderBufferTopology(p_mesh);
							}

							_file.close();
						}
					}
					break;
					case MESH_WORK_MANIFULATE:
					{
						if (loadManipulatorInfo(m_pDataContext, _file_name, i))
						{
							MESH_MANIPULATOR->UpdatePosition(p_mesh, false);
						}
					}
					break;
					default:
					{
						if (p_mesh)
						{
							if (_file.exists())
							{
								if (m_PrevWorkMode != MESH_WORK_MANIFULATE)
								{
									loadDataAllRedoFile(_file_name, p_mesh, m_WorkMode);
								}
								else
								{
									if (loadManipulatorInfo(m_pDataContext, _file_name, i))
									{
										MESH_MANIPULATOR->UpdatePosition(p_mesh, false);
									}
								}

								if (isMeshSelected() && m_PrevWorkMode == MESH_WORK_HOLE_FILL_SELECTED)
								{
									CHoleFillingDlg* dlg = static_cast<CHoleFillingDlg*>(MESH_DIALOG_MANAGER->getMeshDialog(MESH_DIALOG_FILLHOLE));
									if (dlg && dlg->isVisible())
									{
										MESH_HOLE_MANAGER->Process(MESH_WORK_FIND_HOLE);
									}
									else
									{
										MESH_HOLE_MANAGER->getBoundaryIdxPtr()->clear();
									}
								}
							}
						}
					}
					break;
					}
				}
			}
			break;
			default:
			{
				if (p_mesh)
				{
					if (m_PrevWorkMode != MESH_WORK_MANIFULATE)
					{
						if (_file.exists())
						{
							loadDataAllRedoFile(_file_name, p_mesh, m_WorkMode);
						}
					}
					else
					{
						if (loadManipulatorInfo(m_pDataContext, _file_name, i))
						{
							MESH_MANIPULATOR->UpdatePosition(p_mesh, false);
						}
					}

					if (isMeshSelected() && m_WorkMode == MESH_WORK_HOLE_FILL_SELECTED)
					{
						//CHoleFillingDlg* dlg = static_cast<CHoleFillingDlg*>(MESH_DIALOG_MANAGER->getMeshDialog(MESH_DIALOG_FILLHOLE));
						//if (dlg && dlg->isVisible())
						//{
						//	MESH_HOLE_MANAGER->Process(MESH_WORK_FIND_HOLE);
						//}
						//else
						//{
						//	MESH_HOLE_MANAGER->getBoundaryIdxPtr()->clear();
						//}
					}
				}
			}
			break;
			}
		}
	}
}

void ActionUndoRedoMesh::LoadRedoFiles()
{
	if (!m_pDataContext)
	{
		return;
	}

	// Redo 파일 생성여부 체크 및 로딩
	QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
	QFile file(filename);

	if (file.exists() && file.open(QIODevice::ReadOnly))
	{
		loadHoleInfo(filename);

		int n_mesh = m_pDataContext->m_MeshData.GetMeshCount();

		for (int i = 0; i < n_mesh; ++i)
		{
			mip::MeshTopology* p_mesh = m_pDataContext->m_MeshData.GetMesh(i);
			QString _file_name = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id) + QString().sprintf("_%d_tvertex", i);

			QFile _file(_file_name);

			_file_name = filename + QString().sprintf("_%d", i);

			switch (m_WorkMode)
			{
			case MESH_WORK_SMOOTH:
			case MESH_WORK_REDUCTION:
			case MESH_WORK_BRUSH_SCULPT:
			{
				if (_file.exists())
				{
					loadDataAllRedoFile(_file_name, p_mesh, m_WorkMode);
				}
			}
			break;
			case MESH_WORK_POLYGON_CUT:
			case MESH_WORK_POLYLINE_CUT:
			case MESH_WORK_FREEPOLYLINE_CUT:
			{
				if (_file.exists())
				{
					if (p_mesh)
					{
						p_mesh->m_tverts.clear();
						p_mesh->m_ttris.clear();
						p_mesh->m_tVHedges.clear();

						loadTVertexRedoFile(_file_name, p_mesh);
						loadTTrisRedoFile(_file_name, p_mesh);
						loadTHedgeRedoFile(_file_name, p_mesh);

						updateBB(p_mesh);

						p_mesh->updateVertex();
						p_mesh->updateColor(p_mesh->m_baseColor);

						QPolygon* poly_line = MESH_CUT_MANAGER->getPolyLinePtr();
						poly_line->clear();

						auto hole_ptr = MESH_CUT_MANAGER->getHolePtr();
						hole_ptr->clear();

						auto view = WIN_MANAGER->mainMeshWidget->getMainView();
						view->updateGeometryCount();

						WIN_MANAGER->buildRenderBufferTopology(p_mesh);
					}
				}
			}
			break;
			case MESH_WORK_MANIFULATE:
			case MESH_WORK_TRANSFORM:
			{
				if (loadManipulatorInfo(m_pDataContext, _file_name, i))
				{
					MESH_MANIPULATOR->UpdatePosition(p_mesh, false);
				}
			}
			break;
			case MESH_WORK_NONE:
			{
				if (m_WorkMode != m_PrevWorkMode)
				{
					switch (m_PrevWorkMode)
					{
					case MESH_WORK_POLYGON_CUT:
					case MESH_WORK_POLYLINE_CUT:
					case MESH_WORK_FREEPOLYLINE_CUT:
					{
						if (_file.exists())
						{
							if (p_mesh)
							{
								p_mesh->m_tverts.clear();
								p_mesh->m_ttris.clear();
								p_mesh->m_tVHedges.clear();

								loadTVertexRedoFile(_file_name, p_mesh);
								loadTTrisRedoFile(_file_name, p_mesh);
								loadTHedgeRedoFile(_file_name, p_mesh);

								updateBB(p_mesh);

								p_mesh->updateVertex();
								p_mesh->updateColor(p_mesh->m_baseColor);

								QPolygon* poly_line = MESH_CUT_MANAGER->getPolyLinePtr();
								poly_line->clear();

								auto hole_ptr = MESH_CUT_MANAGER->getHolePtr();
								hole_ptr->clear();

								auto view = WIN_MANAGER->mainMeshWidget->getMainView();
								view->updateGeometryCount();

								WIN_MANAGER->buildRenderBufferTopology(p_mesh);
							}
						}
					}
					break;
					case MESH_WORK_MANIFULATE:
					{
						if (loadManipulatorInfo(m_pDataContext, _file_name, i))
						{
							MESH_MANIPULATOR->UpdatePosition(p_mesh, false);
						}
					}
					default:
					{
						if (_file.exists())
						{
							loadDataAllRedoFile(_file_name, p_mesh, m_WorkMode);
						}
					}
					break;
					}
				}
			}
			break;
			default:
			{
				if (_file.exists())
				{
					if (m_PrevWorkMode != MESH_WORK_MANIFULATE)
					{
						loadDataAllRedoFile(_file_name, p_mesh, m_WorkMode);
					}
					else
					{
						if (loadManipulatorInfo(m_pDataContext, _file_name, i))
						{
							MESH_MANIPULATOR->UpdatePosition(p_mesh, false);
						}
					}

					if (isMeshSelected() && m_WorkMode == MESH_WORK_HOLE_FILL_SELECTED)
					{
						//CHoleFillingDlg* dlg = static_cast<CHoleFillingDlg*>(MESH_DIALOG_MANAGER->getMeshDialog(MESH_DIALOG_FILLHOLE));
						//if (dlg && dlg->isVisible())
						//{
						//	MESH_HOLE_MANAGER->Process(MESH_WORK_FIND_HOLE);
						//}
						//else
						//{
						//	MESH_HOLE_MANAGER->getBoundaryIdxPtr()->clear();
						//}
					}
				}
			}
			break;
			}
		}
	}

	file.close();
}

void ActionUndoRedoMesh::UpdateMeshList()
{
	WIN_MANAGER->updateMeshUI();
	for (int i = 0; i < m_vt_pckID.size(); ++i)
	{
		if (m_vt_pckID[i])
		{
			WIN_MANAGER->updateMeshUI(false, i, false, false);
		}
	}
}

void ActionUndoRedoMesh::CheckMeshEditingTab()
{
	MAINTAB_TYPE tab_type = WIN_MANAGER->mainTabType;

	if (tab_type != MAINTAB_MESH_EDITING)
	{
		MedipQT* main_win = WIN_MANAGER->mainWindow;

		if (main_win)
		{
			main_win->callMainTabchange((int)MAINTAB_MESH_EDITING - 1);
		}
	}
}

bool ActionUndoRedoMesh::isMeshSelected() const
{
	return m_CurrentMeshIndex != -1;
}

//////////////////////////////////////////////////////////////////////////////////////////


ActionUpdateMeshVisualPrint::ActionUpdateMeshVisualPrint(DataContext* pDataContext, mint8 mUID, mip::MeshTopology* m, QUndoCommand* parent) :
	QUndoCommand(parent),
	m_UID(mUID),
	m_mesh(m),
	m_pDataContext(pDataContext)
{
	static int s_id = ACT_ID_MESHLIST_UPDATE;

	m_id = s_id++;
}


ActionUpdateMeshVisualPrint::~ActionUpdateMeshVisualPrint()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString str = QString().sprintf("%d", m_id);

		deleteUndoRedoFile(str, &dir);
	}
}

void ActionUpdateMeshVisualPrint::undo()
{
	if (!m_pDataContext)
	{
		return;
	}

	QString current_path = STRING_MANAGER->m_strAppDataLocalPath; //20210319_byPHS_관리자권한으로 인한 수정
	QString folder_name = "Temp";
	QString save_path = current_path + "\\" + folder_name;

	if (!QDir(save_path).exists())
	{
		QDir().mkdir(save_path);
	}

	QDir dir(save_path);

	if (dir.exists())
	{
		QString filename = save_path + QString("/undo%1").arg(m_id);

		QFile file(filename);

		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			m_mesh = new mip::MeshTopology(g_Renderer);

			if (m_mesh)
			{
				QString _file_name = filename + QString().sprintf("_%d", m_UID);

				loadDataAllRedoFile(_file_name, m_mesh);
			}

			file.close();

			filename = save_path + QString("/redo%1").arg(m_id);
			file.setFileName(filename);

			if (!file.exists() && file.open(QIODevice::WriteOnly))
			{
				auto p_mesh = m_pDataContext->m_VisualPrinting_MeshData.GetMesh(m_UID);

				if (p_mesh)
				{
					QString _file_name = filename + QString().sprintf("_%d", m_UID);

					saveDataAllRedoFile(_file_name, p_mesh);
				}
			}

			file.close();
		}

		if (m_mesh)
		{
			m_pDataContext->m_VisualPrinting_MeshData.DeleteMeshTopologyData(m_UID);
			m_pDataContext->m_VisualPrinting_MeshData.InsertMeshToMeshMap(m_UID, m_mesh);

			m_mesh = NULL;
		}
	}

	WIN_MANAGER->renderLater_All();
	WIN_MANAGER->setSaveState(false);
}

void ActionUpdateMeshVisualPrint::redo()
{
	if (!m_pDataContext)
	{
		return;
	}

	QString current_path = STRING_MANAGER->m_strAppDataLocalPath; //20210319_byPHS_관리자권한으로 인한 수정
	QString folder_name = "Temp";
	QString save_path = current_path + "\\" + folder_name;

	if (!QDir(save_path).exists())
	{
		QDir().mkdir(save_path);
	}

	QDir dir(save_path);

	if (dir.exists())
	{
		QString filename = save_path + QString("/undo%1").arg(m_id);

		QFile file(filename);

		if (!file.exists() && file.open(QIODevice::WriteOnly))
		{
			mip::MeshTopology* m = nullptr;

			m = m_pDataContext->m_VisualPrinting_MeshData.GetMesh(m_UID);

			if (m)
			{
				QString _file_name = filename + QString().sprintf("_%d", m_UID);

				saveDataAllRedoFile(_file_name, m);
			}

			file.close();
		}

		filename = save_path + QString("/redo%1").arg(m_id);
		file.setFileName(filename);

		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			//////////////////////////////////////////////////
			////////////USE MEMBER MESH///////////////////////
			//////////////////////////////////////////////////
			m_mesh = new mip::MeshTopology(g_Renderer);
			if (m_mesh)
			{
				QString _file_name = filename + QString().sprintf("_%d", m_UID);

				loadDataAllRedoFile(_file_name, m_mesh);

				m_pDataContext->m_VisualPrinting_MeshData.DeleteMeshTopologyData(m_UID);
				m_pDataContext->m_VisualPrinting_MeshData.InsertMeshToMeshMap(m_UID, m_mesh);
			}
		}

		file.close();
	}

	WIN_MANAGER->renderLater_All();
	WIN_MANAGER->setSaveState(false);
}

// 201030 허 건 대리
// Visual Print Undo Redo Action 
//////////////////////////////////////////////////////////////////////////////////////////
ActionUndoRedoMeshVisualPrint::ActionUndoRedoMeshVisualPrint(DataContext* pDataContext, QUndoCommand* parent) :
	QUndoCommand(parent),
	m_pDataContext(pDataContext)
{
	static int s_id = ACT_ID_MESHLIST_UPDATE;

	m_id = s_id++;
}


ActionUndoRedoMeshVisualPrint::~ActionUndoRedoMeshVisualPrint()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString str = QString().sprintf("%d", m_id);

		deleteUndoRedoFile(str, &dir);
	}
}

void ActionUndoRedoMeshVisualPrint::undo()
{
	QString current_path = STRING_MANAGER->m_strAppDataLocalPath; //20210319_byPHS_관리자권한으로 인한 수정
	QString folder_name = "Temp";
	QString save_path = current_path + "\\" + folder_name;

	if (!QDir(save_path).exists())
	{
		QDir().mkdir(save_path);
	}

	QDir dir(save_path);

	if (dir.exists())
	{
		int n_mesh = m_pDataContext->m_VisualPrinting_MeshData.GetMeshCount();

		QString filename = save_path + QString("/redo%1").arg(m_id);
		QFile file(filename);

		if (!file.exists() && file.open(QIODevice::WriteOnly))
		{
			for (int i = 0; i < n_mesh; ++i)
			{
				QString _file_name = save_path + QString("/undo%1").arg(m_id) + QString().sprintf("_%d_tvertex", i);

				QFile _file(_file_name);

				if (_file.exists())
				{
					auto mesh = m_pDataContext->m_VisualPrinting_MeshData.GetMesh(i);

					if (mesh)
					{
						QString _file_name = filename + QString().sprintf("_%d", i);

						saveDataAllRedoFile(_file_name, mesh);
					}
				}
			}

			file.close();
		}

		filename = save_path + QString("/undo%1").arg(m_id);
		file.setFileName(filename);

		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			for (int i = 0; i < n_mesh; ++i)
			{
				auto p_mesh = new mip::MeshTopology(g_Renderer);
				if (p_mesh)
				{
					QString _file_name = filename + QString().sprintf("_%d", i);

					QFile _file(_file_name + "_tvertex");

					if (_file.exists())
					{
						loadDataAllRedoFile(_file_name, p_mesh);

						m_pDataContext->m_VisualPrinting_MeshData.DeleteMeshTopologyData(i);
						m_pDataContext->m_VisualPrinting_MeshData.InsertMeshToMeshMap(i, p_mesh);
					}
				}
			}

			file.close();
		}
	}

	WIN_MANAGER->renderLater_All();
	WIN_MANAGER->setSaveState(false);

	if (WIN_MANAGER->pVisualPrintMeshWidget->getMainView() != nullptr)
		WIN_MANAGER->pVisualPrintMeshWidget->getMainView()->updateGeometry(true);
}

void ActionUndoRedoMeshVisualPrint::redo()
{
	QString current_path = STRING_MANAGER->m_strAppDataLocalPath; //20210319_byPHS_관리자권한으로 인한 수정
	QString folder_name = "Temp";
	QString save_path = current_path + "\\" + folder_name;

	if (!QDir(save_path).exists())
	{
		QDir().mkdir(save_path);
	}

	QDir dir(save_path);

	if (dir.exists())
	{
		int n_mesh = m_pDataContext->m_VisualPrinting_MeshData.GetMeshCount();

		QString filename = save_path + QString("/undo%1").arg(m_id);

		QFile file(filename);

		if (!file.exists() && file.open(QIODevice::WriteOnly))
		{
			for (int i = 0; i < n_mesh; ++i)
			{
				auto info = m_pDataContext->m_VisualPrinting_MeshData.GetMeshInfo(i);

				if ((info->m_nSmoothLevel) != 0 || (info->m_nReduceLevel != 0))
				{
					auto p_mesh = m_pDataContext->m_VisualPrinting_MeshData.GetMesh(i);

					if (p_mesh)
					{
						QString _file_name = filename + QString().sprintf("_%d", i);

						saveDataAllRedoFile(_file_name, p_mesh);
					}
				}
			}

			file.close();
		}

		filename = save_path + QString("/redo%1").arg(m_id);
		file.setFileName(filename);

		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			for (int i = 0; i < n_mesh; ++i)
			{
				QString _file_name = save_path + QString("/redo%1").arg(m_id) + QString().sprintf("_%d_tvertex", i);

				QFile _file(_file_name);

				if (_file.exists())
				{
					auto p_mesh = new mip::MeshTopology(g_Renderer);
					if (p_mesh)
					{
						QString _file_name = filename + QString().sprintf("_%d", i);

						loadDataAllRedoFile(_file_name, p_mesh);

						m_pDataContext->m_VisualPrinting_MeshData.DeleteMeshTopologyData(i);
						m_pDataContext->m_VisualPrinting_MeshData.InsertMeshToMeshMap(i, p_mesh);
					}
				}
			}
		}

		file.close();
	}

	WIN_MANAGER->renderLater_All();
	WIN_MANAGER->setSaveState(false);

	if (WIN_MANAGER->pVisualPrintMeshWidget->getMainView() != nullptr)
		WIN_MANAGER->pVisualPrintMeshWidget->getMainView()->updateGeometry(true);
}
//////////////////////////////////////////////////////////////////////////////////////////

ActionColorMesh::ActionColorMesh(DataContext* pDataContext, QColor col, muint8 mUID, bool _b_visual_print, QUndoCommand* parent)
	:QUndoCommand(parent),
	m_UID(mUID),
	m_pDataContext(pDataContext)
{
	static int s_id = ACT_ID_MESHLIST_COLOR_CHANGE;

	if (_b_visual_print)
	{
		m_info = m_pDataContext->m_VisualPrinting_MeshData.GetMeshInfo(m_UID);
	}
	else
	{
		m_info = m_pDataContext->m_MeshData.GetMeshInfo(m_UID);
	}


	if (m_info)
	{
		m_color[0] = m_info->color;
	}

	m_color[1] = COLOR(col.red(), col.green(), col.blue());

	m_updateSub = m_info->uid == -1;

	m_id = s_id++;

	m_bVisualPrint = _b_visual_print;
}

ActionColorMesh::~ActionColorMesh()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString str = QString().sprintf("%d", m_id);

		deleteUndoRedoFile(str, &dir);
	}
}

void ActionColorMesh::undo()
{
	m_info->color = m_color[0];

	//if (m_updateSub)
	{
		if (m_bVisualPrint)
		{
			mip::MeshTopology* pMesh = m_pDataContext->m_VisualPrinting_MeshData.GetMesh(m_UID);
			if (pMesh)
			{
				pMesh->m_baseColor = mip::VECTOR4(m_color[0].r / 255.f, m_color[0].g / 255.f, m_color[0].b / 255.f, pMesh->getAlphaVal());
				pMesh->updateColor(pMesh->m_baseColor);

				WIN_MANAGER->buildRenderBufferTopology(pMesh);
			}

			MeshInfo* pInfo = m_pDataContext->m_VisualPrinting_MeshData.GetMeshInfo(m_UID);
			if (pInfo)
			{
				pInfo->color = m_color[0];
			}

			VisualPrintMeshTab* pVisualPrintMeshTab = WIN_MANAGER->GetTab()->getVisualPrintMeshTab();

			if (pVisualPrintMeshTab)
			{
				pVisualPrintMeshTab->UpdateSubWidget(m_UID);
			}

			WIN_MANAGER->updateVisualPrintMeshUI();
		}
		else
		{
			mip::MeshTopology* pMesh = m_pDataContext->m_MeshData.GetMesh(m_UID);

			if (pMesh)
			{
				pMesh->m_baseColor = mip::VECTOR4(m_color[0].r / 255.f, m_color[0].g / 255.f, m_color[0].b / 255.f, pMesh->getAlphaVal());
				pMesh->updateColor(pMesh->m_baseColor);

				WIN_MANAGER->buildRenderBufferTopology(pMesh);
			}

			MeshInfo* pInfo = m_pDataContext->m_MeshData.GetMeshInfo(m_UID);
			if (pInfo)
			{
				pInfo->color = m_color[0];
			}

			MeshTab* tab = WIN_MANAGER->GetTab()->getMeshTab();

			if (tab)
			{
				tab->UpdateSubWidget(m_UID);
			}

			WIN_MANAGER->updateMeshUI();
		}
	}

	//WIN_MANAGER->updateMeshUI(false, m_UID);
	WIN_MANAGER->renderLater_All();
	WIN_MANAGER->setSaveState(false);
	ACTION_MANAGER->SendActionFinished(ACTION_PROCESSING::ACTP_MESH_LAYER_COLOR_CHANGED);
}

void ActionColorMesh::redo()
{
	m_info->color = m_color[1];

	//if (m_updateSub)
	{
		if (m_bVisualPrint)
		{
			mip::MeshTopology* pMesh = m_pDataContext->m_VisualPrinting_MeshData.GetMesh(m_UID);
			if (pMesh)
			{
				pMesh->m_baseColor = mip::VECTOR4(m_color[1].r / 255.f, m_color[1].g / 255.f, m_color[1].b / 255.f, pMesh->getAlphaVal());
				pMesh->updateColor(pMesh->m_baseColor);

				WIN_MANAGER->buildRenderBufferTopology(pMesh);
			}

			MeshInfo* pInfo = m_pDataContext->m_VisualPrinting_MeshData.GetMeshInfo(m_UID);
			if (pInfo)
			{
				pInfo->color = m_color[1];
			}

			VisualPrintMeshTab* pVisualPrintMeshTab = WIN_MANAGER->GetTab()->getVisualPrintMeshTab();
			if (pVisualPrintMeshTab)
			{
				pVisualPrintMeshTab->UpdateSubWidget(m_UID);
			}

			WIN_MANAGER->updateVisualPrintMeshUI();
		}
		else
		{
			mip::MeshTopology* pMesh = m_pDataContext->m_MeshData.GetMesh(m_UID);

			if (pMesh)
			{
				pMesh->m_baseColor = mip::VECTOR4(m_color[1].r / 255.f, m_color[1].g / 255.f, m_color[1].b / 255.f, pMesh->getAlphaVal());
				pMesh->updateColor(pMesh->m_baseColor);

				WIN_MANAGER->buildRenderBufferTopology(pMesh);
			}

			MeshInfo* pInfo = m_pDataContext->m_MeshData.GetMeshInfo(m_UID);
			if (pInfo)
			{
				pInfo->color = m_color[1];
			}

			MeshTab* tab = WIN_MANAGER->GetTab()->getMeshTab();
			if (tab)
			{
				tab->UpdateSubWidget(m_UID);
			}

			WIN_MANAGER->updateMeshUI();
		}
	}


	//WIN_MANAGER->updateMeshUI(false, m_UID);
	WIN_MANAGER->renderLater_All();
	WIN_MANAGER->setSaveState(false);
	ACTION_MANAGER->SendActionFinished(ACTION_PROCESSING::ACTP_MESH_LAYER_COLOR_CHANGED);
}

ActionNameMesh::ActionNameMesh(DataContext* pDataContext, QString newName, muint8 mUID, bool _b_visual_print, QUndoCommand* parent /*= NULL*/)
	:QUndoCommand(parent),
	m_UID(mUID),
	m_bVisualPrint(_b_visual_print),
	m_pDataContext(pDataContext)
{
	static int s_id = ACT_ID_MESHLIST_NAME_CHANGE;

	m_info = m_bVisualPrint ? m_pDataContext->m_VisualPrinting_MeshData.GetMeshInfo(m_UID) : m_pDataContext->m_MeshData.GetMeshInfo(m_UID);

	if (m_info)
		m_name[0] = QString((QChar*)m_info->MeshName);

	m_name[1] = newName;

	m_id = s_id++;
}

ActionNameMesh::~ActionNameMesh()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString str = QString().sprintf("%d", m_id);

		deleteUndoRedoFile(str, &dir);
	}
}

void ActionNameMesh::undo()
{
	if (!m_bVisualPrint)
	{
		m_pDataContext->m_MeshData.SetMeshName(m_name[0], m_UID);

		WIN_MANAGER->updateMeshUI(false, m_UID);
	}
	else
	{
		m_pDataContext->m_VisualPrinting_MeshData.SetMeshName(m_name[0], m_UID);
	}
}

void ActionNameMesh::redo()
{
	if (!m_bVisualPrint)
	{
		m_pDataContext->m_MeshData.SetMeshName(m_name[1], m_UID);

		WIN_MANAGER->updateMeshUI(false, m_UID);
	}
	else
	{
		m_pDataContext->m_VisualPrinting_MeshData.SetMeshName(m_name[1], m_UID);
	}
}

ActionUIDMesh::ActionUIDMesh(DataContext* pDataContext, mint8 oldUID, mint8 newUID, QUndoCommand* parent /*= NULL*/)
	:QUndoCommand(parent),
	m_orgUID(oldUID),
	m_first(true),
	m_pDataContext(pDataContext)
{
	static int s_id = ACT_ID_MESHLIST_UID_CHANGE;

	m_count = abs(m_orgUID - newUID) + 1;

	m_UID = new muint8[m_count];
	memset(m_UID, 0, sizeof(muint8) * m_count);

	m_layerUID = new mint32[m_count];
	memset(m_layerUID, 0, sizeof(mint32) * m_count);

	m_start = m_orgUID < newUID ? m_orgUID : newUID;
	m_end = m_orgUID < newUID ? newUID : m_orgUID;

	m_UID[m_orgUID - m_start] = newUID;

	int index = m_start - 1;

	for (int i = 0; i < m_count; i++)
	{
		MeshInfo* info = m_pDataContext->m_MeshData.GetMeshInfo(m_start + i);

		if (info)
			m_layerUID[i] = info->uid;

		if (i == (m_orgUID - m_start))
			continue;

		for (index = index + 1; index <= m_end; index++)
		{
			if (index != newUID)
				break;
		}

		m_UID[i] = index;


	}

	m_id = s_id++;
}

ActionUIDMesh::~ActionUIDMesh()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString str = QString().sprintf("%d", m_id);

		deleteUndoRedoFile(str, &dir);
	}

	SAFE_DELETES(m_UID);
	SAFE_DELETES(m_layerUID);
}

void ActionUIDMesh::undo()
{
	int count = m_pDataContext->m_MeshData.GetMeshCount();
	m_pDataContext->m_MeshData.ChangeMeshUID(m_UID[m_orgUID - m_start], count, m_layerUID[m_orgUID - m_start]);

	if (m_start == m_orgUID)
	{
		for (int i = m_end; i >= (m_start + 1); i--)
		{
			m_pDataContext->m_MeshData.ChangeMeshUID(m_UID[i - m_start], i, m_layerUID[i - m_start]);
		}
	}
	else
	{
		for (int i = m_start; i < m_end; i++)
		{
			m_pDataContext->m_MeshData.ChangeMeshUID(m_UID[i - m_start], i, m_layerUID[i - m_start]);
		}

	}

	m_pDataContext->m_MeshData.ChangeMeshUID(count, m_orgUID, m_layerUID[m_orgUID - m_start]);
	m_pDataContext->m_MeshData.MoveMeshInfo(m_UID[m_orgUID - m_start], m_orgUID);

	MeshTab* tab = WIN_MANAGER->GetTab()->getMeshTab();
	if (tab)
	{
		for (int i = m_start; i <= m_end; i++)
			tab->ChangeSubUID(i);
	}

	VisualPrintMeshTab* pVisualPrintMeshTab = WIN_MANAGER->GetTab()->getVisualPrintMeshTab();
	if (pVisualPrintMeshTab)
	{
		for (int i = m_start; i <= m_end; i++)
			pVisualPrintMeshTab->ChangeSubUID(i);
	}

	WIN_MANAGER->updateMeshUI();
	WIN_MANAGER->renderLater_All();
}

void ActionUIDMesh::redo()
{
	int count = m_pDataContext->m_MeshData.GetMeshCount();

	m_pDataContext->m_MeshData.ChangeMeshUID(m_orgUID, count, m_layerUID[m_orgUID - m_start]);

	if (m_start == m_orgUID)
	{
		for (int i = m_start + 1; i <= m_end; i++)
		{
			m_pDataContext->m_MeshData.ChangeMeshUID(i, m_UID[i - m_start], m_layerUID[i - m_start]);
		}
	}
	else
	{
		for (int i = m_end - 1; i >= m_start; i--)
		{
			m_pDataContext->m_MeshData.ChangeMeshUID(i, m_UID[i - m_start], m_layerUID[i - m_start]);
		}
	}

	m_pDataContext->m_MeshData.ChangeMeshUID(count, m_UID[m_orgUID - m_start], m_layerUID[m_orgUID - m_start]);
	m_pDataContext->m_MeshData.MoveMeshInfo(m_orgUID, m_UID[m_orgUID - m_start]);

	MeshTab* tab = WIN_MANAGER->GetTab()->getMeshTab();
	if (tab)
	{
		for (int i = m_start; i <= m_end; i++)
			tab->ChangeSubUID(i);
	}

	VisualPrintMeshTab* pVisualPrintMeshTab = WIN_MANAGER->GetTab()->getVisualPrintMeshTab();
	if (pVisualPrintMeshTab)
	{
		for (int i = m_start; i <= m_end; i++)
			pVisualPrintMeshTab->ChangeSubUID(i);
	}


	WIN_MANAGER->updateMeshUI();
	WIN_MANAGER->renderLater_All();
}

ActionConnectMesh::ActionConnectMesh(DataContext* pDataContext, muint32 lUID, QUndoCommand* parent)
	:QUndoCommand(parent),
	m_lUID(lUID),
	m_pDataContext(pDataContext)
{
	static int s_id = ACT_ID_MASKLIST_MESH_CONNECT;

	m_mUID = m_pDataContext->m_MeshData.GetMeshUID(m_lUID);

	m_id = s_id++;
}

ActionConnectMesh::~ActionConnectMesh()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString str = QString().sprintf("%d", m_id);

		deleteUndoRedoFile(str, &dir);
	}
}

void ActionConnectMesh::undo()
{
	MaskInfo* info = m_pDataContext->volume_data.getMaskInfo(m_lUID, true);
	int index = m_pDataContext->volume_data.getMaskIndex(m_lUID);
	if (info)
	{
		info->meshConnected = !(info->meshConnected);

		//connect
		if ((info->meshConnected) && (m_mUID != -1))
		{
			m_pDataContext->m_MeshData.SetConnectMesh(m_mUID, m_lUID);

			MeshTab* tab = WIN_MANAGER->GetTab()->getMeshTab();
			if (tab)
				tab->UpdateSubWidget(m_mUID);

			VisualPrintMeshTab* pVisualPrintMeshTab = WIN_MANAGER->GetTab()->getVisualPrintMeshTab();
			if (pVisualPrintMeshTab)
				pVisualPrintMeshTab->UpdateSubWidget(m_mUID);
		}
		// 
		// 		ROITab2 *tab = WIN_MANAGER->GetTab()->getROITab();
		// 
		// 		if (tab)
		// 			tab->updateROIHeader(L_COL_MESH);
	}
	if (index != -1)
		WIN_MANAGER->updateUI(false, index);
	WIN_MANAGER->renderLater_All();
}

void ActionConnectMesh::redo()
{
	MaskInfo* info = m_pDataContext->volume_data.getMaskInfo(m_lUID, true);
	int index = m_pDataContext->volume_data.getMaskIndex(m_lUID);
	if (info)
	{
		info->meshConnected = !(info->meshConnected);

		//disconnect
		if ((!info->meshConnected) && (m_mUID != -1))
		{
			m_pDataContext->m_MeshData.SetDisconnectMesh(m_lUID);

			MeshTab* tab = WIN_MANAGER->GetTab()->getMeshTab();
			if (tab)
				tab->UpdateSubWidget(m_mUID);

			VisualPrintMeshTab* pVisualPrintMeshTab = WIN_MANAGER->GetTab()->getVisualPrintMeshTab();
			if (pVisualPrintMeshTab)
				pVisualPrintMeshTab->UpdateSubWidget(m_mUID);
		}


		// 		{
		// 			ROITab2 *tab = WIN_MANAGER->GetTab()->getROITab();
		// 
		// 			if (tab)
		// 				tab->updateROIHeader(L_COL_MESH);
		// 		}
	}
	if (index != -1)
		WIN_MANAGER->updateUI(false, index);
	//	WIN_MANAGER->updateUI(false, )
	WIN_MANAGER->renderLater_All();
}

ActionConnectMeshes::ActionConnectMeshes(DataContext* pDataContext, bool show, QUndoCommand* parent)
	:QUndoCommand(parent),
	m_show(show),
	m_pDataContext(pDataContext)
{
	static int s_id = ACT_ID_MASKLIST_MESHES_CONNECT;

	int count = m_pDataContext->volume_data.getMaskInfoListCnt();

	MaskInfo* info;

	for (int i = 0; i < count; i++)
	{
		info = m_pDataContext->volume_data.getMaskInfo(i);

		if (info)
		{
			m_lUID.push_back(info->uid);

			mint8 meshUID = m_pDataContext->m_MeshData.GetMeshUID(info->uid);
			m_mUID.push_back(meshUID);
		}
	}

	m_id = s_id++;
}

ActionConnectMeshes::ActionConnectMeshes(DataContext* pDataContext, std::vector<muint32> lUID, bool Conn, QUndoCommand* parent) :
	QUndoCommand(parent),
	m_show(Conn),
	m_pDataContext(pDataContext)
{
	static int s_id = ACT_ID_MASKLIST_MESHES_CONNECT;

	for (int i = 0; i < lUID.size(); i++)
	{
		int uid = lUID.at(i);
		m_lUID.push_back(uid);
		mint8 meshUID = m_pDataContext->m_MeshData.GetMeshUID(uid);
		m_mUID.push_back(meshUID);
	}

	m_id = s_id++;
}

ActionConnectMeshes::~ActionConnectMeshes()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString str = QString().sprintf("%d", m_id);

		deleteUndoRedoFile(str, &dir);
	}
}

void ActionConnectMeshes::undo()
{
	for (int i = 0; i < m_lUID.size(); i++)
	{
		int lUID = m_lUID.at(i);
		int mUID = m_mUID.at(i);
		MaskInfo* info = m_pDataContext->volume_data.getMaskInfo(lUID, true);
		int index = m_pDataContext->volume_data.getMaskIndex(lUID);
		if (info)
		{
			info->meshConnected = !(m_show);

			//connect
			if (info->meshConnected && (mUID != -1))
			{
				m_pDataContext->m_MeshData.SetConnectMesh(mUID, lUID);
			}
		}
	}

	// 	ROITab2 *tab = WIN_MANAGER->GetTab()->getROITab();
	// 
	// 	if (tab)
	// 		tab->updateROIHeader(L_COL_MESH);

	WIN_MANAGER->updateUI();
	WIN_MANAGER->renderLater_All();

}

void ActionConnectMeshes::redo()
{
	for (int i = 0; i < m_lUID.size(); i++)
	{
		int lUID = m_lUID.at(i);
		int mUID = m_mUID.at(i);
		MaskInfo* info = m_pDataContext->volume_data.getMaskInfo(lUID, true);
		int index = m_pDataContext->volume_data.getMaskIndex(lUID);
		if (info)
		{
			info->meshConnected = m_show;

			//disconnect
			if (!(info->meshConnected) && (mUID != -1))
				m_pDataContext->m_MeshData.SetDisconnectMesh(lUID);

		}
	}

	// 	ROITab2 *tab = WIN_MANAGER->GetTab()->getROITab();
	// 
	// 	if (tab)
	// 		tab->updateROIHeader(L_COL_MESH);

	WIN_MANAGER->updateUI();
	WIN_MANAGER->renderLater_All();
}

ActionScaleMesh::ActionScaleMesh(DataContext* pDataContext, muint8 uid, float* scal, QUndoCommand* parent)
	:QUndoCommand(parent),
	m_UID(uid),
	m_center(-1),
	m_count(-1),
	m_pDataContext(pDataContext)
{
	static int s_id = ACT_ID_MESHLIST_SCALE_CHANGE;

	for (int i = 0; i < 3; i++)
		m_scal[i] = scal[i];


	m_mesh = m_pDataContext->m_MeshData.GetMesh(m_UID);

	if (m_mesh)
	{
		m_count = m_mesh->m_verts.size();
		m_center = m_mesh->m_verts.size() / 2;
	}

	m_id = s_id++;
}

ActionScaleMesh::~ActionScaleMesh()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString str = QString().sprintf("%d", m_id);

		deleteUndoRedoFile(str, &dir);
	}
}

void ActionScaleMesh::undo()
{
	bool res = false;

	if (m_center == -1 ||
		m_count == -1)
		return;

	QDir dir(STRING_MANAGER->cacheFilePath);

	if (dir.exists())
	{
		std::vector<mip::VECTOR3>* verts = &(m_mesh->m_verts);
		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);

		QFile file(filename);

		if ((file.exists()) && file.open(QIODevice::ReadOnly))
		{
			res = true;
			WIN_MANAGER->makeCurrent();

			mip::VECTOR3* dt = m_mesh->m_verts.data();

			file.read((char*)(dt), sizeof(mip::VECTOR3) * m_center);
			dt += m_center;
			file.read((char*)(dt), sizeof(mip::VECTOR3) * (m_count - m_center));
			file.close();

			m_mesh->buildRenderBufferTopology();
			WIN_MANAGER->doneCurrent();
		}
	}

	if (res)
	{
		MeshTab* tab = WIN_MANAGER->GetTab()->getMeshTab();
		if (tab)
			tab->UpdateSubWidget(m_UID);

		VisualPrintMeshTab* pVisualPrintMeshTab = WIN_MANAGER->GetTab()->getVisualPrintMeshTab();
		if (pVisualPrintMeshTab)
			pVisualPrintMeshTab->UpdateSubWidget(m_UID);

		WIN_MANAGER->setSaveState(false);
		WIN_MANAGER->renderLater_All();
	}
}

void ActionScaleMesh::redo()
{
	bool res = false;

	if (m_center == -1 ||
		m_count == -1)
		return;


	QDir dir(STRING_MANAGER->cacheFilePath);

	if (dir.exists())
	{
		std::vector<mip::VECTOR3>* verts = &(m_mesh->m_verts);
		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);

		QFile file(filename);
		WIN_MANAGER->makeCurrent();

		mip::VECTOR3* dt = m_mesh->m_verts.data();
		if ((!file.exists()) && file.open(QIODevice::WriteOnly))
		{
			res = true;

			file.write((char*)(dt), sizeof(mip::VECTOR3) * m_center);
			dt += m_center;
			file.write((char*)(dt), sizeof(mip::VECTOR3) * (m_count - m_center));
			file.close();

			const mip::VECTOR3 centor = (*verts)[m_center];

			for (int i = 0; i < m_count; i++)
			{
				mip::VECTOR3 vecc = centor - (*verts)[i];

				vecc.x *= m_scal[0];
				vecc.y *= m_scal[1];
				vecc.z *= m_scal[2];

				mip::VECTOR3 vecb = centor - vecc;

				(*verts)[i] = vecb;
			}

			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));

			if (file.open(QIODevice::WriteOnly))
			{
				dt = m_mesh->m_verts.data();

				file.write((char*)(dt), sizeof(mip::VECTOR3) * m_center);
				dt += m_center;
				file.write((char*)(dt), sizeof(mip::VECTOR3) * (m_count - m_center));
				file.close();
			}
		}
		else
		{
			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));

			if (file.exists() && file.open(QIODevice::ReadOnly))
			{
				res = true;

				file.read((char*)(dt), sizeof(mip::VECTOR3) * m_center);
				dt += m_center;
				file.read((char*)(dt), sizeof(mip::VECTOR3) * (m_count - m_center));
				file.close();
			}
		}
		if (res)
			m_mesh->buildRenderBufferTopology();

		WIN_MANAGER->doneCurrent();

	}

	if (res)
	{
		MeshTab* tab = WIN_MANAGER->GetTab()->getMeshTab();
		if (tab)
			tab->UpdateSubWidget(m_UID);

		VisualPrintMeshTab* pVisualPrintMeshTab = WIN_MANAGER->GetTab()->getVisualPrintMeshTab();
		if (pVisualPrintMeshTab)
			pVisualPrintMeshTab->UpdateSubWidget(m_UID);

		WIN_MANAGER->setSaveState(false);
		WIN_MANAGER->renderLater_All();
	}
}

WorkMeshAlign::WorkMeshAlign(mip::MeshTopology* newMesh, mip::MeshTopology* oldMesh,
	int Stype /*= 0*/, int Dtype/*=0*/) :
	m_new(newMesh),
	m_old(oldMesh),
	m_stype(Stype),
	m_dtype(Dtype)
{

}

void WorkMeshAlign::setProgressValue(int value, bool init /*= false*/)
{
	static int val = 0;

	if (init)
	{
		val = 0;
		emit progress(val);
		return;
	}

	if (100 <= val)
		return;

	if ((val + 10) <= value)
	{
		val = value;
		emit progress(val);
	}
}

void WorkMeshAlign::threadRun()
{
	setProgressValue(0, true);

	const int size = m_old->m_verts.size();
	const int centerIndex = size / 2;

	mip::VECTOR3 basePoint;
	mip::VECTOR3 worldPoint;

	if (m_stype == 0) //base point (0,0,0)
		basePoint = mip::VECTOR3(0, 0, 0);
	else if (m_stype == 1) //center point
	{
		basePoint = m_old->m_verts.at(centerIndex);
	}
	//...etc

	if (m_dtype == 0) //world center point
	{
		//MEVolumeView* view = WIN_MANAGER->mainMeshWidget->getMainView();
		//MEVolumeView* view = WIN_MANAGER->pVisualPrintMeshWidget->getMainView();
		worldPoint = MESH_RENDER_MANAGER->getWorldCenter(false);
	}
	//...etc

	mip::VECTOR3 diff = worldPoint - basePoint;

	for (int i = 0; i < size; i++)
	{
		mip::VECTOR3 vec = m_old->m_verts.at(i) + diff;

		m_new->m_verts.push_back(vec);

		setProgressValue(((float)i / size) * 100);
	}

	emit finished();
}

ActionAlignMesh::ActionAlignMesh(DataContext* pDataContext, muint8 uid, mip::MeshTopology* newMesh, QUndoCommand* parent)
	:m_UID(uid),
	m_mesh(newMesh),
	m_center(-1),
	m_count(-1),
	m_first(true),
	m_pDataContext(pDataContext)
{
	static int s_id = ACT_ID_MESHLIST_ALIGN_UPDATE;

	m_id = s_id++;
}

ActionAlignMesh::~ActionAlignMesh()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString str = QString().sprintf("%d", m_id);

		deleteUndoRedoFile(str, &dir);
	}
}

void ActionAlignMesh::undo()
{
	bool res = false;

	if (m_center == -1 ||
		m_count == -1)
		return;

	QDir dir(STRING_MANAGER->cacheFilePath);

	if (dir.exists())
	{
		std::vector<mip::VECTOR3>* verts = &(m_mesh->m_verts);
		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);

		QFile file(filename);

		if ((file.exists()) && file.open(QIODevice::ReadOnly))
		{
			res = true;

			mip::VECTOR3* dt = m_mesh->m_verts.data();

			file.read((char*)(dt), sizeof(mip::VECTOR3) * m_center);
			dt += m_center;
			file.read((char*)(dt), sizeof(mip::VECTOR3) * (m_count - m_center));
			file.close();

			WIN_MANAGER->makeCurrent();
			m_mesh->buildRenderBufferTopology();
			WIN_MANAGER->doneCurrent();
		}
	}

	if (res)
	{
		MeshTab* tab = WIN_MANAGER->GetTab()->getMeshTab();
		if (tab)
			tab->UpdateSubWidget(m_UID);

		VisualPrintMeshTab* pVisualPrintMeshTab = WIN_MANAGER->GetTab()->getVisualPrintMeshTab();
		if (pVisualPrintMeshTab)
			pVisualPrintMeshTab->UpdateSubWidget(m_UID);

		WIN_MANAGER->setSaveState(false);
		WIN_MANAGER->renderLater_All();
	}

}

void ActionAlignMesh::redo()
{
	bool res = false;

	if (m_first)
	{
		QDir dir(STRING_MANAGER->cacheFilePath);

		if (dir.exists())
		{
			QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);

			QFile file(filename);

			if (file.open(QIODevice::WriteOnly))
			{
				m_count = m_mesh->m_verts.size();
				m_center = m_count / 2;

				m_first = false;

				mip::VECTOR3* dt = m_mesh->m_verts.data();

				file.write((char*)(dt), sizeof(mip::VECTOR3) * m_center);
				dt += m_center;
				file.write((char*)(dt), sizeof(mip::VECTOR3) * (m_count - m_center));
				file.close();
			}

			SAFE_DELETE(m_mesh);
			m_mesh = m_pDataContext->m_MeshData.GetMesh(m_UID);

			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));

			if (file.open(QIODevice::WriteOnly))
			{
				mip::VECTOR3* dt = m_mesh->m_verts.data();

				file.write((char*)(dt), sizeof(mip::VECTOR3) * m_center);
				dt += m_center;
				file.write((char*)(dt), sizeof(mip::VECTOR3) * (m_count - m_center));
				file.close();
			}
		}
	}

	QDir dir(STRING_MANAGER->cacheFilePath);

	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);

		QFile file(filename);

		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			res = true;
			mip::VECTOR3* dt = m_mesh->m_verts.data();

			file.read((char*)(dt), sizeof(mip::VECTOR3) * m_center);
			dt += m_center;
			file.read((char*)(dt), sizeof(mip::VECTOR3) * (m_count - m_center));
			file.close();

			WIN_MANAGER->makeCurrent();
			m_mesh->buildRenderBufferTopology();
			WIN_MANAGER->doneCurrent();

		}
	}


	if (res)
	{
		MeshTab* tab = WIN_MANAGER->GetTab()->getMeshTab();
		if (tab)
			tab->UpdateSubWidget(m_UID);

		VisualPrintMeshTab* pVisualPrintMeshTab = WIN_MANAGER->GetTab()->getVisualPrintMeshTab();
		if (pVisualPrintMeshTab)
			pVisualPrintMeshTab->UpdateSubWidget(m_UID);

		WIN_MANAGER->setSaveState(false);
		WIN_MANAGER->renderLater_All();
	}
}

ActionTransMesh::ActionTransMesh(DataContext* pDataContext, muint8 uid, mip::VECTOR3 trans, QUndoCommand* parent) :
	QUndoCommand(parent),
	m_UID(uid),
	m_center(-1),
	m_count(-1),
	m_trans(trans),
	m_pDataContext(pDataContext)
{
	static int s_id = ACT_ID_MESHLIST_TRANS_CHANGE;

	m_mesh = m_pDataContext->m_MeshData.GetMesh(m_UID);

	if (m_mesh)
	{
		m_count = m_mesh->m_verts.size();
		m_center = m_mesh->m_verts.size() / 2;
	}

	m_id = s_id++;
}

ActionTransMesh::~ActionTransMesh()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString str = QString().sprintf("%d", m_id);

		deleteUndoRedoFile(str, &dir);
	}
}

void ActionTransMesh::undo()
{
	bool res = false;

	if (m_center == -1 ||
		m_count == -1)
		return;

	QDir dir(STRING_MANAGER->cacheFilePath);

	if (dir.exists())
	{
		std::vector<mip::VECTOR3>* verts = &(m_mesh->m_verts);
		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);

		QFile file(filename);

		if ((file.exists()) && file.open(QIODevice::ReadOnly))
		{
			res = true;
			WIN_MANAGER->makeCurrent();

			mip::VECTOR3* dt = m_mesh->m_verts.data();

			file.read((char*)(dt), sizeof(mip::VECTOR3) * m_center);
			dt += m_center;
			file.read((char*)(dt), sizeof(mip::VECTOR3) * (m_count - m_center));
			file.close();

			m_mesh->buildRenderBufferTopology();
			WIN_MANAGER->doneCurrent();
		}
	}

	if (res)
	{
		MeshTab* tab = WIN_MANAGER->GetTab()->getMeshTab();
		if (tab)
			tab->UpdateSubWidget(m_UID);

		VisualPrintMeshTab* pVisualPrintMeshTab = WIN_MANAGER->GetTab()->getVisualPrintMeshTab();
		if (pVisualPrintMeshTab)
			pVisualPrintMeshTab->UpdateSubWidget(m_UID);

		WIN_MANAGER->setSaveState(false);
		WIN_MANAGER->renderLater_All();
	}
}

void ActionTransMesh::redo()
{
	bool res = false;

	if (m_center == -1 ||
		m_count == -1)
		return;


	QDir dir(STRING_MANAGER->cacheFilePath);

	if (dir.exists())
	{
		std::vector<mip::VECTOR3>* verts = &(m_mesh->m_verts);
		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);

		QFile file(filename);
		WIN_MANAGER->makeCurrent();

		mip::VECTOR3* dt = m_mesh->m_verts.data();
		if ((!file.exists()) && file.open(QIODevice::WriteOnly))
		{
			res = true;

			file.write((char*)(dt), sizeof(mip::VECTOR3) * m_center);
			dt += m_center;
			file.write((char*)(dt), sizeof(mip::VECTOR3) * (m_count - m_center));
			file.close();

			//	mip::mesh::translateMesh(m_trans, (*verts));

			for (int i = 0; i < m_count; i++)
				(*verts)[i] += m_trans;

			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));

			if (file.open(QIODevice::WriteOnly))
			{
				dt = m_mesh->m_verts.data();

				file.write((char*)(dt), sizeof(mip::VECTOR3) * m_center);
				dt += m_center;
				file.write((char*)(dt), sizeof(mip::VECTOR3) * (m_count - m_center));
				file.close();
			}
		}
		else
		{
			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));

			if (file.exists() && file.open(QIODevice::ReadOnly))
			{
				res = true;

				file.read((char*)(dt), sizeof(mip::VECTOR3) * m_center);
				dt += m_center;
				file.read((char*)(dt), sizeof(mip::VECTOR3) * (m_count - m_center));
				file.close();
			}
		}


		if (res)
			m_mesh->buildRenderBufferTopology();

		WIN_MANAGER->doneCurrent();
	}

	if (res)
	{
		//MeshTab* tab = WIN_MANAGER->GetTab()->getMeshTab();
		//if (tab)
		//	tab->UpdateSubWidget(m_UID);

		//VisualPrintMeshTab* pVisualPrintMeshTab = WIN_MANAGER->GetTab()->getVisualPrintMeshTab();
		//if (pVisualPrintMeshTab)
		//	pVisualPrintMeshTab->UpdateSubWidget(m_UID);

		WIN_MANAGER->setSaveState(false);
		WIN_MANAGER->renderLater_All();
	}
}

ActionRotateMesh::ActionRotateMesh(DataContext* pDataContext, muint8 uid, float* fRot, QUndoCommand* parent /*= NULL*/)
	:QUndoCommand(parent),
	m_UID(uid),
	m_pDataContext(pDataContext)
{
	static int s_id = ACT_ID_MESHLIST_ROT_CHANGE;

	m_mesh = m_pDataContext->m_MeshData.GetMesh(m_UID);

	if (m_mesh)
	{
		m_count = m_mesh->m_verts.size();
		m_center = m_mesh->m_verts.size() / 2;
	}

	//pitch, yaw, roll
	//y, z, x
	m_rotate.set(fRot[2], fRot[1], fRot[0]);

	m_id = s_id++;
}

ActionRotateMesh::~ActionRotateMesh()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString str = QString().sprintf("%d", m_id);

		deleteUndoRedoFile(str, &dir);
	}
}

void ActionRotateMesh::undo()
{
	bool res = false;

	if (m_center == -1 ||
		m_count == -1)
		return;

	QDir dir(STRING_MANAGER->cacheFilePath);

	if (dir.exists())
	{
		std::vector<mip::VECTOR3>* verts = &(m_mesh->m_verts);
		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);

		QFile file(filename);

		if ((file.exists()) && file.open(QIODevice::ReadOnly))
		{
			res = true;
			WIN_MANAGER->makeCurrent();

			mip::VECTOR3* dt = m_mesh->m_verts.data();

			file.read((char*)(dt), sizeof(mip::VECTOR3) * m_center);
			dt += m_center;
			file.read((char*)(dt), sizeof(mip::VECTOR3) * (m_count - m_center));
			file.close();

			m_mesh->buildRenderBufferTopology();
			WIN_MANAGER->doneCurrent();
		}
	}

	if (res)
	{
		MeshTab* tab = WIN_MANAGER->GetTab()->getMeshTab();
		if (tab)
			tab->UpdateSubWidget(m_UID);

		VisualPrintMeshTab* pVisualPrintMeshTab = WIN_MANAGER->GetTab()->getVisualPrintMeshTab();
		if (pVisualPrintMeshTab)
			pVisualPrintMeshTab->UpdateSubWidget(m_UID);

		WIN_MANAGER->setSaveState(false);
		WIN_MANAGER->renderLater_All();
	}
}

void ActionRotateMesh::redo()
{
	bool res = false;

	if (m_center == -1 ||
		m_count == -1)
		return;


	QDir dir(STRING_MANAGER->cacheFilePath);

	if (dir.exists())
	{
		std::vector<mip::VECTOR3>* verts = &(m_mesh->m_verts);
		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);

		QFile file(filename);
		WIN_MANAGER->makeCurrent();

		mip::VECTOR3* dt = m_mesh->m_verts.data();
		if ((!file.exists()) && file.open(QIODevice::WriteOnly))
		{
#ifdef MESH_TEST
			res = true;

			file.write((char*)(dt), sizeof(mip::VECTOR3) * m_center);
			dt += m_center;
			file.write((char*)(dt), sizeof(mip::VECTOR3) * (m_count - m_center));
			file.close();


			mip::TRANSFORM tr;

			tr.zero();

			tr.set(m_rotate);
			mip::VECTOR3 oldAxis, newAxis;

			oldAxis = verts->at(m_center);

			mip::mesh_control::transformMesh(tr.getMatrix(), (*verts));

			newAxis = verts->at(m_center);

			oldAxis -= newAxis;

			mip::mesh_control::translateMesh(oldAxis, (*verts));

			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));

			if (file.open(QIODevice::WriteOnly))
			{
				dt = m_mesh->m_verts.data();

				file.write((char*)(dt), sizeof(mip::VECTOR3) * m_center);
				dt += m_center;
				file.write((char*)(dt), sizeof(mip::VECTOR3) * (m_count - m_center));
				file.close();
			}
#endif
		}
		else
		{
			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));

			if (file.exists() && file.open(QIODevice::ReadOnly))
			{
				res = true;

				file.read((char*)(dt), sizeof(mip::VECTOR3) * m_center);
				dt += m_center;
				file.read((char*)(dt), sizeof(mip::VECTOR3) * (m_count - m_center));
				file.close();
			}
		}
		if (res)
			m_mesh->buildRenderBufferTopology();

		WIN_MANAGER->doneCurrent();

	}

	if (res)
	{
		MeshTab* tab = WIN_MANAGER->GetTab()->getMeshTab();
		if (tab)
			tab->UpdateSubWidget(m_UID);

		VisualPrintMeshTab* pVisualPrintMeshTab = WIN_MANAGER->GetTab()->getVisualPrintMeshTab();
		if (pVisualPrintMeshTab)
			pVisualPrintMeshTab->UpdateSubWidget(m_UID);

		WIN_MANAGER->setSaveState(false);
		WIN_MANAGER->renderLater_All();
	}
}

ActionSplitMesh::ActionSplitMesh(DataContext* pDataContext, mint8 mUID, mip::MeshTopology* m, QUndoCommand* parent /*= NULL*/)
	:QUndoCommand(parent),
	m_UID(mUID),
	m_mesh(m),
	m_Smesh(NULL),
	m_first(true),
	m_pDataContext(pDataContext)
{
	static int s_id = ACT_ID_MESHLIST_SPLIT;

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			m_Points[i][j] = -1;
			m_SPoints[j] = -1;
		}
	}

	m_id = s_id++;
}

ActionSplitMesh::~ActionSplitMesh()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString str = QString().sprintf("%d", m_id);

		deleteUndoRedoFile(str, &dir);
	}
}

void ActionSplitMesh::undo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);

	m_pDataContext->m_MeshData.DeleteMeshLayer(m_SUID);

	MeshInfo* info = nullptr;
	for (int i = m_SUID; i < m_pDataContext->m_MeshData.GetMeshCount(); i++)
	{
		info = m_pDataContext->m_MeshData.GetMeshInfo(i);

		if (info)
			m_pDataContext->m_MeshData.ChangeMeshUID(i, i - 1, info->uid);
	}

	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);

		QFile file(filename);

		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			m_mesh = new mip::MeshTopology(g_Renderer);
			if (m_mesh)
			{
				m_mesh->m_verts.reserve(m_Points[0][0]);
				m_mesh->m_tris.reserve(m_Points[0][1]);
				m_mesh->m_normals.reserve(m_Points[0][2]);

				m_mesh->m_verts.assign(m_Points[0][0], mip::VECTOR3());
				m_mesh->m_tris.assign(m_Points[0][1], 0);
				m_mesh->m_normals.assign(m_Points[0][2], mip::VECTOR3());

				mip::VECTOR3* dt = m_mesh->m_verts.data();
				muint32* dtTri = m_mesh->m_tris.data();
				mip::VECTOR3* dtNor = m_mesh->m_normals.data();

				file.read((char*)dt, sizeof(mip::VECTOR3) * m_Points[0][0]);
				file.read((char*)dtTri, sizeof(muint32) * m_Points[0][1]);
				file.read((char*)dtNor, sizeof(mip::VECTOR3) * m_Points[0][2]);

				WIN_MANAGER->makeCurrent();
				m_mesh->buildRenderBufferTopology();
				WIN_MANAGER->doneCurrent();
			}

			file.close();
		}

		if (m_mesh)
		{
			m_pDataContext->m_MeshData.DeleteMeshTopologyData(m_UID);
			m_pDataContext->m_MeshData.InsertMesh(m_UID, m_mesh);
			m_mesh = NULL;
		}
	}

	WIN_MANAGER->updateMeshUI();
	WIN_MANAGER->renderLater_All();
	WIN_MANAGER->setSaveState(false);
}

void ActionSplitMesh::redo()
{
	if (m_first)
	{
		m_pDataContext->m_MeshData.CreateMeshInfo();
		m_SUID = m_pDataContext->m_MeshData.GetCurrentMeshIndex();
		QString strName = "Piece of ";
		strName.append(m_pDataContext->m_MeshData.GetMeshName(m_UID));
		m_pDataContext->m_MeshData.SetMeshName(strName, m_SUID);

		MeshInfo* info = m_pDataContext->m_MeshData.GetMeshInfo(m_SUID);

		if (info)
			m_info = (*info);
	}
	else
		m_pDataContext->m_MeshData.AddMeshInfo(m_SUID, m_info);

	if (!m_first)
	{
		MeshInfo* info = nullptr;
		for (int i = m_pDataContext->m_MeshData.GetMeshCount() - 1; i >= m_SUID; i--)
		{
			info = m_pDataContext->m_MeshData.GetMeshInfo(i);

			if (info)
				m_pDataContext->m_MeshData.ChangeMeshUID(i, i + 1, info->uid);

		}
	}
	else
		m_first = false;

	QDir dir(STRING_MANAGER->cacheFilePath);

	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);

		QFile file(filename);

		if (!file.exists() && file.open(QIODevice::WriteOnly))
		{
			double* v1, * v2, * v3;
			mip::MeshTopology* m = nullptr;

			m = m_pDataContext->m_MeshData.GetMesh(m_UID);
			if (m)
			{
				m_Points[0][0] = m->m_verts.size();
				m_Points[0][1] = m->m_tris.size();
				m_Points[0][2] = m->m_normals.size();

				mip::VECTOR3* dt = m->m_verts.data();
				muint32* dtTri = m->m_tris.data();
				mip::VECTOR3* dtNor = m->m_normals.data();

				file.write((char*)dt, sizeof(mip::VECTOR3) * (m_Points[0][0]));
				file.write((char*)dtTri, sizeof(muint32) * (m_Points[0][1]));
				file.write((char*)dtNor, sizeof(mip::VECTOR3) * (m_Points[0][2]));
			}
			file.close();
		}


		file.setFileName(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));

		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			//////////////////////////////////////////////////
			////////////USE MEMBER MESH///////////////////////
			//////////////////////////////////////////////////
			m_mesh = new mip::MeshTopology(g_Renderer);
			if (m_mesh)
			{
				m_mesh->m_verts.reserve(m_Points[1][0]);
				m_mesh->m_tris.reserve(m_Points[1][1]);
				m_mesh->m_normals.reserve(m_Points[1][2]);

				m_mesh->m_verts.assign(m_Points[1][0], mip::VECTOR3());
				m_mesh->m_tris.assign(m_Points[1][1], 0);
				m_mesh->m_normals.assign(m_Points[1][2], mip::VECTOR3());

				mip::VECTOR3* dt = m_mesh->m_verts.data();
				muint32* dtTri = m_mesh->m_tris.data();
				mip::VECTOR3* dtNor = m_mesh->m_normals.data();

				file.read((char*)dt, sizeof(mip::VECTOR3) * m_Points[1][0]);
				file.read((char*)dtTri, sizeof(muint32) * m_Points[1][1]);
				file.read((char*)dtNor, sizeof(mip::VECTOR3) * m_Points[1][2]);

				WIN_MANAGER->makeCurrent();

				m_mesh->buildRenderBufferTopology();
				WIN_MANAGER->doneCurrent();
			}

			m_Smesh = new mip::MeshTopology(g_Renderer);

			if (m_Smesh)
			{
				m_Smesh->m_verts.reserve(m_SPoints[0]);
				m_Smesh->m_tris.reserve(m_SPoints[1]);
				m_Smesh->m_normals.reserve(m_SPoints[2]);

				m_Smesh->m_verts.assign(m_SPoints[0], mip::VECTOR3());
				m_Smesh->m_tris.assign(m_SPoints[1], 0);
				m_Smesh->m_normals.assign(m_SPoints[2], mip::VECTOR3());

				mip::VECTOR3* dt = m_Smesh->m_verts.data();
				muint32* dtTri = m_Smesh->m_tris.data();
				mip::VECTOR3* dtNor = m_Smesh->m_normals.data();

				file.read((char*)dt, sizeof(mip::VECTOR3) * m_SPoints[0]);
				file.read((char*)dtTri, sizeof(muint32) * m_SPoints[1]);
				file.read((char*)dtNor, sizeof(mip::VECTOR3) * m_SPoints[2]);

				WIN_MANAGER->makeCurrent();
				m_Smesh->buildRenderBufferTopology();
				WIN_MANAGER->doneCurrent();
			}

		}
		else if (file.open(QIODevice::WriteOnly))
		{
			mip::VECTOR3* dt = m_mesh->m_verts.data();
			muint32* dtTri = m_mesh->m_tris.data();
			mip::VECTOR3* dtNor = m_mesh->m_normals.data();
			if (m_mesh)
			{
				m_Points[1][0] = m_mesh->m_verts.size();
				m_Points[1][1] = m_mesh->m_tris.size();
				m_Points[1][2] = m_mesh->m_normals.size();

				file.write((char*)dt, sizeof(mip::VECTOR3) * (m_Points[1][0]));
				file.write((char*)dtTri, sizeof(muint32) * (m_Points[1][1]));
				file.write((char*)dtNor, sizeof(mip::VECTOR3) * (m_Points[1][2]));
			}

			QString strFile = STRING_MANAGER->cacheFilePath + QString("/%1_Split").arg(QString::number(m_UID));

			QFile spFile(strFile);

			if (spFile.open(QIODevice::ReadOnly))
			{
				m_Smesh = new mip::MeshTopology(g_Renderer);

				int count = 0;

				spFile.read((char*)&count, sizeof(int));
				m_SPoints[0] = count;
				m_Smesh->m_verts.reserve(m_SPoints[0]);
				m_Smesh->m_verts.assign(m_SPoints[0], mip::VECTOR3());
				dt = m_Smesh->m_verts.data();
				spFile.read((char*)dt, sizeof(mip::VECTOR3) * m_SPoints[0]);
				count = 0;

				spFile.read((char*)&count, sizeof(int));
				m_SPoints[1] = count;
				m_Smesh->m_tris.reserve(m_SPoints[1]);
				m_Smesh->m_tris.assign(m_SPoints[1], 0);
				dtTri = m_Smesh->m_tris.data();
				spFile.read((char*)dtTri, sizeof(muint32) * m_SPoints[1]);
				count = 0;

				spFile.read((char*)&count, sizeof(int));
				m_SPoints[2] = count;
				m_Smesh->m_normals.reserve(m_SPoints[1]);
				m_Smesh->m_normals.assign(m_SPoints[1], mip::VECTOR3());
				dtNor = m_Smesh->m_normals.data();
				spFile.read((char*)dtNor, sizeof(mip::VECTOR3) * m_SPoints[2]);

				spFile.close();
			}
			spFile.remove();

			file.write((char*)dt, sizeof(mip::VECTOR3) * (m_SPoints[0]));
			file.write((char*)dtTri, sizeof(muint32) * (m_SPoints[1]));
			file.write((char*)dtNor, sizeof(mip::VECTOR3) * (m_SPoints[2]));
		}

		file.close();

		if (m_mesh)
		{
			m_pDataContext->m_MeshData.DeleteMeshTopologyData(m_UID);
			m_pDataContext->m_MeshData.InsertMesh(m_UID, m_mesh);
			m_mesh = NULL;
		}
		if (m_Smesh)
		{
			m_pDataContext->m_MeshData.InsertMesh(m_SUID, m_Smesh);
			m_Smesh = NULL;
		}
	}

	WIN_MANAGER->updateMeshUI();
	WIN_MANAGER->renderLater_All();
	WIN_MANAGER->setSaveState(false);
}

WorkMeshRemesh::WorkMeshRemesh(MESH_REMESH_TYPE type, float offset, DataContext* pDataContext, MEVolumeView* pViewer)
	:
	_type(type),
	_mesh2(nullptr),
	_offset(offset),
	_SmoothMethod(SMM_LAPLACIAN),
	_ReduceMethod(RM_PERCENTAGE),
	_ReducePBChecked(true),
	_edgeLength(1.0),
	m_pDataContext(pDataContext),
	m_pViewer(pViewer)
{
	if (_type == MESH_HOLLOW)
	{
		SAFE_DELETE(_mesh2);
		_mesh2 = new mip::MeshTopology(g_Renderer);
	}
}

void WorkMeshRemesh::updateProgress(float val, void* dt)
{
	WorkMeshRemesh* worker = (WorkMeshRemesh*)dt;
	if (!worker)
	{
		return;
	}
	//worker->setProgressValue(val);

	// 200924 허 건 대리
	//worker->m_ProgressTotal = int((float)val / (float)worker->m_pckMesh) + worker->m_countMesh * worker->m_ProgressInterval;
	worker->m_ProgressTotal += val * worker->m_ProgressInterval * 0.01f / (float)worker->m_pckMesh;
	worker->m_ProgressValue += val * 0.6f;

	if (worker->m_ProgressTotal > 100.f) worker->m_ProgressTotal = 100.f;
	if (worker->m_ProgressValue > 100.f) worker->m_ProgressValue = 100.f;

	QString		name = worker->m_FileName + QString().sprintf("(%d%)", (int)worker->m_ProgressValue);

	emit worker->sig_updateProgress((int)worker->m_ProgressTotal, name);
}

void WorkMeshRemesh::setProgressValue(int value, bool init)
{
	static int val = 0;

	if (init)
	{
		val = 0;
		emit progress(val);
		return;
	}

	if (100 <= val)
		return;

	if ((val + 10) <= value)
	{
		val = value;
		emit progress(val);
	}
}

MESH_WORK_MODE WorkMeshRemesh::getMeshWorkMode()
{
	MESH_WORK_MODE mode = MESH_WORK_MODE::MESH_WORK_NONE;

	switch (_type)
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

	return mode;
}

void WorkMeshRemesh::threadRun()
{
	setProgressValue(0, true);

#ifdef MESH_TEST

	//if (WIN_MANAGER->mainMeshWidget)
	//{
	//	auto p_mesh_view = WIN_MANAGER->mainMeshWidget->getMainView();

	//	if (p_mesh_view)
	//	{
	//		//p_mesh_view->clearPlaneCutParam();
	//	}
	//}

	if (!m_pDataContext || !m_pViewer)
	{
		setProgressValue(100);

		emit finished();

		return;
	}

	std::vector<int> vt_pck_id;
	m_pckMesh = 0;
	int n_mesh = m_pDataContext->m_MeshData.GetMeshCount();
	//for (int i = 0; i < WIN_MANAGER->vt_pckID.size(); ++i)
	for (int i = 0; i < n_mesh; ++i)
	{
		MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(i);
		//if (WIN_MANAGER->vt_pckID[i])
		if (pMeshInfo && pMeshInfo->selected)
		{
			m_pckMesh++;

			vt_pck_id.push_back(i);
		}
	}

	if (m_pckMesh == 0)
	{
		setProgressValue(100);

		emit finished();
	}

	int interval = 90 / m_pckMesh;
	int val_progress = 0;

	m_ProgressInterval = 60.f / (float)m_pckMesh;

	m_countMesh = 0;
	bool isSelection = false;
	//201102 허 건대리 Undo / Redo	
	ACTION_MANAGER->action_UndoRedo_update(m_pDataContext, getMeshWorkMode(), getMeshWorkMode());

	m_ProgressTotal += 10.f;
	val_progress += 10.f;

	for (int i = 0; i < vt_pck_id.size(); ++i)
	{
		int mesh_id = vt_pck_id[i];

		auto p_mesh = m_pDataContext->m_MeshData.GetMesh(mesh_id);

		// 210326 허 건 과장
		// 데이터가 없는경우에는 수행안함
		if (!p_mesh || p_mesh->m_verts.size() < 1)
		{
			continue;
		}

		m_ProgressValue = 0;

		//20210611_byPHS_3mf Case는 메시 편집 시 단색으로 변경
		if (p_mesh->m_uv.size() > 0 && (_type != MESH_SMOOTH && _type != MESH_REDUCTION))
		{
			p_mesh->initShader(mip::SHADERTYPE::SHADER_PONG);
		}

		m_FileName = m_pDataContext->m_MeshData.GetMeshName(mesh_id);

		int NonCnt = mip::mesh_control::getNonManiHEdgeCnt(p_mesh);

		m_ProgressValue += 5.f;

		if (NonCnt >= p_mesh->m_tverts.size())
		{
			emit showQMessageDlg(STR_NONMANIFOLD_HEDGE_CASE, m_FileName);
			emit sig_updateProgress(val_progress + interval, m_FileName + QString().sprintf("(%d%)", 0));
			m_pDataContext->volume_data.threadResult = -1;
			continue;
		}

		switch (_type)
		{
		case MESH_SOLID:
			m_FileName = "[Solid] " + m_FileName;

			emit sig_updateProgress(val_progress, m_FileName + QString().sprintf("(%d%)", (int)m_ProgressValue));

			m_pDataContext->volume_data.threadResult = mip::mesh_control::solid(p_mesh, updateProgress, this);
			break;
		case MESH_HOLLOW:
		{
			m_FileName = "[Hollow] " + m_FileName;

			if (p_mesh->m_treeTris == nullptr)
			{
				setProgressValue(100);
				m_pDataContext->volume_data.threadResult = 0;
				emit finished();
				return;
			}

			emit sig_updateProgress(val_progress, m_FileName + QString().sprintf("(%d%)", (int)m_ProgressValue));

			if (_mesh2 == nullptr)
			{
				_mesh2 = new mip::MeshTopology(g_Renderer);
			}

			int result = mip::mesh_control::hollowTopology(p_mesh, _mesh2, _offset, _edgeLength, updateProgress, this);
			if (result)
			{
				MESH_RENDER_MANAGER->delTempMesh();
				p_mesh->initShader(mip::SHADERTYPE::SHADER_X_RAY);
				_mesh2->updateColor(p_mesh->m_baseColor);
				emit sig_buildRenderBufferTopology(_mesh2);
				MESH_RENDER_MANAGER->setTempMesh(_mesh2, p_mesh);
				_mesh2->m_baseColor = p_mesh->m_baseColor;

				//mip::mesh_control::hollowOk(p_mesh, _mesh2);

			}
			//SAFE_DELETE(_mesh2);
			setProgressValue(100);
			m_pDataContext->volume_data.threadResult = result;
			emit finished();
			return;
		}
		break;
		case MESH_REMESH:
			m_FileName = "[Remesh] " + m_FileName;

			emit sig_updateProgress(val_progress, m_FileName + QString().sprintf("(%d%)", (int)m_ProgressValue));

			if (m_Params.b_select_mode)
			{
				mip::MeshTopology* partial_mesh = new mip::MeshTopology(g_Renderer);
				m_pDataContext->volume_data.threadResult = mip::mesh_control::select_remesh(p_mesh, partial_mesh, (float)m_Params.edge_len_remesh, updateProgress, this);

				SAFE_DELETE(partial_mesh);

				CRemeshDlg* dlg = static_cast<CRemeshDlg*>(MESH_DIALOG_MANAGER->getMeshDialog(MESH_DIALOG_REMESH));
				if (dlg != nullptr)
				{
					dlg->setSelectMode(false);
				}

				m_Params.b_select_mode = false;
			}
			else
			{
				m_pDataContext->volume_data.threadResult = mip::mesh_control::MeshAutoRemesh(p_mesh, (float)m_Params.edge_len_remesh, updateProgress, this);
			}
			break;
		case MESH_REDUCTION:
			m_FileName = "[Reduction] " + m_FileName;

			emit sig_updateProgress(m_ProgressTotal, m_FileName + QString().sprintf("(%d%)", (int)m_ProgressValue));

			if (mip::mesh_control::isTriFlag(p_mesh, mip::SELECTED))
			{
				m_pDataContext->volume_data.threadResult =
					mip::mesh_control::select_reduce(p_mesh, mip::SELECTED, _ReduceMethod, _offset, _ReducePBChecked, updateProgress, this);
			}
			else
			{
				m_pDataContext->volume_data.threadResult =
					mip::mesh_control::collapseTopology(p_mesh, _ReduceMethod, _offset, _ReducePBChecked, updateProgress, this);
			}

			if (static_cast<ReduceDlg*>(MESH_DIALOG_MANAGER->getMeshDialog(MESH_DIALOG_REDUCE)) != nullptr)
			{
				static_cast<ReduceDlg*>(MESH_DIALOG_MANAGER->getMeshDialog(MESH_DIALOG_REDUCE))->setTopology(p_mesh);
				static_cast<ReduceDlg*>(MESH_DIALOG_MANAGER->getMeshDialog(MESH_DIALOG_REDUCE))->initOptionValue(static_cast<ReduceDlg*>(MESH_DIALOG_MANAGER->getMeshDialog(MESH_DIALOG_REDUCE))->getReduceMode());
			}

			//p_mesh->updateVertex();

			//p_mesh->m_tverts.clear();
			//p_mesh->m_ttris.clear();
			//p_mesh->m_tVHedges.clear();

			//p_mesh->setTopologyed(false);
			//p_mesh->mergingVertex();
			//p_mesh->buildTopologyHEdge();
			break;

		case MESH_SMOOTH:
			m_FileName = "[Smooth] " + m_FileName;

			emit sig_updateProgress(m_ProgressTotal, m_FileName + QString().sprintf("(%d%)", 0));

			m_pDataContext->volume_data.threadResult =
				mip::mesh_control::smoothTopology(p_mesh, _SmoothMethod, ((float)_offset), updateProgress, this);
			break;
		case MESH_SUBDIVISION:
			m_FileName = "[Sub-Division] " + m_FileName;

			emit sig_updateProgress(val_progress, m_FileName + QString().sprintf("(%d%)", (int)m_ProgressValue));

			mip::mesh_control::SubDivision(p_mesh, m_Params.method_sd, 3, updateProgress, this);
			break;
		case MESH_ISLAND_FILTER:
			m_FileName = "[Island-Filter] " + m_FileName;

			emit sig_updateProgress(val_progress, m_FileName + QString().sprintf("(%d%)", (int)m_ProgressValue));

			mip::mesh_control::IslandFilter(p_mesh, m_Params.n_remain_if, m_Params.b_inverse_if, m_Params.n_trinagles_if, updateProgress, this);

			break;
		default:
			m_pDataContext->volume_data.threadResult = -1;
			break;
		}

		p_mesh->buildTree();

		m_ProgressTotal += 20.f / (float)m_pckMesh;
		emit sig_updateProgress(m_ProgressTotal, m_FileName + QString().sprintf("(%d%)", 90));

		p_mesh->updateVertex();
		p_mesh->updateColor(p_mesh->m_baseColor);
		//
		//auto view = WIN_MANAGER->mainMeshWidget->getMainView();
		m_pViewer->updateGeometryCount();

		m_ProgressTotal += 10.f / (float)m_pckMesh;
		emit sig_updateProgress(m_ProgressTotal, m_FileName + QString().sprintf("(%d%)", 95));

		//201028 허건 대리 주석처리
		// 렌더링이 안되는 현상 발생
		// signal slot 으로 대체
		{
			//WIN_MANAGER->buildRenderBufferTopology(p_mesh);
			//WIN_MANAGER->renderLater_All();
			emit sig_buildRenderBufferTopology(p_mesh);
			emit sig_renderLater();
		}

		val_progress += interval;

		m_ProgressTotal = val_progress;

		setProgressValue(val_progress);

		m_countMesh++;
#endif
	}

	setProgressValue(100);

	emit finished();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
@breif Duplicate Mesh class 생성자
*/
WorkMeshDuplicate::WorkMeshDuplicate(DataContext* pDataContext, QList<muint32>& _list)
	: m_MeshList(_list)
	, m_pDataContext(pDataContext)
{

}

/*
@breif Duplicate Mesh class 소멸자
*/
WorkMeshDuplicate::~WorkMeshDuplicate()
{

}

/*
@brief
@return
*/
void		WorkMeshDuplicate::single_process()
{
	int n_list = m_MeshList.size();

	int	interval = 90 / n_list;
	int	progress_val_total = 0;

	for (int it = 0; it < n_list; ++it)
	{
		int index_list = m_MeshList[it];

		auto p_mesh = m_pDataContext->m_MeshData.GetMesh(index_list);

		if (p_mesh)
		{
			progress_val_total = it * interval;
			emit sig_progress(progress_val_total, "Duplicate Meshes..");

			WIN_MANAGER->makeCurrent();
			auto	cpy_mesh = new mip::MeshTopology(g_Renderer);
			p_mesh->copyTo(cpy_mesh);
			WIN_MANAGER->doneCurrent();

			progress_val_total += interval / 3;
			emit sig_progress(progress_val_total, "Duplicate Meshes..");

			auto	p_info = m_pDataContext->m_MeshData.GetMeshInfo(index_list);
			auto	layer_uid = p_info->uid;

			// create mesh info
			m_pDataContext->m_MeshData.CreateMeshInfo(true);

			muint32		uid = m_pDataContext->m_MeshData.GetCurrentMeshIndex();

			MeshInfo* info = m_pDataContext->m_MeshData.GetMeshInfo(uid);

			QString		file_name = QString::fromStdWString(p_info->MeshName);
			file_name += "_copy";

			if (info)
			{
				::StringCbPrintf(info->MeshName, MESHINFO_TEXT_LENGTH_MAX * sizeof(WCHAR),
					file_name.toStdWString().c_str());

				info->NameLength = file_name.length();

				info->color = p_info->color;

				info->upScale = false;

				info->uid = uid;
			}

			progress_val_total += interval / 3;
			emit sig_progress(progress_val_total, "Duplicate Meshes..");

			// push pck true 
			//WIN_MANAGER->vt_pckID.push_back(false);
			m_pDataContext->m_MeshData.SetMeshInfoModeSelectMode(uid, false);

			// insert mesh
			m_pDataContext->m_MeshData.InsertMesh(uid, cpy_mesh);

			m_vt_layer_uid[it] = layer_uid;
			m_vt_file_name[it] = file_name;

			auto p_cpy_mesh = m_pDataContext->m_MeshData.GetMesh(uid);
			if (p_cpy_mesh)
			{
				m_vt_p_topology[it] = p_cpy_mesh;
			}

			// update mesh-list
			emit sig_updateUI();

			WIN_MANAGER->setSaveState(false);
			//WIN_MANAGER->buildRenderBufferTopology(p_cpy_mesh);

			QThread::msleep(30);

			progress_val_total += interval / 3;
			emit sig_progress(progress_val_total, "Duplicate Meshes..");
		}
	}
}

/*
@brief
@return
*/
void		WorkMeshDuplicate::parallel_process()
{
	int n_list = m_MeshList.size();

	concurrency::combinable<float>		val_progress;

	float								interval = 90.f / (float)n_list;

	val_progress.local() = 0;

#if 0 //20211020_byPHS_렌더링이 안되는 현상으로 병렬처리 제거
	WIN_MANAGER->makeCurrent();
	concurrency::parallel_for(0, n_list, [&](int it)
		{
			int index_list = m_MeshList[it];

			float tmp_progress = 0.f;

			auto p_mesh = m_pDataContext->m_MeshData.GetMesh(index_list);

			if (p_mesh)
			{
				//WIN_MANAGER->makeCurrent();
				m_vt_p_topology[it] = new mip::MeshTopology(g_Renderer);
				p_mesh->copyTo(m_vt_p_topology[it]);
				//WIN_MANAGER->doneCurrent();

				val_progress.local() += interval / 3.f;
				tmp_progress = val_progress.combine(plus<float>());
				emit sig_progress(tmp_progress, "Duplicate Meshes..");
			}
		});
	WIN_MANAGER->doneCurrent();
#else
	int	progress_val_total = 0;
	for (int it = 0; it < n_list; ++it)
	{
		int index_list = m_MeshList[it];

		auto p_mesh = m_pDataContext->m_MeshData.GetMesh(index_list);

		if (p_mesh)
		{
			progress_val_total = it * interval;
			emit sig_progress(progress_val_total, "Duplicate Meshes..");

			WIN_MANAGER->makeCurrent();
			m_vt_p_topology[it] = new mip::MeshTopology(g_Renderer);
			p_mesh->copyTo(m_vt_p_topology[it]);
			WIN_MANAGER->doneCurrent();

			progress_val_total += interval / 3;
			emit sig_progress(progress_val_total, "Duplicate Meshes..");
		}
	}
#endif


	for (int it = 0; it < n_list; ++it)
	{
		int		index_list = m_MeshList[it];

		float	tmp_progress = 0.f;

		auto	p_info = m_pDataContext->m_MeshData.GetMeshInfo(index_list);
		auto	layer_uid = p_info->uid;

		// create mesh info
		m_pDataContext->m_MeshData.CreateMeshInfo(true);

		muint32		uid = m_pDataContext->m_MeshData.GetCurrentMeshIndex();

		QString		file_name = QString::fromStdWString(p_info->MeshName);
		file_name += "_copy";

		MeshInfo* info = m_pDataContext->m_MeshData.GetMeshInfo(uid);

		if (info)
		{
			::StringCbPrintf(info->MeshName, MESHINFO_TEXT_LENGTH_MAX * sizeof(WCHAR),
				file_name.toStdWString().c_str());

			info->NameLength = file_name.length();

			info->color = p_info->color;

			info->upScale = false;

			info->uid = uid;
		}

		val_progress.local() += interval / 3.f;
		tmp_progress = val_progress.combine(plus<float>());
		emit sig_progress(tmp_progress, "Duplicate Meshes..");

		// push pck true 
		//WIN_MANAGER->vt_pckID.push_back(false);
		m_pDataContext->m_MeshData.SetMeshInfoModeSelectMode(uid, false);

		// insert mesh
		m_pDataContext->m_MeshData.InsertMesh(uid, m_vt_p_topology[it]);

		m_vt_layer_uid[it] = layer_uid;
		m_vt_file_name[it] = file_name;

		auto p_cpy_mesh = m_pDataContext->m_MeshData.GetMesh(uid);
		if (p_cpy_mesh)
		{
			m_vt_p_topology[it] = p_cpy_mesh;
		}

		// update mesh-list
		emit sig_updateUI();

		WIN_MANAGER->setSaveState(false);
		//WIN_MANAGER->buildRenderBufferTopology(p_cpy_mesh);

		val_progress.local() += interval / 3.f;
		tmp_progress = val_progress.combine(plus<float>());
		emit sig_progress(tmp_progress, "Duplicate Meshes..");
	}
}

/*
@brief
@return
*/
void WorkMeshDuplicate::updateResult()
{
	ACTION_MANAGER->action_MeshList_add_Multi(m_pDataContext, m_vt_layer_uid, m_vt_file_name, m_vt_p_topology);

	int n_mesh = m_pDataContext->m_MeshData.GetMeshCount();

	int pck_idx = n_mesh - 1;

	m_pDataContext->m_MeshData.ClearMeshInfoSelectMode();
	m_pDataContext->m_MeshData.SetMeshInfoModeSelectMode(pck_idx, true);

	m_pDataContext->m_MeshData.SetCurrentMeshIndex(pck_idx);

	MESH_MODELVIEW_MANAGER->UpdatePivotPoint(pck_idx);

	emit sig_progress(100, "Duplicate Meshes...");

	m_pDataContext->volume_data.threadStop = true;
}

/*
@brief
@return
*/
void		WorkMeshDuplicate::threadRun()
{
	if (m_MeshList.size() < 1)
	{
		emit finished();
		return;
	}

	emit sig_progress(0, "Duplicate Meshes...");

	m_vt_p_topology.clear();

	m_vt_layer_uid.resize(m_MeshList.size(), -1);
	m_vt_p_topology.resize(m_MeshList.size(), NULL);
	m_vt_file_name.resize(m_MeshList.size());

	const auto	num_core = std::thread::hardware_concurrency();

	//if (m_MeshList.size() == 1 || num_core < 4)
	{
		single_process();
	}
	//else
	//{
	//	parallel_process();
	//}

	updateResult();

	emit finished();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
@breif Attach Mesh class 생성자
*/
WorkMeshAttach::WorkMeshAttach(QList<muint32>& _list, DataContext* pDataContext)
	: m_MeshList(_list)
	, m_pDataContext(pDataContext)
{

}

/*
@brief
@return
*/
void WorkMeshAttach::threadRun()
{
	if (!m_pDataContext || m_MeshList.size() != 2)
	{
		emit finished();
		return;
	}

	emit sig_progress(5, "Attach Meshes...");

	//201102 허 건대리 Undo / Redo
	ACTION_MANAGER->action_UndoRedo_update(m_pDataContext);

	emit sig_progress(20, "Attach Meshes...");

	mip::MeshTopology* p_mesh1 = m_pDataContext->m_MeshData.GetMesh(m_MeshList[0]);
	mip::MeshTopology* p_mesh2 = m_pDataContext->m_MeshData.GetMesh(m_MeshList[1]);

	if (p_mesh1 && p_mesh2)
	{
		p_mesh1->Attach(p_mesh2);

		emit sig_progress(90, "Attach Meshes...");

		sig_buildRenderBufferTopology(p_mesh1);
	}

	emit sig_progress(100, "Attach Meshes...");

	emit finished();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
@brief  WorkMeshToVoxel class 생성자
*/
WorkMeshToMask::WorkMeshToMask(QList<muint32>& _list, DataContext* pDataContext)
{
	m_MeshList = _list;

	m_pDataContext = pDataContext;
}

/*
@brief WorkMeshToVoxel class 소멸자
*/
WorkMeshToMask::~WorkMeshToMask()
{

}

/*
@brief		WorkMeshToVoxel 쓰레드 함수
@return		없음
*/
void		WorkMeshToMask::threadRun()
{
	MeshToMask();
}

/*
@brief		Closed Mesh Voxelization 수행함수
@return		없음
*/
void		WorkMeshToMask::MeshToMask()
{
	if (m_pDataContext && m_MeshList.size())
	{
		VOLUME_DATA& volume_data = m_pDataContext->volume_data;

		int width = volume_data.getCX();
		int height = volume_data.getCY();
		int slice = volume_data.getCZ();

		double spacing[3];

		spacing[0] = volume_data.getSpaceX(true);
		spacing[1] = volume_data.getSpaceY(true);
		spacing[2] = volume_data.getSpaceZ(true);

		emit sig_progress(5, "Mesh To Mask...");

		for (int i = 0; i < m_MeshList.size(); ++i)
		{
			mip::MeshTopology* p_mesh = m_pDataContext->m_MeshData.GetMesh(m_MeshList[i]);
			MeshInfo* mesh_info = m_pDataContext->m_MeshData.GetMeshInfo(m_MeshList[i]);

			if (!p_mesh | !p_mesh)
			{
				continue;
			}

			vtkSmartPointer<vtkPoints> points =
				vtkSmartPointer<vtkPoints>::New();
			vtkSmartPointer<vtkCellArray> vert =
				vtkSmartPointer<vtkCellArray>::New();

			std::vector<mip::VECTOR3>		vt_verts;

			std::vector<mip::TTri>& m_ttris = p_mesh->m_ttris;
			std::vector<mip::TVert>& m_tverts = p_mesh->m_tverts;

			muint32 n_tris = m_ttris.size();
			muint32	cnt_visible = 0;
			std::vector<muint32> vt_tris_idx;
			vt_tris_idx.reserve(n_tris);
			for (muint32 ti = 0; ti < n_tris; ti++)
			{
				if (m_ttris[ti].isD() || m_ttris[ti].isUS1())
				{
					continue;
				}

				cnt_visible++;

				vt_tris_idx.push_back(ti);
			}

			vt_tris_idx.resize(cnt_visible);

			int n_verts = cnt_visible * 3;
			vt_verts.resize(n_verts, mip::VECTOR3(0.f, 0.f, 0.f));

			const UINT		num_core = std::thread::hardware_concurrency();
			int				n_core = (int)num_core;

			concurrency::parallel_for(0, n_core, [&](int ii)
				{
					int interval = cnt_visible / n_core;
					int j = interval * ii;
					int finish = j + interval;
					int remain = 0;

					if (ii == n_core - 1)
					{
						remain = cnt_visible % n_core;
						finish += remain;
					}

					for (; j < finish; ++j)
					{
						int ti = vt_tris_idx[j];
						int pos0 = j * 3;
						int pos1 = pos0 + 1;
						int pos2 = pos0 + 2;

						auto& v1 = m_ttris[ti].vi[0];
						auto& v2 = m_ttris[ti].vi[1];
						auto& v3 = m_ttris[ti].vi[2];

						vt_verts[pos0] = m_tverts[v1].pos;
						vt_verts[pos1] = m_tverts[v2].pos;
						vt_verts[pos2] = m_tverts[v3].pos;
					}
				});

			mip::MATRIX44 trans_mat = p_mesh->getMatrix();

			const int nPoints = vt_verts.size() / 3;

			sig_progress(10, "Mesh To Mask...");
			mip::VECTOR3 vec;

			vtkIdType triangle[3];

			mip::VECTOR3 zero = mip::VECTOR3::Zero;
			for (int i = 0; i < nPoints; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					if (vt_verts[3 * i + j] == zero)
					{
						continue;
					}

					vec = mip::math::VectorTransform(vt_verts[3 * i + j], trans_mat) * 10.f;

					triangle[j] = points->InsertNextPoint(vec.x, vec.y, vec.z);
				}

				vert->InsertNextCell(3, triangle);

				if (volume_data.threadStop)
				{
					emit finished();
					return;
				}
			}

			vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
			polyData->SetPoints(points);
			polyData->SetPolys(vert);

			vtkIdType n_pts = polyData->GetNumberOfPoints();
			vtkIdType n_cells = polyData->GetNumberOfCells();

			if (polyData->GetNumberOfPoints() < 2 || polyData->GetNumberOfCells() < 2)
			{
				emit finished();
				return;
			}

			sig_progress(20, "Mesh To Mask...");

			int dim[3];

			dim[0] = width;
			dim[1] = height;
			dim[2] = slice;

			double origin[3];
			//origin[0] = (spacing[0] * 0.5) - DBL_EPSILON;
			//origin[1] = (spacing[1] * 0.5) - DBL_EPSILON;
			//origin[2] = (spacing[2] * 0.5) - DBL_EPSILON;

			//origin[0] = 0.0001;
			//origin[1] = 0.0001;
			//origin[2] = 0.0001;

			origin[0] = 0.0;
			origin[1] = 0.0;
			origin[2] = 0.0;

			vtkSmartPointer<vtkImageData> image_data = vtkSmartPointer<vtkImageData>::New();
			image_data->SetSpacing(spacing[0], spacing[1], spacing[2]);
			image_data->SetDimensions(dim);
			image_data->SetOrigin(origin);
			image_data->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
			image_data->SetExtent(0, dim[0] - 1, 0, dim[1] - 1, 0, dim[2] - 1);

			// fill the imageData with foreground voxels
			muint8 inval = 255;
			muint8 outval = 0;
			vtkIdType count = image_data->GetNumberOfPoints();
			for (vtkIdType i = 0; i < count; ++i)
			{
				image_data->GetPointData()->GetScalars()->SetTuple1(i, inval);
			}

			sig_progress(30, "Mesh To Mask...");

			// Convert STL polydata to image stencil data:
			vtkSmartPointer<vtkPolyDataToImageStencil> dataToStencil = vtkSmartPointer<vtkPolyDataToImageStencil>::New();
			dataToStencil->SetInputData(polyData);
			dataToStencil->SetOutputOrigin(origin);
			dataToStencil->SetOutputSpacing(spacing[0], spacing[1], spacing[2]);
			dataToStencil->SetOutputWholeExtent(image_data->GetExtent());
			dataToStencil->Update();

			sig_progress(70, "Mesh To Mask...");

			// Create image stencil with image stencil data and grid source:
			vtkSmartPointer<vtkImageStencil> imageStencil = vtkSmartPointer<vtkImageStencil>::New();
			imageStencil->SetInputData(image_data);
			imageStencil->SetStencilConnection(dataToStencil->GetOutputPort());
			//imageStencil->ReverseStencilOff();
			imageStencil->SetBackgroundValue(outval);
			imageStencil->Update();

			image_data->DeepCopy(imageStencil->GetOutput());

			sig_progress(80, "Mesh To Mask...");

			// 데이터 검증용
			{
				//auto res_dim = image_data->GetDimensions();
				//auto res_spacing = image_data->GetSpacing();

				//printf_s("res_dim [%d %d %d]\n", res_dim[0], res_dim[1], res_dim[2]);
				//printf_s("res_spacing [%lf %lf %lf]\n", res_spacing[0], res_spacing[1], res_spacing[2]);

				//FILE *fp;
				//fopen_s(&fp, "image_data_Mesh.txt", "w");

				//int cnt = 0;
				//for (vtkIdType i = 0; i < count; ++i)
				//{
				//	auto value = image_data->GetPointData()->GetScalars()->GetTuple1(i);

				//	fprintf_s(fp, "%d\n", (int)value);
				//	if (value == 255)
				//	{
				//		//printf_s("value == 255 >> %d\n", i);
				//		cnt++;
				//	}
				//}

				//fclose(fp);

				//printf_s("%d / %d\n", cnt, count);
			}

			// Create Mask && Insert Info
			{
				//bool r = m_pDataContext->m_MeshData.createMaskInfo(false, false);
				bool r = m_pDataContext->volume_data.createMaskInfo(true, false);
				if (r)
				{
					auto mask_id = volume_data.getCurrentMaskInfoID();
					auto mask_info = volume_data.getCurrentMaskInfo();

					mask_info->color = mesh_info->color;

					volume_data.setMaskName(QString().fromWCharArray(mesh_info->MeshName) + "_mask", mask_info->uid, true);

					// Insert data
					volume_data.createTempMaskData();
					mask* pm = volume_data.getMaskTempDataPoint();
					for (vtkIdType i = 0; i < count; ++i)
					{
						auto value = image_data->GetPointData()->GetScalars()->GetTuple1(i);

						pm[i] = (uchar)value;
					}

					auto		m_idx = mask_info->uid >= MASK_SECOND_MAX ? (mask_info->uid - MASK_SECOND_MAX) / 8 + 1 : 0;

					auto		m = ((m_idx == 0) ? mask_info->mask_id : mask_info->mask_id2);

					volume_data.applyTempMaskBitAdd(m, m_idx);

					volume_data.updateUIDBoundingBox(mask_info->uid, true);

					auto bb = volume_data.getBoundingBox(mask_info->uid);
					auto cnt = volume_data.fillMaskCount;

					volume_data.setBoundingBox(mask_info->uid, bb);
					volume_data.setVoxelCount(mask_info->uid, cnt, false);

					volume_data.forceUpdateMaskVolume();

					volume_data.setTAState(mask_info->uid, false);

					emit sig_applyMaskToUI(mask_info->uid);
				}
			}

			emit sig_progress(90, "Mesh To Mask...");
		}
	}

	emit sig_progress(100, "Mesh To Mask...");

	emit finished();
}

#if 1
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
@brief  WorkMeshStamp3D class 생성자
*/
WorkMeshStamp3D::WorkMeshStamp3D(DataContext* pDataContext, mip::MeshTopology* pTargetMesh, mip::MeshTopology* pFontMesh, const int pickFaceIdx, float fOffset, bool bIntaglio)
	: targetMesh(pTargetMesh)
	, fontMesh(pFontMesh)
	, faceIdx(pickFaceIdx)
	, offset(fOffset)
	, bIntaglio(bIntaglio)
	, m_pDataContext(pDataContext)
{


}

/*
@brief WorkMeshStamp3D class 소멸자
*/
WorkMeshStamp3D::~WorkMeshStamp3D()
{

}

/*
@brief
@return
*/
void WorkMeshStamp3D::updateProgress(float val, void* dt)
{
	WorkMeshStamp3D* worker = (WorkMeshStamp3D*)dt;
	worker->setProgressValue(val);
}

void WorkMeshStamp3D::setProgressValue(int value, bool init)
{
	static int val = 0;

	if (init)
	{
		val = 0;
		emit progress(val);
		return;
	}

	if (100 <= val)
		return;

	if ((val + 10) <= value)
	{
		val = value;
		emit progress(val);
	}
}


void WorkMeshStamp3D::threadRun()
{
	if (!m_pDataContext)
	{
		setProgressValue(100);

		emit finished();

		return;
	}

	setProgressValue(0, true);

	std::vector<mip::VECTOR3> input;
	mip::mesh_control::Stamp3D(targetMesh, fontMesh, faceIdx, offset, input);

	printf_s("\n finished FontMesh offset..! ");
	setProgressValue(20, true);

	fontMesh->updateVertex();
	//fontMesh->mergingVertex();
	//fontMesh->buildTopologyHEdge();
	fontMesh->buildTree();

#if STAMP3D_TEST_FONTMESH_VISIBLE
	mip::VECTOR4 color(mip::VECTOR4(120 / 255.0f, 150 / 255.0f, 120 / 255.0f, fontMesh->getAlphaVal() / 255.0f));
	fontMesh->updateColor(color);
	fontMesh->setTopologyed(true);

	emit sig_buildRenderBufferTopology(fontMesh);
#endif

#if 1
	mip::MeshTopology* res_mesh = new mip::MeshTopology(g_Renderer);
	int bBooleanOption = MESH_WORK_DIFF;
	if (offset > 0) bBooleanOption = MESH_WORK_UNION;
	mip::mesh_control::boolean(targetMesh, fontMesh, res_mesh, bBooleanOption, g_Renderer, updateProgress, this);
	res_mesh->buildTree();

	res_mesh->setRotate(targetMesh->rotation);
	res_mesh->setTranslate(targetMesh->translation);
	res_mesh->setScale(targetMesh->scale);

	printf_s("\n [finished targetMesh boolean..!] %d ", bBooleanOption);

	int nonManiCnt = mip::mesh_control::getNonManiHEdgeCnt(res_mesh);
	if (nonManiCnt > 0)
	{
		mip::mesh_control::deleteNonManifoldVert(res_mesh);
		res_mesh->updateVertex();
	}

	printf_s("\n [Stamp Result NonManiCnt - %d] ", nonManiCnt);

	//Update Result
	{
		QString		name;

		switch (bBooleanOption)
		{
		case MESH_WORK_DIFF:
			name = "Stamp_Intaglio";
			break;
		case MESH_WORK_UNION:
			name = "Stamp_Relief";
			break;
		}

		m_pDataContext->m_MeshData.CreateMeshInfo();
		muint32 uid_stamp = m_pDataContext->m_MeshData.GetCurrentMeshIndex();
		m_pDataContext->m_MeshData.SetMeshName(name, uid_stamp);
		MeshInfo* info_font = m_pDataContext->m_MeshData.GetMeshInfo(uid_stamp);

		if (info_font)
		{
			info_font->upScale = false;
			info_font->uid = uid_stamp;
		}

		// push pck true 
		//WIN_MANAGER->vt_pckID.push_back(false);
		m_pDataContext->m_MeshData.SetMeshInfoModeSelectMode(uid_stamp, false);

		//
		MeshInfo* mInfo = m_pDataContext->m_MeshData.GetMeshInfo(uid_stamp);
		mip::VECTOR4 color(mip::VECTOR4(mInfo->color.r / 255.0f, mInfo->color.g / 255.0f, mInfo->color.b / 255.0f, res_mesh->getAlphaVal() / 255.0f));
		res_mesh->updateColor(color);
		res_mesh->setTopologyed(true);

		m_pDataContext->m_MeshData.InsertMesh(uid_stamp, res_mesh);
		m_pDataContext->m_MeshData.MeshRenderUpdate(uid_stamp);
		emit sig_updateUI();
		emit sig_buildRenderBufferTopology(res_mesh);
		ACTION_MANAGER->action_MeshList_add(m_pDataContext, -1, name, res_mesh);
		//----------------------------------------
	}

#endif 

	m_pDataContext->volume_data.threadResult = 1;

	setProgressValue(100);

	emit finished();
}
#endif
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////