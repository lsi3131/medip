#include "stdafx.h"
#include "AnnoLength.h"
#include "Windows/AnalMPRPlaneView.h"
#include "Windows/AnalVolumeView.h"
#include "Windows/VolumeView.h"
#include "Windows/glwidget.h"
#include "Windows/windowManager.h"
#include "mipEngine/geometry.h"
#include "mipEngine/intersect.h"
#include "DataContext.h"

AnnoLength::AnnoLength(mip::VECTOR3 v1, mip::VECTOR3 v2, EState state, COLOR color, bool isHidden) :
	Annotation(color)
{
	Init(v1, v2, state, isHidden);
}

AnnoLength::AnnoLength(AnalMPRPlaneView * win, int x, int y, COLOR color, bool isHidden) :
	Annotation(color)
{
	mip::VECTOR3 v_new = getVertex_ScreenToWorld_Measurement(win, x, y);
	Init(v_new, v_new, IN_DRAWING, isHidden);
}

AnnoLength::AnnoLength(const AnnoLength& rhs) :
	Annotation(rhs.m_color)
{
	Init(rhs.m_v1, rhs.m_v2, rhs.m_state, rhs.m_isHidden);
}

AnnoLength::~AnnoLength()
{

}

void AnnoLength::Init(mip::VECTOR3 v1, mip::VECTOR3 v2, EState state, bool isHidden)
{
	m_isHidden = isHidden;

	m_v1 = v1;
	m_v2 = v2;

	m_state = state;
}

float AnnoLength::getLength()
{
	return (m_v1 - m_v2).length();
}

ANNOTATION_TYPE AnnoLength::getType()
{
	return AT_LEN;
}

bool AnnoLength::addDrawingPoint_Measurement(AnalMPRPlaneView * win, int x, int y)
{
	mip::VECTOR3 v_new = getVertex_ScreenToWorld_Measurement(win, x, y);
	if (m_state == IN_DRAWING)
	{
		m_v2 = v_new;

		m_state = DRAWING_FINISHED;
	}
	return true;
}

void AnnoLength::movePosition_Measurement(AnalMPRPlaneView * win, int x, int y)
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
				/* Text 좌표 초기화 */
				if (m_pVText == nullptr)
				{
					mip::VECTOR3 vText = getTextVertex(win);
					m_pVText = std::make_unique<mip::VECTOR3>(vText);
				}
				*m_pVText += delta;

				/* 이동 후 Selected Pos 초기화 */
				m_curSelectedVertex_World = v_new;
			}
		}
	}
}


void AnnoLength::movePosition_Measurement_Vol(AnalVolumeView * win, int x, int y, mip::VECTOR3 v)
{
#if 1
	mip::VECTOR3 v_new;
	if (v != mip::VECTOR3(0,0,0))
	{
		v_new = v;
	}
	else
	{
		//v_new = getVertex_ScreenToWorld_Measurement(win, x, y);
	}
	
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
				/* Text 좌표 초기화 */
				if (m_pVText == nullptr)
				{
					//mip::VECTOR3 vText = getTextVertex(win);
					mip::VECTOR3 vText = v_new;
					m_pVText = std::make_unique<mip::VECTOR3>(vText);
				}
				*m_pVText += delta;

				/* 이동 후 Selected Pos 초기화 */
				m_curSelectedVertex_World = v_new;
			}
		}
	}
#endif
}

bool AnnoLength::isInMPRPlane_Measurement(AnalMPRPlaneView *win, WINDOW_TYPE type)
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

void AnnoLength::drawVolume_Measurement(AnalVolumeView * win, QPainter * p)
{
	mip::VECTOR3 v1_screen = getVertex_WorldToScreen_Volume_Measurement(win, m_v1);
	mip::VECTOR3 v2_screen = getVertex_WorldToScreen_Volume_Measurement(win, m_v2);
	mip::VECTOR3 vText_screen = getVertex_WorldToScreen_Volume_Measurement(win, getTextVertex_Volume_Measurement(win));

	draw(p, v1_screen, v2_screen, vText_screen);
}

void AnnoLength::drawMPRPlane_Measurement(AnalMPRPlaneView * win, QPainter * p)
{
	mip::VECTOR3 v1_screen = getVertex_WorldToScreen_Measurement(win, m_v1);
	mip::VECTOR3 v2_screen = getVertex_WorldToScreen_Measurement(win, m_v2);
	mip::VECTOR3 vText_screen = getVertex_WorldToScreen_Measurement(win, getTextVertex(win));

	draw(p, v1_screen, v2_screen, vText_screen);
}

void AnnoLength::drawVolume(VolumeView * win, QPainter * p)
{
	mip::VECTOR3 v1_screen = getVertex_WorldToScreen_Volume(win, m_v1);
	mip::VECTOR3 v2_screen = getVertex_WorldToScreen_Volume(win, m_v2);
	mip::VECTOR3 vText_screen = getVertex_WorldToScreen_Volume(win, getTextVertex_Volume(win));

	draw(p, v1_screen, v2_screen, vText_screen);
}

void AnnoLength::drawPoint_VolumeMeasurement(AnalMPRPlaneView * win, QPainter * p, WINDOW_TYPE type)
{
	mip::VECTOR3 v1_screen = getVertex_WorldToScreen_Measurement(win, m_v1);
	mip::VECTOR3 v2_screen = getVertex_WorldToScreen_Measurement(win, m_v2);
	mip::VECTOR3 vText_screen = getVertex_WorldToScreen_Measurement(win, getTextVertex(win));	

	if (IsVertexInVolumePlane_Measurement(&DATA_CONTEXT->volume_data, type, m_v1) == true)
	{
		drawPoint(p, v1_screen, VERTEX_1st);
	}

	if (IsVertexInVolumePlane_Measurement(&DATA_CONTEXT->volume_data, type, m_v2) == true)
	{
		drawPoint(p, v2_screen, VERTEX_2nd);
	}
}

bool AnnoLength::isContainLine_Measurement(AnalMPRPlaneView* win, int x, int y)
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

bool AnnoLength::isContainVertex_Measurement(AnalMPRPlaneView* win, int x, int y)
{
	return isContainVertex_Measurement(&m_selectedVertexPosition, win, x, y);
}

bool AnnoLength::isContainVertex_Measurement_Vol(AnalVolumeView* win, int x, int y)
{
	return isContainVertex_Measurement_Vol(&m_selectedVertexPosition, win, x, y);
}

bool AnnoLength::isContainTextBox_Measurement(AnalMPRPlaneView * win, int x, int y)
{
	mip::VECTOR3 vText_screen = getVertex_WorldToScreen_Measurement(win, getTextVertex(win));
	QRect rect = getDrawingTextBox(vText_screen);
	return rect.contains(x, y);
}

bool AnnoLength::isContainVertex_Measurement(EVertex* pOutVertex, AnalMPRPlaneView* win, int x, int y)
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


bool AnnoLength::isContainVertex_Measurement_Vol(EVertex* pOutVertex, AnalVolumeView* win, int x, int y)
{
	AnalVolumeView* win2 = (AnalVolumeView*)win;
	int imgWidth = win2->width();
	int imgHeight = win2->height();

	mip::VECTOR3 ptTemp;
	mip::VECTOR2 pt1st;
	mip::VECTOR2 pt2nd;
	mip::MATRIX44 matView = win2->getCamera().getView();
	mip::MATRIX44 matProj = win2->getCamera().getProj();
	mip::MATRIX44 matWorld;
	mip::TRANSFORM World = win2->getWorld();
	matWorld = World.getMatrix();


	/* 정점값 초기화 */
	ptTemp = mip::geom::WorldToScreen(m_v1, imgWidth, imgHeight, matView, matProj, &matWorld);
	pt1st.set(ptTemp.x, ptTemp.y);

	ptTemp = mip::geom::WorldToScreen(m_v2, imgWidth, imgHeight, matView, matProj, &matWorld);
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

void AnnoLength::draw(QPainter * p, mip::VECTOR3 v1_screen, mip::VECTOR3 v2_screen, mip::VECTOR3 vText_screen)
{
	drawLine(p,
		QPoint(v1_screen.x, v1_screen.y),
		QPoint(v2_screen.x, v2_screen.y)
	);

	drawVertexEllipse(p, QPoint(v1_screen.x, v1_screen.y), VERTEX_1st);
	drawVertexEllipse(p, QPoint(v2_screen.x, v2_screen.y), VERTEX_2nd);

	drawTextBox(p, vText_screen);
}

void AnnoLength::drawLine(QPainter * p, QPoint pos1, QPoint pos2)
{
	p->setBrush(QBrush());

	if (m_hoverType == HT_HOVER_LINE ||
		m_hoverType == HT_HOVER_TEXTBOX)
	{
		p->setPen(m_linePen_Hover_Border);
		p->drawLine(QLine(pos1, pos2));
	}
	else if (m_selectedType == ST_SELECTED)
	{
		p->setPen(m_linePen_Selected_Border);
		p->drawLine(QLine(pos1, pos2));
	}

	p->setPen(m_linePen_Normal);
	p->drawLine(QLine(pos1, pos2));
}

void AnnoLength::drawVertexEllipse(QPainter * p, QPoint pos, EVertex vertexPostion)
{
	p->setPen(QPen());

	if (m_selectedVertexPosition == vertexPostion)
	{
		switch (m_hoverType)
		{
		case HT_HOVER_VERTEX:
			p->setBrush(m_vertexEllipseBrush_Hover_Border);
			p->drawEllipse(pos, m_vertexBorderRadius, m_vertexBorderRadius);
			break;
		}
	}

	p->setBrush(m_vertexEllipseBrush_Normal);
	p->drawEllipse(pos, m_vertexRadius, m_vertexRadius);
}

void AnnoLength::drawTextBox(QPainter * p, mip::VECTOR3 pos)
{
	/* TextBox Drawing */
	QRect textBox = getDrawingTextBox(pos);
	p->setPen(QPen());
	switch (m_hoverType)
	{
	case HT_HOVER_VERTEX:
	case HT_HOVER_LINE:
	case HT_HOVER_TEXTBOX:
		p->setBrush(m_textBoxBrush_Hover);
		break;
	default:
		p->setBrush(m_textBoxBrush_Normal);
		break;
	}
	p->drawRect(textBox);

	/* Text Drawing */
	QString text = getText();
	QPoint textPos(pos.x, pos.y);

	p->setPen(m_textBoxPen_Normal);
	p->drawText(textPos, text);
}

void AnnoLength::drawPoint(QPainter * p, mip::VECTOR3 v_screen, EVertex vertexPostion)
{
	drawVertexEllipse(p, QPoint(v_screen.x, v_screen.y), vertexPostion);
}

QRect AnnoLength::getDrawingTextBox(mip::VECTOR3 pos)
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

QString AnnoLength::getText()
{
	QString text;
	return text.sprintf("Length : %.2f mm", getLength() * 10);
}

mip::VECTOR3 AnnoLength::getTextVertex(AnalMPRPlaneView * win)
{
	if (m_pVText == nullptr)
	{
		mip::VECTOR3 vGap = getVertex_ScreenToWorld_Measurement(win, 5, 0) - getVertex_ScreenToWorld_Measurement(win, 0, 0);
		return m_v2 + vGap;
	}
	else
	{
		return *m_pVText;
	}
}

mip::VECTOR3 AnnoLength::getTextVertex_Volume(VolumeView * win)
{
	if (m_pVText == nullptr)
	{
		mip::VECTOR3 vGap = getVertex_ScreenToWorld_Volume(win, 5, 0) - getVertex_ScreenToWorld_Volume(win, 0, 0);
		return m_v2 + vGap;
	}
	else
	{
		return *m_pVText;
	}
}

mip::VECTOR3 AnnoLength::getTextVertex_Volume_Measurement(AnalVolumeView * win)
{
	if (m_pVText == nullptr)
	{
		mip::VECTOR3 vGap = getVertex_ScreenToWorld_Volume_Measurement(win, 5, 0) - getVertex_ScreenToWorld_Volume_Measurement(win, 0, 0);
		return m_v2 + vGap;
	}
	else
	{
		return *m_pVText;
	}
}

int AnnoLength::getCnt_IsInMPRPlane_Measurement(AnalMPRPlaneView * win, WINDOW_TYPE type)
{
	int cntInPlane = 0;
	if (IsVertexInVolumePlane_Measurement(&DATA_CONTEXT->volume_data, type, m_v1) == true)
	{
		cntInPlane++;
	}

	if (IsVertexInVolumePlane_Measurement(&DATA_CONTEXT->volume_data, type, m_v2) == true)
	{
		cntInPlane++;
	}

	return cntInPlane;
}

