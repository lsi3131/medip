#include "stdafx.h"
#include "anotation.h"
#include "Windows/AnalMPRPlaneView.h"
#include "Windows/AnalVolumeView.h"
#include "Windows/VolumeView.h"
#include "Windows/windowManager.h"
#include "Windows/glwidget.h"
#include "mipEngine/geometry.h"
#include "mipEngine/intersect.h"
#include "DataContext.h"

//=========================================================================
//			Annotation
//=========================================================================
Annotation::Annotation(COLOR color) :
	m_color(color)
{
	m_isHidden = false;

	m_state = IN_DRAWING;
	m_selectedType = ST_UNSELECTED;
	m_hoverType = HT_HOVER_NONE;

	setColor(color);
}

bool Annotation::setHover_Measurement(OpenGLWidget* win, int x, int y)
{
	if (m_state == IN_DRAWING)
	{
		return false;
	}

	AnalMPRPlaneView* MPR_win	= dynamic_cast<AnalMPRPlaneView*>(win);
	AnalVolumeView* Vol_Win		= dynamic_cast<AnalVolumeView*>(win);

	if (MPR_win == nullptr && Vol_Win == nullptr)
	{
		return false;
	}

	m_hoverType = HT_HOVER_NONE;

	if (MPR_win != nullptr)
	{
		if (isContainLine_Measurement(MPR_win, x, y))
		{
			m_hoverType = HT_HOVER_LINE;
		}

		if (isContainVertex_Measurement(MPR_win, x, y))
		{
			m_hoverType = HT_HOVER_VERTEX;
		}

		if (isContainTextBox_Measurement(MPR_win, x, y))
		{
			m_hoverType = HT_HOVER_TEXTBOX;
		}
	}

	if (Vol_Win != nullptr)
	{
		if (isContainVertex_Measurement_Vol(Vol_Win, x, y))
		{
			m_hoverType = HT_HOVER_VERTEX;
		}
	}

	return
		m_hoverType == HT_HOVER_LINE ||
		m_hoverType == HT_HOVER_VERTEX ||
		m_hoverType == HT_HOVER_TEXTBOX;

	return true;
}

bool Annotation::setSelect_Measurement(OpenGLWidget* win, int x, int y)
{
	if (m_state == IN_DRAWING)
	{
		return false;
	}

	AnalMPRPlaneView* MPR_win = dynamic_cast<AnalMPRPlaneView*>(win);
	AnalVolumeView* Vol_Win = dynamic_cast<AnalVolumeView*>(win);

	if (MPR_win == nullptr && Vol_Win == nullptr)
	{
		return false;
	}

	m_selectedType = ST_UNSELECTED;
	m_hoverType = HT_HOVER_NONE;

	if (MPR_win != nullptr)
	{
		m_curSelectedVertex_World = getVertex_ScreenToWorld_Measurement(MPR_win, x, y);

		if (isContainLine_Measurement(MPR_win, x, y))
		{
			m_selectedType = ST_SELECTED;
			m_hoverType = HT_HOVER_LINE;
		}

		if (isContainVertex_Measurement(MPR_win, x, y))
		{
			m_selectedType = ST_SELECTED;
			m_hoverType = HT_HOVER_VERTEX;
		}

		if (isContainTextBox_Measurement(MPR_win, x, y))
		{
			m_selectedType = ST_SELECTED;
			m_hoverType = HT_HOVER_TEXTBOX;
		}
	}

	if (Vol_Win != nullptr)
	{
		if (isContainVertex_Measurement_Vol(Vol_Win, x, y))
		{
			m_hoverType = HT_HOVER_VERTEX;
			m_selectedType = ST_SELECTED;
		}
	}

	return m_selectedType == ST_SELECTED;
}

bool Annotation::setHover_Window(WindowBase * win, int x, int y)
{
	if (m_state == IN_DRAWING)
	{
		return false;
	}

	m_hoverType = HT_HOVER_NONE;
	if (isContainLine_Window(win, x, y))
	{
		m_hoverType = HT_HOVER_LINE;
	}

	if (isContainVertex_Window(win, x, y))
	{
		m_hoverType = HT_HOVER_VERTEX;
	}

	if (isContainTextBox_Window(win, x, y))
	{
		m_hoverType = HT_HOVER_TEXTBOX;
	}

	return
		m_hoverType == HT_HOVER_LINE ||
		m_hoverType == HT_HOVER_VERTEX ||
		m_hoverType == HT_HOVER_TEXTBOX;
}

bool Annotation::setSelect_Window(WindowBase * win, int x, int y)
{
	if (m_state == IN_DRAWING)
	{
		return false;
	}

	m_curSelectedVertex_World = getVertex_ScreenToWorld_Window(win, x, y);

	m_selectedType = ST_UNSELECTED;
	m_hoverType = HT_HOVER_NONE;
	if (isContainLine_Window(win, x, y))
	{
		m_selectedType = ST_SELECTED;
		m_hoverType = HT_HOVER_LINE;
	}

	if (isContainVertex_Window(win, x, y))
	{
		m_selectedType = ST_SELECTED;
		m_hoverType = HT_HOVER_VERTEX;
	}

	if (isContainTextBox_Window(win, x, y))
	{
		m_selectedType = ST_SELECTED;
		m_hoverType = HT_HOVER_TEXTBOX;
	}

	return m_selectedType == ST_SELECTED;
}

void Annotation::setSelect()
{
	m_selectedType = ST_SELECTED;
}

void Annotation::clearSelect()
{
	m_selectedType = ST_UNSELECTED;
	m_hoverType = HT_HOVER_NONE;
}

bool Annotation::isSelected()
{
	return m_selectedType == ST_SELECTED;
}

mip::VECTOR3 Annotation::getVertex_ScreenToWorld_Measurement(AnalMPRPlaneView * win, int x, int y)
{
	int imgWidth = win->width();
	int imgHeight = win->height();

	mip::MATRIX44 matView = win->getCamera().getView();
	mip::MATRIX44 matProj = win->getCamera().getProj();

	/*
		현재 Screen2World 함수의 depth변환에 버그가 있음.
		임시로 CameraIntersectedPlane() 함수로 대체해서 사용
	*/
	//return mip::geom::Screen2World(x, y, imgWidth, imgHeight, matView, matProj);
	mip::VECTOR3 v;
	win->getCameraToPlaneIntersectedPoint(&v, x, y);
	return v;
}

mip::VECTOR3 Annotation::getVertex_WorldToScreen_Measurement(AnalMPRPlaneView * win, mip::VECTOR3 v)
{
	mip::MATRIX44 matView = win->getCamera().getView();
	mip::MATRIX44 matProj = win->getCamera().getProj();

	return mip::geom::WorldToScreen(v, win->width(), win->height(), matView, matProj);
}

mip::VECTOR2 Annotation::getVertex_WorldToScreen_Measurement_2D(AnalMPRPlaneView * win, mip::VECTOR3 v)
{
	mip::VECTOR3 v3D = getVertex_WorldToScreen_Measurement(win, v);
	return mip::VECTOR2(v3D.x, v3D.y);
}

mip::VECTOR3 Annotation::getVertex_ScreenToWorld_Volume_Measurement(AnalVolumeView * win, int x, int y)
{
	int imgWidth = win->width();
	int imgHeight = win->height();

	mip::MATRIX44 matView = win->getCamera().getView();
	mip::MATRIX44 matProj = win->getCamera().getProj();

	return mip::geom::Screen2World(x, y, imgWidth, imgHeight, matView, matProj);
}

mip::VECTOR3 Annotation::getVertex_WorldToScreen_Volume_Measurement(AnalVolumeView * win, mip::VECTOR3 v)
{
	mip::MATRIX44 matView = win->getCamera().getView();
	mip::MATRIX44 matProj = win->getCamera().getProj();
	mip::MATRIX44 matWorld = win->getWorld().getMatrix();

	return mip::geom::WorldToScreen(v, win->width(), win->height(), matView, matProj, &matWorld);
}

mip::VECTOR3 Annotation::getVertex_ScreenToWorld_Volume(VolumeView * win, int x, int y)
{
	int imgWidth = win->width();
	int imgHeight = win->height();

	mip::MATRIX44 matView = win->getCamera().getView();
	mip::MATRIX44 matProj = win->getCamera().getProj();

	return mip::geom::Screen2World(x, y, imgWidth, imgHeight, matView, matProj);
}

mip::VECTOR3 Annotation::getVertex_WorldToScreen_Volume(VolumeView * win, mip::VECTOR3 v)
{
	mip::MATRIX44 matView = win->getCamera().getView();
	mip::MATRIX44 matProj = win->getCamera().getProj();
	mip::MATRIX44 matWorld = win->getWorld().getMatrix();

	return mip::geom::WorldToScreen(v, win->width(), win->height(), matView, matProj, &matWorld);
}

mip::VECTOR3 Annotation::getVertex_WorldToScreen_Window(WindowBase * win, mip::VECTOR3 v)
{
	mip::VECTOR3 v_volume = getWorldToVolume(&DATA_CONTEXT->volume_data, v.x, v.y, v.z);

	QVector3D v_screen_3D = win->volumeToScreenPosition(v_volume.x, v_volume.y, v_volume.z);
	return mip::VECTOR3(v_screen_3D.x(), v_screen_3D.y(), v_screen_3D.z());
}

mip::VECTOR3 Annotation::getVertex_ScreenToWorld_Window(WindowBase * win, int x, int y)
{
	QPoint pos(x, y);
	QVector3D v_volume = win->screenToVolumePosition3(&pos);

	return getVolumeToWorld(&DATA_CONTEXT->volume_data, v_volume.x(), v_volume.y(), v_volume.z());
}

void Annotation::drawAnno(OpenGLWidget* win, QPainter * p, ANNOTATION_DRAWING_VIEWER_TYPE type)
{
	if (win == NULL || p == NULL)
	{
		return;
	}

	/* Drawing Object 초기화 */
	p->setBrush(QBrush());
	p->setPen(QPen());
	p->setFont(QFont());

	if (type == ADV_MPR_PLANE_WINDOW_VIEWER)
	{
		WindowBase* pWinBase = dynamic_cast<WindowBase*>(win);
		if (pWinBase != nullptr)
		{
			if (isInMPRPlane_Window(pWinBase, pWinBase->getType()))
			{
				drawMPRPlane_Window(pWinBase, p);
			}
		}
	}
	else if (type == ADV_MPR_PLANE_MEASUREMENT_VIEWER)
	{
		AnalMPRPlaneView* pAnalMPRView = dynamic_cast<AnalMPRPlaneView*>(win);
		if (pAnalMPRView != nullptr)
		{
			int cntInPlane = getCnt_IsInMPRPlane_Measurement(pAnalMPRView, pAnalMPRView->getType());

			if (cntInPlane == 1)
				drawPoint_VolumeMeasurement(pAnalMPRView, p, pAnalMPRView->getType());

			if (isInMPRPlane_Measurement(pAnalMPRView, pAnalMPRView->getType()))
				drawMPRPlane_Measurement(pAnalMPRView, p);
		}
	}
	else if (type == ADV_MEASUREMENT_VOLUME_VIEWER)
	{
		AnalVolumeView* pAnalVolumeView = dynamic_cast<AnalVolumeView*>(win);
		if (pAnalVolumeView != nullptr)
		{
			drawVolume_Measurement(pAnalVolumeView, p);
		}
	}
	else if (type == ADV_VOLUME_VIEWER)
	{
		VolumeView* pVolumeView = dynamic_cast<VolumeView*>(win);
		if (pVolumeView != nullptr)
		{
			drawVolume(pVolumeView, p);
		}
	}
}


COLOR Annotation::getColor()
{
	return m_color;
}

void Annotation::setColor(COLOR col)
{
	m_color = col;

	QColor hoverBoderColor = QColor(255, 255, 0);
	QColor selectedBoderColor = QColor(255, 255, 255);

	/* Font, Pen, Brush 색상 업데이트 */
	/* Line */
	m_lineWidth = 2;

	m_linePen_Normal.setColor(toQColor(m_color));
	m_linePen_Hover.setColor(QColor(255, 255, 0));
	m_linePen_Selected.setColor(QColor(255, 0, 0));

	m_linePen_Normal.setWidth(m_lineWidth);
	m_linePen_Hover.setWidth(m_lineWidth);
	m_linePen_Selected.setWidth(m_lineWidth);

	m_linePen_Hover_Border.setColor(hoverBoderColor);
	m_linePen_Selected_Border.setColor(selectedBoderColor);

	m_linePen_Hover_Border.setWidth(m_lineWidth + 2);
	m_linePen_Selected_Border.setWidth(m_lineWidth + 2);

	/* Vertex */
	m_vertexRadius = 4;

	m_vertexEllipseBrush_Normal.setColor(toQColor(m_color));
	m_vertexEllipseBrush_Normal.setStyle(Qt::BrushStyle::SolidPattern);

	m_vertexEllipseBrush_Hover.setColor(QColor(255, 255, 0));
	m_vertexEllipseBrush_Hover.setStyle(Qt::BrushStyle::SolidPattern);

	m_vertexEllipseBrush_Selected.setColor(QColor(255, 255, 0));
	m_vertexEllipseBrush_Selected.setStyle(Qt::BrushStyle::SolidPattern);

	m_vertexBorderRadius = m_vertexRadius + 2;
	m_vertexEllipseBrush_Hover_Border.setColor(hoverBoderColor);
	m_vertexEllipseBrush_Hover_Border.setStyle(Qt::BrushStyle::SolidPattern);

	m_vertexEllipseBrush_Selected_Border.setColor(selectedBoderColor);
	m_vertexEllipseBrush_Selected_Border.setStyle(Qt::BrushStyle::SolidPattern);

	/* Text Box */
	m_textBoxPen_Normal.setColor(QColor(255, 255, 0, 200));
	m_textBoxPen_Hover.setColor(QColor(255, 255, 0, 200));
	m_textBoxPen_Selected.setColor(QColor(255, 255, 0, 200));

	m_textBoxBrush_Normal.setColor(QColor(55, 24, 24, 200));
	m_textBoxBrush_Normal.setStyle(Qt::BrushStyle::SolidPattern);

	m_textBoxBrush_Hover.setColor(QColor(118, 8, 8, 200));
	m_textBoxBrush_Hover.setStyle(Qt::BrushStyle::SolidPattern);

	m_textBoxBrush_Selected.setColor(QColor(118, 8, 8, 200));
	m_textBoxBrush_Selected.setStyle(Qt::BrushStyle::SolidPattern);

	/* Text Pen */
	m_textPen_Normal = QPen(QColor(m_color.r, m_color.g, m_color.b));
	m_textPen_Normal.setWidth(1);

	m_textPen_Hover = QPen(QColor(m_color.r, m_color.g, m_color.b));
	m_textPen_Hover.setWidth(1);

	m_textPen_Selected = QPen(QColor(m_color.r, m_color.g, m_color.b));
	m_textPen_Selected.setWidth(2);
}

//=================================================
//		Annotation Utility
//=================================================
bool CompFloatingPoint(double a1, double a2, double absTolerance)
{
	double diff = a1 - a2;
	return fabs(diff) <= absTolerance;
}

bool IsLineContainDot(mip::VECTOR2 lineP1, mip::VECTOR2 lineP2, mip::VECTOR2 dotP3, float minDistance)
{
	/* 점과 직선 사이의 거리 방정식으로 거리(distance)를 구한다. */
	mip::VECTOR2 p1(lineP1);
	mip::VECTOR2 p2(lineP2);
	mip::VECTOR2 p3(dotP3);

	float x1 = p1.x;
	float y1 = p1.y;
	float x2 = p2.x;
	float y2 = p2.y;

	float x = p3.x;
	float y = p3.y;

	float distance = ((y1 - y2)* x + (x2 - x1)* y + x1*y2 - x2*y1) / sqrt(pow(y1 - y2, 2) + pow(x2 - x1, 2));
	distance = fabs(distance);

	/* P1, P2, P3 영역이 벗어나지 않는지 확인한다.  */
	mip::VECTOR2 p1_to_p2 = p2 - p1;
	mip::VECTOR2 p1_to_p3 = p3 - p1;
	mip::VECTOR2 p2_to_p3 = p3 - p2;

	/* 직선과 점 P3가 만나는 점을 X라고 했을 때 P1->X, P2->X길이를 구한다..*/
	float p1_X_Length = sqrt(pow(p1_to_p3.length(), 2) - pow(distance, 2));
	float p2_X_Length = sqrt(pow(p2_to_p3.length(), 2) - pow(distance, 2));
	float sumLength = p1_X_Length + p2_X_Length;

	float lineLength = p1_to_p2.length();

	if (CompFloatingPoint(lineLength, sumLength, 0.001))
	{
		return distance <= minDistance;
	}
	else
	{
		return false;
	}
}

bool IsVertexInVolumePlane_Measurement(VOLUME_DATA * pVolume, WINDOW_TYPE type, mip::VECTOR3 v)
{
	std::vector<mip::VECTOR3> tempList;
	pVolume->getMPRPPlanes(type, tempList); //Measurement TAB에서만 값이 적용됨 
	mip::PLANE plane(tempList[0], tempList[1], tempList[2]);

	float minValue = -0.1f;
	float maxValue = 0.2f;

	float res = plane.dot(v);
	if ((minValue <= res && res < maxValue) == false)
	{
		return false;
	}

	return true;
}

bool IsVertexInVolumePlane_Window(VOLUME_DATA * pVolume, WINDOW_TYPE type, uint depth, mip::VECTOR3 v)
{
	mip::VECTOR3 v_volume = getWorldToVolume(pVolume, v.x, v.y, v.z);

	if (type == WT_CORONAL)
	{
		if (depth == (muint32)v_volume.y)
			return true;
	}
	else if (type == WT_SAGITTAL)
	{
		if (depth == (muint32)v_volume.x)
			return true;
	}
	else if (type == WT_AXIAL)
	{
		if (depth == (muint32)v_volume.z)
			return true;
	}
	return false;
}

QColor toQColor(COLOR c)
{
	return QColor(c.r, c.g, c.b, c.a);
}

COLOR toCOLOR(QColor c)
{
	return COLOR(c.red(), c.green(), c.blue(), c.alpha());
}

mip::VECTOR3 getWorldToVolume(VOLUME_DATA* pVolume, float x, float y, float z)
{
	mip::VECTOR3 basisVec;
	mip::VECTOR3 LocalVec;
	mip::VECTOR3 volumeVec;

	basisVec.set(-pVolume->getSizeX() * 0.5f, -pVolume->getSizeY() * 0.5f, -pVolume->getSizeZ() * 0.5f);

	LocalVec.set(x, y, z);

	volumeVec.set((LocalVec - basisVec));

	volumeVec.x /= pVolume->getSpaceX();
	volumeVec.y /= pVolume->getSpaceY();
	volumeVec.z /= pVolume->getSpaceZ();

	if (volumeVec.x < 0)
		volumeVec.x = 0;
	else if (volumeVec.x >= pVolume->getCX())
		volumeVec.x = pVolume->getCX() - 1;

	if (volumeVec.y < 0)
		volumeVec.y = 0;
	else if (volumeVec.y >= pVolume->getCY())
		volumeVec.y = pVolume->getCY() - 1;

	if (volumeVec.z < 0)
		volumeVec.z = 0;
	else if (volumeVec.z >= pVolume->getCZ())
		volumeVec.z = pVolume->getCZ() - 1;

	volumeVec.x = round(volumeVec.x);
	volumeVec.y = round(volumeVec.y);
	volumeVec.z = round(volumeVec.z);

	return volumeVec;
}

mip::VECTOR3 getVolumeToWorld(VOLUME_DATA* pVolume, float x, float y, float z)
{
	float cx, cy, cz;

	cx = cy = cz = 0;

	cx = pVolume->getSpaceX()* x - pVolume->getSizeX()*0.5f;
	cy = pVolume->getSpaceY()* y - pVolume->getSizeY()*0.5f;
	cz = pVolume->getSpaceZ()* z - pVolume->getSizeZ()*0.5f;

	return mip::VECTOR3(cx, cy, cz);
}

/*
	TODO : getWorldToVolume 사이에서 중복 코드 합칠 것
*/
mip::VECTOR3 getWorldToVolume_PET(VOLUME_DATA_PET * pVolume, float x, float y, float z)
{
	mip::VECTOR3 baseVec_cm;
	mip::VECTOR3 LocalVec_cm;
	mip::VECTOR3 volumeWorld_cm;
	mip::VECTOR3 volumeVec;

	float baseX = -pVolume->getCX_cm() * 0.5f;
	float baseY = -pVolume->getCY_cm() * 0.5f;
	float baseZ = -pVolume->getCZ_cm() * 0.5f;

	baseVec_cm.set(baseX, baseY, baseZ);
	LocalVec_cm.set(x, y, z);

	volumeWorld_cm.set((LocalVec_cm - baseVec_cm));

	mip::VECTOR3 pixelSpacing_cm = pVolume->VolumeInfo()->GetPixelSpacing_cm();
	volumeVec.x = volumeWorld_cm.x / pixelSpacing_cm.x;
	volumeVec.y = volumeWorld_cm.y / pixelSpacing_cm.y;
	volumeVec.z = volumeWorld_cm.z / pixelSpacing_cm.z;

	/* 경계선 처리 진행 */
	if (volumeVec.x < 0)
		volumeVec.x = 0;
	else if (volumeVec.x >= pVolume->getCX())
		volumeVec.x = pVolume->getCX() - 1;

	if (volumeVec.y < 0)
		volumeVec.y = 0;
	else if (volumeVec.y >= pVolume->getCY())
		volumeVec.y = pVolume->getCY() - 1;

	if (volumeVec.z < 0)
		volumeVec.z = 0;
	else if (volumeVec.z >= pVolume->getCZ())
		volumeVec.z = pVolume->getCZ() - 1;

	/* 값을 반올림 */
	volumeVec.x = round(volumeVec.x);
	volumeVec.y = round(volumeVec.y);
	volumeVec.z = round(volumeVec.z);

	return volumeVec;
}

std::vector<mint16> getHU_List_World(VOLUME_DATA * pVolume, mip::VECTOR3 vLT_World, mip::VECTOR3 vRT_World, mip::VECTOR3 vLB_World, mip::VECTOR3 vRB_World)
{
	if (pVolume == nullptr)
	{
		return std::vector<mint16>();
	}

	mip::VECTOR3 volumeSpcaing_cm(
		pVolume->getSpaceX(),
		pVolume->getSpaceY(),
		pVolume->getSpaceZ());

	mip::VECTOR3 vDirectionX = (vRT_World - vLT_World);
	mip::VECTOR3 vDirectionY = (vLB_World - vLT_World);

	mip::VECTOR3 vDirectionX_normalized = (vRT_World - vLT_World).normalize();
	mip::VECTOR3 vDirectionY_normalized = (vLB_World - vLT_World).normalize();

	mip::VECTOR3 vStepX = vDirectionX_normalized * volumeSpcaing_cm.x;
	mip::VECTOR3 vStepY = vDirectionY_normalized * volumeSpcaing_cm.y;

	int countX = ceil(vDirectionX.length() / volumeSpcaing_cm.x);
	int countY = ceil(vDirectionY.length() / volumeSpcaing_cm.y);

	/* 최소값을 1로 설정하여 HU값 반환 */
	countX = max(1, countX);
	countY = max(1, countY);

	std::vector<mint16> HU_List;

	/* LT -> RB 좌표로 이동하면서 Volume 값 계산*/
	for (int y = 0; y < countY; ++y)
	{
		for (int x = 0; x < countX; ++x)
		{
			mip::VECTOR3 v_world = vLT_World + (y * vStepY) + (x * vStepX);
			mip::VECTOR3 v_volume = getWorldToVolume(pVolume, v_world.x, v_world.y, v_world.z);

			/* x,y,z 정수로 반올림 진행*/
			v_volume.x = round(v_volume.x);
			v_volume.y = round(v_volume.y);
			v_volume.z = round(v_volume.z);

			mint16 hu = pVolume->getData(v_volume.x, v_volume.y, v_volume.z);
			HU_List.push_back(hu);
		}
	}

	return HU_List;
}

std::vector<float> getSUV_List_Volume(VOLUME_DATA_PET * pVolume, WINDOW_TYPE type, mip::VECTOR3 vLT_volume, mip::VECTOR3 vRB_volume, bool flipX, bool flipY)
{
	std::vector<float> suv2DMap;

	int depth = 0;
	int startX = 0;
	int endX = 0;
	int startY = 0;
	int endY = 0;
	VOLUME_DATA_PET::EPlane plane = VOLUME_DATA_PET::PL_XY;

		//case WT_CORONAL:
		//	return QVector3D(pos.x(), m_depth, pos.y());
		//case WT_SAGITTAL:
		//	return QVector3D(m_depth, pos.x(), pos.y());

	if (type == WT_AXIAL)
	{
		depth = vLT_volume.z;
		startX = vLT_volume.x;
		startY = vLT_volume.y;
		endX = vRB_volume.x;
		endY = vRB_volume.y;
		plane = VOLUME_DATA_PET::PL_XY;
	}
	else if (type == WT_CORONAL)
	{
		depth = vLT_volume.y;
		startX = vLT_volume.x;
		startY = vLT_volume.z;
		endX = vRB_volume.x;
		endY = vRB_volume.z;
		plane = VOLUME_DATA_PET::PL_XZ;
	}
	else if (type == WT_SAGITTAL)
	{
		depth = vLT_volume.x;
		startX = vLT_volume.y;
		startY = vLT_volume.z;
		endX = vRB_volume.y;
		endY = vRB_volume.z;
		plane = VOLUME_DATA_PET::PL_YZ;
	}
	else
	{
		//INVALID
	}

	int startX_Result = min(startX, endX);
	int startY_Result = min(startY, endY);
	int endX_Result = max(startX, endX);
	int endY_Result = max(startY, endY);

	pVolume->get2DSUVMapRange(suv2DMap, depth, startX_Result, startY_Result, endX_Result, endY_Result, plane, flipX, flipY);

	return suv2DMap;
}
