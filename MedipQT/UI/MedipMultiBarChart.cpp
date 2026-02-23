#include "stdafx.h"
#include "MedipMultiBarChart.h"
#include "UI/AlphaColorMap.h"
#include "UI/RangeWidget.h"
#include "Dialogs/ColorMapDialog.h"
#include "Actions/ActionManager.h"
#include "System/styleManager.h"
#include "System/stringManager.h"
#include "Windows/windowManager.h"
#include "Windows/Main/MainSegmentWidget.h"
#include "Windows/Main/MainAnalWidget.h"
#include "Windows/VolumeView.h"
#include "Windows/AnalVolumeView.h"


#include <qwt_scale_widget.h>
#include <qwt_plot_picker.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_barchart.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#include <qwt_plot_multi_barchart.h>
#include <qwt_legend.h>
#include <qwt_legend_label.h>
#include <qwt_math.h>
#include <qwt_plot_layout.h>
#include <qwt_scale_map.h>
#include <qwt_plot_curve.h>

class QwtScaleEngine;

MedipMultiBarChart::MedipMultiBarChart(QWidget* parent) : 
	QwtPlot(parent)
{
	initPlot();
	initPlotCharts();
}

MedipMultiBarChart::~MedipMultiBarChart()
{
}

void MedipMultiBarChart::mousePressEvent(QMouseEvent* e)
{
	if (e->buttons() & Qt::LeftButton) // volumewidth 움직임
	{
		//CustomHistogram.cpp 참고 
	}
}

void MedipMultiBarChart::mouseReleaseEvent(QMouseEvent* e)
{
	//CustomHistogram.cpp 참고 
}

void MedipMultiBarChart::mouseMoveEvent(QMouseEvent* e)
{
	//CustomHistogram.cpp 참고 
}

void MedipMultiBarChart::mouseDoubleClickEvent(QMouseEvent* e)
{
	//CustomHistogram.cpp 참고 
}

void MedipMultiBarChart::resizeEvent(QResizeEvent* e)
{
	//CustomHistogram.cpp 참고 
}

void MedipMultiBarChart::initPlotCharts()
{
#ifdef USE_BARCHART 
	int barWidth = 60;
	if (m_barChartItem != nullptr)
		delete(m_barChartItem);

	m_barChartItem = new QwtPlotMultiBarChart("Bar Chart ");
	m_barChartItem->setLayoutPolicy(QwtPlotMultiBarChart::FixedSampleSize); //FixedSampleSize 해야 bar 크기 조절 가능
	m_barChartItem->setSpacing(20);
	m_barChartItem->setMargin(3);
	m_barChartItem->setLayoutHint(barWidth);	//실제로 보이는 bar width

	m_barChartItem->attach(this);
#endif

#ifdef USE_PLOT_CURVE //curve
	if (m_plotCurve != nullptr)
		delete(m_plotCurve);

	m_plotCurve = new QwtPlotCurve();
	QwtSymbol* symbol = new QwtSymbol(QwtSymbol::Ellipse,
		QBrush(Qt::white), QPen(QColor(COLOR_SKYBLUE), 4.5), QSize(6, 6));
	m_plotCurve->setPen(QColor(COLOR_SKYBLUE), 2);
	m_plotCurve->setSymbol(symbol);
	m_plotCurve->attach(this);

	QwtText QwtText;
	QwtText.setColor(QColor("black"));
	QwtText.setText("QwtLegend");
	m_plotCurve->setTitle(QwtText);
#endif

	std::vector<float> risk_over_time_test;
	for (int ii = 0; ii < 8; ++ii)
		risk_over_time_test.push_back((ii * 1));

	populate(risk_over_time_test);
	setOrientation(0);
	setAutoReplot(true);
}

void MedipMultiBarChart::initPlot()
{

	setAutoFillBackground(true);
	//insertLegend(new QwtLegend());
	setStyleSheet("background-color: rgba(255, 255, 255, 255);");

#if 1
	QwtPlotGrid* d_grid = new QwtPlotGrid();
	d_grid->enableX(true);
	//d_grid->enableY(true);
	//d_grid->enableXMin(true);
	//d_grid->enableYMin(true);
	//d_grid->setMinorPen(QPen(Qt::white, 0, Qt::SolidLine));
	d_grid->setPen(QPen(Qt::lightGray, 0, Qt::SolidLine));
	d_grid->attach(this);
#endif

#if 0
	QwtText Qwttete;
	Qwttete.setColor(QColor("black"));
	Qwttete.setText("Amplitude [dB]");
	setAxisTitle(QwtPlot::yLeft, Qwttete);

	setTitle(Qwttete);

	QwtAbstractLegend* pLegend = legend(); //legend 범례
#endif	

	setAxisTitle(QwtPlot::xBottom, " ");
	setAxisTitle(QwtPlot::yLeft, "Amplitude [dB]");
	setAxisTitle(QwtPlot::yRight, "Phase [deg]");
	setAxisMaxMajor(QwtPlot::xBottom, 6);
	setAxisMaxMinor(QwtPlot::xBottom, 10);

#if 1
	QwtText qwtText_yLeft;
	qwtText_yLeft.setColor(QColor("black"));
	qwtText_yLeft.setText(" ");
	setAxisTitle(QwtPlot::yLeft, qwtText_yLeft);
#endif
#if 0
	QwtText qwtText_yRight;
	qwtText_yRight.setColor(QColor("black"));
	qwtText_yRight.setText(" s");
	setAxisTitle(QwtPlot::yRight, qwtText_yRight);
#endif

#ifdef USE_PLOT_CURVE //QwtPlotCurve
	m_plotCurve = nullptr;
#endif

#ifdef USE_BARCHART
	m_barChartItem = nullptr;
#endif

	//canvas()->setCursor(Qt::ArrowCursor);

	//dialog 에서 차지하는 면적
	//setFixedHeight(SD_HEIGHT);
	//setFixedWidth(SD_WIDTH);

	// 실제 Plot의 크기
	//resize(SD_WIDTH-100, SD_HEIGHT);
}

void MedipMultiBarChart::AfterFullScreen()
{
	resizeEvent(NULL);
}

void MedipMultiBarChart::refleshBarChart()
{
	setOrientation(0);
	setAutoReplot(true);
}

// ----------------------------------------------------------------------------------
// barchart
// ----------------------------------------------------------------------------------
#ifdef USE_BARCHART_SAMPLE

//template<typename T>
void MedipMultiBarChart::populate(std::vector<float>& vecData)
{
	//static const char* colors[] = { "DarkOrchid", "SteelBlue", "Gold" };
	//static const char* colors[] = { "SteelBlue", "Gold" };
	static const char* colors[] = { "darkRed" };

	const int numSamples = 8;
	const int numBars = sizeof(colors) / sizeof(colors[0]);

	QList< QwtText > titles;
	for (int i = 0; i < numBars; i++)
	{
		QString title("Value %1 ");
		titles += title.arg(i);
		titles[i].setColor(QColor("Black"));
	}

#ifdef USE_BARCHART
	//m_barChartItem->setBarTitles(titles);
	//m_barChartItem->setLegendIconSize(QSize(10, 14));

	for (int i = 0; i < numBars; i++)
	{
		QwtColumnSymbol* symbol = new QwtColumnSymbol(QwtColumnSymbol::Box);
		symbol->setLineWidth(1);
		symbol->setFrameStyle(QwtColumnSymbol::NoFrame);
		//symbol->setFrameStyle(QwtColumnSymbol::Plain); // borderLine
		symbol->setPalette(QPalette(colors[i]));

		m_barChartItem->setSymbol(i, symbol);
	}

	QVector< QVector< double > > series;
	for (int i = 0; i < vecData.size(); i++)
	{
		QVector< double > values;
		values += vecData[i];
		series += values;
	}

	m_barChartItem->setSamples(series);
#endif

#ifdef USE_PLOT_CURVE
	QVector<QPointF> samples;
	for (int i = 0; i < vecData.size(); i++)
	{
		double x = i;
		double values;
		values = vecData[i];

		samples.append(QPointF(x, values));
	}
	m_plotCurve->setSamples(samples);
#endif

}

void MedipMultiBarChart::setMode(int mode)
{
#ifdef USE_BARCHART
	if (mode == 0)
	{
		m_barChartItem->setStyle(QwtPlotMultiBarChart::Grouped);
	}
	else
	{
		m_barChartItem->setStyle(QwtPlotMultiBarChart::Stacked);
	}
#endif
}

void MedipMultiBarChart::setOrientation(int nOrientation)
{
#ifdef USE_BARCHART
	const Qt::Orientation orientation =
		(nOrientation == 0) ? Qt::Vertical : Qt::Horizontal;

	int axis1 = QwtPlot::xBottom;
	int axis2 = QwtPlot::yLeft;

	if (orientation == Qt::Horizontal)
		qSwap(axis1, axis2);

	setAxisScale(axis1, 0, m_barChartItem->dataSize() - 1, 1.0);
	m_barChartItem->setOrientation(orientation);

	QwtScaleDraw* scaleDraw_01 = new ScaleDraw(orientation);
	setAxisMaxMinor(axis1, 3);
	setAxisScaleDraw(axis1, scaleDraw_01);

	scaleDraw_01->enableComponent(QwtScaleDraw::Backbone, false);
	scaleDraw_01->enableComponent(QwtScaleDraw::Ticks, false);

	setAxisMaxMinor(axis2, 3);
	QwtScaleDraw* scaleDraw = new ScaleDraw(orientation);
	scaleDraw->setTickLength(QwtScaleDiv::MediumTick, 4);
	setAxisScaleDraw(axis2, scaleDraw);

	scaleDraw->enableComponent(QwtScaleDraw::Backbone, false);
	scaleDraw->enableComponent(QwtScaleDraw::Ticks, false);

	plotLayout()->setCanvasMargin(0);
	replot();
#endif
}

void MedipMultiBarChart::exportChart()
{
	//QwtPlotRenderer renderer;
	//renderer.exportTo(this, "barchart.pdf");
}

void MedipMultiBarChart::doScreenShot(std::string strPath)
{
	exportPNG(strPath, 800, 600);
}

void MedipMultiBarChart::exportPNG(std::string strPath, int width, int height)
{
	//const QString fileBase = QString("distrowatch-%2x%3").arg(width).arg(height);
	const QString fileBase = QString(strPath.c_str()).arg(width).arg(height);

	const int resolution = qRound(85.0 * width / 800.0);

	const double mmToInch = 1.0 / 25.4;
	const int dotsPerMeter = qRound(resolution * mmToInch * 1000.0);

	QImage image(width, height, QImage::Format_ARGB32);
	image.setDotsPerMeterX(dotsPerMeter);
	image.setDotsPerMeterY(dotsPerMeter);
	image.fill(Qt::transparent);

	QPainter painter(&image);
	render(&painter, QRectF(0, 0, width, height));
	painter.end();

	DEEPCATCH_REPORT* pReport = ACTION_MANAGER->getDeepCatchAnalysisVals();
	if (pReport == nullptr)
	{
		pReport = ACTION_MANAGER->DeepcatchReportInfoSafeCreate();
	}
	pReport->chartImg = image;
#if 0
	image.save(fileBase + ".png");
#endif
}

void MedipMultiBarChart::render(QPainter* painter, const QRectF& targetRect)
{
	const int r = 20;
	const QRectF plotRect = targetRect.adjusted(0.5 * r, 0.5 * r, -0.5 * r, -0.5 * r);

	QwtPlotRenderer renderer;

	if (qApp->styleSheet().isEmpty())
	{
		renderer.setDiscardFlag(QwtPlotRenderer::DiscardBackground, true);

		painter->save();
		painter->setRenderHint(QPainter::Antialiasing, true);
		painter->setPen(QPen(Qt::darkGray, 1));
		painter->setBrush(QColor("WhiteSmoke"));
		painter->drawRoundedRect(targetRect, r, r);
		painter->restore();
	}

	renderer.render(this, painter, plotRect);
}

#endif

// ----------------------------------------------------------------------------------
// sample - distrowatch 
// ----------------------------------------------------------------------------------
#ifdef USE_DISTROWATCH_SAMPLE

void MedipMultiBarChart::setOrientation(int o)
{
	const Qt::Orientation orientation =
		(o == 0) ? Qt::Vertical : Qt::Horizontal;

	//int axis1 = QwtAxis::XBottom;
	//int axis2 = QwtAxis::YLeft;
	int axis1 = QwtPlot::xBottom;
	int axis2 = QwtPlot::yLeft;

	if (orientation == Qt::Horizontal)
		qSwap(axis1, axis2);

	m_chartItem->setOrientation(orientation);

	setAxisTitle(axis1, "Distros");
	setAxisMaxMinor(axis1, 3);
	setAxisScaleDraw(axis1, new ScaleDraw(orientation, m_distros));

	setAxisTitle(axis2, "Hits per day ( HPD )");
	setAxisMaxMinor(axis2, 3);

	QwtScaleDraw* scaleDraw = new QwtScaleDraw();
	scaleDraw->setTickLength(QwtScaleDiv::MediumTick, 4);
	setAxisScaleDraw(axis2, scaleDraw);

	plotLayout()->setCanvasMargin(0);
	replot();
}

void MedipMultiBarChart::exportChart()
{
	QwtPlotRenderer renderer;
	renderer.exportTo(this, "distrowatch.pdf");
}

void MedipMultiBarChart::doScreenShot()
{
	exportPNG(800, 600);
}

void MedipMultiBarChart::exportPNG(int width, int height)
{
	const QString fileBase = QString("distrowatch-%2x%3").arg(width).arg(height);

	const int resolution = qRound(85.0 * width / 800.0);

	const double mmToInch = 1.0 / 25.4;
	const int dotsPerMeter = qRound(resolution * mmToInch * 1000.0);

	QImage image(width, height, QImage::Format_ARGB32);
	image.setDotsPerMeterX(dotsPerMeter);
	image.setDotsPerMeterY(dotsPerMeter);

	image.fill(Qt::transparent);

	QPainter painter(&image);
	render(&painter, QRectF(0, 0, width, height));
	painter.end();

	image.save(fileBase + ".png");
}

void MedipMultiBarChart::render(QPainter* painter, const QRectF& targetRect)
{
	const int r = 20;
	const QRectF plotRect = targetRect.adjusted(0.5 * r, 0.5 * r, -0.5 * r, -0.5 * r);

	QwtPlotRenderer renderer;

	if (qApp->styleSheet().isEmpty())
	{
		renderer.setDiscardFlag(QwtPlotRenderer::DiscardBackground, true);

		painter->save();
		painter->setRenderHint(QPainter::Antialiasing, true);
		painter->setPen(QPen(Qt::darkGray, 1));
		painter->setBrush(QColor("WhiteSmoke"));
		painter->drawRoundedRect(targetRect, r, r);
		painter->restore();
	}

	renderer.render(this, painter, plotRect);
}

#endif 

