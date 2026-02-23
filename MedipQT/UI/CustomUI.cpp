#include "stdafx.h"
#include "UI/CustomUI.h"

CustomToolButton::CustomToolButton(QWidget* parent) : QToolButton(parent)
{

}

CustomToolButton::~CustomToolButton()
{

}

void CustomToolButton::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	QIcon cIcon = icon();

	QSize cSize = size();	

	QPixmap pixmap = cIcon.pixmap(cSize);
	QRect rect(0, 0, cSize.width(), cSize.height());
	
	painter.drawPixmap(rect, pixmap);

}
