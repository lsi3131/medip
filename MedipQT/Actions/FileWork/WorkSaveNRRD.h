#pragma once

#ifndef WORK_SAVE_NRRD_H
#define WORK_SAVE_NRRD_H

#include <QtWidgets>

#include "graphics/volumedata.h"

class WorkSaveNRRD : public QObject
{
	Q_OBJECT
public:
	WorkSaveNRRD(VOLUME_DATA* pVolumeData, const QString& strFileName, bool patchy = false, BoundingBoxI box = BoundingBoxI());

private:
	VOLUME_DATA* m_pVolumeData;
	QString _FileName;
	bool	_patchy;
	BoundingBoxI _box;
	int _addValue;
	void setProgressValue(int value, bool init = false);

public slots:
	void threadRun();

signals:
	void progress(int);
	void finished();
};
#endif
