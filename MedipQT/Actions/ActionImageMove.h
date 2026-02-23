#pragma once

#ifndef ACTION_IMAGE_MOVE_H
#define ACTION_IMAGE_MOVE_H

#include "define.h"
#include <QtWidgets>
#include <QUndoCommand>
#include <qpoint.h>
#include <Qthread>
#include "graphics/BoundingBox.h"

class WindowBase;
class MyThreadQT;
class VOLUME_DATA;

class ActionImageMove : public QUndoCommand
{
public:
	ActionImageMove(VOLUME_DATA * volumData, mask _m, int _mI=0, QUndoCommand *parent = 0);
	virtual ~ActionImageMove();

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

class WorkImageMove : public QObject
{
	Q_OBJECT

public:
	WorkImageMove(mask m, VOLUME_DATA * volume, int mI=0, uchar direction=0)
	{
		m_mask = m;
		m_maskIndex = mI;
		m_volumData = volume;
		m_eDirection = direction;
	}
	virtual ~WorkImageMove() {};
	static void updateProgress(int, void*);
public:
	mask m_mask;
	uchar m_eDirection;
	int m_maskIndex;
	VOLUME_DATA * m_volumData;


private:
	void setProgressValue(int value, bool init = false);

public slots:
	void threadRun();

signals:
	void progress(int);
	void finished();
};
#endif
