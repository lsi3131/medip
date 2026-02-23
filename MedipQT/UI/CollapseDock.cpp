#include "stdafx.h"
#include "CollapseDock.h"

#include "MedipQT.h"
#include "Windows/windowManager.h"
#include "Windows/Main/MainSegmentWidget.h"
#include "Windows/WindowBase.h"

CollapseDock::CollapseDock(QWidget* parent, Qt::WindowFlags flags) : 
	QDockWidget(parent, flags)
{
	isMin = false;

	installEventFilter(this);

	setAttribute(Qt::WA_AcceptTouchEvents);
}

CollapseDock::~CollapseDock()
{
}

void CollapseDock::setWidget(QWidget* widget)
{
	orgWidget = widget;

	QDockWidget::setWidget(orgWidget);
}

void CollapseDock::setMinimum(bool _min)
{
	if (isMin == _min)
		return;

	if (!isFloating())
		return;

	isMin = _min;

	setUpdatesEnabled(false);

	QSize _size = sizeHint();

	if (isMin)
	{
		WindowBase* wb = WIN_MANAGER->mainSegmentWidget->getWindow(WT_AXIAL);

		if (wb)
		{
			if (wb->width() * 0.7 < _size.width())
				_size.setWidth(wb->width() * 0.7);

			if (wb->height() * 0.45 < _size.height())
				_size.setHeight(wb->height() * 0.45);

		}
	}

	resize(_size);

	setUpdatesEnabled(true);
}

void CollapseDock::setMinimumToScreen()
{
	setUpdatesEnabled(false);

	QSize _size = sizeHint();

	WindowBase* wb = WIN_MANAGER->mainSegmentWidget->getWindow(WT_AXIAL);

	if (wb)
	{
		if (wb->width() * 0.7 < _size.width())
			_size.setWidth(wb->width() * 0.7);

		if (wb->height() * 0.45 < _size.height())
			_size.setHeight(wb->height() * 0.45);

	}

	resize(_size);

	setUpdatesEnabled(true);
}

bool CollapseDock::eventFilter(QObject* watched, QEvent* e)
{
	if (watched == NULL)
		return QDockWidget::eventFilter(watched, e);

	if (watched != this &&
		watched != widget())
		return QDockWidget::eventFilter(watched, e);


	QEvent::Type tpe = e->type();

	if (tpe != QEvent::NonClientAreaMouseButtonDblClick &&
		tpe != QEvent::MouseButtonDblClick)
		return QDockWidget::eventFilter(watched, e);

	if (!isFloating())
		return QDockWidget::eventFilter(watched, e);


	QMouseEvent* mEv = (QMouseEvent*)e;
	if (tpe == QEvent::MouseButtonDblClick)
	{
		if (widget()->geometry().contains(mEv->pos()))
			return QDockWidget::eventFilter(watched, e);
	}


	isMin = !isMin;
	setUpdatesEnabled(false);
	QSize _size = sizeHint();

	if (isMin)
	{
		WindowBase* wb = WIN_MANAGER->mainSegmentWidget->getWindow(WT_AXIAL);

		if (wb)
		{
			if (wb->width() * 0.7 < _size.width())
				_size.setWidth(wb->width() * 0.7);

			if (wb->height() * 0.45 < _size.height())
				_size.setHeight(wb->height() * 0.45);

		}
	}

	resize(_size);

	setUpdatesEnabled(true);

	return true;
}

