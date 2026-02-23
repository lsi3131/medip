#pragma once

#ifndef LAYTER_HISTOGRAM_H
#define LAYTER_HISTOGRAM_H

#include <qwidget.h>
#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>
#include "define.h"

class QAction;

class LayerHistogram : public QwtPlot
{
	Q_OBJECT

public:
	LayerHistogram(int layerUID, QWidget *parent = NULL);
	~LayerHistogram();

public:
	void initPlot();

	void CreateHistogram(int fileSeq=-1);
	void SetHistogram(mint16 _min, mint16 _max, QVector<QPointF> _points, QVector<QPointF> _logPoints, int fileSeq=-1);
	void setCheckZoom(bool value);

	QVector<QPointF> getPoints(bool _log = false);
	bool getRange(mint16&, mint16&);
	void OnSaveMacro(QString fileName);

protected:
	void mouseReleaseEvent(QMouseEvent *evt) override;


public slots:
	void OnCopy();
	void OnSave();

private:
	int				fileSeq;
	QAction			*m_actSave;
	QAction			*m_actCopy;
	int m_uid;
	int m_maskIndex;
	mint16 HUMin;
	mint16 HUMax;
	float HURange;
	QwtPlotCurve plotArea;
	QwtPlotCurve plotArea2;
	QVector<QPointF> points;
	QVector<QPointF> logPoints;
};
#endif