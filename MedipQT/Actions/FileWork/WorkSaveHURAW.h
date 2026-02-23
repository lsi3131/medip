#pragma once

#ifndef WORK_SAVE_HURAW_H
#define WORK_SAVE_HURAW_H

#include <QtWidgets>

#include "defineMEDIP.h"

#include "graphics/volumedata.h"

class WorkSaveHURAW : public QObject
{
	Q_OBJECT

public:
	WorkSaveHURAW(VOLUME_DATA* pVolumeData, mask _m, int _mI, const QString& strFileName, bool patchy = false, BoundingBoxI box = BoundingBoxI());

private:
	VOLUME_DATA* m_pVolumeData;
	QString _FileName;
	int _addValue;
	mask _m;
	int _mI;
	bool _patchy;
	BoundingBoxI _box;
	void setProgressValue(int value, bool init = false);

public slots:
	void threadRun();

signals:
	void progress(int);
	void finished();
};
#endif
