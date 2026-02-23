#pragma once

#ifndef ALPHA_COLORMAP_H
#define ALPHA_COLORMAP_H

#include "define.h"
#include <qwidget.h>
#include <qvector.h>


class AlphaColorMap : public QWidget
{
	Q_OBJECT

public:
	AlphaColorMap(QWidget *parent = 0 , int type = 1, bool colorChange = false);
	virtual ~AlphaColorMap();

	/*preset, gradient 설정*/
	void setPreset(SLICE_PRESET preset, bool reset = false, int cusIndex=-1);
	void setType(int type) { this->m_type = type; }
	int	getType() { return m_type; }
	/*custom preset add/del*/
	static void addPreset(int index,bool isImport=false);
	static bool delPreset(int index);

	/*변경된 default stops 적용*/
	void setPresetStops(QGradientStops stops, int type);
	void applyCustomStops();
	/*type(volume/screen)에 따른 preset gradient 획득*/
	static QGradientStops getPresetStops(SLICE_PRESET preset, int type, bool reset=false, int cusIndex = -1);
	
	static QGradientStops setGrayScale();

	QColor getColorPoint(int pos);
	void removeColorPoint(int pos);
	void moveColorPoint(int oldPos, qreal newPos);
	void changeColorPoint(int Pos, QColor changeColor);
	/*gradation point 위치 set*/
	void setColorPoints();
	void setColorChange() { m_colorChange = true; }
	
	bool isColorChange();
	/*color point 삭제를 위한 method(color dialog)*/
	QVector<qreal>& getColorPoints() { return m_points; }
	/*volume/screen color table return*/
	static QVector<QColor> getColorTable(SLICE_PRESET preset, int type, int cusIndex = -1, bool rst=false);
	/*screen histogram edge color return*/
	QColor getEdgeColor(bool isLeft);

	// radiomics color table에서만 사용
	void setStops(QGradientStops stops) { m_stops = stops; }

signals:
	void sig_AddPoints();
	void sig_MovePoints(int,qreal);

protected:
	/*mouse click 시, 알파/색 변경 (색은 default 일 경우에만)*/
	void mousePressEvent(QMouseEvent *e) override;
	/*alpha line 및 gradient draw*/
	void paintEvent(QPaintEvent *e) override;

private:
	/*current preset gradientstops*/
	QGradientStops m_stops;
	/*current preset color points*/
	QVector<qreal> m_points;
	SLICE_PRESET m_preset;
	int			m_custom;
	/*CL_2D/CL_3D*/
	int m_type;
	/*color change enable*/
	bool m_colorClick;
	/*color change check*/
	bool m_colorChange;
};
#endif