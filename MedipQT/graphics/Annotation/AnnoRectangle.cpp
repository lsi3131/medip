#include "stdafx.h"
#include "AnnoRectangle.h"
#include "Windows/AnalMPRPlaneView.h"
#include "Windows/AnalVolumeView.h"
#include "Windows/WindowBase.h"
#include "Windows/VolumeView.h"
#include "Windows/glwidget.h"
#include "Windows/windowManager.h"
#include "mipEngine/geometry.h"
#include "mipEngine/intersect.h"
#include <cmath>
#include "DataContext.h"

using namespace std;

AnnoRectangle::AnnoRectangle(mip::VECTOR3 vLT, mip::VECTOR3 vRT, mip::VECTOR3 vLB, mip::VECTOR3 vRB, EState state, COLOR color, bool isHidden) :
	Annotation(color)
{
	Init(vLT, vRT, vLB, vRB, state, isHidden);
}

AnnoRectangle::AnnoRectangle(AnalMPRPlaneView* win, int x, int y, COLOR color, bool isHidden) :
	Annotation(color)
{
	useMeasurmentMode(win);
	mip::VECTOR3 v_new = getVertex_ScreenToWorld(x, y);
	Init(v_new, v_new, v_new, v_new, IN_DRAWING, isHidden);
}

AnnoRectangle::AnnoRectangle(WindowBase * win, int x, int y, COLOR color, bool isHidden) :
	Annotation(color)
{
	useWindowMode(win);
	mip::VECTOR3 v_new = getVertex_ScreenToWorld(x, y);
	Init(v_new, v_new, v_new, v_new, IN_DRAWING, isHidden);
}

AnnoRectangle::AnnoRectangle(const AnnoRectangle& rhs) :
	Annotation(rhs.m_color)
{
	Init(rhs.m_vLT, rhs.m_vRT, rhs.m_vLB, rhs.m_vRB, rhs.m_state, rhs.m_isHidden);
}

AnnoRectangle::~AnnoRectangle()
{

}

void AnnoRectangle::Init(mip::VECTOR3 vLT, mip::VECTOR3 vRT, mip::VECTOR3 vLB, mip::VECTOR3 vRB, EState state, bool isHidden)
{
	m_textBoxWidth = 150;
	m_textLineHeight = 12;

	m_isHidden = isHidden;

	m_vLT = vLT;
	m_vRT = vRT;
	m_vLB = vLB;
	m_vRB = vRB;

	m_vTempStart = vLT;
	m_vTempEnd = vRT;

	m_state = state;
	m_isRectangleAreaShouldUpdated = true;
}

void AnnoRectangle::updateRectangleAreaInfo()
{
	/* Rectangle을 업데이트를 해야하는 경우에만 Update*/
	if (m_isRectangleAreaShouldUpdated == false)
	{
		return;
	}

	m_isRectangleAreaShouldUpdated = false;

	std::vector<mint16> HU_List = getHU_List_World(
		&DATA_CONTEXT->volume_data,
		m_vLT, m_vRT,
		m_vLB, m_vRB);

	if (HU_List.empty() == false)
	{
		m_currentHUInfo.calc(HU_List, INT16_MIN, INT16_MAX);
	}
	else
	{
		m_currentHUInfo.clear();
	}

	/* WindowBase 기반 View 사용 시에만 적용 */
	if (m_pWindowView)
	{
		mip::VECTOR3 vLT_volume = getWorldToVolume_PET(&DATA_CONTEXT->volume_data_PET, m_vLT.x, m_vLT.y, m_vLT.z);
		mip::VECTOR3 vRB_volume = getWorldToVolume_PET(&DATA_CONTEXT->volume_data_PET, m_vRB.x, m_vRB.y, m_vRB.z);

		WINDOW_TYPE type = m_pWindowView->getType();

		std::vector<float> SUV_List = getSUV_List_Volume(
			&DATA_CONTEXT->volume_data_PET,
			type,
			vLT_volume, vRB_volume
		);

		if (SUV_List.empty() == false)
		{
			/* float 범위 : -3.4 * 10^38 ~ 3.4 * 10^38 */
			float minRange = -3.4 * pow(10, 37.9);
			float maxRange = 3.4 * pow(10, 37.9);
			m_currentSUVInfo.calc(SUV_List, minRange, maxRange);
		}
		else
		{
			m_currentSUVInfo.clear();
		}
	}
}

ANNOTATION_TYPE AnnoRectangle::getType()
{
	return AT_RECTANGLE;
}

bool AnnoRectangle::addDrawingPoint_Measurement(AnalMPRPlaneView * win, int x, int y)
{
	useMeasurmentMode(win);
	return addDrawingPoint(x, y);
}

bool AnnoRectangle::addDrawingPoint_Window(WindowBase* win, int x, int y)
{
	useWindowMode(win);
	return addDrawingPoint(x, y);
}

void AnnoRectangle::movePosition_Measurement(AnalMPRPlaneView * win, int x, int y)
{
	useMeasurmentMode(win);
	movePosition(x, y);
}

void AnnoRectangle::movePosition_Window(WindowBase * win, int x, int y)
{
	useWindowMode(win);
	movePosition(x, y);
}

bool AnnoRectangle::isInMPRPlane_Measurement(AnalMPRPlaneView *win, WINDOW_TYPE type)
{
	useMeasurmentMode(win);

	if (IsVertexInVolumePlane_Measurement(&DATA_CONTEXT->volume_data, type, m_vLT) == false)
	{
		return false;
	}

	if (IsVertexInVolumePlane_Measurement(&DATA_CONTEXT->volume_data, type, m_vRT) == false)
	{
		return false;
	}

	if (IsVertexInVolumePlane_Measurement(&DATA_CONTEXT->volume_data, type, m_vLB) == false)
	{
		return false;
	}

	if (IsVertexInVolumePlane_Measurement(&DATA_CONTEXT->volume_data, type, m_vRB) == false)
	{
		return false;
	}

	return true;
}

bool AnnoRectangle::isInMPRPlane_Window(WindowBase * win, WINDOW_TYPE type)
{
	useWindowMode(win);

	muint32 depth = win->getDepth();
	if (IsVertexInVolumePlane_Window(&DATA_CONTEXT->volume_data, type, depth, m_vLT) == false)
	{
		return false;
	}

	if (IsVertexInVolumePlane_Window(&DATA_CONTEXT->volume_data, type, depth, m_vRT) == false)
	{
		return false;
	}

	if (IsVertexInVolumePlane_Window(&DATA_CONTEXT->volume_data, type, depth, m_vLB) == false)
	{
		return false;
	}

	if (IsVertexInVolumePlane_Window(&DATA_CONTEXT->volume_data, type, depth, m_vRB) == false)
	{
		return false;
	}

	return true;
}

void AnnoRectangle::drawVolume_Measurement(AnalVolumeView * win, QPainter * p)
{
	//Volume Draw SKIP
}

void AnnoRectangle::drawMPRPlane_Measurement(AnalMPRPlaneView * win, QPainter * p)
{
	useMeasurmentMode(win);
	drawMPRPlane(p);
}

void AnnoRectangle::drawVolume(VolumeView * win, QPainter * p)
{
	//Volume Draw SKIP
}

void AnnoRectangle::drawMPRPlane_Window(WindowBase * win, QPainter * p)
{
	useWindowMode(win);
	drawMPRPlane(p);
}

bool AnnoRectangle::isContainLine_Measurement(AnalMPRPlaneView* win, int x, int y)
{
	useMeasurmentMode(win);
	return isContainLine(x, y);
}

bool AnnoRectangle::isContainLine_Window(WindowBase * win, int x, int y)
{
	useWindowMode(win);
	return isContainLine(x, y);
}

bool AnnoRectangle::isContainVertex_Measurement(AnalMPRPlaneView* win, int x, int y)
{
	useMeasurmentMode(win);
	return getContainVertex(&m_selectedVertex, x, y);
}

bool AnnoRectangle::isContainVertex_Window(WindowBase * win, int x, int y)
{
	useWindowMode(win);
	return getContainVertex(&m_selectedVertex, x, y);
}

bool AnnoRectangle::isContainTextBox_Measurement(AnalMPRPlaneView * win, int x, int y)
{
	useMeasurmentMode(win);
	return isContainTextBox(x, y);
}

bool AnnoRectangle::isContainTextBox_Window(WindowBase * win, int x, int y)
{
	useWindowMode(win);
	return isContainTextBox(x, y);
}

bool AnnoRectangle::addDrawingPoint(int x, int y)
{
	mip::VECTOR3 v_new_world = getVertex_ScreenToWorld(x, y);

	if (m_state == IN_DRAWING)
	{
		updateRectangleVertex_ByTempVertex(v_new_world);
		m_state = DRAWING_FINISHED;
	}
	return true;
}

bool AnnoRectangle::getContainVertex(EVertex* pOutVertex, int x, int y)
{
	float minDistance = 5.0f;

	mip::VECTOR3 vLT_Screen_3D = getVertex_WorldToScreen(m_vLT);
	mip::VECTOR3 vRT_Screen_3D = getVertex_WorldToScreen(m_vRT);
	mip::VECTOR3 vLB_Screen_3D = getVertex_WorldToScreen(m_vLB);
	mip::VECTOR3 vRB_Screen_3D = getVertex_WorldToScreen(m_vRB);

	mip::VECTOR2 vLT_Screen(vLT_Screen_3D.x, vLT_Screen_3D.y);
	mip::VECTOR2 vRT_Screen(vRT_Screen_3D.x, vRT_Screen_3D.y);
	mip::VECTOR2 vLB_Screen(vLB_Screen_3D.x, vLB_Screen_3D.y);
	mip::VECTOR2 vRB_Screen(vRB_Screen_3D.x, vRB_Screen_3D.y);

	if ((vLT_Screen - mip::VECTOR2(x, y)).length() <= minDistance)
	{
		*pOutVertex = VERTEX_LEFT_TOP;
		return true;
	}
	else if ((vRT_Screen - mip::VECTOR2(x, y)).length() <= minDistance)
	{
		*pOutVertex = VERTEX_RIGHT_TOP;
		return true;
	}
	else if ((vLB_Screen - mip::VECTOR2(x, y)).length() <= minDistance)
	{
		*pOutVertex = VERTEX_LEFT_BOTTOM;
		return true;
	}
	else if ((vRB_Screen - mip::VECTOR2(x, y)).length() <= minDistance)
	{
		*pOutVertex = VERTEX_RIGHT_BOTTOM;
		return true;
	}

	return false;
}

bool AnnoRectangle::isContainLine(int x, int y)
{
	const float minDistance = 5.f;

	mip::VECTOR3 vLT_Screen_3D = getVertex_WorldToScreen(m_vLT);
	mip::VECTOR3 vRT_Screen_3D = getVertex_WorldToScreen(m_vRT);
	mip::VECTOR3 vLB_Screen_3D = getVertex_WorldToScreen(m_vLB);
	mip::VECTOR3 vRB_Screen_3D = getVertex_WorldToScreen(m_vRB);

	mip::VECTOR2 vLT_Screen(vLT_Screen_3D.x, vLT_Screen_3D.y);
	mip::VECTOR2 vRT_Screen(vRT_Screen_3D.x, vRT_Screen_3D.y);
	mip::VECTOR2 vLB_Screen(vLB_Screen_3D.x, vLB_Screen_3D.y);
	mip::VECTOR2 vRB_Screen(vRB_Screen_3D.x, vRB_Screen_3D.y);

	mip::VECTOR2 dot = mip::VECTOR2(x, y);

	if (IsLineContainDot(vLT_Screen, vRT_Screen, dot, minDistance))
	{
		return true;
	}

	if (IsLineContainDot(vLT_Screen, vLB_Screen, dot, minDistance))
	{
		return true;
	}

	if (IsLineContainDot(vLB_Screen, vRB_Screen, dot, minDistance))
	{
		return true;
	}

	if (IsLineContainDot(vRT_Screen, vRB_Screen, dot, minDistance))
	{
		return true;
	}

	return false;
}

bool AnnoRectangle::isContainTextBox(int x, int y)
{
	mip::VECTOR3 vText_screen = getVertex_WorldToScreen(getTextVertex());
	QRect rect = getDrawingTextBox(vText_screen.x, vText_screen.y);
	return rect.contains(x, y);
}

void AnnoRectangle::movePosition(int x, int y)
{
	mip::VECTOR3 v_new = getVertex_ScreenToWorld(x, y);

	if (m_state == IN_DRAWING)
	{
		updateRectangleVertex_ByTempVertex(v_new);
		m_isRectangleAreaShouldUpdated = true;
	}
	else
	{
		if (m_selectedType == ST_SELECTED)
		{
			if (m_hoverType == HT_HOVER_LINE)
			{
				mip::VECTOR3 delta = v_new - m_curSelectedVertex_World;
				m_vLT += delta;
				m_vRT += delta;
				m_vLB += delta;
				m_vRB += delta;

				/* 이동 후 Selected Pos 초기화 */
				m_curSelectedVertex_World = v_new;
				m_isRectangleAreaShouldUpdated = true;
			}
			else if (m_hoverType == HT_HOVER_VERTEX)
			{
				moveRectangleVertex(v_new);
				m_isRectangleAreaShouldUpdated = true;
			}
			else if (m_hoverType == HT_HOVER_TEXTBOX)
			{
				mip::VECTOR3 delta = v_new - m_curSelectedVertex_World;
				/* Text 좌표 초기화 */
				if (m_pVText == nullptr)
				{
					mip::VECTOR3 vText = getTextVertex();
					m_pVText = std::make_unique<mip::VECTOR3>(vText);
				}
				*m_pVText += delta;

				m_curSelectedVertex_World = v_new;
			}
		}
	}
}

void AnnoRectangle::drawMPRPlane(QPainter * p)
{
	QRect rect_screen = getDrawingRectangle();
	mip::VECTOR3 vText_screen = getVertex_WorldToScreen(getTextVertex());

	updateRectangleAreaInfo();

	drawRectangle(p, rect_screen);

	drawVertexEllipse(p, rect_screen.topLeft(), VERTEX_LEFT_TOP);
	drawVertexEllipse(p, rect_screen.topRight(), VERTEX_RIGHT_TOP);
	drawVertexEllipse(p, rect_screen.bottomLeft(), VERTEX_LEFT_BOTTOM);
	drawVertexEllipse(p, rect_screen.bottomRight(), VERTEX_RIGHT_BOTTOM);

	drawTextBox(p, vText_screen.x, vText_screen.y);
}

void AnnoRectangle::drawRectangle(QPainter * p, QRect rect)
{
	p->setBrush(QBrush());

	if (m_hoverType == HT_HOVER_LINE ||
		m_hoverType == HT_HOVER_TEXTBOX)
	{
		p->setPen(m_linePen_Hover_Border);
		p->drawRect(rect);
	}
	else if (m_selectedType == ST_SELECTED)
	{
		p->setPen(m_linePen_Selected_Border);
		p->drawRect(rect);
	}

	p->setPen(m_linePen_Normal);
	p->drawRect(rect);
}

void AnnoRectangle::drawVertexEllipse(QPainter * p, QPoint pos, EVertex vertexPostion)
{
	p->setPen(QPen());

	if (m_selectedVertex == vertexPostion)
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

void AnnoRectangle::drawTextBox(QPainter * p, int x, int y)
{
	/* TextBox Drawing */
	QRect textBox = getDrawingTextBox(x, y);
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
	QStringList textList = getTextList();
	for (int i = 0; i < textList.size(); ++i)
	{
		int txtX = x + 3;
		int txtY = y + i * m_textLineHeight + 3;
		QPoint textPos(txtX, txtY);

		p->setPen(m_textBoxPen_Normal);
		p->drawText(textPos, textList[i]);
	}
}

QRect AnnoRectangle::getDrawingRectangle()
{
	mip::VECTOR3 LT_screen = getVertex_WorldToScreen(m_vLT);
	mip::VECTOR3 RB_screen = getVertex_WorldToScreen(m_vRB);

	QRect rect = QRect(
		QPoint(LT_screen.x, LT_screen.y),
		QPoint(RB_screen.x, RB_screen.y)
	);

	return rect;
}

QRect AnnoRectangle::getDrawingTextBox(int x, int y)
{
	int textBoxHeight = m_textLineHeight * getTextList().size();
	QRect fontR = QRect(
		0, -m_textLineHeight,
		m_textBoxWidth, textBoxHeight);

	fontR.setWidth(fontR.width() + 2);
	fontR.setHeight(fontR.height() + 2);

	return QRect(
		x,
		y + fontR.y(),
		fontR.width() + 10, fontR.height() + 10
	);
}

QStringList AnnoRectangle::getTextList()
{
	QStringList texts;

	QString str;
	texts << str.sprintf("[  HU  ]");
	texts << str.sprintf("Mean=%.2f, SD=%.2f", m_currentHUInfo.mean(), m_currentHUInfo.standardDeviation());
	texts << str.sprintf("Max=%.2f, Min=%.2f", m_currentHUInfo.max(), m_currentHUInfo.min());

	if (m_currentSUVInfo.isValid())
	{
		texts << "";
		texts << str.sprintf("[  SUV  ]");
		texts << str.sprintf("Mean=%.2f, SD=%.2f", m_currentSUVInfo.mean(), m_currentSUVInfo.standardDeviation());
		texts << str.sprintf("Max=%.2f, Min=%.2f", m_currentSUVInfo.max(), m_currentSUVInfo.min());
	}

	return texts;
}

void AnnoRectangle::updateRectangleVertex_ByTempVertex(mip::VECTOR3 v_new)
{
	m_vTempEnd = v_new;

	mip::VECTOR3 vStartPos_Screen = getVertex_WorldToScreen(m_vTempStart);
	mip::VECTOR3 vEndPos_Screen = getVertex_WorldToScreen(m_vTempEnd);

	QRect rect_screen(
		QPoint(vStartPos_Screen.x, vStartPos_Screen.y),
		QPoint(vEndPos_Screen.x, vEndPos_Screen.y)
	);

	updateRectangleVertex_ByRectScreen(rect_screen, true);
}

void AnnoRectangle::moveRectangleVertex(mip::VECTOR3 v_world)
{
	//1. World -> Screen으로 좌표를 변환한다.
	QRect rect_screen = getDrawingRectangle();

	QPoint v_screen = QPoint(
		getVertex_WorldToScreen(v_world).x,
		getVertex_WorldToScreen(v_world).y);

	/* Vertex 위치 설정 */
	if (m_selectedVertex == VERTEX_LEFT_TOP)
	{
		rect_screen.setTopLeft(v_screen);
	}
	else if (m_selectedVertex == VERTEX_RIGHT_TOP)
	{
		rect_screen.setTopRight(v_screen);
	}
	else if (m_selectedVertex == VERTEX_LEFT_BOTTOM)
	{
		rect_screen.setBottomLeft(v_screen);
	}
	else if (m_selectedVertex == VERTEX_RIGHT_BOTTOM)
	{
		rect_screen.setBottomRight(v_screen);
	}

	/* Rect 상태 Left <-> Right 의 값이 변할 경우 Refresh*/
	if (rect_screen.left() > rect_screen.right())
	{
		switch (m_selectedVertex)
		{
		case VERTEX_LEFT_TOP:
			m_selectedVertex = VERTEX_RIGHT_TOP;
			break;
		case VERTEX_RIGHT_TOP:
			m_selectedVertex = VERTEX_LEFT_TOP;
			break;
		case VERTEX_LEFT_BOTTOM:
			m_selectedVertex = VERTEX_RIGHT_BOTTOM;
			break;
		case VERTEX_RIGHT_BOTTOM:
			m_selectedVertex = VERTEX_LEFT_BOTTOM;
			break;

		}
	}

	/* Rect 상태 Top <-> Bottom 의 값이 변할 경우 Refresh*/
	if (rect_screen.top() > rect_screen.bottom())
	{
		switch (m_selectedVertex)
		{
		case VERTEX_LEFT_TOP:
			m_selectedVertex = VERTEX_LEFT_BOTTOM;
			break;
		case VERTEX_RIGHT_TOP:
			m_selectedVertex = VERTEX_RIGHT_BOTTOM;
			break;
		case VERTEX_LEFT_BOTTOM:
			m_selectedVertex = VERTEX_LEFT_TOP;
			break;
		case VERTEX_RIGHT_BOTTOM:
			m_selectedVertex = VERTEX_RIGHT_TOP;
			break;

		}
	}

	updateRectangleVertex_ByRectScreen(rect_screen, true);
}

mip::VECTOR3 AnnoRectangle::getTextVertex()
{
	if (m_pVText == nullptr)
	{
		mip::VECTOR3 vGap = getVertex_ScreenToWorld(5, 0) - getVertex_ScreenToWorld(0, 0);
		return m_vRT + vGap;
	}
	else
	{
		return *m_pVText;
	}
}

void AnnoRectangle::updateRectangleVertex_ByRectScreen(QRect rect_screen, bool normalizeRect)
{
	if (normalizeRect)
	{
		rect_screen = rect_screen.normalized();
	}

	QPoint LT_Screen = rect_screen.topLeft();
	QPoint LB_Screen = rect_screen.bottomLeft();
	QPoint RT_Screen = rect_screen.topRight();
	QPoint RB_Screen = rect_screen.bottomRight();

	m_vLT = getVertex_ScreenToWorld(LT_Screen.x(), LT_Screen.y());
	m_vLB = getVertex_ScreenToWorld(LB_Screen.x(), LB_Screen.y());
	m_vRT = getVertex_ScreenToWorld(RT_Screen.x(), RT_Screen.y());
	m_vRB = getVertex_ScreenToWorld(RB_Screen.x(), RB_Screen.y());
}

mip::VECTOR3 AnnoRectangle::getVertex_ScreenToWorld(int x, int y)
{
	if (m_pWindowView)
	{
		return getVertex_ScreenToWorld_Window(m_pWindowView, x, y);
	}
	else if (m_pMeasurementView)
	{
		return getVertex_ScreenToWorld_Measurement(m_pMeasurementView, x, y);
	}
	else
	{
		//INVALID
		return mip::VECTOR3();
	}
}

mip::VECTOR3 AnnoRectangle::getVertex_WorldToScreen(mip::VECTOR3 v)
{
	if (m_pWindowView)
	{
		return getVertex_WorldToScreen_Window(m_pWindowView, v);
	}
	else if (m_pMeasurementView)
	{
		return getVertex_WorldToScreen_Measurement(m_pMeasurementView, v);
	}
	else
	{
		//INVALID
		return mip::VECTOR3();
	}
}

void AnnoRectangle::useMeasurmentMode(AnalMPRPlaneView* pMeasurementView)
{
	m_pWindowView = nullptr;
	m_pMeasurementView = pMeasurementView;
}

void AnnoRectangle::useWindowMode(WindowBase * pWindowView)
{
	m_pWindowView = pWindowView;
	m_pMeasurementView = nullptr;
}

