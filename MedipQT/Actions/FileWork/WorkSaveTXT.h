#pragma once

#ifndef WORK_SAVE_TXT_H
#define WORK_SAVE_TXT_H

#include <QtWidgets>

#include "graphics/volumedata.h"

class WorkSaveTXT : public QObject
{
	Q_OBJECT

public:
	WorkSaveTXT(VOLUME_DATA* pVolumeData, mask _m, int _mI, int _index, const QString& strFileName);

private:
	VOLUME_DATA* m_pVolumeData;
	QString _FileName;
	int _addValue;
	int _index;
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
