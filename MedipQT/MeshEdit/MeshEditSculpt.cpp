#include "stdafx.h"
#include "MeshEditSculpt.h"
#include "WindowManager.h"

#include "Renderer/Renderer.h"
#include "Renderer/ShaderMeshData.h"
#include "Actions/ActionManager.h"
#include "graphics/MeshData.h"
#include "graphics/VolumeData.h"
#include "MeshEdit/CMeshWorkManager.h"
#include "MeshEdit/CMeshModelViewManager.h"
#include "MeshEdit/CMeshDlgManager.h"

#include "mipEngine/geometry.h"

#include "Dialogs/BrushSculptDlg.h"

#define SUPPORT_SCULPT_REFINE 1

MeshEditSculpt::MeshEditSculpt(
	mip::Renderer* pRenderer,
	ActionManager* pActionManager,
	WindowManager* pWinManager,
	MeshData* pMeshData,
	VOLUME_DATA* pVolumeData,
	CMeshWorkManager* pMeshWorkManager,
	CMeshModelViewManager* pMeshModelViewManager,
	CMeshDlgManager* pMeshDlgManager) :
	m_pRenderer(pRenderer),
	m_pActionManager(pActionManager),
	m_pWinManager(pWinManager),
	m_pMeshData(pMeshData),
	m_pVolumeData(pVolumeData),
	m_pMeshWorkManager(pMeshWorkManager),
	m_pMeshModelViewManager(pMeshModelViewManager),
	m_pMeshDlgManager(pMeshDlgManager)
{
	m_realPickPT = mip::VECTOR3(0, 0, 0);
	m_pickPoint = mip::VECTOR3(0, 0, 0);
	m_pickVertPoint = mip::VECTOR3(0, 0, 0);

	m_sphereRadius = 0.0f;
}

mip::VECTOR3 MeshEditSculpt::GetPickPoint() const
{
	return m_pickPoint;
}

mip::VECTOR3 MeshEditSculpt::GetPickVertPoint() const
{
	return m_pickVertPoint;
}

void MeshEditSculpt::SetSphereRadius(float value)
{
	m_sphereRadius = value;
}

float MeshEditSculpt::GetSphereRadius() const
{
	return m_sphereRadius;
}

bool MeshEditSculpt::ProcessMove(eMouseMode mouseMode, const QPoint& prevMousePoint, const QPoint& newMousePoint, const QSize& screenSize)
{
	int width = screenSize.width();
	int height = screenSize.height();

	if (m_pActionManager->getAction_state() == ACTP_CALC_MATCHINGDATA)
	{
		if (m_pVolumeData->threadStop == false)
		{
			printf_s("\n The brush thread is not finished yet.");
			return false;
		}
	}

	mip::MATRIX44 view_mat = m_pRenderer->getView();
	mip::MATRIX44 proj_mat = m_pRenderer->getProj();

	mip::VECTOR3 v1 = mip::geom::Screen2World(prevMousePoint.x(), prevMousePoint.y(), width, height, view_mat, proj_mat);
	mip::VECTOR3 v2 = mip::geom::Screen2World(newMousePoint.x(), newMousePoint.y(), width, height, view_mat, proj_mat);

	mip::VECTOR3 dv = v2 - v1;
	float diff = dv.length();

	MESH_WORK_MODE meshWorkMode = m_pMeshWorkManager->getWorkMode();
	mip::MeshTopology* p_mesh = nullptr;
	int n_mesh = m_pMeshData->GetMeshCount();

	int nSelectedID = -1;
	int nSelectedCnt = 0;
	//for (int ii = 0; ii < m_pWinManager->vt_pckID.size(); ii++)
	for (int ii = 0; ii < n_mesh; ii++)
	{
		MeshInfo* pMeshInfo = m_pMeshData->GetMeshInfo(ii);

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

	BrushSculptDlg* dlg = static_cast<BrushSculptDlg*>(m_pMeshDlgManager->getMeshDialog(MESH_DIALOG_SCULPT));
	SCULPT_MODE sculptMode = dlg->getSculptMode();

	float size = m_sphereRadius * ((float)dlg->getSize() * 0.01);
	float strength = dlg->getStrength();
	float sizePer = dlg->getSize();

	float mx;
	float my;

	mip::VECTOR2 diff_pt(newMousePoint.x() - prevMousePoint.x(), newMousePoint.y() - prevMousePoint.y());

	int iter = 1;

	if (diff < size)
	{
		mx = newMousePoint.x();
		my = newMousePoint.y();
	}
	else if (diff * 0.5f < size)
	{
		iter = 2;

		diff_pt.x *= 0.5f;
		diff_pt.y *= 0.5f;

		mx = prevMousePoint.x() + diff_pt.x;
		my = prevMousePoint.y() + diff_pt.y;
	}
	else if (diff * 0.3f < size)
	{
		iter = 3;

		diff_pt.x *= 0.3f;
		diff_pt.y *= 0.3f;

		mx = prevMousePoint.x() + diff_pt.x;
		my = prevMousePoint.y() + diff_pt.y;
	}
	else if (diff * 0.25f < size)
	{
		iter = 4;

		diff_pt.x *= 0.25f;
		diff_pt.y *= 0.25f;

		mx = prevMousePoint.x() + diff_pt.x;
		my = prevMousePoint.y() + diff_pt.y;
	}
	else if (diff * 0.2f < size)
	{
		iter = 5;

		diff_pt.x *= 0.2f;
		diff_pt.y *= 0.2f;

		mx = prevMousePoint.x() + diff_pt.x;
		my = prevMousePoint.y() + diff_pt.y;
	}
	else
	{
		return false;
	}


	for (int j = 0; j < iter; ++j)
	{
		if (j > 0)
		{
			mx += diff_pt.x;
			my += diff_pt.y;
		}
		for (int i = 0; i < n_mesh; ++i)
		{
			MeshInfo* pMeshInfo = m_pMeshData->GetMeshInfo(i);
			if (!pMeshInfo || !pMeshInfo->selected)
				//if (!m_pWinManager->vt_pckID[i])
			{
				continue;
			}

			p_mesh = m_pMeshData->GetMesh(i);

			if (mouseMode == eMouseMode::Release)
			{
				//Sculpt Move
				m_1stDirPickFromScreen = mip::VECTOR3(0.0f, 0.0f, 0.0f);
				m_pickPtOld = mip::VECTOR3(0.0f, 0.0f, 0.0f);
				m_realPickPT = mip::VECTOR3(-9990.0f, -9990.0f, -9990.0f);
				mip::AABB box;
				mip::mesh_control::getMinMax(p_mesh->m_verts, box.min, box.max);
				p_mesh->m_boundingBox = box;
				dlg->setBoundingBox(box);

				j = iter;

				break;
			}

			mip::MATRIX44 matWorld;
			int _vertIdx = -1;
			mip::VECTOR3 tracePoint(mip::VECTOR3(0, 0, 0));

			mip::MATRIX44 wvp = m_pRenderer->getWorld() * m_pRenderer->getView() * m_pRenderer->getProj();

			mip::MATRIX44 mat_offset;
			mip::VECTOR3 offset_center(-m_pVolumeData->getSizeX() * 0.5f, -m_pVolumeData->getSizeY() * 0.5f, -m_pVolumeData->getSizeZ() * 0.5f);

			mat_offset.identity();
			mat_offset.translation(offset_center);

			mip::MATRIX44 view_world = m_pMeshModelViewManager->GetMainTransform();

			mip::RAY ray_pre = mip::geom::ScreenToRay(mip::VECTOR2((float)prevMousePoint.x(), (float)prevMousePoint.y()), width, height,
				m_pMeshModelViewManager->GetCameraPtr()->getView(), m_pMeshModelViewManager->GetCameraPtr()->getProj(), &(p_mesh->getMatrix() * mat_offset * view_world));

			if (!p_mesh->intersectRay(ray_pre.org, ray_pre.dir, tracePoint, _vertIdx, false))
			{
				return false;
			}


			mip::RAY ray = mip::geom::ScreenToRay(mip::VECTOR2(mx, my), width, height,
				m_pMeshModelViewManager->GetCameraPtr()->getView(), m_pMeshModelViewManager->GetCameraPtr()->getProj(), &(p_mesh->getMatrix() * mat_offset * view_world));

			bool bReturn = p_mesh->intersectRay(ray.org, ray.dir, tracePoint, _vertIdx, false);

			if (m_1stDirPickFromScreen == mip::VECTOR3(0.0f, 0.0f, 0.0f) && mouseMode == eMouseMode::Press)
			{
				m_realPickPT = p_mesh->m_tverts[_vertIdx].pos;
				m_pickPtOld = p_mesh->m_tverts[_vertIdx].pos;
				m_1stDirPickFromScreen = p_mesh->m_tverts[_vertIdx].pos - ray.org;

				return true;
			}

			p_mesh->getShaderMeshData()->setRadius(size);
			p_mesh->getShaderMeshData()->setPickPt(m_pickPoint);

			std::vector<mip::VECTOR3> listVec;
			mip::mesh_control::IndexStruct IndexList_MOVE;
			mip::mesh_control::IndexStruct IndexList_DRAG;
			std::vector<muint32> tiList;
			if (m_1stDirPickFromScreen != mip::VECTOR3(0.0f, 0.0f, 0.0f) && mouseMode == eMouseMode::Move)
			{
				if (dlg == nullptr || p_mesh->m_treeTris == nullptr)
				{
					return false;
				}

				mip::VECTOR3 pickPtNew = ray.org + m_1stDirPickFromScreen;

				float size = dlg->getSize() * 0.015;

				if (sculptMode == SM_MOVE)
				{
					if (!mip::mesh_control::brush_Move(p_mesh, p_mesh->m_2ArrforDisplayVerts, IndexList_MOVE,
						_vertIdx, m_pickPtOld, m_pickPtOld, ray, m_1stDirPickFromScreen, size, sizePer, SM_MOVE, dlg->getSelectMode()))
					{
						return false;
					}

					mip::mesh_control::editOctree(p_mesh, p_mesh->m_treeTris->getRoot(), IndexList_MOVE);
				}
				else if (sculptMode == SM_DRAG)
				{
					if (!mip::mesh_control::brush_Move(p_mesh, p_mesh->m_2ArrforDisplayVerts, IndexList_MOVE,
						_vertIdx, m_pickPtOld, m_pickPtOld, ray, m_1stDirPickFromScreen, size, sizePer, SM_DRAG, dlg->getSelectMode(), true))
					{
						return false;
					}

					if (mip::mesh_control::brush_drag(p_mesh, p_mesh->m_2ArrforDisplayVerts, IndexList_DRAG,
						_vertIdx, m_pickPtOld, m_realPickPT, ray, m_1stDirPickFromScreen, dlg->getSelectMode(), strength, size, sizePer) == 1)
					{
					}
#if 1
					IndexList_MOVE.Tris.insert(IndexList_MOVE.Tris.begin(), IndexList_DRAG.Tris.begin(), IndexList_DRAG.Tris.end());
					IndexList_MOVE.Verts.insert(IndexList_MOVE.Verts.begin(), IndexList_DRAG.Verts.begin(), IndexList_DRAG.Verts.end());

					mip::mesh_control::editOctree(p_mesh, p_mesh->m_treeTris->getRoot(), IndexList_MOVE);
#endif 
				}

				m_pickPtOld = pickPtNew;
				m_pickPoint = pickPtNew;
			}

			if (j == iter - 1)
			{
				mip::SRenderBufferParams render_params;
				if (sculptMode == SM_MOVE)
				{
					render_params.b_update_release = false;
					render_params.b_update_index = false;
					render_params.b_update_color = false;
					render_params.b_update_texture = false;
					render_params.b_use_vervex_sub = false;
					render_params.b_use_normal_sub = true;
					//render_params.b_use_color_sub = true;
					//render_params.b_use_index_sub = true;

					render_params.vt_idx.resize(IndexList_MOVE.org_Verts.size());
					std::copy(IndexList_MOVE.org_Verts.begin(), IndexList_MOVE.org_Verts.end(), render_params.vt_idx.begin());

					m_pRenderer->makeCurrent();
					p_mesh->buildRenderBufferTopology(render_params);
					m_pRenderer->doneCurrent();
				}
				else
				{
					mip::mesh_control::updateDisplay(p_mesh);
					p_mesh->updateColor(p_mesh->m_baseColor);
					m_pRenderer->makeCurrent();
					p_mesh->buildRenderBufferTopology();
					m_pRenderer->doneCurrent();
				}
			}
		}
	}

	return bSuccess;
}

bool MeshEditSculpt::ProcessRelease(const QPoint& prevMousePoint, const QPoint& newMousePoint, const QSize& screenSize)
{
	BrushSculptDlg* pDlg = static_cast<BrushSculptDlg*>(m_pMeshDlgManager->getMeshDialog(MESH_DIALOG_SCULPT));
	if (pDlg == nullptr)
	{
		return false;
	}

	SCULPT_MODE mode = pDlg->getSculptMode();

	if (pDlg != nullptr && (mode == SM_MOVE || mode == SM_DRAG))
	{
		ProcessMove(eMouseMode::Release, prevMousePoint, newMousePoint, screenSize);
	}

	//if ((mode == SM_SMOOTH || mode == SM_INFLATE || mode == SM_DEFLATE || SM_REDUCE))
	if ((pDlg != nullptr && (
		(mode == SM_REFINE) || SUPPORT_SCULPT_REFINE) ||
		(
			mode == SM_SMOOTH ||
			mode == SM_INFLATE ||
			mode == SM_DEFLATE ||
			mode == SM_REDUCE)
		))
	{
		if (m_pMeshDlgManager->isMeshDialog())
		{
			//Bounding Box 계산
			int pickMeshIndex = m_pMeshData->GetCurrentMeshIndex();
			MeshInfo* pMeshInfo = m_pMeshData->GetMeshInfo(pickMeshIndex);
			mip::MeshTopology* pMeshData = m_pMeshData->GetMesh(pickMeshIndex);

			if (pMeshData)
			{
				//Sculpt Move
				m_1stDirPickFromScreen = mip::VECTOR3(0.0f, 0.0f, 0.0f);
				m_pickPtOld = mip::VECTOR3(0.0f, 0.0f, 0.0f);
				m_realPickPT = mip::VECTOR3(-9990.0f, -9990.0f, -9990.0f);
				mip::AABB box;
				mip::mesh_control::getMinMax(pMeshData->m_verts, box.min, box.max);
				pMeshData->m_boundingBox = box;
				pDlg->setBoundingBox(box);

				mip::mesh_control::updateDisplay(pMeshData);

				//qInfo() << "[[ !updateDisplay :]]";

				m_pWinManager->buildRenderBufferTopology(pMeshData);
			}
		}
	}

	return true;
}

bool MeshEditSculpt::ReadyBrush(MESH_WORK_MODE _MeshWorkMode, int initial)
{
	mip::MeshTopology* p_mesh = nullptr;
	int meshCnt = m_pMeshData->GetMeshCount();
	//int pickCnt = m_pWinManager->vt_pckID.size();

	int nSelectedID = -1;
	int nSelectedCnt = 0;
	//for (int ii = 0; ii < m_pWinManager->vt_pckID.size(); ii++)
	for (int ii = 0; ii < meshCnt; ii++)
	{
		MeshInfo* pMeshInfo = m_pMeshData->GetMeshInfo(ii);
		//if (m_pWinManager->vt_pckID[ii])
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

	if (m_pMeshDlgManager->isMeshDialog())
	{
		m_pMeshWorkManager->UpdateWorkMode((MESH_WORK_MODE)_MeshWorkMode, true);
	}
	else
	{
		m_pMeshWorkManager->UpdateWorkMode(MESH_WORK_NONE, true);
		return false;
	}

	for (int i = 0; i < meshCnt; ++i)
	{
		MeshInfo* pMeshInfo = m_pMeshData->GetMeshInfo(i);

		//if (!m_pWinManager->vt_pckID[i])
		if (!pMeshInfo || !pMeshInfo->selected)
		{
			continue;
		}

		p_mesh = m_pMeshData->GetMesh(i);

		if (_MeshWorkMode == MESH_WORK_BRUSH_SCULPT)
		{
			std::vector<mip::VECTOR3> _normals;
			std::vector<mip::VECTOR3> _verts;
			std::vector<muint32> _index;

			_verts.reserve(p_mesh->m_ttris.size() * 3);
			_normals.reserve(p_mesh->m_ttris.size() * 3);

			int triCnt = 0;
			int count_verts = 0;
			for (muint32 ii = 0; ii < p_mesh->m_ttris.size(); ii++)
			{
				if (p_mesh->m_ttris[ii].isD() || p_mesh->m_ttris[ii].isUS1())
				{
					for (int jj = 0; jj < mip::TTri::TRI_SIZE; jj++)
					{
						_normals.push_back(p_mesh->m_boundingBox.getCenter());
						_verts.push_back(p_mesh->m_boundingBox.getCenter());
					}
					count_verts += 3;
					continue;
				}

				triCnt++;

				int v1 = p_mesh->m_ttris[ii].vi[0];
				int v2 = p_mesh->m_ttris[ii].vi[1];
				int v3 = p_mesh->m_ttris[ii].vi[2];

				_verts.push_back(p_mesh->m_tverts[v1].pos);
				_verts.push_back(p_mesh->m_tverts[v2].pos);
				_verts.push_back(p_mesh->m_tverts[v3].pos);

				mip::VECTOR3 Normal(0, 0, 0);
				Normal = (p_mesh->m_tverts[v2].pos - p_mesh->m_tverts[v1].pos).cross((p_mesh->m_tverts[v3].pos - p_mesh->m_tverts[v1].pos));
				Normal.normalize();

				for (int jj = 0; jj < mip::TTri::TRI_SIZE; jj++)
				{
					_normals.push_back(Normal);
				}

				count_verts += 3;
			}

			_verts.resize(count_verts);
			_normals.resize(count_verts);

			_index.resize(_verts.size(), -1);
			for (int ii = 0; ii < _index.size(); ii++)
			{
				_index[ii] = ii;
			}

			p_mesh->m_verts.swap(_verts);
			p_mesh->m_tris.swap(_index);
			p_mesh->m_normals.swap(_normals);

			int Cnt = 0; int startPosIdx = 0;
			for (int ii = 0; ii < p_mesh->m_tverts.size(); ii++)
			{
				if (p_mesh->m_tverts[ii].isD() || p_mesh->m_tverts[ii].isUS1())
				{
					continue;
				}

				if (startPosIdx == 0)
				{
					startPosIdx = ii;
				}

				Cnt++;
			}

			if (Cnt < 5)
			{
				return false;
			}

			m_pickPoint = p_mesh->m_tverts[startPosIdx].pos;
			printf_s("\n Id:%d brush Ready \n", i);
#if 0
			mip::mesh_control::updateDisplay(p_mesh);
			mip::mesh_control::getDisplayIndexByVertex(p_mesh, m_BrushData._2ArrforDisplayVerts, m_BrushData._2ArrforDisplayNormals,
				m_BrushData._arrTVertToDisVert, m_BrushData._arrDisVertToTVert, p_mesh->m_tverts.size());
#else
			mip::mesh_control::updateDisplay(p_mesh);
			mip::mesh_control::getDisplayIndexByVertex(p_mesh, p_mesh->m_2ArrforDisplayVerts, p_mesh->m_tverts.size());
#endif

			QDialog* pDialog = m_pMeshDlgManager->getMeshDialog();
			BrushSculptDlg* pSculptDialog = dynamic_cast<BrushSculptDlg*>(pDialog);
			if (pDialog != nullptr && initial == 1)
			{
				pSculptDialog->setVertTriCnt(Cnt, triCnt);
			}
		}
		else if (_MeshWorkMode == MESH_WORK_BRUSH_SELECTION)
		{
		}
	}
	return true;
}

bool MeshEditSculpt::ProcessSelect(eMeshSelectMode selectMode, const QPoint& mousePoint, const QSize& screenSize)
{
	MESH_WORK_MODE mode = m_pMeshWorkManager->getWorkMode();

	mip::MeshTopology* p_mesh = nullptr;

	int n_mesh = m_pMeshData->GetMeshCount();

	int nSelectedID = -1;
	int nSelectedCnt = 0;

	for (int ii = 0; ii < n_mesh; ii++)
	{
		MeshInfo* pMeshInfo = m_pMeshData->GetMeshInfo(ii);
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

	BrushSelectDlg* dlg = static_cast<BrushSelectDlg*>(m_pMeshDlgManager->getMeshDialog(MESH_DIALOG_SELECT));

	for (int i = 0; i < n_mesh; ++i)
	{
		MeshInfo* pMeshInfo = m_pMeshData->GetMeshInfo(i);

		//if (!m_pWinManager->vt_pckID[i])
		if (!pMeshInfo || !pMeshInfo->selected)
		{
			continue;
		}

		p_mesh = m_pMeshData->GetMesh(i);

		mip::MATRIX44 matWorld;
		int _vertIdx = -1;
		mip::VECTOR3 tracePoint(mip::VECTOR3(0, 0, 0));

		//intersectRay 시, Tri flag 초기화 됨
		PointTracing(tracePoint, mousePoint, screenSize, _vertIdx, p_mesh, matWorld, matWorld);

		//pckSphere 위치
		if (_vertIdx != -1)
		{
			int triDeletedCnt = 0;
			for (int jj = 0; jj < p_mesh->m_tverts[_vertIdx].tis.size(); jj++)
			{
				int triIdx = p_mesh->m_tverts[_vertIdx].tis[jj];
				if (p_mesh->m_ttris[triIdx].isD())
				{
					triDeletedCnt++;
				}
			}

			if (!(p_mesh->m_tverts[_vertIdx].isD() || triDeletedCnt >= p_mesh->m_tverts[_vertIdx].tis.size()))
			{
				m_pickPoint = p_mesh->m_tverts[_vertIdx].pos;
			}
		}

		if (dlg == nullptr || _vertIdx < 0)
		{
			return false;
		}

		int select_Method = dlg->getSelectMode(); //byPHS_20201104_ 0 - volume select, else - surface select
		int method = (int)selectMode; //0 - select, 1 - unselect
		float factor = 0.8;
		float radi = dlg->getSize() * 0.01;
		mip::mesh_control::brush_selection(p_mesh, m_pickPoint, method, select_Method, _vertIdx, radi, mip::SELECTED);

		p_mesh->getShaderMeshData()->setRadius(7.5f);
		p_mesh->getShaderMeshData()->setPickPt(m_pickPoint);

		MeshInfo* mInfo = m_pMeshData->GetMeshInfo(i);
		mip::VECTOR4 color(mip::VECTOR4(mInfo->color.r / 255.0f, mInfo->color.g / 255.0f, mInfo->color.b / 255.0f, p_mesh->getAlphaVal() / 255.0f));
		p_mesh->updateColor(color);

		m_pWinManager->makeCurrent();
		p_mesh->buildRenderBufferTopology();
		m_pWinManager->doneCurrent();
	}

	qInfo() << "process select";
	return true;
}

bool MeshEditSculpt::ProcessBrushSculpt(const QPoint& mousePoint, const QSize& screenSize)
{
	if (m_pActionManager->getAction_state() == ACTP_CALC_MATCHINGDATA)
	{
		if (m_pVolumeData->threadStop == false)
		{
			printf_s("\n The brush thread is not finished yet.");
			return false;
		}
	}

	int width = screenSize.width();
	int height = screenSize.height();

	DWORD start, end;
	start = ::GetTickCount();

	MESH_WORK_MODE mode = m_pMeshWorkManager->getWorkMode();

	mip::MeshTopology* p_mesh = nullptr;

	int n_mesh = m_pMeshData->GetMeshCount();

	int nSelectedID = -1;
	int nSelectedCnt = 0;
	//for (int ii = 0; ii < m_pWinManager->vt_pckID.size(); ii++)
	for (int ii = 0; ii < n_mesh; ii++)
	{
		MeshInfo* pMeshInfo = m_pMeshData->GetMeshInfo(ii);
		//if (m_pWinManager->vt_pckID[ii])
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

	BrushSculptDlg* dlg = static_cast<BrushSculptDlg*>(m_pMeshDlgManager->getMeshDialog(MESH_DIALOG_SCULPT));

	for (int i = 0; i < n_mesh; ++i)
	{
		MeshInfo* pMeshInfo = m_pMeshData->GetMeshInfo(i);
		if (!pMeshInfo || !pMeshInfo->selected)
		{
			continue;
		}

		DWORD start1, end1;

		p_mesh = m_pMeshData->GetMesh(i);

		int _vertIdx = -1;
		mip::VECTOR3 tracePoint(mip::VECTOR3(0, 0, 0));

		mip::MATRIX44 mat_offset;
		mip::VECTOR3 offset_center(-m_pVolumeData->getSizeX() * 0.5f, -m_pVolumeData->getSizeY() * 0.5f, -m_pVolumeData->getSizeZ() * 0.5f);

		mat_offset.identity();
		mat_offset.translation(offset_center);

		mip::MATRIX44 view_world = m_pMeshModelViewManager->GetMainTransform();

		mip::MATRIX44 wvp = m_pRenderer->getWorld() * m_pRenderer->getView() * m_pRenderer->getProj();
		mip::RAY ray = mip::geom::ScreenToRay(
			mip::VECTOR2(mousePoint.x(), mousePoint.y()),
			width, height,
			m_pMeshModelViewManager->GetCameraPtr()->getView(),
			m_pMeshModelViewManager->GetCameraPtr()->getProj(),
			&(p_mesh->getMatrix() * mat_offset * view_world));

		bool bReturn = p_mesh->intersectRay(ray.org, ray.dir, tracePoint, _vertIdx, false);

		//pckSphere 위치
		if (_vertIdx != -1)
		{
			mip::VECTOR3 outVec = mip::VECTOR3(0.0f, 0.0f, 0.0f);
			//if (!mip::mesh_control::getRealPickPoint(p_mesh, ray, _vertIdx, outVec)) return false;

			m_pickPoint = p_mesh->m_tverts[_vertIdx].pos;
		}
		else
		{
			//printf_s("_vertIdx : %d\n", _vertIdx);
			return false;
		}

		if (dlg == nullptr || p_mesh->m_treeTris == nullptr)
		{
			return false;
		}

		SCULPT_MODE sculptMode = dlg->getSculptMode();
		float	size = m_sphereRadius * ((float)dlg->getSize() * 0.01);
		float	strength = dlg->getStrength();
		float	nStrength = strength;
		float	sizePer = dlg->getSize();

		p_mesh->getShaderMeshData()->setRadius(size);
		p_mesh->getShaderMeshData()->setPickPt(m_pickPoint);

		if (sculptMode == SM_SMOOTH || sculptMode == SM_INFLATE || sculptMode == SM_DEFLATE)
		{
			strength = strength * 0.01;
		}

		mip::SRenderBufferParams render_params;

		mip::mesh_control::IndexStruct	_indexStruct;
		if (sculptMode == SM_SMOOTH)
		{
			start1 = ::GetTickCount();
			int SMOOTH = 0;
			mip::mesh_control::brush_smooth(p_mesh, p_mesh->m_2ArrforDisplayVerts, render_params.vt_idx,
				SMOOTH, dlg->getSelectMode(), m_pickPoint, _vertIdx, strength, size);

			end1 = ::GetTickCount();
			//printf_s("Finish brush_smooth : %lf\n", (double)(end1 - start1) / 1000.);
		}
		else if (sculptMode == SM_INFLATE)
		{
			int INFLATE = 3;
			bool bDeflate = false;
			mip::mesh_control::brush_Inflate(p_mesh, p_mesh->m_2ArrforDisplayVerts, _indexStruct, render_params.vt_idx,
				m_pickPoint, _vertIdx, strength, size, dlg->getSelectMode(), bDeflate);

			mip::mesh_control::editOctree(p_mesh, p_mesh->m_treeTris->getRoot(), _indexStruct);
		}
		else if (sculptMode == SM_DEFLATE)
		{
			int INFLATE = 3;
			bool bDeflate = true;
			mip::mesh_control::brush_Inflate(p_mesh, p_mesh->m_2ArrforDisplayVerts, _indexStruct, render_params.vt_idx,
				m_pickPoint, _vertIdx, strength, size, dlg->getSelectMode(), bDeflate);
		}

		else if (sculptMode == SM_REDUCE)
		{
			mip::mesh_control::brush_reduce(p_mesh, p_mesh->m_2ArrforDisplayVerts, render_params.vt_idx,
				dlg->getSelectMode(), m_pickPoint, _vertIdx, strength, size);
		}
#if SUPPORT_SCULPT_REFINE
		else if (sculptMode == SM_REFINE)
		{
			start1 = ::GetTickCount();
			mip::mesh_control::brush_refine(p_mesh, p_mesh->m_2ArrforDisplayVerts, _indexStruct, _vertIdx, m_pickPoint,
				dlg->getSelectMode(), nStrength, size, sizePer);

			//printf_s("\n _indexStruct.org_Verts.size(); %d", _indexStruct.org_Verts.size());			

			mip::mesh_control::editOctree(p_mesh, p_mesh->m_treeTris->getRoot(), _indexStruct);
			end1 = ::GetTickCount();
			//printf_s("Brush SM_REFINE : %lf\n", (double)(end1 - start1) / 1000.);
		}
#endif

		if (sculptMode == SM_SMOOTH || sculptMode == SM_DEFLATE || sculptMode == SM_INFLATE)
		{
			render_params.b_update_release = false;
			render_params.b_update_index = false;
			render_params.b_update_color = false;
			render_params.b_update_texture = false;
			render_params.b_use_vervex_sub = true;
			render_params.b_use_normal_sub = true;
			//render_params.b_use_color_sub = true;
			//render_params.b_use_index_sub = true;

			//DWORD start1, end1;
			start1 = ::GetTickCount();

			m_pRenderer->makeCurrent();
			p_mesh->buildRenderBufferTopology(render_params);
			m_pRenderer->doneCurrent();

			end1 = ::GetTickCount();
			//printf_s("Finish buildRenderBufferTopology : %lf, idx : %d\n", (double)(end1 - start1) / 1000., render_params.vt_idx.size());
		}
		else if (sculptMode == SM_REDUCE)
		{
			render_params.b_update_release = false;
			render_params.b_update_texture = false;
			render_params.b_use_vervex_sub = false;
			render_params.b_use_normal_sub = true;
			render_params.b_use_color_sub = true;
			render_params.b_use_index_sub = true;

			//DWORD start1, end1;
			start1 = ::GetTickCount();

			m_pRenderer->makeCurrent();
			p_mesh->buildRenderBufferTopology(render_params);
			m_pRenderer->doneCurrent();

			end1 = ::GetTickCount();
			//printf_s("Finish buildRenderBufferTopology : %lf, idx : %d\n", (double)(end1 - start1) / 1000., render_params.vt_idx.size());
		}
		else
		{
			//mip::mesh_control::updateDisplay(p_mesh);
			//p_mesh->updateColor(p_mesh->m_baseColor);
			//printf_s("\n [buildRenderBufferTopology]");
			//mip::mesh_control::updateDisplay(p_mesh);
			m_pRenderer->makeCurrent();
			p_mesh->buildRenderBufferTopology();
			m_pRenderer->doneCurrent();
		}

		end = ::GetTickCount();
		//printf_s("Finish process_BrushSculpt : %lf\n", (double)(end - start) / 1000.);
	}

	qInfo() << "Process BrushSculpt";
	return true;
}

bool MeshEditSculpt::MoveSphere(const QPoint& mousePoint, const QSize& screenSize)
{
	int width = screenSize.width();
	int height = screenSize.height();

	MESH_WORK_MODE mode = m_pMeshWorkManager->getWorkMode();
	mip::MeshTopology* p_mesh = nullptr;

	int n_mesh = m_pMeshData->GetMeshCount();
	int nSelectedID = -1;

	for (int ii = 0; ii < n_mesh; ii++)
	{
		MeshInfo* pMeshInfo = m_pMeshData->GetMeshInfo(ii);
		if (pMeshInfo && pMeshInfo->selected)
		{
			nSelectedID = ii;
		}
	}

	if (nSelectedID < 0)
	{
		return false;
	}

	for (int i = 0; i < n_mesh; ++i)
	{
		MeshInfo* pMeshInfo = m_pMeshData->GetMeshInfo(i);

		if (!pMeshInfo || !pMeshInfo->selected)
		{
			continue;
		}

		p_mesh = m_pMeshData->GetMesh(i);

		int _vertIdx = -1;
		mip::VECTOR3 tracePoint(mip::VECTOR3(0, 0, 0));

		mip::MATRIX44 wvp = m_pRenderer->getWorld() * m_pRenderer->getView() * m_pRenderer->getProj();

		mip::MATRIX44 mat_offset;
		mip::VECTOR3 offset_center(-m_pVolumeData->getSizeX() * 0.5f, -m_pVolumeData->getSizeY() * 0.5f, -m_pVolumeData->getSizeZ() * 0.5f);

		mat_offset.identity();
		mat_offset.translation(offset_center);

		mip::MATRIX44 view_world = m_pMeshModelViewManager->GetMainTransform();

		mip::RAY ray = mip::geom::ScreenToRay(
			mip::VECTOR2(mousePoint.x(), mousePoint.y()),
			width, height,
			m_pMeshModelViewManager->GetCameraPtr()->getView(),
			m_pMeshModelViewManager->GetCameraPtr()->getProj(),
			&(p_mesh->getMatrix() * mat_offset * view_world)
		);

		if (p_mesh->m_ttris.size() > 0)
		{
			bool bReturn = p_mesh->intersectRay(ray.org, ray.dir, tracePoint, _vertIdx, false);
		}

#define PRINT_LOG 0

		//pckSphere 위치
#if PRINT_LOG == 1
		printf_s("\n [_vertIdx - %d]", _vertIdx);
#endif		

		if (_vertIdx != -1 && !p_mesh->m_tverts[_vertIdx].isD())
		{
			mip::VECTOR3 outVec = mip::VECTOR3(0.0f, 0.0f, 0.0f);
			int pick_ti = 0;
			//			if ((pick_ti = mip::mesh_control::getRealPickPoint(p_mesh, ray, _vertIdx, outVec)))
			//			{
			//				m_pickPoint = outVec;				
			//#if PRINT_LOG == 1
			//				int v0 = p_mesh->m_ttris[pick_ti].vi[0];
			//				int v1 = p_mesh->m_ttris[pick_ti].vi[1];
			//				int v2 = p_mesh->m_ttris[pick_ti].vi[2];
			//				printf_s("\n pick_ti - %d", pick_ti);
			//				printf_s("\n[Success - ti %d ( %d, %d, %d )]\n", pick_ti, v0, v1, v2);
			//#endif
			//			}
			//			else
			{
				m_pickPoint = p_mesh->m_tverts[_vertIdx].pos;
			}

			m_pickVertPoint = p_mesh->m_tverts[_vertIdx].pos;

			BrushSculptDlg* dlg = static_cast<BrushSculptDlg*>(m_pMeshDlgManager->getMeshDialog(MESH_DIALOG_SCULPT));
			int size = 0;
			if (dlg != nullptr)
			{
				size = dlg->getSize();
			}

			float fInput = m_sphereRadius * (size * 0.01f);
			p_mesh->getShaderMeshData()->setRadius(fInput);
			p_mesh->getShaderMeshData()->setPickPt(m_pickPoint);
		}
		else
		{
		}
	}

	return true;
}


bool MeshEditSculpt::PointTracing(
	mip::VECTOR3& result, 
	const QPoint& point, 
	const QSize& screenSize,
	int& vertIdx, 
	mip::MeshTopology* in_mesh, 
	const mip::MATRIX44& mat, 
	const mip::MATRIX44& matworld, 
	bool saveFlag)
{
	if (in_mesh)
	{
		int width = screenSize.width();
		int height = screenSize.height();

		mip::MATRIX44 wvp = m_pRenderer->getWorld() * m_pRenderer->getView() * m_pRenderer->getProj();

		mip::MATRIX44 mat_offset;
		mip::VECTOR3 offset_center(-m_pVolumeData->getSizeX() * 0.5f, -m_pVolumeData->getSizeY() * 0.5f, -m_pVolumeData->getSizeZ() * 0.5f);

		mat_offset.identity();
		mat_offset.translation(offset_center);

		mip::MATRIX44 view_world = m_pMeshModelViewManager->GetMainTransform();

		mip::RAY ray = mip::geom::ScreenToRay(
			mip::VECTOR2(point.x(), point.y()), 
			width, height,
			m_pMeshModelViewManager->GetCameraPtr()->getView(), 
			m_pMeshModelViewManager->GetCameraPtr()->getProj(), 
			&(in_mesh->getMatrix() * mat_offset * view_world));

		vertIdx = -1;

		bool bReturn = in_mesh->intersectRay(ray.org, ray.dir, result, vertIdx, false);
		return bReturn;
	}

	return false;
}

