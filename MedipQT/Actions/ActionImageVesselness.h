#pragma once

#ifndef ACTION_IMAGE_VESSELNESS_H
#define ACTION_IMAGE_VESSELNESS_H

#include "define.h"
#include <QtWidgets>
#include <QUndoCommand>
#include "graphics/BoundingBox.h"
#include <qpoint.h>
#include <Qthread>

class WindowBase;
class MyThreadQT;
class VOLUME_DATA;

class ActionImageVesselness : public QUndoCommand
{
public:
	ActionImageVesselness(VOLUME_DATA * volumData, mask _m,int _mI=0, QUndoCommand *parent = 0);
	virtual ~ActionImageVesselness();

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }
	mask getMask() const { return m_mask; }
private:
	int						m_id;
	int						m_fillCount[2];
	int						m_uid;
	mask					m_mask;
	BoundingBoxI			m_orgbox[2];
	bool					m_TAState;
	int						m_maskIndex;
	VOLUME_DATA *			m_volumData;
};

class WorkImageVesselness : public QObject
{
	Q_OBJECT

public:
	WorkImageVesselness(mask m, VOLUME_DATA * volume, int mI=0)
	{
		_mask = m;
		_maskIndex = mI;
		_volumData = volume;
	}
	virtual ~WorkImageVesselness() {};
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
