#pragma once

#ifndef ANNOPRRECTANGLE_H
#define ANNOPRRECTANGLE_H

#include "defineMEDIP.h"
#include "graphics/anotation.h"
#include "AnnoRectangleCalculator.h"

class AnalMPRPlaneView;


class AnnoRectangle : public Annotation
{
public:
	enum EVertex
	{
		VERTEX_LEFT_TOP,
		VERTEX_RIGHT_TOP,
		VERTEX_LEFT_BOTTOM,
		VERTEX_RIGHT_BOTTOM,
	};

	struct RectangleVertex
	{
		mip::VECTOR3 LT;
		mip::VECTOR3 RT;
		mip::VECTOR3 LB;
		mip::VECTOR3 RB;
	};



public:
	AnnoRectangle(mip::VECTOR3 vLT, mip::VECTOR3 vRT, mip::VECTOR3 vLB, mip::VECTOR3 vRB, EState state, COLOR color = COLOR(255, 0, 0), bool isHidden = false);
	AnnoRectangle(AnalMPRPlaneView* win, int x, int y, COLOR color = COLOR(255, 0, 0), bool isHidden = false);
	AnnoRectangle(WindowBase* win, int x, int y, COLOR color = COLOR(255, 0, 0), bool isHidden = false);
	AnnoRectangle(const AnnoRectangle& rhs);

	virtual ~AnnoRectangle();

	void Init(mip::VECTOR3 vLT, mip::VECTOR3 vRT, mip::VECTOR3 vLB, mip::VECTOR3 vRB, EState state, bool isHidden);

	mip::VECTOR3 getVertexLeftTop() { return m_vLT; }
	mip::VECTOR3 getVertexRightTop() { return m_vRT; }
	mip::VECTOR3 getVertexLeftBottom() { return m_vLB; }
	mip::VECTOR3 getVertexRightBottom() { return m_vRB; }

	void setVertex(mip::VECTOR3 vLT,
		mip::VECTOR3 vRT,
		mip::VECTOR3 vLB,
		mip::VECTOR3 vRB)
	{
		m_vLT = vLT;
		m_vRT = vRT;
		m_vLB = vLB;
		m_vRB = vRB;
	}

public:
	virtual ANNOTATION_TYPE getType() override;

	virtual bool addDrawingPoint_Measurement(AnalMPRPlaneView* win, int x, int y) override;
	virtual bool addDrawingPoint_Window(WindowBase* win, int x, int y) override;

	virtual bool isContainLine_Measurement(AnalMPRPlaneView* win, int x, int y) override;
	virtual bool isContainVertex_Measurement(AnalMPRPlaneView* win, int x, int y) override;
	virtual bool isContainTextBox_Measurement(AnalMPRPlaneView* win, int x, int y) override;

	virtual bool isContainLine_Window(WindowBase* win, int x, int y) override;
	virtual bool isContainVertex_Window(WindowBase* win, int x, int y) override;
	virtual bool isContainTextBox_Window(WindowBase* win, int x, int y) override;

	virtual void movePosition_Measurement(AnalMPRPlaneView* win, int x, int y) override;
	virtual void movePosition_Window(WindowBase* win, int x, int y) override;

	virtual bool isInMPRPlane_Measurement(AnalMPRPlaneView *win, WINDOW_TYPE type) override;
	virtual bool isInMPRPlane_Window(WindowBase * win, WINDOW_TYPE type) override;

	virtual void drawVolume_Measurement(AnalVolumeView * win, QPainter *p) override;
	virtual void drawMPRPlane_Measurement(AnalMPRPlaneView * win, QPainter *p) override;

	virtual void drawVolume(VolumeView * win, QPainter *p) override;
	virtual void drawMPRPlane_Window(WindowBase * win, QPainter *p) override;
private:
	bool addDrawingPoint(int x, int y);

	bool getContainVertex(EVertex* pOutVertex, int x, int y);
	bool isContainLine(int x, int y);
	bool isContainTextBox(int x, int y);

	void movePosition(int x, int y);

	void drawMPRPlane(QPainter* p);

	void drawRectangle(QPainter * p, QRect rect);
	void drawVertexEllipse(QPainter * p, QPoint pos, EVertex vertexPostion);
	void drawTextBox(QPainter * p, int x, int y);

	QRect getDrawingRectangle();
	QRect getDrawingTextBox(int x, int y);
	QStringList getTextList();

	void updateRectangleVertex_ByTempVertex(mip::VECTOR3 v_new);

	void moveRectangleVertex(mip::VECTOR3 v);
	mip::VECTOR3 getTextVertex();

	void updateRectangleVertex_ByRectScreen(QRect rect_screen, bool normalizeRect);

	void useMeasurmentMode(AnalMPRPlaneView* pMeasurementView);
	void useWindowMode(WindowBase* pWindowView);


	mip::VECTOR3 getVertex_ScreenToWorld(int x, int y);
	mip::VECTOR3 getVertex_WorldToScreen(mip::VECTOR3 v);

	void updateRectangleAreaInfo();
private:
	WindowBase* m_pWindowView = nullptr;
	AnalMPRPlaneView* m_pMeasurementView = nullptr;

	mip::VECTOR3 m_vTempStart;
	mip::VECTOR3 m_vTempEnd;
	mip::VECTOR3 m_vLT;
	mip::VECTOR3 m_vRT;
	mip::VECTOR3 m_vLB;
	mip::VECTOR3 m_vRB;

	std::unique_ptr<mip::VECTOR3> m_pVText;

	EVertex m_selectedVertex;
	int m_textBoxWidth;
	int m_textLineHeight;

	bool m_isRectangleAreaShouldUpdated;

	AnnoRectangleCalculator<mint16> m_currentHUInfo;
	AnnoRectangleCalculator<float> m_currentSUVInfo;
};

#endif