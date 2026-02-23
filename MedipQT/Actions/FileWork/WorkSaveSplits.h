#pragma once

#ifndef WORK_SAVE_SPLITS_H
#define WORK_SAVE_SPLITS_H

#include <QtWidgets>

#include "defineMEDIP.h"

#include "graphics/volumedata.h"

class WorkSaveSplits : public QObject
{
	Q_OBJECT

public:
	WorkSaveSplits(VOLUME_DATA* pVolumeData, mask _m, int _mI, int uid, int spCount, QString filePath, int fileType = 0);

private:
	mask	_m;
	int		_mI;
	int		_spCount;
	int		_uid;
	int		_fileType;
	QString _filePath;
	int _addValue;

	VOLUME_DATA* m_pVolumeData;

	void setProgressValue(int value, bool init = false);

public slots:
	void threadRun();

signals:
	void progress(int);
	void finished();
};
#endif
