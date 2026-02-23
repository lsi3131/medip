#pragma once

#ifndef WORK_SAVE_COORDINATENII_H
#define WORK_SAVE_COORDINATENII_H

#include <QtWidgets>

#include "graphics/volumedata.h"

class WorkSaveCoordinateNII : public QObject
{
	Q_OBJECT

public:
	WorkSaveCoordinateNII(VOLUME_DATA* pVolumeData, mask _m, int _mI, const QString& strFileName);

private:
	VOLUME_DATA* m_pVolumeData;
	mask _m;
	int _mI;
	QString _FileName;
	int _addValue;
	void setProgressValue(int value, bool init = false);

public slots:
	void threadRun();

signals:
	void progress(int);
	void finished();
};
#endif
