
#ifndef RANGEDISPLAYCHART_H
#define RANGEDISPLAYCHART_H

#ifdef QT_CHARTS_USE

#pragma once

#include <QtCharts>
#include <QGraphicsView>

class rangeDisplayChart : public QChart
{

public:
	rangeDisplayChart(QGraphicsItem *parent = Q_NULLPTR);
	~rangeDisplayChart();

	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) override;

	// º¯¼ö
public:
	typedef enum {
		eCSWidth = 470,
		eCSHeight = 110,
		eCSTextLabelHeight = 20,
	} eChartSize;

private:
	QValueAxis *axisX;
};

#endif	// QT_CHARTS_USE
#endif