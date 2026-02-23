#pragma once

#ifndef WORK_SAVE_JSON_H
#define WORK_SAVE_JSON_H

#include <QtWidgets>

#include "graphics/volumedata.h"

typedef struct {
	int x;
	int y;
	int z;
} sMaskCoordinate;

typedef struct {
	QString maskName;
	QVector<sMaskCoordinate> curMaskCoordinateVec;
} sMaskCoordinateArrayData;

class WorkSaveJSON : public QObject
{
	Q_OBJECT

public:
	WorkSaveJSON(const QString& strFileName, QVector<QPair<QString, QString>>& _clinicalInfo, QVector<sLocalClinicalInfo>& _localClinicalInfo, VOLUME_DATA* pVolumeData);

private:
	QString _FileName;
	QVector<QPair<QString, QString>> clinicalInfo;
	QVector<sLocalClinicalInfo> localClinicalInfo;

	VOLUME_DATA* m_pVolumeData;

	void setProgressValue(int value, bool init = false);

public slots:
	void threadRun();

signals:
	void progress(int);
	void finished();
};
#endif
