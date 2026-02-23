#pragma once

#ifndef ANNOARROW_H
#define ANNOARROW_H

#include "defineMEDIP.h"
#include "graphics/anotation.h"

class AnnoArrow : public Annotation
{
public:
	enum EVertex
	{
		VERTEX_1st,
		VERTEX_2nd,
	};

	enum ESelectedMode
	{
		SELECT_NONE,
		SELECT_LINE,
		SELECT_VERTEX_1st,
		SELECT_VERTEX_2nd,
	};

public:
	AnnoArrow(mip::VECTOR3 v1, mip::VECTOR3 v2, EState state, COLOR color = COLOR(255, 0, 0), bool isHidden = false);
	AnnoArrow(AnalMPRPlaneView* win, int x, int y, COLOR color = COLOR(255, 0, 0), bool isHidden = false);
	AnnoArrow(const AnnoArrow& rhs);
	virtual ~AnnoArrow();

	void Init(mip::VECTOR3 v1, mip::VECTOR3 v2, EState state, bool isHidden);

	void setV1(mip::VECTOR3 v) { m_v1 = v; }
	void setV2(mip::VECTOR3 v) { m_v2 = v; }

	mip::VECTOR3 getV1() { return m_v1; }
	mip::VECTOR3 getV2() { return m_v2; }

public:
	virtual ANNOTATION_TYPE getType() override;

	virtual bool addDrawingPoint_Measurement(AnalMPRPlaneView* win, int x, int y) override;

	virtual bool isContainLine_Measurement(AnalMPRPlaneView* win, int x, int y) override;
	virtual bool isContainVertex_Measurement(AnalMPRPlaneView* win, int x, int y) override;

	virtual void movePosition_Measurement(AnalMPRPlaneView* win, int x, int y) override;

	virtual bool isInMPRPlane_Measurement(AnalMPRPlaneView *win, WINDOW_TYPE type) override;
	virtual void drawVolume_Measurement(AnalVolumeView * win, QPainter *p) override;
	virtual void drawMPRPlane_Measurement(AnalMPRPlaneView * win, QPainter *p) override;
	virtual void drawVolume(VolumeView * win, QPainter *p) override;

private:
	bool isContainVertex_Measurement(EVertex* pOutVertex, AnalMPRPlaneView* win, int x, int y);
	void draw(QPainter * p, mip::VECTOR3 v1_screen, mip::VECTOR3 v2_screen);
	void drawLine(QPainter * p, QPoint pos1, QPoint pos2);
	void drawVertexEllipse(QPainter * p, QPoint pos, EVertex vertexPostion);

private:
	mip::VECTOR3 m_v1;
	mip::VECTOR3 m_v2;
	EVertex m_selectedVertexPosition;
};
#endif