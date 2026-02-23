#pragma once

#ifndef ACTION_IMAGE_FISSURENESS_H
#define ACTION_IMAGE_FISSURENESS_H

#include "define.h"
#include <QtWidgets>
#include <QUndoCommand>
#include <qpoint.h>
#include <Qthread>
#include "graphics/BoundingBox.h"

class WindowBase;
class MyThreadQT;
class VOLUME_DATA;

class ActionImageFissureness : public QUndoCommand
{
public:
	ActionImageFissureness(VOLUME_DATA * volumData, mask _m,int _mI=0, QUndoCommand *parent = 0);
	virtual ~ActionImageFissureness();

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }
	mask getMask() const { return m_mask; }
private:
	int						m_id;
	int						m_uid;
	int						m_fillCount[2];
	BoundingBoxI			m_orgbox[2];
	mask					m_mask;
	int						m_maskIndex;
	bool					m_TAState;
	VOLUME_DATA *			m_volumData;
};

class WorkImageFissureness : public QObject
{
	Q_OBJECT

public:
	WorkImageFissureness(mask m, VOLUME_DATA * volume, int mI=0)
	{
		_mask = m;
		_maskIndex = mI;
		_volumData = volume;
	}
	virtual ~WorkImageFissureness() {};
public:
	mask _mask;
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
