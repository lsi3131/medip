#pragma once

#ifndef ACTION_IMAGE_HOLEFILLING_H
#define ACTION_IMAGE_HOLEFILLING_H

#include "define.h"
#include <QtWidgets>
#include <QUndoCommand>
#include <qpoint.h>
#include <Qthread>
#include "graphics/BoundingBox.h"
#include "algorithm/MagicCut.h"

class WindowBase;
class MyThreadQT;
class VOLUME_DATA;

class ActionImageHoleFilling : public QUndoCommand
{
public:
	ActionImageHoleFilling(VOLUME_DATA * volumData, mask _m, int _mI, QUndoCommand *parent = 0);
	virtual ~ActionImageHoleFilling();

	void undo() override;
	void redo() override;
	void Do();
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }
	mask getMask() const { return m_mask; }
private:
	int						m_id;
	int						m_fillCount[2];
	int						m_uid;
	mask					m_mask;
	int						m_maskIndex;
	bool					m_TAState;
	BoundingBoxI			m_orgbox[2];
	VOLUME_DATA *			m_volumData;
};

class WorkImageHoleFilling : public QObject
{
	Q_OBJECT

public:
	WorkImageHoleFilling(int uid, mask m, VOLUME_DATA * volume, int mI=0, mip::HoleFilling::Mode mode = mip::HoleFilling::Mode::Mode3D)
	{
		_uid = uid;
		_mask = m;
		_maskIndex = mI;
		_volumData = volume;
		_mode = mode;
	}
	virtual ~WorkImageHoleFilling() {};

	static void updateProgress(int, void*);
public:
	mask _mask;
	int	_maskIndex;
	int _uid;
	mip::HoleFilling::Mode _mode;
	VOLUME_DATA * _volumData;

private:
	void setProgressValue(int value, bool init = false);

	struct Position3D
	{
		int x;
		int y;
		int z;
	};

public slots:
	void threadRun();

signals:
	void progress(int);
	void finished();
};
#endif
