#include "stdafx.h"
#include "WorkDicomOpen.h"

WorkDicomOpen::WorkDicomOpen(const QString& strFilename, VOLUME_DATA* volume)
{
	_volumData = volume;
	_strFilename = strFilename;
	_result = 0;
}

WorkDicomOpen::~WorkDicomOpen()
{
}

void WorkDicomOpen::threadRun()
{
	emit finished();
}