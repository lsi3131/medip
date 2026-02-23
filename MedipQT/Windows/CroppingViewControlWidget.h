#pragma once

#ifndef CROPPINGVIEW_CONTROL_WIDGET_H
#define CROPPINGVIEW_CONTROL_WIDGET_H

#include <qwidget>
#include <qdial>
#include <qspinbox>

class CroppingView;

class CroppingViewControlWidget : public QWidget
{
public:
	Q_OBJECT

public:
	CroppingViewControlWidget(QWidget *parent = NULL);
	virtual ~CroppingViewControlWidget();

public:
	void InitLayout(WINDOW_TYPE winType);
	void Init();
	void Reset();
	CroppingView* View();

private:
	int ToDialWidgetValue(float angle);
	float ToAngleFromDialWidget(int value);

Q_SIGNALS:
	void sig_viewRotated(WINDOW_TYPE type, int value);

private slots:
	void slot_OnScrollView(int value);
	void slot_OnDialRotateAnglelView(int value);
	void slot_OnSpinBoxRotateAnglelView(double value);

private:
	CroppingView* m_view;
	QScrollBar* m_viewScrollBar;
	QDial* m_dialRotateAngle;
	QDoubleSpinBox* m_spinBoxRotateAngle;
	WINDOW_TYPE m_winType;
};
#endif