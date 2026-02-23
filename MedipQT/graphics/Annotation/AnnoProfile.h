#pragma once

#ifndef ANNOPROFILE_H
#define ANNOPROFILE_H

#include "defineMEDIP.h"
#include "graphics/anotation.h"
#include "mipEngine/geometry.h"

class AnnoProfile : public Annotation
{
public:
	enum EVertex
	{
		VERTEX_1st,
		VERTEX_2nd,
	};

public:
	AnnoProfile(mip::VECTOR3 v1, mip::VECTOR3 v2, EState state, COLOR color = COLOR(255, 0, 0), bool isHidden = false);
	AnnoProfile(AnalMPRPlaneView* win, int x, int y, COLOR color = COLOR(255, 0, 0), bool isHidden = false);
	virtual ~AnnoProfile();

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

	virtual void movePosition_Measurement(AnalMPRPlaneView* win, int x, int y) override;

	virtual bool isInMPRPlane_Measurement(AnalMPRPlaneView *win, WINDOW_TYPE type) override;
	virtual void drawVolume_Measurement(AnalVolumeView * win, QPainter *p) override;
	virtual void drawMPRPlane_Measurement(AnalMPRPlaneView * win, QPainter *p) override;
	virtual void drawVolume(VolumeView * win, QPainter *p) override;
private:
	bool isContainVertex_Measurement(EVertex* pOutVertex, AnalMPRPlaneView* win, int x, int y);
	void drawLine(QPainter * p, QPoint pos1, QPoint pos2);
	void drawVertexEllipse(QPainter * p, QPoint pos, EVertex vertexPostion);

	QRect getDrawingTextBox(mip::VECTOR3 pos);
	QString getText();

public:

private:
	mip::VECTOR3 m_v1;
	mip::VECTOR3 m_v2;

	EVertex m_selectedVertexPosition;
};

class ProLine : public AnnoProfile
{
public:
	ProLine(std::vector<mip::VECTOR3> pckPoints, QMap<float, mint16> huPoints, mint16 huMin, mint16 huMax, QColor color = QColor(255, 0, 0), bool isHidden = false);
	
	void drawProfile(AnalMPRPlaneView *win);

private:
	float length;
	QMap<float, mint16> huDirs;
};
#endif


