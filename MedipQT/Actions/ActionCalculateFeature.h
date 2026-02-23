#pragma once

#ifndef  ACTION_CALCULATE_FEATURE_H
#define  ACTION_CALCULATE_FEATURE_H

#include "define.h"
#include <QtWidgets>
#include <QUndoCommand>
#include "UI/TASubClassResultCtrl.h"
#include <map>
#include "System/stringManager.h"

// Radiomics)
#include "ImageArray.h"

#include "volumedata.h"

class ActionCalculateFeature
{
public:
	ActionCalculateFeature();
	~ActionCalculateFeature();

	void redo(void);
private:
	QString m_strGroupName;
	mask	m_maskBit;
	int		m_nMaskArrayIndex;
	mask *  m_pMaskVolume;
	TASubClassResultCtrl* m_pResult;
};

class WorkCalculateFeature : public QObject
{
	Q_OBJECT
public:
	WorkCalculateFeature(VOLUME_DATA* pVolumeData, const QString &strGroupName, muint32 uid, TASubClassResultCtrl* pResult, bool _bShape3DWithoutDiameter);

	virtual ~WorkCalculateFeature() {};

	static void updateProgress(int, void*);

	// (Radiomics)
private:
	map<std::string, RSTRING> featureMapping;
	void setFeatureMapping();
	RSTRING findRSTRING(QString valueStr);
	bool isValidCategory(RSTRING category);
	void makeCropItem(muint32 m_uid, ImageArray<short> *cropImage, ImageArray<int> *cropMask);
	map<RSTRING, double> processFeature(RSTRING currentFeatureClass, ImageArray<short> *cropImage, ImageArray<int> *cropMask);
	void updateFeatureValueString(TASubClassResultCtrl* m_pResult, RSTRING featureClass, map<RSTRING, double> listFeatureValue);
	void debugPrintFeatureValue(QString featureClassName, map<RSTRING, double> listFeatureValue);

private:
	int numProgress;
	void setProgressValue(int value, bool init = false);
	void _setProgressValue(int value, bool init = false);

private:
	QString m_strGroupName;
	int m_nLoopCount;
	muint32 m_uid;
	//mask *  m_pMaskVolume;
	TASubClassResultCtrl* m_pResult;
	bool bShape3DWithoutDiameter;

	VOLUME_DATA* m_pVolumeData;

public slots:
	void threadRun();

signals:
	void progress(int);
	void finished();
private:

};

class ActionCalculate3DMap : public QUndoCommand
{
public:
	ActionCalculate3DMap();
	virtual ~ActionCalculate3DMap();


};

class WorkCalculate3DMap : public QObject
{
	Q_OBJECT
public:
	WorkCalculate3DMap(VOLUME_DATA* pVolumeData, const QString &strGroupName, float* p3DVolumeMap, mint32 nROIIdx, std::vector<std::pair<float, float>>*pColorCategory);
	virtual ~WorkCalculate3DMap();

private:
	void setProgressValue(int value, bool init = false);

	QString			m_strGroupName;
	float*			m_p3DVolumeMap;
	std::vector<pair<float, float>> *m_pColorCategory;
	mint32			m_nROIIdx;

	VOLUME_DATA*	m_pVolumeData;

public slots:
	void threadRun();

signals:
	void progress(int);
	void finished();

};
#endif
