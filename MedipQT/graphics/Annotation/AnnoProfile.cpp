#include "stdafx.h"
#include "AnnoProfile.h"
#include "Windows/windowManager.h"
#include "Windows/AnalMPRPlaneView.h"
#include "Windows/AnalVolumeView.h"
#include "Windows/VolumeView.h"
#include "Windows/glwidget.h"
#include "mipEngine/geometry.h"
#include "mipEngine/intersect.h"
#include "DataContext.h"

AnnoProfile::AnnoProfile(mip::VECTOR3 v1, mip::VECTOR3 v2, EState state, COLOR color, bool isHidden) :
	Annotation(color)
{
	Init(v1, v2, state, isHidden);
}

AnnoProfile::AnnoProfile(AnalMPRPlaneView * win, int x, int y, COLOR color, bool isHidden) :
	Annotation(color)
{
	mip::VECTOR3 v_new = getVertex_ScreenToWorld_Measurement(win, x, y);
	Init(v_new, v_new, IN_DRAWING, isHidden);
}

AnnoProfile::~AnnoProfile()
{

}

void AnnoProfile::Init(mip::VECTOR3 v1, mip::VECTOR3 v2, EState state, bool isHidden)
{
	m_isHidden = isHidden;

	m_v1 = v1;
	m_v2 = v2;

	m_state = state;
}

float AnnoProfile::getLength()
{
	return (m_v1 - m_v2).length();
}

ANNOTATION_TYPE AnnoProfile::getType()
{
	return AT_PT_LINE;
}

bool AnnoProfile::addDrawingPoint_Measurement(AnalMPRPlaneView * win, int x, int y)
{
	mip::VECTOR3 v_new = getVertex_ScreenToWorld_Measurement(win, x, y);
	if (m_state == IN_DRAWING)
	{
		m_v2 = v_new;

		m_state = DRAWING_FINISHED;
	}
	return true;
}

void AnnoProfile::movePosition_Measurement(AnalMPRPlaneView * win, int x, int y)
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
			else if (m_hoverType == HT_HOVER_TEXTBOX)
			{
				mip::VECTOR3 delta = v_new - m_curSelectedVertex_World;

				/* 이동 후 Selected Pos 초기화 */
				m_curSelectedVertex_World = v_new;
			}
		}
	}
}

bool AnnoProfile::isInMPRPlane_Measurement(AnalMPRPlaneView *win, WINDOW_TYPE type)
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

void AnnoProfile::drawVolume_Measurement(AnalVolumeView * win, QPainter * p)
{
}

void AnnoProfile::drawMPRPlane_Measurement(AnalMPRPlaneView * win, QPainter * p)
{
	mip::VECTOR3 v1_screen = getVertex_WorldToScreen_Measurement(win, m_v1);
	mip::VECTOR3 v2_screen = getVertex_WorldToScreen_Measurement(win, m_v2);

	drawLine(p,
		QPoint(v1_screen.x, v1_screen.y),
		QPoint(v2_screen.x, v2_screen.y)
	);

	drawVertexEllipse(p, QPoint(v1_screen.x, v1_screen.y), VERTEX_1st);
	drawVertexEllipse(p, QPoint(v2_screen.x, v2_screen.y), VERTEX_2nd);
}

void AnnoProfile::drawVolume(VolumeView * win, QPainter * p)
{
}

bool AnnoProfile::isContainLine_Measurement(AnalMPRPlaneView* win, int x, int y)
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

bool AnnoProfile::isContainVertex_Measurement(AnalMPRPlaneView* win, int x, int y)
{
	return isContainVertex_Measurement(&m_selectedVertexPosition, win, x, y);
}

bool AnnoProfile::isContainVertex_Measurement(EVertex* pOutVertex, AnalMPRPlaneView* win, int x, int y)
{
	AnalMPRPlaneView* win2 = (AnalMPRPlaneView*)win;
	int imgWidth = win2->width();
	int imgHeight = win2->height();

	mip::VECTOR3 ptTemp;
	mip::VECTOR2 pt1st;
	mip::VECTOR2 pt2nd;
	mip::MATRIX44 matView = win2->getCamera().getView();
	mip::MATRIX44 matProj = win2->getCamera().getProj();

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

void AnnoProfile::drawLine(QPainter * p, QPoint pos1, QPoint pos2)
{
	p->setBrush(QBrush());

	if (m_hoverType == HT_HOVER_LINE ||
		m_hoverType == HT_HOVER_TEXTBOX)
	{
		p->setPen(m_linePen_Hover);
	}
	else if (m_selectedType == ST_SELECTED)
	{
		p->setPen(m_linePen_Selected);
	}
	else
	{
		p->setPen(m_linePen_Normal);
	}

	p->drawLine(QLine(pos1, pos2));
}

void AnnoProfile::drawVertexEllipse(QPainter * p, QPoint pos, EVertex vertexPostion)
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

QRect AnnoProfile::getDrawingTextBox(mip::VECTOR3 pos)
{
	QFont font;
	QFontMetrics fontMet(font);
	QRect fontR = fontMet.boundingRect(getText());
	fontR.setWidth(fontR.width() + 2);
	fontR.setHeight(fontR.height() + 2);

	return QRect(
		pos.x,
		pos.y - fontR.height(),
		fontR.width() + 2, fontR.height() + 2
	);
}

QString AnnoProfile::getText()
{
	return QString("Length : %1 mm").arg(getLength() * 10);
}

/* Proline 체크 */
//ProLine::ProLine(std::vector<mip::VECTOR3> pckPoints, QMap<float, mint16> huPoints, mint16 huMin, mint16 huMax, QColor color, bool isHidden)
//{
//	length = (pckPoints.at(0) - pckPoints.at(pckPoints.size() - 1)).length();
//
//	pickPoints = pckPoints;
//	this->huDirs = huPoints;
//
//	this->HuMax = huMax;
//	this->HuMin = huMin;
//
//	for (auto val = huPoints.begin(); val != huPoints.end(); val++)
//		this->huPoints.push_back((*val));
//
//	HuAvg = std::accumulate(this->huPoints.begin(), this->huPoints.end(), 0.0) / this->huPoints.size();
//
//	this->m_color = COLOR(color.red(), color.green(), color.blue());
//	this->m_isHidden = isHidden;
//
//
//}
//
//void ProLine::drawProfile(AnalMPRPlaneView * win)
//{
//	if (NULL == win) return;
//
//	LineProfileDialog dlg(huDirs, HuMin, HuMax, HuAvg, length, win);
//
//	dlg.exec();
//}
//
