#pragma once

#ifndef WORK_NII_OPEN_H
#define WORK_NII_OPEN_H

#include <QtWidgets>

#include "graphics/volumedata.h"

class WorkNIIOpen : public QObject
{
	Q_OBJECT

public:
	WorkNIIOpen(const QString& filename, VOLUME_DATA* volume_data);

	void setProgressValue(int value, bool init = false);

public slots:
	void threadRun();
signals:
	void progress(int);
	void finished();
	void resetRC();
	void initUI(int, int);

private:
	VOLUME_DATA* _volumData;
	QString			_strFilename;
	int				_result;
	int				_addValue;
};
#endif
