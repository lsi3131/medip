#include "stdafx.h"
#include "glwidget.h"
#include <QPainter>

#include "windowManager.h"
#include "LicenseManager.h"
#include "System/resourceManager.h"

#include "Windows/Tabwindow.h"

#include "Renderer/Renderer.h"

static const qreal MinimumDiameter = 3.0;
static const qreal MaximumDiameter = 50.0;

OpenGLWidget::OpenGLWidget(QWidget *parent)
	: QWidget(parent)
	, m_fullscreen(false)
	, m_tempPainter(NULL)
{
	m_touchEvent = false;
	m_touchZoomEvent = false;
	m_elapsed = 0;
	m_updateFrameSet = false;
	m_useOpenGL = false;
	m_windowType = WT_VOLUME;
	m_cursor = Qt::ArrowCursor;
	m_ctrl = m_shift = m_alt = false;

	setAutoFillBackground(false);
	setAttribute(Qt::WA_AcceptTouchEvents);
}

void OpenGLWidget::animate()
{
	m_elapsed = (m_elapsed + qobject_cast<QTimer*>(sender())->interval()) % 1000;
	update();
}

WINDOW_TYPE OpenGLWidget::getType()
{
	return m_windowType; 
}

void OpenGLWidget::setUpdateFrame(bool value)
{
	m_updateFrameSet = value;
	renderLater();
}

bool OpenGLWidget::getUpdateFrameState() 
{
	return m_updateFrameSet; 
}

QPainter* OpenGLWidget::getCurrentPainter() 
{
	return m_tempPainter; 
}

bool OpenGLWidget::getFullscreen() const 
{
	return m_fullscreen; 
}

void OpenGLWidget::render(QPainter *p)
{

}

void OpenGLWidget::paintEvent(QPaintEvent *event)
{
	/*if (m_useOpenGL == true)
	{
		render(NULL);
	}
	else
	{*/
	QPainter painter(this);
	m_tempPainter = &painter;
	render(m_tempPainter);
	m_tempPainter = NULL;
	//}
}

void OpenGLWidget::checkModifiers(QKeyEvent * e, bool _press, bool *chkRender)
{
	if (NULL == e) return;

	const int _modifier = e->key();

	if ((_modifier == Qt::Key_Alt) && (m_alt != _press))
	{
		m_alt = _press;
		if (chkRender)
		{
			(*chkRender) = true;
		}
	}

	if ((_modifier == Qt::Key_Control) && (m_ctrl != _press))
	{
		m_ctrl = _press;
		if (chkRender)
		{
			(*chkRender) = true;
		}
	}


	if ((_modifier == Qt::Key_Shift) && (m_shift != _press))
	{
		m_shift = _press;
		if (chkRender)
		{
			(*chkRender) = true;
		}
	}
}

void OpenGLWidget::checkModifiers(QMouseEvent * e, bool *chkRender)
{
	if (NULL == e)
	{
		return;
	}

	const Qt::KeyboardModifiers _modifier = e->modifiers();

	if ((_modifier & Qt::AltModifier) && !m_alt)
	{
		m_alt = true;
		if (chkRender)
		{
			(*chkRender) = true;
		}
	}
	else if (!(_modifier & Qt::AltModifier) && m_alt)
	{
		m_alt = false;
		if (chkRender)
		{
			(*chkRender) = true;
		}
	}

	if ((_modifier & Qt::ControlModifier) && !m_ctrl)
	{
		m_ctrl = true;
		if (chkRender)
		{
			(*chkRender) = true;
		}
	}
	else if (!(_modifier & Qt::ControlModifier) && m_ctrl)
	{
		m_ctrl = false;
		if (chkRender)
		{
			(*chkRender) = true;
		}
	}

	if ((_modifier & Qt::ShiftModifier) && !m_shift)
	{
		m_shift = true;
		if (chkRender)
		{
			(*chkRender) = true;
		}
	}
	else if (!(_modifier & Qt::ShiftModifier) && m_shift)
	{
		m_shift = false;
		if (chkRender)
		{
			(*chkRender) = true;
		}
	}
}

void OpenGLWidget::renderLater()
{
	//qDebug() << m_updateFrameSet << "(m_updateFrameSet)";

	if (m_updateFrameSet == true)
	{
		return;
	}

	update(); // QWidget::update()
}

void OpenGLWidget::drawWaterMark(QPainter * p)
{
//#if !defined(DEEP_CATCH_VER) && !defined(COVID19_VER)
	if (!WIN_MANAGER->IsLicensePass() || LICENSE_DATA->getTrialVer())
//#endif // COVID19
	{
		int width = this->width();
		int height = this->height();

		QImage * WaterImage = RESOURCE_MANAGER->getWaterMark();
		if (WaterImage != NULL)
		{
			p->drawImage(QPoint(width / 2 - width / 4 - WaterImage->size().width() / 2, (height / 2 - height / 4)), *WaterImage);
			p->drawImage(QPoint(width / 2 - width / 4 - WaterImage->size().width() / 2, (height / 2 + height / 4)), *WaterImage);
			p->drawImage(QPoint(width / 2 + width / 4 - WaterImage->size().width() / 2, (height / 2 - height / 4)), *WaterImage);
			p->drawImage(QPoint(width / 2 + width / 4 - WaterImage->size().width() / 2, (height / 2 + height / 4)), *WaterImage);
		}
	}
}


bool OpenGLWidget::event(QEvent * event)
{
	switch (event->type())
	{
	case QEvent::TouchBegin:
	case QEvent::TouchUpdate:
	case QEvent::TouchEnd:
	{
		QTouchEvent *touch = static_cast<QTouchEvent *>(event);
		QList<QTouchEvent::TouchPoint> touchPoints = static_cast<QTouchEvent *>(event)->touchPoints();

		int touch_count = touchPoints.size();
		int move_count = 0;

		int id = 0;
		foreach(const QTouchEvent::TouchPoint &touchPoint, touchPoints)
		{
			id++;
			if (id > 3)
			{
				continue;
			}

			switch (touchPoint.state())
			{
			case Qt::TouchPointStationary: // do not moved
				continue;
			case Qt::TouchPointReleased:
				touchRelease();
				if (id == 1) 
				{
					m_touchEvent = false; 
					m_touchZoomEvent = false; 
				}
				if (id == 2)
				{
					m_touchZoomEvent = false;
				}
				continue;
			case Qt::TouchPointPressed:
				if (id == 1) 
					m_touchEvent = true;

				if (id == 2) 
					m_touchZoomEvent = true;

				m_preTouchPoint[id - 1] = touchPoint.pos();
				continue;
			case Qt::TouchPointMoved:
			default:
			{
				m_currentTouchPoint[id - 1] = touchPoint.pos();
				move_count++;
			}
			break;
			}
		}

		if (move_count == 2)
		{
			QPointF v1 = m_currentTouchPoint[0] - m_preTouchPoint[0];
			QPointF v2 = m_currentTouchPoint[1] - m_preTouchPoint[1];
			if (QPointF::dotProduct(v1, v2) > 0) // move
			{
				//QApplication::postEvent(this, new QWheelEvent(QPointF(0, 0), 1, Qt::MouseButton(), Qt::KeyboardModifier()));
				touchMove(m_preTouchPoint[0].x(), m_preTouchPoint[0].y(), m_currentTouchPoint[0].x(), m_currentTouchPoint[0].y());
				touchMove(v1.x(), v1.y());
			}
			else // zoom
			{
				mip::VECTOR2 L1(float(m_preTouchPoint[0].x() - m_preTouchPoint[1].x()), float(m_preTouchPoint[0].y() - m_preTouchPoint[1].y()));
				mip::VECTOR2 L2(float(m_currentTouchPoint[0].x() - m_currentTouchPoint[1].x()), float(m_currentTouchPoint[0].y() - m_currentTouchPoint[1].y()));

				float dZoom = L2.length() - L1.length();
				touchZoom(dZoom);
				if (this)
					QApplication::postEvent(this, new QWheelEvent(QPointF(0, 0), dZoom >= 0.0f ? 1 : -1, Qt::MouseButton(), Qt::KeyboardModifier()));
			}

			m_preTouchPoint[0] = m_currentTouchPoint[0];
			m_preTouchPoint[1] = m_currentTouchPoint[1];
		}
		else if (move_count == 3)
		{


		}

		//id = 0;
		//foreach(const QTouchEvent::TouchPoint &touchPoint, touchPoints)
		//{
		//	id++;
		//	if (id > 3) continue;

		//	if ( Qt::TouchPointMoved == touchPoint.state() )
		//	{
		//		m_preTouchPoint[id - 1] = touchPoint.pos();
		//	}
		//}
	}
	break;

	default:
		return QWidget::event(event);
	}

	return true;
}