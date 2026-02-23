#include "stdafx.h"
#include "deepcatchAreaChart.h"

#ifdef QT_CHARTS_USE

#include "windowManager.h"
#include "volumedata.h"
#include "ActionManager.h"

deepcatchAreaChart::deepcatchAreaChart(QGraphicsItem *parent, int limitHeight)
	: QChart(parent)
	, m_nHorizontalAxesMinValue(0)
	, m_nHorizontalAxesMaxValue(0)
	, m_nVerticalAxesMinValue(0)
	, m_nVerticalAxesMaxValue(0)
	, m_nChartWidth(0)
	, m_nChartHeight(0)
{
	int chartAxesLableFontSize;
	DEEPCATCH_REPORT *pReport = ACTION_MANAGER->getDeepCatchAnalysisVals();
	switch (pReport->curA4Res)
	{
	//	eART96PPI:
	//	break;
		case eART150PPI:
		{
			if (limitHeight)
			{
				m_nChartWidth = limitHeight+(25*limitHeight/335);
				m_nChartHeight = m_nChartWidth;
			}
			else
			{
				m_nChartWidth = 360;
				m_nChartHeight = 360;
			}
			chartAxesLableFontSize = 11;
		}
		break;
	//	eART300PPI:
	//	break;
		case eART72PPI:
		default:
		{
			if (limitHeight)
			{
				m_nChartWidth = limitHeight+(16*limitHeight/158);
				m_nChartHeight = m_nChartWidth;
			}
			else
			{
				m_nChartWidth = 174;
				m_nChartHeight = 174;
			}
			chartAxesLableFontSize = 7;
		}
		break;
	}

	lineSeries_muscle = &pReport->areaChartInfo.lineSeries_muscle;
	lineSeries_avf = &pReport->areaChartInfo.lineSeries_avf;
	lineSeries_of = &pReport->areaChartInfo.lineSeries_of;

	m_nHorizontalAxesMinValue = pReport->areaChartInfo.nHorizontalAxesMinValue;
	m_nHorizontalAxesMaxValue = pReport->areaChartInfo.nHorizontalAxesMaxValue;
	m_nVerticalAxesMinValue = pReport->areaChartInfo.nVerticalAxesMinValue;
	m_nVerticalAxesMaxValue = pReport->areaChartInfo.nVerticalAxesMaxValue;

	QAreaSeries *areaSeries1 = new QAreaSeries(lineSeries_avf, nullptr);
//	areaSeries1->setName("Batman");
//	QPen pen(0x059605);
//	pen.setWidth(3);
//	areaSeries1->setPen(pen);

//	QLinearGradient gradient(QPointF(0, 0), QPointF(0, 1));
//	gradient.setColorAt(0.0, 0x3cc63c);
//	gradient.setColorAt(1.0, 0x26f626);
//	gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
	areaSeries1->setPen(Qt::NoPen);
	areaSeries1->setBrush(QBrush(0x68B2A0));

	QAreaSeries *areaSeries2 = new QAreaSeries(lineSeries_avf, lineSeries_muscle);
//	areaSeries2->setName("spyder man");
//	areaSeries2->setPen(pen);
	areaSeries2->setPen(Qt::NoPen);
	areaSeries2->setBrush(QBrush(0xE34E55));

	QAreaSeries *areaSeries3 = new QAreaSeries(lineSeries_muscle, lineSeries_of);
//	areaSeries2->setName("spyder man");
//	areaSeries3->setPen(pen);
	areaSeries3->setPen(Qt::NoPen);
	areaSeries3->setBrush(QBrush(0xFFFF24));


	this->addSeries(areaSeries1);
	this->addSeries(areaSeries2);
	this->addSeries(areaSeries3);
//  this->setTitle("Simple areachart example");
	this->createDefaultAxes();
	this->axes(Qt::Horizontal).first()->setRange(m_nHorizontalAxesMinValue, m_nHorizontalAxesMaxValue);
	this->axes(Qt::Vertical).first()->setRange(m_nVerticalAxesMinValue, m_nVerticalAxesMaxValue);

	this->axes(Qt::Horizontal).first()->hide();
//	this->axes(Qt::Vertical).first()->hide();
	((QValueAxis*)(this->axes(Qt::Vertical).first()))->setTickCount(3);
	((QValueAxis*)(this->axes(Qt::Vertical).first()))->setLabelFormat("%d(%)");
	QFont labelFont("Arial", chartAxesLableFontSize, QFont::Bold);
	this->axes(Qt::Vertical).first()->setLabelsFont(labelFont);
	this->axes(Qt::Vertical).first()->setLabelsAngle(270);
	this->axes(Qt::Vertical).first()->setGridLineVisible(true);

	this->legend()->hide();
//	this->legend()->setAlignment(Qt::AlignBottom);

//	this->setBackgroundVisible(false);
	this->setBackgroundBrush(Qt::transparent);

	this->setMargins({ 0, 0, 0, 0 });
	this->layout()->setContentsMargins(0, 0, 0, 0);
	this->setBackgroundRoundness(0);

	this->resize(m_nChartWidth, m_nChartHeight);

	// theme
//	this->setTheme(ChartThemeQt);
}

deepcatchAreaChart::~deepcatchAreaChart()
{

}

#endif	// QT_CHARTS_USE
