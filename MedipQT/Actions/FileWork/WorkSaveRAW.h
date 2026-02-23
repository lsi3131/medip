#pragma once

#ifndef WORK_SAVE_RAW_H
#define WORK_SAVE_RAW_H

#include <QtWidgets>

#include "graphics/volumedata.h"

class WorkSaveRAW : public QObject
{
	Q_OBJECT

public:
	WorkSaveRAW(VOLUME_DATA* pVolumeData, mask _m, int _mI, const QString& strFileName);

private:
	VOLUME_DATA* m_pVolumeData;
	QString _FileName;
	int _addValue;
	mask _m;
	int _mI;
	void setProgressValue(int value, bool init = false);

public slots:
	void threadRun();

signals:
	void progress(int);
	void finished();
};
#endif
