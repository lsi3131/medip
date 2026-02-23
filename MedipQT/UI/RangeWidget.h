#pragma once

#ifndef RANGEWIDGET_H
#define RANGEWIDGET_H

#include "define.h"
#include <QWidget>
#include <QPainter>
#include <QStyle>
#include <QMouseEvent>

class QMenu;

class RangeWidget : public QWidget
{
    Q_OBJECT
private:
    Q_DISABLE_COPY(RangeWidget)

    Qt::Orientation _orientation;

    int _handleWidth;
    int _handleHeight;

    int _minimum;
    int _maximum;

    int _firstValue;
    int _secondValue;


	int _TipWidth;
	int _TipHeight;
	int _TipValue;
	bool _TipShow;
	QPoint _TipPoint;
	QFont _TipFont;

    bool _firstHandlePressed;
    bool _secondHandlePressed;

    bool _firstHandleHovered;
    bool _secondHandleHovered;

    QColor _firstHandleColor;
    QColor _secondHandleColor;

	QColor _fillColor;
	QColor _slideColor;

	bool bTrans;

	QMenu	*contextMenu;
	QAction *actSeedApply;
	QAction *actRangeApply;
protected:
    void paintEvent(QPaintEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);

    QRectF firstHandleRect() const;
    QRectF secondHandleRect() const;
    QRectF handleRect(int value) const;
    qreal span() const;

	void setContextMenu();
public:
    RangeWidget(Qt::Orientation orientation = Qt::Vertical, QWidget *parent = nullptr, bool bTrans = false);

    QSize minimumSizeHint() const;
	void setFillColor(QColor color) { _fillColor = color; }

    inline int firstValue() const { return _firstValue; }
    inline int secondValue() const { return _secondValue; }
    inline int minimum() const { return _minimum; }
    inline int maximum() const { return _maximum; }
    inline Qt::Orientation orientation() const { return _orientation; }
    inline int interval() const { return secondValue()-firstValue(); }
    inline unsigned int absInterval() const { return qAbs(interval()); }
	bool GetFirstPressed() { return _firstHandlePressed; }
	bool GetSecondPressed() { return _secondHandlePressed; }
signals:
    void firstValueChanged(int firstValue, bool bChanged=true);
    void secondValueChanged(int secondValue, bool bChanged = true);
    void rangeChanged(int min, int max);
    void sliderPressed();
    void sliderReleased();

private slots:
	void	slot_OnContextSeedApply();
	void	slot_OnContextRangeApply();


public slots:
    void setFirstValue(int firstValue, bool bChanged= true);
    void setSecondValue(int secondValue, bool bChanged= true);
    void setMinimum(int min);
    void setMaximum(int max);
    void setRange(int min, int max);
    void setOrientation(Qt::Orientation orientation);
};
#endif