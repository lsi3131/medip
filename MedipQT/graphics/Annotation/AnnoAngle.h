#pragma once

#ifndef ANNOANGLE_H
#define ANNOANGLE_H

#include "defineMEDIP.h"
#include "graphics/anotation.h"
#include <memory>

class AnnoAngle : public Annotation
{
	enum EVertex
	{
		VERTEX_1st,
		VERTEX_2nd,
		VERTEX_3rd,
	};

public:
	AnnoAngle(mip::VECTOR3 v1, mip::VECTOR3 v2, mip::VECTOR3 v3, EState state, COLOR color = COLOR(255, 0, 0), bool isHidden = false);
	AnnoAngle(AnalMPRPlaneView* win, int x, int y, COLOR color = COLOR(255, 0, 0), bool isHidden = false);
	AnnoAngle(const AnnoAngle& rhs);

	void Init(mip::VECTOR3 v1, mip::VECTOR3 v2, mip::VECTOR3 v3, EState state, bool isHidden);

	virtual ~AnnoAngle();

	float getAngle();

	void setV1(mip::VECTOR3 v) { m_v1 = v; }
	void setV2(mip::VECTOR3 v) { m_v2 = v; }
	void setV3(mip::VECTOR3 v) { m_v3 = v; }

	mip::VECTOR3 getV1() { return m_v1; }
	mip::VECTOR3 getV2() { return m_v2; }
	mip::VECTOR3 getV3() { return m_v3; }

public:
	virtual ANNOTATION_TYPE getType() override;

	virtual bool addDrawingPoint_Measurement(AnalMPRPlaneView* win, int x, int y) override;

	virtual bool isContainLine_Measurement(AnalMPRPlaneView* win, int x, int y) override;
	virtual bool isContainVertex_Measurement(AnalMPRPlaneView* win, int x, int y) override;
	virtual bool isContainTextBox_Measurement(AnalMPRPlaneView* win, int x, int y) override;

	virtual void movePosition_Measurement(AnalMPRPlaneView* win, int x, int y);

	virtual bool isInMPRPlane_Measurement(AnalMPRPlaneView *win, WINDOW_TYPE type) override;
	virtual void drawVolume_Measurement(AnalVolumeView * win, QPainter *p) override;
	virtual void drawMPRPlane_Measurement(AnalMPRPlaneView * win, QPainter *p) override;
	virtual void drawVolume(VolumeView * win, QPainter *p) override;

private:
	bool getContainVertex(EVertex* pOutVertex, AnalMPRPlaneView* win, int x, int y);

	void draw(QPainter * p, mip::VECTOR3 v1_screen, mip::VECTOR3 v2_screen, mip::VECTOR3 v3_screen, mip::VECTOR3 vText_screen);
	void drawLine(QPainter * p, QPoint pos1, QPoint pos2);
	void drawVertexEllipse(QPainter * p, QPoint pos, EVertex vertexPostion);
	void drawTextBox(QPainter * p, mip::VECTOR3 pos);

	QRect getDrawingTextBox(mip::VECTOR3 pos);
	QString getText();

	mip::VECTOR3 getTextVertex(AnalMPRPlaneView * win);
	mip::VECTOR3 getTextVertex_Volume(VolumeView * win);
	mip::VECTOR3 getTextVertex_Volume_Measurement(AnalVolumeView * win);
private:
	mip::VECTOR3 m_v1;
	mip::VECTOR3 m_v2;
	mip::VECTOR3 m_v3;

	std::unique_ptr<mip::VECTOR3> m_pVText;

private:
	EVertex m_selectedVertexPosition;
};
#endif