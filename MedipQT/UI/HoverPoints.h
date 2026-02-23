#pragma once

#ifndef HOVERPOINTS_H
#define HOVERPOINTS_H

#include "define.h"
#include <qwidget.h>
#include <QPainter>
#include <QMouseEvent>

class QPushButton;

typedef QPoint QAlphaStop;
typedef QVector<QAlphaStop> QAlphaStops;

class HoverPoints : public QWidget
{
	Q_OBJECT

public:
	QAlphaStops getAlphaPoints() { return m_points; }
	void ResetAlphaPoint();
	static QVector<int> getAlphaGraph(int type, SLICE_PRESET preset, int cusIndex = -1, bool rst=false);
private:
	QPainterPath path;
	QPainterPath pathPoly;
	QAlphaStops m_points;
	bool m_move;
	int m_index;
	int m_type;
	int m_custom;
	bool m_change;
	SLICE_PRESET m_preset;
	QPoint startPos;
	QPoint movePos;
protected:
	void paintEvent(QPaintEvent *e);
	void mousePressEvent(QMouseEvent* e);
	void mouseMoveEvent(QMouseEvent* e);
	void mouseReleaseEvent(QMouseEvent* e);

public:
	HoverPoints(int type, SLICE_PRESET preset, QWidget *parent = nullptr, int cusIndex = -1);
	bool ApplyAlpha();
	bool isChange() { return m_change; }

	/*custom preset alpha add/del*/
	static void AddPresetAlpha(int index, bool isImport=false);
	static bool DelPresetAlpha(int index);

	static QAlphaStops GetPresetAlpha(SLICE_PRESET preset, int type, bool reset=false, int cusIndex = -1);
	static void SetPresetAlpha(QAlphaStops stops, SLICE_PRESET preset, int type, int cusIndex = -1);

signals:
	void ChangePoint();
};
#endif