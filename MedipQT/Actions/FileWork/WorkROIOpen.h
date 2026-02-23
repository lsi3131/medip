#pragma once

#ifndef WORK_ROI_OPEN_H
#define WORK_ROI_OPEN_H

#include <QtWidgets>

#include "graphics/volumedata.h"

class WorkROIOpen : public QObject
{
	Q_OBJECT

public:
	WorkROIOpen(QString filename, VOLUME_DATA* volume_data);
	void setProgressValue(int value, bool init = false);

public slots:
	void threadRun();
signals:
	void progress(int);
	void finished();

private:
	VOLUME_DATA* _volumData;
	QString			_strFilename;
	int				_addValue;
};
#endif
