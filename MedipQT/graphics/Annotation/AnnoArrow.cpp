#include "stdafx.h"
#include "AnnoArrow.h"
#include "Windows/AnalMPRPlaneView.h"
#include "Windows/AnalVolumeView.h"
#include "Windows/VolumeView.h"
#include "Windows/glwidget.h"
#include "Windows/windowManager.h"
#include "mipEngine/geometry.h"
#include "mipEngine/intersect.h"
#include "DataContext.h"

AnnoArrow::AnnoArrow(mip::VECTOR3 v1, mip::VECTOR3 v2, EState state, COLOR color, bool isHidden) :
	Annotation(color)
{
	Init(v1, v2, state, isHidden);
}

AnnoArrow::AnnoArrow(AnalMPRPlaneView * win, int x, int y, COLOR color, bool isHidden) :
	Annotation(color)
{
	mip::VECTOR3 v_new = getVertex_ScreenToWorld_Measurement(win, x, y);
	Init(v_new, v_new, IN_DRAWING, isHidden);
}

AnnoArrow::AnnoArrow(const AnnoArrow& rhs) :
	Annotation(rhs.m_color)
{
	Init(rhs.m_v1, rhs.m_v2, rhs.m_state, rhs.m_isHidden);
}

AnnoArrow::~AnnoArrow()
{

}

void AnnoArrow::Init(mip::VECTOR3 v1, mip::VECTOR3 v2, EState state, bool isHidden)
{
	m_isHidden = isHidden;

	m_v1 = v1;
	m_v2 = v2;

	m_state = state;
}

ANNOTATION_TYPE AnnoArrow::getType()
{
	return AT_ARROW;
}

bool AnnoArrow::addDrawingPoint_Measurement(AnalMPRPlaneView * win, int x, int y)
{
	mip::VECTOR3 v_new = getVertex_ScreenToWorld_Measurement(win, x, y);
	if (m_state == IN_DRAWING)
	{
		m_v2 = v_new;

		m_state = DRAWING_FINISHED;
	}
	return true;
}

void AnnoArrow::movePosition_Measurement(AnalMPRPlaneView * win, int x, int y)
{
	mip::VECTOR3 v_new = getVertex_ScreenToWorld_Measurement(win, x, y);
	if (m_state == IN_DRAWING)
	{
		m_v2 = v_new;
	}
	else
	{
		if (m_selectedType == ST_SELECTED)
		{
			if (m_hoverType == HT_HOVER_LINE)
			{
				mip::VECTOR3 delta = v_new - m_curSelectedVertex_World;
				m_v1 += delta;
				m_v2 += delta;

				/* 이동 후 Selected Pos 초기화 */
				m_curSelectedVertex_World = v_new;
			}
			else if (m_hoverType == HT_HOVER_VERTEX)
			{
				if (m_selectedVertexPosition == VERTEX_1st)
				{
					m_v1 = v_new;
				}
				else if (m_selectedVertexPosition == VERTEX_2nd)
				{
					m_v2 = v_new;
				}
			}
		}
	}
}

bool AnnoArrow::isInMPRPlane_Measurement(AnalMPRPlaneView *win, WINDOW_TYPE type)
{
	if (IsVertexInVolumePlane_Measurement(&DATA_CONTEXT->volume_data, type, m_v1) == false)
	{
		return false;
	}

	if (IsVertexInVolumePlane_Measurement(&DATA_CONTEXT->volume_data, type, m_v2) == false)
	{
		return false;
	}

	return true;
}

void AnnoArrow::drawVolume_Measurement(AnalVolumeView * win, QPainter * p)
{
	mip::VECTOR3 v1_screen = getVertex_WorldToScreen_Volume_Measurement(win, m_v1);
	mip::VECTOR3 v2_screen = getVertex_WorldToScreen_Volume_Measurement(win, m_v2);

	draw(p, v1_screen, v2_screen);
}

void AnnoArrow::drawMPRPlane_Measurement(AnalMPRPlaneView * win, QPainter * p)
{
	mip::VECTOR3 v1_screen = getVertex_WorldToScreen_Measurement(win, m_v1);
	mip::VECTOR3 v2_screen = getVertex_WorldToScreen_Measurement(win, m_v2);

	draw(p, v1_screen, v2_screen);
}

void AnnoArrow::drawVolume(VolumeView * win, QPainter * p)
{
	mip::VECTOR3 v1_screen = getVertex_WorldToScreen_Volume(win, m_v1);
	mip::VECTOR3 v2_screen = getVertex_WorldToScreen_Volume(win, m_v2);

	draw(p, v1_screen, v2_screen);
}

bool AnnoArrow::isContainLine_Measurement(AnalMPRPlaneView* win, int x, int y)
{
	const float minDistance = 5.f;

	mip::VECTOR3 v1 = getVertex_WorldToScreen_Measurement(win, m_v1);
	mip::VECTOR3 v2 = getVertex_WorldToScreen_Measurement(win, m_v2);
	mip::VECTOR3 dot = mip::VECTOR3(x, y, 0);

	return IsLineContainDot(
		mip::VECTOR2(v1.x, v1.y),
		mip::VECTOR2(v2.x, v2.y),
		mip::VECTOR2(dot.x, dot.y),
		minDistance);
}

bool AnnoArrow::isContainVertex_Measurement(AnalMPRPlaneView* win, int x, int y)
{
	return isContainVertex_Measurement(&m_selectedVertexPosition, win, x, y);
}

bool AnnoArrow::isContainVertex_Measurement(EVertex* pOutVertex, AnalMPRPlaneView* win, int x, int y)
{
	int imgWidth = win->width();
	int imgHeight = win->height();

	mip::VECTOR3 ptTemp;
	mip::VECTOR2 pt1st;
	mip::VECTOR2 pt2nd;
	mip::MATRIX44 matView = win->getCamera().getView();
	mip::MATRIX44 matProj = win->getCamera().getProj();

	/* 정점값 초기화 */
	ptTemp = mip::geom::WorldToScreen(m_v1, imgWidth, imgHeight, matView, matProj);
	pt1st.set(ptTemp.x, ptTemp.y);

	ptTemp = mip::geom::WorldToScreen(m_v2, imgWidth, imgHeight, matView, matProj);
	pt2nd.set(ptTemp.x, ptTemp.y);

	if ((pt1st - mip::VECTOR2(x, y)).length() <= 5.0f) //v1
	{
		*pOutVertex = VERTEX_1st;
		return true;
	}
	else if ((pt2nd - mip::VECTOR2(x, y)).length() <= 5.0f) //v2
	{
		*pOutVertex = VERTEX_2nd;
		return true;
	}
	return false;
}

void AnnoArrow::draw(QPainter * p, mip::VECTOR3 v1_screen, mip::VECTOR3 v2_screen)
{
	drawLine(p,
		QPoint(v1_screen.x, v1_screen.y),
		QPoint(v2_screen.x, v2_screen.y)
	);

	drawVertexEllipse(p, QPoint(v1_screen.x, v1_screen.y), VERTEX_1st);
	drawVertexEllipse(p, QPoint(v2_screen.x, v2_screen.y), VERTEX_2nd);
}

void AnnoArrow::drawLine(QPainter * p, QPoint pos1, QPoint pos2)
{
	p->setBrush(QBrush());

	/* 직선 */
	QLineF line = QLineF(pos1, pos2);

	/* 화살표 */
	QLineF lineArrow1 = QLineF(pos2, pos1);
	QLineF lineArrow2 = QLineF(pos2, pos1);

	float angle = line.angle();
	lineArrow1.setAngle(angle + 120);
	lineArrow1.setLength(8);

	lineArrow2.setAngle(angle - 120);
	lineArrow2.setLength(8);

	if (m_hoverType == HT_HOVER_LINE ||
		m_hoverType == HT_HOVER_TEXTBOX)
	{
		p->setPen(m_linePen_Hover_Border);
		p->drawLine(line);
		p->drawLine(lineArrow1);
		p->drawLine(lineArrow2);
	}
	else if (m_selectedType == ST_SELECTED)
	{
		p->setPen(m_linePen_Selected_Border);
		p->drawLine(line);
		p->drawLine(lineArrow1);
		p->drawLine(lineArrow2);
	}

	p->setPen(m_linePen_Normal);
	p->drawLine(line);
	p->drawLine(lineArrow1);
	p->drawLine(lineArrow2);

	p->setBrush(QBrush());
}

void AnnoArrow::drawVertexEllipse(QPainter * p, QPoint pos, EVertex vertexPostion)
{
	p->setPen(QPen());

	if (m_selectedVertexPosition == vertexPostion)
	{
		switch (m_hoverType)
		{
		case HT_HOVER_VERTEX:
			p->setBrush(m_vertexEllipseBrush_Hover);
			break;
		default:
			p->setBrush(m_vertexEllipseBrush_Normal);
			break;
		}
	}
	else
	{
		p->setBrush(m_vertexEllipseBrush_Normal);
	}

	p->drawEllipse(pos, m_vertexRadius, m_vertexRadius);
}
