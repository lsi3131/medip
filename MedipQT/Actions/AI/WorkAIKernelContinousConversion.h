#pragma once

#include "graphics/volumedata.h"
#include "Actions/WorkBase.h"

class ActionManager;

class WorkAIKernelContinousConversion : public WorkBase
{
	Q_OBJECT
public:
	WorkAIKernelContinousConversion(VOLUME_DATA* pVolumeData, ActionManager* pActionManager);
	virtual ~WorkAIKernelContinousConversion();

public slots:
	void threadRun();

private:
	bool doPredict(int addValue);

private:
	std::vector<std::vector<mint16>> m_OutputData;
	VOLUME_DATA* m_pVolumeData;
	ActionManager* m_pActionManager;

	QString m_inputPredictDirPath;
	QString m_AIWeightDirPath;
	QString m_AIWeightFileName;
};
