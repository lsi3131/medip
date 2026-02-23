#include "stdafx.h"
#include "RangeWidget.h"
#include "Windows/windowManager.h"
#include "UI/CustomHistogram.h"
#include "Windows/Main/MainSegmentWidget.h"
#include "System/stringManager.h"
#include "Windows/Tabwindow.h"

RangeWidget::RangeWidget(Qt::Orientation orientation, QWidget* parent, bool bTrans)
	: QWidget(parent),
	_orientation(orientation),
	_handleWidth(0),
	_handleHeight(0),
	_minimum(0),
	_maximum(100),
	_firstValue(10),
	_secondValue(90),
	_TipValue(0),
	_TipShow(false),
	_firstHandlePressed(false),
	_secondHandlePressed(false),
	_firstHandleColor(style()->standardPalette().highlight().color()),
	_secondHandleColor(style()->standardPalette().highlight().color())
{
	setMouseTracking(true);
	this->bTrans = bTrans;
	if (bTrans)
	{
		_fillColor = QColor(156, 27, 66, 110);
		//	_fillColor = QColor(48, 48, 48, 110);
		_slideColor = QColor(48, 48, 48, 10);
	}
	else
	{
		_fillColor = QColor(203, 228, 239, 255);
		_slideColor = QColor(48, 48, 48, 255);
	}
	contextMenu = new QMenu(this);
	actSeedApply = new QAction(STRING_MANAGER->getString(STR_SEED_APPLY), this);
	actRangeApply = new QAction(STRING_MANAGER->getString(STR_ALL_SELECT), this);

	connect(actSeedApply, &QAction::triggered, this, &RangeWidget::slot_OnContextSeedApply);
	connect(actRangeApply, &QAction::triggered, this, &RangeWidget::slot_OnContextRangeApply);

	setContextMenu();

	if (!(PRODUCT_MANAGER->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(PRODUCT_MANAGER->m_strMedipMDBox))
	{
		_handleWidth = WIN_MANAGER->ScreenWidth / 128;
		_handleHeight = WIN_MANAGER->ScreenHeight / 27;
	}
	else
	{
		_handleWidth = WIN_MANAGER->ScreenWidth / 192;
		_handleHeight = WIN_MANAGER->ScreenHeight / 27;
	}

}

void RangeWidget::paintEvent(QPaintEvent* event)
{
	QPainter p(this);
	QPainterPath path;
	// First value handle rect
	QRectF rv1 = firstHandleRect();
	QColor c1(_firstHandleColor);
	if (_firstHandleHovered)
		c1 = c1.darker();

	// Second value handle rect
	QRectF rv2 = secondHandleRect();
	QColor c2(_secondHandleColor);
	if (_secondHandleHovered)
		c2 = c2.darker();

	// Background
	QRect r;
	if (_orientation == Qt::Horizontal)
		r = QRect(0, (height() - _handleWidth) / 2, width() - 1, _handleWidth);
	else
		r = QRect((width() - _handleWidth) / 2, 0, _handleWidth, height() - 1);
	p.fillRect(r, _slideColor);

	// Handles
	QRectF rf(r);
	if (_orientation == Qt::Horizontal)
	{
		rf.setLeft(rv1.right());
		rf.setRight(rv2.left());
		rf.setBottom(rf.bottom() + 1);
	}
	else
	{
		rf.setTop(rv1.bottom());
		rf.setBottom(rv2.top());
		rf.setRight(rf.right() + 1);
	}
	p.fillRect(rf, _fillColor);
	p.fillRect(rv1, c1);
	p.fillRect(rv2, c2);

	if (bTrans)
	{
		if (_TipShow)
		{
			if (_TipPoint.x() < 0) _TipPoint.setX(0);
			else if (_TipPoint.x() >= this->width()) _TipPoint.setX(this->width() - 1);

			p.setPen(QPen(QBrush(QColor(255, 255, 255)), 0.5));
			p.setBrush(QColor(48, 48, 48));
			p.setRenderHints(QPainter::TextAntialiasing | QPainter::Antialiasing);
			if (_TipPoint.x() + _TipWidth < this->width()) // (x). .
			{
				path.moveTo(QPointF(_TipPoint.x(), (this->height() - 1) - 15));
				path.lineTo(QPointF(_TipPoint.x(), (this->height() - 1)));
				path.lineTo(QPointF(_TipPoint.x() + 5, (this->height() - 1) - 15));
				path.addRoundedRect(QRectF(_TipPoint.x() - 2, (this->height() - 2) - (_TipHeight + 5 + 17), _TipWidth + 8, _TipHeight + 8), 5, 5);
				path.addText(QPointF(_TipPoint.x() + 2, (this->height() - 1) - 20), _TipFont, QString("%2: %1").arg(_TipValue).arg(WIN_MANAGER->getUnitString()));
				path = path.simplified();
				p.drawPath(path);
			}
			else // . .(x)
			{
				path.moveTo(QPointF(_TipPoint.x() - 5, (this->height() - 1) - 15));
				path.lineTo(QPointF(_TipPoint.x(), (this->height() - 1)));
				path.lineTo(QPointF(_TipPoint.x(), (this->height() - 1) - 15));
				path.addRoundedRect(QRectF(_TipPoint.x() - (4 + _TipWidth), (this->height() - 2) - (_TipHeight + 5 + 17), _TipWidth + 8, _TipHeight + 8), 5, 5);
				path.addText(QPointF(_TipPoint.x() - (2 + _TipWidth), (this->height() - 2) - 20), _TipFont, QString("%2: %1").arg(_TipValue).arg(WIN_MANAGER->getUnitString()));
				path = path.simplified();
				p.drawPath(path);
			}
		}
	}
}

qreal RangeWidget::span() const
{
	int interval = qAbs(_maximum - _minimum);

	if (_orientation == Qt::Horizontal)
		return qreal(width() - _handleWidth) / qreal(interval);
	else
		return qreal(height() - _handleWidth) / qreal(interval);
}

void RangeWidget::setContextMenu()
{
	contextMenu->setStyleSheet("background: rgba(48, 48, 48, 255);");
	contextMenu->setContextMenuPolicy(Qt::DefaultContextMenu);

	contextMenu->addAction(actSeedApply);
	contextMenu->addAction(actRangeApply);
}

QRectF RangeWidget::firstHandleRect() const
{
	return handleRect(_firstValue);
}

QRectF RangeWidget::secondHandleRect() const
{
	return handleRect(_secondValue);
}

QRectF RangeWidget::handleRect(int value) const
{
	qreal s = span();

	QRectF r;
	if (_orientation == Qt::Horizontal)
	{
		r = QRectF(0, (height() - _handleHeight) / 2, _handleWidth, _handleHeight);
		r.moveLeft(s * (value - _minimum));
	}
	else
	{
		r = QRectF((width() - _handleHeight) / 2, 0, _handleHeight, _handleWidth);
		r.moveTop(s * (value - _minimum));
	}
	return r;
}

void RangeWidget::mousePressEvent(QMouseEvent* event)
{
	int interval = qAbs(_maximum - _minimum);

	if (event->buttons() & Qt::LeftButton)
	{
		_TipShow = true;
		_secondHandlePressed = secondHandleRect().contains(event->pos());
		_firstHandlePressed = !_secondHandlePressed && firstHandleRect().contains(event->pos());
		_TipValue = event->pos().x() * interval / (width() - _handleWidth) + (_minimum);
		if (_TipValue > _maximum) _TipValue = _maximum;
		else if (_TipValue < _minimum) _TipValue = _minimum;
		_TipPoint = event->pos();
		if ((_secondHandlePressed || _firstHandlePressed) && bTrans)
		{
			//	_TipValue = event->pos().x()*interval / (width() - _handleWidth) + (_minimum);
			//	WIN_MANAGER->mainSegmentWidget->getHistogramView()->OnSliderTooltipShow(_TipValue);
			QFontMetrics metrics(_TipFont);
			_TipWidth = metrics.boundingRect(QString("%2: %1").arg(_TipValue).arg(WIN_MANAGER->getUnitString())).width();
			_TipHeight = metrics.boundingRect(QString("%2: %1").arg(_TipValue).arg(WIN_MANAGER->getUnitString())).height();

			repaint();
		}
		emit sliderPressed();
	}
	else if (event->buttons() & Qt::RightButton)
	{
		if (bTrans)
		{
			//contextmenu »ý¼º
			contextMenu->exec(event->globalPos());

			WIN_MANAGER->renderLater_GridView(false);
		}
	}


}

void RangeWidget::mouseMoveEvent(QMouseEvent* event)
{
	int interval = qAbs(_maximum - _minimum);
	int tmpVal;
	if (event->buttons() & Qt::LeftButton)
	{
		const int range = 20;
		//	interval += _minimum;
		if (_secondHandlePressed)
		{
			if (_orientation == Qt::Horizontal)
			{
				tmpVal = event->pos().x() * interval / (width() - _handleWidth) + (_minimum);
				if (tmpVal > (firstValue() + range))
					setSecondValue(event->pos().x() * interval / (width() - _handleWidth) + (_minimum));
			}
			else
			{
				tmpVal = event->pos().y() * interval / (height() - _handleWidth) + (_minimum);
				if (tmpVal > (firstValue() + range))
					setSecondValue(event->pos().y() * interval / (height() - _handleWidth) + (_minimum));
			}
		}
		else if (_firstHandlePressed)
		{
			if (_orientation == Qt::Horizontal)
			{
				tmpVal = (event->pos().x() * interval / (width() - _handleWidth)) + (_minimum);
				if (tmpVal < (secondValue() - range))
					setFirstValue((event->pos().x() * interval / (width() - _handleWidth)) + (_minimum));
			}
			else
			{
				tmpVal = event->pos().y() * interval / (height() - _handleWidth) + (_minimum);
				if (tmpVal < (secondValue() - range))
					setFirstValue(event->pos().y() * interval / (height() - _handleWidth) + (_minimum));
			}
		}
		if (bTrans)
		{
			tmpVal = event->pos().x() * interval / (width() - _handleWidth) + (_minimum);
			if (_minimum <= tmpVal && tmpVal <= _maximum)
			{
				_TipShow = true;
				_TipValue = tmpVal;
				_TipPoint = event->pos();
				//		WIN_MANAGER->mainSegmentWidget->getHistogramView()->OnSliderTooltipShow(_TipValue);
				QFontMetrics metrics(_TipFont);
				_TipWidth = metrics.boundingRect(QString("%2: %1").arg(_TipValue).arg(WIN_MANAGER->getUnitString())).width();
				_TipHeight = metrics.boundingRect(QString("%2: %1").arg(_TipValue).arg(WIN_MANAGER->getUnitString())).height();

				repaint();
			}
		}
	}
	else if (bTrans)
	{
		_TipShow = false;
		//	WIN_MANAGER->mainSegmentWidget->getHistogramView()->OnSliderTooltipHide();
		repaint();
	}

	QRectF rv2 = secondHandleRect();
	QRectF rv1 = firstHandleRect();
	_secondHandleHovered = _secondHandlePressed || (!_firstHandlePressed && rv2.contains(event->pos()));
	_firstHandleHovered = _firstHandlePressed || (!_secondHandleHovered && rv1.contains(event->pos()));
	update(rv2.toRect());
	update(rv1.toRect());
}

void RangeWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (bTrans)
	{
		_TipShow = false;
		//		WIN_MANAGER->mainSegmentWidget->getHistogramView()->OnSliderTooltipHide();
		repaint();
	}

	if (_firstHandlePressed || _secondHandlePressed)
		emit sliderReleased();


	_firstHandlePressed = false;
	_secondHandlePressed = false;
}

QSize RangeWidget::minimumSizeHint() const
{
	return QSize(_handleHeight, _handleHeight);
}


void RangeWidget::slot_OnContextSeedApply()
{
	if (WIN_MANAGER->GetTab())
	{
		ThreSholdTab* tab = WIN_MANAGER->GetTab()->getThreSholdTab();

		if (tab)
		{
			tab->ApplyRegionGrowing();
		}
	}
	/*ROITab * tab = static_cast<ROITab *>(WIN_MANAGER->mainSegmentWidget->getRoiTab());

	if (NULL == tab) return;

	tab->contextRGOK();*/
}

void RangeWidget::slot_OnContextRangeApply()
{
	if (WIN_MANAGER->GetTab())
	{
		ThreSholdTab* tab = WIN_MANAGER->GetTab()->getThreSholdTab();

		if (tab)
		{
			tab->ApplyThreshold();
		}
	}
	//ROITab * tab = static_cast<ROITab *>(WIN_MANAGER->mainSegmentWidget->getRoiTab());

	//if (NULL == tab) return;

	//tab->contextALLSelect();
}

void RangeWidget::setSecondValue(int secondValue, bool bChanged)
{
	if (secondValue > _maximum)
		secondValue = _maximum;

	if (secondValue < _minimum)
		secondValue = _minimum;

	_secondValue = secondValue;
	emit secondValueChanged(_secondValue, bChanged);

	update();
}

void RangeWidget::setFirstValue(int firstValue, bool bChanged)
{
	if (firstValue > _maximum)
		firstValue = _maximum;

	if (firstValue < _minimum)
		firstValue = _minimum;

	_firstValue = firstValue;
	emit firstValueChanged(_firstValue, bChanged);

	update();
}

void RangeWidget::setMaximum(int max)
{
	if (max >= minimum())
		_maximum = max;
	else
	{
		int oldMin = minimum();
		_maximum = oldMin;
		_minimum = max;
	}

	update();

	if (firstValue() > maximum())
		setFirstValue(maximum());

	if (secondValue() > maximum())
		setSecondValue(maximum());

	emit rangeChanged(minimum(), maximum());
}

void RangeWidget::setRange(int min, int max)
{
	setMinimum(min);
	setMaximum(max);
}

void RangeWidget::setMinimum(int min)
{
	if (min <= maximum())
		_minimum = min;
	else
	{
		int oldMax = maximum();
		_minimum = oldMax;
		_maximum = min;
	}

	update();

	if (firstValue() < minimum())
		setFirstValue(minimum());

	if (secondValue() < minimum())
		setSecondValue(minimum());

	emit rangeChanged(minimum(), maximum());
}

void RangeWidget::setOrientation(Qt::Orientation orientation)
{
	if (_orientation == orientation)
		return;

	_orientation = orientation;
	update();
}
