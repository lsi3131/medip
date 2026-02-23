#pragma once

#include "define.h"
#include "graphics/volumedata.h"
#include "Actions/AI/ActionPredictAddEx.h"
#include "Actions/AI/WorkDeepPredictResult.h"
#include "FunctionLevel.h"

class LicenseManager;
class ActionManager;
class ProductManager;

class WorkDeepPredict : public QObject
{
	Q_OBJECT
public:
	WorkDeepPredict(
		VOLUME_DATA* pVolumeData,
		LicenseManager* pLicenseManager,
		ActionManager* pActionManager,
		ProductManager* pProductManager,
		QString predictDirPath,
		QString weightDirPath,
		eMEDIP_FUNCTION_LEVEL weightType,
		QString projectName,
		BoundingBoxI box,
		bool useGPU,
		eDeepPredictAICopyMask copyMaskType,
		int nFilterIdx = -1);

public:
	const WorkDeepPredictResult* GetResult() const;
	void setProgressValue(int value, bool init = false);

private:
	void setRawDataWithVolumePlane_WholeBodyIO(mint16* maskRaw, VOLUME_DATA* vol_dt, int cx, int cy, int i);
	void setRawDataWithVolumePlane(mint16* maskRaw, VOLUME_DATA* vol_dt, int cx, int cy, int i, bool isFilterExist, int indexAI, int fliterValue);
	ActionPredictAddEx* getActionPredictAdd() const;


public slots:
	void threadRun();

signals:
	void progress(int);
	void finished();

private:
	QString m_predictDirPath;
	QString m_weightDirPath;
	eMEDIP_FUNCTION_LEVEL m_weightType;
	QString m_projectName;
	bool m_useGPU;
	eDeepPredictAICopyMask m_copyMaskType;
	int m_nFilterIdx = -1;

	BoundingBoxI m_BoundingBox;
	int m_start;
	int m_end;

	VOLUME_DATA* m_pVolumeData;

	LicenseManager* m_pLicenseManager;
	ActionManager* m_pActionManager;
	ProductManager* m_pProductManager;
	ActionPredictAddEx* m_pActionPredictAdd;

	std::unique_ptr<WorkDeepPredictResult> m_pResult;
};



