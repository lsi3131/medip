#pragma once

#ifndef CLICKLABEL_H
#define CLICKLABEL_H

#include "define.h"
#include <QLabel>

//For color context menu(2019.01)
class ClickLabel : public QLabel
{
	Q_OBJECT

public:
	ClickLabel(QWidget* parent = NULL);

protected:
	void mousePressEvent(QMouseEvent *ev) override; //click check
	void mouseReleaseEvent(QMouseEvent *ev)override; //click check

public:
signals:
	void clicked();

private:
	bool _click;
};
#endif
