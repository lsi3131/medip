#pragma once

#ifndef MEDIPMULTIBARCHART_H
#define MEDIPMULTIBARCHART_H

#include <qwidget.h>
#include <qwt_plot.h>

class AlphaColorMap;
class QComboBox;
class QPushButton;
class RangeWidget;
class QwtPlotCurve;
class QwtPlotPicker;
class QwtPlotBarChart;

class QwtPlotMultiBarChart;

#include <qwt_plot_renderer.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_barchart.h>
#include <qwt_column_symbol.h>
#include <qwt_plot_layout.h>
#include <qwt_legend.h>
#include <qwt_scale_draw.h>
#include <qwt_text.h>
#include <qstring.h>
#include <qcolor>
#include <qpainter.h>
#include <qpaintengine.h>
#include <qwt_painter.h>
#include <qwt_point_mapper.h>
#include <qwt_symbol.h>

//#define USE_DISTROWATCH_SAMPLE
#define USE_BARCHART_SAMPLE
#define USE_BARCHART
#define USE_PLOT_CURVE


#define OFFSET 0
#define SD_WIDTH 720 - OFFSET
#define SD_HEIGHT 480
#define COLOR_SKYBLUE 102, 153, 255

namespace
{
	class ScaleDraw : public QwtScaleDraw
	{
	public:
		ScaleDraw(Qt::Orientation orientation/*, const QStringList& labels*/)
			/*: m_labels(labels)*/
		{
			setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		}

#if 1
		virtual void drawLabel(QPainter* painter, double value) const // https://qwt.sourceforge.io/qwt__scale__draw_8cpp_source.html
		{
			QwtText lbl = tickLabel(painter->font(), value);
			if (lbl.isEmpty())
				return;
			
			QPointF pos = labelPosition(value);
			QSizeF labelSize = lbl.textSize(painter->font());

#if 1	//modify
			qreal val = (pos.x() + 1);
			pos.setX(val);

			//qreal width = labelSize.width();
			//labelSize.setWidth(width + 10);
#endif		

			const QTransform transform = labelTransformation(pos, labelSize);

			painter->save();
			painter->setWorldTransform(transform, true);

			lbl.draw(painter, QRect(QPoint(0, 0), labelSize.toSize()));

			painter->restore();
		}
#endif

		virtual QwtText label(double v) const
		{
			Alignment align = alignment();
			QString category = "";
			if (align == BottomScale) 
				category = "y";

			QwtText qwttext;
			qwttext.setColor(QColor("black"));
			if (align == BottomScale)
				qwttext.setText(QLocale().toString(v+1) + category);
			else
				qwttext.setText(QLocale().toString(v) + category);

			return qwttext;
		}

	private:
		const QStringList m_labels;
	};

}

class MedipMultiBarChart : public QwtPlot
{
	Q_OBJECT

public:
#ifdef USE_BARCHART
	QwtPlotMultiBarChart* m_barChartItem;
#endif
#ifdef USE_PLOT_CURVE //QwtPlotCurve
	QwtPlotCurve * m_plotCurve;
#endif

public:
	MedipMultiBarChart(QWidget *parent = NULL);
	~MedipMultiBarChart();

protected:
	void mousePressEvent(QMouseEvent *);
	void mouseReleaseEvent(QMouseEvent *);
	void mouseMoveEvent(QMouseEvent *);
	void mouseDoubleClickEvent(QMouseEvent *);
	void resizeEvent(QResizeEvent *);
	
public:
	void initPlotCharts();
	void initPlot();	
	void AfterFullScreen();
	//template<typename T>
	void populate(std::vector<float> & vecData);
	void refleshBarChart();

// ----------
#ifdef USE_BARCHART_SAMPLE
public Q_SLOTS:
	void setMode(int);
	void setOrientation(int);
	void exportChart();
	void doScreenShot(std::string strPath = "chartScreenShot-%2x%3");
	void exportPNG(std::string strPath, int width, int height);
	void render(QPainter* painter, const QRectF& targetRect);
private:
	
#endif
// ----------
// ----------
#ifdef USE_DISTROWATCH_SAMPLE
public Q_SLOTS:
	void setOrientation(int);
	void exportChart();
	void doScreenShot();

private:
	void populate();
	void exportPNG(int width, int height);
	void render(QPainter* painter, const QRectF& targetRect);

	QwtPlotBarChart* m_chartItem;
	QStringList m_distros;
#endif
// ----------
	
private:

private slots:

};

#endif