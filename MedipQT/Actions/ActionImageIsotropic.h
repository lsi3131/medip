#pragma once

#ifndef ACTION_IMAGE_ISOTROPIC_CONVERSION_H
#define ACTION_IMAGE_ISOTROPIC_CONVERSION_H

#include "define.h"
#include <QtWidgets>
#include <QUndoCommand>
#include <qpoint.h>
#include <Qthread>
#include "ActionWorkBase.h"

class WindowBase;
class MyThreadQT;
class VOLUME_DATA;
class BoundingBoxI;


class ActionImageIsotropicConversion : public QUndoCommand
{
public:
	ActionImageIsotropicConversion(VOLUME_DATA* pVolumeData,int newCZ,QUndoCommand *parent = 0);
	virtual ~ActionImageIsotropicConversion();

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }
private:
	int						m_id;
	int						m_maskIndex;
	int						maskCount;
	int						*voxelUID;
	int						*voxelCounts[2];
	bool					*m_TAState;
	BoundingBoxI			*boxes[2];
	int						dataCZ[2];
	float					spaceZ[2];
	VOLUME_DATA *			m_volumData;
};

class WorkImageIsotropicConversion : public QObject
{
	Q_OBJECT

public:
	WorkImageIsotropicConversion(VOLUME_DATA* pVolumeData);
	virtual ~WorkImageIsotropicConversion() {};
public:
	mask _mask;
	int _maskIndex;
	VOLUME_DATA * _volumData;

	static void updateProgress(int, void*);

private:
	void setProgressValue(int value, bool init = false);

	public slots:
	void threadRun();
	void Run(void *input, void *output, int    width, int    height, int input_slice, int output_slice, bool isMask = false, mask maskID = 0);

signals:
	void progress(int);
	void finished();
	void strProgress(QString);
};

////
class ActionImageIsotropicConversionModification : public QUndoCommand
{
public:
	ActionImageIsotropicConversionModification(VOLUME_DATA* pVolumData, int newCZ, QUndoCommand *parent = 0);
	virtual ~ActionImageIsotropicConversionModification();

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }
private:
	int						m_id;
	int						m_maskIndex;
	int						maskCount;
	int						*voxelUID;
	int						*voxelCounts[2];
	bool					*m_TAState;
	BoundingBoxI			*boxes[2];
	int						dataCZ[2];
	float					spaceZ[2];
	VOLUME_DATA *			m_volumData;
};

class WorkImageIsotropicConversionModification : public ActionWorkBase
{
	Q_OBJECT

public:
	WorkImageIsotropicConversionModification(VOLUME_DATA* pVolumeData);
	virtual ~WorkImageIsotropicConversionModification() {};
public:
	mask _mask;
	int _maskIndex;
	VOLUME_DATA * _volumData;

	static void updateProgress(int, void*);

private:
//	void setProgressValue(int value, bool init = false);

	public slots:
	void threadRun();

//signals:
//	void progress(int);
//	void finished();
//	void strProgress(QString);
};
#endif

