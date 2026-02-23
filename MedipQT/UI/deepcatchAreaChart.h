#ifndef DEEPCATCHAREACHART_H
#define DEEPCATCHAREACHART_H

#include "define.h"

#ifdef QT_CHARTS_USE

#pragma once

#include <QtCharts>
#include <QGraphicsView>

class deepcatchAreaChart : public QChart
{

public:
	deepcatchAreaChart(QGraphicsItem *parent = Q_NULLPTR, int limitHeight = 0);
	~deepcatchAreaChart();

	QLineSeries *lineSeries_muscle;
	QLineSeries *lineSeries_avf;
	QLineSeries *lineSeries_of;

	int	m_nHorizontalAxesMinValue;
	int	m_nHorizontalAxesMaxValue;
	int	m_nVerticalAxesMinValue;
	int	m_nVerticalAxesMaxValue;

	int m_nChartWidth;
	int m_nChartHeight;
};

#endif	// QT_CHARTS_USE
#endif
