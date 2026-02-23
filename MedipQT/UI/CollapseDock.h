#pragma once

#ifndef COLLAPSEDOCK_H
#define COLLAPSEDOCK_H

#include "define.h"
#include <QWidget>
#include <qdockwidget.h>

class QScrollArea;
class QVBoxLayout;

class CollapseDock : public QDockWidget
{
	Q_OBJECT

public:
// 	explicit CollapseDock(const QString &title, QWidget *parent = Q_NULLPTR,
// 		Qt::WindowFlags flags = Qt::WindowFlags());
	explicit CollapseDock(QWidget *parent = Q_NULLPTR,
		Qt::WindowFlags flags = Qt::WindowFlags());
	virtual ~CollapseDock();	
		
	void setWidget(QWidget *widget);
	void setMinimum(bool hidden);
	void setMinimumToScreen();
//	bool getHidden() { return isMin; }
protected:

	virtual bool eventFilter(QObject *watched, QEvent *e) override;
private:
	QWidget *orgWidget;
//	QWidget *hiddenWidget;
	bool isMin; /*false : orgWidget show, true : hiddenWidget show*/
};
#endif