#pragma once

#ifndef MEDIPCHART_MANAGER_H
#define MEDIPCHART_MANAGER_H

#include "MedipMultiBarChart.h"

class MedipChartManager
{
public:
	MedipChartManager();
	~MedipChartManager();

public:		
	QWidget* initPlotBarCharts(std::vector<float> & vecData);
	bool makeBarChartPng(std::string strPath = "chartScreenShot-%2x%3");


public:
	QWidget* getBarChartWidget() {
		if (m_pBarChart == nullptr) return nullptr;
		return m_pBarChart->parentWidget();
	};

	MedipMultiBarChart* getBarChart() {		
		return m_pBarChart;
	};


private:
	MedipMultiBarChart* m_pBarChart = nullptr;
};

#endif