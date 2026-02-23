#pragma once

#include "graphics/volumedata.h"
#include "Actions/WorkBase.h"

class ActionManager;

class WorkAICTQuantization : public WorkBase
{
	Q_OBJECT
public:
	WorkAICTQuantization(VOLUME_DATA* pVolumeData, ActionManager* pActionManager);
	virtual ~WorkAICTQuantization();

public slots:
	void threadRun();

private:
	bool doPredict(int addValue);

private:
	ActionManager* m_pActionManager;
	std::vector<mint16> m_OutputData;
	VOLUME_DATA* m_pVolumeData;

	QString m_inputPredictDirPath;
	QString m_AIWeightDirPath;
	QString m_AIWeightFileName;
};

