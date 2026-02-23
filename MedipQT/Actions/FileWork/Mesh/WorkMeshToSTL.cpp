#include "stdafx.h"
#include "WorkMeshToSTL.h"
#include "Windows/windowManager.h"
#include "Renderer/model.h"

WorkMeshToSTL::WorkMeshToSTL(VOLUME_DATA* pVolumeData, MeshData* pMeshData, bool bPatientCoordinate)
	:
	_scale(1.f),
	_lUID(-1),
	m_bPatientCoordinate(bPatientCoordinate),
	m_pMeshData(pMeshData),
	m_pVolumeData(pVolumeData)
{

}

WorkMeshToSTL::WorkMeshToSTL(VOLUME_DATA* pVolumeData, MeshData* pMeshData, QString& strFilename, float sc, bool bPatientCoordinate) :
	_strFilename(strFilename),
	_scale(sc),
	m_bPatientCoordinate(bPatientCoordinate),
	m_pMeshData(pMeshData),
	m_pVolumeData(pVolumeData)
{

}

void WorkMeshToSTL::Write(mip::MeshTopology* mesh)
{
	single_process(mesh);
}

void WorkMeshToSTL::setProgressValue(int value, bool init /*= false*/)
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

/*
@brief
@return
*/
void WorkMeshToSTL::threadRun()
{
	int n_export = WIN_MANAGER->exportList.size();

	if (n_export == 1)
	{
		int mesh_id = WIN_MANAGER->exportList[0];
		printf_s("mesh_id : %d\n", mesh_id);
		mip::MeshTopology* mesh = m_pMeshData->GetMesh(mesh_id);

		single_process(mesh);
	}
	else if (n_export > 1)
	{
		multi_process();
	}
}

/*
@brief
@return
*/
void		WorkMeshToSTL::single_process(mip::MeshTopology* pMesh)
{
	progress(0);

	// adapt transform matrix
	mip::MeshTopology* tmp_mesh = new mip::MeshTopology(g_Renderer);
	if (pMesh && tmp_mesh)
	{
		auto& verts = tmp_mesh->m_verts;
		auto& tris = tmp_mesh->m_tris;
		auto& org_verts = pMesh->m_verts;
		auto& org_tris = pMesh->m_tris;

		verts.resize(org_verts.size());
		std::copy(org_verts.begin(), org_verts.end(), verts.begin());
		tris.resize(org_tris.size());
		std::copy(org_tris.begin(), org_tris.end(), tris.begin());

		progress(20);

		auto& tverts = tmp_mesh->m_tverts;
		auto& ttris = tmp_mesh->m_ttris;
		auto& org_tverts = pMesh->m_tverts;
		auto& org_ttris = pMesh->m_ttris;

		tmp_mesh->m_tverts.resize(pMesh->m_tverts.size());
		std::copy(pMesh->m_tverts.begin(), pMesh->m_tverts.end(), tmp_mesh->m_tverts.begin());
		tmp_mesh->m_ttris.resize(pMesh->m_ttris.size());
		std::copy(pMesh->m_ttris.begin(), pMesh->m_ttris.end(), tmp_mesh->m_ttris.begin());
		tmp_mesh->m_normals.resize(pMesh->m_normals.size());
		std::copy(pMesh->m_normals.begin(), pMesh->m_normals.end(), tmp_mesh->m_normals.begin());

		progress(40);

		auto org_mat = pMesh->getMatrix();

		auto offset = org_mat.getOrigin();

		if (m_bPatientCoordinate)
		{
			float xdir[3];
			float ydir[3];

			m_pVolumeData->getImgOrientation(true, xdir);
			m_pVolumeData->getImgOrientation(false, ydir);

			float zdir[3];
			vtkMath::Cross(xdir, ydir, zdir);

			float position[3];

			position[0] = m_pVolumeData->getImgPosX() * 0.1f;
			position[1] = m_pVolumeData->getImgPosY() * 0.1f;
			position[2] = m_pVolumeData->getImgPosZ() * 0.1f;


			mip::MATRIX44 patientMat;
			patientMat.identity();

			for (int i = 0; i < 3; i++)
			{
				patientMat.m[i][0] = xdir[i];
				patientMat.m[i][1] = ydir[i];
				patientMat.m[i][2] = zdir[i];
				patientMat.m[i][3] = position[i];
			}

			patientMat.m[3][3] = 1;


			const int	nSlice = m_pVolumeData->getCZ();
			double		spaceZ = m_pVolumeData->getSpaceZ();

			//patientMat.m[0][3] -= zdir[0] * space[2] * (nSlice - 1);
			//patientMat.m[1][3] -= zdir[1] * space[2] * (nSlice - 1);
			patientMat.m[2][3] -= zdir[2] * spaceZ * (nSlice - 1);

			patientMat.transpose();
			mip::math::MatrixMultiply(&org_mat, &org_mat, &patientMat);
		}

		for (int vi = 0; vi < tverts.size(); ++vi)
		{
			tverts[vi].pos = mip::math::VectorTransform(tverts[vi].pos, org_mat);
		}

		tmp_mesh->updateVertex();

		progress(60);

		m_pVolumeData->threadResult = mip::model::SaveSTLFile(_strFilename.toLocal8Bit().constData(),
			tmp_mesh, _scale, true, false);
	}

	if (tmp_mesh)
	{
		SAFE_DELETE(tmp_mesh);
	}

	progress(100);

	emit finished();
}

/*
@brief
@return
*/
void		WorkMeshToSTL::multi_process()
{
	progress(0);

	int n_export = WIN_MANAGER->exportList.size();
	if (n_export <= 0)
	{
		progress(100);
		emit finished();
		return;
	}
	int interval = 100 / n_export;
	int val_progress = 0;

	auto folder_path = WIN_MANAGER->exportPath;

	for (int idx = 0; idx < n_export; ++idx)
	{
		int mesh_id = WIN_MANAGER->exportList[idx];
		mip::MeshTopology* mesh = m_pMeshData->GetMesh(mesh_id);
		MeshInfo* mesh_info = m_pMeshData->GetMeshInfo(mesh_id);

		if (mesh)
		{
			// adapt transform matrix
			mip::MeshTopology* tmp_mesh = new mip::MeshTopology(g_Renderer);
			{
				auto& verts = tmp_mesh->m_verts;
				auto& tris = tmp_mesh->m_tris;
				auto& org_verts = mesh->m_verts;
				auto& org_tris = mesh->m_tris;

				verts.resize(org_verts.size());
				std::copy(org_verts.begin(), org_verts.end(), verts.begin());
				tris.resize(org_tris.size());
				std::copy(org_tris.begin(), org_tris.end(), tris.begin());

				val_progress += interval / 5;
				progress(val_progress);

				auto& tverts = tmp_mesh->m_tverts;
				auto& ttris = tmp_mesh->m_ttris;
				auto& org_tverts = mesh->m_tverts;
				auto& org_ttris = mesh->m_ttris;

				tmp_mesh->m_tverts.resize(org_tverts.size());
				std::copy(org_tverts.begin(), org_tverts.end(), tverts.begin());
				tmp_mesh->m_ttris.resize(org_ttris.size());
				std::copy(org_ttris.begin(), org_ttris.end(), ttris.begin());

				val_progress += interval / 5;
				progress(val_progress);

				auto& normals = tmp_mesh->m_normals;
				auto& org_normals = mesh->m_normals;

				normals.resize(org_normals.size());
				std::copy(org_normals.begin(), org_normals.end(), normals.begin());

				auto org_mat = mesh->getMatrix();

				auto offset = org_mat.getOrigin();

				if (m_bPatientCoordinate)
				{
					float xdir[3];
					float ydir[3];

					m_pVolumeData->getImgOrientation(true, xdir);
					m_pVolumeData->getImgOrientation(false, ydir);

					float zdir[3];
					vtkMath::Cross(xdir, ydir, zdir);

					float position[3];

					position[0] = m_pVolumeData->getImgPosX() * 0.1f;
					position[1] = m_pVolumeData->getImgPosY() * 0.1f;
					position[2] = m_pVolumeData->getImgPosZ() * 0.1f;


					mip::MATRIX44 patientMat;
					patientMat.identity();

					for (int i = 0; i < 3; i++)
					{
						patientMat.m[i][0] = xdir[i];
						patientMat.m[i][1] = ydir[i];
						patientMat.m[i][2] = zdir[i];
						patientMat.m[i][3] = position[i];
					}

					patientMat.m[3][3] = 1;


					const int	nSlice = m_pVolumeData->getCZ();
					double		spaceZ = m_pVolumeData->getSpaceZ();

					//patientMat.m[0][3] -= zdir[0] * space[2] * (nSlice - 1);
					//patientMat.m[1][3] -= zdir[1] * space[2] * (nSlice - 1);
					patientMat.m[2][3] -= zdir[2] * spaceZ * (nSlice - 1);

					patientMat.transpose();
					mip::math::MatrixMultiply(&org_mat, &org_mat, &patientMat);
				}

				for (int vi = 0; vi < tverts.size(); ++vi)
				{
					tverts[vi].pos = mip::math::VectorTransform(tverts[vi].pos, org_mat);
				}

				tmp_mesh->updateVertex();
				tmp_mesh->updateColor(tmp_mesh->m_baseColor);

				val_progress += interval / 5;
				progress(val_progress);

				//mesh_info->MeshName;
				QString file_path = folder_path + "/" + m_pMeshData->GetMeshName(mesh_id) + ".stl";

				//printf_s("Stl scale : %f\n", mesh_info->uid != -1 ? 10.0f : 1.0f);

				m_pVolumeData->threadResult = mip::model::SaveSTLFile(file_path.toLocal8Bit().constData(),
					tmp_mesh, _scale, true, false);


				if (m_pVolumeData->threadResult && m_pVolumeData->threadStop)
				{
					LPCWSTR strVariable = (const wchar_t*)file_path.utf16();

					::DeleteFile(strVariable);
				}
			}

			val_progress += interval / 5;
			progress(val_progress);

			if (tmp_mesh)
			{
				SAFE_DELETE(tmp_mesh);
			}

			val_progress = interval * (idx + 1);

			progress(val_progress);
		}
	}

	WIN_MANAGER->exportList.clear();

	progress(100);


	emit finished();
}