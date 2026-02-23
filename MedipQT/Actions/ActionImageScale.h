#pragma once

#ifndef ACTION_IMAGE_SCALE_H
#define ACTION_IMAGE_SCALE_H

#include "define.h"
#include <QUndoCommand>
#include <Qthread>




class VOLUME_DATA;
class BoundingBoxI;

struct ScaleHeader
{
	mint32 orgX;
	mint32 orgY;
	mint32 orgZ;
	float	orgSpX;
	float	orgSpY;
	float	orgSpZ;

	mint32 newX;
	mint32 newY;
	mint32 newZ;
	float	newSpX;
	float	newSpY;
	float	newSpZ;
};


class ActionImageScale : public QUndoCommand
{
public:
	ActionImageScale(VOLUME_DATA* pVolumeData, QUndoCommand *parent = 0);
	virtual ~ActionImageScale();

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }
private:
	int						m_id;
	int						m_maskIndex;
	int						maskCount;
	int						*voxelUID;
	bool					*m_TAState;
	int						*voxelCounts[2];
	BoundingBoxI			*boxes[2];
	ScaleHeader				m_scInfo;
	VOLUME_DATA *			m_volumData;
};







class WorkImageScale : public QObject
{
	Q_OBJECT

public:
	WorkImageScale(VOLUME_DATA* pVolume, bool isDownscale = true)
	{
		_volumeData = pVolume;
		_Downscale = isDownscale;
	};

	virtual ~WorkImageScale();

	static void updateProgress(int, void*);
private:
	void setProgressValue(int value, bool init = false);

public slots:
	void threadRun();

signals:
	void progress(int);
	void finished();

private:
	int _addValue;
	bool	_Downscale;
	VOLUME_DATA *_volumeData;
};


class WorkImageCrop : public QObject
{
	Q_OBJECT

public:
	WorkImageCrop(VOLUME_DATA* pVolumeData);


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

class ActionImageCrop : public QUndoCommand
{
public:
	ActionImageCrop(VOLUME_DATA* pVolumeData, QUndoCommand *parent = 0);
	virtual ~ActionImageCrop();

	void undo() override;
	void redo() override;
	int id() const override { return m_id; }
private:
	int						m_id;
	int						m_maskIndex;
	int						maskCount;
	int						*voxelUID;
	int						*voxelCounts[2];
	bool					*m_TAState;
	BoundingBoxI			*boxes[2];
	VOLUME_DATA*			m_volumData;
};
#endif