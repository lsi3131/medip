#pragma once

#ifndef WORK_MESHTOVTK_H
#define WORK_MESHTOVTK_H

#include "define.h"

#include <QtWidgets>

#include "graphics/volumedata.h"
#include "graphics/MeshData.h"

namespace mip
{
	class MeshTopology;
}

class WorkMeshToVTK : public QObject
{
	Q_OBJECT

public:
	WorkMeshToVTK(VOLUME_DATA* pVolumeData, QString& strFilename, mip::MeshTopology* m, float sc, bool bPatientCoordinate = false);

	void setProgressValue(int value, bool init = false);
	static void upateprogress(vtkObject*, unsigned long eid, void* clientdata, void* calldata);

private:
	QString			m_vtkFilePath;
	mip::MeshTopology* _mesh;
	float			_scale;
	int				_addValue;
	bool			m_bPatientCoordinate;
	VOLUME_DATA* m_pVolumeData;

public slots:
	void threadRun();

signals:
	void progress(int);
	void finished();
};
#endif
