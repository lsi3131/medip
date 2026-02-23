#pragma once

#ifndef WORK_DICOM_OPEN_H
#define WORK_DICOM_OPEN_H

#include <QtWidgets>

#include "graphics/volumedata.h"

class WorkDicomOpen : public QObject
{
	Q_OBJECT

public:
	WorkDicomOpen(const QString& strFilename, VOLUME_DATA* volume);
	virtual ~WorkDicomOpen();
public:
	VOLUME_DATA* _volumData;
	QString			_strFilename;
	int				_result;

public slots:
	void threadRun();
signals:
	void progress(int);
	void finished();
};
#endif
