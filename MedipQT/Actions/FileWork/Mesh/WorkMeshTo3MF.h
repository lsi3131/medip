#pragma once

#ifndef WORK_MESHTO3MF_H
#define WORK_MESHTO3MF_H

#include <QtWidgets>

#include "graphics/volumedata.h"
#include "graphics/MeshData.h"

class WorkMeshTo3MF : public QObject
{
	Q_OBJECT
public:
	WorkMeshTo3MF(VOLUME_DATA* pVolumeData, MeshData* pMeshData, QString& strFilename, mip::MeshTopology*, float sc, bool bPatientCoordinate = false);

	void setProgressValue(int value, bool init = false);
public slots:
	void threadRun();

signals:
	void progress(int);
	void finished();
private:
	mip::MeshTopology* _mesh;
	float			_scale;
	QString _strFilename;
	mint32 _lUID;
	bool m_bPatientCoordinate;

	VOLUME_DATA* m_pVolumeData;
	MeshData* m_pMeshData;

};
//#endif
#endif
