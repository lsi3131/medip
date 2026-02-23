/*
@brief		Brush Data 생성 클래스
@author		byPHS
@date		2021.06.24
*/

#pragma once

#ifndef WORK_BRUSH_THREAD_H
#define WORK_BRUSH_THREAD_H

#include <QtWidgets>

#include "DataContext.h"

namespace mip
{
	class MeshTopology;
};

class WorkBrushThread : public QObject
{
	Q_OBJECT
public:
	WorkBrushThread(DataContext* pDataContext);
	WorkBrushThread(DataContext* pDataContext, mip::MeshTopology* pMesh, mip::VECTOR3 pos);

	void setProgressValue(int value, bool init = false);

public:
	bool b_loop;

public slots:
	void threadRun();


signals:
	void sig_updateProgress(int, QString);
	void progress(int);
	void finished();

private:
	mip::MeshTopology* _mesh;
	mip::VECTOR3		_position;

	DataContext* m_pDataContext;
};
#endif
