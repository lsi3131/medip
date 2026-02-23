#pragma once

#include "defineMEDIP.h"

#include "graphics/volumedata.h"
#include "graphics/MeshData.h"

// preview
class VolumeTo3DSurface
{
public:
	VolumeTo3DSurface(VOLUME_DATA* pVolumData, EXPORT_3D_SURFACE_MESH_METHOD method);
	~VolumeTo3DSurface();

	void Create3DSurface(mip::MeshTopology* pMesh, const mask data, int maskIdx);

protected:
	void create3DSurfaceVoxel(mip::MeshTopology* pMesh, const mask data, int maskIdx);
	void create3DSurfaceContourFilter(mip::MeshTopology* pMesh, const mask data, int maskIdx);

	EXPORT_3D_SURFACE_MESH_METHOD	m_Method;

	VOLUME_DATA* m_pVolumData;

	std::vector<mip::VECTOR4>		m_vecVoxelPosition;
};