#include "stdafx.h"
#include "rangeDisplayChart.h"

#ifdef QT_CHARTS_USE

rangeDisplayChart::rangeDisplayChart(QGraphicsItem *parent)
	: QChart(parent)
{
	QBarSet *set0 = new QBarSet("test");
	*set0 << 100;

	QHorizontalBarSeries *series = new QHorizontalBarSeries();

	//	series->setLabelsVisible(true);
	//	series->setLabelsPosition(QAbstractBarSeries::LabelsCenter);
	series->append(set0);

	this->addSeries(series);
	//	this->setAnimationOptions(QChart::SeriesAnimations);

	//	QStringList categories;
	//	categories << "Weight";
	//	QBarCategoryAxis *axisY = new QBarCategoryAxis();
	//	axisY->append(categories);
	//	this->setAxisY(axisY, series);

	axisX = new QValueAxis();
//	axisX->setLinePenColor(Qt::darkGray);
	axisX->setRange(55, 220);
	axisX->setTickCount(12);
//	axisX->setGridLineColor(Qt::darkGray);
	this->setAxisX(axisX, series);
//	axisX->applyNiceNumbers();

	this->legend()->hide();
//	this->legend()->setAlignment(Qt::AlignBottom);

	this->setMargins({ 0, 0, 0, 0 });
	this->layout()->setContentsMargins(0, 0, 0, eCSTextLabelHeight);
	this->setBackgroundRoundness(0);

//	this->setBackgroundVisible(false);
	this->resize(eCSWidth, eCSHeight);

	// theme
	this->setTheme(ChartThemeQt);
}

rangeDisplayChart::~rangeDisplayChart()
{
}

void rangeDisplayChart::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= Q_NULLPTR*/)
{
//	throw std::logic_error("The method or operation is not implemented.");

	QChart::paint(painter, option, widget);

	painter->setRenderHint(QPainter::Antialiasing);

	QRectF rect = this->plotArea();
	rect.setTop(rect.bottom());
	rect.setBottom(rect.top()+ eCSTextLabelHeight);
	rect.translate(0, rect.height());
	QRectF regionRect = rect;

	// 보통이하
	qreal separateWidth = qAbs(rect.width() / 11);
	regionRect.setWidth(separateWidth*2);
	painter->setPen(QColor(Qt::black));
	painter->setBrush(QColor(144, 238, 144));
	painter->drawRect(regionRect);
	painter->drawText(regionRect, Qt::AlignCenter, "Substandard");

	// 보통
	regionRect.translate(regionRect.width(), 0);
	painter->setPen(QColor(Qt::black));
	painter->setBrush(QColor(0, 128, 0));
	painter->drawRect(regionRect);
	painter->drawText(regionRect, Qt::AlignCenter, "Standard");

	// 보통 이상
	regionRect.translate(regionRect.width(), 0);
	regionRect.setWidth(separateWidth * 7);
	painter->setPen(QColor(Qt::black));
	painter->setBrush(QColor(0, 100, 0));
	painter->drawRect(regionRect);
	painter->drawText(regionRect, Qt::AlignCenter, "More than standard");

}

#endif	// QT_CHARTS_USE