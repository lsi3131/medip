#pragma once

#ifndef WORK_VISUALPRINT_SURFACE_EXPORT_H
#define WORK_VISUALPRINT_SURFACE_EXPORT_H

#include <QtWidgets>

#include "graphics/volumedata.h"
#include "graphics/MeshData.h"

#include "algorithm/VolumeToSurface3D/VolumeTo3DSurface.h"

// preview
class VisualPrintWorkSurfaceExport : public QObject, VolumeTo3DSurface
{
	Q_OBJECT

public:
	VisualPrintWorkSurfaceExport(QVector<int>& vecROIList, VOLUME_DATA* pVolume, MeshData* pMeshData, EXPORT_3D_SURFACE_MESH_METHOD method, bool bExtract = false);
	virtual ~VisualPrintWorkSurfaceExport();

public slots:
	void threadRun();

signals:
	void sig_progress(int, QString);
	void progress(int);
	void finished();
	void delMask(int);
	void sig_updateMeshUI(bool, int, bool);

	void sig_buildRenderBufferTopology(mip::MeshTopology* _p_mesh);
	void sig_renderLater();

private:
	void singleProcess();
	void parallelProcess();

	QVector<int>	m_vecROIList;

	bool			m_bExtract;

	MeshData* m_pMeshData;
};
#endif
