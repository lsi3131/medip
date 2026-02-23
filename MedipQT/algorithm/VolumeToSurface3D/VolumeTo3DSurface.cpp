#include "stdafx.h"
#include "VolumeTo3DSurface.h"
#include "MeshControl.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// VolumeTo3DSurface Class Member Functions - Start
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
VolumeTo3DSurface::VolumeTo3DSurface(
	VOLUME_DATA* pVolumData,
	EXPORT_3D_SURFACE_MESH_METHOD	method
)
	: m_pVolumData(pVolumData)
	, m_Method(method)
{
	if (m_pVolumData)
	{
		const int	nHeight = m_pVolumData->getCY();
		const int	nWidth = m_pVolumData->getCX();
		const int	nSlice = m_pVolumData->getCZ();
		const int	nLength = nHeight * nWidth * nSlice;

		m_vecVoxelPosition.resize(nLength);
	}
}

VolumeTo3DSurface::~VolumeTo3DSurface()
{

}

void VolumeTo3DSurface::Create3DSurface(
	mip::MeshTopology* pMesh,
	const mask			data,
	int					maskIdx
)
{
	if (!pMesh || !m_pVolumData)
	{
		return;
	}

	switch (m_Method)
	{
	case CONTOURE_FILTER:
		create3DSurfaceContourFilter(pMesh, data, maskIdx);
		break;
	case FIT_VOXEL_SIZE:
	default:
		create3DSurfaceVoxel(pMesh, data, maskIdx);
		break;
	}
}

void VolumeTo3DSurface::create3DSurfaceVoxel(mip::MeshTopology* pMesh, const mask data, int maskIdx)
{
	if (!m_pVolumData)
	{
		return;
	}

	const int	nHeight = m_pVolumData->getCY();
	const int	nWidth = m_pVolumData->getCX();
	const int	nSlice = m_pVolumData->getCZ();
	const int	nLength = nHeight * nWidth * nSlice;

	// check voxel
	for (int z = 0, index = 0; z < nSlice; ++z)
	{
		if (m_pVolumData->threadStop)
		{
			break;
		}

		for (int y = 0; y < nHeight; ++y)
		{
			if (m_pVolumData->threadStop)
			{
				break;
			}

			for (int x = 0; x < nWidth; ++x, ++index)
			{
				if (m_pVolumData->threadStop)
				{
					break;
				}

				m_vecVoxelPosition[index].x = (float)x;
				m_vecVoxelPosition[index].y = (float)y;
				m_vecVoxelPosition[index].z = (float)z;

				if (m_pVolumData->isMaskBit(x, y, z, data, maskIdx))
				{
					m_vecVoxelPosition[index].w = 1.f;
				}
				else
				{
					m_vecVoxelPosition[index].w = 0.f;
				}
			}
		}
	}

	const double spaceX = (double)m_pVolumData->getSpaceX();
	const double spaceY = (double)m_pVolumData->getSpaceY();
	const double spaceZ = (double)m_pVolumData->getSpaceZ();

	mip::VECTOR3 extent((float)nWidth, (float)nHeight, (float)nSlice);
	mip::VECTOR3 space(spaceX, spaceY, spaceZ);
	mip::VECTOR3 origin;

	mip::mesh_control::maskToMeshTopology(
		pMesh,
		m_vecVoxelPosition,
		extent,
		space,
		origin,
		false
	);
}

void VolumeTo3DSurface::create3DSurfaceContourFilter(mip::MeshTopology* pMesh, const mask data, int maskIdx)
{
	const muint32	nHeight = m_pVolumData->getCY();
	const muint32	nWidth = m_pVolumData->getCX();
	const muint32	nSlice = m_pVolumData->getCZ();
	const muint32	nLength = nHeight * nWidth * nSlice;

	const double spaceX = (double)m_pVolumData->getSpaceX();
	const double spaceY = (double)m_pVolumData->getSpaceY();
	const double spaceZ = (double)m_pVolumData->getSpaceZ();

	const double halfSpaceX = spaceX * 0.5;
	const double halfSpaceY = spaceY * 0.5;
	const double halfSpaceZ = spaceZ * 0.5;


	vtkSmartPointer<vtkUnsignedCharArray>		scalars = vtkSmartPointer<vtkUnsignedCharArray>::New();

	for (int z = 0, index = 0; z < nSlice; z++)
	{
		if (m_pVolumData->threadStop)
		{
			break;
		}

		for (int y = 0; y < nHeight; y++)
		{
			if (m_pVolumData->threadStop)
			{
				break;
			}

			for (int x = 0; x < nWidth; x++, ++index)
			{
				if (m_pVolumData->threadStop)
				{
					break;
				}

				double dt = 0;

				if (m_pVolumData->isMaskBit(x, y, z, data, maskIdx))
				{
					dt = 255.0; //임의 값. contour 값과 동일하게 맞춰줄 것
				}

				scalars->InsertTuple1(index, dt);
			}
		}
	}

	vtkSmartPointer<vtkImageData> image = vtkSmartPointer<vtkImageData>::New();
	image->SetDimensions(nWidth, nHeight, nSlice);
	image->SetSpacing(spaceX, spaceY, spaceZ);
	image->GetPointData()->SetScalars(scalars);

	vtkSmartPointer<vtkContourFilter> contourExtractor = vtkSmartPointer<vtkContourFilter>::New();;
	contourExtractor->SetInputData(image);
	contourExtractor->SetValue(0, 255.);
	contourExtractor->Update();

	if (m_pVolumData->threadStop == true)
	{
		return;
	}

	vtkPolyData* output = contourExtractor->GetOutput();
	vtkIdType		n_polys = output->GetNumberOfPolys();

	if (n_polys > 0)
	{
		if (m_pVolumData->threadStop == true)
		{
			return;
		}

		vtkSmartPointer<vtkFillHolesFilter> fillHolesFilter = vtkSmartPointer<vtkFillHolesFilter>::New();
		fillHolesFilter->SetInputConnection(contourExtractor->GetOutputPort());
		fillHolesFilter->SetHoleSize(1000.0);

		if (m_pVolumData->threadStop == true)
		{
			return;
		}

		vtkSmartPointer<vtkPolyDataNormals>  Normals = vtkSmartPointer<vtkPolyDataNormals>::New();
		Normals->SetInputConnection(fillHolesFilter->GetOutputPort());
		Normals->SetFeatureAngle(60.0);
		Normals->ConsistencyOn();
		Normals->SplittingOff();
		Normals->Update();

		if (m_pVolumData->threadStop == true)
		{
			return;
		}

		vtkPoints* pts;
		vtkCellArray* polys;
		vtkCellArray* strips;
		vtkPolyData* input;

		input = vtkPolyData::SafeDownCast(Normals->GetOutput());

		polys = input->GetPolys();
		strips = input->GetStrips();
		pts = input->GetPoints();

		vtkIdType npts = 0;
		vtkIdType* indx = 0;
		vtkSmartPointer<vtkCellArray> polyStrips = vtkSmartPointer<vtkCellArray>::New();
		if (polys->GetNumberOfCells() > 0)
		{
			vtkIdType* ptIds = 0;
			for (polys->InitTraversal(); polys->GetNextCell(npts, ptIds);)
			{
				if (m_pVolumData->threadStop)
					break;
				vtkTriangleStrip::DecomposeStrip(npts, ptIds, polyStrips);
			}
		}

		if (m_pVolumData->threadStop == true)
		{
			return;
		}

		double n[3], v1[3], v2[3], v3[3];
		int numCell = polyStrips->GetNumberOfCells();
		int iCnt = 0;

		for (polyStrips->InitTraversal(); polyStrips->GetNextCell(npts, indx); iCnt++)
		{
			if (m_pVolumData->threadStop)
			{
				break;
			}

			pts->GetPoint(indx[0], v1);
			pts->GetPoint(indx[1], v2);
			pts->GetPoint(indx[2], v3);

			mip::VECTOR3 va(v1[0] + halfSpaceX, v1[1] + halfSpaceY, v1[2] + halfSpaceZ);
			mip::VECTOR3 vb(v2[0] + halfSpaceX, v2[1] + halfSpaceY, v2[2] + halfSpaceZ);
			mip::VECTOR3 vc(v3[0] + halfSpaceX, v3[1] + halfSpaceY, v3[2] + halfSpaceZ);

#ifdef USE_RIGHT_HAND
			pMesh->m_verts.push_back(va);
			pMesh->m_verts.push_back(vb);
			pMesh->m_verts.push_back(vc);
#else
			p_topology->m_verts.push_back(vc);
			p_topology->m_verts.push_back(vb);
			p_topology->m_verts.push_back(va);
#endif
			pMesh->m_tris.push_back(pMesh->m_tris.size());
			pMesh->m_tris.push_back(pMesh->m_tris.size());
			pMesh->m_tris.push_back(pMesh->m_tris.size());

#ifdef USE_RIGHT_HAND
			mip::VECTOR3 normal = (vb - va) ^ (vc - va);
#else
			mip::VECTOR3 normal = (vc - va) ^ (vb - va);
#endif
			//mip::VECTOR3 normal(n[0], n[1], n[2]);
			normal.normalize();
			pMesh->m_normals.push_back(normal);
			pMesh->m_normals.push_back(normal);
			pMesh->m_normals.push_back(normal);
		}
	}

	pMesh->mergingVertex();
	pMesh->buildTopologyHEdge();
	pMesh->updateVertex();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// VolumeTo3DSurface Class Member Functions - End
//////////////////////////////////////////////////////////////////////////////////////////////////////////////