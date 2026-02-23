#pragma once

#ifndef WORK_SAVE_NII_H
#define WORK_SAVE_NII_H

#include <QtWidgets>

#include "graphics/volumedata.h"

// nii HU region
class WorkSaveNII : public QObject
{
	Q_OBJECT
public:
	WorkSaveNII(const QString& strFileName, VOLUME_DATA* pVolumeData, bool patchy = false, BoundingBoxI box = BoundingBoxI());

private:
	QString _FileName;
	bool	_patchy;
	BoundingBoxI _box;
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
