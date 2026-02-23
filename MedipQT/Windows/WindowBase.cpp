#include "stdafx.h"
#include "WindowBase.h"
#include "windowManager.h"

#include "System/resourceManager.h"
#include "System/stringManager.h"
#include "System/VolumeCalculator.h"
#include "System/styleManager.h"

#include "Actions/ActionManager.h"

#include "Windows/Main/MainSegmentWidget.h"
#include "Windows/Main/MainTAWidget.h"
#include "Windows/Tab/AISegTabDeepCatch.h"

#include "graphics/anotation.h"

#include "Dialogs/AnnoTextDlg.h"
#include "Dialogs/AnnoControlDlg.h"
#include "Dialogs/AnnoPathUsageDlg.h"
#include "Dialogs/DrawcutDlg.h"
#include "Dialogs/SelectSliceDlg.h"

#include "Tabwindow.h"

#include "UI/MaskList.h"

#include "algorithm\FeatureExtractor.h"
#include "algorithm/MagicCut.h"

#include "define.h"
#include "MedipQT.h"
#include "MeshControl.h"

#include "DataContext.h"

bool WindowBase::m_bWorkMode = false;

WindowBase::WindowBase(QWidget* parent, MPRViewSharedInfo* pViewerSharedInfo)
	: OpenGLWidget(parent)
	, m_pDataContext(nullptr)
	, m_rootWidget(nullptr)
	, m_btnFullScreen(0)
	, m_mouseDown(false)
	, m_focus(false)
	, m_fullscreen(false)
	, m_preMousePos(0, 0)
	, m_MousePos(0, 0)
	, m_LbuttonDown(false)
	, m_MbuttonDown(false)
	, m_RbuttonDown(false)
	, m_slidebar(0)
	, m_btnShowBounding(0)
	, m_btnBoundaryEdge(0)
	, m_pContextMenu(nullptr)
	, m_pActSeedPoint(nullptr)
	, m_pActSeedShow(nullptr)
	, m_pActAnnoList(nullptr)
	, m_pActSeedApply(nullptr)
	, m_pActRangeApply(nullptr)
	, m_pActShowAniPoint(nullptr)
	, m_pActShowAnnoText(nullptr)
	, m_pViewerSharedInfo(pViewerSharedInfo)
	, m_bUpdateMeshOutline(false)
{
	m_bWorkMode = false;

	m_latestClickDepth = m_depth = 50;
	m_preDepth = 0;
	m_zoomFactorX = 1.0f;
	m_zoomFactorY = 1.0f;
	m_fZoomStepX = 1.0f;
	m_fZoomStepY = 1.0f;
	m_SlicePosition = QPoint(0, 0);
	m_ctrl = false;
	m_alt = false;
	m_shift = false;
	m_mouse_action_mode = MAM_NONE;
	m_pre_mouse_action_mode = MAM_NONE;
	m_flipY = true;
	m_flipX = false;
	m_showRightMenu = false;
	m_polyProcessCheck = false;
	m_colorData = nullptr;
	m_maskData = nullptr;
	m_dutyCheck = false;
	m_cursor = Qt::ArrowCursor;
	m_annoEditMode = false;

	m_nIconSize = WIN_MANAGER->mainWindow->IconSize;

	m_pContextMenu = new QMenu(this);

	//#if !defined(COVID19_VER)
	m_pActSeedPoint = PRODUCT_FACTORY->createObject<QAction>(MFL_Common_Segmentation_Threshold_Thresholdbasic, this);
	if (m_pActSeedPoint)
	{
		m_pActSeedPoint->setText(STRING_MANAGER->getString(STR_SEED_POINT));
		connect(m_pActSeedPoint, &QAction::triggered, this, &WindowBase::slot_OnContextSeed);
	}

	m_pActSeedShow = PRODUCT_FACTORY->createObject<QAction>(MFL_Common_Segmentation_Threshold_Thresholdbasic, this);
	if (m_pActSeedShow)
	{
		m_pActSeedShow->setText(tr("-"));
		connect(m_pActSeedShow, &QAction::triggered, this, &WindowBase::slot_OnContextSeedShow);
	}

	m_pActSeedApply = PRODUCT_FACTORY->createObject<QAction>(MFL_Common_Segmentation_Threshold_Thresholdbasic, this);
	if (m_pActSeedApply)
	{
		m_pActSeedApply->setText(STRING_MANAGER->getString(STR_SEED_APPLY));
		connect(m_pActSeedApply, &QAction::triggered, this, &WindowBase::slot_OnContextSeedApply);
	}

	m_pActRangeApply = PRODUCT_FACTORY->createObject<QAction>(MFL_Common_Segmentation_Regiongrowing_ModeHURange, this);
	if (m_pActRangeApply)
	{
		m_pActRangeApply->setText(STRING_MANAGER->getString(STR_ALL_SELECT));
		connect(m_pActRangeApply, &QAction::triggered, this, &WindowBase::slot_OnContextRangeApply);
	}

	m_pActShowAniPoint = PRODUCT_FACTORY->createObject<QAction>(MFL_Common_Annotation_PathAnimation, this);
	if (m_pActShowAniPoint)
	{
		m_pActShowAniPoint->setText(STRING_MANAGER->getString(STR_SHOW_ANI_POINT));
		connect(m_pActShowAniPoint, &QAction::triggered, this, &WindowBase::slot_OnContextAniPoints);
	}

	//#endif
	m_pActAnnoList = new QAction(STRING_MANAGER->getString(STR_ANNO_LIST), this);
	m_pActShowAnnoText = new QAction(STRING_MANAGER->getString(STR_HIDE_ANNO), this);

	connect(m_pActAnnoList, &QAction::triggered, this, &WindowBase::slot_OnContextAnnoList);
	connect(m_pActShowAnnoText, &QAction::triggered, this, &WindowBase::slot_OnContextAnnoText);

	setAcceptDrops(true);
	setContextMenu();
	setMouseTracking(true);

	//	installEventFilter(this);

	m_WheelTimer = new QTimer(this);

	if (m_WheelTimer)
	{
		m_WheelTimer->stop();
		m_WheelTimer->setSingleShot(true);
		connect(m_WheelTimer, SIGNAL(timeout()), this, SLOT(slot_OnWheelTimeout()));
	}

	m_pHoverWidget = nullptr;

#ifdef SEGMENTATION_COLOR_CODING

	if (m_pColorTable == nullptr)
	{
		m_pColorTable = new ColorTable(eColorGradientPostion::eLeftPos, this);
		m_pColorTable->setFixedSize(m_pColorTable->getWidgetSize());
		m_pColorTable->setStops();
		m_pColorTable->setGradientImage();
		m_pColorTable->hide();
	}
	m_heightMapColorTable = m_pColorTable->getHeightMapColorTable();
#endif

	m_pDataContext = DATA_CONTEXT;
}

WindowBase::~WindowBase()
{
	resetResource();
}

void WindowBase::resetResource()
{
	SAFE_DELETES(m_colorData);
	SAFE_DELETES(m_maskData);
}

void WindowBase::syncFullscreen()
{
	slot_OnFullScreen();
}

void WindowBase::InitWheelSliderFunc(bool set)
{
	if (m_pDataContext == nullptr)
	{
		return;
	}

	m_slidebar->blockSignals(true);
	if (set)//wheel - zoom
	{
		if (m_windowType == WT_AXIAL)
		{
			m_slidebar->setMinimum(0);
			m_slidebar->setMaximum(m_pDataContext->volume_data.getCZ() - 1);
#ifdef USE_RIGHT_HAND
			m_slidebar->setValue((m_pDataContext->volume_data.getCZ() - 1) - m_depth);
#else
			m_slidebar->setValue(m_depth);
#endif
		}
		else if (m_windowType == WT_CORONAL)
		{
			m_slidebar->setMinimum(0);
			m_slidebar->setMaximum(m_pDataContext->volume_data.getCY() - 1);
			m_slidebar->setValue(m_depth);
		}
		else if (m_windowType == WT_SAGITTAL)
		{
			m_slidebar->setMinimum(0);
			m_slidebar->setMaximum(m_pDataContext->volume_data.getCX() - 1);
			m_slidebar->setValue(m_depth);
		}
	}
	else
	{
		int nCurVal = qRound(m_zoomFactorX / m_fZoomStepX);

		m_slidebar->setMinimum(m_nMinZoomNum + 1);
		m_slidebar->setMaximum(m_nZoomStepCnt);
		m_slidebar->setValue(nCurVal);

	}
	m_slidebar->blockSignals(false);
}

void WindowBase::init(DataContext* pDataContext)
{
	SAFE_DELETES(m_colorData);
	SAFE_DELETES(m_maskData);

	m_pDataContext = pDataContext;

	unsigned int c_x;
	unsigned int c_y;
	unsigned int c_depth;
	m_pDataContext->volume_data.getLengthForScreen(m_windowType, c_x, c_y, c_depth);

	m_preDepth = m_depth + 1;
	m_dutyCheck = true;

	if (c_x > 0 && c_y)
	{
		m_colorData = new COLOR[c_x * c_y];
		m_maskData = new COLOR[c_x * c_y];
	}
}

bool WindowBase::isFullScreen()
{
	return m_fullscreen;
}

void WindowBase::setFullScreenDelegate(QWidget* root)
{
	m_rootWidget = root;
}

QImage WindowBase::getCaptureImage(bool mode, bool type)
{
	unsigned int c_x;
	unsigned int c_y;
	unsigned int c_depth;
	m_pDataContext->volume_data.getLengthForScreen(m_windowType, c_x, c_y, c_depth);

	QRect pos = getPosition();
	QImage sliceImg((uchar*)m_colorData, c_x, c_y, QImage::Format_RGBA8888);

	//TODO:: data & ROI
	QImage img = QImage(this->width(), this->height(), QImage::Format_RGBA8888);
	QPainter p(&img);
	p.fillRect(QRect(0, 0, this->width(), this->height()), Qt::black);
	p.drawImage(pos, sliceImg);

	if (WIN_MANAGER->bThrePreview)
		drawPreview(&p);
	else
	{
		if (m_maskData)
		{
			QImage ROIImg((uchar*)m_maskData, c_x, c_y, QImage::Format_RGBA8888);

			p.drawImage(pos, ROIImg);
		}
	}

	if (type)
	{
		//TODO:: draw coord, annotation, etc,,,
		drawAnnotations(&p);

		if (WIN_MANAGER->getDirectionMode())
		{
			drawDirection(5, &p);
		}

		QFontMetrics fontMet(this->font());
		QVector3D vec = screenToVolumePosition3(&(m_MousePos));
		checkVolumeArea(vec);
		mint16 HU = getData(m_MousePos, m_depth);
		QString str = QString("Coord(%1,%2,%3) %5(%4)").arg(vec.x()).arg(vec.y()).arg(vec.z()).arg(HU).arg(WIN_MANAGER->getUnitString());
		QRect rect = fontMet.boundingRect(str);
		p.setPen(Qt::white);
		p.drawText(10, rect.height(), str);

		drawBoundLine(&p);
		drawCoordLine(&p);

		muint32 downscaledCnt = m_pDataContext->volume_data.getDownScaledCnt();
		int downscaleHeight = rect.height();
		if (downscaledCnt > 0)
		{
			str = QString("1/%1 Downscale").arg(downscaledCnt == 1 ? 2 : 4);
			rect = fontMet.boundingRect(str);
			downscaleHeight += rect.height() + 5;
			p.drawText(10, downscaleHeight, str);
		}
	}

	if (mode)	//specific window
	{
		p.end();
		return img;
	}
	else
	{
		p.end();
		int w, h;

		w = abs(m_polyLine.at(0).x() - m_polyLine.at(1).x());
		w++;

		h = abs(m_polyLine.at(0).y() - m_polyLine.at(1).y());
		h++;

		if ((1 >= w) && (1 >= h))
		{
			return QImage();
		}

		QRect rect;

		QPoint leftTop = QPoint(m_polyLine.at(0));
		QPoint rightBottom = QPoint(m_polyLine.at(1));

		int tmpPoint;

		if (leftTop.x() > rightBottom.x())
		{
			tmpPoint = leftTop.x();
			leftTop.setX(rightBottom.x());
			rightBottom.setX(tmpPoint);
		}
		if (leftTop.y() > rightBottom.y())
		{
			tmpPoint = leftTop.y();
			leftTop.setY(rightBottom.y());
			rightBottom.setY(tmpPoint);
		}


		if (leftTop.x() < 0 || (leftTop.x() >= this->width()))
		{
			int nX = leftTop.x() > 0 ? (this->width() - 1) : 0;
			leftTop.setX(nX);
		}
		if (rightBottom.x() < 0 || (rightBottom.x() >= this->width()))
		{
			int nX = rightBottom.x() > 0 ? (this->width() - 1) : 0;
			rightBottom.setX(nX);
		}

		if (leftTop.y() < 0 || (leftTop.y() >= this->height()))
		{
			int nY = leftTop.y() > 0 ? (this->height() - 1) : 0;
			leftTop.setY(nY);
		}
		if (rightBottom.y() < 0 || (rightBottom.y() >= this->height()))
		{
			int nY = rightBottom.y() > 0 ? (this->height() - 1) : 0;
			rightBottom.setY(nY);
		}

		rect.setCoords(leftTop.x(), leftTop.y(), rightBottom.x(), rightBottom.y());

		w = rect.width();
		h = rect.height();

		if ((w <= 1) && (h <= 1))
			return QImage();

		QImage rectImg = QImage(w, h, QImage::Format_RGBA8888);
		QPainter p2(&rectImg);
		p2.drawImage(0, 0, img.copy(rect));
		p2.end();

		//	SAFE_DELETE(img);

		return rectImg;
	}
}

void WindowBase::render(QPainter* p)
{
	if (p == nullptr)
	{
		return;
	}
}

void WindowBase::renderHeatMap(QPainter* p)
{
	if (m_pDataContext->volume_data.m_vecAIResultData.empty())
	{
		return;
	}

	AISetTab* pAISettab = WIN_MANAGER->GetTab()->getAITab()->m_tabSet;
	if (pAISettab == nullptr || pAISettab->getColorMapState() != Qt::Checked)
	{
		m_pColorTable->hide();
		m_pColorTable->clearCategoryMap();

		return;
	}

	MaskInfo* pMaskInfo = m_pDataContext->volume_data.getCurrentMaskInfo();
	int nIdx = -1;
	for (int i = 0; i < m_pDataContext->volume_data.m_vecAIResultData.size(); ++i)
	{
		if (m_pDataContext->volume_data.m_vecAIResultData[i].first == pMaskInfo->uid)
		{
			nIdx = i;
			break;
		}
	}

	if (nIdx == -1)
	{
		return;
	}

	std::vector<std::pair<float, float>> vCategoryValue;
	vCategoryValue.push_back(make_pair<float, float>(0.0, 1.0));
	vCategoryValue.push_back(make_pair<float, float>(0.25, 0.75));
	vCategoryValue.push_back(make_pair<float, float>(0.5, 0.5));
	vCategoryValue.push_back(make_pair<float, float>(0.75, 0.25));
	vCategoryValue.push_back(make_pair<float, float>(1.0, 0.0));

	m_pColorTable->setGradientCategoryValue(vCategoryValue);
	m_pColorTable->show();

	unsigned int c2d_width;
	unsigned int c2d_height;
	unsigned int c2d_depth;

	m_pDataContext->volume_data.getLengthForScreen(m_windowType, c2d_width, c2d_height, c2d_depth);

	int count = 1;
	// 체크박스 켰을 때 처음 한번만 메모리 할당으로 바꿀 것
	//unsigned char* outputVolume = new unsigned char[c2d_width * c2d_height * count];
	//memset(outputVolume, 0, sizeof(char)*c2d_width * c2d_height * count);

	std::vector<unsigned char> vec3DVolume(c2d_width * c2d_height * count, 0);

	// outputVolume 데이터 0~1
	recon2DMap(m_pDataContext->volume_data.m_vecAIResultData[nIdx].second, vec3DVolume, c2d_width, c2d_height, c2d_depth);
	// 
	QSize tableSize = m_heightMapColorTable.size();

	QImage image(c2d_width, c2d_height, QImage::Format_RGBA8888);

	// make heatmap
	for (int j = 0; j < c2d_height; j++)
	{
		for (int i = 0; i < c2d_width; i++)
		{
			if (vec3DVolume[j * c2d_width + i] == 0)
			{
				image.setPixel(i, j, 0);
			}
			else
			{
				int tablePos = vec3DVolume[j * c2d_width + i] / 256.f * (tableSize.height());
				uint color = m_heightMapColorTable.pixel(QPoint(0, tablePos));
				image.setPixel(i, j, color);
			}
		}
	}

	//	SAFE_DELETES(outputVolume);
	/////////////////////////////////

	p->drawImage(getPosition(), image);
}

void WindowBase::recon2DMap(std::vector<unsigned char>& vecInData, std::vector<unsigned char>& vecOutData, unsigned int c2d_width, unsigned int c2d_height, unsigned int c2d_depth)
{
	unsigned int c_z;

	int nCX = m_pDataContext->volume_data.getCX();
	int nCY = m_pDataContext->volume_data.getCY();
	int nCZ = m_pDataContext->volume_data.getCZ();

	int n3D_X_coeff = 0, n3D_Y_coeff = 0, n3D_Z_coeff = 0, dx = 0, dy = 0;
	auto nInitX = 0, nInitY = 0, nExitX = 0, nExitY = 0;

	if (m_windowType == WT_AXIAL)
	{
		n3D_X_coeff = 1;
		n3D_Y_coeff = nCX;
		n3D_Z_coeff = nCY * nCX;
		nInitX = 0, nInitY = 0, nExitX = c2d_width, nExitY = c2d_height;
		dx = dy = 1;
		//c_z = c2d_depth - m_slidebar->value() - 1;
		c_z = getDepth();
	}
	else if (m_windowType == WT_CORONAL)
	{
		n3D_X_coeff = 1;
		n3D_Y_coeff = nCY * nCX;	// 방향 반대
		n3D_Z_coeff = nCX;
		nInitX = 0, nInitY = c2d_height - 1, nExitX = c2d_width, nExitY = 0;
		dx = 1, dy = -1;
		//c_z = m_slidebar->value();
		c_z = getDepth();
	}
	else if (m_windowType == WT_SAGITTAL)
	{
		n3D_X_coeff = nCX;	// 방향반대
		n3D_Y_coeff = nCY * nCX;		// 방향반대
		n3D_Z_coeff = 1;
		nInitX = c2d_width - 1, nInitY = c2d_height - 1, nExitX = 0, nExitY = 0;
		dx = -1;
		dy = -1;
		//c_z = m_slidebar->value();
		c_z = getDepth();
	}

	for (int y = nInitY, y2d = 0; y < c2d_height; y += dy, ++y2d)
	{
		for (int x = nInitX; x < c2d_width; x += dx)
		{
			int index3D = c_z * n3D_Z_coeff + y * n3D_Y_coeff + x * n3D_X_coeff;
			int index2D = y2d * c2d_width + x;
			//p2DMap[index2D] = (WIN_MANAGER->m_pRadiomicsResult[index3D] + 1.f)*0.5f;
			//p2DMap[index2D] = m_pDataContext->volume_data.m_vec2dClassData[nIdx].second[index3D];
			vecOutData[index2D] = vecInData[index3D];
		}
	}
}

void WindowBase::mousePressEvent(QMouseEvent* e)
{
	WIN_MANAGER->setMoveFocus(true);
	//	this->setFocus();

	m_globalPos = e->globalPos();
	m_MousePos = e->pos();
	m_preMousePos = e->pos();
	m_bMoveMouse = false;

	if (m_pDataContext->volume_data.isValidate() == false)
		return;

	if (m_mouseDown == false)
	{
		m_mouseDown = true;

		switch (e->buttons())
		{
		case Qt::LeftButton:
			m_LbuttonDown = true;
			break;
		case Qt::MiddleButton:
			m_MbuttonDown = true;
			break;
		case Qt::RightButton:
			m_RbuttonDown = true;
			break;
		default:
			break;
		}

		if (m_LbuttonDown && m_mouse_action_mode != MAM_NONE && m_mouse_action_mode < MAM_BOUNDING_MOVE)
		{
			if (m_mouse_action_mode & MAM_BOUNDING_MAXX_OVER)
			{
				m_mouse_action_mode &= ~MAM_BOUNDING_MAXX_OVER;
				m_mouse_action_mode |= MAM_BOUNDING_MAXX_MOVE;
			}
			if (m_mouse_action_mode & MAM_BOUNDING_MINX_OVER)
			{
				m_mouse_action_mode &= ~MAM_BOUNDING_MINX_OVER;
				m_mouse_action_mode |= MAM_BOUNDING_MINX_MOVE;
			}
			if (m_mouse_action_mode & MAM_BOUNDING_MAXY_OVER)
			{
				m_mouse_action_mode &= ~MAM_BOUNDING_MAXY_OVER;
				m_mouse_action_mode |= MAM_BOUNDING_MAXY_MOVE;
			}
			if (m_mouse_action_mode & MAM_BOUNDING_MINY_OVER)
			{
				m_mouse_action_mode &= ~MAM_BOUNDING_MINY_OVER;
				m_mouse_action_mode |= MAM_BOUNDING_MINY_MOVE;
			}

			m_pDataContext->volume_data.setPreBoundingBox();
		}
	}

	if (m_mouseDown && m_LbuttonDown)
	{
		if (m_pViewerSharedInfo)
		{
			//m_pViewerSharedInfo->VolumePosition = screenToCoord3(&e->pos());
			m_pViewerSharedInfo->VolumeVoxelPosition = screenToVolumePosition3(&e->pos());
			m_pViewerSharedInfo->HU = getData(e->pos(), m_depth);
			m_pViewerSharedInfo->winType = m_windowType;
			m_pViewerSharedInfo->depth = m_depth;
		}
	}
}

void WindowBase::mouseMoveEvent(QMouseEvent* e)
{
	m_bMoveMouse = true;
	m_preMousePos = e->pos();

	if (m_mouseDown && m_LbuttonDown)
	{
		if (m_pViewerSharedInfo)
		{
			m_pViewerSharedInfo->VolumeVoxelPosition = screenToVolumePosition3(&e->pos());
			m_pViewerSharedInfo->HU = getData(e->pos(), m_depth);
		}
	}
}

void WindowBase::mouseReleaseEvent(QMouseEvent* e)
{
	if (e->modifiers() & Qt::ShiftModifier)
		m_shift = true;
	else
		m_shift = false;

	if (m_LbuttonDown)
	{
		if (m_mouse_action_mode > MAM_BOUNDING_MOVE)
		{
			BoundingBoxI box = m_pDataContext->volume_data.getBoundingBox();

			if (m_mouse_action_mode & MAM_BOUNDING_MAXX_MOVE)
			{
				if (box.getMinX(m_windowType) + 1 >= box.getMaxX(m_windowType))
					box.setMaxX(m_windowType, box.getMinX(m_windowType) + 1);
			}

			if (m_mouse_action_mode & MAM_BOUNDING_MINX_MOVE)
			{
				if (box.getMaxX(m_windowType) - 1 <= box.getMinX(m_windowType))
					box.setMinX(m_windowType, box.getMaxX(m_windowType) - 1);
			}

			if (m_mouse_action_mode & MAM_BOUNDING_MAXY_MOVE)
			{
				if (box.getMinY(m_windowType) + 1 >= box.getMaxY(m_windowType))
					box.setMaxY(m_windowType, box.getMinY(m_windowType) + 1);
			}

			if (m_mouse_action_mode & MAM_BOUNDING_MINY_MOVE)
			{
				if (box.getMaxY(m_windowType) - 1 <= box.getMinY(m_windowType))
					box.setMinY(m_windowType, box.getMaxY(m_windowType) - 1);
			}

			box.validateCheck();

			ACTION_MANAGER->action_BoundingBox_Modify(box);
		}
	}

	m_LbuttonDown = false;
	m_RbuttonDown = false;
	m_MbuttonDown = false;
	m_mouseDown = false;
	m_annoEditMode = false;
	m_mouse_action_mode = MAM_NONE;
	m_pre_mouse_action_mode = MAM_NONE;
}

void WindowBase::mouseDoubleClickEvent(QMouseEvent* e)
{
	if (e->buttons() & Qt::LeftButton)
	{
		WIN_MANAGER->setCoordType(REDRAW_TYPE);
		WIN_MANAGER->renderLater_GridView(false);
	}
}

void WindowBase::setShowBoundingBoxMode(bool value)
{
	static QIcon boundPress = RESOURCE_MANAGER->getIcon(ICON_VIEW_COORD, m_nIconSize, m_nIconSize);
	static QIcon boundLeave = RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_COORD, m_nIconSize, m_nIconSize);

	WIN_MANAGER->setShowClippingBox(value);

	if (m_btnShowBounding)
	{
		m_btnShowBounding->setChecked(value);

		if (value)
			m_btnShowBounding->setIcon(boundPress);
		else
			m_btnShowBounding->setIcon(boundLeave);
	}

	WIN_MANAGER->renderLater_GridView(true);
}

void WindowBase::setBoundingBoxEdgeMode(bool value)
{
	static QIcon edgePress = RESOURCE_MANAGER->getIcon(ICON_VIEW_BOUNARY, m_nIconSize, m_nIconSize);
	static QIcon edgeLeave = RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_BOUNDARY, m_nIconSize, m_nIconSize);

	WIN_MANAGER->setEdgeMode(value);
	WIN_MANAGER->setBoundaryMode(value);

	if (m_btnBoundaryEdge)
	{
		m_btnBoundaryEdge->setChecked(value);

		if (value)
			m_btnBoundaryEdge->setIcon(edgePress);
		else
			m_btnBoundaryEdge->setIcon(edgeLeave);
	}

	WIN_MANAGER->forceUpdate2DViewData(false, true);
	WIN_MANAGER->renderLater_GridView(false);
}

void WindowBase::focusInEvent(QFocusEvent* e)
{
	m_focus = true;
	renderLater();
}

void WindowBase::focusOutEvent(QFocusEvent*)
{
	m_focus = false;

	renderLater();
}

void WindowBase::keyPressEvent(QKeyEvent* e)
{
	m_key = e->text();

	bool chkRender = false;

	checkModifiers(e, true, &chkRender);

	if (chkRender)
		renderLater();
}

void WindowBase::keyReleaseEvent(QKeyEvent* e)
{
	bool chkRender = false;
	checkModifiers(e, false, &chkRender);
	bool chkRG = false;

	if (WIN_MANAGER->getRGType() == LAYER_RG_CTRL)
	{
		if (!m_ctrl)
			chkRG = true;
	}
	else if (WIN_MANAGER->getRGType() == LAYER_RG_ALT)
	{
		if (!m_alt)
			chkRG = true;
	}
	else if (WIN_MANAGER->getRGType() == LAYER_RG_SHIFT)
	{
		if (!m_shift)
			chkRG = true;
	}

	if (chkRG && WIN_MANAGER->getRGLock())
	{
		if ((!WIN_MANAGER->layerRGLocation.isEmpty()))
		{
			if (WIN_MANAGER->GetTab())
			{
				ThreSholdTab* tab = WIN_MANAGER->GetTab()->getThreSholdTab();

				if (tab)
				{
					tab->ApplyRegionGrowing();
					if (chkRender)chkRender = false;
				}
			}
			if (WIN_MANAGER->getRGDrawLock(true))
			{
				WIN_MANAGER->layerRGLocation.clear();
				WIN_MANAGER->setRGDrawUnlock();
			}
		}
		WIN_MANAGER->setRGUnlock();
	}

	if (e->key() == Qt::Key_Space)
		slot_OnFullScreen();

	if (chkRender)
		renderLater();
}

void WindowBase::resizeEvent(QResizeEvent* event)
{
	if (m_pDataContext == nullptr)
	{
		return;
	}

	if (event == nullptr)
	{
		return;
	}

	int width = event->size().width();
	int height = event->size().height();

	updateScreenMenu(width, height);

	if (m_pColorTable)
	{
		QSize size = m_pColorTable->getWidgetSize();
		m_pColorTable->move(0, (height - size.height()) / 2);
	}

	if (m_windowType == WT_AXIAL)
	{
		InitZoomFactor(m_pDataContext->volume_data.getSpaceX(true), m_pDataContext->volume_data.getSpaceY(true), true);
	}
	else if (m_windowType == WT_CORONAL)
	{
		InitZoomFactor(m_pDataContext->volume_data.getSpaceX(true), m_pDataContext->volume_data.getSpaceZ(true), true);
	}
	else if (m_windowType == WT_SAGITTAL)
	{
		InitZoomFactor(m_pDataContext->volume_data.getSpaceY(true), m_pDataContext->volume_data.getSpaceZ(true), true);
	}
}

uint WindowBase::getDepth(bool flip)
{
	if (flip == false)
	{
		return m_depth;
	}
	else
	{
		switch (m_windowType)
		{
		case WT_CORONAL:
			return m_pDataContext->volume_data.getCY() - m_depth - 1;
		case WT_SAGITTAL:
			return m_pDataContext->volume_data.getCX() - m_depth - 1;
		case WT_AXIAL:
		default:
			return m_pDataContext->volume_data.getCZ() - m_depth - 1;
		}
	}
}

uint WindowBase::getLatestDepth()
{
	return m_latestClickDepth;
}

uint WindowBase::getAniDepth(int index, bool isLine, bool isSpiral)
{
	if (WIN_MANAGER->aniLine.count() <= 0)
	{
		return -1;
	}

	if (isLine)
	{
		bool bSpiral = isSpiral;
		QVector<QVector3D>* Lines = bSpiral ? &(WIN_MANAGER->spiralLine) : &(WIN_MANAGER->aniLine);

		if (index >= Lines->count())
		{
			return -1;
		}

		switch (m_windowType)
		{
		case WT_CORONAL:
			return Lines->at(index).y();
		case WT_SAGITTAL:
			return Lines->at(index).x();
		case WT_AXIAL:
		default:
			return Lines->at(index).z();
		}
	}
	else
	{
		switch (m_windowType)
		{
		case WT_CORONAL:
			return WIN_MANAGER->LinePos.y();
		case WT_SAGITTAL:
			return WIN_MANAGER->LinePos.x();
		case WT_AXIAL:
		default:
			return WIN_MANAGER->LinePos.z();
		}
	}
}

float WindowBase::getfDepth(bool flip)
{
	if (flip == false)
	{
		return m_fdepth;
	}
	else
	{
		switch (m_windowType)
		{
		case WT_CORONAL:
			return m_pDataContext->volume_data.getCY() - m_fdepth - 1;
		case WT_SAGITTAL:
			return m_pDataContext->volume_data.getCX() - m_fdepth - 1;
		case WT_AXIAL:
		default:
			return m_pDataContext->volume_data.getCZ() - m_fdepth - 1;
		}
	}
}

void WindowBase::setDepth(uint depth)
{
	switch (m_windowType)
	{
	case WT_CORONAL:
		if ((m_pDataContext->volume_data.getCY() - 1) < depth)
			depth = m_pDataContext->volume_data.getCY() - 1;
		break;
	case WT_SAGITTAL:
		if ((m_pDataContext->volume_data.getCX() - 1) < depth)
			depth = m_pDataContext->volume_data.getCX() - 1;
		break;
	case WT_AXIAL:
		if ((m_pDataContext->volume_data.getCZ() - 1) < depth)
			depth = m_pDataContext->volume_data.getCZ() - 1;
		break;
	}

	if (depth < 0)
	{
		depth = 0;
	}

	if (m_depth != depth)
	{
		m_depth = depth;

		m_pDataContext->volume_data.setDepth(m_windowType, depth);

		m_dutyCheck = true;

		WIN_MANAGER->invisibleModeUpdate(m_windowType);
	}
}

void WindowBase::setfDepth(float depth)
{
	switch (m_windowType)
	{
	case WT_CORONAL:
		if ((m_pDataContext->volume_data.getCY() - 1) < depth) depth = m_pDataContext->volume_data.getCY() - 1;
		break;
	case WT_SAGITTAL:
		if ((m_pDataContext->volume_data.getCX() - 1) < depth) depth = m_pDataContext->volume_data.getCX() - 1;
		break;
	case WT_AXIAL:
	default:
		if ((m_pDataContext->volume_data.getCZ() - 1) < depth) depth = m_pDataContext->volume_data.getCZ() - 1;
		break;
	}

	if (depth < 0.0)
	{
		depth = 0;
	}

	m_fdepth = depth;
}

void WindowBase::updatePlaneDatas()
{

}

void WindowBase::updateSliderPosition()
{
	if (m_slidebar)
	{
		if (WIN_MANAGER->getWheelZoom())
		{
			InitWheelSliderFunc(true);
		}
		else
		{
			InitWheelSliderFunc(false);
		}
	}
}

WINDOW_TYPE WindowBase::getType()
{
	return m_windowType;
}

void WindowBase::setType(WINDOW_TYPE type)
{
	m_windowType = type;
}

QRect WindowBase::getPosition()
{
	unsigned int c_x;
	unsigned int c_y;
	unsigned int c_depth;
	m_pDataContext->volume_data.getLengthForScreen(m_windowType, c_x, c_y, c_depth);

	QPoint scaled(qRound(float(c_x) * m_zoomFactorX), qRound(float(c_y) * m_zoomFactorY));
	int w = this->size().width();
	int h = this->size().height();
	QPoint imageZero = (QPoint(this->size().width(), this->size().height()) / 2) - (scaled / 2);

	return QRect(imageZero.x() + m_SlicePosition.x(), imageZero.y() + m_SlicePosition.y(), scaled.x(), scaled.y());
}

QPoint WindowBase::screenToVolumePosition(const QPoint* point)
{
	unsigned int c_x;
	unsigned int c_y;
	unsigned int c_depth;
	m_pDataContext->volume_data.getLengthForScreen(m_windowType, c_x, c_y, c_depth);

	QPointF p = QPointF(m_flipX ? size().width() - point->x() - 1 : point->x(), m_flipY ? size().height() - point->y() - 1 : point->y());
	QPointF scaled(float(c_x) * m_zoomFactorX, float(c_y) * m_zoomFactorY);
	QPointF imageZero = (QPointF(this->size().width(), this->size().height()) / 2) - (scaled / 2);

	p = ((p - imageZero) - QPointF(m_flipX ? -m_SlicePosition.x() : m_SlicePosition.x(), m_flipY ? -m_SlicePosition.y() : m_SlicePosition.y()));
	p.setX(p.x() / m_zoomFactorX);
	p.setY(p.y() / m_zoomFactorY);

	QPoint r = QPoint(qFloor(p.x()), qFloor(p.y()));

	if (r.x() < 0)
		r.setX(0);
	else if (r.x() >= c_x)
		r.setX(c_x - 1);

	if (r.y() < 0)
		r.setY(0);
	else if (r.y() >= c_y)
		r.setY(c_y - 1);

	return r;
}

QPointF WindowBase::screenToVolumePositionF(const QPointF* point)
{
	unsigned int c_x;
	unsigned int c_y;
	unsigned int c_depth;
	m_pDataContext->volume_data.getLengthForScreen(m_windowType, c_x, c_y, c_depth);

	QPointF p = QPointF(m_flipX ? size().width() - point->x() - 1 : point->x(), m_flipY ? size().height() - point->y() - 1 : point->y());
	QPointF scaled(float(c_x) * m_zoomFactorX, float(c_y) * m_zoomFactorY);
	QPointF imageZero = (QPointF(this->size().width(), this->size().height()) / 2) - (scaled / 2);

	p = ((p - imageZero) - QPointF(m_flipX ? -m_SlicePosition.x() : m_SlicePosition.x(), m_flipY ? -m_SlicePosition.y() : m_SlicePosition.y()));
	p.setX(p.x() / m_zoomFactorX);
	p.setY(p.y() / m_zoomFactorY);

	return p;
}

QVector3D WindowBase::screenToVolumePosition3(const QPoint* point)
{
	QPoint pos = screenToVolumePosition(point);

	switch (m_windowType)
	{
	case WT_CORONAL:
		return QVector3D(pos.x(), m_depth, pos.y());
	case WT_SAGITTAL:
		return QVector3D(m_depth, pos.x(), pos.y());
	case WT_AXIAL:
		return QVector3D(pos.x(), pos.y(), m_depth);
	default:
		return QVector3D(0, 0, 0);
	}
}

QVector3D WindowBase::screenToCoord3(const QPoint* point)
{
	QVector3D vec = screenToVolumePosition3(point);

	vec.setZ(m_pDataContext->volume_data.getCZ() - (vec.z() + 1));

	// 0~cx-1 => 1~cx  : 범위표시 변경
	vec.setX(vec.x() + 1);
	vec.setY(vec.y() + 1);
	vec.setZ(vec.z() + 1);


	return vec;
}

void WindowBase::checkVolumeArea(QVector3D& area)
{
	unsigned int c_x = m_pDataContext->volume_data.getCX();
	unsigned int c_y = m_pDataContext->volume_data.getCY();
	unsigned int c_z = m_pDataContext->volume_data.getCZ();
	//	m_pDataContext->volume_data.getLengthForScreen(m_windowType, c_x, c_y, c_z);
	if (area.x() > c_x)
		area.setX(c_x);
	if (area.y() > c_y)
		area.setY(c_y);
	if (area.z() > c_z)
		area.setZ(c_z);

	if (area.x() < 0)
		area.setX(0);
	if (area.y() < 0)
		area.setY(0);
	if (area.z() < 0)
		area.setZ(0);
}

QVector3D WindowBase::volumeToScreenPosition(int volx, int voly, int volz, bool scale)
{
	QVector3D vec;
	unsigned int c_x;
	unsigned int c_y;
	unsigned int c_depth;
	m_pDataContext->volume_data.getLengthForScreen(m_windowType, c_x, c_y, c_depth);

	QRect pos = getPosition();
	switch (m_windowType)
	{
	case WT_CORONAL://xzy
		vec.setX(pos.x() + (m_flipX ? c_x - volx - 1 : volx) * (scale ? m_zoomFactorX : 1));
		vec.setY(pos.y() + (m_flipY ? c_y - volz - 1 : volz) * (scale ? m_zoomFactorY : 1));
		vec.setZ(voly);
		break;
	case WT_SAGITTAL://yzx
		vec.setX(pos.x() + (m_flipX ? c_x - voly - 1 : voly) * (scale ? m_zoomFactorX : 1));
		vec.setY(pos.y() + (m_flipY ? c_y - volz - 1 : volz) * (scale ? m_zoomFactorY : 1));
		vec.setZ(volx);
		break;
	case WT_AXIAL://xyz
		vec.setX(pos.x() + (m_flipX ? c_x - volx - 1 : volx) * (scale ? m_zoomFactorX : 1));
		vec.setY(pos.y() + (m_flipY ? c_y - voly - 1 : voly) * (scale ? m_zoomFactorY : 1));
		vec.setZ(volz);
		break;
	default:
		break;;
	}

	return vec;
}


QVector3D WindowBase::volumeToScreenPositionAuto(muint32 x, muint32 y, muint32 depth, bool scale)
{
	switch (m_windowType)
	{
	case WT_CORONAL:
		return volumeToScreenPosition(x, depth, y);
	case WT_SAGITTAL:
		return volumeToScreenPosition(depth, x, y);
	case WT_AXIAL:
	default:
		return volumeToScreenPosition(x, y, depth);
	}
}

int WindowBase::volumeToScreenDepth(QVector3D pos)
{
	switch (m_windowType)
	{
	case WT_CORONAL:
		return pos.y();
	case WT_SAGITTAL:
		return pos.x();
	case WT_AXIAL:
	default:
		return pos.z();
	}
}

void WindowBase::setDutyCheck() 
{
	m_dutyCheck = true; 
}

mint16 WindowBase::getData(const QPoint& point, muint32 depth)
{
	QPoint p = screenToVolumePosition(&point);

	switch (m_windowType)
	{
	case WT_CORONAL:
		return m_pDataContext->volume_data.getData(p.x(), depth, p.y());
	case WT_SAGITTAL:
		return m_pDataContext->volume_data.getData(depth, p.x(), p.y());
	case WT_AXIAL:
	default:
		return m_pDataContext->volume_data.getData(p.x(), p.y(), depth);
	}
	return 0;
}

void WindowBase::createCirclePointList(muint32 sizeEllipse)
{
	m_list.clear();
	float eSize = sizeEllipse / 2;

	m_list.reserve((sizeEllipse + 2) * (sizeEllipse + 2));
	for (float tx = -eSize; tx <= eSize; tx++)
	{
		for (float ty = -eSize; ty <= eSize; ty++)
		{
			if (sqrtf(tx * tx + ty * ty) <= eSize)
			{
				m_list.push_back(QPoint(qFloor(tx), qFloor(ty)));
			}
		}
	}
}

void WindowBase::createCursorShape(bool _auto)
{
	QPolygon poly;

	int nSize = (WIN_MANAGER->getWorkMode() == WORK_RECTROI || WIN_MANAGER->getDCutShape() == DRAW_CUT_SHAPE_ANGULAR) ?
		WIN_MANAGER->AngularSize_S : WIN_MANAGER->CirculSize_S;
	float rSize = nSize / 2;

	poly.reserve((nSize + 2) * (nSize + 2));
	for (float tx = -rSize; tx <= rSize; tx++)
	{
		for (float ty = -rSize; ty <= rSize; ty++)
		{
			poly.push_back(QPoint(tx, ty));
		}
	}
	m_list = poly;

	if ((m_cursor.shape() == Qt::BitmapCursor) || _auto)
	{
		m_fZoomLow = m_pDataContext->volume_data.getMinSpace(true);

		m_cursor = getBitmapCursor();
		this->setCursor(m_cursor);
	}
}

void WindowBase::updateMeshOutline()
{
	m_bUpdateMeshOutline = true;
}

void WindowBase::calcMesh2DOutline()
{
	Visualize2DTab* pVisualize2D = WIN_MANAGER->GetTab()->get2DTab();
	if (!pVisualize2D || !pVisualize2D->isOutlineMode())
	{
		return;
	}

	int currentDepth = m_pDataContext->volume_data.getDepth(m_windowType);
	int prevDepth = m_pDataContext->volume_data.getPrevDepth(m_windowType);

	if (!m_bUpdateMeshOutline && ((currentDepth == prevDepth) || !ACTION_MANAGER->isActionFinished()))
	{
		return;
	}

	std::vector<mip::VECTOR3>	tempList;
	tempList = m_pDataContext->volume_data.GetMPRPPlanesByDepth(m_windowType, currentDepth);

	double halfSpaceX = m_pDataContext->volume_data.getSpaceX() * 0.25f;
	double halfSpaceY = m_pDataContext->volume_data.getSpaceY() * 0.25f;
	double halfSpaceZ = m_pDataContext->volume_data.getSpaceZ() * 0.25f;

	switch (m_windowType)
	{
	case WT_CORONAL:
	{
		tempList[0].y += halfSpaceY;
		tempList[1].y += halfSpaceY;
		tempList[2].y += halfSpaceY;
	}
	break;
	case WT_SAGITTAL:
	{
		tempList[0].x += halfSpaceX;
		tempList[1].x += halfSpaceX;
		tempList[2].x += halfSpaceX;
	}
	break;
	case WT_AXIAL:
	default:
	{
		tempList[0].z += halfSpaceZ;
		tempList[1].z += halfSpaceZ;
		tempList[2].z += halfSpaceZ;
	}
	break;
	}

	mip::PLANE plane;
	plane.fromTri(tempList[1], tempList[2], tempList[0]);

	// get offset mat(mesh <->volume)
	mip::VECTOR3 offset(mip::VECTOR3(-m_pDataContext->volume_data.getSizeX() * 0.5f, -m_pDataContext->volume_data.getSizeY() * 0.5f, -m_pDataContext->volume_data.getSizeZ() * 0.5f));

	mip::MATRIX44 offsetMat;
	offsetMat.translation(offset);

	int n_mesh = m_pDataContext->m_MeshData.GetMeshCount();
	for (int mi = 0; mi < n_mesh; ++mi)
	{
		MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(mi);

		if (pMeshInfo)
		{
			mip::MeshTopology* pMesh = m_pDataContext->m_MeshData.GetMesh(mi);

			if (pMesh)
			{
				mip::MATRIX44 meshMat = pMesh->getMatrix() * offsetMat;

				std::vector<std::vector<mip::VECTOR3>>* pVecOutline2D;

				switch (m_windowType)
				{
				case WT_CORONAL:
					pVecOutline2D = &pMeshInfo->vecOutline2DCoronal;
					break;
				case WT_SAGITTAL:
					pVecOutline2D = &pMeshInfo->vecOutline2DSagittal;
					break;
				case WT_AXIAL:
				default:
					pVecOutline2D = &pMeshInfo->vecOutline2DAxial;
					break;
				}

				mip::mesh_control::calcMesh2Outline(*pVecOutline2D, plane, pMesh, &meshMat);
			}
		}
	}

	m_pDataContext->volume_data.setPrevDepth(m_windowType, currentDepth);

	m_bUpdateMeshOutline = false;
}

void WindowBase::drawMeshOutline(QPainter* p)
{
	Visualize2DTab* pVisualize2D = WIN_MANAGER->GetTab()->get2DTab();
	if (!pVisualize2D || !pVisualize2D->isOutlineMode() || !ACTION_MANAGER->isActionFinished())
	{
		return;
	}

	float size_x = m_pDataContext->volume_data.getSizeX();
	float size_y = m_pDataContext->volume_data.getSizeY();
	float size_z = m_pDataContext->volume_data.getSizeZ();

	float space_x = m_pDataContext->volume_data.getSpaceX();
	float space_y = m_pDataContext->volume_data.getSpaceY();
	float space_z = m_pDataContext->volume_data.getSpaceZ();

	int n_mesh = m_pDataContext->m_MeshData.GetMeshCount();
	for (int mi = 0; mi < n_mesh; ++mi)
	{
		MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(mi);

		if (pMeshInfo && pMeshInfo->show)
		{
			mip::MeshTopology* pMesh = m_pDataContext->m_MeshData.GetMesh(mi);

			if (pMesh)
			{
				p->setPen(QPen(QColor(pMeshInfo->color.r, pMeshInfo->color.g, pMeshInfo->color.b), 2));

				std::vector<std::vector<mip::VECTOR3>>* pVecOutline2D;

				switch (m_windowType)
				{
				case WT_CORONAL:
					pVecOutline2D = &pMeshInfo->vecOutline2DCoronal;
					break;
				case WT_SAGITTAL:
					pVecOutline2D = &pMeshInfo->vecOutline2DSagittal;
					break;
				case WT_AXIAL:
				default:
					pVecOutline2D = &pMeshInfo->vecOutline2DAxial;
					break;
				}

				int n_lines = (int)pVecOutline2D->size();
				for (int j = 0; j < n_lines; ++j)
				{
					if ((*pVecOutline2D)[j].size() != 2)
					{
						continue;
					}

					QVector3D pt3d[2];
					QPointF	  pt2d[2];
					for (int k = 0; k < (*pVecOutline2D)[j].size(); ++k)
					{
						float newX = ((*pVecOutline2D)[j][k].x + size_x * 0.5f) / space_x;
						float newY = ((*pVecOutline2D)[j][k].y + size_y * 0.5f) / space_y;
						float newZ = ((*pVecOutline2D)[j][k].z + size_z * 0.5f) / space_z;

						int xPos = std::round(newX);
						int yPos = std::round(newY);
						int zPos = std::round(newZ) - 1; // -1은 추후 디버깅 예정(원인 분석필요) 허 건 과장

						int idx = k % 2;
						pt3d[idx] = volumeToScreenPosition(xPos, yPos, zPos);

						pt2d[idx].setX(pt3d[idx].x());
						pt2d[idx].setY(pt3d[idx].y());

						if (idx == 1)
						{
							p->drawLine(pt2d[0], pt2d[1]);
						}
					}
				}
			}
		}
	}
}

void WindowBase::drawAniLine(QPainter* p)
{
	int _group = WIN_MANAGER->getAnimationGroup();
	if (WIN_MANAGER->aniCount.at(_group) <= 0)
	{
		return;
	}

	bool bInArea = false;
	static OpenGLWidget* volumeView = WIN_MANAGER->mainSegmentWidget->getViewVolume();

	AnimationTab* tab = WIN_MANAGER->GetTab()->getAniTab();
	bool bSpiral = tab ? tab->isSpiral() : false;
	QVector<QVector3D>* Lines = bSpiral ? &(WIN_MANAGER->spiralLine) : nullptr;
	QPolygonF polyLines;
	if (bSpiral)
	{
		p->setPen(QPen(Qt::darkRed, 2));
		for (int i = 0; i < Lines->count(); i++)
		{
			bInArea = false;

			QVector3D vec = Lines->at(i);
			if (getDepth() == getAniDepth(i))
				bInArea = true;

			if (!bInArea)
			{
				if (!polyLines.isEmpty())
				{
					p->drawPolyline(polyLines);
				}
				polyLines.clear();
				continue;
			}

			QVector3D pos = volumeToScreenPosition(Lines->at(i).x(), Lines->at(i).y(), Lines->at(i).z());
			polyLines.push_back(QPointF(pos.x(), pos.y()));
		}

		if (!polyLines.isEmpty())
		{
			p->drawPolyline(polyLines);
		}
	}

	int nStart, nEnd;
	nEnd = nStart = WIN_MANAGER->getAniStartPoint(_group);
	nEnd += WIN_MANAGER->aniCount.at(_group);
	Lines = &(WIN_MANAGER->aniLine);
	p->setPen(QPen(Qt::red, m_zoomFactorX + 2));

	for (int i = nStart; i < nEnd; i++)
	{
		bInArea = false;

		if (getDepth() == getAniDepth(i, true, false))
		{
			bInArea = true;
		}

		if (!bInArea)
		{
			continue;
		}

		QVector3D pos = volumeToScreenPosition(Lines->at(i).x(), Lines->at(i).y(), Lines->at(i).z());
		p->drawEllipse(QPoint(pos.x(), pos.y()), 2, 2);
	}

	bool state = volumeView->getUpdateFrameState();

	if (!state && WIN_MANAGER->aniMove && (getDepth() == getAniDepth(-1, false)))
	{
		p->setPen(QPen(Qt::green, m_zoomFactorX + 2));
		QVector3D pos = volumeToScreenPosition(WIN_MANAGER->LinePos.x(), WIN_MANAGER->LinePos.y(), WIN_MANAGER->LinePos.z());
		p->drawEllipse(QPoint(pos.x(), pos.y()), 1, 1);
	}
}


void WindowBase::forceUpdateSliceColorData()
{
	m_preDepth = m_depth + 1;
}

void WindowBase::enableControls(bool bEnable)
{
}

void WindowBase::setShowBounding(bool show)
{
	if (m_btnShowBounding)
	{
		m_btnShowBounding->setChecked(show);
	}
}

void WindowBase::initInScreenMenu()
{
	if (m_slidebar == nullptr)
	{
		m_slidebar = new QSlider(Qt::Horizontal, this);
		m_slidebar->setStyleSheet(STYLE_MANAGER->slideBarView);
		m_slidebar->installEventFilter(this);
		connect(m_slidebar, &QSlider::valueChanged, this, &WindowBase::slot_OnDepthSlideChanged);
		connect(m_slidebar, &QSlider::sliderReleased, this, &WindowBase::slot_OnDepthSlideReleased);

		m_slidebar->hide();
	}
}

void WindowBase::initInScreenRightTopMenu()
{
}

void WindowBase::initInScreenRightBottomMenu()
{
	if (m_btnShowBounding == nullptr)
	{
		m_btnShowBounding = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_COORD, m_nIconSize, m_nIconSize), "", this);
		m_btnShowBounding->setIconSize(QSize(m_nIconSize, m_nIconSize));
		m_btnShowBounding->setFixedSize(QSize(m_nIconSize, m_nIconSize));
		m_btnShowBounding->setCheckable(true);
		m_btnShowBounding->setMouseTracking(true);
		m_btnShowBounding->installEventFilter(this);
		m_btnShowBounding->hide();
		m_btnShowBounding->setStyleSheet("color: black;");
		m_btnShowBounding->setToolTip("Region proposal\n(Working region)");

		connect(m_btnShowBounding, &QPushButton::clicked, this, &WindowBase::slot_OnShowBounding);
	}

	if (m_btnBoundaryEdge == nullptr)
	{
		m_btnBoundaryEdge = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_BOUNDARY, m_nIconSize, m_nIconSize), "", this);
		m_btnBoundaryEdge->setIconSize(QSize(m_nIconSize, m_nIconSize));
		m_btnBoundaryEdge->setFixedSize(QSize(m_nIconSize, m_nIconSize));
		m_btnBoundaryEdge->setCheckable(true);
		m_btnBoundaryEdge->setMouseTracking(true);
		m_btnBoundaryEdge->installEventFilter(this);
		m_btnBoundaryEdge->hide();
		m_btnBoundaryEdge->setStyleSheet("color: black;");
		m_btnBoundaryEdge->setToolTip("Boundary Viewer\n(Border of segmentation mask)");

		connect(m_btnBoundaryEdge, &QPushButton::clicked, this, &WindowBase::slot_OnShowBoundaryEdge);
	}
}

void WindowBase::initInScreenLeftBottomMenu()
{
	if (m_btnFullScreen == nullptr)
	{
		m_btnFullScreen = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_FULL_SCREEN, m_nIconSize, m_nIconSize), "", this);
		m_btnFullScreen->setIconSize(QSize(m_nIconSize, m_nIconSize));
		m_btnFullScreen->setFixedSize(QSize(m_nIconSize, m_nIconSize));
		m_btnFullScreen->setCheckable(true);
		m_btnFullScreen->setMouseTracking(true);
		m_btnFullScreen->installEventFilter(this);
		m_btnFullScreen->setObjectName("SegButtonFullscreen");
		m_btnFullScreen->hide();
		m_btnFullScreen->setStyleSheet("color: black;");
		m_btnFullScreen->setToolTip("Full screen");
		connect(m_btnFullScreen, &QPushButton::clicked, this, &SegmentationView::slot_OnFullScreen);
	}
}

void WindowBase::updateScreenMenu(int width, int height)
{
	int w = 0;
	if (m_btnShowBounding)
	{
		m_btnShowBounding->move(width - m_btnShowBounding->width(), height - m_btnShowBounding->height());
		w += m_btnShowBounding->width();
	}

	if (m_btnBoundaryEdge)
	{
		m_btnBoundaryEdge->move(width - (w + m_btnBoundaryEdge->width()), height - m_btnBoundaryEdge->height());
	}

	if (m_slidebar)
	{
		m_slidebar->resize(2 * (width / 3), 20);
		m_slidebar->move(width / 2 - m_slidebar->size().width() / 2, height - 30);
	}

	if (m_btnFullScreen)
	{
		m_btnFullScreen->move(0, height - m_btnFullScreen->height());
	}
}

void WindowBase::updateSliceColorData(bool forceUpdate)
{
	unsigned int c_x;
	unsigned int c_y;
	unsigned int c_depth;
	m_pDataContext->volume_data.getLengthForScreen(m_windowType, c_x, c_y, c_depth);

	if (m_colorData == nullptr && c_x > 0 && c_y > 0)
	{
		m_colorData = new COLOR[c_x * c_y];
		forceUpdate = true;
	}

	if (m_preDepth != m_depth)
	{
		forceUpdate = true;
		m_preDepth = m_depth;
	}

	if (m_colorData == nullptr)
	{
		return;
	}

	if (forceUpdate == true)
	{
		int window_level = WIN_MANAGER->getWindowLevel();
		int window_width = WIN_MANAGER->getWindowWidth();
		BoundingBoxI box = m_pDataContext->volume_data.getBoundingBoxForScreen(m_windowType);
		bool colorMode = WIN_MANAGER->getColor2DMode();
		bool invertMode = WIN_MANAGER->getInvertingLUTMode();
		float gamma = 1.0f / WIN_MANAGER->getGamma();
		bool cliped = false;
		COLOR color;

		for (int y = 0; y < c_y; y++)
		{
			for (int x = 0; x < c_x; x++)
			{
				switch (m_windowType)
				{
				case WT_CORONAL:
					color = m_pDataContext->volume_data.getGrayColorData
					(m_flipX ? c_x - x - 1 : x, m_depth, m_flipY ? c_y - y - 1 : y, window_level, window_width, colorMode, invertMode);
					break;
				case WT_SAGITTAL:
					color = m_pDataContext->volume_data.getGrayColorData
					(m_depth, m_flipX ? c_x - x - 1 : x, m_flipY ? c_y - y - 1 : y, window_level, window_width, colorMode, invertMode);
					break;
				case WT_AXIAL:
				default:
					color = m_pDataContext->volume_data.getGrayColorData
					(m_flipX ? c_x - x - 1 : x, m_flipY ? c_y - y - 1 : y, m_depth, window_level, window_width, colorMode, invertMode);
					break;
				}

				//////   process cliped area gray
				cliped = false;

				if (m_flipX)
				{
					if (x > box.getMinX(true) || x < box.getMaxX(true) || y < box.minY || y > box.maxY)
					{
						color = COLOR(color.r / 5, color.g / 5, color.b / 5, color.a);
						cliped = true;
					}
				}
				else if (m_flipY)
				{
					if (x < box.minX || x > box.maxX || y > box.getMinY(true) || y < box.getMaxY(true))
					{
						color = COLOR(color.r / 5, color.g / 5, color.b / 5, color.a);
						cliped = true;
					}
				}
				else
				{
					if (x < box.minX || x > box.maxX || y < box.minY || y > box.maxY)
					{
						color = COLOR(color.r / 5, color.g / 5, color.b / 5, color.a);
						cliped = true;
					}
				}

				if (cliped == false)
				{
					if (m_depth < box.minZ || m_depth > box.maxZ)
					{
						color = COLOR(color.r / 2, color.g / 2, color.b / 2, color.a);
					}
				}

				int val;
				if (color.r > 0)
				{
					val = (pow(color.r / 255.0f, gamma) * 255.0f);
					color.r = val <= 255 ? (muint8)val : 0;
				}
				if (color.g > 0)
				{
					val = (pow(color.g / 255.0f, gamma) * 255.0f);
					color.g = val <= 255 ? (muint8)val : 0;
				}
				if (color.b > 0)
				{
					val = (pow(color.b / 255.0f, gamma) * 255.0f);
					color.b = val <= 255 ? (muint8)val : 0;
				}

#ifdef SUPPORT_L_T_CHECK
				if (!(c_x * c_y <= (c_x * y) + x))
					m_colorData[(c_x * y) + x] = color;
#else
				m_colorData[(c_x * y) + x] = color;
#endif
			}
		}
	}

}

void WindowBase::drawSlice(QPainter* p)
{
	updateSliceColorData();

	unsigned int c_x;
	unsigned int c_y;
	unsigned int c_depth;
	m_pDataContext->volume_data.getLengthForScreen(m_windowType, c_x, c_y, c_depth);

	QRect pos = getPosition();

	if (m_colorData)
	{
		QImage image((uchar*)m_colorData, c_x, c_y, QImage::Format_RGBA8888);
		p->drawImage(pos, image);
	}
}

void WindowBase::updateMouseActionMode()
{
	if (WIN_MANAGER->getShowClippingBox() == false)
	{
		return;
	}

	if (m_mouse_action_mode >= MAM_BOUNDING_MOVE)
	{
		BoundingBoxI box = m_pDataContext->volume_data.getBoundingBox();
		QPoint Pos = screenToVolumePosition(&m_MousePos);

		if (m_mouse_action_mode & MAM_BOUNDING_MAXX_MOVE)
			box.setMaxX(m_windowType, m_flipX ? -Pos.x() : Pos.x());
		if (m_mouse_action_mode & MAM_BOUNDING_MINX_MOVE)
			box.setMinX(m_windowType, m_flipX ? -Pos.x() : Pos.x());
		if (m_mouse_action_mode & MAM_BOUNDING_MAXY_MOVE)
			box.setMaxY(m_windowType, Pos.y());
		if (m_mouse_action_mode & MAM_BOUNDING_MINY_MOVE)
			box.setMinY(m_windowType, Pos.y());

		m_pDataContext->volume_data.setBoundingBox(box);

		renderLater();
	}
	else  // mouse over check
	{
		BoundingBoxI box = m_pDataContext->volume_data.getBoundingBoxForScreen(m_windowType);
		QRect pos = getPosition();

		unsigned int depth = WINDOW_BOUNDING_DEPTH;

		int minX = (m_flipX) ? (pos.x() + (box.getMinX(m_flipX) + 1) * m_zoomFactorX) : (pos.x() + box.getMinX(m_flipX) * m_zoomFactorX);
		int maxX = (m_flipX) ? (pos.x() + box.getMaxX(m_flipX) * m_zoomFactorX) : (pos.x() + (box.getMaxX(m_flipX) + 1) * m_zoomFactorX);
		int minY = (m_flipY) ? (pos.y() + (box.getMinY(m_flipY) + 1) * m_zoomFactorY) : (pos.y() + box.getMinY(m_flipY) * m_zoomFactorY);
		int maxY = (m_flipY) ? (pos.y() + box.getMaxY(m_flipY) * m_zoomFactorY) : (pos.y() + (box.getMaxY(m_flipY) + 1) * m_zoomFactorY);

		m_mouse_action_mode = MAM_NONE;

		if (m_MousePos.x() > (minX - depth) && m_MousePos.x() < (minX + depth))
			m_mouse_action_mode |= MAM_BOUNDING_MINX_OVER;
		else if (m_MousePos.x() > (maxX - depth) && m_MousePos.x() < (maxX + depth))
			m_mouse_action_mode |= MAM_BOUNDING_MAXX_OVER;

		if (m_MousePos.y() > (minY - depth) && m_MousePos.y() < (minY + depth))
			m_mouse_action_mode |= MAM_BOUNDING_MINY_OVER;
		else if (m_MousePos.y() > (maxY - depth) && m_MousePos.y() < (maxY + depth))
			m_mouse_action_mode |= MAM_BOUNDING_MAXY_OVER;


		if (m_pre_mouse_action_mode != m_mouse_action_mode)
			renderLater();

		m_pre_mouse_action_mode = m_mouse_action_mode;
	}
}

void WindowBase::drawCoordLine(QPainter* p)
{
	if (WIN_MANAGER->getShowClippingBox() == false)
	{
		return;
	}

	if (!(REDRAW_TYPE == WIN_MANAGER->getCoordType()))
	{
		return;
	}

	WindowBase* winY;
	WindowBase* winX;
	unsigned int c_x;
	unsigned int c_y;
	unsigned int c_depth;
	m_pDataContext->volume_data.getLengthForScreen(m_windowType, c_x, c_y, c_depth);

	if (WIN_MANAGER->mainTabType == MAINTAB_SEGMENTATION)
	{
		switch (m_windowType)
		{
		case WT_CORONAL:
			winY = WIN_MANAGER->mainSegmentWidget->getWindow(WT_AXIAL);
			winX = WIN_MANAGER->mainSegmentWidget->getWindow(WT_SAGITTAL);
			break;
		case WT_SAGITTAL:
			winY = WIN_MANAGER->mainSegmentWidget->getWindow(WT_AXIAL);
			winX = WIN_MANAGER->mainSegmentWidget->getWindow(WT_CORONAL);
			break;
		case WT_AXIAL:
		default:
			winY = WIN_MANAGER->mainSegmentWidget->getWindow(WT_CORONAL);
			winX = WIN_MANAGER->mainSegmentWidget->getWindow(WT_SAGITTAL);
			break;
		}
	}
	else if (WIN_MANAGER->mainTabType == MAINTAB_TA)
	{
		switch (m_windowType)
		{
		case WT_CORONAL:
			winY = WIN_MANAGER->mainTAWidget->getWindow(WT_AXIAL);
			winX = WIN_MANAGER->mainTAWidget->getWindow(WT_SAGITTAL);
			break;
		case WT_SAGITTAL:
			winY = WIN_MANAGER->mainTAWidget->getWindow(WT_AXIAL);
			winX = WIN_MANAGER->mainTAWidget->getWindow(WT_CORONAL);
			break;
		case WT_AXIAL:
		default:
			winY = WIN_MANAGER->mainTAWidget->getWindow(WT_CORONAL);
			winX = WIN_MANAGER->mainTAWidget->getWindow(WT_SAGITTAL);
			break;
		}
	}

	if (winY == nullptr || winX == nullptr)
	{
		return;
	}

	QRect pos = getPosition();
	p->setPen(QPen(Qt::red, 1));
	p->drawLine(0, pos.y() + ((float)winY->getDepth(m_flipY) + 0.5) * m_zoomFactorY, this->size().width(), pos.y() + ((float)winY->getDepth(m_flipY) + 0.5) * m_zoomFactorY);

	p->setPen(QPen(Qt::green, 1));
	p->drawLine(pos.x() + ((float)winX->getDepth(m_flipX) + 0.5) * m_zoomFactorX, 0, pos.x() + ((float)winX->getDepth(m_flipX) + 0.5) * m_zoomFactorX, this->size().height());

}

void WindowBase::drawBoundLine(QPainter* p)
{
	if (WIN_MANAGER->getShowClippingBox() == false)
	{
		return;
	}

	BoundingBoxI box = m_pDataContext->volume_data.getBoundingBoxForScreen(m_windowType);
	QRect pos = getPosition();

	int minX = (m_flipX) ? (pos.x() + (box.getMinX(m_flipX) + 1) * m_zoomFactorX) : (pos.x() + box.getMinX(m_flipX) * m_zoomFactorX);
	int maxX = (m_flipX) ? (pos.x() + box.getMaxX(m_flipX) * m_zoomFactorX) : (pos.x() + (box.getMaxX(m_flipX) + 1) * m_zoomFactorX);
	int minY = (m_flipY) ? (pos.y() + (box.getMinY(m_flipY) + 1) * m_zoomFactorY) : (pos.y() + box.getMinY(m_flipY) * m_zoomFactorY);
	int maxY = (m_flipY) ? (pos.y() + box.getMaxY(m_flipY) * m_zoomFactorY) : (pos.y() + (box.getMaxY(m_flipY) + 1) * m_zoomFactorY);

	int nX, nY;
	bool selX = false, selY = false;

	if ((MAM_BOUNDING_MINX_OVER & m_mouse_action_mode) || (MAM_BOUNDING_MINX_MOVE & m_mouse_action_mode))
	{
		p->setPen(QPen(Qt::blue, WINDOW_BOUNDING_DEPTH));
		selX = true;
		nX = minX;
	}
	else
	{
		p->setPen(QPen(Qt::white, 1));
	}
	p->drawLine(minX, 0, minX, this->size().height());

	if ((MAM_BOUNDING_MAXX_OVER & m_mouse_action_mode) || (MAM_BOUNDING_MAXX_MOVE & m_mouse_action_mode))
	{
		p->setPen(QPen(Qt::blue, WINDOW_BOUNDING_DEPTH));
		selX = true;
		nX = maxX;
	}
	else
	{
		p->setPen(QPen(Qt::white, 1));
	}
	p->drawLine(maxX, 0, maxX, this->size().height());

	if ((MAM_BOUNDING_MINY_OVER & m_mouse_action_mode) || (MAM_BOUNDING_MINY_MOVE & m_mouse_action_mode))
	{
		p->setPen(QPen(Qt::blue, WINDOW_BOUNDING_DEPTH));
		selY = true;
		nY = minY;
	}
	else
	{
		p->setPen(QPen(Qt::white, 1));
	}
	p->drawLine(0, minY, this->size().width(), minY);

	if ((MAM_BOUNDING_MAXY_OVER & m_mouse_action_mode) || (MAM_BOUNDING_MAXY_MOVE & m_mouse_action_mode))
	{
		p->setPen(QPen(Qt::blue, WINDOW_BOUNDING_DEPTH));
		selY = true;
		nY = maxY;
	}
	else
	{
		p->setPen(QPen(Qt::white, 1));
	}
	p->drawLine(0, maxY, this->size().width(), maxY);

	if (selX && selY)
	{
		p->setPen(Qt::red);
		p->setBrush(Qt::red);
		p->drawEllipse(QPoint(nX, nY), WINDOW_BOUNDING_DEPTH, WINDOW_BOUNDING_DEPTH);
	}
}

void WindowBase::updateSliceMaskData(unsigned char m, int mI, COLOR _color, BoundingBoxI& maskArea, bool forceUpdate, COLOR* buffer)
{
	unsigned int c_x;
	unsigned int c_y;
	unsigned int c_depth;
	m_pDataContext->volume_data.getLengthForScreen(m_windowType, c_x, c_y, c_depth);

	if (buffer == nullptr && m_maskData == nullptr)
	{
		return;
	}

	if (forceUpdate == true)
	{
		int window_level = WIN_MANAGER->getWindowLevel();
		int window_width = WIN_MANAGER->getWindowWidth();
		COLOR* buffer_data = buffer;

		unsigned char mcheck[6];

		COLOR color_zero = COLOR(0, 0, 0, 0);
		COLOR color_mask = _color;
		COLOR color_result;

		bool isDrawcut = false;

		if (buffer_data == nullptr) buffer_data = m_maskData;

		if (0 == mI)
		{
			switch (m)
			{
			case VM_MASK0:
				isDrawcut = true;
				color_mask = WIN_MANAGER->getSeedColor(0);
				break;
			case VM_MASK1:
				isDrawcut = true;
				color_mask = WIN_MANAGER->getSeedColor(1);
			default:
				break;
			}
		}
		bool r = false;
		bool edge_check = false;
		bool cliped = false;

		BoundingBoxI box = m_pDataContext->volume_data.getBoundingBoxForScreen(m_windowType);
		bool Colormode = WIN_MANAGER->getColor2DMode();
		bool InvertMode = WIN_MANAGER->getInvertingLUTMode();
		COLOR destCol;

		for (int y = (m_flipY ? maskArea.getMaxY(true) : maskArea.minY); y <= (m_flipY ? maskArea.getMinY(true) : maskArea.maxY); y++)
		{
			for (int x = maskArea.minX; x <= maskArea.maxX; x++)
			{
				edge_check = false;
				switch (m_windowType)
				{
				case WT_CORONAL:
					destCol = m_pDataContext->volume_data.getGrayColorData
					(m_flipX ? c_x - x - 1 : x, m_depth, m_flipY ? c_y - y - 1 : y, window_level, window_width, Colormode, InvertMode);
					r = m_pDataContext->volume_data.isMaskBit(m_flipX ? c_x - x - 1 : x, m_depth, m_flipY ? c_y - y - 1 : y, m, mI);
					if (r && WIN_MANAGER->getBoundaryMode())
					{
						if (x - 1 >= 0 && x + 1 < c_x && (m_flipY ? c_y - y - 1 : y) - 1 >= 0 && (m_flipY ? c_y - y - 1 : y) + 1 < c_y && m_depth - 1 >= 0 && m_depth < c_depth)
						{
							mcheck[0] = m_pDataContext->volume_data.getMaskData(x - 1, m_depth, (m_flipY ? c_y - y - 1 : y), mI);
							mcheck[1] = m_pDataContext->volume_data.getMaskData(x + 1, m_depth, (m_flipY ? c_y - y - 1 : y), mI);
							mcheck[2] = m_pDataContext->volume_data.getMaskData(x, m_depth, (m_flipY ? c_y - y - 1 : y) - 1, mI);
							mcheck[3] = m_pDataContext->volume_data.getMaskData(x, m_depth, (m_flipY ? c_y - y - 1 : y) + 1, mI);
							//	mcheck[4] = m_pDataContext->volume_data.getMaskData(x, m_depth - 1, y);
							//	mcheck[5] = m_pDataContext->volume_data.getMaskData(x, m_depth + 1, y);
							edge_check = true;
						}
					}
					break;
				case WT_SAGITTAL:
					destCol = m_pDataContext->volume_data.getGrayColorData
					(m_depth, m_flipX ? c_x - x - 1 : x, m_flipY ? c_y - y - 1 : y, window_level, window_width, Colormode, InvertMode);
					r = m_pDataContext->volume_data.isMaskBit(m_depth, m_flipX ? c_x - x - 1 : x, m_flipY ? c_y - y - 1 : y, m, mI);
					if (r && WIN_MANAGER->getBoundaryMode())
					{
						if (x - 1 >= 0 && x + 1 < c_x && (m_flipY ? c_y - y - 1 : y) - 1 >= 0 && (m_flipY ? c_y - y - 1 : y) + 1 < c_y && m_depth - 1 >= 0 && m_depth < c_depth)
						{
							mcheck[0] = m_pDataContext->volume_data.getMaskData(m_depth, x - 1, (m_flipY ? c_y - y - 1 : y), mI);
							mcheck[1] = m_pDataContext->volume_data.getMaskData(m_depth, x + 1, (m_flipY ? c_y - y - 1 : y), mI);
							mcheck[2] = m_pDataContext->volume_data.getMaskData(m_depth, x, (m_flipY ? c_y - y - 1 : y) - 1, mI);
							mcheck[3] = m_pDataContext->volume_data.getMaskData(m_depth, x, (m_flipY ? c_y - y - 1 : y) + 1, mI);
							//	mcheck[4] = m_pDataContext->volume_data.getMaskData(m_depth - 1, x, y);
							//	mcheck[5] = m_pDataContext->volume_data.getMaskData(m_depth + 1, x, y);
							edge_check = true;
						}
					}
					break;
				case WT_AXIAL:
					destCol = m_pDataContext->volume_data.getGrayColorData
					(m_flipX ? c_x - x - 1 : x, m_flipY ? c_y - y - 1 : y, m_depth, window_level, window_width, Colormode, InvertMode);
					r = m_pDataContext->volume_data.isMaskBit(m_flipX ? c_x - x - 1 : x, (m_flipY ? c_y - y - 1 : y), m_depth, m, mI);
					if (r && WIN_MANAGER->getBoundaryMode())
					{
						if (x - 1 >= 0 && x + 1 < c_x && (m_flipY ? c_y - y - 1 : y) - 1 >= 0 && (m_flipY ? c_y - y - 1 : y) + 1 < c_y && m_depth - 1 >= 0 && m_depth < c_depth)
						{
							mcheck[0] = m_pDataContext->volume_data.getMaskData(x - 1, (m_flipY ? c_y - y - 1 : y), m_depth, mI);
							mcheck[1] = m_pDataContext->volume_data.getMaskData(x + 1, (m_flipY ? c_y - y - 1 : y), m_depth, mI);
							mcheck[2] = m_pDataContext->volume_data.getMaskData(x, (m_flipY ? c_y - y - 1 : y) - 1, m_depth, mI);
							mcheck[3] = m_pDataContext->volume_data.getMaskData(x, (m_flipY ? c_y - y - 1 : y) + 1, m_depth, mI);
							//mcheck[4] = m_pDataContext->volume_data.getMaskData(x, y, m_depth - 1);
							//mcheck[5] = m_pDataContext->volume_data.getMaskData(x, y, m_depth + 1);
							edge_check = true;
						}
					}
					break;
				default:
					break;;
				}
				bool edge = false;
				if (edge_check)
				{
					for (int k = 0; k < 4; k++)
					{
						if (edge == false & !(mcheck[k] & m))
						{
							edge = true;
							break;
						}
					}

					if (edge == true)
						color_result = color_mask;
					else
						continue;
				}
				else
				{
					if (r == true)
						color_result = color_mask;
					else
						continue;
				}

				//////   process cliped area gray
				cliped = false;

				if (m_flipX)
				{
					if (x > box.getMinX(true) || x < box.getMaxX(true) || y < box.minY || y > box.maxY)
					{
						color_result = COLOR(color_result.r / 5, color_result.g / 5, color_result.b / 5, color_result.a);
						cliped = true;
					}
				}
				else if (m_flipY)
				{
					if (x < box.minX || x > box.maxX || y > box.getMinY(true) || y < box.getMaxY(true))
					{
						color_result = COLOR(color_result.r / 5, color_result.g / 5, color_result.b / 5, color_result.a);
						cliped = true;
					}
				}
				else
				{
					if (x < box.minX || x > box.maxX || y < box.minY || y > box.maxY)
					{
						color_result = COLOR(color_result.r / 5, color_result.g / 5, color_result.b / 5, color_result.a);
						cliped = true;
					}
				}

				if (cliped == false)
				{
					if (m_depth < box.minZ || m_depth > box.maxZ)
					{
						color_result = COLOR(color_result.r / 2, color_result.g / 2, color_result.b / 2, color_result.a);
					}

				}
				if (!isDrawcut)
				{
					float alpha = WIN_MANAGER->getLayer2DAlphaValue();
					alpha *= color_result.a; /*0~255*/
					alpha /= 255.0f; /*0~1.0f*/

					if (buffer_data[(c_x * y) + x].a != 0)
						destCol = buffer_data[(c_x * y) + x];

					color_result = COLOR(destCol.r * (1.0f - alpha) + color_result.r * alpha, destCol.g * (1.0f - alpha) + color_result.g * alpha,
						destCol.b * (1.0f - alpha) + color_result.b * alpha, 255);
				}

				else if (WIN_MANAGER->getEdgeMode())
				{
					color_result.a = 255;
				}

				buffer_data[(c_x * y) + x] = color_result;
			}
		}
	}
}

void WindowBase::drawMaskAll(QPainter* p, bool forceUpdate)
{
	bool mask_render = true;

	if (forceUpdate == true)
	{
		mask_render = false;
		unsigned int c_x;
		unsigned int c_y;
		unsigned int c_depth;
		m_pDataContext->volume_data.getLengthForScreen(m_windowType, c_x, c_y, c_depth);

		if (c_x <= 0 || c_y <= 0)
		{
			return;
		}

		if (m_maskData == nullptr)
		{
			m_maskData = new COLOR[c_x * c_y];
		}

		if (m_maskData == nullptr)
		{
			return;
		}

		memset(m_maskData, 0, c_x * c_y * sizeof(COLOR));

		for (int n = m_pDataContext->volume_data.getMaskInfoListCnt() - 1; n >= 0; n--)
		{
			MaskInfo* info = m_pDataContext->volume_data.getMaskInfo(n);

			if (info->uid >= MASK_MAX)
			{
				continue;
			}

			if (info == nullptr || info->show == false)
			{
				continue;
			}

			if (m_pDataContext->volume_data.getVoxelCount(info->uid) <= 0)
			{
				continue;
			}

			int mI = info->uid >= MASK_SECOND_MAX ? (info->uid - MASK_SECOND_MAX) / 8 + 1 : 0;
			info->color.a = info->layerAlpha;

			if (info->color.a == 0)
			{
				continue;
			}

			BoundingBoxI box = m_pDataContext->volume_data.getLayerBoundingBoxForScreen(m_windowType, info->uid);
			if (m_depth < box.minZ || m_depth > box.maxZ)
			{
				continue;
			}

			mask_render = true;

			updateSliceMaskData(0 == mI ? info->mask_id : info->mask_id2, mI, info->color, box, true);
		}
	}

	if (mask_render == true)
	{
		drawMask(p);
	}
}

void WindowBase::drawMask(QPainter* p, unsigned char m, int mI, COLOR _color)
{
	unsigned int c_x;
	unsigned int c_y;
	unsigned int c_depth;

	m_pDataContext->volume_data.getLengthForScreen(m_windowType, c_x, c_y, c_depth);
	//	m_pDataContext->volume_data.getLayerLengthForScreen(m_windowType, x, y, z, true);

	if (c_x <= 0 || c_y <= 0)
	{
		return;
	}

	BoundingBoxI box = m_pDataContext->volume_data.getLayerBoundingBoxForScreen(m_windowType, MASK_MAX);

	COLOR* buffer = new COLOR[c_x * c_y];
	memset(buffer, 0, sizeof(COLOR) * c_x * c_y);

	updateSliceMaskData(m, mI, _color, box, true, buffer);

	QRect pos = getPosition();

	QImage image((uchar*)buffer, c_x, c_y, QImage::Format_RGBA8888);
	p->drawImage(pos, image);

	SAFE_DELETES(buffer);
}

void WindowBase::drawMask(QPainter* p)
{
	if (m_maskData == nullptr)
	{
		return;
	}

	unsigned int c_x;
	unsigned int c_y;
	unsigned int c_depth;

	m_pDataContext->volume_data.getLengthForScreen(m_windowType, c_x, c_y, c_depth);

	QRect pos = getPosition();

	QImage image((uchar*)m_maskData, c_x, c_y, QImage::Format_RGBA8888);
	p->drawImage(pos, image);
}

void WindowBase::drawMouseWork(QPainter* p)
{
	bool focusWin = WIN_MANAGER->getLatestActiveViewType() == m_windowType;

	switch (WIN_MANAGER->getWorkMode())
	{
	case WORK_CAPTURE:
	{
		if (!focusWin)
		{
			break;
		}

		bool mode = WIN_MANAGER->GetTab()->getCaptureTab()->getCaptureMode();

		if (mode)
		{
			QPoint p1, p2;

			p1 = QPoint(0, 0);
			p2 = QPoint(this->width() - 1, 0);

			p->setPen(Qt::red);
			p->drawLine(p1, p2);

			p2 = QPoint(0, this->height() - 1);
			p->drawLine(p1, p2);

			p1 = QPoint(this->width() - 1, this->height() - 1);
			p->drawLine(p1, p2);

			p2 = QPoint(this->width() - 1, 0);
			p->drawLine(p1, p2);
		}

		if (m_LbuttonDown)
		{
			if (m_polyLine.count() >= 2)
			{
				p->setPen(QPen(Qt::red, m_zoomFactorX + 1));

				QRect rect;

				rect.setCoords(m_polyLine.at(0).x(), m_polyLine.at(0).y(),
					m_polyLine.at(1).x(), m_polyLine.at(1).y());

				p->drawRect(rect);
			}
		}
	}
	break;
	case WORK_POLYROI:
	{
		p->setPen(QPen(Qt::yellow, m_zoomFactorX + 1));
		for (auto pl = m_polyLine.begin(); pl != m_polyLine.end(); ++pl)
		{
			p->drawPoint(*pl);
		}

		p->setPen(QPen(Qt::green, 1));
		p->drawPolyline(m_polyLine);

		if (m_polyLine.size() > 1)
		{
			QPoint q = m_polyLine[0] - m_MousePos;
			if (q.manhattanLength() < 10)
			{
				p->setPen(QPen(Qt::red, m_zoomFactorX + 4));
				p->drawPoint(m_polyLine[0]);
			}
		}
	}
	break;
	case WORK_PICKERROI:
	case WORK_REGION_ROI:
		if (m_LbuttonDown || m_RbuttonDown)
		{
			if (m_LbuttonDown)
			{
				MaskInfo* info = m_pDataContext->volume_data.getCurrentMaskInfo();
				if (info)
					p->setPen(QPen(QColor(info->color.r, info->color.g, info->color.b), m_zoomFactorX));
			}
			else
				p->setPen(QPen(Qt::white, m_zoomFactorX));

			p->drawPolyline(m_polyLine);
		}
		break;
	case WORK_OVALROI:
	case WORK_SKETCHDRAWSEGMENTATION:
	case WORK_RECTROI:
	{
		bool bType = (WIN_MANAGER->getWorkMode() == WORK_RECTROI || WIN_MANAGER->getDCutShape() == DRAW_CUT_SHAPE_ANGULAR);
		bool bDraw = bType ? WIN_MANAGER->pencil_S : WIN_MANAGER->oval_S;
		if (bDraw)
		{
			if (m_LbuttonDown || m_RbuttonDown)
			{
				if (WIN_MANAGER->getWorkMode() == WORK_SKETCHDRAWSEGMENTATION)
				{
#ifndef MULTI_DRAWCUT_MODE
					COLOR _col = WIN_MANAGER->getSeedColor(m_LbuttonDown); //false : backseed, true:foreseed
#else
					COLOR _col = WIN_MANAGER->getDrawingSeedColor(m_LbuttonDown); //false : backseed, true:foreseed
#endif


					p->setPen(QPen(QColor(_col.r, _col.g, _col.b), m_zoomFactorX));
				}
				else
				{
					if (m_LbuttonDown)
					{
						MaskInfo* info = m_pDataContext->volume_data.getCurrentMaskInfo();
						if (info)
							p->setPen(QPen(QColor(info->color.r, info->color.g, info->color.b), m_zoomFactorX));
					}
					else
						p->setPen(QPen(Qt::white, m_zoomFactorX));
				}

				if (bType)
					p->drawPolyline(m_polyLine);
				else if (m_polyLine.size() >= 2)
				{
					QPainterPath region;
					QRect rect;

					rect.setCoords(m_polyLine.at(0).x(), m_polyLine.at(0).y(),
						m_polyLine.at(1).x(), m_polyLine.at(1).y());

					if (m_shift)
					{
						QPoint cent = rect.center();
						double radi = rect.width() < rect.height() ? rect.height() : rect.width();

						rect.setWidth(radi);
						rect.setHeight(radi);
					}

					region.addEllipse(rect);

					p->drawPath(region);
				}
			}
		}
		else
			drawSquareLine(p, WIN_MANAGER->getWorkMode() == WORK_SKETCHDRAWSEGMENTATION);

		if (WIN_MANAGER->getWorkMode() == WORK_SKETCHDRAWSEGMENTATION)
		{
			drawMask(p, VM_MASK0);
			drawMask(p, VM_MASK1);
		}
	}
	break;
	case WORK_ANNOTATION_PATH:
	case WORK_ANNOTATION_PATH_PLAY:
		if ((WIN_MANAGER->bStartAni || WIN_MANAGER->bCreateAni))
			drawAniLine(p);
		break;
	case WORK_NONE:
		if (WIN_MANAGER->getSeedShow() == RT_SHOW)
		{
			//draw seed point
			drawSeedTip(p);
		}

		break;
	default:
		break;
	}

	if (WIN_MANAGER->bShowAniPoint && !((WIN_MANAGER->bStartAni || WIN_MANAGER->bCreateAni)))
	{
		drawAniLine(p);
	}
}

void WindowBase::drawSquareLine(QPainter* p, bool drawcut)
{
	if (drawcut)
	{
		COLOR _col;
#ifndef MULTI_DRAWCUT_MODE
		_col = WIN_MANAGER->getSeedColor(m_LbuttonDown); //false : backseed, true:foreseed
#else
		_col = WIN_MANAGER->getDrawingSeedColor(m_LbuttonDown); //false : backseed, true:foreseed
#endif

		p->setPen(QPen(QColor(_col.r, _col.g, _col.b)));
		p->setBrush(QBrush(QColor(_col.r, _col.g, _col.b)));
	}
	else
	{
		if (m_RbuttonDown)
		{
			//	p->setPen(QPen(Qt::white, m_zoomFactorX));
			p->setPen(QPen(Qt::white));
			p->setBrush(QBrush(Qt::white));
		}
		else
		{
			QColor col = WIN_MANAGER->getSelectedMaskColor();
			//	p->setPen(QPen(col, m_zoomFactorX));
			p->setPen(QPen(col));
			p->setBrush(QBrush(col));
		}
	}

	m_rectRegion.setFillRule(Qt::WindingFill);

	if (!m_rectRegion.isEmpty())
	{
		p->drawPath(m_rectRegion);
	}
}

void WindowBase::drawUnloaded(QPainter* p)
{
	if (p == nullptr)
	{
		return;
	}

	p->fillRect(0, 0, width(), height(), RESOURCE_MANAGER->getUnloadBackGroundColor());

	QImage* unloadImage = RESOURCE_MANAGER->getUnloadImage(m_windowType);
	if (unloadImage != nullptr)
	{
		if (unloadImage->isNull() == false)
		{
			QSize size = (this->size() / 2) - (unloadImage->size() / 2);

			p->drawImage(QPoint(size.width(), size.height()), *unloadImage);
		}
	}
}

void WindowBase::drawAnnotations(QPainter* p)
{
	if (p == nullptr)
	{
		return;
	}

	if (!WIN_MANAGER->bShowAnnoText)
	{
		return;
	}

	for (auto ano = WIN_MANAGER->anotationList.begin(); ano != WIN_MANAGER->anotationList.end(); ++ano)
	{
		if ((*ano)->getType() != AT_TEXT)
		{
			continue;
		}

		if ((*ano)->isAnnoHidden())
		{
			continue;
		}

		(*ano)->drawAnno(this, p, ADV_MPR_PLANE_WINDOW_VIEWER);
	}
}

void WindowBase::drawSeedTip(QPainter* p)
{
	for (int i = 0; i < WIN_MANAGER->seedLocation.size(); i++)
	{
		bool bInArea = false;
		QVector3D seedPoint = WIN_MANAGER->seedLocation.at(i);
		switch (m_windowType)
		{
		case WT_CORONAL:
			if (getDepth() == seedPoint.y())
			{
				bInArea = true;
			}
			break;
		case WT_SAGITTAL:
			if (getDepth() == seedPoint.x())
			{
				bInArea = true;
			}
			break;
		case WT_AXIAL:
		default:
			if (getDepth() == seedPoint.z())
			{
				bInArea = true;
			}
			break;
		}

		if (!bInArea)
		{
			continue;
		}

		p->setPen(QPen(QBrush(QColor(255, 0, 0)), 5));
		QVector3D vec3 = volumeToScreenPosition(seedPoint.x(), seedPoint.y(), seedPoint.z());
		p->drawRoundedRect(QRectF(vec3.x(), vec3.y(), 1, 1), 10, 10, Qt::SizeMode::RelativeSize);
		p->setPen(QPen(QBrush(QColor(247, 235, 235)), 5));
		p->drawRoundedRect(QRectF(vec3.x(), vec3.y(), 0.05, 0.05), 30, 30, Qt::SizeMode::RelativeSize);
	}

	if (WIN_MANAGER->getRGDrawLock())
	{
		for (int i = 0; i < WIN_MANAGER->layerRGLocation.size(); i++)
		{
			bool bInArea = false;
			QVector3D seedPoint = WIN_MANAGER->layerRGLocation.at(i);
			switch (m_windowType)
			{
			case WT_CORONAL:
				if (getDepth() == seedPoint.y())
				{
					bInArea = true;
				}
				break;
			case WT_SAGITTAL:
				if (getDepth() == seedPoint.x())
				{
					bInArea = true;
				}
				break;
			case WT_AXIAL:
			default:
				if (getDepth() == seedPoint.z())
				{
					bInArea = true;
				}
				break;
			}

			if (!bInArea)
			{
				continue;
			}

			p->setPen(QPen(QBrush(QColor(255, 0, 0)), 5));
			QVector3D vec3 = volumeToScreenPosition(seedPoint.x(), seedPoint.y(), seedPoint.z());
			p->drawRoundedRect(QRectF(vec3.x(), vec3.y(), 1, 1), 10, 10, Qt::SizeMode::RelativeSize);
			p->setPen(QPen(QBrush(QColor(247, 235, 235)), 5));
			p->drawRoundedRect(QRectF(vec3.x(), vec3.y(), 0.05, 0.05), 30, 30, Qt::SizeMode::RelativeSize);
		}
		WIN_MANAGER->setRGDrawUnlock();
	}
}

void WindowBase::drawDirection(int startWidth, QPainter* p)
{
	if (p == nullptr)
	{
		return;
	}

	QImage* LDirection, * RDirection, * TDirection, * BDirection;

	TDirection = RESOURCE_MANAGER->getDirectionImage(m_windowType, POS_TOP);
	BDirection = RESOURCE_MANAGER->getDirectionImage(m_windowType, POS_BOTTOM);
	RDirection = RESOURCE_MANAGER->getDirectionImage(m_windowType, POS_RIGHT);
	LDirection = RESOURCE_MANAGER->getDirectionImage(m_windowType, POS_LEFT);

	if (nullptr != TDirection)
		p->drawImage(QPoint(this->width() / 2 - TDirection->size().width() / 2, (TDirection->size().height() + 10)),
			*TDirection);
	if (nullptr != BDirection)
		p->drawImage(QPoint(this->width() / 2 - BDirection->size().width() / 2, this->height() - (BDirection->size().height() + 30)),
			*BDirection);
	if (nullptr != RDirection)
		p->drawImage(QPoint(this->width() - ((float)RDirection->size().width() * 1.5), this->height() / 2 - RDirection->size().height() / 2),
			*RDirection);
	if (nullptr != LDirection)
		p->drawImage(QPoint(LDirection->size().width() / 2, this->height() / 2 - LDirection->size().height() / 2),
			*LDirection);

	QPen prePen = p->pen();
	p->setPen(QPen(QColor(62, 137, 219)));

	p->drawText((this->width() / 2 - startWidth / 2) - 5, this->height() - 30, getDepthString());
	p->drawText((this->width() / 2 + startWidth / 2) - 5, this->height() - 30, getDepthString(false));
	p->setPen(prePen);
}
#ifdef DEV_VER
void WindowBase::drawBoundPoints(QPainter* p)
{
	if (!m_shift) return;
	bool res = false;
	QVector3D points[2];

	p->setPen(Qt::green);

	for (int i = 0; i < WIN_MANAGER->boundPoints.size(); i++)
	{
		switch (m_windowType)
		{
		case WT_CORONAL:
			if (getDepth() == (muint32)WIN_MANAGER->boundPoints.at(i).y())
				res = true;
			break;
		case WT_SAGGITAL:
			if (getDepth() == (muint32)WIN_MANAGER->boundPoints.at(i).x())
				res = true;
			break;
		case WT_AXIAL:
			if (getDepth() == (muint32)WIN_MANAGER->boundPoints.at(i).z())
				res = true;
			break;
		default:
			break;
		}
		if (!res) break;
		QVector3D vec = WIN_MANAGER->boundPoints.at(i);
		points[i] = volumeToScreenPosition(vec.x(), vec.y(), vec.z());
		if (WIN_MANAGER->boundPoints.size() >= 2)
		{
			if (i == 0) continue;
			p->drawLine(QPoint(points[0].x(), points[0].y()), QPoint(points[1].x(), points[1].y()));
		}
		else
			p->drawPoint(QPoint(points[0].x(), points[1].y()));
	}

}
#endif
void WindowBase::drawPreview(QPainter* p)
{
	if (nullptr == p)
	{
		return;
	}

	int window_level = WIN_MANAGER->getWindowLevel();
	int window_width = WIN_MANAGER->getWindowWidth();
	qDebug() << window_level << "(window_level)";
	qDebug() << window_width << "(window_width)";

	unsigned int c_x;
	unsigned int c_y;
	unsigned int c_depth;

	m_pDataContext->volume_data.getLengthForScreen(m_windowType, c_x, c_y, c_depth);
	if (m_depth >= c_depth)
	{
		m_depth = 0;
	}
	qDebug() << c_x << "(c_x)";
	qDebug() << c_y << "(c_y)";
	qDebug() << c_depth << "(c_depth)";

	// preview 저장 포인터
	COLOR* pData = new COLOR[c_x * c_y];

	COLOR color_zero = COLOR(0, 0, 0, 0);
	QColor preColor = WIN_MANAGER->GetTab()->getThreSholdTab()->getPreviewColor();
	COLOR color_mask = COLOR(preColor.red(), preColor.green(), preColor.blue(), 100);
	COLOR color_result;

	bool isDrawcut = false;

	int HUData;
	int startHU, endHU;
	startHU = WIN_MANAGER->getThreLower();
	endHU = WIN_MANAGER->getThreUpper();
	qDebug() << startHU << "(startHU)";
	qDebug() << endHU << "(endHU)";

	bool edge_check = false;
	bool cliped = false;
	bool Colormode = WIN_MANAGER->getColor2DMode();
	bool Invertmode = WIN_MANAGER->getInvertingLUTMode();
	BoundingBoxI box = m_pDataContext->volume_data.getBoundingBoxForScreen(m_windowType);
	COLOR destCol;

	qDebug() << m_flipX << "= m_flipX";
	qDebug() << m_flipY << "= m_flipY";

	for (int y = 0; y < c_y; y++)
	{
		for (int x = 0; x < c_x; x++)
		{
			edge_check = false;

			switch (m_windowType)
			{
			case WT_AXIAL:
				// 16bit voxel을 gray로 변환 (화면에 출력해야 하니까? ???), 뭐야 안쓰자나. updateSlice...() 복붙이라서 그런듯.
				destCol = m_pDataContext->volume_data.getGrayColorData
				(m_flipX ? c_x - x - 1 : x, m_flipY ? c_y - y - 1 : y, m_depth, window_level, window_width, Colormode, Invertmode);

				HUData = m_pDataContext->volume_data.getData(m_flipX ? c_x - x - 1 : x, (m_flipY ? c_y - y - 1 : y), m_depth);

				// HU 범위 설정에 따른 
				if (startHU <= HUData && HUData <= endHU)
					color_result = color_mask; // preview로 설정된 color에 alpha값 100으로 미리보기 출력
				else
					color_result = color_zero;

				break;

			case WT_CORONAL:
				destCol = m_pDataContext->volume_data.getGrayColorData
				(m_flipX ? c_x - x - 1 : x, m_depth, m_flipY ? c_y - y - 1 : y, window_level, window_width, Colormode, Invertmode);
				HUData = m_pDataContext->volume_data.getData(m_flipX ? c_x - x - 1 : x, m_depth, m_flipY ? c_y - y - 1 : y);

				if (startHU <= HUData && HUData <= endHU)
					color_result = color_mask;
				else
					color_result = color_zero;

				break;

			case WT_SAGITTAL:
				destCol = m_pDataContext->volume_data.getGrayColorData
				(m_depth, m_flipX ? c_x - x - 1 : x, m_flipY ? c_y - y - 1 : y, window_level, window_width, Colormode, Invertmode);
				HUData = m_pDataContext->volume_data.getData(m_depth, m_flipX ? c_x - x - 1 : x, m_flipY ? c_y - y - 1 : y);

				if (startHU <= HUData && HUData <= endHU)
					color_result = color_mask;
				else
					color_result = color_zero;

				break;

			default:
				break;
			}

			//////   process cliped area gray
			cliped = false;

			if (m_flipX)
			{
				if (x > box.getMinX(true) || x < box.getMaxX(true) || y < box.minY || y > box.maxY)
				{
					color_result = color_zero;
					cliped = true;
				}
			}
			else if (m_flipY)
			{
				if (x < box.minX || x > box.maxX || y > box.getMinY(true) || y < box.getMaxY(true))
				{
					color_result = color_zero;
					cliped = true;
				}
			}
			else
			{
				if (x < box.minX || x > box.maxX || y < box.minY || y > box.maxY)
				{
					color_result = color_zero;
					cliped = true;
				}
			}

			if (cliped == false)
			{
				if (m_depth < box.minZ || m_depth > box.maxZ)
				{
					color_result = COLOR(color_result.r / 2, color_result.g / 2, color_result.b / 2, color_result.a);
				}
			}

			pData[(c_x * y) + x] = color_result;
		}
	}

	QImage image((uchar*)pData, c_x, c_y, QImage::Format_RGBA8888);
	QRect pos = getPosition();

	p->setRenderHint(QPainter::Antialiasing);
	p->drawImage(pos, image);

	SAFE_DELETES(pData);
}

void WindowBase::drawText(QPainter* p, QPoint pt, QColor color, QString text, QRect* pOutDrawRectArea)
{
	if (text.isEmpty())
	{
		return;
	}
	QFontMetrics fontMet(p->font());

	const QPen prePen = p->pen();

	p->setPen(QPen(color));
	QRect textRect = fontMet.boundingRect(text);
	textRect.moveTo(pt + QPoint(0, textRect.height()));

	p->drawText(textRect.x(), textRect.y(), text);
	p->setPen(prePen);

	if (pOutDrawRectArea)
	{
		*pOutDrawRectArea = textRect;
	}
}

void WindowBase::drawCoordAndUnitTypeText(QPainter* p, QPoint pt, QColor color, QVector3D vec, mint16 HU, QString unitText, QRect* pOutDrawRectArea, QString appendText)
{
	QString text;

	bool isInvalidHUValue = false;
	if (m_pViewerSharedInfo)
	{
		isInvalidHUValue = m_pViewerSharedInfo->HU == SHRT_MIN;
	}

	QString slicePosition = WIN_MANAGER->ConvertText_VoxelPosToSliceImagePos_WithTotalSliceImageCount(vec.x(), vec.y(), vec.z());
	if (isInvalidHUValue)
	{
		text = QString("Coord(%1) %2(none)").arg(slicePosition).arg(unitText) + appendText;
	}
	else
	{
		text = QString("Coord(%1) %3(%2)").arg(slicePosition).arg(HU).arg(unitText) + appendText;
	}


	drawText(p, pt, color, text, pOutDrawRectArea);
}

void WindowBase::drawWorkModeText(QPainter* p, QPoint pt, QColor color, QRect* pOutDrawRectArea)
{
	if (WIN_MANAGER->getWorkMode() != WORK_NONE)
	{
		QString workModeText = QString("%1").arg(WIN_MANAGER->getWorkModeString());
		drawText(p, pt, color, workModeText, pOutDrawRectArea);
	}
}

void WindowBase::drawThresholdModeText(QPainter* p, QPoint pt, QColor color, QRect* pOutDrawRectArea)
{
	QString headerText = "Threshold work mode : ";
	QString modeText;
	if (WIN_MANAGER->isTheadholdWorkModeOn(THRESHOLD_WORK_WITHIN_BRUSH))
	{
		appendTextWithComma(&modeText, "Within Brush");
	}

	if (WIN_MANAGER->isTheadholdWorkModeOn(THRESHOLD_WORK_WITHIN_SELECTED_LAYER))
	{
		appendTextWithComma(&modeText, "Within Selected Layer");
	}

	if (WIN_MANAGER->isTheadholdWorkModeOn(THRESHOLD_WORK_SHOW_PREVIEW))
	{
		appendTextWithComma(&modeText, "Show Preview");
	}

	if (!modeText.isEmpty())
	{
		drawText(p, pt, color, headerText + modeText, pOutDrawRectArea);
	}
}

void WindowBase::appendTextWithComma(QString* pOutAppendText, const QString& appendText)
{
	if (!pOutAppendText->isEmpty())
	{
		*pOutAppendText += ", ";
	}
	*pOutAppendText += appendText;
}

void WindowBase::calcVolumePreview()
{
	if (VOLUME_CALCULATOR->IsIntialized())
	{
		if (VOLUME_CALCULATOR->IsPreviewON())
		{
			/* 현재 Axial, Coronal, Sagittal 평면만 Calculate*/
			int depth = getDepth();

			if (VOLUME_CALCULATOR->CalcHU_Plane(
				&m_pDataContext->volume_data,
				m_windowType,
				depth) == false)
			{
				return;
			}
		}
	}
}

QString WindowBase::getDepthString(bool isFront)
{
	QString str;

	switch (m_windowType)
	{
	case WT_CORONAL:
		if (isFront)
			str = "A";
		else
			str = "P";
		break;
	case WT_SAGITTAL:
		if (isFront)
			str = "R";
		else
			str = "L";
		break;
	case WT_AXIAL:
	default:
		if (isFront)
			str = "S";
		else
			str = "I";
		break;
	}

	return str;
}

void WindowBase::preDepth()
{
	m_depth++;
	m_dutyCheck = true;

	switch (m_windowType)
	{
	case WT_CORONAL:
		if (m_depth >= m_pDataContext->volume_data.getCY())
		{
			m_depth = m_pDataContext->volume_data.getCY() - 1;
		}
		break;
	case WT_SAGITTAL:
		if (m_depth >= m_pDataContext->volume_data.getCX())
		{
			m_depth = m_pDataContext->volume_data.getCX() - 1;
		}
		break;
	case WT_AXIAL:
		if (m_depth >= m_pDataContext->volume_data.getCZ())
		{
			m_depth = m_pDataContext->volume_data.getCZ() - 1;
		}
		break;
	default:
		return;
	}

	switch (WIN_MANAGER->getWorkMode())
	{
	default:
		m_polyLine.clear();
		break;
	}

	if (m_slidebar != nullptr)
	{
		// Mesh 외곽선 업데이트 위한 코드 수정부
		int depth = m_depth;
		if (m_windowType == WT_AXIAL)
		{
#ifdef USE_RIGHT_HAND
			depth = (m_pDataContext->volume_data.getCZ() - 1) - m_depth;
#else
			m_slidebar->setValue(mSlice);
#endif
		}

		m_slidebar->setValue(depth);
		m_pDataContext->volume_data.setDepth(m_windowType, m_depth);
	}
}

void WindowBase::nextDepth()
{
	if (m_depth == 0)
	{
		return;
	}

	m_depth--;
	m_dutyCheck = true;

	switch (WIN_MANAGER->getWorkMode())
	{
	default:
		m_polyLine.clear();
		break;
	}

	if (m_slidebar != nullptr)
	{
		if (WIN_MANAGER->getWheelZoom())
		{
			// Mesh 외곽선 업데이트 위한 코드 수정부
			int depth = m_depth;
			if (m_windowType == WT_AXIAL)
			{
#ifdef USE_RIGHT_HAND
				depth = (m_pDataContext->volume_data.getCZ() - 1) - m_depth;
#else
				m_slidebar->setValue(mSlice);
#endif
			}

			m_slidebar->setValue(depth);
			m_pDataContext->volume_data.setDepth(m_windowType, m_depth);
		}
	}
}

void WindowBase::touchMove(float dx, float dy)
{
	moveSlicePosition(dx, dy);
	renderLater();
}

void WindowBase::touchZoom(float dt)
{
	zoom(dt, m_fZoomStepX * 3, m_fZoomStepY * 3);

	renderLater();
}

void WindowBase::processSlideZ(float dt, bool isSlider, bool isRotate)
{
	if (!isSlider)
	{
		dt = dt > 0 ? 1.0f : -1.0f;
	}

	mip::VECTOR3* planes = nullptr;
	//mip::VECTOR3 slideOffset;
	switch (m_windowType)
	{
	case WT_AXIAL:
		if (isSlider && (!isRotate))
		{
			++dt;
			dt -= (float)m_pDataContext->volume_data.getCZ() / 2.f;
		}
		dt *= (float)m_pDataContext->volume_data.getSpaceZ();
		planes = &m_pDataContext->volume_data.axialPPlane_Base[0];
		//slideOffset = WIN_MANAGER->analSlideOffset[0];
		break;
	case WT_CORONAL:
		if (isSlider && (!isRotate))
		{
			++dt;
			dt -= (float)m_pDataContext->volume_data.getCY() / 2.f;
		}
		dt *= (float)m_pDataContext->volume_data.getSpaceY();
		planes = &m_pDataContext->volume_data.coronalPPlane_Base[0];
		//slideOffset = WIN_MANAGER->analSlideOffset[1];
		break;
	case WT_SAGITTAL:
		if (isSlider && (!isRotate))
			dt -= (float)m_pDataContext->volume_data.getCX() / 2.f;
		dt *= (float)m_pDataContext->volume_data.getSpaceX();
		planes = &m_pDataContext->volume_data.saggitalPPlane_Base[0];
		//slideOffset = WIN_MANAGER->analSlideOffset[2];
		break;
	default:
		break;
	}

	if (planes == nullptr)
	{
		return;
	}

	if (isSlider)
	{
		m_pDataContext->volume_data.getAnal3DPlanes(m_windowType, planes, !isRotate);
	}

	mip::VECTOR3 v = ((planes[0] - planes[1]) ^ (planes[2] - planes[1])).normalize();

	v *= dt;

	for (int n = 0; n < 4; ++n)
	{
		planes[n] += v;
	}

	if (isSlider)
	{
		//if (slideOffset != slideOffset.Zero)
		//{
		//	for (int i = 0; i < 4; i++)
		//		planes[i] += slideOffset;
		//}
	}

	WIN_MANAGER->renderLater_All(true);
}

void WindowBase::zoom(float dt, float factorX, float factorY)
{
	if (dt > 0)
	{
		if ((factorX + m_zoomFactorX) <= m_fMaxZoomFactorX && (factorY + m_zoomFactorY) <= m_fMaxZoomFactorY)
		{
			m_zoomFactorX += factorX;
			m_zoomFactorY += factorY;

			unsigned int c_x;
			unsigned int c_y;
			unsigned int c_depth;
			m_pDataContext->volume_data.getLengthForScreen(m_windowType, c_x, c_y, c_depth);
			QPoint scaled(qRound(float(c_x) * m_zoomFactorX) * 0.08f, qRound(float(c_y) * m_zoomFactorY) * 0.08f);

			int centerX = getSlicePositionX();
			int centerY = getSlicePositionY();
			moveSlicePosition(centerX * 1 / float(scaled.x()), centerY * 1 / float(scaled.y()));
		}
	}
	else
	{
		if ((m_zoomFactorX - factorX) >= m_fMinZoomFactorX && (m_zoomFactorY - factorY) >= m_fMinZoomFactorY)
		{
			m_zoomFactorX -= factorX;
			m_zoomFactorY -= factorY;

			unsigned int c_x;
			unsigned int c_y;
			unsigned int c_depth;
			m_pDataContext->volume_data.getLengthForScreen(m_windowType, c_x, c_y, c_depth);
			QPoint scaled(qRound(float(c_x) * m_zoomFactorX) * 0.08f, qRound(float(c_y) * m_zoomFactorY) * 0.08f);

			int centerX = getSlicePositionX();
			int centerY = getSlicePositionY();
			moveSlicePosition(-centerX * 1 / float(scaled.x()), -centerY * 1 / float(scaled.y()));
		}
	}
}

void WindowBase::zoomSlider(int val, float factorX, float factorY)
{
	m_zoomFactorX = val * factorX;
	m_zoomFactorY = val * factorY;

	unsigned int c_x;
	unsigned int c_y;
	unsigned int c_depth;
	m_pDataContext->volume_data.getLengthForScreen(m_windowType, c_x, c_y, c_depth);
	QPoint scaled(qRound(float(c_x) * m_zoomFactorX), qRound(float(c_y) * m_zoomFactorY));

	int centerX = getSlicePositionX();
	int centerY = getSlicePositionY();

	bool preVal = ((float)m_zoomFactorX / factorX) > val ? false : true;

	if (!preVal)
	{
		centerX = -centerX;
		centerY = -centerY;
	}

	moveSlicePosition(centerX * 1 / float(scaled.x()), centerY * 1 / float(scaled.y()));
}

void WindowBase::moveSlicePosition(int dx, int dy)
{
	m_SlicePosition.setX(m_SlicePosition.x() + dx);
	m_SlicePosition.setY(m_SlicePosition.y() + dy);
}

void WindowBase::slot_OnFullScreen()
{
	if (!m_pDataContext->volume_data.isValidate())
	{
		return;
	}

	if (m_bWorkMode == true)
	{
		return;
	}

	m_fullscreen = !m_fullscreen;

	static QIcon prIcon = RESOURCE_MANAGER->getIcon(ICON_VIEW_FULL_SCREEN, m_nIconSize, m_nIconSize);
	static QIcon reIcon = RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_FULL_SCREEN, m_nIconSize, m_nIconSize);

	if (m_fullscreen)
	{
		m_btnFullScreen->setIcon(prIcon);
	}
	else
	{
		m_btnFullScreen->setIcon(reIcon);
	}

	emit this->setFullScreen(m_fullscreen == true ? this : nullptr);

	renderLater();
}


void WindowBase::slot_OnL3Modify()
{
	MaskInfo* pMaskinfo = m_pDataContext->volume_data.getCurrentMaskInfo();
	QString strCurrentMaskname = m_pDataContext->volume_data.getMaskName(pMaskinfo->uid, true);
	if (strCurrentMaskname != DEEPCATCH_WHOLEBODY_MASKNAME_L3)
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(STR_SELECT_L3));
		return;
	}
	WIN_MANAGER->SelectSliceNum(ST_SINGLE, DEEPCATCH_WHOLEBODY_MASKNAME_L3);
}

void WindowBase::slot_OnAWModify()
{
	MaskInfo* pMaskinfo = m_pDataContext->volume_data.getCurrentMaskInfo();
	QString strCurrentMaskname = m_pDataContext->volume_data.getMaskName(pMaskinfo->uid, true);
	if (strCurrentMaskname != DEEPCATCH_WHOLEBODY_MASKNAME_ABDOMINAL_WAIST)
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(STR_SELECT_ABDOMINAL_WAIST));
		return;
	}
	WIN_MANAGER->SelectSliceNum(ST_MULTIPLE, DEEPCATCH_WHOLEBODY_MASKNAME_ABDOMINAL_WAIST);
}

void WindowBase::slot_OnContextSeed()
{
	//non select 로 설정 (seedshow value 변경 x)
	if (WIN_MANAGER->getSeedShow() & RT_SHOW)
	{
		WIN_MANAGER->setSeedShow(RT_SHOW);
	}
	else
	{
		WIN_MANAGER->setSeedShow(RT_NONE);
	}

	QVector3D pos = screenToVolumePosition3(&m_MousePos);
	mint16 HU = getData(m_MousePos, m_depth);
	WIN_MANAGER->seedLocation.push_back(pos);

	WIN_MANAGER->setRegionGrowingSeed(pos.x(), pos.y(), pos.z(), HU);

}

void WindowBase::slot_OnContextSeedShow()
{
	int showType = 0;

#ifdef SELECT_HIDE
	if (WIN_MANAGER->getSeedShow() & RT_SELECT)
		showType |= RT_SELECT;
#endif

	if (!(WIN_MANAGER->getSeedShow() & RT_SHOW))
		showType |= RT_SHOW;

	WIN_MANAGER->setSeedShow(showType);
}

void WindowBase::slot_OnContextAnnoList()
{
	QVector3D vecVolume = screenToVolumePosition3(&m_MousePos);

	AnnoControlDlg Condlg(&vecVolume, this);
	Condlg.exec();

	if (Condlg.isAdd())
	{
		AnnoTextDlg dlg(this);
		if ((dlg.exec() == QDialog::Accepted) && (dlg.Text.isEmpty() == false))
		{
			checkVolumeArea(vecVolume);

			mip::VECTOR3 vecWorld = getVolumeToWorld(
				&m_pDataContext->volume_data,
				vecVolume.x(), vecVolume.y(), vecVolume.z());

			AnnoString* pAnnoString = new AnnoString(
				vecWorld,
				Annotation::DRAWING_FINISHED,
				dlg.Text,
				dlg.FontSize,
				toCOLOR(dlg.Color)
			);

			ACTION_MANAGER->action_Annotation_Text_Add(pAnnoString);
		}
	}
}

void WindowBase::slot_OnContextSeedApply()
{
	if (WIN_MANAGER->GetTab())
	{
		ThreSholdTab* tab = WIN_MANAGER->GetTab()->getThreSholdTab();

		if (tab)
		{
			tab->ApplyRegionGrowing();
		}
	}
}

void WindowBase::slot_OnContextRangeApply()
{
	if (WIN_MANAGER->GetTab())
	{
		ThreSholdTab* tab = WIN_MANAGER->GetTab()->getThreSholdTab();

		if (tab)
		{
			tab->ApplyThreshold();
		}
	}
}

void WindowBase::slot_OnContextAniPoints()
{
	WIN_MANAGER->bShowAniPoint = !WIN_MANAGER->bShowAniPoint;

	WIN_MANAGER->renderLater_GridView(false);
}

void WindowBase::slot_OnContextAnnoText()
{
	WIN_MANAGER->bShowAnnoText = !WIN_MANAGER->bShowAnnoText;

	WIN_MANAGER->renderLater_GridView();
}

void WindowBase::slot_OnWheelTimeout()
{
	if (WIN_MANAGER->getWheelZoom())
	{
		if (m_latestClickDepth != m_depth)
		{
			m_latestClickDepth = m_depth;
			WIN_MANAGER->setLatestActiveViewDepth(m_latestClickDepth);
		}
		WIN_MANAGER->setLatestActiveViewZoomX(getZoomFactorX());
		WIN_MANAGER->setLatestActiveViewZoomY(getZoomFactorY());

		WIN_MANAGER->renderLater_SubView();

		if (m_cursor.shape() == Qt::BitmapCursor)
		{
			m_cursor = getBitmapCursor();
			this->setCursor(m_cursor);
		}
	}
}

void WindowBase::slot_OnShowBounding()
{
	WIN_MANAGER->setShowBoundingBox(m_btnShowBounding->isChecked());
}

void WindowBase::slot_OnShowBoundaryEdge()
{
	WIN_MANAGER->setBoundingBoxEdge(m_btnBoundaryEdge->isChecked());
}

void WindowBase::slot_OnDepthSlideChanged(int value)
{
	if (WIN_MANAGER->getWheelZoom())
	{
		float dt;
		bool isRotate = m_pDataContext->volume_data.checkRotatePlanes(m_windowType, false);
		if (isRotate)
		{
			float fRan = max(m_pDataContext->volume_data.getSizeX(),
				max(m_pDataContext->volume_data.getSizeY(),
					m_pDataContext->volume_data.getSizeZ()));

			float fSlice =
				fRan == m_pDataContext->volume_data.getSizeX() ? m_pDataContext->volume_data.getSpaceX() :
				fRan == m_pDataContext->volume_data.getSizeY() ? m_pDataContext->volume_data.getSpaceY() : m_pDataContext->volume_data.getSpaceZ();

			if (m_windowType == WT_AXIAL)
			{
				dt = value;
			}
			else if (m_windowType == WT_CORONAL)
			{
				dt = SLIDER_RANGE - value;
			}
			else if (m_windowType == WT_SAGITTAL)
			{
				dt = value;
			}

			dt /= SLIDER_RANGE;
			dt *= fRan;
			dt -= (fRan / 2);
			dt /= fSlice;
		}
		else
		{
			/* 회전 일반 Slider 이동 처리 */
			if (m_windowType == WT_AXIAL)
			{
				dt = value;
			}
			else if (m_windowType == WT_CORONAL)
			{
				dt = m_slidebar->maximum() - value;
			}
			else if (m_windowType == WT_SAGITTAL)
			{
				dt = value;
			}
		}

		processSlideZ(dt, true, isRotate);

		if (m_windowType == WT_AXIAL)
			value = m_pDataContext->volume_data.getCZ() - value - 1;

		WindowBase::setDepth(value);

		updateViewerShaderedInfo();

		WIN_MANAGER->renderLater_GridView(true);
	}
	else //wheel
	{
		zoomSlider(value, m_fZoomStepX, m_fZoomStepY);
		renderLater();
	}
}

void WindowBase::slot_OnDepthSlideReleased()
{
	if (WIN_MANAGER->getWheelZoom())
	{

	}
	else //wheel
	{
		renderLater();

		if (m_latestClickDepth != m_depth)
		{
			m_latestClickDepth = m_depth;
			WIN_MANAGER->setLatestActiveViewDepth(m_latestClickDepth);
		}
		WIN_MANAGER->setLatestActiveViewZoomX(getZoomFactorX());
		WIN_MANAGER->setLatestActiveViewZoomY(getZoomFactorY());

		WIN_MANAGER->renderLater_SubView();

		if (m_cursor.shape() == Qt::BitmapCursor)
		{
			m_cursor = getBitmapCursor();
			this->setCursor(m_cursor);
		}
	}

}

bool WindowBase::eventFilter(QObject* target, QEvent* e)
{
	if (target == nullptr)
	{
		return QWidget::eventFilter(target, e);
	}

	if (!target->inherits("QPushButton") && !target->inherits("QSlider"))
	{
		return QWidget::eventFilter(target, e);
	}

	QEvent::Type _type = e->type();
	if (_type == QEvent::HoverEnter)
	{
		this->unsetCursor();
	}
	else if (
		_type == QEvent::HoverLeave ||
		_type == QEvent::Leave ||
		_type == QEvent::Show)
	{
		if (m_pHoverWidget)
		{
			if (!m_pHoverWidget->isChecked())
			{
				m_pHoverWidget->setIcon(m_IconLeave);
			}
		}

		this->setCursor(m_cursor);
	}

	if (isMouseEventAndContainPos(target, e))
	{
		for (int i = 0; i < m_viewerButtonInfos.size(); ++i)
		{
			if (checkButtonHoverStatus(target, m_viewerButtonInfos[i].Button))
			{
				setButtonHoverStatus(
					m_viewerButtonInfos[i].Button,
					m_viewerButtonInfos[i].IconNormal,
					m_viewerButtonInfos[i].IconHover
				);
			}
		}
	}

	return QWidget::eventFilter(target, e);
}

void WindowBase::setContextMenu()
{
	m_pContextMenu->clear();

	if (m_pActSeedShow)
	{
		if ((WIN_MANAGER->getSeedShow() & RT_SHOW))
		{
			m_pActSeedShow->setText(STRING_MANAGER->getString(STR_SEED_POINT_HIDE));
		}
		else
		{
			m_pActSeedShow->setText(STRING_MANAGER->getString(STR_SEED_POINT_SHOW));
		}
	}

	if (m_pActShowAniPoint)
	{
		if (WIN_MANAGER->bShowAniPoint)
		{
			m_pActShowAniPoint->setText(STRING_MANAGER->getString(STR_HIDE_ANI_POINT));
		}
		else
		{
			m_pActShowAniPoint->setText(STRING_MANAGER->getString(STR_SHOW_ANI_POINT));
		}
	}
	m_pActShowAnnoText->setText(QString("%1(%2)").arg(WIN_MANAGER->bShowAnnoText ?
		STRING_MANAGER->getString(STR_HIDE_ANNO) : STRING_MANAGER->getString(STR_SHOW_ANNO))
		.arg(STRING_MANAGER->getString(STR_ANNO_TEXT)));

	m_pContextMenu->setContextMenuPolicy(Qt::DefaultContextMenu);

	if (m_pActSeedShow)
		m_pContextMenu->addAction(m_pActSeedShow);

	if (m_pActSeedPoint)
		m_pContextMenu->addAction(m_pActSeedPoint);

	if (m_pActSeedApply)
		m_pContextMenu->addAction(m_pActSeedApply);

	if (m_pActRangeApply)
		m_pContextMenu->addAction(m_pActRangeApply);

	m_pContextMenu->addAction(m_pContextMenu->addSeparator());

	if (m_pActAnnoList)
		m_pContextMenu->addAction(m_pActAnnoList);

	if (m_pActShowAnnoText)
		m_pContextMenu->addAction(m_pActShowAnnoText);

	if (m_pActShowAniPoint && WIN_MANAGER->aniLine.count() > 0)
		m_pContextMenu->addAction(m_pActShowAniPoint);

	m_pContextMenu->addAction(m_pContextMenu->addSeparator());
}

bool WindowBase::isShowUsage()
{
	QString value;

	bool res = WIN_MANAGER->getConfigValue(ELEMENT_FILE, "ANNO_PATH_USAGE", value);

	if ((!res) || (1 == value.toInt()))
	{
		return true;
	}

	return false;
}

void WindowBase::setOtherMPRPlaneDepth_InMouseEvent(MAINTAB_TYPE type)
{
	WindowBase* winY = nullptr;
	WindowBase* winX = nullptr;

	WindowBase* axial = nullptr;
	WindowBase* saggital = nullptr;
	WindowBase* coronal = nullptr;

	if (type == MAINTAB_SEGMENTATION)
	{
		axial = WIN_MANAGER->mainSegmentWidget->getWindow(WT_AXIAL);
		saggital = WIN_MANAGER->mainSegmentWidget->getWindow(WT_SAGITTAL);
		coronal = WIN_MANAGER->mainSegmentWidget->getWindow(WT_CORONAL);
	}
	else if (type == MAINTAB_TA)
	{
		axial = WIN_MANAGER->mainTAWidget->getWindow(WT_AXIAL);
		saggital = WIN_MANAGER->mainTAWidget->getWindow(WT_SAGITTAL);
		coronal = WIN_MANAGER->mainTAWidget->getWindow(WT_CORONAL);
	}
	else
	{
		//SEGMETATION, Radiomics에서만 적용. 나머지 SKIP
		return;
	}

	if (m_LbuttonDown)
	{
		if (WIN_MANAGER->getRepositioningMode() == true)
		{
			WIN_MANAGER->setCoordType(REDRAW_TYPE);

			switch (m_windowType)
			{
			case WT_CORONAL:
				winY = axial;
				winX = saggital;

				break;
			case WT_SAGITTAL:
				winY = axial;
				winX = coronal;
				break;
			case WT_AXIAL:
			default:
				winY = coronal;
				winX = saggital;
				break;
			}

			if (winY != nullptr && winX != nullptr)
			{
				QPoint pos = screenToVolumePosition(&m_MousePos);
				winX->setDepth(pos.x());
				winY->setDepth(pos.y());

				WIN_MANAGER->renderLater_GridView(false);
			}
		}
	}
}

bool WindowBase::isMouseEventAndContainPos(QObject* target, QEvent* e)
{
	bool result = false;

	QWidget* w = dynamic_cast<QWidget*>(target);
	{
		QMouseEvent* evt = dynamic_cast<QMouseEvent*>(e);
		if (evt)
		{
			if (w)
			{
				if (w->rect().contains(evt->pos()))
					result = true;
			}
		}
	}
	{
		QHoverEvent* evt = dynamic_cast<QHoverEvent*>(e);
		if (evt)
		{
			if (w)
			{
				if (w->rect().contains(evt->pos()))
					result = true;
			}
		}
	}
	{
		QWheelEvent* evt = dynamic_cast<QWheelEvent*>(e);
		if (evt)
		{
			if (w)
			{
				if (w->rect().contains(evt->pos()))
					result = true;
			}
		}
	}

	return result;
}

bool WindowBase::checkButtonHoverStatus(QObject* target, QPushButton* button)
{
	return (target == button) && (button->isChecked() == false);
}

void WindowBase::setButtonHoverStatus(QPushButton* button, const QIcon& iconNormal, const QIcon& iconHover)
{
	button->setIcon(iconHover);
	m_pHoverWidget = button;
	m_IconLeave = iconNormal;
}

void WindowBase::registerViewerButtonInfo(QPushButton* button, const QIcon& iconNormal, const QIcon& iconHover, const QIcon& iconPressed)
{
	if (button != nullptr)
	{
		ViewerButtonInfo info;
		info.Button = button;
		info.IconNormal = iconNormal;
		info.IconHover = iconHover;
		info.IconPressed = iconPressed;
		m_viewerButtonInfos.push_back(info);
	}
}

void WindowBase::updateViewerShaderedInfo()
{
	QVector3D vPos = getShaderSliderDepthPosition();

	if (m_pViewerSharedInfo)
	{
		m_pViewerSharedInfo->VolumeVoxelPosition = vPos;
		m_pViewerSharedInfo->HU = m_pDataContext->volume_data.getData(vPos.x(), vPos.y(), vPos.z());
	}
}

void WindowBase::updateZoomInfo()
{
	unsigned int c_x;
	unsigned int c_y;
	unsigned int c_depth;
	m_pDataContext->volume_data.getLengthForScreen(m_windowType, c_x, c_y, c_depth);

	int width_px = this->width();
	int height_px = this->height();

	float width_mm = (float)c_x * m_fSpacingX;
	float height_mm = (float)c_y * m_fSpacingY;

	float fZoomX = (float)width_px / width_mm;
	float fZoomY = (float)height_px / height_mm;

	float fZoom = 0.f;
	if (fZoomX > fZoomY)
		fZoom = fZoomY;
	else
		fZoom = fZoomX;

	m_zoomFactorX = fZoom * m_fSpacingX;
	m_zoomFactorY = fZoom * m_fSpacingY;

	m_fZoomStepX = m_zoomFactorX * 0.1f;
	m_fZoomStepY = m_zoomFactorY * 0.1f;

	m_fMinZoomFactorX = m_zoomFactorX - (m_fZoomStepX * (float)m_nMinZoomNum);
	m_fMinZoomFactorY = m_zoomFactorY - (m_fZoomStepY * (float)m_nMinZoomNum);

	m_fMaxZoomFactorX = m_zoomFactorX + (m_fZoomStepX * (float)(m_nZoomStepCnt - 10));
	m_fMaxZoomFactorY = m_zoomFactorY + (m_fZoomStepY * (float)(m_nZoomStepCnt - 10));
}

void WindowBase::InitZoomFactor(float x, float y, bool isUpdate)
{
	m_fSpacingX = x;
	m_fSpacingY = y;

	if (isUpdate)
	{
		/* 이전 Zoom Count 초기화 */
		float fOldZoomCntX = m_zoomFactorX / m_fZoomStepX;
		float fOldZoomCntY = m_zoomFactorY / m_fZoomStepY;

		/* Zoom Info 값 Update */
		updateZoomInfo();

		/* zoom Factor 추가값 refresh */
		m_zoomFactorX += m_fZoomStepX * (fOldZoomCntX - (float)10);
		m_zoomFactorY += m_fZoomStepY * (fOldZoomCntY - (float)10);
	}
	else
	{
		/* Zoom Info 값 Update */
		updateZoomInfo();

		/* zoom Factor 추가값 refresh */
		m_zoomFactorX -= m_fZoomStepX * 2;
		m_zoomFactorY -= m_fZoomStepY * 2;
	}
	m_fZoomLow = m_pDataContext->volume_data.getMinSpace(true);
}

void WindowBase::setZoomFactor(float x, float y)
{
	m_zoomFactorX = x;
	m_zoomFactorY = y;
}

void WindowBase::SetZoomFactorByStepNum(float fStepNumX, float fStepNumY)
{
	updateZoomInfo();

	// Step Num 값으로 Zoom 업데이트 
	m_zoomFactorX += m_fZoomStepX * (fStepNumX - (float)10);
	m_zoomFactorY += m_fZoomStepY * (fStepNumY - (float)10);
}

void WindowBase::GetZoomStepNum(float& fStepNumX, float& fStepNumY)
{
	fStepNumX = m_zoomFactorX / m_fZoomStepX;
	fStepNumY = m_zoomFactorY / m_fZoomStepY;
}

void WindowBase::resetSlicePosition()
{
	m_SlicePosition.setX(0);
	m_SlicePosition.setY(0);
}

void WindowBase::setSlicePosition(int x, int y)
{
	m_SlicePosition.setX(x);
	m_SlicePosition.setY(y);
}

int WindowBase::getSlicePositionX()
{
	return m_SlicePosition.x();
}

int WindowBase::getSlicePositionY()
{
	return m_SlicePosition.y();
}

float WindowBase::getZoomFactorX()
{
	return m_zoomFactorX;
}

float WindowBase::getZoomFactorY()
{
	return  m_zoomFactorY;
}

float WindowBase::getFactorX()
{
	return m_fZoomStepX;
}

float WindowBase::getFactorY()
{
	return  m_fZoomStepY;
}

QVector3D WindowBase::getLatestClickPos()
{
	return m_latestClickPos;
}

bool WindowBase::getFlipY()
{
	return m_flipY;
}

bool WindowBase::getFlipX()
{
	return m_flipX;
}

bool WindowBase::isAnnoEditMode()
{
	return m_annoEditMode;
}

void WindowBase::setAnnoEditMode(bool editmode)
{
	m_annoEditMode = editmode;
}

QCursor WindowBase::getBitmapCursor()
{
	if (m_list.size() == 0)
	{
		createCursorShape();
	}

	QCursor cur;

	bool bType = (WIN_MANAGER->getWorkMode() == WORK_RECTROI || WIN_MANAGER->getDCutShape() == DRAW_CUT_SHAPE_ANGULAR);
	bool res = bType ? WIN_MANAGER->pencil_S : WIN_MANAGER->oval_S;

	if (res)
	{
		cur.setShape(Qt::ArrowCursor);
	}
	else
	{
		QRect rect;
		auto coord1 = m_list.begin();
		auto coord2 = m_list.last();
		cur.setShape(Qt::BitmapCursor);
		float zoomLow = m_fZoomLow;
		rect.setCoords(m_MousePos.x() + (coord1->x() * zoomLow), m_MousePos.y() + (coord1->y() * zoomLow),
			m_MousePos.x() + (coord2.x() * zoomLow), m_MousePos.y() + (coord2.y() * zoomLow));

		QPixmap pix(rect.width() + 2, rect.height() + 2);
		pix.fill(Qt::transparent);
		QPainter p(&pix);
		//p.setPen(QPen(QColor(Qt::black)));
		//p.setBrush(QBrush(Qt::white));
		QPen pen;
		pen.setColor(QColor(Qt::black));
		pen.setWidth(WIN_MANAGER->getBrushThickness());
		p.setPen(pen);
		p.setBrush(QBrush(QColor(255, 255, 255, WIN_MANAGER->getBrushOpacity())));

		if (bType)
		{
			p.drawRect(0, 0, rect.width() + 1, rect.height() + 1);
		}
		else
		{
			p.drawEllipse(0, 0, rect.width() + 1, rect.height() + 1);
		}
		p.end();

		cur = (pix);
	}

	return cur;
}

void WindowBase::dropEvent(QDropEvent* event)
{
	const QMimeData* mimeData = event->mimeData();
	if (mimeData->text().isEmpty())
	{
		event->ignore();
		return;
	}

	QString filename;
	QUrl fileUrl = mimeData->urls().at(0);

	filename = fileUrl.toLocalFile();

	/*local/remote 구분*/
	if (-1 != filename.indexOf("//"))
	{
		//경고창
		QMessageBox::warning(this, STRING_MANAGER->getString(STR_REMOTE_FILE),
			STRING_MANAGER->getString(STR_REMOTE_FILE_DELAYED));
	}

	//	WIN_MANAGER->fileOpen(filename);
	WIN_MANAGER->mainWindow->fileOpen(filename);

	event->acceptProposedAction();
}

void WindowBase::dragEnterEvent(QDragEnterEvent* ev)
{
	ev->accept();
}

void WindowBase::hideControls()
{
	if (m_btnFullScreen)
		m_btnFullScreen->hide();

	if (m_btnL3Modify)
		m_btnL3Modify->hide();

	if (m_btnAbdominalWaistModify)
		m_btnAbdominalWaistModify->hide();

	if (m_btnShowBounding)
		m_btnShowBounding->hide();

	if (m_btnBoundaryEdge)
		m_btnBoundaryEdge->hide();
}

void WindowBase::showControls()
{
	if (m_btnFullScreen)
		m_btnFullScreen->show();

	if (m_btnL3Modify)
		m_btnL3Modify->show();

	if (m_btnAbdominalWaistModify)
		m_btnAbdominalWaistModify->show();

	if (m_btnShowBounding)
		m_btnShowBounding->show();

	if (m_btnBoundaryEdge)
		m_btnBoundaryEdge->show();
}

