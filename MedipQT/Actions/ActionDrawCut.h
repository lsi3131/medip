#pragma once

#ifndef ACTION_DRAWCUT_H
#define ACTION_DRAWCUT_H

#include "define.h"
#include <QtWidgets>
#include <QUndoCommand>
#include <graphics\BoundingBox.h>

class WindowBase;
class VOLUME_DATA;

class ActionImageDrawCut : public QUndoCommand
{
public:
	ActionImageDrawCut(VOLUME_DATA * volumData, mask _m, int _mI=0, QUndoCommand *parent = 0);

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	int						m_id;
	mask					m_mask;
	int						m_maskIndex;
	int						m_maskNumber;
	int						m_seedVoxel[2];
	int						m_fillCount;
	BoundingBoxI			m_orgbox[2];
	BoundingBoxI			m_seedBox[2];
	bool					m_TAState;
	VOLUME_DATA *			m_volumData;
};


class WorkImageDrawCut : public QObject
{
	Q_OBJECT

public:
	WorkImageDrawCut(mask m, VOLUME_DATA * volume, int mI=0, double GPC_lambda=50.0)
	{
		_mask = m;
		_maskIndex = mI;
		_volumData = volume;
		_result = 0;
		_GPC_lambda = GPC_lambda;
	}
	virtual ~WorkImageDrawCut() {};
public:
	mask			_mask;
	VOLUME_DATA *	_volumData;
	int				_result;
	int				_maskIndex;
	double			_GPC_lambda;
	void setProgressValue(int value, bool init=false);
	static void updateprogress(int value, void * data);
	static void updateprogress2(int value, void * data);

public:
	void processDrawCut();
	void processTargetedDrawCut();


public slots:
	void threadRun();

signals:
	void progress(int);
	void finished();
};
#endif
