#pragma once

#ifndef WORK_SURFACE_SAVE_STL_H
#define WORK_SURFACE_SAVE_STL_H

#include "define.h"

#include <QtWidgets>

#include "graphics/volumedata.h"
#include "graphics/MeshData.h"

#include "algorithm/VolumeToSurface3D/VolumeTo3DSurface.h"

// export
class WorkSurfaceSaveSTL : public QObject, VolumeTo3DSurface
{
	Q_OBJECT

public:
	WorkSurfaceSaveSTL(
		VOLUME_DATA* pVolumeData,
		MeshData* pMeshData,
		mask maskData,
		int maskIdx,
		QString& strFilename,
		EXPORT_3D_SURFACE_MESH_METHOD method,
		bool bPatientCoordinate = false,
		bool bExtract = false
	);
	virtual ~WorkSurfaceSaveSTL();
public:
	mask				m_MaskData;
	int					m_MaskIdx;

	QString				m_strFilename;

	bool				m_bExtract;
	bool				m_bPatientCoordinate;

	MeshData* m_pMeshData;

	void setProgressValue(int value, bool init = false);

public slots:
	void threadRun();

signals:
	void progress(int);
	void finished();
};
#endif
