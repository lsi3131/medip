#pragma once

#ifndef ANNOTATION_H
#define ANNOTATION_H

#include "define.h"
#include "color.h"
#include "Renderer/Slice.h"

#include <QString>
#include <QRect>
#include <QLineF>

class QPainter;
class AnalMPRPlaneView;
class AnalVolumeView;
class WindowBase;
class VolumeView;
class OpenGLWidget;
class VOLUME_DATA;
class VOLUME_DATA_PET;

class Annotation
{
public:
	enum EState
	{
		IN_DRAWING,
		DRAWING_FINISHED,
	};

	enum ESelectedType
	{
		ST_UNSELECTED = 0,
		ST_SELECTED
	};

	enum EHoverType
	{
		HT_HOVER_NONE,
		HT_HOVER_VERTEX,
		HT_HOVER_LINE,
		HT_HOVER_TEXTBOX,
	};

public:
	Annotation(COLOR color);
	~Annotation() {}

public:
	bool isAnnoHidden() { return m_isHidden; }
	void setAnnoHidden(bool val) { m_isHidden = val; }

	void drawAnno(OpenGLWidget * win, QPainter *p, ANNOTATION_DRAWING_VIEWER_TYPE type);

	COLOR getColor();
	void setColor(COLOR color);

	bool setHover_Measurement(OpenGLWidget* win, int x, int y);
	bool setSelect_Measurement(OpenGLWidget* win, int x, int y);

	bool setHover_Window(WindowBase* win, int x, int y);
	bool setSelect_Window(WindowBase* win, int x, int y);

	void setSelect();
	void clearSelect();

	bool isSelected();
public:
	virtual ANNOTATION_TYPE getType() = 0;
	virtual EState GetState() { return m_state; }

	virtual bool addDrawingPoint_Measurement(AnalMPRPlaneView* win, int x, int y) { return false; }
	virtual bool addDrawingPoint_Window(WindowBase* win, int x, int y) { return false; }

	virtual bool isContainLine_Measurement(AnalMPRPlaneView* win, int x, int y) { return false; }
	virtual bool isContainVertex_Measurement(AnalMPRPlaneView* win, int x, int y) { return false; }
	virtual bool isContainVertex_Measurement_Vol(AnalVolumeView* win, int x, int y) { return false; }
	virtual bool isContainTextBox_Measurement(AnalMPRPlaneView* win, int x, int y) { return false; }

	virtual bool isContainLine_Window(WindowBase* win, int x, int y) { return false; }
	virtual bool isContainVertex_Window(WindowBase* win, int x, int y) { return false; }
	virtual bool isContainTextBox_Window(WindowBase* win, int x, int y) { return false; }

	virtual void movePosition_Measurement(AnalMPRPlaneView* win, int x, int y) {}
	virtual void movePosition_Measurement_Vol(AnalVolumeView* win, int x, int y, mip::VECTOR3 v = mip::VECTOR3(0, 0, 0)) {}
	virtual void movePosition_Window(WindowBase* win, int x, int y) {}

	virtual bool isInMPRPlane_Measurement(AnalMPRPlaneView * win, WINDOW_TYPE type) { return false; }
	virtual bool isInMPRPlane_Window(WindowBase * win, WINDOW_TYPE type) { return false; }

	virtual void drawVolume_Measurement(AnalVolumeView * win, QPainter *p) {}
	virtual void drawMPRPlane_Measurement(AnalMPRPlaneView * win, QPainter *p) {}

	virtual void drawVolume(VolumeView * win, QPainter *p) {}
	virtual void drawMPRPlane_Window(WindowBase * win, QPainter *p) {}

	virtual void drawPoint_VolumeMeasurement(AnalMPRPlaneView * win, QPainter *p, WINDOW_TYPE type) {}
protected:
	mip::VECTOR3 getVertex_ScreenToWorld_Measurement(AnalMPRPlaneView* win, int x, int y);
	mip::VECTOR3 getVertex_WorldToScreen_Measurement(AnalMPRPlaneView * win, mip::VECTOR3 v);
	mip::VECTOR2 getVertex_WorldToScreen_Measurement_2D(AnalMPRPlaneView * win, mip::VECTOR3 v);

	mip::VECTOR3 getVertex_ScreenToWorld_Volume_Measurement(AnalVolumeView* win, int x, int y);
	mip::VECTOR3 getVertex_WorldToScreen_Volume_Measurement(AnalVolumeView * win, mip::VECTOR3 v);

	mip::VECTOR3 getVertex_ScreenToWorld_Volume(VolumeView* win, int x, int y);
	mip::VECTOR3 getVertex_WorldToScreen_Volume(VolumeView * win, mip::VECTOR3 v);

	mip::VECTOR3 getVertex_WorldToScreen_Window(WindowBase * win, mip::VECTOR3 v);
	mip::VECTOR3 getVertex_ScreenToWorld_Window(WindowBase * win, int x, int y);
	//mip::VECTOR3 getVertex_VolumeToScreen(WindowBase * win, mip::VECTOR3 v);

	virtual int  getCnt_IsInMPRPlane_Measurement(AnalMPRPlaneView *win, WINDOW_TYPE type) { return 0; }
protected:
	COLOR m_color;
	bool m_isHidden;

	EState m_state;
	ESelectedType m_selectedType;
	EHoverType m_hoverType;

	QPen m_linePen_Normal;
	QPen m_linePen_Hover;
	QPen m_linePen_Selected;

	QPen m_linePen_Hover_Border;
	QPen m_linePen_Selected_Border;

	int m_lineWidth;

	QBrush m_vertexEllipseBrush_Normal;
	QBrush m_vertexEllipseBrush_Hover;
	QBrush m_vertexEllipseBrush_Selected;

	QBrush m_vertexEllipseBrush_Hover_Border;
	QBrush m_vertexEllipseBrush_Selected_Border;
	int m_vertexRadius;
	int m_vertexBorderRadius;

	QPen m_textBoxPen_Normal;
	QPen m_textBoxPen_Hover;
	QPen m_textBoxPen_Selected;

	QBrush m_textBoxBrush_Normal;
	QBrush m_textBoxBrush_Hover;
	QBrush m_textBoxBrush_Selected;

	QPen m_textPen_Normal;
	QPen m_textPen_Hover;
	QPen m_textPen_Selected;

	mip::VECTOR3 m_curSelectedVertex_World;
};

//=================================================
//		Annotation Utility
//=================================================
bool CompFloatingPoint(double a1, double a2, double absTolerance);
bool IsLineContainDot(mip::VECTOR2 lineP1, mip::VECTOR2 lineP2, mip::VECTOR2 dotP3, float minDistance);
bool IsVertexInVolumePlane_Measurement(VOLUME_DATA* pVolume, WINDOW_TYPE type, mip::VECTOR3 v);
bool IsVertexInVolumePlane_Window(VOLUME_DATA* pVolume, WINDOW_TYPE type, uint depth, mip::VECTOR3 v);

QColor toQColor(COLOR c);
COLOR toCOLOR(QColor c);

mip::VECTOR3 getWorldToVolume(VOLUME_DATA* pVolume, float x, float y, float z);
mip::VECTOR3 getVolumeToWorld(VOLUME_DATA* pVolume, float x, float y, float z);

mip::VECTOR3 getWorldToVolume_PET(VOLUME_DATA_PET* pVolume, float x, float y, float z);
mip::VECTOR3 getVolumeToWorld_PET(VOLUME_DATA_PET* pVolume, float x, float y, float z);

std::vector<mint16> getHU_List_World(VOLUME_DATA* pVolume, mip::VECTOR3 vLT_World, mip::VECTOR3 vRT_World, mip::VECTOR3 vLB_World, mip::VECTOR3 vRB_World);

/* TODO : 추후 PET Volume에 World좌표계 도입 후 구현할 것*/
std::vector<float> getSUV_List_World(VOLUME_DATA_PET* pVolume, mip::VECTOR3 vLT_World, mip::VECTOR3 vRT_World, mip::VECTOR3 vLB_World, mip::VECTOR3 vRB_World);

std::vector<float> getSUV_List_Volume(VOLUME_DATA_PET* pVolume, WINDOW_TYPE type, mip::VECTOR3 vLT_volume, mip::VECTOR3 vRB_volume, bool flipX=false, bool flipY=false);


#endif