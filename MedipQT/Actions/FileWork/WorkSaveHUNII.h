#pragma once

#ifndef WORK_SAVE_HUNII_H
#define WORK_SAVE_HUNII_H

#include <QtWidgets>

#include "graphics/volumedata.h"

// nii hu
class WorkSaveHUNII : public QObject
{
	Q_OBJECT

public:
	WorkSaveHUNII(VOLUME_DATA* pVolumeData, mask _m, int _mI, const QString& strFileName);

private:
	mask _m;
	int _mI;
	QString _FileName;
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
