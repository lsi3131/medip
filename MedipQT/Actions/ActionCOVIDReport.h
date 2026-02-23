#ifndef ACTIONCOVIDREPORT_H
#define ACTIONCOVIDREPORT_H

#pragma once
#include "define.h"
#include <QtWidgets>
#include <QUndoCommand>

class WindowBase;
class MyThreadQT;
class VOLUME_DATA;

class ActionCOVIDReport
{
public:
	ActionCOVIDReport();
	~ActionCOVIDReport();

private:
	
};

struct COVID_TEXTURE_FEATURE
{	
	std::string strROIName = "";
	float	fObjectVolume = 0.f;
	float	fMeanHU = 0;
	float	fStdDeviation = 0.f;	
	float	fPneumoniaBurden = 0.f;
	float	fRatio = 0.f;
};

struct COVID_REPORT
{	
	int		nUID = -1;
	int		nAxialDepth = 0;	// z
	int		nCoronalDepth = 0;	// y
	int		nSaggitalDepth = 0;	// x
	std::vector<COVID_TEXTURE_FEATURE> vecFeature;
};

class WorkCOVIDReport : public QObject
{
	Q_OBJECT
public:
	WorkCOVIDReport(VOLUME_DATA * volume, COVID_REPORT* pOutReport);
	virtual ~WorkCOVIDReport() {};

	//static void updateProgress(int, void*);

private:
	void setProgressValue(int value, bool init = false);
		
	VOLUME_DATA *m_pVolumeData;
	COVID_REPORT *m_pOutReport;
	
public slots:
	void threadRun();

signals:
	void progress(int);
	void finished();
private:

};

#endif // ACTIONCOVIDREPORT_H
