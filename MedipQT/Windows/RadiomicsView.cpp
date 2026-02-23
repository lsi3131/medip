#include "stdafx.h"
#include "RadiomicsView.h"
#include "windowManager.h"
#include "System/resourceManager.h"
#include "System/styleManager.h"
#include "Dialogs/HeightMapDlg.h"
#include "Main/MainTAWidget.h"
#include "graphics/AnnotationFactory.h"
#include "Actions/ActionManager.h"
#include "DataContext.h"

RadiomicsView::RadiomicsView(QWidget* parent, WINDOW_TYPE winType, DataContext* pDataContext, MPRViewSharedInfo* pViewerSharedInfo) :
	WindowBase(parent, pViewerSharedInfo)
	, m_pDataContext(pDataContext)
{
	setType(winType);

	if (winType == WT_AXIAL)
	{
		m_flipY = false;
	}

	m_heightMapColorTable = m_pColorTable->getHeightMapColorTable();

	if (m_pColorTable == nullptr)
	{
		m_pColorTable = new ColorTable(eColorGradientPostion::eLeftPos, this);
		m_pColorTable->setFixedSize(m_pColorTable->getWidgetSize());
		m_pColorTable->setStops();
		m_pColorTable->setGradientImage();
	}

	m_pColorTable->hide();

	initInScreenMenu();
	initInScreenRightTopMenu();
	initInScreenRightBottomMenu();
	initInScreenLeftBottomMenu();

	registerViewerButtonInfo(m_btnHeightMap,
		RESOURCE_MANAGER->getIcon(ICON_NON_RADIOMICS_HEIGHTMAP, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_RADIOMICS_HEIGHTMAP_HOVER, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_RADIOMICS_HEIGHTMAP, m_nIconSize, m_nIconSize)
	);

	registerViewerButtonInfo(m_btnAnnoRect,
		RESOURCE_MANAGER->getIcon(ICON_NON_ANNO_RECT, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_ANNO_RECT_HOVER, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_ANNO_RECT, m_nIconSize, m_nIconSize)
	);

	registerViewerButtonInfo(m_btnFullScreen,
		RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_FULL_SCREEN, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_VIEW_FULL_SCREEN_HOVER, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_VIEW_FULL_SCREEN, m_nIconSize, m_nIconSize)
	);

	registerViewerButtonInfo(m_btnBoundaryEdge,
		RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_BOUNDARY, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_BOUNARY_HOVER, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_VIEW_BOUNARY, m_nIconSize, m_nIconSize)
	);

	registerViewerButtonInfo(m_btnShowBounding,
		RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_COORD, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_VIEW_COORD_HOVER, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_VIEW_COORD, m_nIconSize, m_nIconSize)
	);

	setShowBoundingBoxMode(WIN_MANAGER->getShowClippingBox());
	setBoundingBoxEdgeMode(WIN_MANAGER->getEdgeMode());
}

RadiomicsView::~RadiomicsView()
{

}

void RadiomicsView::enableControls(bool bEnable)
{
	WindowBase::enableControls(bEnable);

	if (m_btnAnnoRect)
		m_btnAnnoRect->setEnabled(bEnable);
}

void RadiomicsView::resetUI()
{
	if (m_slidebar)
	{
		if (m_windowType == WT_AXIAL)
		{
			m_depth = m_pDataContext->volume_data.getCZ() / 2;
		}
		else if (m_windowType == WT_CORONAL)
		{
			m_depth = m_pDataContext->volume_data.getCY() / 2;
		}
		else if (m_windowType == WT_SAGITTAL)
		{
			m_depth = m_pDataContext->volume_data.getCX() / 2;
		}

		if (WIN_MANAGER->getWheelZoom())
		{
			InitWheelSliderFunc(true);
		}
		else
		{
			InitWheelSliderFunc(false);
		}
		m_slidebar->show();

		if (m_windowType == WT_AXIAL)
		{
			m_latestClickPos.setX(m_pDataContext->volume_data.getCX() / 2);
			m_latestClickPos.setY(m_pDataContext->volume_data.getCY() / 2);
			m_latestClickPos.setZ(m_pDataContext->volume_data.getCZ() / 2);
		}
		else if (m_windowType == WT_CORONAL)
		{
			m_latestClickPos.setX(m_pDataContext->volume_data.getCZ() / 2);
			m_latestClickPos.setY(m_pDataContext->volume_data.getCX() / 2);
			m_latestClickPos.setZ(m_pDataContext->volume_data.getCY() / 2);
		}
		else if (m_windowType == WT_SAGITTAL)
		{
			m_latestClickPos.setX(m_pDataContext->volume_data.getCZ() / 2);
			m_latestClickPos.setY(m_pDataContext->volume_data.getCY() / 2);
			m_latestClickPos.setZ(m_pDataContext->volume_data.getCX() / 2);
		}

		m_pDataContext->volume_data.setDepth(m_windowType, m_depth);
	}

	enableControls(WIN_MANAGER->IsEnableViewControls());
}

void RadiomicsView::setDepth(uint depth)
{
	if (m_slidebar != NULL)
	{
		if (WIN_MANAGER->getWheelZoom())
		{
			if (m_windowType == WT_AXIAL)
			{
#ifdef USE_RIGHT_HAND
				m_slidebar->setValue((m_pDataContext->volume_data.getCZ() - 1) - depth);
#else
				m_slidebar->setValue(mSlice);
#endif
			}
			else
			{
				m_slidebar->setValue(depth);
			}
		}
	}
}

void RadiomicsView::setWorkMode(RADIOMICS_WORK_MODE mode, bool iconRefresh)
{
	static QIcon annoRectIcon[] = {
		RESOURCE_MANAGER->getIcon(ICON_NON_ANNO_RECT, m_nIconSize, m_nIconSize) ,
		RESOURCE_MANAGER->getIcon(ICON_ANNO_RECT, m_nIconSize, m_nIconSize) };

	bool chkMode = false;
	bool chkRefresh = false;

	WIN_MANAGER->setRadiomicsWorkMode(mode);

	if (m_btnAnnoRect)
	{
		if (chkRefresh = ((mode == RADIOMICS_WORK_ANNO_RECTANGLE)) != (chkMode = m_btnAnnoRect->isChecked()))
		{
			chkMode = !chkMode;
			m_btnAnnoRect->setChecked(chkMode);
			m_btnAnnoRect->setIcon(annoRectIcon[chkMode]);
		}
		else if (!chkRefresh && iconRefresh)
		{
			m_btnAnnoRect->setIcon(annoRectIcon[chkMode]);
		}
	}
}

void RadiomicsView::render(QPainter* p)
{
	WindowBase::render(p);

	if (p == NULL)
		return;

	if (m_pDataContext->volume_data.isValidate() && WIN_MANAGER->getRenderable() == true && !WIN_MANAGER->IsCropOn())
	{
		p->fillRect(0, 0, width(), height(), QColor(0, 0, 0));

		//	QPoint pos = screenToVolumePosition(&m_MousePos);
		QVector3D vec;
		mint16 HU;

		if (m_pViewerSharedInfo)
		{
			vec = m_pViewerSharedInfo->VolumeVoxelPosition;
			HU = m_pViewerSharedInfo->HU;
		}
		else
		{
			vec = screenToVolumePosition3(&(m_MousePos));
			HU = getData(m_MousePos, m_depth);
		}
		checkVolumeArea(vec);

		drawSlice(p);

		p->setPen(QPen(QColor(255, 255, 255)));

		QFontMetrics fontMet(p->font());

		QString appendText;

		QRect drawTextRect;
		//if (WIN_MANAGER->mainTabType == MAINTAB_SEGMENTATION)
		{
			if (WIN_MANAGER->bThrePreview)
				drawPreview(p);
			else
			{
				drawMaskAll(p, m_dutyCheck);
				m_dutyCheck = false;
			}

			renderHeatMap(p);

			if (m_pDataContext->volume_data_PET.isValidate()
				&& m_pDataContext->volume_data_PET.isVisible())
			{
				drawSlice_PET(p);

				float SUV = getData_PET_SUV(m_MousePos, m_depth);
				appendText += QString(" , SUV(%1)").arg(SUV);
			}

			drawMouseWork(p);

			drawAnnotations(p);

			if (WIN_MANAGER->getDirectionMode())
			{
				drawDirection(m_slidebar->size().width(), p);
			}


			QPoint ptCoord(10, 0);
			QString unitText = WIN_MANAGER->getUnitString();
			drawCoordAndUnitTypeText(p, ptCoord, Qt::white, vec, HU, unitText, &drawTextRect, appendText);

			QPoint ptWordMode(10, drawTextRect.top() + 5);
			drawWorkModeText(p, ptWordMode, QColor(62, 137, 219), &drawTextRect);

			drawBoundLine(p);

			drawCoordLine(p);

#ifdef DEV_VER
			drawBoundPoints(p);
#endif

		}

		muint32 downscaledCnt = m_pDataContext->volume_data.getDownScaledCnt();
		if (downscaledCnt > 0)
		{
			QString text = QString("1/%1 Downscaled").arg(downscaledCnt == 1 ? 2 : 4);
			QPoint ptDownScaleCount(10, drawTextRect.top() + 5);
			drawText(p, ptDownScaleCount, QColor(62, 137, 219), text, &drawTextRect);
		}


		drawWaterMark(p);
	}
	else
	{
		drawUnloaded(p);
	}
}

void RadiomicsView::initInScreenMenu()
{
	WindowBase::initInScreenMenu();
}

void RadiomicsView::initInScreenRightTopMenu()
{
	WindowBase::initInScreenRightTopMenu();

	m_btnAnnoRect = new QPushButton(this);
	if (m_btnAnnoRect)
	{
		m_btnAnnoRect->setIcon(RESOURCE_MANAGER->getIcon(ICON_NON_ANNO_RECT, m_nIconSize, m_nIconSize));
		m_btnAnnoRect->setIconSize(QSize(m_nIconSize, m_nIconSize));
		m_btnAnnoRect->setFixedSize(QSize(m_nIconSize, m_nIconSize));
		m_btnAnnoRect->setCheckable(true);
		m_btnAnnoRect->setMouseTracking(true);
		m_btnAnnoRect->installEventFilter(this);
		m_btnAnnoRect->hide();
		m_btnAnnoRect->setStyleSheet("color: black;");
		m_btnAnnoRect->setToolTip("Annotation - Rectangle ROI");
		m_btnAnnoRect->setObjectName("AnalRectangleROI");
		connect(m_btnAnnoRect, &QPushButton::clicked, this, &RadiomicsView::slot_OnAnnoRect);
	}
}

void RadiomicsView::initInScreenRightBottomMenu()
{
	WindowBase::initInScreenRightBottomMenu();

	if (m_btnHeightMap == nullptr)
	{
		m_btnHeightMap = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_RADIOMICS_HEIGHTMAP, m_nIconSize, m_nIconSize), "", this);
		m_btnHeightMap->setIconSize(QSize(34, 34));
		m_btnHeightMap->setFixedSize(QSize(34, 34));
		m_btnHeightMap->setCheckable(false);
		m_btnHeightMap->setMouseTracking(true);
		m_btnHeightMap->installEventFilter(this);
		m_btnHeightMap->setToolTip("HeightMap");
		m_btnHeightMap->setObjectName("HeightMap");
		connect(m_btnHeightMap, &QPushButton::clicked, this, &RadiomicsView::slot_onHeightMap);

		m_btnHeightMap->hide();
	}
}

void RadiomicsView::initInScreenLeftBottomMenu()
{
	WindowBase::initInScreenLeftBottomMenu();
}

void RadiomicsView::updateScreenMenu(int width, int height)
{
	WindowBase::updateScreenMenu(width, height);

	if (m_btnHeightMap)
	{
		m_btnHeightMap->move(
			width - m_btnHeightMap->width(),
			height - m_btnHeightMap->height() - m_btnShowBounding->height() - 5);
	}

	int h = 0;
	if (m_btnAnnoRect)
	{
		m_btnAnnoRect->move(width - m_btnAnnoRect->width(), h);
		h += m_btnAnnoRect->height();
	}
}

QVector3D RadiomicsView::getShaderSliderDepthPosition()
{
	QVector3D pos;

	//if (WIN_MANAGER->mainTabType == MAINTAB_TA)
	{
		WindowBase* pAxial = WIN_MANAGER->mainTAWidget->getWindow(WT_AXIAL);
		WindowBase* pCoronal = WIN_MANAGER->mainTAWidget->getWindow(WT_CORONAL);
		WindowBase* pSagittal = WIN_MANAGER->mainTAWidget->getWindow(WT_SAGITTAL);

		int _x_idx = pSagittal->getDepth();
		int _y_idx = pCoronal->getDepth();
		int _z_idx = pAxial->getDepth();

		pos = QVector3D(_x_idx, _y_idx, _z_idx);
	}

	return pos;
}

void RadiomicsView::showControls()
{
	WindowBase::showControls();

	if (m_btnHeightMap && WIN_MANAGER->pRadiomics3DVolume != nullptr)
		m_btnHeightMap->show();

	if (m_btnAnnoRect)
		m_btnAnnoRect->show();
}

void RadiomicsView::hideControls()
{
	WindowBase::hideControls();

	if (m_btnHeightMap)
		m_btnHeightMap->hide();

	if (m_btnAnnoRect)
		m_btnAnnoRect->hide();
}

void RadiomicsView::wheelEvent(QWheelEvent* ev)
{
	this->setFocus();

	float delta = ev->delta();

	if (m_pDataContext->volume_data.isValidate() == false) 
		return;

	if (m_touchZoomEvent == true) 
		return;

	if (m_bWorkMode == true) 
		return;

	if (WIN_MANAGER->getWheelZoom())
	{
		if (m_WheelTimer)
		{
			m_WheelTimer->stop();
			m_WheelTimer->start(500);
		}
		zoom(delta, m_fZoomStepX * 2, m_fZoomStepY * 2);
	}
	else
	{
		if (delta > 0)
		{
#ifdef USE_RIGHT_HAND
			int val = m_depth;
			val--;
			if (val < 0)
				WindowBase::setDepth(0);
			else
				WindowBase::setDepth(m_depth - 1);
#else
			WindowBase::setDepth((mSlice + 1));
#endif
		}
		else
		{
#ifdef USE_RIGHT_HAND
			WindowBase::setDepth((m_depth + 1));
#else
			WindowBase::setDepth(mSlice - 1);
#endif
		}

		bool res = false;
		int direction = -1;
		if (m_windowType == WT_AXIAL || m_windowType == WT_SAGITTAL)
		{
			direction = 1;
		}
		else if (m_windowType == WT_CORONAL)
		{
			direction = -1;
		}
		delta *= direction;

		if (!m_pDataContext->volume_data.checkRotatePlanes(m_windowType, false))
		{
			res = m_pDataContext->volume_data.getAllThickness(0.0f, delta, m_windowType);
		}

		if (!res)
		{
			processSlideZ(delta);
		}

		updateViewerShaderedInfo();

		WIN_MANAGER->renderLater_GridView(false);
	}
	renderLater();
}

void RadiomicsView::mouseMoveEvent(QMouseEvent* e)
{
	checkModifiers(e);

	if (WIN_MANAGER->getMoveFocus())
	{
		this->setFocus();
	}

	WIN_MANAGER->setLatestActiveViewType(m_windowType);

	if (m_pDataContext->volume_data.isValidate() == false)
	{
		return;
}

	m_MousePos = e->pos();

	if (m_mouseDown)
	{
		switch (e->buttons())
		{
		case Qt::LeftButton:
			break;
		case Qt::MiddleButton:
			moveSlicePosition(m_MousePos.x() - m_preMousePos.x(), m_MousePos.y() - m_preMousePos.y());
			renderLater();
			break;
		case Qt::RightButton:
			break;
		default:
			break;
		}
	}

	processWork_MouseMove();

	WindowBase::mouseMoveEvent(e);
}

void RadiomicsView::mousePressEvent(QMouseEvent* e)
{
	WindowBase::mousePressEvent(e);

	processWork_MousePress();
}

void RadiomicsView::mouseReleaseEvent(QMouseEvent* e)
{
	WindowBase::mouseReleaseEvent(e);

	processWork_MouseRelease();
}

void RadiomicsView::mouseDoubleClickEvent(QMouseEvent* e)
{
	WindowBase::mouseDoubleClickEvent(e);
}

bool RadiomicsView::eventFilter(QObject* target, QEvent* e)
{
	return WindowBase::eventFilter(target, e);;
}

void RadiomicsView::preDepth()
{
	WindowBase::preDepth();
}

void RadiomicsView::nextDepth()
{
	WindowBase::nextDepth();
}

void RadiomicsView::renderHeatMap(QPainter* p)
{
	if (WIN_MANAGER->pRadiomics3DVolume == nullptr)
	{
		m_pColorTable->hide();
		m_pColorTable->clearCategoryMap();

		return;
	}

	if (WIN_MANAGER->bRadiomicsDataChangeFlag[m_windowType - WT_AXIAL])
	{
		m_pColorTable->setGradientCategoryValue(WIN_MANAGER->vecRadiomicsColorCategory);
		WIN_MANAGER->bRadiomicsDataChangeFlag[m_windowType - WT_AXIAL] = false;
	}

	m_pColorTable->show();



	unsigned int c2d_width;
	unsigned int c2d_height;
	unsigned int c2d_depth;

	m_pDataContext->volume_data.getLengthForScreen(m_windowType, c2d_width, c2d_height, c2d_depth);

	int count = 1;
	// 체크박스 켰을 때 처음 한번만 메모리 할당으로 바꿀 것

	float* p2DMap = new float[c2d_width * c2d_height * count];

	// outputVolume 데이터 0~1
	recon2DMap(WIN_MANAGER->pRadiomics3DVolume, p2DMap, c2d_width, c2d_height, c2d_depth);
	// 
	QSize tableSize = m_heightMapColorTable.size();

	QImage image(c2d_width, c2d_height, QImage::Format_RGBA8888);

	// make heatmap
	for (int j = 0; j < c2d_height; j++)
	{
		for (int i = 0; i < c2d_width; i++)
		{
			if (p2DMap[j * c2d_width + i] == -4.5f)
			{
				image.setPixel(i, j, 0);
			}
			else
			{
				//int tablePos = vec3DVolume[j*c2d_width + i] / 256.f * (tableSize.height());
				int tablePos = round(p2DMap[j * c2d_width + i] * (tableSize.height() - 1));
				uint color = m_heightMapColorTable.pixel(QPoint(0, tablePos));
				image.setPixel(i, j, color);
			}
		}
	}

	SAFE_DELETES(p2DMap);
	/////////////////////////////////

	p->drawImage(getPosition(), image);
}

void RadiomicsView::recon2DMap(float* pIn3DData, float* pOutData, unsigned int c2d_width, unsigned int c2d_height, unsigned int c2d_depth)
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
			pOutData[index2D] = (pIn3DData[index3D] + 1.f) * 0.5f;
		}
	}
}

void RadiomicsView::drawSlice_PET(QPainter* p)
{
	int x = m_depth;
	int y = m_depth;
	int z = m_depth;

	/* CT Depth px-> Real Position mm -> PET Depth px 변환*/
	mip::VECTOR3 realDepthPos = m_pDataContext->volume_data.toWorldPosition(mip::VECTOR3(x, y, z));
	mip::VECTOR3 petDepthPos = m_pDataContext->volume_data_PET.VolumeInfo()->ToVolumePosition(realDepthPos);

	std::vector<COLOR> color2DMap;

	QRect drawRegion = getPosition();
	int widgetWidth = drawRegion.width();
	int widgetHeight = drawRegion.height();
	int depth = 0;
	VOLUME_DATA_PET::EPlane plain;

	if (m_windowType == WT_AXIAL)
	{
		depth = petDepthPos.z;
		plain = VOLUME_DATA_PET::EPlane::PL_XY;
	}
	else if (m_windowType == WT_CORONAL)
	{
		depth = petDepthPos.y;
		plain = VOLUME_DATA_PET::EPlane::PL_XZ;
	}
	else if (m_windowType == WT_SAGITTAL)
	{
		depth = petDepthPos.x;
		plain = VOLUME_DATA_PET::EPlane::PL_YZ;
	}
	else
	{
		qInfo() << "invalid window type : " << m_windowType;
		return;
	}

	if (m_pDataContext->volume_data_PET.get2DColorMap(color2DMap,
		depth,
		widgetWidth,
		widgetHeight,
		plain,
		m_flipX,
		m_flipY) == false)
	{
		return;
	}

	/////////////////////////////////
	QImage image((uchar*)color2DMap.data(), widgetWidth, widgetHeight, QImage::Format_RGBA8888);

	p->setOpacity(m_pDataContext->volume_data_PET.opacity());
	p->drawImage(drawRegion, image);
	p->setOpacity(1.0);
}

void RadiomicsView::drawAnnotations(QPainter* p)
{
	if (m_pCurAnnotation)
	{
		m_pCurAnnotation->drawAnno(this, p, ADV_MPR_PLANE_WINDOW_VIEWER);
	}

	for (auto ano = WIN_MANAGER->anotationList.begin(); ano != WIN_MANAGER->anotationList.end(); ++ano)
	{
		if ((*ano)->isAnnoHidden())
		{
			continue;
		}

		if ((*ano)->getType() == AT_RECTANGLE)
		{
			if (!WIN_MANAGER->bShowAnnoRectangle)
			{
				continue;
			}

			(*ano)->drawAnno(this, p, ADV_MPR_PLANE_WINDOW_VIEWER);
		}
	}
}

float RadiomicsView::getData_PET_SUV(const QPoint& screenPos, muint32 depth)
{
	QPoint p = screenToPETVolumePosition(&screenPos);

	int x = m_depth;
	int y = m_depth;
	int z = m_depth;

	mip::VECTOR3 realDepthPos = m_pDataContext->volume_data.toWorldPosition(mip::VECTOR3(x, y, z));
	mip::VECTOR3 petDepthPos = m_pDataContext->volume_data_PET.VolumeInfo()->ToVolumePosition(realDepthPos);

	if (m_windowType == WT_AXIAL)
	{
		depth = petDepthPos.z;
		return m_pDataContext->volume_data_PET.getData(p.x(), p.y(), depth);
	}
	else if (m_windowType == WT_CORONAL)
	{
		depth = petDepthPos.y;
		return m_pDataContext->volume_data_PET.getData(p.x(), depth, p.y());
	}
	else if (m_windowType == WT_SAGITTAL)
	{
		depth = petDepthPos.x;
		return m_pDataContext->volume_data_PET.getData(depth, p.x(), p.y());
	}
	else
	{
		qInfo() << "get PET SUV failed. invalid window type : " << m_windowType;
		return 0;
	}
}

QPoint RadiomicsView::screenToPETVolumePosition(const QPoint* point)
{
	unsigned int c_x_PET;
	unsigned int c_y_PET;
	unsigned int c_depth_PET;
	m_pDataContext->volume_data_PET.getLengthForScreen(m_windowType, c_x_PET, c_y_PET, c_depth_PET);

	unsigned int c_x_CT;
	unsigned int c_y_CT;
	unsigned int c_depth_CT;
	m_pDataContext->volume_data.getLengthForScreen(m_windowType, c_x_CT, c_y_CT, c_depth_CT);

	float factorX = m_zoomFactorX * (c_x_CT / (float)c_x_PET);
	float factorY = m_zoomFactorY * (c_y_CT / (float)c_y_PET);

	QSizeF imgSize = size();
	QSizeF volumeSize = QSizeF(float(c_x_PET) * factorX, float(c_y_PET) * factorY);

	QPointF p = QPointF(m_flipX ? size().width() - point->x() - 1 : point->x(), m_flipY ? size().height() - point->y() - 1 : point->y());
	QPointF scaled(float(c_x_PET) * factorX, float(c_y_PET) * factorY);
	QPointF imageZero = (QPointF(imgSize.width(), imgSize.height()) / 2) - (scaled / 2);

	p = ((p - imageZero) - QPointF(m_flipX ? -m_SlicePosition.x() : m_SlicePosition.x(), m_flipY ? -m_SlicePosition.y() : m_SlicePosition.y()));
	p.setX(p.x() / factorX);
	p.setY(p.y() / factorY);

	QPoint r = QPoint(qFloor(p.x()), qFloor(p.y()));

	if (r.x() < 0) r.setX(0);
	else if (r.x() >= c_x_PET) r.setX(c_x_PET - 1);

	if (r.y() < 0)r.setY(0);
	else if (r.y() >= c_y_PET) r.setY(c_y_PET - 1);

	return r;
}

void RadiomicsView::processWork_MouseMove()
{
	if (m_pDataContext->volume_data.isValidate() == false)
		return;

	if (m_touchZoomEvent == true)
		return;

	RADIOMICS_WORK_MODE mode = WIN_MANAGER->getRadiomicsWorkMode();

	switch (mode)
	{
	case RADIOMICS_WORK_ANNO_RECTANGLE:
		mouseMove_Annotation_InDrawingMode();
		break;
	case RADIOMICS_WORK_NONE:
	default:
		mouseMove_Annotation_NormalMode();
		setOtherMPRPlaneDepth_InMouseEvent(MAINTAB_TA);
		break;
	}
}

void RadiomicsView::processWork_MousePress()
{
	m_StartPos = m_MousePos;
	m_preMovePos = m_MousePos;

	if (m_pDataContext->volume_data.isValidate() == false)
		return;

	RADIOMICS_WORK_MODE mode = WIN_MANAGER->getRadiomicsWorkMode();
	switch (mode)
	{
	case RADIOMICS_WORK_ANNO_RECTANGLE:
		mousePress_Annotation_InDrawingMode(mode);
		break;
	case RADIOMICS_WORK_NONE:
	default:
		mousePress_Annotation_NormalMode();
		setOtherMPRPlaneDepth_InMouseEvent(MAINTAB_TA);
		break;
	}
}

void RadiomicsView::processWork_MouseRelease()
{
	m_RbuttonDown = false;
	m_LbuttonDown = false;
	m_MbuttonDown = false;
	m_bMoveMouse = false;

	renderLater();
}

void RadiomicsView::processWork_MouseDoubleClick(QMouseEvent* e)
{
}

void RadiomicsView::mousePress_Annotation_InDrawingMode(RADIOMICS_WORK_MODE mode)
{
	if (m_LbuttonDown)
	{
		ANNOTATION_TYPE annoType = AT_NONE;
		switch (mode)
		{
		case RADIOMICS_WORK_ANNO_RECTANGLE:
			annoType = AT_RECTANGLE;
			break;
		default:
			break;
		}

		/* Anno Type이 NONE 이면 Skip*/
		if (annoType == AT_NONE)
		{
			return;
		}

		QString text;
		muint16 fontSize = 0;
		QColor qcolor = WIN_MANAGER->annoColor;
		COLOR color = toCOLOR(qcolor);

		/*
			현재 Annotation이 없으면 생성,
			없으면 Drawing Point 추가
		*/
		if (m_pCurAnnotation == nullptr)
		{
			m_pCurAnnotation = AnnotationFactory::createInstance(annoType, this, m_MousePos.x(), m_MousePos.y(), color, text, fontSize);
		}
		else
		{
			m_pCurAnnotation->addDrawingPoint_Window(this, m_MousePos.x(), m_MousePos.y());
		}

		/* 현재 Annotation이 null이면 skip */
		if (m_pCurAnnotation == nullptr)
		{
			return;
		}

		/* Drawing이 완료 처리 */
		if (m_pCurAnnotation->GetState() == Annotation::DRAWING_FINISHED)
		{
			if (m_pCurAnnotation->getType() == AT_RECTANGLE)
			{
				AnnoRectangle* annoRect = (AnnoRectangle*)m_pCurAnnotation.get();
				ACTION_MANAGER->action_Annotation_Rectangle_Add(annoRect);
			}

			/* 상태 초기화 */
			m_pCurAnnotation = nullptr;
			setWorkMode(RADIOMICS_WORK_NONE);

			/* Default 색상 Table 업데이트 */
			WIN_MANAGER->getAnnotationColor();
		}
	}
}

void RadiomicsView::mousePress_Annotation_NormalMode()
{
	if (m_LbuttonDown == false)
	{
		return;
	}

	if (WIN_MANAGER->anotationList.size() == 0)
	{
		return;
	}

	/* Annotation Select 상태 초기화 */
	for (int i = WIN_MANAGER->anotationList.size() - 1; i >= 0; i--)
	{
		Annotation* ano = WIN_MANAGER->anotationList.at(i);
		(*ano).clearSelect();
	}

	for (int i = WIN_MANAGER->anotationList.size() - 1; i >= 0; i--)
	{
		Annotation* ano = WIN_MANAGER->anotationList.at(i);

		/* Annotation Rectangle만 초기화 */
		if (ano->getType() == AT_RECTANGLE)
		{
			bool preSelect = ano->isSelected();
			bool curSelect = ano->setSelect_Window(this, m_MousePos.x(), m_MousePos.y());

			/* 새로 선택되었을 경우 화면 업데이트*/
			if (preSelect != curSelect)
			{
				/* 원본 Annotation 정보 저장 */
				WIN_MANAGER->renderLater_AnalView();
			}

			/* 선택이 되었다면 원본 Annotation 객체 생성 */
			if (curSelect)
			{
				if (ano->getType() == AT_RECTANGLE)
				{
					m_pOriginAnnotation = std::make_unique<AnnoRectangle>(*(AnnoRectangle*)ano);
				}
				break;
			}
		}
	}
}

void RadiomicsView::mouseMove_Annotation_InDrawingMode()
{
	if (m_pCurAnnotation != nullptr)
	{
		m_pCurAnnotation->movePosition_Window(this, m_MousePos.x(), m_MousePos.y());
		renderLater();
	}
}

void RadiomicsView::mouseMove_Annotation_NormalMode()
{
	for (auto ano = WIN_MANAGER->anotationList.begin(); ano != WIN_MANAGER->anotationList.end(); ++ano)
	{
		if ((*ano)->isSelected() && m_LbuttonDown)
		{
			(*ano)->movePosition_Window(this, m_MousePos.x(), m_MousePos.y());
		}
		else
		{
			(*ano)->setHover_Window(this, m_MousePos.x(), m_MousePos.y());
		}
		renderLater();
	}
}

void RadiomicsView::slot_OnAnnoRect()
{
	if (m_btnAnnoRect->isChecked())
		setWorkMode(RADIOMICS_WORK_ANNO_RECTANGLE, true);
	else
		setWorkMode(RADIOMICS_WORK_NONE, true);
}

void RadiomicsView::slot_onHeightMap()
{
	if (WIN_MANAGER->pRadiomics3DVolume == nullptr)
		return;

	unsigned int c2d_width;
	unsigned int c2d_height;
	unsigned int c2d_depth;

	m_pDataContext->volume_data.getLengthForScreen(m_windowType, c2d_width, c2d_height, c2d_depth);

	int count = 1;
	//std::vector<float> vec3DVolume(c2d_width * c2d_height * count, 0);
	float* p2DMap = new float[c2d_width * c2d_height * count];
	recon2DMap(WIN_MANAGER->pRadiomics3DVolume, p2DMap, c2d_width, c2d_height, c2d_depth);

	HeightMapDlg* dlg = new HeightMapDlg(p2DMap, c2d_height, c2d_width, m_heightMapColorTable, (QWidget*)WIN_MANAGER->mainWindow);
	dlg->setWindowTitle("Height Map");
	dlg->resize(WIN_MANAGER->ScreenWidth / 3, WIN_MANAGER->ScreenHeight / 2);
	dlg->setModal(true);
	dlg->setWindowModality(Qt::ApplicationModal);
	dlg->setAttribute(Qt::WA_DeleteOnClose);

	dlg->show();
	SAFE_DELETES(p2DMap);
}