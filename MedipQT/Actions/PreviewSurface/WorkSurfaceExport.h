#pragma once

#ifndef WORK_SURFACE_EXPORT_H
#define WORK_SURFACE_EXPORT_H

#include <QtWidgets>

#include "DataContext.h"

#include "algorithm/VolumeToSurface3D/VolumeTo3DSurface.h"

class WorkSurfaceExport : public QObject, VolumeTo3DSurface
{
	Q_OBJECT

public:
	WorkSurfaceExport(
		QVector<int>& vecExportList,
		DataContext* pDataContext,
		EXPORT_3D_SURFACE_MESH_METHOD		method,
		bool						bExtract = false,
		bool						bPatientCoordinate = false
	);

	virtual ~WorkSurfaceExport();

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
	void sig_updateUI();

private:
	void singleProcess();
	void parallelProcess();

	DataContext* m_pDataContext;

	bool					m_bExtract;

	bool					m_bPatientCoordinate;

	QVector<int>			m_vecExportList;
};
#endif
