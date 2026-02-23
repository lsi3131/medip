#pragma once

#ifndef WORK_SURFACE_SAVE_VTK_H
#define WORK_SURFACE_SAVE_VTK_H

#include "define.h"

#include <QtWidgets>

#include "graphics/volumedata.h"
#include "graphics/MeshData.h"

#include "algorithm/VolumeToSurface3D/VolumeTo3DSurface.h"

class WorkSurfaceSaveVTK : public QObject, VolumeTo3DSurface
{
	Q_OBJECT

public:
	WorkSurfaceSaveVTK(
		VOLUME_DATA* pVolume,
		mask maskData,
		int maskIdx,
		QString& strFilename,
		EXPORT_3D_SURFACE_MESH_METHOD method,
		bool bPatientCoordinate = false,
		bool bExtract = false
	);
	virtual ~WorkSurfaceSaveVTK();

public slots:
	void threadRun();

signals:
	void progress(int);
	void finished();

private:
	void setProgressValue(int value, bool init = false);

	mask				m_MaskData;
	int					m_MaskIdx;

	QString				m_strFilename;

	bool				m_bExtract;
	bool				m_bPatientCoordinate;
};
#endif
