#include "stdafx.h"
#include "ActionHccPrediction.h"

#include "System/stringManager.h"
#include "Windows/WindowBase.h"
#include "Windows/windowManager.h"
#include "Windows/VolumeView.h"
#include "Windows/AnalMPRPlaneView.h"
#include "DeepInsthink.h"
#include "UI\MedipChartManager.h"

#include "Actions/ActionManager.h"
#include "DataContext.h"

WorkHccPrediction::WorkHccPrediction(
	const float& age,			// "Age" from UI;
	const float& gender,		// "Gender" from UI; NOTE: (Male: 1.f, Female: 0.f)
	const float& agent,			// "Antivirals agent" from UI; NOTE: (Entecavir: 1.f, Tenofovir: 0.f)
	const float& cirr,			// "Cirrhosis, baseline" from UI; NOTE: (Yes: 1.f, No: 0.f)
	const float& hbeag,			// "Presence of HBeAg, baseline" from UI; NOTE: (Yes: 1.f, No: 0.f)
	const float& plt,			// "Platelet, baseline" from UI;
	const float& alb,			// "Albumin, baseline" from UI;
	const float& tb,			// "Total bilirubin, baseline" from UI;
	const float& alt,			// "ALT, baseline" from UI;
	const float& hbv,			// "HBV DNA, baseline" from UI;
	const float& liver,			// "Liver Volume" from DeepCatch;
	const float& spleen
)
{
	m_age		= age;
	m_gender	= gender;
	m_agent		= agent;
	m_cirr		= cirr;
	m_hbeag		= hbeag;
	m_plt		= plt;
	m_alb		= alb;
	m_tb		= tb;
	m_alt		= alt;
	m_hbv		= hbv;
	m_liver		= liver;
	m_spleen	= spleen;
}

void WorkHccPrediction::setProgressValue(int value, bool init)
{
	static int val = 0;

	if (init)
	{
		val = 0;
		emit progress(val);
		return;
	}

	bool res = true;
	res = true;

	if (100 <= val)
		return;

	if ((val + 10) <= value)
	{
		val = value;
		emit progress(val);
	}
}

void WorkHccPrediction::predict()
{
	ACTION_MANAGER->action_ProgressUpdate(5);
	//setProgressValue(5);	
#if 1
	Prediction::setExtension("mipx");

	const QString weightPath = STRING_MANAGER->AISegmentationPath + "/weight_deepcatch";

	std::string load_weight_dir = weightPath.toStdString();
	std::string load_weight_name = "HCC_Prediction.mipx";

	// deepcatch
	std::vector< std::vector<int> > DEEPCATCH_REG_SALT{
		{ 1237511163, 1838418459, 1093931361, 1105727818, 735048887, 772392164, 1310096029, 1921377113,
		863180821, 1687579508, 1370944241, 1562928510, 1634900440, 554518886, 731955677, 1603490642,
		341951092, 377537154, 1206264283, 814105533, 1611262396, 1025494219, 1187428308, 1307761987,
		890281657, 1493280231, 524789833, 156392073, 1651402060, 1521714240, 1544494579, 1854541861 }
	};

	Prediction mp(load_weight_dir, load_weight_name, true, DEEPCATCH_REG_SALT);


	int PERCENTAGE_CNT = 8;
	int nCount = 0;

	int _addValue = 20;
	int milestone = _addValue;

	bool bResult = mp.medipPredict(m_age, m_gender, m_agent, m_cirr, m_hbeag, m_plt,
		m_alb, m_tb, m_alt, m_hbv, m_liver, m_spleen);

	CheckData check;
	int percent = 0;
	vector<float> risk_over_time;

	while (true)
	{
		check = mp.getCheckData();
		if (check.status == Status::NORMAL) {
			if (percent != check.progress) {
				percent = check.progress;
				ACTION_MANAGER->action_ProgressUpdate(percent);
				//setProgressValue(percent);
				std::cout << percent << std::endl; // 진행 퍼센트 출력 15, 85, 100
			}
		}
		else if (check.status == Status::STOPPING) {
						
			//ACTION_MANAGER->m_qThreadNext.front().pTempData = std::make_shared<std::vector<int>>(SliceNum);
			printf_s("Stopping\n");
			//emit finished();
			return;

		}
		else {
			break;
		}
	}

	if (check.status == Status::EXCEPTION_TERMINATED) {
		printf(check.exception.c_str());
	}
	else if (check.status == Status::DONE) {
		// 현재 상태가 Done 일 경우, 결과 벡터 반환
		risk_over_time = mp.getResults();
		for (int ii = 0; ii < 8; ++ii)
		{
			std::cout << (ii + 1) << " year : " << risk_over_time[ii] << std::endl;
		}
	}

#else
	std::vector<float> risk_over_time;

	for (int ii = 0; ii < 8; ++ii)
	{
		int value = ii;
		if (ii == 5) value += 5;
		risk_over_time.push_back((value * 10));
	}
#endif

	DEEPCATCH_REPORT *pReport = ACTION_MANAGER->getDeepCatchAnalysisVals();
	if (pReport == nullptr)
		pReport = ACTION_MANAGER->DeepcatchReportInfoSafeCreate();

	risk_over_time.swap(pReport->HCC_Result_Percentage);
}

void WorkHccPrediction::threadRun()
{
//
	qDebug() << "<<<<<<<<<<<<<<<<<<<< WorkHccPrediction thread Run";

	VOLUME_DATA * vol_dt = &DATA_CONTEXT->volume_data;

	predict();

	bool res = true;
	res = true;	

	if (!res)
	{
		vol_dt->threadResult = -1;
		emit finished();
		return;
	}
	else
	{
		vol_dt->threadResult = 1;
	}

	emit finished();
	return;
}
