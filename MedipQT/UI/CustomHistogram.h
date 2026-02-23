#pragma once

#ifndef CUSTOM_HISTOGRAM_H
#define CUSTOM_HISTOGRAM_H

#include <qwidget.h>
#include <qwt_plot.h>

class AlphaColorMap;
class QComboBox;
class QPushButton;
class RangeWidget;
class QwtPlotCurve;
class QwtPlotPicker;

enum COMBO_LIST
{
	CL_2D = 0,
	CL_3D,
};

class CustomHistogram : public QwtPlot
{
	Q_OBJECT

public:
	CustomHistogram(QWidget *parent= NULL);
	~CustomHistogram();

	QwtPlotCurve *plotArea;
	QwtPlotCurve *plotArea2;
	AlphaColorMap * lineVolumeWidth;
	QComboBox * m_WindowCombo;
	QWidget * EmptyDark;
	QWidget * EmptyLight;
	QWidget * lineWidth;
	QWidget * lineRange;
	QPushButton * actHidden;
	RangeWidget * HuRangeSlider;

protected:
	void mousePressEvent(QMouseEvent *);
	void mouseReleaseEvent(QMouseEvent *);
	void mouseMoveEvent(QMouseEvent *);
	void mouseDoubleClickEvent(QMouseEvent *);
	void resizeEvent(QResizeEvent *);
	
public:
	void initPlot();

	void setChart(QVector<QPointF>, QVector<QPointF>);

	void setMaxCount(int value) { maxCount = value; }
	void addMaxCount(int value) { maxCount += value; }
	float getMaxCount();

	int getComboType();

	void setCheckZoom(bool value);
	bool getCheckZoom() { return bCheckZoom; }
	void SetWidthLine();
	void SetVolumeWidthLine(bool change = false);
	void SetWindowWidthLine(bool change = false);
	void AfterFullScreen();

	void CreateWindowCombo();
	
	void getWidthLevelValue(int & value, QPoint start, QPoint end, bool isLevel = true);
	QPointF getGeometry(float x, float y);

	
private:
	void setEmptyColor();
	
	qreal getValue(int pos);
private:
	int maxCount;
	bool bCheckZoom;
	bool bMoveLevel;
	QPoint startPos;
	QPoint movePos;
	QString screenFirstColor;
	QString screenSecondColor;
	QString volumeFirstColor;
	QString volumeSecondColor;
	QVector<QPointF> sample, logsample;

private slots:
	void OnComboChanged(int);
};
#endif
