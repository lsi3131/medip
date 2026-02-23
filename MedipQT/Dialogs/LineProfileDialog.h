#pragma once

#ifndef LINEPROFILE_DLG_H
#define LINEPROFILE_DLG_H

#include <qwidget.h>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include "mipEngine/geometry.h"
#include <qdialog.h>
#include <QMap>
class OpenGLWidget;
class QVBoxLayout;
class QPushButton;

class LineHistogram : public QwtPlot
{
	Q_OBJECT

public:
	LineHistogram(float length,QWidget *parent=NULL);
	~LineHistogram();

public:
	void initPlot();

	void CreateHistogram(QMap<float, mint16> huPoints, mint16 huMin, mint16 huMax);
	void setCheckZoom(bool value);
private:

	QwtPlotCurve plotArea;
	QwtPlotCurve plotArea2;
	QVector<QPointF> points;
	QVector<QPointF> logPoints;
	float length;
};

class LineProfileDialog : public QDialog
{
	Q_OBJECT

public:
	LineProfileDialog(QMap<float, mint16> huPoints, mint16 huMin, mint16 huMax, mint16 huAvg, float length, QWidget *parent = NULL);

private slots:
	void slot_OnLogHistogram();

private:
	QVBoxLayout * layout;
	LineHistogram *histogram;
	QPushButton *btnLog;
};
#endif