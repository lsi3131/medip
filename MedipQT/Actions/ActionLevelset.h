#pragma once

#ifndef ACTION_LEVELSET_H
#define ACTION_LEVELSET_H

#include "define.h"
#include <QThread>
#include <QObject>
#include <QUndoCommand>
#include "graphics/volumedata.h"


class ActionLevelset : public QUndoCommand
{
public:
	ActionLevelset(VOLUME_DATA *volumData, mask _m, int _mI = 0, QUndoCommand *parent = 0);
	virtual ~ActionLevelset();

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }
	mask getMask() const { return m_mask; }
private:
	int						m_id;
	int						m_fillCount[2];
	int						m_uid;
	BoundingBoxI			m_orgbox[2];
	mask					m_mask;
	bool					m_TAState;
	int						m_maskIndex;
	VOLUME_DATA *			m_volumData;
};

class WorkLevelset : public QObject
{
	Q_OBJECT

public:
	WorkLevelset(mask m, VOLUME_DATA *volume, int mI = 0, int iter=500, double lambda=0.1, int radious= 10)
	{
		_mask = m;
		_maskIndex = mI;
		_volumeData = volume;
		_iter = iter;
		_lambda = lambda;
		_radious = radious;
	}

	virtual ~WorkLevelset() {};

private:
	void setProgressValue(int value, bool init = false);
public:
	mask _mask;
	int _maskIndex;
	VOLUME_DATA * _volumeData;
	double _lambda;
	int _iter;
	int _radious;

	public slots:
	void threadRun();

signals:
	void progress(int);
	void finished();
};
#endif
