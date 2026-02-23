#pragma once

#include "Actions/WorkBase.h"
#include "graphics/volumedata.h"

class ActionManager;

class WorkAIKernelNeutralization : public WorkBase
{
	Q_OBJECT
public:
	WorkAIKernelNeutralization(VOLUME_DATA* pVolumeData, ActionManager* pActionManager);

public slots:
	void threadRun();

private:
	bool doPredict(int addValue);

private:
	int m_id;

	std::vector<mint16> m_targetData;
	std::vector<mint16> m_OutputData;
	VOLUME_DATA* m_pVolumeData;
	ActionManager* m_pActionManager;

	QString m_inputPredictDirPath;
	QString m_AIWeightDirPath;
	QString m_AIWeightFileName;
};
