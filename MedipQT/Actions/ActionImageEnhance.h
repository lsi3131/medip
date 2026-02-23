#pragma once

#ifndef ACTION_IMAGE_ENHANCE_H
#define ACTION_IMAGE_ENHANCE_H

#include "define.h"
#include <QtWidgets>
#include <QUndoCommand>
#include <qpoint.h>
#include <Qthread>

class WindowBase;
class MyThreadQT;
class VOLUME_DATA;

class ActionImageEnhance : public QUndoCommand
{
public:
	ActionImageEnhance(VOLUME_DATA * volumData, mask _m, QUndoCommand *parent = 0);
	virtual ~ActionImageEnhance();

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }
	mask getMask() const { return m_mask; }
private:
	int						m_id;
	mask					m_mask;
	bool					m_TAState[MASK_MAX];
	VOLUME_DATA *			m_volumData;
};

class ActionImageEnhanceBackOrigin : public QUndoCommand
{
public:
	ActionImageEnhanceBackOrigin(VOLUME_DATA * volumData, QUndoCommand *parent = 0);
	virtual ~ActionImageEnhanceBackOrigin();

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }
private:
	int						m_id;
	VOLUME_DATA *			m_volumData;
	bool					m_TAState[MASK_MAX];
};

class WorkImageEnhance : public QObject
{
	Q_OBJECT

public:
	WorkImageEnhance(mint32 index, mask m, VOLUME_DATA * volume)
	{
		_index = index;
		_mask = m;
		_volumData = volume;
	}
	virtual ~WorkImageEnhance() {};
public:
	mint32 _index;
	mask _mask;

	VOLUME_DATA * _volumData;
	static void updateProgress(int, void*);
	/* Total variation minimization for rician denoising */
	//int riciandenoise3(double *u, const double *f, int M, int N, int P, double sigma, double lambda, double Tol, int min_x, int max_x, int min_y, int max_y, int min_z, int max_z);
	//int riciandenoise2(double *u, const double *f, int M, int N, double sigma, double lambda, double Tol, int min_x, int max_x, int min_y, int max_y);

private:
	void setProgressValue(int value, bool init = false);

public slots:
	void threadRun();

signals:
	void progress(int);
	void finished();
};



class ActionImageGaussian : public QUndoCommand
{
public:
	ActionImageGaussian(VOLUME_DATA * volumData, QUndoCommand *parent = 0);
	virtual ~ActionImageGaussian();

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }
private:
	int						m_id;
	bool					m_TAState[MASK_MAX];
	VOLUME_DATA *			m_volumData;
};

class WorkImageGaussian : public QObject
{
	Q_OBJECT

public:
	WorkImageGaussian(mint32 index, VOLUME_DATA * volume)
	{
		if (index == 0)
			kernel_size = 3;
		else
			kernel_size = 5;
		_volumData = volume;
	}
	virtual ~WorkImageGaussian() {};
	static void updateProgress(int, void*);
public:
	mint32 kernel_size;

	VOLUME_DATA * _volumData;

private:
	void setProgressValue(int value, bool init = false);
public slots:
	void threadRun();

signals:
	void progress(int);
	void finished();
};


class ActionImageLaplacian : public QUndoCommand
{
public:
	ActionImageLaplacian(VOLUME_DATA * volumData, QUndoCommand *parent = 0);
	virtual ~ActionImageLaplacian();

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }
private:
	int						m_id;
	bool					m_TAState[MASK_MAX];
	VOLUME_DATA *			m_volumData;
};

class WorkImageLaplacian : public QObject
{
	Q_OBJECT

public:
	WorkImageLaplacian(mint32 index, VOLUME_DATA * volume)
	{
		if (index == 0)
			kernel_size = 3;
		else
			kernel_size = 5;
		_volumData = volume;
	}
	virtual ~WorkImageLaplacian() {};
	static void updateProgress(int, void*);
public:
	mint32 kernel_size;

	VOLUME_DATA * _volumData;

	int ConvolutionFiltering(short *output, short *input, int kernel_size, double *kernel, int width, int height, int slice, int minX, int maxX, int minY, int maxY, int minZ, int maxZ);

private:
	void setProgressValue(int value, bool init = false);
	void getLaplacian(int width, int height, int slice, double sigma, double* kernel);
	public slots:
	void threadRun();

signals:
	void progress(int);
	void finished();
};
#endif

