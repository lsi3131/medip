#include "stdafx.h"
#include "WorkVTKOpen.h"
#include "windowManager.h"
#include "ActionManager.h"
#include "Renderer/MeshTopology.h"

WorkVTKOpen::WorkVTKOpen(QStringList filename, DataContext* pDataContext)
{
	_strFilename = filename;
	m_pDataContext = pDataContext;
	//_meshCore = _mesh;
}

void WorkVTKOpen::setProgressValue(int value, bool init /*= false*/)
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

void WorkVTKOpen::threadRun()
{
	// 210726 허 건 과장
	// 기존소스코드 주석처리
	//	setProgressValue(0, true);
	//	_addValue = 1;
	//
	//	WIN_MANAGER->setRenderable(false);
	//
	//	vtkSmartPointer<vtkGenericDataObjectReader> reader = vtkSmartPointer<vtkGenericDataObjectReader>::New();
	//
	//	reader->SetFileName(_strFilename.toLocal8Bit().constData());
	//	reader->Update();
	//
	//	if (reader->IsFilePolyData())
	//	{
	//		vtkPolyData* output = reader->GetPolyDataOutput();
	//		vtkPoints *pts;
	//		vtkCellArray *polys;
	//		vtkCellArray *strips;
	//
	//		polys = output->GetPolys();
	//		strips = output->GetStrips();
	//		pts = output->GetPoints();
	//
	//		vtkIdType npts = 0;
	//		vtkIdType *indx = 0;
	//		vtkSmartPointer<vtkCellArray> polyStrips = vtkSmartPointer<vtkCellArray>::New();
	//		if (polys->GetNumberOfCells() > 0)
	//		{
	//			vtkIdType *ptIds = 0;
	//			for (polys->InitTraversal(); polys->GetNextCell(npts, ptIds);)
	//			{
	//				vtkTriangleStrip::DecomposeStrip(npts, ptIds, polyStrips);
	//
	//				if (_volumData->threadStop == true)
	//				{
	//					_volumData->threadResult = -1;
	//					emit finished();
	//					return;
	//				}
	//			}
	//		}
	//
	//		double n[3], v1[3], v2[3], v3[3];
	//		int numCell = polyStrips->GetNumberOfCells();
	//		int iCnt = 0;
	//
	//		mip::VECTOR3 offset_center(-DATA_CONTEXT->volume_data.getSizeX()*0.5f, -DATA_CONTEXT->volume_data.getSizeY()*0.5f, -DATA_CONTEXT->volume_data.getSizeZ()*0.5f);
	//
	//		for (polyStrips->InitTraversal(); polyStrips->GetNextCell(npts, indx); iCnt++)
	//		{
	//			pts->GetPoint(indx[0], v1);
	//			pts->GetPoint(indx[1], v2);
	//			pts->GetPoint(indx[2], v3);
	//
	//			mip::VECTOR3 va(v1[0], v1[1], v1[2]);
	//			mip::VECTOR3 vb(v2[0], v2[1], v2[2]);
	//			mip::VECTOR3 vc(v3[0], v3[1], v3[2]);
	//
	//			//va += offset_center;
	//			//vb += offset_center;
	//			//vc += offset_center;
	//
	//#ifdef USE_RIGHT_HAND
	//			_meshCore->m_verts.push_back(va*0.1f);
	//			_meshCore->m_verts.push_back(vb*0.1f);
	//			_meshCore->m_verts.push_back(vc*0.1f);
	//#else
	//			_meshCore->m_verts.push_back(vc*0.1f);
	//			_meshCore->m_verts.push_back(vb*0.1f);
	//			_meshCore->m_verts.push_back(va*0.1f);
	//#endif
	//			_meshCore->m_tris.push_back(_meshCore->m_tris.size());
	//			_meshCore->m_tris.push_back(_meshCore->m_tris.size());
	//			_meshCore->m_tris.push_back(_meshCore->m_tris.size());
	//
	//			//vtkTriangle::ComputeNormal(pts, npts, indx, n);
	//#ifdef USE_RIGHT_HAND
	//			mip::VECTOR3 normal = (vb - va) ^ (vc - va);
	//#else
	//			mip::VECTOR3 normal = (vc - va) ^ (vb - va);
	//#endif
	//			//mip::VECTOR3 normal(n[0], n[1], n[2]);
	//			normal.normalize();
	//			_meshCore->m_normals.push_back(normal);
	//			_meshCore->m_normals.push_back(normal);
	//			_meshCore->m_normals.push_back(normal);
	//
	//
	//			if ((((float)iCnt / numCell) * 100) >= (_addValue * 10))
	//			{
	//				setProgressValue(_addValue * 10);
	//				_addValue++;
	//			}
	//			if (_volumData->threadStop == true)
	//			{
	//				_volumData->threadResult = -1;
	//				emit finished();
	//				return;
	//			}
	//			//n[0], n[1]n, [2]
	//		}
	//
	//		_volumData->threadResult = 1;
	//
	//		emit finished();
	//		return;
	//	}
	//	else
	//	{
	//		_volumData->threadResult = -1;
	//		emit finished();
	//		return;
	//	}

	setProgressValue(0, true);
	_addValue = 1;

	WIN_MANAGER->setRenderable(false);

	int n_list = _strFilename.size();

	std::vector<mint32>					vt_layer_uid(n_list, -1);

	std::vector<QString>				vt_file_name(n_list);

	std::vector<mip::MeshTopology*>     vt_topology(n_list, NULL);

	int						progress_val = 0;
	int						interval = 90 / n_list;

	for (int i = 0; i < n_list; ++i)
	{
		emit sig_updateProgress(progress_val, _strFilename[i]);

		QString temp_name = _strFilename[i];
		temp_name.chop(4);

		vt_file_name[i] = temp_name;

		auto fn = _strFilename[i].toLocal8Bit();
		auto file_name = fn.constData();

		vtkSmartPointer<vtkGenericDataObjectReader> reader = vtkSmartPointer<vtkGenericDataObjectReader>::New();

		reader->SetFileName(file_name);
		reader->Update();

		if (reader->IsFilePolyData())
		{
			vtkPolyData* output = reader->GetPolyDataOutput();
			vtkPoints* pts;
			vtkCellArray* polys;
			vtkCellArray* strips;
			vtkCellArray* verts;

			polys = output->GetPolys();
			strips = output->GetStrips();
			pts = output->GetPoints();
			verts = output->GetVerts();

			vtkCellArray* pCellArray = nullptr;

			if (polys->GetNumberOfCells() > 0) pCellArray = polys;
			if (strips->GetNumberOfCells() > 0) pCellArray = strips;
			if (verts->GetNumberOfCells() > 0) pCellArray = verts;


			vtkIdType npts = 0;
			vtkIdType* indx = 0;
			vtkSmartPointer<vtkCellArray> polyStrips = vtkSmartPointer<vtkCellArray>::New();
			if (pCellArray->GetNumberOfCells() > 0)
			{
				vtkIdType* ptIds = 0;
				for (pCellArray->InitTraversal(); pCellArray->GetNextCell(npts, ptIds);)
				{
					vtkTriangleStrip::DecomposeStrip(npts, ptIds, polyStrips);

					if (m_pDataContext->volume_data.threadStop == true)
					{
						m_pDataContext->volume_data.threadResult = -1;
						emit finished();
						return;
					}
				}
			}

			double n[3], v1[3], v2[3], v3[3];
			int numCell = polyStrips->GetNumberOfCells();
			int iCnt = 0;

			mip::VECTOR3 offset_center(-m_pDataContext->volume_data.getSizeX() * 0.5f, -m_pDataContext->volume_data.getSizeY() * 0.5f, -m_pDataContext->volume_data.getSizeZ() * 0.5f);

			auto tmp_mesh = new mip::MeshTopology(g_Renderer);

			for (polyStrips->InitTraversal(); polyStrips->GetNextCell(npts, indx); iCnt++)
			{
				pts->GetPoint(indx[0], v1);
				pts->GetPoint(indx[1], v2);
				pts->GetPoint(indx[2], v3);

				mip::VECTOR3 va(v1[0], v1[1], v1[2]);
				mip::VECTOR3 vb(v2[0], v2[1], v2[2]);
				mip::VECTOR3 vc(v3[0], v3[1], v3[2]);

				//va += offset_center;
				//vb += offset_center;
				//vc += offset_center;

#ifdef USE_RIGHT_HAND
				tmp_mesh->m_verts.push_back(va * 0.1f);
				tmp_mesh->m_verts.push_back(vb * 0.1f);
				tmp_mesh->m_verts.push_back(vc * 0.1f);
#else
				_meshCore->m_verts.push_back(vc * 0.1f);
				_meshCore->m_verts.push_back(vb * 0.1f);
				_meshCore->m_verts.push_back(va * 0.1f);
#endif
				tmp_mesh->m_tris.push_back(tmp_mesh->m_tris.size());
				tmp_mesh->m_tris.push_back(tmp_mesh->m_tris.size());
				tmp_mesh->m_tris.push_back(tmp_mesh->m_tris.size());

				//vtkTriangle::ComputeNormal(pts, npts, indx, n);
#ifdef USE_RIGHT_HAND
				mip::VECTOR3 normal = (vb - va) ^ (vc - va);
#else
				mip::VECTOR3 normal = (vc - va) ^ (vb - va);
#endif
				//mip::VECTOR3 normal(n[0], n[1], n[2]);
				normal.normalize();
				tmp_mesh->m_normals.push_back(normal);
				tmp_mesh->m_normals.push_back(normal);
				tmp_mesh->m_normals.push_back(normal);


				//if ((((float)iCnt / numCell) * 100) >= (_addValue * 10))
				//{
				//	setProgressValue(_addValue * 10);
				//	_addValue++;
				//}

				if (m_pDataContext->volume_data.threadStop == true)
				{
					//_volumData->threadResult = -1;
					//emit finished();
					//return;
					continue;
				}
				//n[0], n[1]n, [2]
			}

			QFileInfo f(_strFilename[i]);

			QString tempName = f.fileName();
			tempName.chop(4);
			{
				// create mesh info
				m_pDataContext->m_MeshData.CreateMeshInfo();

				muint32 uid = m_pDataContext->m_MeshData.GetCurrentMeshIndex();

				m_pDataContext->m_MeshData.SetMeshName(tempName, uid);

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
				m_pDataContext->m_MeshData.InsertMesh(uid, tmp_mesh);

				int idx = m_pDataContext->m_MeshData.GetMeshCount() - 1;
				auto p_mesh = m_pDataContext->m_MeshData.GetMesh(idx);

				if (p_mesh)
				{
					vt_topology[i] = p_mesh;

					//WIN_MANAGER->moveMeshCenterScene(i);
				}

				vt_layer_uid[i] = -1;

				// update 
				emit sig_updateUI();

				WIN_MANAGER->setSaveState(false);

				WIN_MANAGER->MEViewRenderLater();
			}

			progress_val += interval;
		}
		else
		{
			//_volumData->threadResult = -1;
			//emit finished();
			//return;

			progress_val += interval;

			continue;
		}
	}

	emit sig_updateProgress(90, QString("Final Process"));

	ACTION_MANAGER->action_MeshList_add_Multi(m_pDataContext, vt_layer_uid, vt_file_name, vt_topology);

	m_pDataContext->volume_data.threadResult = 1;

	emit sig_updateProgress(100, QString("Final Process"));

	emit finished();
}