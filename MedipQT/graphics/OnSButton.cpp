#include "stdafx.h"
#include "OnSButton.h"

QPainterPath OnSButton::path() const
{
	return myPath;
}

QPoint OnSButton::position() const
{
	return myPosition;
}

QColor OnSButton::color() const
{
	return myColor;
}

QString OnSButton::toolTip() const
{
	return myToolTip;
}

void OnSButton::setPath(const QPainterPath &path)
{
	myPath = path;
}

void OnSButton::setToolTip(const QString &toolTip)
{
	myToolTip = toolTip;
}

void OnSButton::setPosition(const QPoint &position)
{
	myPosition = position;
}

void OnSButton::setColor(const QColor &color)
{
	myColor = color;
}