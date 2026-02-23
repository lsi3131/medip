#include "stdafx.h"
#include "Dialogs/LineProfileDialog.h"
#include <qwt_scale_widget.h>
#include <qwt_plot_layout.h>
#include "System/styleManager.h"

LineHistogram::LineHistogram(float length, QWidget *parent)
	:QwtPlot(parent)
{
	setStyleSheet("background-color: rgba(255, 255, 255, 255); color: rgba(0, 0, 0, 255);");
	setMouseTracking(true);
	this->length = length;
}

LineHistogram::~LineHistogram()
{
}

void LineHistogram::initPlot()
{
	axisWidget(QwtPlot::yLeft)->setContentsMargins(0, 0, 0, 0);
	axisWidget(QwtPlot::xBottom)->setContentsMargins(0, 0, 0, 0);
	axisWidget(QwtPlot::xBottom)->setTitle(QString("Distance(mm) : %1mm").arg(length*10));
	plotLayout()->setAlignCanvasToScales(true);
	plotLayout()->setSpacing(0);
}

void LineHistogram::CreateHistogram(QMap<float, mint16> huPoints, mint16 huMin, mint16 huMax)
{
	int maxCount = 0;
	float maxLogCount = 0;

	std::map<float, mint16> countMap = huPoints.toStdMap();

	float *logCount = new float[countMap.size()];
	int i = 0;
	memset(logCount, 0, sizeof(float)*(countMap.size()));

	for (auto val = countMap.begin(); val != countMap.end(); val++)
	{
		
		points << QPointF(val->first, val->second);
		
		
		mint16 hu = val->second;
		
		hu -= huMin;

		if (hu > 0)
			logCount[i] = log10f(hu);

		if (hu > maxCount)
			maxCount = hu;
		if (logCount[i] > maxLogCount)
			maxLogCount = logCount[i];

		i++;
	}

	i = 0;
	for (auto tmpval = countMap.begin(); tmpval != countMap.end(); tmpval++)
	{
		float val = logCount[i++] / maxLogCount*maxCount;

		val += huMin;

		logPoints << QPointF(tmpval->first, val);
	}

	SAFE_DELETES(logCount);
	setAxisScale(QwtPlot::xBottom, 0.0, length*10);

	plotArea.setCurveAttribute(QwtPlotCurve::Inverted);
	plotArea.setSamples(points);
	setAxisScale(QwtPlot::yLeft, huMin, huMax);
	plotArea.setPen(QColor(0, 0, 0, 255), 1);
	plotArea.setRenderHint(QwtPlotItem::RenderHint::RenderAntialiased);
	

	plotArea2.setCurveAttribute(QwtPlotCurve::Inverted);
	plotArea2.setSamples(logPoints);
	setAxisScale(QwtPlot::yLeft, huMin, huMax);
	plotArea2.setPen(QColor(255, 0, 0, 255), 1);
	plotArea2.setRenderHint(QwtPlotItem::RenderHint::RenderAntialiased);



	plotArea.attach(this);
	canvas()->setCursor(Qt::ArrowCursor);
}

void LineHistogram::setCheckZoom(bool value)
{
	if (value)
		plotArea2.attach(this);
	else
		plotArea2.detach();

	replot();
}

LineProfileDialog::LineProfileDialog(QMap<float, mint16> huPoints, mint16 huMin, mint16 huMax, mint16 huAvg, float length, QWidget *parent)
	:QDialog(parent)
{
	setMouseTracking(true);
	setWindowTitle("Line profiling");

	this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

	layout = new QVBoxLayout(this);
	if(huPoints.size() > 1)
	{
		QVBoxLayout * histoLayout = new QVBoxLayout();

		layout->addLayout(histoLayout);

		histogram = new LineHistogram(length, this);
		histogram->initPlot();
		histogram->CreateHistogram(huPoints, huMin, huMax);

		histoLayout->addWidget(histogram);
		setStyleSheet("background-color: rgba(255, 255, 255, 255);");
		btnLog = NULL;


		QString str;

		str = QString("Count : %1\nMin : %2\nMax : %3\nAvg : %4\n").arg(huPoints.size()).arg(huMin).arg(huMax).arg(huAvg);

		QHBoxLayout * labelLayout = new QHBoxLayout();
		layout->addLayout(labelLayout);

		QLabel *labelmsg = new QLabel(str, this);
		labelmsg->setStyleSheet("color : rgba(0,0,0,255);");
		labelLayout->addWidget(labelmsg);

		QVBoxLayout * logLayout = new QVBoxLayout();
		layout->addLayout(logLayout);

		btnLog = new QPushButton(this);
		btnLog->setFixedSize(80, 25);
		btnLog->setText(QString("Log"));
		btnLog->setCheckable(true);
		btnLog->setChecked(false);
		btnLog->setStyleSheet(STYLE_MANAGER->buttonNormal);
		logLayout->addWidget(btnLog);
		connect(btnLog, &QPushButton::released, this, &LineProfileDialog::slot_OnLogHistogram);
	}
	else
	{
		QLabel *label = new QLabel("It can not be measured because it is smaller than the minimum measurement unit.");
		layout->addWidget(label);
	}
}

void LineProfileDialog::slot_OnLogHistogram()
{
	if (btnLog->isChecked())
		btnLog->setText(QString("Non-Log"));
	else
		btnLog->setText(QString("Log"));

	histogram->setCheckZoom(btnLog->isChecked());
}
