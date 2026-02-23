#pragma once

#ifndef WORK_VTK_OPEN_H
#define WORK_VTK_OPEN_H

#include "define.h"

#include <QtWidgets>

#include "DataContext.h"

class WorkVTKOpen : public QObject
{
	Q_OBJECT

public:
	//WorkVTKOpen(QStringList filename, mip::MeshTopology *_mesh,VOLUME_DATA* volume_data)
	WorkVTKOpen(QStringList filename, DataContext* pDataContext);

	void setProgressValue(int value, bool init = false);

public slots:
	void threadRun();
signals:
	void sig_updateProgress(int, QString);
	void sig_updateUI();
	void progress(int);
	void finished();

private:
	DataContext* m_pDataContext;
	QStringList			_strFilename;
	int				_addValue;
	//mip::MeshTopology * _meshCore;
};
#endif
