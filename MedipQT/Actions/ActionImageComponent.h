#pragma once

#ifndef ACTION_IMAGECOMPONENT_H
#define ACTION_IMAGECOMPONENT_H

#include "define.h"
#include <QtWidgets>
#include <QUndoCommand>
#include <qpoint.h>
#include <Qthread>
#include "graphics/BoundingBox.h"

class WindowBase;
class MyThreadQT;
class VOLUME_DATA;

class ActionImageComponent : public QUndoCommand
{
public:
	ActionImageComponent(VOLUME_DATA* volumData, mask _m, int _mI = 0, QUndoCommand* parent = 0);
	virtual ~ActionImageComponent();

	void undo() override;
	void redo() override;
	void Do();
	bool mergeWith(const QUndoCommand* command) override;
	int id() const override { return m_id; }
	mask getMask() const { return m_mask; }
private:
	int m_id;
	int m_fillCount[2];
	int m_uid;
	bool m_TAState;
	BoundingBoxI m_orgbox[2];
	mask m_mask;
	int m_maskIndex;
	VOLUME_DATA* m_volumData;
};

class WorkImageComponent : public QObject
{
	Q_OBJECT

public:
	WorkImageComponent(mask m, VOLUME_DATA* volume, int mI = 0, int componentNum = 1)
	{
		_mask = m;
		_maskIndex = mI;
		_volumData = volume;
		_componentNum = componentNum;
	}
	virtual ~WorkImageComponent() {};
	static void updateProgress(int, void*);
public:
	mask _mask;
	int _componentNum;
	int _maskIndex;
	VOLUME_DATA* _volumData;

private:
	void setProgressValue(int value, bool init = false);

public slots:
	void threadRun();

signals:
	void progress(int);
	void finished();
};

class CustomImageComponent
{
public:
	CustomImageComponent(int nCx, int nCy, int nCz, int componentNum = 1, int nThreshold = 0)
	{
		m_nCx = nCx;
		m_nCy = nCy;
		m_nCz = nCz;
		m_nComponentNum = componentNum;
		m_nThreshold = nThreshold;
	}
	~CustomImageComponent() {};
	void startFunc(const std::vector<unsigned char>& vecInput, std::vector<mint8>& vecResult);

private:
	int m_nCx = 0;
	int m_nCy = 0;
	int m_nCz = 0;
	int m_nThreshold = 0;
	int m_nComponentNum = 0;
};
#endif
