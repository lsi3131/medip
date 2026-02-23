#pragma once

#ifndef ONSBUTTON_H
#define ONSBUTTON_H

#include "define.h"

#include <QColor>
#include <QPainterPath>
#include <QPoint>

class OnSButton
{
public:
	void setPath(const QPainterPath &path);
	void setToolTip(const QString &toolTip);
	void setPosition(const QPoint &position);
	void setColor(const QColor &color);

	QPainterPath path() const;
	QPoint position() const;
	QColor color() const;
	QString toolTip() const;

private:
	QPainterPath myPath;
	QPoint myPosition;
	QColor myColor;
	QString myToolTip;
};
#endif