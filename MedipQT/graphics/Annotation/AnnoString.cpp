#include "stdafx.h"
#include "AnnoString.h"
#include "Windows/AnalMPRPlaneView.h"
#include "Windows/AnalVolumeView.h"
#include "Windows/VolumeView.h"
#include "Windows/glwidget.h"
#include "Windows/windowManager.h"
#include "mipEngine/geometry.h"
#include "mipEngine/intersect.h"
#include "DataContext.h"

AnnoString::AnnoString(mip::VECTOR3 v_world, EState state, QString text, muint16 fontSize, COLOR color, muint16 texttype, bool isHidden) :
	Annotation(color)
{
	Init(v_world, state, text, fontSize, texttype, isHidden);
}

AnnoString::AnnoString(AnalMPRPlaneView * win, int x, int y, QString text, muint16 fontSize, COLOR color, muint16 texttype, bool isHidden) :
	Annotation(color)
{
	mip::VECTOR3 v_new = getVertex_ScreenToWorld_Measurement(win, x, y);
	Init(v_new, DRAWING_FINISHED, text, fontSize, texttype, isHidden);
}

AnnoString::AnnoString(const AnnoString& rhs) :
	Annotation(rhs.m_color)
{
	Init(rhs.m_v_world, rhs.m_state, rhs.m_text, rhs.m_fontSize, rhs.m_textType, rhs.m_isHidden);
}

AnnoString::~AnnoString()
{

}

void AnnoString::Init(mip::VECTOR3 v, EState state, QString text, muint16 fontSize, muint16 textType, bool isHidden)
{
	m_v_world = v;
	m_state = state;

	m_isHidden = isHidden;
	m_text = text;
	m_fontSize = fontSize;

	m_textType = textType;
}

ANNOTATION_TYPE AnnoString::getType()
{
	return AT_TEXT;
}

bool AnnoString::addDrawingPoint_Measurement(AnalMPRPlaneView * win, int x, int y)
{
	m_state = DRAWING_FINISHED;
	return true;
}

void AnnoString::movePosition_Measurement(AnalMPRPlaneView * win, int x, int y)
{
	mip::VECTOR3 v_new = getVertex_ScreenToWorld_Measurement(win, x, y);
	if (m_state == IN_DRAWING)
	{
	}
	else
	{
		if (m_selectedType == ST_SELECTED)
		{
			if (m_hoverType == HT_HOVER_TEXTBOX)
			{
				mip::VECTOR3 delta = v_new - m_curSelectedVertex_World;
				m_v_world += delta;

				/* 이동 후 Selected Pos 초기화 */
				m_curSelectedVertex_World = v_new;
			}
		}
	}
}

void AnnoString::movePosition_Window(WindowBase* win, int x, int y)
{
	mip::VECTOR3 v_new = getVertex_ScreenToWorld_Window(win, x, y);

	if (m_state == IN_DRAWING)
	{
	}
	else
	{
		if (m_selectedType == ST_SELECTED)
		{
			if (m_hoverType == HT_HOVER_TEXTBOX)
			{
				mip::VECTOR3 delta = v_new - m_curSelectedVertex_World;
				m_v_world += delta;

				/* 이동 후 Selected Pos 초기화 */
				m_curSelectedVertex_World = v_new;
			}
		}
	}
}


bool AnnoString::isInMPRPlane_Window(WindowBase * win, WINDOW_TYPE type)
{
	uint depth = win->getDepth();

	return IsVertexInVolumePlane_Window(&DATA_CONTEXT->volume_data, type, depth, m_v_world);
}


bool AnnoString::isInMPRPlane_Measurement(AnalMPRPlaneView *win, WINDOW_TYPE type)
{
	return IsVertexInVolumePlane_Measurement(&DATA_CONTEXT->volume_data, type, m_v_world);
}

void AnnoString::drawVolume_Measurement(AnalVolumeView * win, QPainter * p)
{
	mip::VECTOR3 v_screen = getVertex_WorldToScreen_Volume_Measurement(win, m_v_world);

	drawTextBox(p, v_screen);
}

void AnnoString::drawMPRPlane_Window(WindowBase * win, QPainter *p)
{
	mip::VECTOR3 v_screen = getVertex_WorldToScreen_Window(win, m_v_world);

	drawTextBox(p, v_screen);
}

void AnnoString::drawMPRPlane_Measurement(AnalMPRPlaneView * win, QPainter * p)
{
	mip::VECTOR3 v_screen = getVertex_WorldToScreen_Measurement(win, m_v_world);

	drawTextBox(p, v_screen);
}

void AnnoString::drawVolume(VolumeView * win, QPainter * p)
{
	mip::VECTOR3 v_screen = getVertex_WorldToScreen_Volume(win, m_v_world);

	drawTextBox(p, v_screen);
}

bool AnnoString::isContainTextBox_Measurement(AnalMPRPlaneView * win, int x, int y)
{
	mip::VECTOR3 v_screen = getVertex_WorldToScreen_Measurement(win, m_v_world);
	QRect rect = getDrawingTextBox(v_screen);
	return rect.contains(x, y);
}

bool AnnoString::isContainTextBox_Window(WindowBase* win, int x, int y)
{
	mip::VECTOR3 v_screen = getVertex_WorldToScreen_Window(win, m_v_world);

	QRect rect = getDrawingTextBox(v_screen);
	return rect.contains(x, y);
}

void AnnoString::drawTextBox(QPainter * p, mip::VECTOR3 pos)
{
	/* TextBox Drawing */
	QRect textBox = getDrawingTextBox(pos);
	QFont fontPrev = p->font();
	QPen penPrev = p->pen();
	QBrush brushPrev = p->brush();

	QFont font = getFont();

	p->setBrush(QBrush());
	if (m_selectedType == ST_SELECTED)
	{
		p->setPen(m_textPen_Selected);
		p->drawRect(textBox);
	}
	else
	{
		if (m_hoverType == HT_HOVER_TEXTBOX)
		{
			p->setPen(m_textPen_Hover);
			p->drawRect(textBox);
		}
		else
		{
			p->setPen(m_textPen_Normal);
		}
	}

	/* Text Drawing */
	QString text = getText();
	QPoint textPos(pos.x, pos.y);

	p->setFont(font);
	p->drawText(textPos, text);

	p->setFont(fontPrev);
	p->setPen(penPrev);
	p->setBrush(brushPrev);
}

QRect AnnoString::getDrawingTextBox(mip::VECTOR3 pos)
{
	QFont font = getFont();

	QFontMetrics fontMet(font);
	QRect fontR = fontMet.boundingRect(getText());
	fontR.setWidth(fontR.width() + 2);
	fontR.setHeight(fontR.height() + 2);

	return QRect(
		pos.x - 2,
		pos.y - fontR.height() - 2,
		fontR.width() + 4, fontR.height() + 4
	);
}

QFont AnnoString::getFont()
{
	QFont font;
	font.setPointSize(m_fontSize);
	if (m_selectedType == ST_SELECTED)
	{
		font.setBold(true);
	}
	return font;
}


void AnnoString::setText(QString value)
{
	m_text = value;
}

QString AnnoString::getText()
{
	return m_text;
}

void AnnoString::setFontSize(muint16 value)
{
	m_fontSize = value;
}

muint16 AnnoString::getFontSize()
{
	return m_fontSize;
}

muint16 AnnoString::getTextType()
{
	return m_textType;
}

void AnnoString::setVolumePos(int x, int y, int z)
{
	m_v_world = getVolumeToWorld(&DATA_CONTEXT->volume_data, x, y, z);
}

mip::VECTOR3 AnnoString::getVolumePos()
{
	return getWorldToVolume(
		&DATA_CONTEXT->volume_data,
		m_v_world.x,
		m_v_world.y,
		m_v_world.z
	);
}

