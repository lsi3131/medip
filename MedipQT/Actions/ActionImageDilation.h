#pragma once

#ifndef ACTION_IMAGEDILATION_H
#define ACTION_IMAGEDILATION_H

#include "define.h"
#include <QtWidgets>
#include <QUndoCommand>
#include <qpoint.h>
#include <Qthread>
#include "graphics/BoundingBox.h"

class WindowBase;
class MyThreadQT;
class VOLUME_DATA;

class ActionImageDilation : public QUndoCommand
{
public:
	ActionImageDilation(VOLUME_DATA * volumData, mask _m, int _mI=0, QUndoCommand *parent = 0);
	virtual ~ActionImageDilation();

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }
	mask getMask() const { return m_mask; }
private:
	int						m_id;
	int						m_fillCount[2];
	int						m_uid;
	bool					m_TAState;
	BoundingBoxI			m_orgbox[2];
	mask					m_mask;
	int						m_maskIndex;
	VOLUME_DATA *			m_volumData;
};

class WorkImageDilation : public QObject
{
	Q_OBJECT

public:
	WorkImageDilation(mask m, VOLUME_DATA * volume, int mI=0, uchar direction=0)
	{
		_mask = m;
		_maskIndex = mI;
		_volumData = volume;
		_direction = direction;
	}
	virtual ~WorkImageDilation() {};
	static void updateProgress(int, void*);
public:
	mask _mask;
	uchar _direction;
	int _maskIndex;
	VOLUME_DATA * _volumData;


private:
	void setProgressValue(int value, bool init = false);

public slots:
	void threadRun();

signals:
	void progress(int);
	void finished();
};
#endif
