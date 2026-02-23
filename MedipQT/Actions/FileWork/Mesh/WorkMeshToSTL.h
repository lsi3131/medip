#pragma once

#ifndef WORK_MESHTOSTL_H
#define WORK_MESHTOSTL_H

#include "define.h"

#include <QtWidgets>

#include "graphics/volumedata.h"
#include "graphics/MeshData.h"

class WorkMeshToSTL : public QObject
{
	Q_OBJECT
public:
	WorkMeshToSTL(VOLUME_DATA* pVolumeData, MeshData* pMeshData, bool bPatientCoordinate = false);
	WorkMeshToSTL(VOLUME_DATA* pVolumeData, MeshData* pMeshData, QString& strFilename, float sc, bool bPatientCoordinate = false);

	void setProgressValue(int value, bool init = false);

	void Write(mip::MeshTopology* pMesh);

public slots:
	void threadRun();

signals:
	void progress(int);
	void finished();
private:

	void single_process(mip::MeshTopology* pMesh);
	void multi_process();

	float			_scale;
	QString _strFilename;
	mint32 _lUID;

	bool m_bPatientCoordinate;

	MeshData* m_pMeshData;
	VOLUME_DATA* m_pVolumeData;
};
#endif
