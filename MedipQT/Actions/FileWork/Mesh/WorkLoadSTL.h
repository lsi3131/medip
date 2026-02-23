/*
@brief		Stl load 클래스
@author		허 건 대리
@date		2020.09.04
*/
#pragma once

#ifndef WORKLOADSTL_H
#define WORKLOADSTL_H

#include <QtWidgets>

#include "DataContext.h"

class WorkLoadSTL : public QObject
{
	Q_OBJECT

public:
	WorkLoadSTL(DataContext* pDataContext, QStringList& _list_files);
	~WorkLoadSTL() {}

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
	QStringList	 m_list_files;

	DataContext* m_pDataContext;
};
#endif
