#pragma once

#ifndef ACTION_IMAGEBOUNDARY_H
#define ACTION_IMAGEBOUNDARY_H

#include "define.h"
#include <QtWidgets>
#include <QUndoCommand>
#include <qpoint.h>
#include <Qthread>
#include "graphics/BoundingBox.h"

class WindowBase;
class MyThreadQT;
class VOLUME_DATA;

class ActionImageBoundary : public QUndoCommand
{
public:
	ActionImageBoundary(VOLUME_DATA * volumData, mask _m, int _mI=0, QUndoCommand *parent = 0);
	virtual ~ActionImageBoundary();

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }
	mask getMask() const { return m_mask; }
private:
	int						m_id;
	int						m_fillCount[2];
	int						m_uid;
	int						m_maskIndex;
	BoundingBoxI			m_orgbox[2];
	mask					m_mask;
	VOLUME_DATA *			m_volumData;
};

class WorkImageBoundary : public QObject
{
	Q_OBJECT

public:
	WorkImageBoundary(WINDOW_TYPE type, muint32 depth, mask m, VOLUME_DATA * volume, int mI)
	{
		_mask = m;
		_maskIndex = mI;
		_volumData = volume;
		_windowType = type;
		_depth = depth;
	}
	virtual ~WorkImageBoundary() {};
public:
	mask _mask;
	int _maskIndex;
	VOLUME_DATA * _volumData;
	WINDOW_TYPE _windowType;
	muint32 _depth;

private:
	void setProgressValue(int value, bool init = false);

public slots:
	void threadRun();

signals:
	void progress(int);
	void finished();
};
#endif
