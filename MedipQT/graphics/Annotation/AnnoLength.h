#pragma once

#ifndef ANNOLENGTH_H
#define ANNOLENGTH_H

#include "defineMEDIP.h"
#include "graphics/anotation.h"
#include <memory>

class AnnoLength : public Annotation
{
public:
	enum EVertex
	{
		VERTEX_1st,
		VERTEX_2nd,
	};

public:
	AnnoLength(mip::VECTOR3 v1, mip::VECTOR3 v2, EState state, COLOR color = COLOR(255, 0, 0), bool isHidden = false);
	AnnoLength(AnalMPRPlaneView* win, int x, int y, COLOR color = COLOR(255, 0, 0), bool isHidden = false);
	AnnoLength(const AnnoLength& rhs);
	virtual ~AnnoLength();

	void Init(mip::VECTOR3 v1, mip::VECTOR3 v2, EState state, bool isHidden);

	float getLength();

	void setV1(mip::VECTOR3 v) { m_v1 = v; }
	void setV2(mip::VECTOR3 v) { m_v2 = v; }

	mip::VECTOR3 getV1() { return m_v1; }
	mip::VECTOR3 getV2() { return m_v2; }

public:
	virtual ANNOTATION_TYPE getType() override;

	virtual bool addDrawingPoint_Measurement(AnalMPRPlaneView* win, int x, int y) override;

	virtual bool isContainLine_Measurement(AnalMPRPlaneView* win, int x, int y) override;
	virtual bool isContainVertex_Measurement(AnalMPRPlaneView* win, int x, int y) override;
	virtual bool isContainTextBox_Measurement(AnalMPRPlaneView* win, int x, int y) override;

	virtual bool isContainVertex_Measurement_Vol(AnalVolumeView* win, int x, int y) override;

	virtual void movePosition_Measurement(AnalMPRPlaneView* win, int x, int y) override;
	virtual void movePosition_Measurement_Vol(AnalVolumeView* win, int x, int y, mip::VECTOR3 v = mip::VECTOR3(0,0,0)) override;

	virtual bool isInMPRPlane_Measurement(AnalMPRPlaneView *win, WINDOW_TYPE type) override;
	virtual void drawVolume_Measurement(AnalVolumeView * win, QPainter *p) override;
	virtual void drawMPRPlane_Measurement(AnalMPRPlaneView * win, QPainter *p) override;
	virtual void drawVolume(VolumeView * win, QPainter *p) override;
	virtual void drawPoint_VolumeMeasurement(AnalMPRPlaneView * win, QPainter *p, WINDOW_TYPE type) override;

private:
	bool isContainVertex_Measurement(EVertex* pOutVertex, AnalMPRPlaneView* win, int x, int y);
	bool isContainVertex_Measurement_Vol(EVertex* pOutVertex, AnalVolumeView* win, int x, int y);

	void draw(QPainter * p, mip::VECTOR3 v1_screen, mip::VECTOR3 v2_screen, mip::VECTOR3 vText_screen);
	void drawLine(QPainter * p, QPoint pos1, QPoint pos2);
	void drawVertexEllipse(QPainter * p, QPoint pos, EVertex vertexPostion);
	void drawTextBox(QPainter * p, mip::VECTOR3 pos);
	void drawPoint(QPainter * p, mip::VECTOR3 v_screen, EVertex vertexPostion);

	QRect getDrawingTextBox(mip::VECTOR3 pos);
	QString getText();

	mip::VECTOR3 getTextVertex(AnalMPRPlaneView * win);
	mip::VECTOR3 getTextVertex_Volume(VolumeView * win);
	mip::VECTOR3 getTextVertex_Volume_Measurement(AnalVolumeView * win);
	virtual int  getCnt_IsInMPRPlane_Measurement(AnalMPRPlaneView *win, WINDOW_TYPE type);

private:
	mip::VECTOR3 m_v1;
	mip::VECTOR3 m_v2;
	std::unique_ptr<mip::VECTOR3> m_pVText;

	EVertex m_selectedVertexPosition;
};
#endif