#include "stdafx.h"
#include "AnnoAngle.h"

#include "Windows/windowManager.h"
#include "Windows/AnalMPRPlaneView.h"
#include "Windows/AnalVolumeView.h"
#include "Windows/VolumeView.h"
#include "Windows/glwidget.h"
#include "mipEngine/geometry.h"
#include "mipEngine/intersect.h"
#include "DataContext.h"

AnnoAngle::AnnoAngle(mip::VECTOR3 v1, mip::VECTOR3 v2, mip::VECTOR3 v3, EState state, COLOR color, bool isHidden) :
	Annotation(color)
{
	Init(v1, v2, v3, state, isHidden);
}

AnnoAngle::AnnoAngle(AnalMPRPlaneView * win, int x, int y, COLOR color, bool isHidden) :
	Annotation(color)
{
	mip::VECTOR3 new_v = getVertex_ScreenToWorld_Measurement(win, x, y);
	Init(new_v, new_v, mip::VECTOR3::Zero, IN_DRAWING,  isHidden);
}

AnnoAngle::AnnoAngle(const AnnoAngle& rhs) :
	Annotation(rhs.m_color)
{
	Init(rhs.m_v1, rhs.m_v2, rhs.m_v3, rhs.m_state, rhs.m_isHidden);
}


AnnoAngle::~AnnoAngle()
{
}

void AnnoAngle::Init(mip::VECTOR3 v1, mip::VECTOR3 v2, mip::VECTOR3 v3, EState state, bool isHidden)
{
	m_pVText = nullptr;
	m_v1 = v1;
	m_v2 = v2;
	m_v3 = v3;
	m_isHidden = isHidden;
	m_state = state;
}

bool AnnoAngle::isContainLine_Measurement(AnalMPRPlaneView * win, int x, int y)
{
	const float minDistance = 5.f;

	mip::VECTOR3 v1 = getVertex_WorldToScreen_Measurement(win, m_v1);
	mip::VECTOR3 v2 = getVertex_WorldToScreen_Measurement(win, m_v2);
	mip::VECTOR3 v3 = getVertex_WorldToScreen_Measurement(win, m_v3);
	mip::VECTOR3 dot = mip::VECTOR3(x, y, 0);

	bool isContain;
	//Line 1->2
	isContain = IsLineContainDot(
		mip::VECTOR2(v1.x, v1.y),
		mip::VECTOR2(v2.x, v2.y),
		mip::VECTOR2(dot.x, dot.y),
		minDistance);
	if (isContain)
	{
		return true;
	}

	//Line 2->3
	isContain = IsLineContainDot(
		mip::VECTOR2(v2.x, v2.y),
		mip::VECTOR2(v3.x, v3.y),
		mip::VECTOR2(dot.x, dot.y),
		minDistance);
	if (isContain)
	{
		return true;
	}

	return false;
}

bool AnnoAngle::isContainVertex_Measurement(AnalMPRPlaneView * win, int x, int y)
{
	return getContainVertex(&m_selectedVertexPosition, win, x, y);
}

bool AnnoAngle::isContainTextBox_Measurement(AnalMPRPlaneView * win, int x, int y)
{
	mip::VECTOR3 vText_screen = getVertex_WorldToScreen_Measurement(win, getTextVertex(win));
	QRect rect = getDrawingTextBox(vText_screen);
	return rect.contains(x, y);
}

bool AnnoAngle::getContainVertex(EVertex * pOutVertex, AnalMPRPlaneView * win, int x, int y)
{
	AnalMPRPlaneView* win2 = (AnalMPRPlaneView*)win;
	mip::VECTOR3 ptTemp;
	mip::VECTOR2 pt1st;
	mip::VECTOR2 pt2nd;
	mip::VECTOR2 pt3rd;

	int imgWidth = win2->width();
	int imgHeight = win2->height();

	mip::MATRIX44 matView = win2->getCamera().getView();
	mip::MATRIX44 matProj = win2->getCamera().getProj();

	ptTemp = mip::geom::WorldToScreen(m_v1, imgWidth, imgHeight, matView, matProj);
	pt1st.set(ptTemp.x, ptTemp.y);

	ptTemp = mip::geom::WorldToScreen(m_v2, imgWidth, imgHeight, matView, matProj);
	pt2nd.set(ptTemp.x, ptTemp.y);

	ptTemp = mip::geom::WorldToScreen(m_v3, imgWidth, imgHeight, matView, matProj);
	pt3rd.set(ptTemp.x, ptTemp.y);

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
	else if ((pt3rd - mip::VECTOR2(x, y)).length() <= 5.0f) //v3
	{
		*pOutVertex = VERTEX_3rd;
		return true;
	}

	return false;
}

void AnnoAngle::draw(QPainter * p, mip::VECTOR3 v1_screen, mip::VECTOR3 v2_screen, mip::VECTOR3 v3_screen, mip::VECTOR3 vText_screen)
{
	//Draw Line 1 -> 2
	drawLine(p,
		QPoint(v1_screen.x, v1_screen.y),
		QPoint(v2_screen.x, v2_screen.y)
	);

	//Draw Line 2->3
	if (m_v3.isNearlyZero() == false)
	{
		drawLine(p,
			QPoint(v2_screen.x, v2_screen.y),
			QPoint(v3_screen.x, v3_screen.y)
		);
	}

	//Draw Vertex 1 -> 2
	drawVertexEllipse(p, QPoint(v1_screen.x, v1_screen.y), VERTEX_1st);
	drawVertexEllipse(p, QPoint(v2_screen.x, v2_screen.y), VERTEX_2nd);

	//Draw Vertex 2 -> 3
	if (m_v3.isNearlyZero() == false)
	{
		drawVertexEllipse(p, QPoint(v3_screen.x, v3_screen.y), VERTEX_3rd);
	}

	if (m_v3.isNearlyZero() == false)
	{
		drawTextBox(p, vText_screen);
	}
}

void AnnoAngle::drawLine(QPainter * p, QPoint pos1, QPoint pos2)
{
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

void AnnoAngle::drawVertexEllipse(QPainter * p, QPoint pos, EVertex vertexPostion)
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

void AnnoAngle::drawTextBox(QPainter * p, mip::VECTOR3 pos)
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

QRect AnnoAngle::getDrawingTextBox(mip::VECTOR3 pos)
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

QString AnnoAngle::getText()
{
	QString text;
	QString angleUnit = QChar(0x00B0);
	return text.sprintf("Angle : %.2f", getAngle()) + angleUnit;
}

float AnnoAngle::getAngle()
{
	mip::VECTOR3 l1 = m_v2 - m_v1;
	mip::VECTOR3 l2 = m_v2 - m_v3;

	l1 = l1.normalize();
	l2 = l2.normalize();

	float fdot = l1.dot(l2);
	float fradian = acos(fdot);

	return 180.0f * fradian / 3.141592f;
}

ANNOTATION_TYPE AnnoAngle::getType()
{
	return AT_ANGLE;
}

bool AnnoAngle::addDrawingPoint_Measurement(AnalMPRPlaneView* win, int x, int y)
{
	mip::VECTOR3 v_new = getVertex_ScreenToWorld_Measurement(win, x, y);
	if (m_state == IN_DRAWING)
	{
		if (m_v3.isNearlyZero() == false)
		{
			m_state = DRAWING_FINISHED;
		}
		m_v3 = v_new;
	}
	return true;
}

void AnnoAngle::movePosition_Measurement(AnalMPRPlaneView* win, int x, int y)
{
	mip::VECTOR3 v_new = getVertex_ScreenToWorld_Measurement(win, x, y);
	if (m_state == IN_DRAWING)
	{
		if (m_v3.isNearlyZero())
		{
			m_v2 = v_new;
		}
		else
		{
			m_v3 = v_new;
		}
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
				m_v3 += delta;

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
				else if (m_selectedVertexPosition == VERTEX_3rd)
				{
					m_v3 = v_new;
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

bool AnnoAngle::isInMPRPlane_Measurement(AnalMPRPlaneView *win, WINDOW_TYPE type)
{
	if (IsVertexInVolumePlane_Measurement(&DATA_CONTEXT->volume_data, type, m_v1) == false)
	{
		return false;
	}

	if (IsVertexInVolumePlane_Measurement(&DATA_CONTEXT->volume_data, type, m_v2) == false)
	{
		return false;
	}

	if (IsVertexInVolumePlane_Measurement(&DATA_CONTEXT->volume_data, type, m_v3) == false)
	{
		return false;
	}

	return true;
}

void AnnoAngle::drawVolume_Measurement(AnalVolumeView * win, QPainter * p)
{
	mip::VECTOR3 v1_screen = getVertex_WorldToScreen_Volume_Measurement(win, m_v1);
	mip::VECTOR3 v2_screen = getVertex_WorldToScreen_Volume_Measurement(win, m_v2);
	mip::VECTOR3 v3_screen = getVertex_WorldToScreen_Volume_Measurement(win, m_v3);
	mip::VECTOR3 vText_screen = getVertex_WorldToScreen_Volume_Measurement(win, getTextVertex_Volume_Measurement(win));

	draw(p, v1_screen, v2_screen, v3_screen, vText_screen);
}

void AnnoAngle::drawMPRPlane_Measurement(AnalMPRPlaneView * win, QPainter * p)
{
	mip::VECTOR3 v1_screen = getVertex_WorldToScreen_Measurement(win, m_v1);
	mip::VECTOR3 v2_screen = getVertex_WorldToScreen_Measurement(win, m_v2);
	mip::VECTOR3 v3_screen = getVertex_WorldToScreen_Measurement(win, m_v3);
	mip::VECTOR3 vText_screen = getVertex_WorldToScreen_Measurement(win, getTextVertex(win));

	draw(p, v1_screen, v2_screen, v3_screen, vText_screen);
}

void AnnoAngle::drawVolume(VolumeView * win, QPainter * p)
{
	mip::VECTOR3 v1_screen = getVertex_WorldToScreen_Volume(win, m_v1);
	mip::VECTOR3 v2_screen = getVertex_WorldToScreen_Volume(win, m_v2);
	mip::VECTOR3 v3_screen = getVertex_WorldToScreen_Volume(win, m_v3);
	mip::VECTOR3 vText_screen = getVertex_WorldToScreen_Volume(win, getTextVertex_Volume(win));

	draw(p, v1_screen, v2_screen, v3_screen, vText_screen);
}

mip::VECTOR3 AnnoAngle::getTextVertex(AnalMPRPlaneView * win)
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

mip::VECTOR3 AnnoAngle::getTextVertex_Volume(VolumeView * win)
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

mip::VECTOR3 AnnoAngle::getTextVertex_Volume_Measurement(AnalVolumeView * win)
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

