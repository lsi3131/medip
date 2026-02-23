#pragma once

#ifndef WORK_RAW_OPEN_H
#define WORK_RAW_OPEN_H

#include <QtWidgets>

#include "graphics/volumedata.h"

class WorkRawOpen : public QObject
{
	Q_OBJECT

public:
	WorkRawOpen(QString filename, VOLUME_DATA* volume_data, bool _reverse);

	void setProgressValue(int value, bool init = false);

public slots:
	void threadRun();
signals:
	void progress(int);
	void finished();

private:
	VOLUME_DATA* _volumData;
	QString			_strFilename;
	int				_result;
	int				_addValue;
	bool			_reverse;
};
#endif
