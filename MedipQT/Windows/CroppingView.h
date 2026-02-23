#pragma once

#ifndef CROPPINGVIEW_H
#define CROPPINGVIEW_H

#include <QPainter>
#include <qwidget.h>
#include "define.h"
#include "graphics\color.h"
#include "graphics\BoundingBox.h"
#include "UI\RangeWidget.h"
#include "Windows\windowManager.h"

class CroppingView : public QWidget
{
	Q_OBJECT

public:
	CroppingView(QWidget* parent = 0, WINDOW_TYPE eWinType = WT_AXIAL);
	~CroppingView();

public:
	void createData(int cx, int cy, int cz, int width, int level, mint16* pHUdata);	
	void destroyData();

	void initTexture(void);
	void initZoomFactor(float fSpacingX, float fSpacingY, bool isUpdate);

	void render(QPainter *p);
	void renderLater();

	void reset();

	void setBoundingBox(BoundingBoxI box);
	BoundingBoxI getBouningBox();
	MOUSE_ACTION_MODE getMouseAction();

	void setRotationAngle(float fAngle);
	float getRotationAngle();
	
	int getMaxDepth();
	void setDepth(int depth);
	int getDepth();

protected:
	void paintEvent(QPaintEvent *event) override;
	void wheelEvent(QWheelEvent *ev) override;
	void mousePressEvent(QMouseEvent *e) override;
	void mouseMoveEvent(QMouseEvent *e) override;
	void mouseReleaseEvent(QMouseEvent *e) override;

private:
	void updateTexture();
	void updateImageByTexture();
	void drawTexture(QPainter *p);
	void drawBoundLine(QPainter *p);
	QRect getPostion();

signals:
	void sig_angleUpdate(WINDOW_TYPE eWinType);
	void sig_BoxUpdate(WINDOW_TYPE eWinType);

private:
	const int PEN_SIZE = 5;

private:
	uchar* m_pTexture = nullptr;
	QImage m_image;

	WINDOW_TYPE m_eWinType;

	int m_cx;
	int m_cy;
	int m_cz;

	int m_imageWidth;
	int m_imageHeight;
	int m_imageDepth;

	float m_rotAngle = 0.f;

	mint16* m_pData_HU = nullptr;

	int m_window_width;
	int m_window_level;

	float m_zoomFactorX;
	float m_zoomFactorY;
	float m_fZoomStepX = 1.0;			// zoom의 단위 step 값
	float m_fZoomStepY = 1.0;

	float m_fMinZoomFactorX;
	float m_fMinZoomFactorY;
	float m_fMaxZoomFactorX;
	float m_fMaxZoomFactorY;
	int m_nZoomStepCnt = 100;
	int m_nMinZoomNum = 5;

	BoundingBoxI m_boundingBox;
	BoundingBoxI m_beforeClicked;
	MOUSE_ACTION_MODE m_mouse_action_mode;
	bool m_LbuttonDown;

	int m_depth;
};
#endif