#include "stdafx.h"
#include "CustomHistogram.h"
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
#include "DataContext.h"

#include <qwt_scale_widget.h>
#include <qwt_plot_picker.h>
#include <qwt_plot_histogram.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_marker.h>

CustomHistogram::CustomHistogram(QWidget* parent)
	:QwtPlot(parent)
{
	plotArea = NULL;
	plotArea2 = NULL;
	HuRangeSlider = NULL;
	m_WindowCombo = NULL;
	lineWidth = NULL;
	lineVolumeWidth = NULL;
	lineRange = NULL;
	bCheckZoom = false;
	bMoveLevel = false;
	EmptyDark = NULL;
	EmptyLight = NULL;
	actHidden = NULL;
	maxCount = 0;
	screenFirstColor = QString("background-color: rgba(0,0,0,255)");
	screenSecondColor = QString("background-color: rgba(255,255,255,255)");

	volumeFirstColor = QString("background-color: rgba(0,0,0,255)");
	volumeSecondColor = QString("background-color: rgba(255,255,255,255)");

	setStyleSheet("background-color: rgba(255, 255, 255, 255);");
	setMouseTracking(true);
}

CustomHistogram::~CustomHistogram()
{
}

void CustomHistogram::mousePressEvent(QMouseEvent* e)
{
	if (e->buttons() & Qt::LeftButton) // volumewidth 움직임
	{
		QRect rect(lineVolumeWidth->pos(), lineVolumeWidth->size());
		if (rect.contains(e->pos()))
		{
			bMoveLevel = true;
			startPos = e->pos();

			if (getComboType() == CL_3D)
			{
				WIN_MANAGER->useSkipRender(0);
			}

		}
	}
}

void CustomHistogram::mouseReleaseEvent(QMouseEvent* e)
{
	bMoveLevel = false;

	if (getComboType() == CL_3D)
	{
		WIN_MANAGER->useSkipRender(0, false);
		WIN_MANAGER->renderLater_3DView();
	}
}

void CustomHistogram::mouseMoveEvent(QMouseEvent* e)
{
	QRect rect(lineVolumeWidth->pos(), lineVolumeWidth->size());
	if (e->buttons() & Qt::LeftButton)
	{
		if (bMoveLevel) // volumewidth 움직임
		{
			int levelVal;
			int widthVal;
			int xGap, yGap;

			movePos = e->pos();
			xGap = abs(movePos.x() - startPos.x());
			yGap = abs(movePos.y() - startPos.y());

			if (xGap >= yGap)
			{
				if (getComboType() == CL_2D)
				{
					levelVal = WIN_MANAGER->getWindowLevel();

					getWidthLevelValue(levelVal, startPos, movePos);
					ACTION_MANAGER->action_Window_Level(levelVal);
				}
				else
				{
					levelVal = WIN_MANAGER->getVolumeLevel();

					getWidthLevelValue(levelVal, startPos, movePos);
					ACTION_MANAGER->action_Volume_Level(levelVal);
				}
			}
			else
			{
				if (getComboType() == CL_2D)
				{
					widthVal = WIN_MANAGER->getWindowWidth();

					getWidthLevelValue(widthVal, startPos, movePos, false);
					ACTION_MANAGER->action_Window_Width(widthVal);
				}
				else
				{
					widthVal = WIN_MANAGER->getVolumeWidth();

					getWidthLevelValue(widthVal, startPos, movePos, false);
					ACTION_MANAGER->action_Volume_Width(widthVal);
				}
			}
			resizeEvent(NULL);
			startPos = e->pos();
		}
	}
}

void CustomHistogram::mouseDoubleClickEvent(QMouseEvent* e)
{
	if (!WIN_MANAGER->IsLicensePass()) return;

	QRect rect(lineVolumeWidth->pos(), lineVolumeWidth->size());
	if (!rect.contains(e->pos())) return;

	ColorMapDialog dlg(getComboType(), this);

	dlg.exec();

	if (dlg.isChanged())
	{
		lineVolumeWidth->setPresetStops(dlg.getColorMapStops(), getComboType());
		switch (getComboType())
		{
		case CL_3D:
			WIN_MANAGER->updatePresetTexture(WIN_MANAGER->getSelectedVolumePreset(), WIN_MANAGER->getSelectedCustomPreset(CL_3D));
			WIN_MANAGER->renderLater_3DView();
			break;
		case CL_2D:
		default:
			DATA_CONTEXT->volume_data.updateColorTable();
			WIN_MANAGER->update2DPresetTexture(WIN_MANAGER->getSelectedPreset(), WIN_MANAGER->getSelectedCustomPreset(CL_2D));
			DATA_CONTEXT->volume_data.forceUpdateMPR();//2dviewdata
			WIN_MANAGER->forceUpdate2DViewData();
			setEmptyColor();
			WIN_MANAGER->renderLater_GridView(false);
			break;
		}

	}
	if (dlg.isAlphaChanged())
	{
		if (getComboType() == CL_3D)
		{
			WIN_MANAGER->updatePresetTexture(WIN_MANAGER->getSelectedVolumePreset(), WIN_MANAGER->getSelectedCustomPreset(CL_3D));
			WIN_MANAGER->renderLater_3DView();
		}
	}
	if (dlg.isWLApply())
	{
		int cusIndex = WIN_MANAGER->getSelectedCustomPreset(getComboType());
		bool res = cusIndex <= -1;
		switch (getComboType())
		{
		case CL_3D:
			WIN_MANAGER->setConfigWL(QString("VOLUME_WL%1").arg(res ? QString::number(WIN_MANAGER->getSelectedVolumePreset()) : ""),
				WIN_MANAGER->getVolumeWidth(), WIN_MANAGER->getVolumeLevel(), cusIndex);
			break;
		case CL_2D:
		default:
			WIN_MANAGER->setConfigWL(QString("SCREEN_WL%1").arg(res ? QString::number(WIN_MANAGER->getSelectedPreset()) : ""),
				WIN_MANAGER->getWindowWidth(), WIN_MANAGER->getWindowLevel(), cusIndex);
			break;
		}
	}
}

void CustomHistogram::resizeEvent(QResizeEvent* e)
{
	if (HuRangeSlider)
	{
		QPointF start, end;
		float width;
		int HURange = (DATA_CONTEXT->volume_data.getHuMax() - DATA_CONTEXT->volume_data.getHuMin()) / 2;

		HURange += (DATA_CONTEXT->volume_data.getHuMin());

		start = getGeometry((float)DATA_CONTEXT->volume_data.getHuMin(), 0.0f);
		end = getGeometry((float)DATA_CONTEXT->volume_data.getHuMax(), (float)getMaxCount());

		HuRangeSlider->setGeometry(start.x(), 0, end.x(), end.y());

		if (getComboType() == CL_2D)
			SetWindowWidthLine();
		else
			SetVolumeWidthLine();

		actHidden->setGeometry(QRect(this->width() - 30, 0, 25, 25));
		m_WindowCombo->setGeometry(QRect(this->width() - 125, 0, 90, 25));
	}
}

void CustomHistogram::initPlot()
{
	setStyleSheet("background-color: rgba(255, 255, 255, 255);");
	setAxisScale(QwtPlot::yLeft, 0, getMaxCount());
	setAxisScale(QwtPlot::xBottom, DATA_CONTEXT->volume_data.getHuMin(), DATA_CONTEXT->volume_data.getHuMax());
	axisWidget(QwtPlot::yLeft)->setContentsMargins(0, 0, 0, 0);
	axisWidget(QwtPlot::xBottom)->setContentsMargins(0, 0, 0, 0);

	axisWidget(QwtPlot::yLeft)->hide();
	axisWidget(QwtPlot::xBottom)->hide();

	enableAxis(QwtPlot::yLeft, false);
	enableAxis(QwtPlot::xBottom, false);
	plotLayout()->setAlignCanvasToScales(true);
	plotLayout()->setSpacing(0);
	setContentsMargins(0, 0, 0, 15);
	setFixedHeight(100);
}

void CustomHistogram::setChart(QVector<QPointF> sample, QVector<QPointF> logsample)
{
	this->sample = sample;
	this->logsample = logsample;
	bCheckZoom = false;
	QLinearGradient gradient(QPointF(0, 0), QPointF(1, 0));
	gradient.setColorAt(0.0, QColor(200, 200, 200));
	gradient.setCoordinateMode(QGradient::ObjectBoundingMode);

	plotArea->setCurveAttribute(QwtPlotCurve::Inverted);
	plotArea->setSamples(this->sample);
	plotArea->setPen(QColor(0, 0, 0, 255), 1);
	plotArea->setBrush(gradient);
	plotArea->setRenderHint(QwtPlotItem::RenderHint::RenderAntialiased);

	plotArea2->setCurveAttribute(QwtPlotCurve::Inverted);
	plotArea2->setSamples(this->logsample);
	plotArea2->setPen(QColor(255, 0, 0, 255), 1);
	plotArea2->setBrush(gradient);
	plotArea2->setRenderHint(QwtPlotItem::RenderHint::RenderAntialiased);

	plotArea->attach(this);

	canvas()->setCursor(Qt::ArrowCursor);
}

float CustomHistogram::getMaxCount()
{
	float zoomOut = plotArea->maxYValue();

	return zoomOut;
}

int CustomHistogram::getComboType()
{
	return m_WindowCombo->currentIndex();
}

void CustomHistogram::setCheckZoom(bool value)
{
	bCheckZoom = value;
	if (value)
	{
		plotArea->detach();
		plotArea2->attach(this);
		plotArea->attach(this);
	}
	else
		plotArea2->detach();

	replot();
}

void CustomHistogram::SetWidthLine()
{
	int index = getComboType();
	switch (index)
	{
	case CL_2D:
		SetWindowWidthLine(true);
		break;
	case CL_3D:
		SetVolumeWidthLine();
		break;
	default:
		break;
	}
}
void CustomHistogram::SetVolumeWidthLine(bool change)
{
	if (getComboType() != CL_3D) return;

	QPointF start, end;
	float width;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	width = WIN_MANAGER->getVolumeLevel() + ((float)WIN_MANAGER->getVolumeWidth() / 2);
	end = getGeometry(width, (float)getMaxCount());
	width = WIN_MANAGER->getVolumeLevel() - ((float)WIN_MANAGER->getVolumeWidth() / 2);
	start = getGeometry(width, 0);

	lineWidth->setGeometry(start.x(), 0, round(end.x() - start.x()), 82);
	//lineWidth->setStyleSheet(QString("background-color: rgba(0,255,255,100)"));
	lineWidth->setStyleSheet(QString("background-color:rgba(255, 255, 255, 80);"));


	lineVolumeWidth->setGeometry(start.x(), 82, round(end.x() - start.x()), 20);

	if (change)
		lineVolumeWidth->setPreset(WIN_MANAGER->getSelectedVolumePreset(), false, WIN_MANAGER->getSelectedCustomPreset(CL_3D));

	start = getGeometry(DATA_CONTEXT->volume_data.getHuMin(), getMaxCount());
	end = getGeometry(DATA_CONTEXT->volume_data.getHuMax(), 0);
	lineRange->setGeometry(start.x(), start.y(), round(end.x() - start.x()), 1);
	lineRange->setStyleSheet(QString("background-color: rgba(0,0,0,255)"));
}

void CustomHistogram::SetWindowWidthLine(bool change)
{
	if (getComboType() != CL_2D) return;

	QPointF start, end;
	int extend;
	float width;

	width = WIN_MANAGER->getWindowLevel() + ((float)WIN_MANAGER->getWindowWidth() / 2);
	end = getGeometry(width, (float)getMaxCount());
	width = WIN_MANAGER->getWindowLevel() - ((float)WIN_MANAGER->getWindowWidth() / 2);
	start = getGeometry(width, 0);

	lineWidth->setGeometry(start.x(), 0, round(end.x() - start.x()), 82);
	lineWidth->setStyleSheet(QString("background-color: rgba(0,255,255,100)"));

	lineVolumeWidth->setGeometry(start.x(), 82, round(end.x() - start.x()), 20);
	if (change)
	{
		lineVolumeWidth->setPreset(WIN_MANAGER->getSelectedPreset(), false, WIN_MANAGER->getSelectedCustomPreset(CL_2D));
		setEmptyColor();
	}
	////////////////////////////////////////////////////////////////////////
	extend = (int)(end.x() - start.x()) <= round(end.x() - start.x()) ? 0 : 1;

	width = WIN_MANAGER->getWindowLevel() - ((float)WIN_MANAGER->getWindowWidth() / 2);
	start = getGeometry(DATA_CONTEXT->volume_data.getHuMin(), 0);
	end = getGeometry(width, getMaxCount());

	EmptyDark->setGeometry(start.x(), 82, round(end.x() - start.x()), 20);
	EmptyDark->setStyleSheet(screenFirstColor);

	width = WIN_MANAGER->getWindowLevel() + ((float)WIN_MANAGER->getWindowWidth() / 2);
	start = getGeometry(width, 0);
	end = getGeometry(DATA_CONTEXT->volume_data.getHuMax(), getMaxCount());

	EmptyLight->setGeometry(start.x() - 1, 82, round(end.x() - start.x()), 20);
	EmptyLight->setStyleSheet(screenSecondColor);

	start = getGeometry(DATA_CONTEXT->volume_data.getHuMin(), getMaxCount());
	end = getGeometry(DATA_CONTEXT->volume_data.getHuMax(), 0);

	lineRange->setGeometry(start.x(), start.y(), round(end.x() - start.x()), 1);
	lineRange->setStyleSheet(QString("background-color: rgba(0,0,0,255)"));
}

void CustomHistogram::AfterFullScreen()
{
	resizeEvent(NULL);
}

void CustomHistogram::CreateWindowCombo()
{
	m_WindowCombo = new QComboBox(this);
	m_WindowCombo->addItem(STRING_MANAGER->getString(STR_2D_SCREEN));
	m_WindowCombo->addItem(STRING_MANAGER->getString(STR_3D_VOLUME));
	m_WindowCombo->setStyleSheet(STYLE_MANAGER->comboBoxNormal);
	connect(m_WindowCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(OnComboChanged(int)));
}

void CustomHistogram::getWidthLevelValue(int& value, QPoint start, QPoint end, bool isLevel)
{
	const int HURange = DATA_CONTEXT->volume_data.getHuMax() - DATA_CONTEXT->volume_data.getHuMin();
	int interval;

	if (isLevel)
	{
		interval = abs((getValue(start.x()) - getValue(end.x())));
		if ((start.x() - end.x()) < 0)
		{
			//levelVal val 증가
			if ((value + interval) <= DATA_CONTEXT->volume_data.getHuMax())
				value += interval;
			else
				value = DATA_CONTEXT->volume_data.getHuMax();
		}
		else if ((start.x() - end.x()) > 0)
		{
			//levelVal val 감소
			if ((value - interval) >= DATA_CONTEXT->volume_data.getHuMin())
				value -= interval;
			else
				value = DATA_CONTEXT->volume_data.getHuMin();
		}
	}
	else
	{
		interval = 10;
		if ((start.y() - end.y()) > 0)
		{
			//WidthVAL val 증가
			if ((value + interval) <= HURange)
				value += interval;
			else
				value = HURange;
		}
		else if ((start.y() - end.y()) < 0)
		{
			//WidthVAL val 감소
			if ((value - interval) >= 1)
				value -= interval;
			else
				value = 1;
		}
	}
}

void CustomHistogram::setEmptyColor()
{
	QColor left = lineVolumeWidth->getEdgeColor(true);
	QColor right = lineVolumeWidth->getEdgeColor(false);

	screenFirstColor = QString("background-color: rgba(%1,%2,%3,255)").arg(QString::number(left.red()),
		QString::number(left.green()), QString::number(left.blue()));
	screenSecondColor = QString("background-color: rgba(%1,%2,%3,255)").arg(QString::number(right.red()),
		QString::number(right.green()), QString::number(right.blue()));
	EmptyDark->setStyleSheet(screenFirstColor);
	EmptyLight->setStyleSheet(screenSecondColor);
}

QPointF CustomHistogram::getGeometry(float x, float y)
{
	QwtScaleMap xMap = canvasMap(QwtPlot::xBottom);
	QwtScaleMap yMap = canvasMap(QwtPlot::yLeft);

	xMap.setPaintInterval(2, this->width() - 1);
	xMap.setScaleInterval(DATA_CONTEXT->volume_data.getHuMin(), DATA_CONTEXT->volume_data.getHuMax());

	yMap.setPaintInterval(0, this->height() - 18);
	yMap.setScaleInterval(0, getMaxCount());
	return QPointF(xMap.transform(x),
		yMap.transform(y));
}

qreal CustomHistogram::getValue(int pos)
{
	return plotArea->plot()->invTransform(QwtPlot::xBottom, pos);
}

void CustomHistogram::OnComboChanged(int index)
{
	switch (index)
	{
	case CL_2D:
		lineVolumeWidth->setType(false);
		lineVolumeWidth->setPreset(WIN_MANAGER->getSelectedPreset(), false, WIN_MANAGER->getSelectedCustomPreset(CL_2D));
		//	lineVolumeWidth->setMapImage(screenGradient);
		SetWindowWidthLine();
		setEmptyColor();
		EmptyDark->show();
		EmptyLight->show();
		break;
	case CL_3D:
		lineVolumeWidth->setType(true);
		EmptyDark->hide();
		EmptyLight->hide();
		lineVolumeWidth->setPreset(WIN_MANAGER->getSelectedVolumePreset(), false, WIN_MANAGER->getSelectedCustomPreset(CL_3D));
		//	lineVolumeWidth->setMapImage(volumeGradient);
		SetVolumeWidthLine();
		break;
	default:
		break;
	}

}
