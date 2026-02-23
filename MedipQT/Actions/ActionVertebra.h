#pragma once

#ifndef WORK_CALCULATE_SLICEPOSITION_H
#define WORK_CALCULATE_SLICEPOSITION_H

#include "define.h"
#include <QtWidgets>
#include <QUndoCommand>
//#include "graphics/BoundingBox.h"
#include <qpoint.h>
#include "volumedata.h"
//#include "Renderer/Slice.h"

//class WindowBase;
//class OpenGLWidget;
//class AnalMPRPlaneView;


class WorkCalculateSlicePostion : public QObject
{
	Q_OBJECT

public:
	WorkCalculateSlicePostion(VOLUME_DATA* pVolumeData, std::vector<int>  VbNumList, int selectType, float cutOffset = 3.0f);
	void setProgressValue(int value, bool init = false);

public slots:
	void threadRun();
signals:
	void progress(int);
	void finished();

private:
	int getLowestSagittalX(unsigned char* pMask3D, mask maskBit, int cx, int cy, int cz);
	int getSlicePostion(QString strMaskName, int returnType);
	QString getVBString(int vbNum);
private:
	float					m_cutOffset;
	std::vector<int>		m_VbNumList;

	int						m_SelectType;

	VOLUME_DATA*			m_pVolumeData;
};


class WorkFindPickMaskName : public QObject
{
	Q_OBJECT

public:
	WorkFindPickMaskName(VOLUME_DATA* pVolumeData, std::vector<QString> & _strMaskNameList, float cutOffset = 3.0f);
	void setProgressValue(int value, bool init = false);

	public slots:
	void threadRun();
signals:
	void progress(int);
	void finished();

private:
	//int getLowestSagittalX(unsigned char* pMask3D, mask maskBit, int cx, int cy, int cz);
private:
	QPoint					m_pick;
	float					m_cutOffset;
	std::vector<QString>*	m_strMaskNameList;
	QString					m_strMaskName;

	VOLUME_DATA*			m_pVolumeData;
};
#endif