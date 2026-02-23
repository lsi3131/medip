#pragma once

#include <QtWidgets>

#include "graphics/volumedata.h"
#include "MeshData.h"

class WindowManager;

class WorkSaveMIP : public QObject
{
	Q_OBJECT

public:
	WorkSaveMIP(const QString& mipFilePath, WindowManager* pWinManager, VOLUME_DATA* pVolumeData, MeshData* pMeshData);

private:
	void setProgressValue(int value, bool init = false);

public slots:
	void threadRun();

signals:
	void progress(int);
	void finished();
private:
	QString m_mipFilePath;
	int m_addValue;
	WindowManager* m_pWinManager;
	VOLUME_DATA* m_pVolumeData;
	MeshData* m_pMeshData;
};
