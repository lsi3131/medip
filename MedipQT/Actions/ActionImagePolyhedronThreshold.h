#pragma once

#ifndef ACTION_IMAGE_POLYHEDRON_THRESHOLD_H
#define ACTION_IMAGE_POLYHEDRON_THRESHOLD_H

#include "define.h"
#include <QtWidgets>
#include <QUndoCommand>
#include <qpoint.h>
#include "graphics/BoundingBox.h"
#include <Qthread>

class WindowBase;
class MyThreadQT;
class VOLUME_DATA;


class ActionImagePolyhedronThreshold : public QUndoCommand
{
public:
	ActionImagePolyhedronThreshold(VOLUME_DATA * volumData, mask _m, int _maskIndex=0, QUndoCommand *parent = 0);
	virtual ~ActionImagePolyhedronThreshold();

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }
	mask getMask() const { return m_mask; }
private:
	int						m_id;
	mask					m_mask;
	int						m_fillCount[2];
	BoundingBoxI			m_orgbox[2];
	int						m_uid;
	bool					m_TAState;
	VOLUME_DATA *			m_volumData;
	int						m_maskIndex;
	
};

class WorkImagePolyhedronThreshold : public QObject
{
	Q_OBJECT

public:
	WorkImagePolyhedronThreshold(mask m, VOLUME_DATA * volume, int mI=0, uchar direction = 0)
	{
		_mask = m;
		_maskIndex = mI;
		_volumData = volume;
		_direction = direction;
	}
	virtual ~WorkImagePolyhedronThreshold() {};
	static void updateProgress(int, void*);
public:
	mask _mask;
	int _maskIndex;
	uchar	_direction;

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
