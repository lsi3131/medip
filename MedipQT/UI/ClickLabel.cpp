#include "stdafx.h"
#include "UI/ClickLabel.h"

ClickLabel::ClickLabel(QWidget* parent /*= NULL*/)
	:QLabel(parent)
{
	_click = false;
}

void ClickLabel::mousePressEvent(QMouseEvent * ev)
{
	_click = true;
}

void ClickLabel::mouseReleaseEvent(QMouseEvent * ev)
{
	QPoint pos = ev->pos();

	if (_click)
		emit clicked();

	_click = false;
}
