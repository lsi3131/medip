#pragma once

#include "Actions/WorkBase.h"
#include "graphics/volumedata.h"

class ActionManager;

class WorkAILowdoseReconstruction : public WorkBase
{
	Q_OBJECT
public:
	WorkAILowdoseReconstruction(VOLUME_DATA* pVolumeData, ActionManager* pActionManager);

private:
	bool doPredict(int addValue);

public slots:
	void threadRun();

private:
	std::vector<mint16> m_OutputData;
	VOLUME_DATA* m_pVolumeData;
	ActionManager* m_pActionManager;

	QString m_inputPredictDirPath;
	QString m_AIWeightDirPath;
	QString m_AIWeightFileName;
};

