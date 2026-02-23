#pragma once

#ifndef WORK_MESHTOOBJ_H
#define WORK_MESHTOOBJ_H

#include "define.h"

#include <QtWidgets>

#include "graphics/volumedata.h"
#include "graphics/MeshData.h"

namespace mip
{
	class MeshTopology;
}

class WorkMeshToOBJ : public QObject
{
	Q_OBJECT

public:
	WorkMeshToOBJ(VOLUME_DATA* pVolumeData, QString& strFilename, mip::MeshTopology* m, float sc, bool bPatientCoordinate = false);

	void setProgressValue(int value, bool init = false);
	static void upateprogress(vtkObject*, unsigned long eid, void* clientdata, void* calldata);

private:
	QString			m_objFilepath;
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
