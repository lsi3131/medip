#include "stdafx.h"

#include "WorkSurfaceSaveVTK.h"
#include "WorkMeshToVTK.h"

#include "windowManager.h"
#include "Renderer/MeshTopology.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WorkSurfaceSaveVTK Class Member Functions - Start
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
WorkSurfaceSaveVTK::WorkSurfaceSaveVTK(
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
}

WorkSurfaceSaveVTK::~WorkSurfaceSaveVTK()
{

}

void WorkSurfaceSaveVTK::threadRun()
{
	setProgressValue(0, true);

	mip::MeshTopology* pMesh = new mip::MeshTopology(g_Renderer);

	if (!m_pVolumData || !pMesh)
	{
		emit finished();
		return;
	}

	setProgressValue(10);

	if (m_pVolumData->threadStop == true)
	{
		emit finished();
		return;
	}

	Create3DSurface(pMesh, m_MaskData, m_MaskIdx);

	WorkMeshToVTK work(m_pVolumData, m_strFilename, pMesh, 10.f, m_bPatientCoordinate);
	work.threadRun();

	setProgressValue(100);

	emit finished();
}

void WorkSurfaceSaveVTK::setProgressValue(int value, bool init)
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
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WorkSurfaceSaveVTK Class Member Functions - End
/////////////////////////////////////////////////////////////////////////////////////////////////////////////