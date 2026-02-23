#include "stdafx.h"
#include "WorkMeshToVTK.h"
#include "Renderer/MeshTopology.h"
#include <ppl.h>
#include <vtkAutoInit.h>

VTK_MODULE_INIT(vtkRenderingOpenGL2); // VTK was built with vtkRenderingOpenGL2
VTK_MODULE_INIT(vtkInteractionStyle);

WorkMeshToVTK::WorkMeshToVTK(VOLUME_DATA* pVolumeData, QString& strFilename, mip::MeshTopology* m, float sc, bool bPatientCoordinate) :
	m_vtkFilePath(strFilename),
	_mesh(m),
	_scale(sc),
	m_bPatientCoordinate(bPatientCoordinate),
	m_pVolumeData(pVolumeData)
{

}

void WorkMeshToVTK::setProgressValue(int value, bool init /*= false*/)
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

void WorkMeshToVTK::upateprogress(vtkObject*, unsigned long eid, void* clientdata, void* calldata)
{
	WorkMeshToVTK* worker = (WorkMeshToVTK*)clientdata;
	if (worker)
	{
		worker->setProgressValue((*(double*)calldata) * 20 + worker->_addValue);
	}
}

void WorkMeshToVTK::threadRun()
{
	vtkSmartPointer<vtkPoints> points =
		vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> vert =
		vtkSmartPointer<vtkCellArray>::New();

	std::vector<mip::VECTOR3>		vt_verts;

	auto& m_ttris = _mesh->m_ttris;
	auto& m_tverts = _mesh->m_tverts;

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

	const auto		num_core = std::thread::hardware_concurrency();
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

	auto trans_mat = _mesh->getMatrix();

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
		mip::math::MatrixMultiply(&trans_mat, &trans_mat, &patientMat);
	}

	const int nPoints = vt_verts.size() / 3;

	setProgressValue(0, true);
	mip::VECTOR3 vec;

	int index = 0;
	vtkIdType triangle[3];
	for (int i = 0; i < nPoints; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			vec = mip::math::VectorTransform(vt_verts[3 * i + j], trans_mat) * _scale;

			triangle[j] = points->InsertNextPoint(vec.x, vec.y, vec.z);
		}
		vert->InsertNextCell(3, triangle);
		setProgressValue(50 * ((float)i / nPoints));

		if (m_pVolumeData->threadStop)
		{
			emit finished();
			return;
		}
	}

	vtkSmartPointer<vtkPolyData> polyData =
		vtkSmartPointer<vtkPolyData>::New();
	polyData->SetPoints(points);
	polyData->SetPolys(vert);

	auto n_poly = polyData->GetNumberOfPolys();
	if (points > 0)
	{
		vtkSmartPointer<vtkPolyDataNormals> Normals =
			vtkSmartPointer<vtkPolyDataNormals>::New();
		Normals->SetInputData(polyData);
		Normals->SetFeatureAngle(60.0);
		Normals->ConsistencyOn();
		Normals->SplittingOff();
		Normals->Update();

		setProgressValue(65);

		//vtkSmartPointer<vtkCleanPolyData> cleanPolyData =
		//	vtkSmartPointer<vtkCleanPolyData>::New();
		//cleanPolyData->SetInputConnection(Normals->GetOutputPort());
		//cleanPolyData->Update();

		if (m_pVolumeData->threadStop)
		{
			emit finished();
			return;
		}

		polyData = vtkPolyData::SafeDownCast(Normals->GetOutput());

		setProgressValue(85);
	}

	vtkSmartPointer<vtkPolyDataWriter> vtkWriter = vtkSmartPointer<vtkPolyDataWriter>::New();
	vtkWriter->SetFileName(m_vtkFilePath.toLocal8Bit().constData());
	vtkWriter->SetInputData(polyData);

	int result = vtkWriter->Write();
	if (result == 1)
	{
		m_pVolumeData->threadResult = 1; /* success */
	}
	else
	{
		m_pVolumeData->threadResult = 0; /* fail */
	}

	setProgressValue(100);

	emit finished();
}
