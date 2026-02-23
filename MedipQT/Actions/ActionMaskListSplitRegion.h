#pragma once

#ifndef ACTION_MARKLIST_SPLIT_REGION_H
#define ACTION_MARKLIST_SPLIT_REGION_H

#include "define.h"
#include "graphics/BoundingBox.h"
#include "volumedata.h"
#include <QtWidgets>
#include <QUndoCommand>
#include <qpoint.h>
#include <Qthread>

class WindowBase;
class MyThreadQT;
class VOLUME_DATA;

struct sWorkMaskListSplitRegionResult {
	QString maskName = "";
	muint32 numberOfSplitRegion = 0;
	muint32 length = 0;
	std::shared_ptr<int> pThreadResult;
};

class ActionMaskListSplitRegion : public QUndoCommand
{
public:
	ActionMaskListSplitRegion(VOLUME_DATA* pVolumeData, sWorkMaskListSplitRegionResult &_resultData, QUndoCommand *parent = 0);
	virtual ~ActionMaskListSplitRegion();

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	int						m_id;
	bool					m_first;
	sWorkMaskListSplitRegionResult resultData;
	QVector<QPair<muint32, MaskInfo>> separateMaskVector;

	VOLUME_DATA* m_pVolumeData;
};

class WorkMaskListSplitRegion : public QObject
{
	Q_OBJECT

public:
	WorkMaskListSplitRegion(VOLUME_DATA* pVolumeData)			
	{
		m_pVolumeData = pVolumeData;
	};
	virtual ~WorkMaskListSplitRegion()	{};
	static void updateProgress(int, void*);

private:
	void setProgressValue(int value, bool init = false);

	VOLUME_DATA* m_pVolumeData;
public slots:
	void threadRun();

signals:
	void progress(int);
	void finished();
};
#endif