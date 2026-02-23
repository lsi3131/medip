#pragma once

#ifndef WORKLOADOBJ_H
#define WORKLOADOBJ_H

#include <QtWidgets>

#include "DataContext.h"

/*
@brief		OBJ load 클래스
@author		허 건 과장
@date		2021.07.26
*/
class WorkLoadOBJ : public QObject
{
	Q_OBJECT

public:
	WorkLoadOBJ(DataContext* pDataContext, QStringList& _list_files);
	~WorkLoadOBJ() {}

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
