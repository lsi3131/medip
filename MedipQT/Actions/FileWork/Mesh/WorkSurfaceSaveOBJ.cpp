#include "stdafx.h"
#include "WorkSurfaceSaveOBJ.h"
#include "WorkMeshToOBJ.h"
#include "windowManager.h"
#include "Renderer/MeshTopology.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WorkSurfaceSaveOBJ Class Member Functions - Strat
////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
WorkSurfaceSaveOBJ::WorkSurfaceSaveOBJ(
	VOLUME_DATA* pVolume,
	mask maskData,
	int maskIdx,
	QString& strFilename,
	EXPORT_3D_SURFACE_MESH_METHOD method,
	bool bPatientCoordinate,
	bool bExtract
)
	: VolumeTo3DSurface(pVolume, method)
{
	m_MaskData = maskData;
	m_MaskIdx = maskIdx;

	m_strFilename = strFilename;
	m_bExtract = bExtract;
	m_bPatientCoordinate = bPatientCoordinate;

	m_pVolumeData = pVolume;
}

WorkSurfaceSaveOBJ::~WorkSurfaceSaveOBJ()
{

}

void WorkSurfaceSaveOBJ::setProgressValue(int value, bool init)
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

void WorkSurfaceSaveOBJ::threadRun()
{
	setProgressValue(0, true);

	mip::MeshTopology* pMesh = new mip::MeshTopology(g_Renderer);

	if (!m_pVolumData || !pMesh)
	{
		emit finished();
		return;
	}

	if (m_pVolumData->threadStop == true)
	{
		emit finished();
		return;
	}

	setProgressValue(10);

	Create3DSurface(pMesh, m_MaskData, m_MaskIdx);

	setProgressValue(70);

	if (m_pVolumData->threadStop == true)
	{
		emit finished();
		return;
	}

	WorkMeshToOBJ work(m_pVolumeData, m_strFilename, pMesh, 10.f, m_bPatientCoordinate);
	work.threadRun();

	setProgressValue(100);
	emit finished();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WorkSurfaceSaveOBJ Class Member Functions - End
////////////////////////////////////////////////////////////////////////////////////////////////////////////// 