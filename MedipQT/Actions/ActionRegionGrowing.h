#pragma once

#ifndef ACTION_REGION_GROWING_H
#define ACTION_REGION_GROWING_H

#include "define.h"
#include <QtWidgets>
#include <QUndoCommand>
#include "graphics/BoundingBox.h"
#include <qpoint.h>
#include <Qthread>
#include "algorithm/MagicCut.h"

class WindowBase;
class MyThreadQT;
class VOLUME_DATA;
class QProgressDialog;

class ActionRegionGrowing : public QUndoCommand
{
public:
	ActionRegionGrowing(LAYER_RG_SHORTCUT type,VOLUME_DATA * volumData, mask _m, int _mI=0, QUndoCommand *parent = 0);
	~ActionRegionGrowing();

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }
	mask getMask() const { return m_mask; }
private:
	LAYER_RG_SHORTCUT		m_type;
	int						m_id;
	int						m_fillCount[2];
	BoundingBoxI			m_orgbox[2];
	int						m_uid;
	mask					m_mask;
	int						m_maskIndex;
	bool					m_TAState;
	VOLUME_DATA *			m_volumData;
};

class WorkRegionGrowing : public QObject
{
	Q_OBJECT

public:
	WorkRegionGrowing(mint32 x, mint32 y, mint32 z, mint32 upper, muint32 lower, mask m,
		bool underROI, VOLUME_DATA * volume, int mI, QVector<QVector3D> *points,
		bool connect_6=true, LAYER_RG_SHORTCUT ctrlApply=LAYER_RG_NONE)
	{
		_x = x;
		_y = y;
		_z = z;
		_upper = upper;
		_lower = lower;
		_mask = m;
		_maskIndex = mI;
		_volumData = volume;
		_underROI = underROI;
		_connect_6 = connect_6;

		rg_s.clear();

		for (int i = 0; i < points->size(); i++)
		{
			mip::Position3D pt;
			QVector3D vec = points->at(i);
			pt.x = vec.x();
			pt.y = vec.y();
			pt.z = vec.z();

			rg_s.push_back(pt);
		}

		shortcutRG = ctrlApply;

		if (shortcutRG != LAYER_RG_NONE)
			points->clear();

	}
	virtual ~WorkRegionGrowing() {};
public:
	mint32 _x;
	mint32 _y;
	mint32 _z;
	mint32 _upper;
	mint32 _lower;
	mask _mask;
	int _maskIndex;
	bool _underROI;
	bool _connect_6;
	LAYER_RG_SHORTCUT	shortcutRG;
	VOLUME_DATA * _volumData;
	std::vector<mip::Position3D> rg_s;

	void setProgressValue(int value, bool init = false);
	static void updateProgress(int, void*);

public slots:
	void threadRun();

signals:
	void progress(int);
	void finished();
};




class ActionThresholdSelect : public QUndoCommand
{
public:
	ActionThresholdSelect(VOLUME_DATA * volumData, mask _m, int _mI=0,QUndoCommand *parent = 0);
	~ActionThresholdSelect();

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }
	mask getMask() const { return m_mask; }
	void Do();
private:
	int						m_id;
	int						m_fillCount[2];
	BoundingBoxI			m_orgbox[2];
	int						m_uid;
	mask					m_mask;
	int						m_maskIndex;
	bool					m_TAState;
	VOLUME_DATA *			m_volumData;
};



class WorkThresholdSelect : public QObject
{
	Q_OBJECT

public:
	WorkThresholdSelect(mint32 upper, mint32 lower, bool within, mask m, VOLUME_DATA * volume, int mI=0)
	{
		_upper = upper;
		_lower = lower;
		_mask = m;
		_maskIndex = mI;
		_volumData = volume;
		_within = within;
	}
	virtual ~WorkThresholdSelect() {};

private:
	void setProgressValue(int value, bool init = false);

public:
	mint32 _x;
	mint32 _y;
	mint32 _z;
	mint32 _upper;
	mint32 _lower;
	mask _mask;
	bool _within;
	int _maskIndex;

	VOLUME_DATA * _volumData;

	static void updateProgress(int, void*);

public slots:
	void threadRun();

signals:
	void progress(int);
	void finished();
};
#endif
