#pragma once

#ifndef WORKLOAD3MF_H
#define WORKLOAD3MF_H

#include <QtWidgets>

#include "DataContext.h"

/*
@brief		3mf load Å¬·¡½º
@author		byPHS
@date		2021.02.15
*/
class WorkLoad3MF : public QObject
{
	Q_OBJECT

public:
	WorkLoad3MF(DataContext* pDataContext, QStringList& _list_files);
	~WorkLoad3MF();

signals:
	void sig_updateProgress(int, QString);
	void finished();
	void sig_updateMeshUI(bool, int, bool);

	void sig_buildRenderBufferTopology(mip::MeshTopology* _p_mesh);
	void sig_renderLater();
	void sig_updateUI();

public slots:
	void threadRun();

private:
	DataContext* m_pDataContext;

	QStringList m_list_files;

	VOLUME_DATA* m_pVolumeData;
	MeshData* m_pMeshData;
};

//#endif
#endif
