#include "stdafx.h"
#include "WorkSurfaceSaveSTL.h"
#include "windowManager.h"
#include "WorkMeshToSTL.h"
#include "Renderer/MeshTopology.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WorkSurfaceSaveSTL Class Member Functions - Start
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
WorkSurfaceSaveSTL::WorkSurfaceSaveSTL(
	VOLUME_DATA* pVolumeData,
	MeshData* pMeshData,
	mask maskData,
	int maskIdx,
	QString& strFilename,
	EXPORT_3D_SURFACE_MESH_METHOD method,
	bool bPatientCoordinate,
	bool bExtract
)
	: VolumeTo3DSurface(pVolumeData, method)
{
	m_MaskData = maskData;
	m_MaskIdx = maskIdx;

	m_strFilename = strFilename;
	m_bExtract = bExtract;
	m_bPatientCoordinate = bPatientCoordinate;

	m_pMeshData = pMeshData;
}

WorkSurfaceSaveSTL::~WorkSurfaceSaveSTL()
{

}

void WorkSurfaceSaveSTL::setProgressValue(int value, bool init)
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

void WorkSurfaceSaveSTL::threadRun()
{
	mip::MeshTopology* pMesh = new mip::MeshTopology(g_Renderer);

	if (!m_pVolumData || !pMesh)
	{
		emit finished();
		return;
	}

	setProgressValue(0, true);

	if (m_pVolumData->threadStop == true)
	{
		emit finished();
		return;
	}

	Create3DSurface(pMesh, m_MaskData, m_MaskIdx);

	setProgressValue(70);

	WorkMeshToSTL work(m_pVolumData, m_pMeshData, m_strFilename, 10.f, m_bPatientCoordinate);
	work.Write(pMesh);

	SAFE_DELETE(pMesh);

	setProgressValue(100);

	emit finished();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WorkSurfaceSaveSTL Class Member Functions - End
//////////////////////////////////////////////////////////////////////////////////////////////////////////////