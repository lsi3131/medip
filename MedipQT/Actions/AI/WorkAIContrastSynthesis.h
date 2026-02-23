#pragma once

#include "defineMEDIP.h"
#include "WorkBase.h"
#include "graphics/volumedata.h"

class ActionManager;

class WorkAIContrastSynthesis : public WorkBase
{
	Q_OBJECT

public:
	static const std::vector<std::vector<int>> MEDIP_CONTRAST_SYNTHESIS_TRANS_SALT;

public:
	WorkAIContrastSynthesis(VOLUME_DATA* pVolumeData, ActionManager* pActionManager, eAIContrastSynthesisOption option, bool useGPU);
	virtual ~WorkAIContrastSynthesis();

public slots:
	void threadRun();

private:
	void initWorkDirectory();
	bool saveRawSlice();
	bool doPredict(int addValue);

signals:
	void progress(int);
	void finished();

private:
	eAIContrastSynthesisOption m_option;
	bool m_useGPU;
	std::vector<mint16> m_translatedData;
	VOLUME_DATA* m_pVolumeData;
	ActionManager* m_pActionManager;

	QString m_inputPredictDirPath;
	QString m_inputRawSliceDirPath;
	QString m_outputPredictResultDirPath;
	QString m_AIWeightDirPath;
	QString m_AIWeightFileName;
};

