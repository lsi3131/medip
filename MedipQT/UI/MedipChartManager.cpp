#include "stdafx.h"
#include "MedipChartManager.h"

MedipChartManager::MedipChartManager()
{	
}

MedipChartManager::~MedipChartManager()
{
#if 1
	SAFE_DELETE(m_pBarChart);
#endif
}


QWidget * MedipChartManager::initPlotBarCharts(std::vector<float> & vecData)
{
	SAFE_DELETE(m_pBarChart);

	QWidget* pWidget = nullptr;
	pWidget = new QWidget;
	m_pBarChart = new MedipMultiBarChart(pWidget);

	//dialog에서 plot이 차지하는 면적
	m_pBarChart->setFixedHeight(SD_HEIGHT);
	m_pBarChart->setFixedWidth(SD_WIDTH);
	
	m_pBarChart->populate(vecData);
	m_pBarChart->refleshBarChart();	

	return pWidget;
}

bool MedipChartManager::makeBarChartPng(std::string strPath)
{
	if (m_pBarChart == nullptr) return false;
	m_pBarChart->doScreenShot(strPath);
	return true;
}
