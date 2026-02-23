#pragma  once
#include "stdafx.h"
#include "AnalMPRPlaneView.h"
#include "windowManager.h"
#include "Renderer/Renderer.h"
#include "mipEngine/Camera.h"
#include "mipEngine/intersect.h"
#include "mipEngine/geometry.h"

#include "System/resourceManager.h"
#include "System/stringManager.h"
#include "System/styleManager.h"
#include "Actions/ActionManager.h"

#include "Windows/Main/MainAnalWidget.h"
#include "Windows/Main/MainMeshWidget.h"
#include "Windows/Main/MainTAWidget.h"
#include "Windows/Tabwindow.h"

#include "Dialogs/AnnoControlDlg.h"
#include "Dialogs/AnnoTextDlg.h"
#include "Dialogs/LineProfileDialog.h"
#include "Dialogs/ThickDialog.h"
#include "UI/CustomHistogram.h"

#include "MedipQT.h"

#include "Math/Math.h"
#include "AnnotationFactory.h"

#include "MeshControl.h"

#include "DataContext.h"

#include <QComboBox>

AnalMPRPlaneView::AnalMPRPlaneView(WindowManager* pWinManager, WINDOW_TYPE windowType, DataContext* pDataContext, QWidget* parent)
	: OpenGLWidget(parent)
	, m_btnAnnoLength(0)
	, m_btnAnnoText(0)
	, m_btnAnnoAngle(0)
	, m_btnAnnoArrow(0)
	, m_btnAnnoRect(0)
	, m_btnSegmentPoly(0)
	, m_btnSegmentOval(0)
	, m_btnSegmentDraw(0)
	, m_btnSegmentPicker(0)
	, m_btnSegmentRect(0)
	, m_btnProfileLine(0)
	, m_btnFullScreen(0)
	, m_btnShowAxis(0)
	, m_btnThickness(0)
	, m_pCurAnnotation(nullptr)
	, m_pOriginAnnotation(nullptr)
	, m_pDataContext(pDataContext)
{
	m_windowType = windowType;
	m_pWindowManager = pWinManager;
	m_CoronalSeleted = false;
	m_AxialSeleted = false;
	m_SaggitalSeleted = false;
	m_RbuttonDown = false;
	m_LbuttonDown = false;
	m_MbuttonDown = false;
	m_bMoveMouse = false;
	m_polycheck = false;
	m_translationMPR = false;
	m_rotateMPR = false;
	m_bUpdateMeshOutline = false;
	m_userThick = m_thickness = 0.0f;

	m_nIconSize = m_pWindowManager->mainWindow->IconSize;

	m_contextMenu = new QMenu(this);
	m_actAnnoList = new QAction(STRING_MANAGER->getString(STR_ANNO_LIST), this);
	m_actShowAniPoint = new QAction(STRING_MANAGER->getString(STR_SHOW_ANI_POINT), this);

	m_actShowAnnoText = new QAction(this);
	m_actShowAnnoLength = new QAction(this);
	m_actShowAnnoAngle = new QAction(this);

	m_actShowAnnoArrow = new QAction(this);
	m_actShowAnnoRectangle = new QAction(this);

#ifdef DEV_VER
	m_actPlane = new QAction("Profile plane", this);
#endif

	connect(m_actAnnoList, &QAction::triggered, this, &AnalMPRPlaneView::slot_OnContextAnnoList);
	connect(m_actShowAnnoText, &QAction::triggered, this, &AnalMPRPlaneView::slot_OnContextAnnoText);
	connect(m_actShowAnnoLength, &QAction::triggered, this, &AnalMPRPlaneView::slot_OnContextAnnoLength);
	connect(m_actShowAniPoint, &QAction::triggered, this, &AnalMPRPlaneView::slot_OnShowAniPoint);
	connect(m_actShowAnnoAngle, &QAction::triggered, this, &AnalMPRPlaneView::slot_OnContextAnnoAngle);
	connect(m_actShowAnnoArrow, &QAction::triggered, this, &AnalMPRPlaneView::slot_OnContextAnnoArrow);
	connect(m_actShowAnnoRectangle, &QAction::triggered, this, &AnalMPRPlaneView::slot_OnContextAnnoRectangle);

#ifdef DEV_VER
	connect(m_actPlane, &QAction::triggered, this, &AnalViewBase::OnContextProfPlane);
#endif
	m_HoverWidget = NULL;
	setContextMenu();
	setAcceptDrops(true);
	/* 변수 초기화 */
	m_labelUnloadImage = 0;
	m_slidebar = 0;
	m_comboThickness = 0;
	m_thickness = 0.0f;

	m_Winsize = m_pDataContext->volume_data.getSizeX() * 0.5f;
	if (m_Winsize < m_pDataContext->volume_data.getSizeY() * 0.5f)
	{
		m_Winsize = m_pDataContext->volume_data.getSizeY() * 0.5f;
	}

	if (m_Winsize < m_pDataContext->volume_data.getSizeZ() * 0.5f)
	{
		m_Winsize = m_pDataContext->volume_data.getSizeZ() * 0.5f;
	}

	m_sliceCamera.setZoomMax(m_Winsize * 3);
	m_sliceCamera.setZoom(m_Winsize * 2.0f);
	m_sliceCamera.setNearFar(0.1f, m_Winsize * 8);
	m_sliceCamera.setAt(mip::VECTOR3(0, 0, 0));
	m_initOffset = m_sliceCamera.getOffset();

	if (m_windowType == WT_AXIAL)
	{
		m_sliceCamera.setPos(mip::VECTOR3(0, 0, -m_Winsize * 2));
	}
	else if (m_windowType == WT_CORONAL)
	{
		m_sliceCamera.setPos(mip::VECTOR3(0, -m_Winsize * 2, 0));
		m_sliceCamera.setUp(mip::VECTOR3(0, 0, 1));
	}
	else if (m_windowType == WT_SAGITTAL)
	{
		m_sliceCamera.setPos(mip::VECTOR3(m_Winsize * 2, 0, 0));
	}


	initInScreenRightMenu();
	initInScreenMenu();
}

AnalMPRPlaneView::AnalMPRPlaneView(WINDOW_TYPE windowType, DataContext* pDataContext, QWidget* parent) :
	AnalMPRPlaneView(WIN_MANAGER, windowType, pDataContext, parent)
{
}

AnalMPRPlaneView::~AnalMPRPlaneView()
{

}

void AnalMPRPlaneView::showControls()
{
	static QIcon axisPress = RESOURCE_MANAGER->getIcon(ICON_VIEW_2D_AXISES, m_nIconSize, m_nIconSize);
	static QIcon axisLeave = RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_2D_AXISES, m_nIconSize, m_nIconSize);

	bool isEnable = true;
	if (g_Renderer->isAvailableVolumeRender() == false)
	{
		isEnable = false;
		if (m_pWindowManager->getAnalWorkMode() != ANAL_WORK_NONE)
		{
			m_pWindowManager->setAnalWorkMode(ANAL_WORK_NONE);
		}
	}

	if (MAINTAB_MEASUREMENT == m_pWindowManager->mainTabType)
	{
		setWorkMode(m_pWindowManager->getAnalWorkMode());

		if (m_btnSegmentPoly)
			m_btnSegmentPoly->show();

		if (m_btnSegmentPicker)
			m_btnSegmentPicker->show();

		if (m_btnSegmentDraw)
			m_btnSegmentDraw->show();

		if (m_btnSegmentOval)
			m_btnSegmentOval->show();

		if (m_btnSegmentRect)
			m_btnSegmentRect->show();

		if (m_btnAnnoLength)
			m_btnAnnoLength->show();

		if (m_btnAnnoText)
			m_btnAnnoText->show();

		if (m_btnAnnoAngle)
			m_btnAnnoAngle->show();

		if (m_btnAnnoArrow)
			m_btnAnnoArrow->show();

		if (m_btnAnnoRect)
			m_btnAnnoRect->show();

		if (m_btnProfileLine)
			m_btnProfileLine->show();
	}

	if (m_btnFullScreen)
		m_btnFullScreen->show();

	if (m_btnShowAxis)
	{
		m_btnShowAxis->setChecked(m_pWindowManager->getShowViewAxises());

		if (m_pWindowManager->getShowViewAxises())
			m_btnShowAxis->setIcon(axisPress);
		else
			m_btnShowAxis->setIcon(axisLeave);

		m_btnShowAxis->show();
	}

	if (m_comboThickness)
	{
		if (m_comboThickness->isHidden())
		{
			m_comboThickness->show();

			if (!m_btnThickness->isCheckable())
			{
				m_btnThickness->setCheckable(true);
				m_btnThickness->setFixedHeight(m_comboThickness->height());
			}

			if (m_comboThickness->currentIndex() == W2PTK_CUSTOM)
			{
				m_btnThickness->show();
			}
		}
	}
}

void AnalMPRPlaneView::hideControls()
{
	if (m_btnSegmentPoly) m_btnSegmentPoly->hide();
	if (m_btnSegmentPicker) m_btnSegmentPicker->hide();
	if (m_btnSegmentDraw) m_btnSegmentDraw->hide();
	if (m_btnSegmentOval) m_btnSegmentOval->hide();
	if (m_btnSegmentRect) m_btnSegmentRect->hide();

	if (m_btnAnnoLength) m_btnAnnoLength->hide();
	if (m_btnAnnoText)	m_btnAnnoText->hide();
	if (m_btnAnnoAngle) m_btnAnnoAngle->hide();
	if (m_btnAnnoArrow) m_btnAnnoArrow->hide();
	if (m_btnAnnoRect) m_btnAnnoRect->hide();

	if (m_btnProfileLine) m_btnProfileLine->hide();

	if (m_btnFullScreen) m_btnFullScreen->hide();

	if (m_btnShowAxis) m_btnShowAxis->hide();

	if (m_comboThickness)
	{
		if (!m_comboThickness->isHidden())
		{
			m_comboThickness->hide();
			m_btnThickness->hide();
			renderLater();
		}
	}
}

void AnalMPRPlaneView::setWorkMode(ANAL_WORK_MODE mode, bool iconRefresh)
{
	ANAL_WORK_MODE prevMode = m_pWindowManager->getAnalWorkMode();

	m_polyLine.clear();

	/* prev mode에 따라 Window Manager 상태 업데이트*/
	if (mode != prevMode)
	{
		m_pWindowManager->setAnalWorkMode(mode);

		clearWindowManagerStatus(prevMode);
	}

	bool volumeUpdate = false;

	/* draw cut ON/OFF 설정*/
	m_pWindowManager->DrawCutOnOff(mode == ANAL_WORK_SKETCHDRAWSEGMENTATION);

	/* 버튼 상태정보 업데이트*/
	updateRightMenuButtonStatus_SelectedOrNot(mode, iconRefresh);

	/* Mouse 커서 정보 업데이트*/
	bool captureMode = m_pWindowManager->GetTab()->getCaptureTab()->getCaptureMode();
	updateMouseCursor(mode, captureMode);

	/* Annotation 상태 업데이트*/
	clearSelectedAnnotation(mode, &volumeUpdate);

	/* Work Mode, volumeUpdate 상태 업데이트*/
	updateVolumeUpdateStatus(mode, prevMode, &volumeUpdate);

	/* 화면 상태 업데이트 */
	if (m_pWindowManager->mainTabType == MAINTAB_MEASUREMENT)
	{
		m_pWindowManager->renderLater_GridView(volumeUpdate);
	}
}

bool AnalMPRPlaneView::isWorkMode()
{
	ANAL_WORK_MODE mode = m_pWindowManager->getAnalWorkMode();
	bool result = false;

	switch (mode)
	{
	case ANAL_WORK_ANNO_LENGTH:
	case ANAL_WORK_ANNO_TEXT:
	case ANAL_WORK_ANNO_ANGLE:
	case ANAL_WORK_ANNO_ARROW:
	case ANAL_WORK_ANNO_RECTANGLE:
	case ANAL_WORK_ANNO_PROFILE_LINE:
		if (m_pCurAnnotation != nullptr)
		{
			result = true;
		}
		break;
	case ANAL_WORK_SEGMENT_POLY:
		if (m_polyLine.isEmpty() == false)
		{
			result = true;
		}
		break;
	case ANAL_WORK_SEGMENT_FREEDRAW:
	case ANAL_WORK_SEGMENT_PICKER:
	case ANAL_WORK_SEGMENT_RECT:
	case ANAL_WORK_SEGMENT_OVAL:
	case ANAL_WORK_CAPTURE:
	case ANAL_WORK_SKETCHDRAWSEGMENTATION:
		if (m_LbuttonDown || m_RbuttonDown)
		{
			result = true;
		}
		break;
	case ANAL_WORK_3D_CURVE_SPLIT:
	case ANAL_WORK_3D_PLANE_SPLIT:
	case ANAL_WORK_PATH:
	case ANAL_WORK_PATH_3D_PLAY:
	case ANAL_WORK_NONE:
	default:
		break;
	}
	return result;
}

void AnalMPRPlaneView::initInScreenMenu()
{
	if (m_slidebar == NULL)
	{
		int range = 0;
		int value = 0;
		if (m_windowType == WT_AXIAL)
		{
			range = m_pDataContext->volume_data.getCZ() - 1;
			value = m_pDataContext->volume_data.getCZ() / 2;
		}
		else if (m_windowType == WT_CORONAL)
		{
			range = m_pDataContext->volume_data.getCY() - 1;
			value = m_pDataContext->volume_data.getCY() / 2;
		}
		else if (m_windowType == WT_SAGITTAL)
		{
			range = m_pDataContext->volume_data.getCX() - 1;
			value = m_pDataContext->volume_data.getCX() / 2;
		}

		m_slidebar = new QSlider(Qt::Horizontal, this);
		m_slidebar->setRange(0, range);
		m_slidebar->setValue(value);
		m_slidebar->setStyleSheet(STYLE_MANAGER->slideBarView);
		m_slidebar->setSliderDown(true);
		m_slidebar->installEventFilter(this);

		connect(m_slidebar, &QSlider::valueChanged, this, &AnalMPRPlaneView::slot_OnDepthSlideChanged);

		if (!m_pDataContext->volume_data.isValidate())
		{
			m_slidebar->hide();
		}
	}

	if (m_comboThickness == NULL)
	{
		m_comboThickness = new QComboBox(this);
		m_comboThickness->addItem("00mm");
		m_comboThickness->addItem("05mm");
		m_comboThickness->addItem("10mm");
		m_comboThickness->addItem("15mm");
		m_comboThickness->addItem("20mm");
		m_comboThickness->addItem("User-defined");
		m_comboThickness->installEventFilter(this);
		QFontMetrics ft(this->font());
		QRect _rect = ft.boundingRect(m_comboThickness->itemText(W2PTK_CUSTOM));
		m_comboThickness->setMinimumWidth(_rect.width() + _rect.width() / 3);

		m_comboThickness->setStyleSheet(STYLE_MANAGER->comboBoxNormal);

		m_comboThickness->hide();
		if (!g_Renderer->isAvailableVolumeRender())
			m_comboThickness->setDisabled(true);

		connect(m_comboThickness, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_OnComboThicknessChanged(int)));
	}
}

void AnalMPRPlaneView::initInScreenRightMenu()
{
	if (NULL == m_btnAnnoLength)
	{
		//m_btnAnnoLength = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_ANNO_LENGTH), "", this);
		m_btnAnnoLength = PRODUCT_FACTORY->createWidget<QPushButton>(MFL_Common_Annotation_Length, this);
		if (m_btnAnnoLength)
		{
			m_btnAnnoLength->setIcon(RESOURCE_MANAGER->getIcon(ICON_NON_ANNO_LENGTH, m_nIconSize, m_nIconSize));
			m_btnAnnoLength->setIconSize(QSize(m_nIconSize, m_nIconSize));
			m_btnAnnoLength->setFixedSize(QSize(m_nIconSize, m_nIconSize));
			m_btnAnnoLength->setCheckable(true);
			m_btnAnnoLength->setMouseTracking(true);
			m_btnAnnoLength->installEventFilter(this);
			m_btnAnnoLength->hide();
			m_btnAnnoLength->setStyleSheet("color: black;");
			m_btnAnnoLength->setToolTip("Annotation - Length");
			m_btnAnnoLength->setObjectName("AnalLength");
			connect(m_btnAnnoLength, &QPushButton::clicked, this, &AnalMPRPlaneView::slot_OnAnnoLength);
		}

		//m_btnAnnoText = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_ANNO_TEXT), "", this);
		m_btnAnnoText = PRODUCT_FACTORY->createWidget<QPushButton>(MFL_Common_Annotation_Text, this);
		if (m_btnAnnoText)
		{
			m_btnAnnoText->setIcon(RESOURCE_MANAGER->getIcon(ICON_NON_ANNO_TEXT, m_nIconSize, m_nIconSize));
			m_btnAnnoText->setIconSize(QSize(m_nIconSize, m_nIconSize));
			m_btnAnnoText->setFixedSize(QSize(m_nIconSize, m_nIconSize));
			m_btnAnnoText->setCheckable(true);
			m_btnAnnoText->setMouseTracking(true);
			m_btnAnnoText->installEventFilter(this);
			m_btnAnnoText->hide();
			m_btnAnnoText->setStyleSheet("color: black;");
			m_btnAnnoText->setToolTip("Annotation - Text");
			m_btnAnnoText->setObjectName("AnalText");
			connect(m_btnAnnoText, &QPushButton::clicked, this, &AnalMPRPlaneView::slot_OnAnnoText);
		}

		//m_btnAnnoAngle = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_ANNO_ANGLE), "", this);
		m_btnAnnoAngle = PRODUCT_FACTORY->createWidget<QPushButton>(MFL_Common_Annotation_Angle, this);
		if (m_btnAnnoAngle)
		{
			m_btnAnnoAngle->setIcon(RESOURCE_MANAGER->getIcon(ICON_NON_ANNO_ANGLE, m_nIconSize, m_nIconSize));
			m_btnAnnoAngle->setIconSize(QSize(m_nIconSize, m_nIconSize));
			m_btnAnnoAngle->setFixedSize(QSize(m_nIconSize, m_nIconSize));
			m_btnAnnoAngle->setCheckable(true);
			m_btnAnnoAngle->setMouseTracking(true);
			m_btnAnnoAngle->installEventFilter(this);
			m_btnAnnoAngle->hide();
			m_btnAnnoAngle->setStyleSheet("color: black;");
			m_btnAnnoAngle->setToolTip("Annotation - Angle");
			m_btnAnnoAngle->setObjectName("AnalAngle");
			connect(m_btnAnnoAngle, &QPushButton::clicked, this, &AnalMPRPlaneView::slot_OnAnnoAngle);
		}

		//m_btnInterOval = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_OVALROI), "", this);
		m_btnSegmentOval = PRODUCT_FACTORY->createWidget<QPushButton>(MFL_Common_Annotation_Text, this);
		if (m_btnSegmentOval)
		{
			m_btnSegmentOval->setIcon(RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_OVALROI, m_nIconSize, m_nIconSize));
			m_btnSegmentOval->setIconSize(QSize(m_nIconSize, m_nIconSize));
			m_btnSegmentOval->setFixedSize(QSize(m_nIconSize, m_nIconSize));
			m_btnSegmentOval->setCheckable(true);
			m_btnSegmentOval->setMouseTracking(true);
			m_btnSegmentOval->installEventFilter(this);
			m_btnSegmentOval->hide();
			m_btnSegmentOval->setStyleSheet("color: black;");
			m_btnSegmentOval->setToolTip("Selection for interpolation in oval form");
			m_btnSegmentOval->setObjectName("AnalOval");
			connect(m_btnSegmentOval, &QPushButton::clicked, this, &AnalMPRPlaneView::slot_OnInterOval);
		}

		//m_btnInterPicker = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_PICKERROI), "", this);
		m_btnSegmentPicker = PRODUCT_FACTORY->createWidget<QPushButton>(MFL_Common_Segmentation_Manualdrawingtools_2DViewManualselectionfunction, this);
		if (m_btnSegmentPicker)
		{
			m_btnSegmentPicker->setIcon(RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_PICKERROI, m_nIconSize, m_nIconSize));
			m_btnSegmentPicker->setIconSize(QSize(m_nIconSize, m_nIconSize));
			m_btnSegmentPicker->setFixedSize(QSize(m_nIconSize, m_nIconSize));
			m_btnSegmentPicker->setCheckable(true);
			m_btnSegmentPicker->setMouseTracking(true);
			m_btnSegmentPicker->installEventFilter(this);
			m_btnSegmentPicker->hide();
			m_btnSegmentPicker->setStyleSheet("color: black;");
			m_btnSegmentPicker->setToolTip("Selection for interpolation in line form");
			m_btnSegmentPicker->setObjectName("AnalPicker");
			connect(m_btnSegmentPicker, &QPushButton::clicked, this, &AnalMPRPlaneView::slot_OnInterPicker);
		}

		//m_btnInterRect = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_ANGLEDROI), "", this);
		m_btnSegmentRect = PRODUCT_FACTORY->createWidget<QPushButton>(MFL_Common_Segmentation_Manualdrawingtools_2DViewManualselectionfunction, this);
		if (m_btnSegmentRect)
		{
			m_btnSegmentRect->setIcon(RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_ANGLEDROI, m_nIconSize, m_nIconSize));
			m_btnSegmentRect->setIconSize(QSize(m_nIconSize, m_nIconSize));
			m_btnSegmentRect->setFixedSize(QSize(m_nIconSize, m_nIconSize));
			m_btnSegmentRect->setCheckable(true);
			m_btnSegmentRect->setMouseTracking(true);
			m_btnSegmentRect->installEventFilter(this);
			m_btnSegmentRect->hide();
			m_btnSegmentRect->setStyleSheet("color: black;");
			m_btnSegmentRect->setToolTip("Selection for interpolation in angled form");
			m_btnSegmentRect->setObjectName("AnalRect");
			connect(m_btnSegmentRect, &QPushButton::clicked, this, &AnalMPRPlaneView::slot_OnInterRect);
		}


		//m_btnAnnoArrow = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_ANNO_ARROW), "", this);
		m_btnAnnoArrow = PRODUCT_FACTORY->createWidget<QPushButton>(MFL_Common_Annotation_Arrow, this);
		if (m_btnAnnoArrow)
		{
			m_btnAnnoArrow->setIcon(RESOURCE_MANAGER->getIcon(ICON_NON_ANNO_ARROW, m_nIconSize, m_nIconSize));
			m_btnAnnoArrow->setIconSize(QSize(m_nIconSize, m_nIconSize));
			m_btnAnnoArrow->setFixedSize(QSize(m_nIconSize, m_nIconSize));
			m_btnAnnoArrow->setCheckable(true);
			m_btnAnnoArrow->setMouseTracking(true);
			m_btnAnnoArrow->installEventFilter(this);
			m_btnAnnoArrow->hide();
			m_btnAnnoArrow->setStyleSheet("color: black;");
			m_btnAnnoArrow->setToolTip("Annotation - Arrow");
			m_btnAnnoArrow->setObjectName("AnalArrow");
			connect(m_btnAnnoArrow, &QPushButton::clicked, this, &AnalMPRPlaneView::slot_OnAnnoArrow);
		}

		//m_btnAnnoRect = PRODUCT_FACTORY->createWidget<QPushButton>(MFL_ANNOTATION_RECTANGLE, this);
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
			connect(m_btnAnnoRect, &QPushButton::clicked, this, &AnalMPRPlaneView::slot_OnAnnoRect);
		}

		//m_btnInterPoly = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_POLYROI), "", this);
		m_btnSegmentPoly = PRODUCT_FACTORY->createWidget<QPushButton>(MFL_Common_Segmentation_Manualdrawingtools_2DViewManualselectionfunction, this);
		if (m_btnSegmentPoly)
		{
			m_btnSegmentPoly->setIcon(RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_POLYROI, m_nIconSize, m_nIconSize));
			m_btnSegmentPoly->setIconSize(QSize(m_nIconSize, m_nIconSize));
			m_btnSegmentPoly->setFixedSize(QSize(m_nIconSize, m_nIconSize));
			m_btnSegmentPoly->setMouseTracking(true);
			m_btnSegmentPoly->installEventFilter(this);
			m_btnSegmentPoly->hide();
			m_btnSegmentPoly->setCheckable(true);
			m_btnSegmentPoly->setChecked(false);
			m_btnSegmentPoly->setStyleSheet("color: black;");
			m_btnSegmentPoly->setToolTip("Selection for interpolation in polygon form");
			m_btnSegmentPoly->setObjectName("AnalPolygon");
			connect(m_btnSegmentPoly, &QPushButton::clicked, this, &AnalMPRPlaneView::slot_OnInterPoly);
		}


		//m_btnProfileLine = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_ANNO_PROF_LINE), "", this);
		m_btnProfileLine = PRODUCT_FACTORY->createWidget<QPushButton>(MFL_Common_Measurement_LifeProfile, this);
		if (m_btnProfileLine)
		{
			m_btnProfileLine->setIcon(RESOURCE_MANAGER->getIcon(ICON_NON_ANNO_PROF_LINE, m_nIconSize, m_nIconSize));
			m_btnProfileLine->setIconSize(QSize(m_nIconSize, m_nIconSize));
			m_btnProfileLine->setFixedSize(QSize(m_nIconSize, m_nIconSize));
			m_btnProfileLine->setMouseTracking(true);
			m_btnProfileLine->installEventFilter(this);
			m_btnProfileLine->setCheckable(true);
			m_btnProfileLine->setChecked(false);
			m_btnProfileLine->hide();
			m_btnProfileLine->setStyleSheet("color: black;");
			m_btnProfileLine->setToolTip("Profiling - Line");
			m_btnProfileLine->setObjectName("AnalProfileLine");
			connect(m_btnProfileLine, &QPushButton::clicked, this, &AnalMPRPlaneView::slot_OnProfileLine);
		}

		//m_btnInterDraw = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_FREEDRAWROI), "", this);
		m_btnSegmentDraw = PRODUCT_FACTORY->createWidget<QPushButton>(MFL_Common_Segmentation_Manualdrawingtools_2DViewManualselectionfunction, this);
		if (m_btnSegmentDraw)
		{
			m_btnSegmentDraw->setIcon(RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_FREEDRAWROI, m_nIconSize, m_nIconSize));
			m_btnSegmentDraw->setIconSize(QSize(m_nIconSize, m_nIconSize));
			m_btnSegmentDraw->setFixedSize(QSize(m_nIconSize, m_nIconSize));
			m_btnSegmentDraw->setMouseTracking(true);
			m_btnSegmentDraw->installEventFilter(this);
			m_btnSegmentDraw->setCheckable(true);
			m_btnSegmentDraw->setChecked(false);
			m_btnSegmentDraw->hide();
			m_btnSegmentDraw->setStyleSheet("color: black;");
			m_btnSegmentDraw->setToolTip("Selection for interpolation in freehand form");
			m_btnSegmentDraw->setObjectName("AnalRegion");
			connect(m_btnSegmentDraw, &QPushButton::clicked, this, &AnalMPRPlaneView::slot_OnInterDraw);
		}

		//m_btnShowAxis = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_2D_AXISES), "", this);
		m_btnShowAxis = PRODUCT_FACTORY->createWidget<QPushButton>(MFL_Common_Measurement_AxisesCoordinating, this);
		if (m_btnShowAxis)
		{
			m_btnShowAxis->setIcon(RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_2D_AXISES, m_nIconSize, m_nIconSize));
			m_btnShowAxis->setIconSize(QSize(m_nIconSize, m_nIconSize));
			m_btnShowAxis->setFixedSize(QSize(m_nIconSize, m_nIconSize));
			m_btnShowAxis->setCheckable(true);
			m_btnShowAxis->setMouseTracking(true);
			m_btnShowAxis->installEventFilter(this);
			m_btnShowAxis->hide();
			m_btnShowAxis->setStyleSheet("color: black;");
			m_btnShowAxis->setToolTip("2 Axises Coordinating");
			m_btnShowAxis->setObjectName("AnalAxis");
			connect(m_btnShowAxis, &QPushButton::clicked, this, &AnalMPRPlaneView::slot_OnShowAxises);
		}

		m_btnFullScreen = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_FULL_SCREEN, m_nIconSize, m_nIconSize), "", this);
		m_btnFullScreen->setIconSize(QSize(m_nIconSize, m_nIconSize));
		m_btnFullScreen->setFixedSize(QSize(m_nIconSize, m_nIconSize));
		m_btnFullScreen->setCheckable(true);
		m_btnFullScreen->setMouseTracking(true);
		m_btnFullScreen->installEventFilter(this);
		m_btnFullScreen->hide();
		m_btnFullScreen->setStyleSheet("color: black;");
		m_btnFullScreen->setToolTip("Full screen");
		m_btnFullScreen->setObjectName("AnalFullscreen");
		connect(m_btnFullScreen, &QPushButton::clicked, this, &AnalMPRPlaneView::slot_OnFullScreen);

		//m_btnThickness = new QPushButton(QString(QChar(0x2713)), this);
		m_btnThickness = PRODUCT_FACTORY->createWidget<QPushButton>(MFL_Common_Measurement_ProjectionFunction, this);
		//	m_btnThickness->setFixedSize()
		if (m_btnThickness)
		{
			m_btnThickness->setCheckable(false);
			m_btnThickness->setChecked(false);
			m_btnThickness->setMouseTracking(true);
			m_btnThickness->installEventFilter(this);
			m_btnThickness->hide();

			QFontMetrics ft(this->font());
			QRect _rect = ft.boundingRect("@");
			if (!(PRODUCT_MANAGER->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(PRODUCT_MANAGER->m_strMedipMDBox))
				m_btnThickness->setFixedSize(_rect.width() + 10, _rect.height() + 5);
			else
				m_btnThickness->setFixedSize(_rect.width() + 2, _rect.height());
			m_btnThickness->setStyleSheet("color: black;");
			m_btnThickness->setToolTip("Set user-defined thickness");
			m_btnThickness->setObjectName("AnalFullscreen");
			connect(m_btnThickness, &QPushButton::clicked, this, &AnalMPRPlaneView::slot_OnThickness);
		}

	}
}

void AnalMPRPlaneView::updateScreenRightMenu(int width, int height)
{
	if (m_btnAnnoLength)
	{
		int h = 0;
		m_btnSegmentPoly->move(width - m_btnSegmentPoly->width(), h);
		h += m_btnSegmentPoly->height();
		m_btnSegmentDraw->move(width - m_btnSegmentDraw->width(), h);
		h += m_btnSegmentDraw->height();
		m_btnSegmentPicker->move(width - m_btnSegmentPicker->width(), h);
		h += m_btnSegmentPicker->height();
		m_btnSegmentOval->move(width - m_btnSegmentOval->width(), h);
		h += m_btnSegmentOval->height();
		m_btnSegmentRect->move(width - m_btnSegmentRect->width(), h);
		h += 5 + m_btnSegmentRect->height();

		m_btnAnnoLength->move(width - m_btnAnnoLength->width(), h);
		h += m_btnAnnoLength->height();
		m_btnAnnoText->move(width - m_btnAnnoText->width(), h);
		h += m_btnAnnoText->height();
		m_btnAnnoAngle->move(width - m_btnAnnoAngle->width(), h);
		h += m_btnAnnoAngle->height();
		m_btnAnnoArrow->move(width - m_btnAnnoArrow->width(), h);
		h += m_btnAnnoArrow->height();
		m_btnAnnoRect->move(width - m_btnAnnoRect->width(), h);
		h += 5 + m_btnAnnoRect->height();

		m_btnProfileLine->move(width - m_btnProfileLine->width(), h);
		h += m_btnProfileLine->height();

		m_btnFullScreen->move(0, height - m_btnFullScreen->height());
		m_btnShowAxis->move(width - m_btnShowAxis->width(), height - m_btnShowAxis->height());
	}
}

void AnalMPRPlaneView::renderScene(QPainter* p, bool skip_mode)
{
	//checkDepth();

	std::vector<mip::VECTOR3> tempList;
	std::vector<mip::VECTOR3> coordList;

	m_sliceCamera.setScreenSize(this->width(), this->height());
	m_sliceCamera.updateOrtho();

	m_pDataContext->volume_data.getMPRPPlanes(m_windowType, tempList);
	m_sliceCamera.updateSliceCamera(tempList);

	mip::MATRIX44 matWorld = mip::MATRIX44::Identity;
	mip::MATRIX44 matView = m_sliceCamera.getView();
	mip::MATRIX44 matProj = m_sliceCamera.getProj();

	g_Renderer->setWorld(matWorld);
	g_Renderer->setView(matView);
	g_Renderer->setProj(matProj);

	tempList.clear();
	m_pDataContext->volume_data.getMPRPlaneSurfaceList(m_windowType, tempList, coordList, 0, m_thickness);

	float HuMin = m_pWindowManager->getWindowLevel() - m_pWindowManager->getWindowWidth() / 2;
	if (HuMin < m_pDataContext->volume_data.getHuMin())
		HuMin = m_pDataContext->volume_data.getHuMin();

	float HuMax = m_pWindowManager->getWindowLevel() + m_pWindowManager->getWindowWidth() / 2;
	if (HuMax > m_pDataContext->volume_data.getHuMax())
		HuMax = m_pDataContext->volume_data.getHuMax();

	if (!g_Renderer->beginRender(0, this->size().width(), this->size().height()))
	{
		return;
	}

	g_Renderer->clear(mip::COLOR(0, 0, 0));

	if (g_Renderer->beginScene())
	{
		//todo check work mode
		//if(...)
		bool seed_layer = m_pWindowManager->getAnalWorkMode() == ANAL_WORK_SKETCHDRAWSEGMENTATION;

		muint8 color_alpha = muint8(m_pWindowManager->getLayer2DAlphaValue() * 255.f);

		COLOR _col = m_pWindowManager->getSeedColor(0);
		g_Renderer->setMaskColor(0, mip::COLOR(_col.r, _col.g, _col.b, seed_layer ? color_alpha : 0));
		_col = m_pWindowManager->getSeedColor(1);
		g_Renderer->setMaskColor(1, mip::COLOR(_col.r, _col.g, _col.b, seed_layer ? color_alpha : 0));

		for (int n = m_pDataContext->volume_data.getMaskInfoListCnt() - 1; n >= 0; n--)
		{
			MaskInfo* minfo = m_pDataContext->volume_data.getMaskInfo(n);
			COLOR color = minfo->color;
			g_Renderer->setMaskColor(minfo->uid + 2, mip::COLOR(color.r, color.g, color.b, minfo->show ? color_alpha : 0));
		}

		muint32 slice_count = tempList.size() / 6;
		//float alpha_value = 1.0f / float(slice_count);
		float alpha_value = m_pWindowManager->getLayer2DAlphaValue();
		float gamma = m_pWindowManager->getGamma();

		int index = m_pWindowManager->getSelectedCustomPreset(CL_2D);
		bool Graymode = !m_pWindowManager->getColor2DMode();
		bool Colormode = index <= -1 ? true : false;
		if (m_pWindowManager->volume_renderer.drawVolumeAdvPlaneBegin(
			g_Renderer, m_pWindowManager->getVolumeMixMode() ? 2 : 0,
			m_pWindowManager->getVolumeTexture(),
			m_pWindowManager->getMaskTexture(),
			m_pWindowManager->get2DPresetTexture(),
			HuMin, HuMax,
			Graymode ? (SP_COUNT + m_pWindowManager->getPresetCount()) : Colormode ? m_pWindowManager->getSelectedPreset() : (index + SP_COUNT),
			alpha_value, gamma))
		{
			for (int n = 0; n < slice_count; n++)
			{
				std::vector<mip::VECTOR3> vets;
				std::vector<mip::VECTOR3> texs;
				for (int v = 0; v < 6; v++)
				{
					vets.push_back(tempList[n * 6 + v]);
					texs.push_back(coordList[n * 6 + v]);
				}

				m_pWindowManager->volume_renderer.drawVolumeAdvPlaneRender(g_Renderer, vets, texs, slice_count == 1 ? 0 : n + 1);
			}

			m_pWindowManager->volume_renderer.drawVolumeAdvPlaneEnd(g_Renderer);
		}

		calcMesh2DOutline();

		drawMeshOutline(p);

		g_Renderer->endScene();

		void* rData = g_Renderer->getBackBuffer(0, 0, this->width(), this->height());

		if (rData != NULL)
		{
			QImage image((uchar*)rData, this->width(), this->height(), QImage::Format_RGB32);
			m_img = QImage(this->width(), this->height(), QImage::Format_RGB32);
			QPainter p2(&m_img);
			p2.drawImage(0, 0, image);
			p2.end();

			p->drawImage(0, 0, image);
		}
	}

	g_Renderer->endRender();
}

void AnalMPRPlaneView::renderScene2D(QPainter* p)
{
	std::vector<mip::VECTOR3> tempList;
	std::vector<mip::VECTOR2> coordList;

	m_sliceCamera.setScreenSize(this->width(), this->height());
	m_sliceCamera.updateOrtho();

	m_pDataContext->volume_data.getMPRPPlanes(m_windowType, tempList);
	m_sliceCamera.updateSliceCamera(tempList);

	mip::MATRIX44 matWorld = mip::MATRIX44::Identity;
	mip::MATRIX44 matView = m_sliceCamera.getView();
	mip::MATRIX44 matProj = m_sliceCamera.getProj();

	g_Renderer->setWorld(matWorld);
	g_Renderer->setView(matView);
	g_Renderer->setProj(matProj);

	tempList.clear();
	m_pDataContext->volume_data.getMPRPlaneSurfaceList2D(m_windowType, tempList, coordList, 0, m_thickness);

	float HuMin = m_pWindowManager->getWindowLevel() - m_pWindowManager->getWindowWidth() / 2;
	if (HuMin < m_pDataContext->volume_data.getHuMin())
		HuMin = m_pDataContext->volume_data.getHuMin();

	float HuMax = m_pWindowManager->getWindowLevel() + m_pWindowManager->getWindowWidth() / 2;
	if (HuMax > m_pDataContext->volume_data.getHuMax())
		HuMax = m_pDataContext->volume_data.getHuMax();

	float xLength = 0.0f, yLength = 0.0f;
	float xSpace = 0.0f, ySpace = 0.0f;
	switch (m_windowType)
	{
	case WT_SAGITTAL:
		xLength = m_pDataContext->volume_data.getCX();
		yLength = m_pDataContext->volume_data.getCY();
		xSpace = m_pDataContext->volume_data.getSpaceX();
		ySpace = m_pDataContext->volume_data.getSpaceY();
		break;
	case WT_CORONAL:
		xLength = m_pDataContext->volume_data.getCX();
		yLength = m_pDataContext->volume_data.getCZ();
		xSpace = m_pDataContext->volume_data.getSpaceX();
		ySpace = m_pDataContext->volume_data.getSpaceZ();
		break;
	case WT_AXIAL:
	default:
		xLength = m_pDataContext->volume_data.getCY();
		yLength = m_pDataContext->volume_data.getCZ();
		xSpace = m_pDataContext->volume_data.getSpaceY();
		ySpace = m_pDataContext->volume_data.getSpaceZ();
		break;
	}

	if (!g_Renderer->beginRender(0, this->size().width(), this->size().height()))
	{
		return;
	}

	g_Renderer->clear(mip::COLOR(0, 0, 0));

	if (g_Renderer->beginScene())
	{
		for (int n = m_pDataContext->volume_data.getMaskInfoListCnt() - 1; n >= 0; n--)
		{
			MaskInfo* minfo = m_pDataContext->volume_data.getMaskInfo(n);
			COLOR color = minfo->color;
			g_Renderer->setMaskColor(minfo->uid + 2, mip::COLOR(color.r, color.g, color.b, minfo->show ? minfo->layerAlpha : 0));
		}
		int index = m_pWindowManager->getSelectedCustomPreset(CL_2D);
		bool res = index <= -1 ? true : false;
		if (m_pWindowManager->volume_renderer.drawVolumeAdvPlaneBegin2D(g_Renderer, m_pWindowManager->getVolumeMixMode() ? 2 : 0,
			m_pWindowManager->get2DPresetTexture(), HuMin, HuMax, xLength, yLength, xSpace, ySpace, res ? m_pWindowManager->getSelectedPreset() : (index + SP_COUNT)))
		{
			float gamma = m_pWindowManager->getGamma();
			float alpha = m_pWindowManager->getLayer2DAlphaValue();
			m_pWindowManager->volume_renderer.drawVolumeAdvPlaneRender2D(g_Renderer, m_pWindowManager->get2DMPR(m_windowType), 0, tempList, coordList, 0, alpha, gamma);

			m_pWindowManager->volume_renderer.drawVolumeAdvPlaneEnd(g_Renderer);
		}

		g_Renderer->endScene();

		void* rData = g_Renderer->getBackBuffer(0, 0, this->width(), this->height());
		if (rData != NULL)
		{
			QImage image((uchar*)rData, this->width(), this->height(), QImage::Format_RGB32);
			m_img = QImage(this->width(), this->height(), QImage::Format_RGB32);
			QPainter p2(&m_img);
			p2.drawImage(0, 0, image);
			p2.end();
			//postProcess(image);
			//p->setRenderHint(QPainter::Antialiasing);

			p->drawImage(0, 0, image);
		}
	}

	g_Renderer->endRender();
}

void AnalMPRPlaneView::drawPlaneLine(QPainter* p)
{
	std::vector<QPointF> lineList;
	std::vector<mip::VECTOR3> tempList;
	std::vector<mip::VECTOR2> planePoints;
	mip::MATRIX44 matView = m_sliceCamera.getView();
	mip::MATRIX44 matProj = m_sliceCamera.getProj();

	lineList.clear();
	tempList.clear();

	if (!m_pWindowManager->getShowViewAxises())
	{
		return;
	}

	m_pDataContext->volume_data.getMPRPPlanes(m_windowType, tempList);
	const QColor coroCol = QColor(228, 97, 117);
	const QColor axialCol = QColor(80, 152, 205);
	const QColor sagiCol = QColor(94, 179, 119);

	switch (m_windowType)
	{
	case WT_CORONAL:
		p->setPen(QPen(coroCol, 1));
		break;
	case WT_SAGITTAL:
		p->setPen(QPen(sagiCol, 1));
		break;
	case WT_AXIAL:
	default:
		p->setPen(QPen(axialCol, 1));
		break;
	}

	if (tempList.size() > 1)
	{
		for (auto item = tempList.begin(); tempList.end() != item; ++item)
		{
			mip::VECTOR3 v = mip::geom::WorldToScreen(*item, this->width(), this->height(), matView, matProj);
			lineList.push_back(QPointF(v.x, v.y));
		}

		if (lineList.size() >= 4)
		{
			p->drawLine(lineList[0], lineList[1]);
			p->drawLine(lineList[1], lineList[2]);
			p->drawLine(lineList[2], lineList[3]);
			p->drawLine(lineList[3], lineList[0]);
		}
	}

	lineList.clear();
	tempList.clear();
	float thickness = 0.0f;
	float diff = 0.0f;
	const int alphaCol = 120;
	QPen selPen = QPen(Qt::white, 2);

	QLineF l1, l2;
	QPen pen1, pen2;

	AnalMPRPlaneView* lineView = NULL;
	switch (m_windowType)
	{
	case WT_CORONAL:
	{
		selPen.setColor(axialCol);
		if (MAINTAB_MEASUREMENT == m_pWindowManager->mainTabType)
			lineView = m_pWindowManager->mainAnalWidget->getWindow(WT_AXIAL);

		if (lineView)
		{
			thickness = lineView->getThickness();
			thickness /= 0.5f;/*0~4(mm)*/
			thickness *= 5; /*0~20(mm)*/
		}

		m_pDataContext->volume_data.getMPRPPlanes(WT_AXIAL, tempList);
		if (m_AxialSeleted)
		{
			p->setPen(selPen);
		}
		else
		{
			p->setPen(QPen(axialCol, 1));
		}
		break;
	}
	case WT_SAGITTAL:
	case WT_AXIAL:
	default:
	{
		selPen.setColor(coroCol);
		if (MAINTAB_MEASUREMENT == m_pWindowManager->mainTabType)
			lineView = m_pWindowManager->mainAnalWidget->getWindow(WT_CORONAL);

		if (lineView)
		{
			thickness = lineView->getThickness();
			thickness /= 0.5f; /*0~4*/
			thickness *= 5; /*0~20(mm)*/
		}

		m_pDataContext->volume_data.getMPRPPlanes(WT_CORONAL, tempList);
		if (m_CoronalSeleted)
		{
			p->setPen(selPen);
		}
		else
		{
			p->setPen(QPen(coroCol, 1));
		}
		break;
	}
	}

	if (thickness > 0)
	{
		mip::VECTOR3 v1, v2;
		float fInterval;
		mip::MATRIX44 matView = getCamera().getView();
		mip::MATRIX44 matProj = getCamera().getProj();
		v1 = mip::geom::Screen2World(10, 10, this->width(), this->height(), matView, matProj);
		v2 = mip::geom::Screen2World(10, 11, this->width(), this->height(), matView, matProj);

		fInterval = (v2 - v1).length();

		if (fInterval > 0.0f)
		{
			thickness /= fInterval * 10;
		}
	}


	if (tempList.size() > 1)
	{
		for (auto item = tempList.begin(); tempList.end() != item; ++item)
		{
			mip::VECTOR3 v = mip::geom::WorldToScreen(*item, this->width(), this->height(), matView, matProj);
			lineList.push_back(QPointF(v.x, v.y));
		}

		if (lineList.size() >= 4)
		{
			float a = mip::VECTOR2(lineList[0].x() - lineList[2].x(), lineList[0].x() - lineList[2].y()).length();
			if (a < mip::VECTOR2(lineList[1].x() - lineList[3].x(), lineList[1].y() - lineList[3].y()).length())
			{
				if (lineList[1].y() > lineList[3].y()) // 첫 점이 상단에 있는 경우
				{
					l1.setP1(lineList[1]);
					l1.setP2(lineList[3]);

					diff = ((this->height() - 1) - l1.p2().y());

					if (diff > 0)
						l1.setLength(l1.length() + diff);

					if (l1.p1().y() > 0)
					{
						QPointF p1 = l1.p1();
						l1.setP1(l1.p2());
						l1.setP2(p1);
						diff = l1.p2().y();
						l1.setLength(l1.length() + diff);
					}
				}
				else if (lineList[3].y() > lineList[1].y())
				{
					l1.setP1(lineList[3]);
					l1.setP2(lineList[1]);

					diff = ((this->height() - 1) - l1.p2().y());

					if (diff > 0)
						l1.setLength(l1.length() + diff);

					if (l1.p1().y() > 0)
					{
						QPointF p1 = l1.p1();
						l1.setP1(l1.p2());
						l1.setP2(p1);
						diff = l1.p2().y();
						l1.setLength(l1.length() + diff);
					}
				}
				else //동일한 경우(width 비교)
				{
					if (lineList[1].x() < lineList[3].x())
					{
						l1.setP1(lineList[1]);
						l1.setP2(lineList[3]);
					}
					else
					{
						l1.setP1(lineList[3]);
						l1.setP2(lineList[1]);
					}

					diff = (this->width() - 1) - l1.p2().x();

					if (diff > 0)
						l1.setLength(l1.length() + diff);

					if (l1.p1().x() > 0)
					{
						QPointF p1 = l1.p1();
						l1.setP1(l1.p2());
						l1.setP2(p1);

						diff = l1.p1().x();

						l1.setLength(l1.length() + diff);
					}
				}
				lineList[1].setX(l1.p1().x());
				lineList[1].setY(l1.p1().y());

				lineList[3].setX(l1.p2().x());
				lineList[3].setY(l1.p2().y());

				if (thickness > 0.0f)
				{
					QPen pen, prePen;
					pen = prePen = p->pen();
					QColor col = pen.color();
					col.setAlpha(alphaCol);
					pen.setWidth(pen.width() + thickness);
					pen.setColor(col);
					p->setPen(pen);
					p->drawLine(l1);
					p->setPen(prePen);
				}
				//			p->drawLine(l1);

				planePoints.push_back(mip::VECTOR2(lineList[1].x(), lineList[1].y()));
				planePoints.push_back(mip::VECTOR2(lineList[3].x(), lineList[3].y()));
			}
			else
			{
				if (lineList[0].y() > lineList[2].y()) // 첫 점이 상단에 있는 경우
				{
					l1.setP1(lineList[0]);
					l1.setP2(lineList[2]);

					diff = ((this->height() - 1) - l1.p2().y());

					if (diff > 0)
						l1.setLength(l1.length() + diff);

					if (l1.p1().y() > 0)
					{
						QPointF p1 = l1.p1();
						l1.setP1(l1.p2());
						l1.setP2(p1);
						diff = l1.p2().y();
						l1.setLength(l1.length() + diff);
					}
				}
				else if (lineList[2].y() > lineList[0].y())
				{
					l1.setP1(lineList[2]);
					l1.setP2(lineList[0]);

					diff = ((this->height() - 1) - l1.p2().y());

					if (diff > 0)
						l1.setLength(l1.length() + diff);

					if (l1.p1().y() > 0)
					{
						QPointF p1 = l1.p1();
						l1.setP1(l1.p2());
						l1.setP2(p1);
						diff = l1.p2().y();
						l1.setLength(l1.length() + diff);
					}
				}
				else //동일한 경우(width 비교)
				{
					if (lineList[0].x() < lineList[2].x())
					{
						l1.setP1(lineList[0]);
						l1.setP2(lineList[2]);
					}
					else
					{
						l1.setP1(lineList[2]);
						l1.setP2(lineList[0]);
					}

					diff = (this->width() - 1) - l1.p2().x();

					if (diff > 0)
						l1.setLength(l1.length() + diff);

					if (l1.p1().x() > 0)
					{
						QPointF p1 = l1.p1();
						l1.setP1(l1.p2());
						l1.setP2(p1);

						diff = l1.p1().x();

						l1.setLength(l1.length() + diff);
					}
				}
				lineList[0].setX(l1.p1().x());
				lineList[0].setY(l1.p1().y());

				lineList[2].setX(l1.p2().x());
				lineList[2].setY(l1.p2().y());

				if (thickness > 0.0f)
				{
					QPen pen, prePen;
					pen = prePen = p->pen();
					QColor col = pen.color();
					col.setAlpha(alphaCol);
					pen.setWidth(pen.width() + thickness);
					pen.setColor(col);
					p->setPen(pen);
					p->drawLine(l1);
					p->setPen(prePen);
				}
				//			p->drawLine(l1);

				planePoints.push_back(mip::VECTOR2(lineList[0].x(), lineList[0].y()));
				planePoints.push_back(mip::VECTOR2(lineList[2].x(), lineList[2].y()));
			}
		}
	}
	l2 = l1;
	pen2 = p->pen();

	lineList.clear();
	tempList.clear();

	switch (m_windowType)
	{
	case WT_SAGITTAL:
	{
		selPen.setColor(axialCol);
		if (MAINTAB_MEASUREMENT == m_pWindowManager->mainTabType)
			lineView = m_pWindowManager->mainAnalWidget->getWindow(WT_AXIAL);

		if (lineView)
		{
			thickness = lineView->getThickness();
			thickness /= 0.5f;/*0~4*/
			thickness *= 5; /*0~20(mm)*/
		}

		m_pDataContext->volume_data.getMPRPPlanes(WT_AXIAL, tempList);
		if (m_AxialSeleted)
		{
			p->setPen(selPen);
		}
		else
		{
			p->setPen(QPen(axialCol, 1));
		}
		break;
	}
	case WT_AXIAL:
	case WT_CORONAL:
	default:
	{
		selPen.setColor(sagiCol);
		if (MAINTAB_MEASUREMENT == m_pWindowManager->mainTabType)
			lineView = m_pWindowManager->mainAnalWidget->getWindow(WT_SAGITTAL);

		if (lineView)
		{
			thickness = lineView->getThickness();
			thickness /= 0.5f;/*0~4*/
			thickness *= 5;/*0~20(mm)*/
		}

		m_pDataContext->volume_data.getMPRPPlanes(WT_SAGITTAL, tempList);
		if (m_SaggitalSeleted)
		{
			p->setPen(selPen);
		}
		else
		{
			p->setPen(QPen(sagiCol, 1));
		}
		break;
	}
	}

	if (thickness > 0)
	{
		mip::VECTOR3 v1, v2;
		float fInterval;
		mip::MATRIX44 matView = getCamera().getView();
		mip::MATRIX44 matProj = getCamera().getProj();
		v1 = mip::geom::Screen2World(10, 10, this->width(), this->height(), matView, matProj);
		v2 = mip::geom::Screen2World(10, 11, this->width(), this->height(), matView, matProj);

		fInterval = (v2 - v1).length();

		if (fInterval > 0.0f)
		{
			thickness /= fInterval * 10;
		}
	}


	if (tempList.size() > 1)
	{
		for (auto item = tempList.begin(); tempList.end() != item; ++item)
		{
			mip::VECTOR3 v = mip::geom::WorldToScreen(*item, this->width(), this->height(), matView, matProj);
			lineList.push_back(QPointF(v.x, v.y));
		}

		if (lineList.size() >= 4)
		{
			float a = mip::VECTOR2(lineList[0].x() - lineList[2].x(), lineList[0].x() - lineList[2].y()).length();
			if (a < mip::VECTOR2(lineList[1].x() - lineList[3].x(), lineList[1].y() - lineList[3].y()).length())
			{
				if (lineList[1].y() > lineList[3].y()) // 첫 점이 상단에 있는 경우
				{
					l1.setP1(lineList[1]);
					l1.setP2(lineList[3]);

					diff = (this->height() - 1) - l1.p2().y();

					if (diff > 0)
						l1.setLength(l1.length() + diff);

					if (l1.p1().y() > 0)
					{
						QPointF p1 = l1.p1();
						l1.setP1(l1.p2());
						l1.setP2(p1);
						diff = l1.p2().y();
						l1.setLength(l1.length() + diff);
					}
				}
				else if (lineList[3].y() > lineList[1].y())
				{
					l1.setP1(lineList[3]);
					l1.setP2(lineList[1]);

					diff = (this->height() - 1) - l1.p2().y();

					if (diff > 0)
						l1.setLength(l1.length() + diff);

					if (l1.p1().y() > 0)
					{
						QPointF p1 = l1.p1();
						l1.setP1(l1.p2());
						l1.setP2(p1);
						diff = l1.p2().y();
						l1.setLength(l1.length() + diff);
					}
				}
				else //동일한 경우(width 비교)
				{
					if (lineList[1].x() < lineList[3].x())
					{
						l1.setP1(lineList[1]);
						l1.setP2(lineList[3]);
					}
					else
					{
						l1.setP1(lineList[3]);
						l1.setP2(lineList[1]);
					}

					diff = (this->width() - 1) - l1.p2().x();

					if (diff > 0)
						l1.setLength(l1.length() + diff);

					if (l1.p1().x() > 0)
					{
						QPointF p1 = l1.p1();
						l1.setP1(l1.p2());
						l1.setP2(p1);

						diff = l1.p1().x();
						l1.setLength(l1.length() + diff);
					}
				}
				lineList[1].setX(l1.p1().x());
				lineList[1].setY(l1.p1().y());

				lineList[3].setX(l1.p2().x());
				lineList[3].setY(l1.p2().y());

				if (thickness > 0.0f)
				{
					QPen pen, prePen;
					pen = prePen = p->pen();
					QColor col = pen.color();
					col.setAlpha(alphaCol);
					pen.setWidth(pen.width() + thickness);
					pen.setColor(col);
					p->setPen(pen);
					p->drawLine(l1);
					p->setPen(prePen);
				}
				//		p->drawLine(l1);

				planePoints.push_back(mip::VECTOR2(lineList[1].x(), lineList[1].y()));
				planePoints.push_back(mip::VECTOR2(lineList[3].x(), lineList[3].y()));
			}
			else
			{
				if (lineList[0].y() > lineList[2].y()) // 첫 점이 상단에 있는 경우
				{
					l1.setP1(lineList[0]);
					l1.setP2(lineList[2]);

					diff = (this->height() - 1) - l1.p2().y();

					if (diff > 0)
						l1.setLength(l1.length() + diff);

					if (l1.p1().y() > 0)
					{
						QPointF p1 = l1.p1();
						l1.setP1(l1.p2());
						l1.setP2(p1);
						diff = l1.p2().y();
						l1.setLength(l1.length() + diff);
					}
				}
				else if (lineList[2].y() > lineList[0].y())
				{
					l1.setP1(lineList[2]);
					l1.setP2(lineList[0]);

					diff = (this->height() - 1) - l1.p2().y();

					if (diff > 0)
						l1.setLength(l1.length() + diff);

					if (l1.p1().y() > 0)
					{
						QPointF p1 = l1.p1();
						l1.setP1(l1.p2());
						l1.setP2(p1);
						diff = l1.p2().y();
						l1.setLength(l1.length() + diff);
					}
				}
				else //동일한 경우(width 비교)
				{
					if (lineList[0].x() < lineList[2].x())
					{
						l1.setP1(lineList[0]);
						l1.setP2(lineList[2]);
					}
					else
					{
						l1.setP1(lineList[2]);
						l1.setP2(lineList[0]);
					}

					diff = (this->width() - 1) - l1.p2().x();

					if (diff > 0)
						l1.setLength(l1.length() + diff);

					if (l1.p1().x() > 0)
					{
						QPointF p1 = l1.p1();
						l1.setP1(l1.p2());
						l1.setP2(p1);

						diff = l1.p1().x();
						l1.setLength(l1.length() + diff);
					}
				}
				lineList[0].setX(l1.p1().x());
				lineList[0].setY(l1.p1().y());

				lineList[2].setX(l1.p2().x());
				lineList[2].setY(l1.p2().y());

				if (thickness > 0.0f)
				{
					QPen pen, prePen;
					pen = prePen = p->pen();
					QColor col = pen.color();
					col.setAlpha(alphaCol);
					pen.setWidth(pen.width() + thickness);
					pen.setColor(col);
					p->setPen(pen);
					p->drawLine(l1);
					p->setPen(prePen);
				}

				//				p->drawLine(l1);

				planePoints.push_back(mip::VECTOR2(lineList[0].x(), lineList[0].y()));
				planePoints.push_back(mip::VECTOR2(lineList[2].x(), lineList[2].y()));
			}
		}
	}
	pen1 = p->pen();

	if (planePoints.size() >= 4)
	{
		bool drawCircle = m_translationMPR;


		mip::geom::GetCrossPoint(&m_planeCenter, planePoints[0], planePoints[1], planePoints[2], planePoints[3]);
		int nRan = 9;
		if (drawCircle == false)
		{
			if ((m_planeCenter - mip::VECTOR2(m_MousePos.x(), m_MousePos.y())).length() <= nRan)
				drawCircle = true;
		}
		QPen pen = QPen(Qt::white, 1);
		QBrush preBrush = p->brush();

		p->setBrush(Qt::white);
		if (drawCircle)
		{
			pen.setColor(Qt::red);
			p->setBrush(Qt::red);
			pen1.setWidth(2);
			pen2.setWidth(2);
		}
		p->setPen(pen);

		p->drawEllipse(QPoint(m_planeCenter.x, m_planeCenter.y), 1, 1);
		p->setBrush(preBrush);
		QLineF mid1, mid2;

		mid1.setP1(l1.p1());
		mid1.setP2(QPoint(m_planeCenter.x, m_planeCenter.y));
		mid1.setLength(mid1.length() - nRan);

		mid2.setP1(l1.p2());
		mid2.setP2(QPoint(m_planeCenter.x, m_planeCenter.y));
		mid2.setLength(mid2.length() - nRan);

		p->setPen(pen1);
		p->drawLine(mid1);
		p->drawLine(mid2);

		mid1.setP1(l2.p1());
		mid1.setP2(QPoint(m_planeCenter.x, m_planeCenter.y));
		mid1.setLength(mid1.length() - nRan);

		mid2.setP1(l2.p2());
		mid2.setP2(QPoint(m_planeCenter.x, m_planeCenter.y));
		mid2.setLength(mid2.length() - nRan);

		p->setPen(pen2);
		p->drawLine(mid1);
		p->drawLine(mid2);
	}
}

void AnalMPRPlaneView::drawAnnotations(QPainter* p)
{
	if (NULL == p)
	{
		return;
	}

	drawAnnotationList(p);
}

void AnalMPRPlaneView::drawMouseWork(QPainter* p)
{
	if (NULL == p)
	{
		return;
	}
	bool drawAniPoint = false;

	switch (m_pWindowManager->getAnalWorkMode())
	{
		/* Path Animation*/
	case ANAL_WORK_PATH:
	case ANAL_WORK_PATH_3D_PLAY:
		if ((m_pWindowManager->bStartAni || m_pWindowManager->bCreateAni))
		{
			drawAniPoint = true;
		}
		break;

		/* Segmentation */
	case ANAL_WORK_SEGMENT_POLY:
		drawMousework_Segmentation_PolyLine(p);
		break;
	case ANAL_WORK_SEGMENT_PICKER:
	case ANAL_WORK_SEGMENT_FREEDRAW:
		drawMousework_Segmentation_Pick_or_FreeDraw(p);
		break;
	case ANAL_WORK_SKETCHDRAWSEGMENTATION:
	case ANAL_WORK_SEGMENT_RECT:
	case ANAL_WORK_SEGMENT_OVAL:
		drawMousework_Segmentation_MagicCut_or_RectOval(p);
		break;

		/* Capture*/
	case ANAL_WORK_CAPTURE:
		drawMousework_CaptureArea(p);
		break;

	default:
		break;
	}

	if (!drawAniPoint)
	{
		drawAniPoint = m_pWindowManager->bShowAniPoint;
	}

	if (drawAniPoint)
	{
		drawMousework_Animation_Line(p);
	}
}

void AnalMPRPlaneView::drawSquareLine(QPainter* p, bool drawcut)
{
	float factor = m_sliceCamera.getZoomMax() - m_sliceCamera.getZoom(); /*0 ~ zoommax*/
	factor /= m_sliceCamera.getZoomMax();
	factor += 0.1f;

	if (drawcut)
	{
		COLOR _col = m_pWindowManager->getSeedColor(m_LbuttonDown); //false : backseed, true:foreseed

		p->setPen(QPen(QColor(_col.r, _col.g, _col.b)));
		p->setBrush(QBrush(QColor(_col.r, _col.g, _col.b)));
	}
	else
	{
		if (m_RbuttonDown)
		{
			p->setPen(QPen(Qt::white));
			p->setBrush(QBrush(Qt::white));
		}
		else
		{
			QColor col = m_pWindowManager->getSelectedMaskColor();
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

void AnalMPRPlaneView::drawRuler(QPainter* p)
{
	if (!m_pWindowManager->getShowViewAxises()) return;

	if (p == NULL) return;

	float minX, minY;
	muint32 cx, cy, cz;
	float sx, sy, sz;
	float vInterval, vInterval2;
	float hInterval, hInterval2;
	mip::MATRIX44 matView = getCamera().getView();
	mip::MATRIX44 matProj = getCamera().getProj();
	m_pDataContext->volume_data.getLengthForScreen(m_windowType, cx, cy, cz);
	m_pDataContext->volume_data.getSpacingForScreen(m_windowType, sx, sy, sz, true);

	mip::VECTOR3 v1, v2; //세로 눈금자
	mip::VECTOR3 h1, h2; //가로 눈금자

	QLineF l1, l2;
	QPointF point, point2;
	v1 = mip::geom::Screen2World(10, this->height() / 2, this->width(), this->height(), matView, matProj);
	v2 = mip::geom::Screen2World(10, (this->height() / 2) / 2, this->width(), this->height(), matView, matProj);

	h1 = mip::geom::Screen2World(this->width() / 2, 5, this->width(), this->height(), matView, matProj);
	h2 = mip::geom::Screen2World((this->width() / 2) / 2, 5, this->width(), this->height(), matView, matProj);

	vInterval = (v2 - v1).length();
	vInterval /= 5;

	hInterval = (h2 - h1).length();
	hInterval /= 5;

	p->setPen(Qt::green);

	vInterval2 = (this->height() / 2) / 10;
	hInterval2 = (this->width() / 2) / 10;
	point.setX(10);
	point.setY((float)this->height() / 2);

	point2.setY(this->height() - 30);
	point2.setX((float)this->width() / 2);

	p->drawLine(QPointF(point.x(), point.y()), QPointF(point.x() + 10, point.y()));
	p->drawLine(QPointF(point2.x(), point2.y()), QPointF(point2.x(), point2.y() - 10));

	for (int i = 1; i <= 5; i++)
	{
		int len = 5;

		point.setY((float)this->height() / 2 + vInterval2 * i);
		point2.setX((float)this->width() / 2 + hInterval2 * i);
		if (i >= 5)
		{
			len += 5;
			l1.setP1(point);
			l2.setP1(point2);
			QFontMetrics fm(p->font());
			QString str = QString("%1 mm").arg(round(vInterval * 100));
			QRect rect = fm.boundingRect(str);
			p->drawText(QPointF(point.x(), point.y() + rect.height()), str);
			str = QString("%1 mm").arg(round(hInterval * 100));
			rect = fm.boundingRect(str);
			p->drawText(QPointF(point2.x() - 3, point2.y() - rect.height()), str);
		}

		p->drawLine(QPointF(point.x(), point.y()), QPointF(point.x() + len, point.y()));
		p->drawLine(QPointF(point2.x(), point2.y()), QPointF(point2.x(), point2.y() - len));

		point.setY((float)this->height() / 2 - vInterval2 * i);
		point2.setX((float)this->width() / 2 - hInterval2 * i);
		if (i >= 5)
		{
			l1.setP2(point);
			l2.setP2(point2);
			QFontMetrics fm(p->font());
			QString str = "0 mm";
			QRect rect = fm.boundingRect(str);
			p->drawText(QPointF(point.x(), point.y() - 3), str);
			p->drawText(QPointF(point2.x() - 3, point2.y() - rect.height()), str);

		}
		p->drawLine(QPointF(point.x(), point.y()), QPointF(point.x() + len, point.y()));
		p->drawLine(QPointF(point2.x(), point2.y()), QPointF(point2.x(), point2.y() - len));
	}

	p->drawLine(l1);
	p->drawLine(l2);
}

void AnalMPRPlaneView::drawThickness(QPainter* p, int* pOutOffset)
{
	*pOutOffset = 0;
	QPen preP = p->pen();
	QFontMetrics fontMet(p->font());
	if (m_comboThickness->isHidden())
	{
		QString str = QString("Thickness(%1)").arg(m_comboThickness->currentText());
		QRect rect = fontMet.boundingRect(str);
		*pOutOffset = rect.width() + 10;

		p->setPen(Qt::white);
		p->drawText(QPoint(10, rect.height()), str);
		p->setPen(preP);
	}
	else
	{
		*pOutOffset = m_comboThickness->width();
	}
}

void AnalMPRPlaneView::drawCoordHU(QPainter* p, int offset)
{
	mip::VECTOR2 vScreen = m_sliceCamera.getScreenXY();

	mip::VECTOR3 volumePointIndex;
	mint16 HU = getHU_Point_Screen(vScreen.x, vScreen.y, &volumePointIndex);

	mip::VECTOR3 volumePointScreen;
	int posX_screen = volumePointIndex.x;
	int posY_screen = volumePointIndex.y;
	int posZ_screen = volumePointIndex.z;

	QString posText = m_pWindowManager->ConvertText_VoxelPosToSliceImagePos_WithTotalSliceImageCount(posX_screen, posY_screen, posZ_screen);

	QString str = QString(" / Coord(%1) %3(%2)")
		.arg(posText)
		.arg(HU)
		.arg(m_pWindowManager->getUnitString());

	QFontMetrics fontMet(p->font());
	QRect rect = fontMet.boundingRect(str);
	p->setPen(Qt::white);
	p->drawText((offset + 2), rect.height(), str);
}

void AnalMPRPlaneView::drawWorkMode(QPainter* p)
{
	if (m_pWindowManager->getAnalWorkMode() != ANAL_WORK_NONE)
	{
		QPen preP = p->pen();
		p->setPen(QPen(QColor(62, 137, 219)));
		QString str = QString("%1").arg(m_pWindowManager->getWorkModeString(false, true));
		if (str.length() > 1)
		{
			QFontMetrics fontMet(p->font());
			QRect rect = fontMet.boundingRect(str);
			int offset = m_comboThickness->height();
			p->drawText(m_comboThickness->isHidden() ? 10 : 0, offset + rect.height(), str);
			p->setPen(preP);
		}
	}
}

void AnalMPRPlaneView::drawMeshOutline(QPainter* p)
{
	Visualize2DTab* pVisualize2D = m_pWindowManager->GetTab()->get2DTab();
	if (!pVisualize2D || !pVisualize2D->isOutlineMode() || !ACTION_MANAGER->isActionFinished())
	{
		return;
	}

	float spaceX = m_pDataContext->volume_data.getSpaceX();
	float spaceY = m_pDataContext->volume_data.getSpaceY();
	float spaceZ = m_pDataContext->volume_data.getSpaceZ();
	float halfSpaceX = m_pDataContext->volume_data.getSpaceX() * 0.5f;
	float halfSpaceY = m_pDataContext->volume_data.getSpaceY() * 0.5f;
	float halfSpaceZ = m_pDataContext->volume_data.getSpaceZ() * 0.5f;

	mip::VECTOR3 posCamera = m_sliceCamera.getPos() * 0.1f;

	int n_mesh = m_pDataContext->m_MeshData.GetMeshCount();
	for (int mi = 0; mi < n_mesh; ++mi)
	{
		MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(mi);

		if (pMeshInfo && pMeshInfo->show)
		{
			mip::MeshTopology* pMesh = m_pDataContext->m_MeshData.GetMesh(mi);

			if (pMesh)
			{
				std::vector<std::vector<mip::VECTOR3>>* pVecOutline2D;

				mip::VECTOR3 offset;

				switch (m_windowType)
				{
				case WT_CORONAL:
					pVecOutline2D = &pMeshInfo->vecOutline2DCoronal;
					//offset = m_pWindowManager->analSlideOffset[1];
					offset = m_pWindowManager->analSlideOffsetSub[1];
					break;
				case WT_SAGITTAL:
					pVecOutline2D = &pMeshInfo->vecOutline2DSagittal;
					//offset = m_pWindowManager->analSlideOffset[2];
					offset = m_pWindowManager->analSlideOffsetSub[2];
					break;
				case WT_AXIAL:
				default:
					pVecOutline2D = &pMeshInfo->vecOutline2DAxial;
					//offset = m_pWindowManager->analSlideOffset[0];
					offset = m_pWindowManager->analSlideOffsetSub[0];
					break;
				}

				offset *= 0.1f;

				std::vector<mip::VECTOR3> vecline(2);
				int n_lines = (int)pVecOutline2D->size();
				for (int j = 0; j < n_lines; ++j)
				{
					for (int k = 0; k < (*pVecOutline2D)[j].size(); ++k)
					{
						vecline[k] = (*pVecOutline2D)[j][k] + posCamera;

						vecline[k].x -= halfSpaceX;
						vecline[k].y -= halfSpaceY;
						vecline[k].z -= halfSpaceZ;

						vecline[k] -= offset;
					}

					g_Renderer->renderLineList(vecline, mip::COLOR(pMeshInfo->color.r, pMeshInfo->color.g, pMeshInfo->color.b), 1.f, false);
				}
			}
		}
	}
}

void AnalMPRPlaneView::render(QPainter* p)
{
	/*
		preRenderProcess()가 호출되어야 이미지가 MPR View가 Draw 된다.
		VolumeViewer, AnalVolumeViewer에서 미리 호출되어서
	*/
	m_pWindowManager->preRenderProcess();

	m_pWindowManager->preRenderProcessMPR();

	/* Return 상태 체크 */
	if (isRenderable() == false)
	{
		drawUnloadedImage(p);
		return;
	}

	/* Unload Label Hide 진행 */
	if (m_labelUnloadImage != nullptr)
	{
		m_labelUnloadImage->hide();
	}

	if (g_Renderer->isAvailableVolumeRender() == false
		|| m_pWindowManager->getShaderQuality() == SQ_INVISIBLE)
	{
		renderScene2D(p);
	}
	else
	{
		renderScene(p);
	}

	drawPlaneLine(p);

	//TODO : 전체 Drawing 하기전 MEASUREMENT TAB 체크를 먼저할 수 있는지 확인
	if (m_pWindowManager->mainTabType == MAINTAB_MEASUREMENT)
	{
		drawMouseWork(p);

		drawAnnotations(p);
	}

	drawRuler(p);

	int offset = 0;

	drawThickness(p, &offset);

	drawWaterMark(p);

	drawCoordHU(p, offset);

	drawWorkMode(p);
}

bool AnalMPRPlaneView::eventFilter(QObject* target, QEvent* e)
{
	if (target == NULL)
	{
		return QWidget::eventFilter(target, e);
	}

	if (!target->inherits("QPushButton") && !target->inherits("QSlider")
		&& !target->inherits("QComboBox"))
	{
		return QWidget::eventFilter(target, e);
	}

	QEvent::Type type = e->type();

	/* Mouse Cursor 상태 업데이트 */
	updateMouseCursor_By_EventType(type);

	/* Menu Button Hover 상태 업데이트 */
	updateRightMenuButtonStatus_HoverOrNot(type, e, target);

	return QWidget::eventFilter(target, e);
}

mip::VECTOR3 AnalMPRPlaneView::getVolumeToWorld(mip::VECTOR3 _v1)
{
	float cx, cy, cz;
	VOLUME_DATA& vd = m_pDataContext->volume_data;

	cx = cy = cz = 0;

	cx = vd.getSpaceX() * _v1.x - vd.getSizeX() * 0.5f;
	cy = vd.getSpaceY() * _v1.y - vd.getSizeY() * 0.5f;
	cz = vd.getSpaceZ() * _v1.z - vd.getSizeZ() * 0.5f;

	return mip::VECTOR3(cx, cy, cz);
}

mip::VECTOR3 AnalMPRPlaneView::getVolumeToWorldAuto(mip::VECTOR3 _v1)
{
	float cx, cy, cz;
	VOLUME_DATA& vd = m_pDataContext->volume_data;

	cx = cy = cz = 0;

	switch (m_windowType)
	{
	case WT_AXIAL:
		cx = vd.getSpaceX() * _v1.x - vd.getSizeX() * 0.5f;
		cy = vd.getSpaceY() * _v1.y - vd.getSizeY() * 0.5f;
		cz = vd.getSpaceZ() * _v1.z - vd.getSizeZ() * 0.5f;
		break;
	case WT_CORONAL:
		cx = vd.getSpaceX() * _v1.x - vd.getSizeX() * 0.5f;
		cy = vd.getSpaceY() * _v1.z - vd.getSizeY() * 0.5f;
		cz = vd.getSpaceZ() * _v1.y - vd.getSizeZ() * 0.5f;
		break;
	case WT_SAGITTAL:
		cx = vd.getSpaceX() * _v1.z - vd.getSizeX() * 0.5f;
		cy = vd.getSpaceY() * _v1.x - vd.getSizeY() * 0.5f;
		cz = vd.getSpaceZ() * _v1.y - vd.getSizeZ() * 0.5f;
		break;
	}
	return mip::VECTOR3(cx, cy, cz);
}

mip::VECTOR3 AnalMPRPlaneView::getWorldToVolume(float x, float y, float z)
{
	mip::VECTOR3 basisVec;
	mip::VECTOR3 LocalVec;
	mip::VECTOR3 volumeVec;
	VOLUME_DATA& vd = m_pDataContext->volume_data;

	basisVec.set(-vd.getSizeX() * 0.5f, -vd.getSizeY() * 0.5f, -vd.getSizeZ() * 0.5f);

	LocalVec.set(x, y, z);

	volumeVec.set((LocalVec - basisVec));

	volumeVec.x /= m_pDataContext->volume_data.getSpaceX();
	volumeVec.y /= m_pDataContext->volume_data.getSpaceY();
	volumeVec.z /= m_pDataContext->volume_data.getSpaceZ();

	if (volumeVec.x < 0)
		volumeVec.x = 0;
	else if (volumeVec.x >= vd.getCX())
		volumeVec.x = vd.getCX() - 1;

	if (volumeVec.y < 0)
		volumeVec.y = 0;
	else if (volumeVec.y >= vd.getCY())
		volumeVec.y = vd.getCY() - 1;

	if (volumeVec.z < 0)
		volumeVec.z = 0;
	else if (volumeVec.z >= vd.getCZ())
		volumeVec.z = vd.getCZ() - 1;

	return volumeVec;
}

/**
	Camera와 Volume의 Plane와 마주하는 점의 위치 반환
*/
bool AnalMPRPlaneView::getCurrentCameraToPlaneIntersectedPoint(mip::VECTOR3* pOutPoint)
{
	mip::VECTOR2 v = m_sliceCamera.getScreenXY();
	return getCameraToPlaneIntersectedPoint(pOutPoint, v.x, v.y);
}

bool AnalMPRPlaneView::getCameraToPlaneIntersectedPoint(mip::VECTOR3* pOutPoint, int screenX, int screenY)
{
	std::vector<mip::VECTOR3> vertexesMPRPlane;
	m_pDataContext->volume_data.getMPRPPlanes(m_windowType, vertexesMPRPlane);

	mip::Slice tempSliceCamera = m_sliceCamera;
	mip::PLANE plane(vertexesMPRPlane[0], vertexesMPRPlane[1], vertexesMPRPlane[2]);
	return getCameraToPlaneIntersectedPoint(tempSliceCamera, plane, pOutPoint, screenX, screenY);
}

bool AnalMPRPlaneView::getCameraToPlaneIntersectedPoint(mip::Slice& sliceCamera, mip::PLANE& plane, mip::VECTOR3* pOutPoint, int screenX, int screenY)
{
	mip::VECTOR2 prevXY = sliceCamera.getScreenXY();

	sliceCamera.setScreenXY(screenX, screenY);

	mip::VECTOR3 cameraCurrentPoint = sliceCamera.getWorldPoint(0);
	mip::VECTOR3 cameraDirectionPoint = sliceCamera.getWorldPoint(1);

	int RayResult = mip::geom::Intersect_LinePlane(plane, cameraCurrentPoint, cameraDirectionPoint, pOutPoint);

	return RayResult > 0;
}


void AnalMPRPlaneView::setContextMenu()
{
	m_contextMenu->clear();
	if (m_pWindowManager->mainTabType == MAINTAB_MEASUREMENT)
	{
		m_actShowAnnoText->setText(QString("%1(%2)").arg(m_pWindowManager->bShowAnnoText ?
			STRING_MANAGER->getString(STR_HIDE_ANNO) : STRING_MANAGER->getString(STR_SHOW_ANNO))
			.arg(STRING_MANAGER->getString(STR_ANNO_TEXT)));

		m_actShowAnnoLength->setText(QString("%1(%2)").arg(m_pWindowManager->bShowAnnoLength ?
			STRING_MANAGER->getString(STR_HIDE_ANNO) : STRING_MANAGER->getString(STR_SHOW_ANNO))
			.arg(STRING_MANAGER->getString(STR_ANNO_LENGTH)));

		m_actShowAnnoAngle->setText(QString("%1(%2)").arg(m_pWindowManager->bShowAnnoAngle ?
			STRING_MANAGER->getString(STR_HIDE_ANNO) : STRING_MANAGER->getString(STR_SHOW_ANNO))
			.arg(STRING_MANAGER->getString(STR_ANNO_ANGLE)));

		m_actShowAnnoArrow->setText(QString("%1(%2)").arg(m_pWindowManager->bShowAnnoArrow ?
			STRING_MANAGER->getString(STR_HIDE_ANNO) : STRING_MANAGER->getString(STR_SHOW_ANNO))
			.arg(STRING_MANAGER->getString(STR_ANNO_ARROW)));

		m_actShowAnnoRectangle->setText(QString("%1(%2)").arg(m_pWindowManager->bShowAnnoRectangle ?
			STRING_MANAGER->getString(STR_HIDE_ANNO) : STRING_MANAGER->getString(STR_SHOW_ANNO))
			.arg(STRING_MANAGER->getString(STR_ANNO_RECTANGLE)));

		m_contextMenu->setContextMenuPolicy(Qt::DefaultContextMenu);

		m_contextMenu->addAction(m_actAnnoList);
		m_contextMenu->addAction(m_actShowAnnoText);
		m_contextMenu->addAction(m_actShowAnnoLength);
		m_contextMenu->addAction(m_actShowAnnoAngle);
		m_contextMenu->addAction(m_actShowAnnoArrow);
		m_contextMenu->addAction(m_actShowAnnoRectangle);

		if (m_pWindowManager->bShowAniPoint)
			m_actShowAniPoint->setText(STRING_MANAGER->getString(STR_HIDE_ANI_POINT));
		else
			m_actShowAniPoint->setText(STRING_MANAGER->getString(STR_SHOW_ANI_POINT));

		m_actShowAnnoText->setText(QString("%1(%2)").arg(m_pWindowManager->bShowAnnoText ?
			STRING_MANAGER->getString(STR_HIDE_ANNO) : STRING_MANAGER->getString(STR_SHOW_ANNO))
			.arg(STRING_MANAGER->getString(STR_ANNO_TEXT)));

		m_contextMenu->addAction(m_actShowAniPoint);

#ifdef DEV_VER
		m_contextMenu->addAction(m_actPlane);
#endif
	}
}

void AnalMPRPlaneView::depthChanged(int val)
{
	if (m_windowType == WT_AXIAL)
		val = (m_pDataContext->volume_data.getCZ() - 1) - val;

	slot_OnDepthSlideChanged(val);
	m_slidebar->blockSignals(true);
	m_slidebar->setValue(val);
	m_slidebar->blockSignals(false);
}

void AnalMPRPlaneView::setThickness(float _thick)
{
	if (m_pWindowManager->IsLicensePass())
	{
		int index = _thick / 0.5f;
		if (m_comboThickness)
		{
			m_thickness = _thick;

			if (!((index * 0.5f == _thick) && index < W2PTK_CUSTOM))
			{
				index = W2PTK_CUSTOM;
				m_userThick = m_thickness;
			}

			m_comboThickness->setCurrentIndex(index);
		}
	}
}

void AnalMPRPlaneView::setWheelSliderFunc(bool is_SliderSlice_WheelZoom)
{
	m_slidebar->blockSignals(true);

	if (is_SliderSlice_WheelZoom)
	{
		int range = 0;
		int value = 0;
		mip::VECTOR3* point = nullptr;

		if (m_windowType == WT_AXIAL)
		{
			m_pWindowManager->analSlideOffsetSub[0] = m_pWindowManager->analSlideOffset[0].Zero;
			m_pWindowManager->analSlideOffset[0] = m_pWindowManager->analSlideOffset[0].Zero;

			point = &m_pDataContext->volume_data.axialPPlane[0];
			range = m_pDataContext->volume_data.getCZ() - 1;
			value = m_pDataContext->volume_data.getCZ() / 2;
		}
		else if (m_windowType == WT_CORONAL)
		{
			m_pWindowManager->analSlideOffsetSub[1] = m_pWindowManager->analSlideOffset[1].Zero;
			m_pWindowManager->analSlideOffset[1] = m_pWindowManager->analSlideOffset[1].Zero;

			point = &m_pDataContext->volume_data.coronalPPlane[0];
			range = m_pDataContext->volume_data.getCY() - 1;
			value = m_pDataContext->volume_data.getCY() / 2;
		}
		else if (m_windowType == WT_SAGITTAL)
		{
			m_pWindowManager->analSlideOffsetSub[2] = m_pWindowManager->analSlideOffset[2].Zero;
			m_pWindowManager->analSlideOffset[2] = m_pWindowManager->analSlideOffset[2].Zero;

			point = &m_pDataContext->volume_data.saggitalPPlane[0];
			range = m_pDataContext->volume_data.getCX() - 1;
			value = m_pDataContext->volume_data.getCX() / 2;
		}

		if (point != nullptr)
		{
			m_pDataContext->volume_data.getAnal3DPlanes(m_windowType, point, true);
		}

		m_slidebar->setRange(0, range);
		m_slidebar->setValue(value);
	}
	else
	{
		m_slidebar->setRange(0, 99);
		float val = m_sliceCamera.getZoom();
		val /= m_Winsize * 3.0f;
		val *= 100;
		val -= 100;
		val = -val;
		m_slidebar->setValue(val);
	}

	m_slidebar->blockSignals(false);
}

void AnalMPRPlaneView::setLowSpecOptions()
{
	if (!g_Renderer->isAvailableVolumeRender())
	{
		return;
	}

	if (m_pWindowManager->getShaderQuality() == SQ_INVISIBLE)
	{
		m_comboThickness->setCurrentIndex(0);
		m_comboThickness->setDisabled(true);
	}
	else
	{
		m_comboThickness->setDisabled(false);
	}
}

void AnalMPRPlaneView::setSliderMid()
{
	if (m_slidebar)
	{
		if (m_windowType == WT_AXIAL)
		{
			m_pWindowManager->analSlideOffset[0] = m_pWindowManager->analSlideOffset[0].Zero;
		}
		else if (m_windowType == WT_CORONAL)
		{
			m_pWindowManager->analSlideOffset[1] = m_pWindowManager->analSlideOffset[1].Zero;
		}
		else if (m_windowType == WT_SAGITTAL)
		{
			m_pWindowManager->analSlideOffset[2] = m_pWindowManager->analSlideOffset[2].Zero;
		}

		m_slidebar->blockSignals(true);
		m_slidebar->setRange(0, SLIDER_RANGE);//50 이상 양수, 미만 음수 (plane의 위치)
		m_slidebar->setValue(SLIDER_MID);
		m_slidebar->blockSignals(false);

		m_pDataContext->volume_data.setAnal3DPlanes(m_windowType);
	}
}

mint16 AnalMPRPlaneView::getHU_Point_Screen(int screenX, int screenY, mip::VECTOR3* pVolumePoint)
{
	mip::VECTOR3 worldPoint;
	if (getCameraToPlaneIntersectedPoint(&worldPoint, screenX, screenY))
	{
		mip::VECTOR3 v_volume = getWorldToVolume(worldPoint.x, worldPoint.y, worldPoint.z);

		/* x,y,z 정수로 반올림 진행*/
		//v_volume.x = round(v_volume.x);
		//v_volume.y = round(v_volume.y);
		//v_volume.z = round(v_volume.z);

		if (pVolumePoint)
		{
			*pVolumePoint = v_volume;
		}

		return m_pDataContext->volume_data.getData(
			v_volume.x,
			v_volume.y,
			v_volume.z
		);
	}
	else
	{
		return 0;
	}
}

void AnalMPRPlaneView::enableControls(bool bEnable)
{
	if (m_btnSegmentPoly)
		m_btnSegmentPoly->setEnabled(bEnable);

	if (m_btnSegmentOval)
		m_btnSegmentOval->setEnabled(bEnable);

	if (m_btnSegmentDraw)
		m_btnSegmentDraw->setEnabled(bEnable);

	if (m_btnSegmentPicker)
		m_btnSegmentPicker->setEnabled(bEnable);

	if (m_btnSegmentRect)
		m_btnSegmentRect->setEnabled(bEnable);

	enableAnnoControls(bEnable);
}

void AnalMPRPlaneView::enableAnnoControls(bool bEnable)
{
	if (m_btnAnnoLength)
		m_btnAnnoLength->setEnabled(bEnable);

	if (m_btnAnnoText)
		m_btnAnnoText->setEnabled(bEnable);

	if (m_btnAnnoAngle)
		m_btnAnnoAngle->setEnabled(bEnable);

	if (m_btnAnnoArrow)
		m_btnAnnoArrow->setEnabled(bEnable);

	if (m_btnAnnoRect)
		m_btnAnnoRect->setEnabled(bEnable);
}

void AnalMPRPlaneView::resetUI()
{
	reInit();
	enableControls(m_pWindowManager->IsEnableViewControls());
}

void AnalMPRPlaneView::reInit()
{
	/* Camera 위치 초기화 */
	m_Winsize = m_pDataContext->volume_data.getSizeX() * 0.5f;
	if (m_Winsize < m_pDataContext->volume_data.getSizeY() * 0.5f)
	{
		m_Winsize = m_pDataContext->volume_data.getSizeY() * 0.5f;
	}

	if (m_Winsize < m_pDataContext->volume_data.getSizeZ() * 0.5f)
	{
		m_Winsize = m_pDataContext->volume_data.getSizeZ() * 0.5f;
	}

	m_sliceCamera.setZoomMax(m_Winsize * 3.0f);
	m_sliceCamera.setZoom(m_Winsize * 2.0f);
	m_sliceCamera.setNearFar(0.1f, m_Winsize * 8);
	m_sliceCamera.setAt(mip::VECTOR3(0, 0, 0));
	m_sliceCamera.setOffset(m_initOffset);

	if (m_windowType == WT_AXIAL)
	{
		m_sliceCamera.setPos(mip::VECTOR3(0, 0, -m_Winsize * 2));
	}
	else if (m_windowType == WT_CORONAL)
	{
		m_sliceCamera.setPos(mip::VECTOR3(0, -m_Winsize * 2, 0));
		m_sliceCamera.setUp(mip::VECTOR3(0, 0, 1));
	}
	else if (m_windowType == WT_SAGITTAL)
	{
		m_sliceCamera.setPos(mip::VECTOR3(m_Winsize * 2, 0, 0));
	}

	/* Slider 초기화 */
	if (m_slidebar)
	{
		m_slidebar->blockSignals(true);
		int range = 0;
		int value = 0;
		mip::VECTOR3* point = nullptr;

		if (m_windowType == WT_AXIAL)
		{
			m_pWindowManager->analSlideOffsetSub[0] = m_pWindowManager->analSlideOffset[0].Zero;
			m_pWindowManager->analSlideOffset[0] = m_pWindowManager->analSlideOffset[0].Zero;

			point = &m_pDataContext->volume_data.axialPPlane[0];
			range = m_pDataContext->volume_data.getCZ() - 1;
			value = m_pDataContext->volume_data.getCZ() / 2;
		}
		else if (m_windowType == WT_CORONAL)
		{
			m_pWindowManager->analSlideOffsetSub[1] = m_pWindowManager->analSlideOffset[1].Zero;
			m_pWindowManager->analSlideOffset[1] = m_pWindowManager->analSlideOffset[1].Zero;

			point = &m_pDataContext->volume_data.coronalPPlane[0];
			range = m_pDataContext->volume_data.getCY() - 1;
			value = m_pDataContext->volume_data.getCY() / 2;
		}
		else if (m_windowType == WT_SAGITTAL)
		{
			m_pWindowManager->analSlideOffsetSub[2] = m_pWindowManager->analSlideOffset[2].Zero;
			m_pWindowManager->analSlideOffset[2] = m_pWindowManager->analSlideOffset[2].Zero;

			point = &m_pDataContext->volume_data.saggitalPPlane[0];
			range = m_pDataContext->volume_data.getCX() - 1;
			value = m_pDataContext->volume_data.getCX() / 2;
		}

		if (point != nullptr)
		{
			m_pDataContext->volume_data.getAnal3DPlanes(m_windowType, point, true);
		}

		m_slidebar->setRange(0, range);
		m_slidebar->setValue(value);
		m_slidebar->show();
		m_slidebar->blockSignals(false);

		renderLater();
	}


	/* ComboBox Thickness 초기화 */
	if (m_comboThickness)
	{
		if (m_comboThickness->isHidden())
		{
			m_comboThickness->show();

			if (!m_btnThickness->isCheckable())
			{
				m_btnThickness->setCheckable(true);
				m_btnThickness->setFixedHeight(m_comboThickness->height());
			}

			if (m_comboThickness->currentIndex() == W2PTK_CUSTOM)
				m_btnThickness->show();
		}
	}

	m_bUpdateMeshOutline = true;
}

void AnalMPRPlaneView::keyPressEvent(QKeyEvent* e)
{
	bool chkRender = false;
	checkModifiers(e, true, &chkRender);

	if (chkRender)
		renderLater();
}

void AnalMPRPlaneView::keyReleaseEvent(QKeyEvent* e)
{
	bool chkRender = false;
	checkModifiers(e, false, &chkRender);

	if (e->key() == Qt::Key_Space && isWorkMode() == false)
	{
		if (!m_pDataContext->volume_data.isValidate())
		{
			return;
		}

		m_fullscreen = !m_fullscreen;
		chkRender = false;
		emit setFullScreen(m_fullscreen == true ? this : NULL);
	}

	if (e->key() == Qt::Key_Escape)
	{
		/* Drawing 상태 Reset */
		setWorkMode(ANAL_WORK_NONE);
		chkRender = false;
		m_pCurAnnotation = nullptr;
		m_pOriginAnnotation = nullptr;
		renderLater();
	}

	if (chkRender)
	{
		renderLater();
	}

}

void AnalMPRPlaneView::mouseMoveEvent(QMouseEvent* e)
{
	if (m_pWindowManager->getMoveFocus())
	{
		this->setFocus();
	}
	checkModifiers(e);

	m_pWindowManager->setLatestActiveViewType(m_windowType);

	m_MousePos = e->pos();
	m_sliceCamera.setScreenXY(m_MousePos.x(), m_MousePos.y());

	if (m_RbuttonDown)
	{
		renderLater();
	}
	else if (m_LbuttonDown)
	{
		processRotate();
	}
	else if (m_MbuttonDown)
	{
		m_sliceCamera.addOffsetDelta();

		renderLater();
	}

	processMouseMove();

	m_preMousePos = e->pos();
	m_sliceCamera.setPreScreenXY(m_preMousePos.x(), m_preMousePos.y());
}

void AnalMPRPlaneView::mousePressEvent(QMouseEvent* e)
{
	processMousePress(e);
}

void AnalMPRPlaneView::mouseReleaseEvent(QMouseEvent* e)
{
	processMouseRelease();
}

void AnalMPRPlaneView::mouseDoubleClickEvent(QMouseEvent* e)
{
	processMouseDoubleClick(e);
}

void AnalMPRPlaneView::resizeEvent(QResizeEvent* e)
{
	if (e == NULL)
	{
		return;
	}

	int width = e->size().width();
	int height = e->size().height();

	updateScreenRightMenu(width, height);

	processResize(e->size().width(), e->size().height());

	if (m_slidebar)
	{
		m_slidebar->resize(2 * (width / 3), 20);
		m_slidebar->move(width / 2 - m_slidebar->size().width() / 2, height - 30);
	}

	if (m_comboThickness)
	{
		m_comboThickness->setMinimumContentsLength(5);
		m_comboThickness->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);
		m_comboThickness->adjustSize();
		m_comboThickness->move(0, 0);
		m_btnThickness->move(m_comboThickness->width() + 1, 0);
	}

	renderLater();
}

void AnalMPRPlaneView::wheelEvent(QWheelEvent* event)
{
	if (m_touchEvent == true)
	{
		return;
	}

	if (isWorkMode() == true)
	{
		return;
	}

	float delta = event->delta();

	if (m_pWindowManager->getWheelZoom())
	{
		//TODO : 중복되는 부분 리팩토링
		if (m_windowType == WT_AXIAL)
		{
			float val = m_sliceCamera.getZoom();

			val /= (m_Winsize * 3.0f);
			if (delta > 0)
				val -= 0.02f;
			else
				val += 0.02f;

			val *= m_Winsize * 3.0f;

			if (0 < val && val <= (m_Winsize * 3.0f))
				m_sliceCamera.setZoom(val);

		}
		else if (m_windowType == WT_CORONAL || m_windowType == WT_SAGITTAL)
		{
			float val = delta;
			float preVal = m_sliceCamera.getZoom();

			preVal /= (m_Winsize * 3.0f);
			if (delta > 0)
			{
				val = preVal - 0.02f;
				val *= m_Winsize * 3.0f;

				if (val > 0)
				{
					m_sliceCamera.setZoom(val);
				}
			}
			else
			{
				val = preVal + 0.02f;
				val *= m_Winsize * 3.0f;

				if (val <= (m_Winsize * 3.0f))
				{
					m_sliceCamera.setZoom(val);
				}
			}
		}

		if ((m_cursor.shape() == Qt::BitmapCursor))
		{
			m_cursor = getBitmapCursor();
			setCursor(m_cursor);
		}
	}
	else
	{
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
			res = m_pDataContext->volume_data.getAllThickness(m_thickness, delta, m_windowType);
		}

		if (!res)
		{
			processSlideZ(delta);
			updatePlaneDatas();
		}

	}

	renderLater();
}

void AnalMPRPlaneView::touchMove(mint32 preX, mint32 preY, mint32 currX, mint32 currY)
{
	mip::VECTOR3 v1 = mip::geom::Screen2Camera(preX, preY, this->width(), this->height(), m_sliceCamera.getProj());
	mip::VECTOR3 v2 = mip::geom::Screen2Camera(currX, currY, this->width(), this->height(), m_sliceCamera.getProj());

	m_sliceCamera.addOffset(v2 - v1);
	update();
}

void AnalMPRPlaneView::touchZoom(float dt)
{
	if (m_slidebar)
	{
		if (m_pWindowManager->getWheelZoom())
		{
			m_sliceCamera.wheelZoom(dt * 0.1f);
			renderLater();
		}
		else
		{
			int v = m_slidebar->value(); //0~100
			m_slidebar->setValue(v + (dt > 0.0f ? 1 : -1));
		}
	}
}

void AnalMPRPlaneView::dragEnterEvent(QDragEnterEvent* ev)
{
	ev->accept();
}

void AnalMPRPlaneView::dropEvent(QDropEvent* event)
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

	//	m_pWindowManager->fileOpen(filename);
	m_pWindowManager->mainWindow->fileOpen(filename);

	event->acceptProposedAction();
}

void AnalMPRPlaneView::mouseDblClick_Annotation()
{
	if (m_pOriginAnnotation == nullptr)
	{
		return;
	}

	if (m_LbuttonDown == false)
	{
		return;
	}
	muint32 selectedAnnoIndex = 0;
	Annotation* editAnno = nullptr;

	if (m_pWindowManager->anotationList.size() <= 0)
	{
		return;
	}

	/* Annotation Select 진행 */
	for (int i = m_pWindowManager->anotationList.size() - 1; i >= 0; i--)
	{
		Annotation* ano = m_pWindowManager->anotationList.at(i);

		bool preSelect = ano->isSelected();
		bool curSelect = ano->setSelect_Measurement(this, m_MousePos.x(), m_MousePos.y());

		if (preSelect != ano->isSelected())
		{
			m_pWindowManager->renderLater_AnalView();
		}

		if (curSelect)
		{
			editAnno = ano;
			selectedAnnoIndex = i;
			break;
		}
	}

	/* Annotation이 선택되었을 경우 Handling */
	if (editAnno != nullptr)
	{
		if (editAnno->getType() == AT_TEXT)
		{
			AnnoString* annoStr = (AnnoString*)editAnno;
			AnnoTextDlg dlg(this, annoStr->getText(), annoStr->getFontSize(), annoStr->getColor());
			if ((dlg.exec() == QDialog::Accepted) && (dlg.Text.isEmpty() == false))
			{
				annoStr->setText(dlg.Text);
				annoStr->setFontSize(dlg.FontSize);
				annoStr->setColor(toCOLOR(dlg.Color));
				commitToUpdateNewAnnotationStatus(editAnno, selectedAnnoIndex);
			}
		}
		else
		{
			QColorDialog dlg(this);
			if (dlg.exec() == QDialog::Accepted)
			{
				QColor color = dlg.selectedColor();
				editAnno->setColor(toCOLOR(color));
				commitToUpdateNewAnnotationStatus(editAnno, selectedAnnoIndex);
			}
		}

		editAnno->clearSelect();
	}

	m_pOriginAnnotation = nullptr;
}

void AnalMPRPlaneView::mousePress_CaptureArea()
{
	if (m_LbuttonDown)
	{
		bool mode = m_pWindowManager->GetTab()->getCaptureTab()->getCaptureMode();
		bool type = m_pWindowManager->GetTab()->getCaptureTab()->getCaptureType();

		m_polyLine.clear();

		if (mode)	//specific window
		{
			QImage img = getCaptureImage(mode, type);

			if (img != QImage())
				ACTION_MANAGER->action_Capture_image_Add(img);

			setWorkMode(ANAL_WORK_NONE);
			renderLater();
		}
		else //rectangle
		{
			m_polyLine.append(m_MousePos);
		}
	}
}

void AnalMPRPlaneView::mousePress_SegmentPolyLine()
{
	m_polycheck = false;

	if (m_LbuttonDown || m_RbuttonDown)
	{
		if (isMousePointInBoundaryBox())
		{
			m_polyLine.append(m_MousePos);

			if (m_polyLine.size() >= 4)
			{
				QPoint q = m_polyLine[0] - m_polyLine[m_polyLine.size() - 1];
				if (q.manhattanLength() < 10)
				{
					bool _del = false;

					if (m_RbuttonDown)
						_del = true;

					ACTION_MANAGER->action_InterDraw(this, m_polyLine, m_pWindowManager->getSelectedMask(),
						m_pWindowManager->getSelectedMaskByteIndex(), _del);

					m_polyLine.clear();
				}
			}

			renderLater();
		}
	}
}

void AnalMPRPlaneView::mousePress_Segmentation_MagicCut_or_SegmentRectOval()
{
	if (m_LbuttonDown || m_RbuttonDown)
	{
		bool bType = (m_pWindowManager->getAnalWorkMode() == ANAL_WORK_SEGMENT_RECT || 2 == m_pWindowManager->getDCutShape(true));
		bool bDraw = bType ? m_pWindowManager->pencil_A : m_pWindowManager->oval_A;

		m_polyLine.clear();
		m_polyLine.append(m_MousePos);
		if (!bDraw)
		{
			m_rectRegion = QPainterPath();
			QRect rect;
			float factor = m_sliceCamera.getZoomMax() - m_sliceCamera.getZoom(); /*0 ~ zoommax*/
			factor /= m_sliceCamera.getZoomMax();
			factor += 0.1f;
			auto coord1 = m_list.begin();
			auto coord2 = m_list.last();

			rect.setCoords(m_MousePos.x() + (coord1->x() * factor), m_MousePos.y() + (coord1->y() * factor),
				m_MousePos.x() + (coord2.x() * factor), m_MousePos.y() + (coord2.y() * factor));

			if (bType)
				m_rectRegion.addRect(rect);
			else
				m_rectRegion.addEllipse(rect);
		}
	}
}

void AnalMPRPlaneView::mousePress_Segmentation_Pick_or_FreeDraw()
{
	if (m_LbuttonDown || m_RbuttonDown)
	{
		m_polyLine.clear();

		if (isMousePointInBoundaryBox())
		{
			m_polyLine.append(m_MousePos);
		}
	}
}

void AnalMPRPlaneView::mousePress_InNormalMode()
{
	if (m_LbuttonDown)
	{
		/* Annotation Select를 진행 */
		if (m_pWindowManager->anotationList.size() != 0)
		{
			/* Annotation Select 상태 초기화 */
			for (int i = m_pWindowManager->anotationList.size() - 1; i >= 0; i--)
			{
				Annotation* ano = m_pWindowManager->anotationList.at(i);
				(*ano).clearSelect();
			}

			for (int i = m_pWindowManager->anotationList.size() - 1; i >= 0; i--)
			{
				Annotation* ano = m_pWindowManager->anotationList.at(i);

				bool preSelect = ano->isSelected();
				bool curSelect = ano->setSelect_Measurement(this, m_MousePos.x(), m_MousePos.y());

				/* 새로 선택되었을 경우 화면 업데이트*/
				if (preSelect != curSelect)
				{
					/* 원본 Annotation 정보 저장 */
					m_pWindowManager->renderLater_AnalView();
				}

				/* 선택이 되었다면 원본 Annotation 객체 생성 */
				if (curSelect)
				{
					if (ano->getType() == AT_LEN)
					{
						m_pOriginAnnotation = std::make_unique<AnnoLength>(*(AnnoLength*)ano);
					}
					else if (ano->getType() == AT_ARROW)
					{
						m_pOriginAnnotation = std::make_unique<AnnoArrow>(*(AnnoArrow*)ano);
					}
					else if (ano->getType() == AT_ANGLE)
					{
						m_pOriginAnnotation = std::make_unique<AnnoAngle>(*(AnnoAngle*)ano);
					}
					else if (ano->getType() == AT_RECTANGLE)
					{
						m_pOriginAnnotation = std::make_unique<AnnoRectangle>(*(AnnoRectangle*)ano);
					}
					else if (ano->getType() == AT_TEXT)
					{
						m_pOriginAnnotation = std::make_unique<AnnoString>(*(AnnoString*)ano);
					}

					break;
				}
			}
		}

		/* Axis 축 이동, 회전 처리를 진행한다. */
		switch (m_windowType)
		{
		case WT_CORONAL:
			if (m_AxialSeleted && m_SaggitalSeleted)
			{
				m_translationMPR = true;
				m_rotateMPR = false;
			}
			else if (m_AxialSeleted || m_SaggitalSeleted)
			{
				m_translationMPR = false;
				m_rotateMPR = true;
			}
			break;
		case WT_SAGITTAL:
			if (m_CoronalSeleted && m_AxialSeleted)
			{
				m_translationMPR = true;
				m_rotateMPR = false;
			}
			else if (m_CoronalSeleted || m_AxialSeleted)
			{
				m_translationMPR = false;
				m_rotateMPR = true;
			}
			break;
		default:
		case WT_AXIAL:
			if (m_SaggitalSeleted && m_CoronalSeleted)
			{
				m_translationMPR = true;
				m_rotateMPR = false;
			}
			else if (m_SaggitalSeleted || m_CoronalSeleted)
			{
				m_translationMPR = false;
				m_rotateMPR = true;
			}
			break;
		}

		if (!(m_translationMPR || m_rotateMPR)) //click profile
		{
		}
	}
	else
	{
		m_AxialSeleted = m_SaggitalSeleted = m_CoronalSeleted = m_translationMPR = m_rotateMPR = false;
	}
}

void AnalMPRPlaneView::mousePress_Annotation_InDrawingMode(ANAL_WORK_MODE workMode)
{
	if (m_LbuttonDown)
	{
		if (isMousePointInBoundaryBox() == false)
		{
			return;
		}

		ANNOTATION_TYPE annoType = AT_NONE;
		switch (workMode)
		{
		case ANAL_WORK_ANNO_LENGTH:
			annoType = AT_LEN;
			break;
		case ANAL_WORK_ANNO_TEXT:
			annoType = AT_TEXT;
			break;
		case ANAL_WORK_ANNO_ANGLE:
			annoType = AT_ANGLE;
			break;
		case ANAL_WORK_ANNO_ARROW:
			annoType = AT_ARROW;
			break;
		case ANAL_WORK_ANNO_RECTANGLE:
			annoType = AT_RECTANGLE;
			break;
		case ANAL_WORK_ANNO_PROFILE_LINE:
			annoType = AT_PT_LINE;
			break;
		default:
			break;
		}

		/* Anno Type이 NONE 이면 Skip*/
		if (annoType == AT_NONE)
		{
			return;
		}

		/* Text Type일 경우 Dialog에서 Text 초기화 */
		QString text;
		muint16 fontSize;
		QColor qcolor = m_pWindowManager->annoColor;

		if (annoType == AT_TEXT)
		{
			AnnoTextDlg dlg(this);
			if ((dlg.exec() == QDialog::Accepted) && (dlg.Text.isEmpty() == false))
			{
				text = dlg.Text;
				fontSize = dlg.FontSize;
				qcolor = dlg.Color;
			}
			else
			{
				//CANCEL 시 skip
				return;
			}
		}
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
			m_pCurAnnotation->addDrawingPoint_Measurement(this, m_MousePos.x(), m_MousePos.y());
		}

		/* 현재 Annotation이 null이면 skip */
		if (m_pCurAnnotation == nullptr)
		{
			return;
		}

		/* Drawing이 완료 처리 */
		if (m_pCurAnnotation->GetState() == Annotation::DRAWING_FINISHED)
		{
			/* Annotation List에 추가*/
			if (m_pCurAnnotation->getType() == AT_ARROW)
			{
				AnnoArrow* annoArrow = (AnnoArrow*)m_pCurAnnotation.get();
				ACTION_MANAGER->action_Annotation_Arrow_Add(annoArrow);
			}
			else if (m_pCurAnnotation->getType() == AT_ANGLE)
			{
				AnnoAngle* annoAngle = (AnnoAngle*)m_pCurAnnotation.get();
				ACTION_MANAGER->action_Annotation_Angle_Add(annoAngle);
			}
			else if (m_pCurAnnotation->getType() == AT_LEN)
			{
				AnnoLength* annoLength = (AnnoLength*)m_pCurAnnotation.get();
				ACTION_MANAGER->action_Annotation_Len_Add(annoLength);
			}
			else if (m_pCurAnnotation->getType() == AT_TEXT)
			{
				AnnoString* annoText = (AnnoString*)m_pCurAnnotation.get();
				ACTION_MANAGER->action_Annotation_Text_Add(annoText);
			}
			else if (m_pCurAnnotation->getType() == AT_RECTANGLE)
			{
				AnnoRectangle* annoRect = (AnnoRectangle*)m_pCurAnnotation.get();
				ACTION_MANAGER->action_Annotation_Rectangle_Add(annoRect);
			}
			else if (m_pCurAnnotation->getType() == AT_PT_LINE)
			{
				//TODO : 해당 로직 외부로 변경 
				/* Profile 로직 처리 */
				AnnoProfile* annoProfile = (AnnoProfile*)m_pCurAnnotation.get();

				mip::VECTOR3 start_point = annoProfile->getV1();
				mip::VECTOR3 end_point = annoProfile->getV2();
				mip::VECTOR3 unitDirection = (end_point - start_point).normalize();
				float lineLength = (start_point - end_point).length();

				/* TODO : 왜 가장 작은 Pixel Spacing을 구하는가? */
				float pixelSpace = mip::math::Min(mip::math::Min(
					m_pDataContext->volume_data.getSpaceX(),
					m_pDataContext->volume_data.getSpaceY()),
					m_pDataContext->volume_data.getSpaceZ());

				/* Pixel Spacing 값이 음수이면 Skip */
				if (pixelSpace <= 0)
				{
					return;
				}

				muint32 expect_count = (lineLength / pixelSpace) + 1;

				/* TODO : 왜 Expect Count가 10000개가 넘으면 Skip 하는가? */
				if (expect_count > 10000)
				{
					//TODO warning dlg
					return;
				}

				QMap<float, mint16> huPoints;

				std::vector<mip::VECTOR3> volPoints;
				volPoints.reserve(expect_count);

				/* 1px당 길이 Vector 계산 */
				mip::VECTOR3 pixelUnitDirection = unitDirection * pixelSpace;
				float pixelUnitLength_cm = (start_point - (start_point + pixelUnitDirection)).length();
				float pixelUnitLength_mm = pixelUnitLength_cm * 10;

				/* Start -> End 사이의 Point 저장 */
				mip::VECTOR3 point = start_point;
				mip::AABB box3D = m_pDataContext->volume_data.getBoundingBox3DAABB();
				for (int n = 0; n < expect_count; n++)
				{
					if (box3D.checkPoint(point) == true)
					{
						mip::VECTOR3 value = getWorldToVolume(point.x, point.y, point.z);
						volPoints.push_back(value);
					}

					point += pixelUnitDirection;
				}

				/* 중복되는 Point 제거*/
				std::vector<mip::VECTOR3>::iterator pos;
				pos = std::unique(volPoints.begin(), volPoints.end());
				volPoints.erase(pos, volPoints.end());

				/* Pixel Unit 길이(mm) 별 HU값 초기화 */
				mint16 hu;
				mint16 huMin = MAX_int16;
				mint16 huMax = MIN_int16;

				for (int i = 0; i < volPoints.size(); i++)
				{
					mip::VECTOR3 value = volPoints.at(i);
					hu = m_pDataContext->volume_data.getData(value.x, value.y, value.z);

					huPoints.insert(pixelUnitLength_mm * i, hu);

					if (huMin > hu)
						huMin = hu;
					if (huMax < hu)
						huMax = hu;
				}

				mint16 HuAvg = std::accumulate(huPoints.begin(), huPoints.end(), 0.0) / huPoints.size();

				LineProfileDialog dlg(huPoints, huMin, huMax, HuAvg, lineLength, this);
				dlg.exec();
			}

			/* 상태 초기화 */
			m_pCurAnnotation = nullptr;
			setWorkMode(ANAL_WORK_NONE);

			/* Default 색상 Table 업데이트 */
			m_pWindowManager->getAnnotationColor();

			//m_pWindowManager->renderLater_AnalView();
		}
	}

	ANAL_WORK_MODE analWorkMode = m_pWindowManager->getAnalWorkMode();
	return;
}

void AnalMPRPlaneView::mouseMove_SegmentPolyLine()
{
	/* 정점이 3개 이상 및 버튼을 누르지 않고 마우스 움직임만 있을 경우 */
	if (!(m_LbuttonDown || m_RbuttonDown || m_MbuttonDown) && m_polyLine.size() >= 3)
	{
		if (m_polyLine.size() > 0)
		{
			QPoint q = m_polyLine[0] - m_MousePos;
			if (q.manhattanLength() < 10)
			{
				/* 시작점 위에 올라왔을 경우*/
				if (m_polycheck == false)
				{
					m_polycheck = true;
					renderLater();
				}
			}
			else if (m_polycheck == true)
			{
				/* 시작점 위에서 빠져나왔을 경우 */
				m_polycheck = false;
				renderLater();
			}
		}
		else
		{
			m_polycheck = false;
			renderLater();
		}
	}
}

void AnalMPRPlaneView::mouseMove_Segmentation_Pick_or_FreeDraw()
{
	if (m_LbuttonDown || m_RbuttonDown)
	{
		if (isMousePointInBoundaryBox())
		{
			if (m_polyLine.size() <= 0)
			{
				m_polyLine.append(m_MousePos);
			}
			else
			{
				/* Pick, FreeDraw 영역 Drawing영역 업데이트 */
				const QPoint a = m_polyLine[m_polyLine.size() - 1];

				if (a != m_MousePos) //a : exist, pos : new
				{
					int point[4] = { a.x(),a.y(), m_MousePos.x(), m_MousePos.y() };
					int dx = point[2] - point[0];
					int dy = point[3] - point[1];
					int abs_x = abs(dx);
					int abs_y = abs(dy);
					int dx2 = abs_x << 1;
					int dy2 = abs_y << 1;
					int x_inc = (dx == 0) ? 0 : (dx < 0) ? -1 : 1;
					int y_inc = (dy == 0) ? 0 : (dy < 0) ? -1 : 1;

					QPoint pV;

					if ((abs_x >= abs_y)) {
						int err_1 = dy2 - abs_x;
						for (int i = 0; i < abs_x; i++) {
							//	output[point[1] * width + point[0]] = symbol;
							if (err_1 > 0) {
								point[1] += y_inc;
								err_1 -= dx2;
							}
							err_1 += dy2;
							point[0] += x_inc;

							pV = QPoint(point[0], point[1]);

							if (!m_polyLine.contains(pV))
								m_polyLine.append(pV);
						}
					}
					else {
						int err_1 = dx2 - abs_y;//0-5
						for (int i = 0; i < abs_y; i++) {
							//	output[point[1] * width + point[0]] = symbol;
							if (err_1 > 0) {
								point[0] += x_inc;
								err_1 -= dy2;
							}
							err_1 += dx2;//-5
							point[1] += y_inc;//

							pV = QPoint(point[0], point[1]);
							if (!m_polyLine.contains(pV))
								m_polyLine.append(pV);
						}
					}
					pV = QPoint(point[0], point[1]);

					if (!m_polyLine.contains(pV))
						m_polyLine.append(pV);

					pV = m_MousePos;

					if (!m_polyLine.contains(pV))
						m_polyLine.append(pV);

				}

			}
			renderLater();
		}
	}
}

void AnalMPRPlaneView::mouseMove_Segmentation_MagicCut_or_RectOval()
{
	if (m_LbuttonDown || m_RbuttonDown)
	{
		bool bType = (m_pWindowManager->getAnalWorkMode() == ANAL_WORK_SEGMENT_RECT || 2 == m_pWindowManager->getDCutShape(true)); //rect/angular type -> true
		bool bDraw = bType ? m_pWindowManager->pencil_A : m_pWindowManager->oval_A;

		if (bDraw)
		{
			if (bType)
			{
				if (m_polyLine.size() <= 0)
				{
					m_polyLine.append(m_MousePos);
				}
				else
				{
					const QPoint a = m_polyLine[m_polyLine.size() - 1];

					if (a != m_MousePos) //a : exist, pos : new
					{
						int point[4] = { a.x(),a.y(), m_MousePos.x(), m_MousePos.y() };
						int dx = point[2] - point[0];
						int dy = point[3] - point[1];
						int abs_x = abs(dx);
						int abs_y = abs(dy);
						int dx2 = abs_x << 1;
						int dy2 = abs_y << 1;
						int x_inc = (dx == 0) ? 0 : (dx < 0) ? -1 : 1;
						int y_inc = (dy == 0) ? 0 : (dy < 0) ? -1 : 1;

						QPoint pV;

						if ((abs_x >= abs_y)) {
							int err_1 = dy2 - abs_x;
							for (int i = 0; i < abs_x; i++) {
								//	output[point[1] * width + point[0]] = symbol;
								if (err_1 > 0) {
									point[1] += y_inc;
									err_1 -= dx2;
								}
								err_1 += dy2;
								point[0] += x_inc;

								pV = QPoint(point[0], point[1]);

								if (!m_polyLine.contains(pV))
									m_polyLine.append(pV);
							}
						}
						else {
							int err_1 = dx2 - abs_y;//0-5
							for (int i = 0; i < abs_y; i++) {
								//	output[point[1] * width + point[0]] = symbol;
								if (err_1 > 0) {
									point[0] += x_inc;
									err_1 -= dy2;
								}
								err_1 += dx2;//-5
								point[1] += y_inc;//

								pV = QPoint(point[0], point[1]);
								if (!m_polyLine.contains(pV))
									m_polyLine.append(pV);
							}
						}
						pV = QPoint(point[0], point[1]);

						if (!m_polyLine.contains(pV))
							m_polyLine.append(pV);

						pV = m_MousePos;

						if (!m_polyLine.contains(pV))
							m_polyLine.append(pV);

					}

				}
			}
			else
			{
				if (m_polyLine.size() >= 2)
					m_polyLine.replace(1, m_MousePos);
				else
					m_polyLine.append(m_MousePos);
			}
		}
		else
		{
			auto coord1 = m_list.begin();
			auto coord2 = m_list.last();
			float factor = m_sliceCamera.getZoomMax() - m_sliceCamera.getZoom(); /*0 ~ zoommax*/
			factor /= m_sliceCamera.getZoomMax();
			factor += 0.1f;
			if (m_polyLine.size() <= 0)
			{
				QRect rect;
				rect.setCoords(m_MousePos.x() + (coord1->x() * factor), m_MousePos.y() + (coord1->y() * factor),
					m_MousePos.x() + (coord2.x() * factor), m_MousePos.y() + (coord2.y() * factor));

				if (bType)
					m_rectRegion.addRect(rect);
				else
					m_rectRegion.addEllipse(rect);

				m_polyLine.append(m_MousePos);
			}
			else
			{
				const QPoint& a = m_polyLine[m_polyLine.size() - 1];

				if (a != m_MousePos)
				{
					QPointF dir = (m_MousePos - a);
					QPoint p, pV;
					int length = qSqrt(dir.x() * dir.x() + dir.y() * dir.y());
					dir.setX(dir.x() / length);
					dir.setY(dir.y() / length);
					for (int j = 0; j < length; j++)
					{
						p = a + QPoint(dir.x() * j, dir.y() * j);
						pV = p;

						QRect rect;

						rect.setCoords(pV.x() + (coord1->x() * factor), pV.y() + (coord1->y() * factor),
							pV.x() + (coord2.x() * factor), pV.y() + (coord2.y() * factor));

						if (bType)
							m_rectRegion.addRect(rect);
						else
							m_rectRegion.addEllipse(rect);
					}
					m_polyLine.append(m_MousePos);
				}
			}
		}
		renderLater();
	}
}

void AnalMPRPlaneView::mouseMove_CaptureArea()
{
	bool mode = m_pWindowManager->GetTab()->getCaptureTab()->getCaptureMode();

	if (m_LbuttonDown)
	{
		if (!mode) //rectangle
		{
			if (m_polyLine.count() >= 2)
			{
				m_polyLine.replace(1, m_MousePos);
			}
			else
			{
				m_polyLine.push_back(m_MousePos);
			}
		}
		renderLater();
	}
}

void AnalMPRPlaneView::mouseMove_Annotation_InDrawingMode()
{
	if (isMousePointInBoundaryBox())
	{
		if (m_pCurAnnotation != nullptr)
		{
			m_pCurAnnotation->movePosition_Measurement(this, m_MousePos.x(), m_MousePos.y());
			renderLater();
		}
	}
}

void AnalMPRPlaneView::mouseMove_Annotation_NormalMode()
{
	for (auto ano = m_pWindowManager->anotationList.begin(); ano != m_pWindowManager->anotationList.end(); ++ano)
	{
		if (isMousePointInBoundaryBox())
		{
			if ((*ano)->isSelected() && m_LbuttonDown)
			{
				(*ano)->movePosition_Measurement(this, m_MousePos.x(), m_MousePos.y());
			}
			else
			{
				(*ano)->setHover_Measurement(this, m_MousePos.x(), m_MousePos.y());
			}
			renderLater();
		}
	}
}

void AnalMPRPlaneView::mouseRelease_CaptureArea()
{
	if (m_LbuttonDown && m_polyLine.count() >= 2)
	{
		bool type = m_pWindowManager->GetTab()->getCaptureTab()->getCaptureType();

		QImage img = getCaptureImage(false, type);

		if (img != QImage())
			ACTION_MANAGER->action_Capture_image_Add(img);

		m_polyLine.clear();
	}
	setWorkMode(ANAL_WORK_NONE);
}

void AnalMPRPlaneView::mouseRelease_Animation()
{
	if (m_LbuttonDown || m_RbuttonDown)
	{
		bool del = m_RbuttonDown;
		mip::VECTOR3 point;
		QVector3D vec;

		if (getCurrentCameraToPlaneIntersectedPoint(&point))
		{
			mip::AABB box3D = m_pDataContext->volume_data.getBoundingBox3DAABB();
			if (isBoxAABB_ContainPoint(box3D, point))
			{
				point = getWorldToVolume(point.x, point.y, point.z);
				vec.setX(point.x);
				vec.setY(point.y);
				vec.setZ(point.z);

				int group = m_pWindowManager->getAnimationGroup();
				if (del)
				{
					if (m_pWindowManager->aniCount.at(group) > 0)
						ACTION_MANAGER->action_Annotation_Path_Clear(group);
				}
				else
				{
					if (!m_pWindowManager->isContainAniPoint(vec, group))
						ACTION_MANAGER->action_Annotation_Path_Add(vec, group);
				}
			}
		}
	}
}

void AnalMPRPlaneView::mouseRelease_Segmentation_MagicCut_or_RectOval()
{
	bool bType = (m_pWindowManager->getAnalWorkMode() == ANAL_WORK_SEGMENT_RECT || 2 == m_pWindowManager->getDCutShape(true));
	bool bDraw = bType ? m_pWindowManager->pencil_A : m_pWindowManager->oval_A;

	if (!bDraw)
	{
		if (m_pWindowManager->getAnalWorkMode() == ANAL_WORK_SKETCHDRAWSEGMENTATION)
		{
			ACTION_MANAGER->action_MarkingInterSquare(m_rectRegion, this, m_RbuttonDown ? VM_MASK0 : VM_MASK1);
		}
		else if (m_LbuttonDown || m_RbuttonDown)
		{
			ACTION_MANAGER->action_MarkingInterSquare(m_rectRegion, this, m_pWindowManager->getSelectedMask(), m_pWindowManager->getSelectedMaskByteIndex(), m_RbuttonDown);
		}

		m_rectRegion = QPainterPath();
	}
	else
	{
		if (m_pWindowManager->getAnalWorkMode() == ANAL_WORK_SKETCHDRAWSEGMENTATION)
		{
			ACTION_MANAGER->action_InterDraw(this, m_polyLine,
				m_RbuttonDown ? VM_MASK0 : VM_MASK1,
				0, false,
				!bType, true);
		}
		else if (m_LbuttonDown || m_RbuttonDown)
		{
			ACTION_MANAGER->action_InterDraw(this, m_polyLine,
				m_pWindowManager->getSelectedMask(),
				m_pWindowManager->getSelectedMaskByteIndex(), m_RbuttonDown,
				!bType, true, m_shift);
		}
	}
	m_polyLine.clear();
	renderLater();
}

void AnalMPRPlaneView::mouseRelease_Segmentation_Pick_or_FreeDraw()
{
	if (m_LbuttonDown || m_RbuttonDown)
	{
		bool del = m_RbuttonDown;

		if (m_polyLine.size() > 0)
		{
			ACTION_MANAGER->action_InterDraw(
				this,
				m_polyLine,
				m_pWindowManager->getSelectedMask(),
				m_pWindowManager->getSelectedMaskByteIndex(), del, false,
				ANAL_WORK_SEGMENT_PICKER == m_pWindowManager->getAnalWorkMode());
		}
		m_polyLine.clear();
	}
}

void AnalMPRPlaneView::mouseRelease_EditSelectedAnnotation()
{
	int index = 0;
	for (auto ano = m_pWindowManager->anotationList.begin(); ano != m_pWindowManager->anotationList.end(); ++ano)
	{
		if ((*ano)->isSelected())
		{
			if (m_MousePos != m_MousePos_Pressed)
			{
				if (commitToUpdateNewAnnotationStatus(*ano, index))
				{
					break;
				}
			}
		}
		index++;
	}
}

bool AnalMPRPlaneView::commitToUpdateNewAnnotationStatus(Annotation* ano, int index)
{
	if (ano->getType() == AT_LEN)
	{
		AnnoLength* pAnnoOrigin = (AnnoLength*)m_pOriginAnnotation.get();
		AnnoLength* pAnnoNew = (AnnoLength*)ano;
		ACTION_MANAGER->action_Annotation_Len_Edit(index, pAnnoOrigin, pAnnoNew);

		return true;
	}
	else if (ano->getType() == AT_ARROW)
	{
		AnnoArrow* pAnnoOrigin = (AnnoArrow*)m_pOriginAnnotation.get();
		AnnoArrow* pAnnoNew = (AnnoArrow*)ano;
		ACTION_MANAGER->action_Annotation_Arrow_Edit(index, pAnnoOrigin, pAnnoNew);

		return true;
	}
	else if (ano->getType() == AT_ANGLE)
	{
		AnnoAngle* pAnnoOrigin = (AnnoAngle*)m_pOriginAnnotation.get();
		AnnoAngle* pAnnoNew = (AnnoAngle*)ano;
		ACTION_MANAGER->action_Annotation_Angle_Edit(index, pAnnoOrigin, pAnnoNew);

		return true;
	}
	else if (ano->getType() == AT_TEXT)
	{
		AnnoString* pAnnoOrigin = (AnnoString*)m_pOriginAnnotation.get();
		AnnoString* pAnnoNew = (AnnoString*)ano;
		ACTION_MANAGER->action_Annotation_Text_Edit(NULL, index, pAnnoOrigin, pAnnoNew);

		return true;
	}
	else if (ano->getType() == AT_RECTANGLE)
	{
		AnnoRectangle* pAnnoOrigin = (AnnoRectangle*)m_pOriginAnnotation.get();
		AnnoRectangle* pAnnoNew = (AnnoRectangle*)ano;
		ACTION_MANAGER->action_Annotation_Rectangle_Edit(index, pAnnoOrigin, pAnnoNew);

		return true;
	}

	return false;
}

void AnalMPRPlaneView::drawAnnotationList(QPainter* p)
{
	if (m_pCurAnnotation)
	{
		m_pCurAnnotation->drawAnno(this, p, ADV_MPR_PLANE_MEASUREMENT_VIEWER);
	}

	for (auto ano = m_pWindowManager->anotationList.begin(); ano != m_pWindowManager->anotationList.end(); ++ano)
	{
		if ((*ano)->isAnnoHidden())
		{
			continue;
		}

		if ((*ano)->getType() == AT_LEN)
		{
			if (!m_pWindowManager->bShowAnnoLength)
				continue;

			(*ano)->drawAnno(this, p, ADV_MPR_PLANE_MEASUREMENT_VIEWER);
		}
		else if ((*ano)->getType() == AT_TEXT)
		{
			if (!m_pWindowManager->bShowAnnoText)
				continue;

			(*ano)->drawAnno(this, p, ADV_MPR_PLANE_MEASUREMENT_VIEWER);
		}
		else if ((*ano)->getType() == AT_ANGLE)
		{
			if (!m_pWindowManager->bShowAnnoAngle)
				continue;

			(*ano)->drawAnno(this, p, ADV_MPR_PLANE_MEASUREMENT_VIEWER);
		}
		else if ((*ano)->getType() == AT_ARROW)
		{
			if (!m_pWindowManager->bShowAnnoArrow)
				continue;

			(*ano)->drawAnno(this, p, ADV_MPR_PLANE_MEASUREMENT_VIEWER);
		}
		else if ((*ano)->getType() == AT_RECTANGLE)
		{
			if (!m_pWindowManager->bShowAnnoRectangle)
				continue;

			(*ano)->drawAnno(this, p, ADV_MPR_PLANE_MEASUREMENT_VIEWER);
		}
	}
}

void AnalMPRPlaneView::drawMousework_Segmentation_MagicCut_or_RectOval(QPainter* p)
{
	bool bType = (m_pWindowManager->getAnalWorkMode() == ANAL_WORK_SEGMENT_RECT || 2 == m_pWindowManager->getDCutShape(true));
	bool bDraw = bType ? m_pWindowManager->pencil_A : m_pWindowManager->oval_A;
	if (bDraw)
	{
		if (m_RbuttonDown || m_LbuttonDown)
		{
			QPainterPath region;

			if (bType)
			{
				if (!m_polyLine.isEmpty())
					region.addPolygon(m_polyLine);
			}
			else if (m_polyLine.size() >= 2)
			{
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
			}

			if (m_pWindowManager->getAnalWorkMode() == ANAL_WORK_SKETCHDRAWSEGMENTATION)
			{
				COLOR _col = m_pWindowManager->getSeedColor(m_LbuttonDown); //false : backseed, true:foreseed

				p->setPen(QPen(QColor(_col.r, _col.g, _col.b)));
			}
			else
			{
				if (m_RbuttonDown)
				{
					p->setPen(QPen(Qt::black, 1.2));
					if (!region.isEmpty())
						p->drawPath(region);
				}

				if (m_LbuttonDown)
					p->setPen(QPen(m_pWindowManager->getSelectedMaskColor()));
				else
					p->setPen(QPen(Qt::white, 1.0));
			}

			if (!region.isEmpty())
				p->drawPath(region);
		}
	}
	else
	{
		drawSquareLine(p, m_pWindowManager->getAnalWorkMode() == ANAL_WORK_SKETCHDRAWSEGMENTATION);
	}
}

void AnalMPRPlaneView::drawMousework_Segmentation_PolyLine(QPainter* p)
{
	if (m_polyLine.size() > 0)
	{
		p->setPen(QPen(Qt::green, 1.25));
		p->drawPolyline(m_polyLine);

		p->setPen(QPen(Qt::yellow, 1));

		for (int i = 1; i < m_polyLine.size(); i++)
			p->drawEllipse(QPoint(m_polyLine.at(i).x(), m_polyLine.at(i).y()), 2, 2);

		if (m_polycheck)
			p->setPen(QPen(Qt::red, 2.5));

		p->drawEllipse(QPoint(m_polyLine.at(0).x(), m_polyLine.at(0).y()), 2, 2);
	}
}

void AnalMPRPlaneView::drawMousework_Segmentation_Pick_or_FreeDraw(QPainter* p)
{
	if ((m_RbuttonDown || m_LbuttonDown) && m_polyLine.size() > 0)
	{
		if (m_LbuttonDown)
		{
			p->setPen(QPen(m_pWindowManager->getSelectedMaskColor()));
			p->drawPolyline(m_polyLine);
		}
		else
		{
			p->setPen(QPen(Qt::black, 1.2));
			p->drawPolyline(m_polyLine);
			p->setPen(QPen(Qt::white, 1.0));
			p->drawPolyline(m_polyLine);
		}
	}
}

void AnalMPRPlaneView::drawMousework_CaptureArea(QPainter* p)
{
	bool windowFocused = m_pWindowManager->getLatestActiveViewType() == m_windowType;
	if (!windowFocused)
	{
		return;
	}

	bool mode = m_pWindowManager->GetTab()->getCaptureTab()->getCaptureMode();

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

	if (m_polyLine.count() >= 2)
	{
		p->setPen(QPen(Qt::red, 1.5));

		QRect rect;

		rect.setCoords(m_polyLine.at(0).x(), m_polyLine.at(0).y(),
			m_polyLine.at(1).x(), m_polyLine.at(1).y());

		p->drawRect(rect);
	}
}

void AnalMPRPlaneView::drawMousework_Animation_Line(QPainter* p)
{
	int group = m_pWindowManager->getAnimationGroup();
	if (m_pWindowManager->aniCount.at(group) <= 0)
	{
		return;
	}

	std::vector<mip::VECTOR3> tempList;
	QVector<QVector3D>* Lines = &(m_pWindowManager->aniLine);
	mip::MATRIX44 matView = getCamera().getView();
	mip::MATRIX44 matProj = getCamera().getProj();
	int nStart, nEnd;

	m_pDataContext->volume_data.getMPRPPlanes(getType(), tempList);
	mip::PLANE pl(tempList[0], tempList[1], tempList[2]);

	nEnd = nStart = m_pWindowManager->getAniStartPoint(group);
	nEnd += m_pWindowManager->aniCount.at(group);

	p->setPen(QPen(Qt::red, 2));
	VOLUME_DATA& vd = m_pDataContext->volume_data;

	for (int i = nStart; i < nEnd; i++)
	{
		QVector3D pos = Lines->at(i);

		bool bInArea = false;
		float cx, cy, cz;

		cx = cy = cz = 0;

		cx = vd.getSpaceX() * pos.x() - vd.getSizeX() * 0.5f;
		cy = vd.getSpaceY() * pos.y() - vd.getSizeY() * 0.5f;
		cz = vd.getSpaceZ() * pos.z() - vd.getSizeZ() * 0.5f;
		mip::VECTOR3 v1(cx, cy, cz);

		float res = pl.dot(v1);

		if ((-0.1f <= res && res <= 0.2f))
		{
			bInArea = true;
		}

		if (!bInArea)
		{
			continue;
		}

		v1 = mip::geom::WorldToScreen(v1, width(), height(), matView, matProj);

		p->drawEllipse(QPoint(v1.x, v1.y), 2, 2);
	}
}


void AnalMPRPlaneView::clearWindowManagerStatus(ANAL_WORK_MODE mode)
{
	switch (mode)
	{
	case ANAL_WORK_PATH_3D_PLAY:
		m_pWindowManager->AniType = WT_NONE;
		m_pWindowManager->bStartAni = false;
		break;
	case ANAL_WORK_PATH:
		m_pWindowManager->bCreateAni = false;
		if (m_pWindowManager->GetTab())
		{
			AnimationTab* tab = m_pWindowManager->GetTab()->getAniTab();

			if (tab)
				tab->CompleteAnimation();
		}
		break;
	case ANAL_WORK_3D_PLANE_SPLIT:
		m_pWindowManager->setClipOpt();
		break;
	case ANAL_WORK_CAPTURE:
		if (m_pWindowManager->GetTab()->getCaptureTab())
		{
			m_pWindowManager->GetTab()->getCaptureTab()->cancelCapture();
		}
		break;
	case ANAL_WORK_ANNO_PROFILE_LINE:
	case ANAL_WORK_SEGMENT_OVAL:
	case ANAL_WORK_SEGMENT_POLY:
	case ANAL_WORK_SEGMENT_FREEDRAW:
	case ANAL_WORK_SEGMENT_PICKER:
	case ANAL_WORK_SEGMENT_RECT:
	case ANAL_WORK_ANNO_ARROW:
	case ANAL_WORK_ANNO_ANGLE:
	case ANAL_WORK_ANNO_LENGTH:
	case ANAL_WORK_ANNO_TEXT:
	case ANAL_WORK_ANNO_RECTANGLE:
	case ANAL_WORK_NONE:
	default:
		break;
	}
}

void AnalMPRPlaneView::updateRightMenuButtonStatus_SelectedOrNot(ANAL_WORK_MODE mode, bool iconRefresh)
{
	static QIcon segmentPolyIcon[] = { RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_POLYROI, m_nIconSize, m_nIconSize) ,
		RESOURCE_MANAGER->getIcon(ICON_VIEW_POLYROI, m_nIconSize, m_nIconSize) };
	static QIcon segmentPickIcon[] = { RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_PICKERROI, m_nIconSize, m_nIconSize) ,
		RESOURCE_MANAGER->getIcon(ICON_VIEW_PICKERROI, m_nIconSize, m_nIconSize) };
	static QIcon segmentFreeDrawIcon[] = { RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_FREEDRAWROI, m_nIconSize, m_nIconSize) ,
		RESOURCE_MANAGER->getIcon(ICON_VIEW_FREEDRAWROI, m_nIconSize, m_nIconSize) };
	static QIcon segmentOvalIcon[] = { RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_OVALROI, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_VIEW_OVALROI, m_nIconSize, m_nIconSize) };
	static QIcon segmentRectIcon[] = { RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_ANGLEDROI, m_nIconSize, m_nIconSize) ,
		RESOURCE_MANAGER->getIcon(ICON_VIEW_ANGLEDROI, m_nIconSize, m_nIconSize) };

	static QIcon annoLenIcon[] = { RESOURCE_MANAGER->getIcon(ICON_NON_ANNO_LENGTH, m_nIconSize, m_nIconSize) ,
		RESOURCE_MANAGER->getIcon(ICON_ANNO_LENGTH, m_nIconSize, m_nIconSize) };
	static QIcon annoRectIcon[] = { RESOURCE_MANAGER->getIcon(ICON_NON_ANNO_RECT, m_nIconSize, m_nIconSize) ,
		RESOURCE_MANAGER->getIcon(ICON_ANNO_RECT, m_nIconSize, m_nIconSize) };
	static QIcon annoAngleIcon[] = { RESOURCE_MANAGER->getIcon(ICON_NON_ANNO_ANGLE, m_nIconSize, m_nIconSize) ,
		RESOURCE_MANAGER->getIcon(ICON_ANNO_ANGLE, m_nIconSize, m_nIconSize) };
	static QIcon annoArrowIcon[] = { RESOURCE_MANAGER->getIcon(ICON_NON_ANNO_ARROW, m_nIconSize, m_nIconSize) ,
		RESOURCE_MANAGER->getIcon(ICON_ANNO_ARROW, m_nIconSize, m_nIconSize) };
	static QIcon annoTextIcon[] = { RESOURCE_MANAGER->getIcon(ICON_NON_ANNO_TEXT, m_nIconSize, m_nIconSize) ,
		RESOURCE_MANAGER->getIcon(ICON_ANNO_TEXT, m_nIconSize, m_nIconSize) };

	static QIcon profileLineIcon[] = { RESOURCE_MANAGER->getIcon(ICON_NON_ANNO_PROF_LINE, m_nIconSize, m_nIconSize) ,
		RESOURCE_MANAGER->getIcon(ICON_ANNO_PROF_LINE, m_nIconSize, m_nIconSize) };


	bool chkMode = false;
	bool chkRefresh = false;

	if (m_btnSegmentPoly)
	{
		if (chkRefresh = ((mode == ANAL_WORK_SEGMENT_POLY) != (chkMode = m_btnSegmentPoly->isChecked())))
		{
			chkMode = !chkMode;
			m_btnSegmentPoly->setChecked(chkMode);
			m_btnSegmentPoly->setIcon(segmentPolyIcon[chkMode]);
		}
		else if (!chkRefresh && iconRefresh)
			m_btnSegmentPoly->setIcon(segmentPolyIcon[chkMode]);
	}

	if (m_btnSegmentPicker)
	{
		if (chkRefresh = ((mode == ANAL_WORK_SEGMENT_PICKER) != (chkMode = m_btnSegmentPicker->isChecked())))
		{
			chkMode = !chkMode;
			m_btnSegmentPicker->setChecked(chkMode);
			m_btnSegmentPicker->setIcon(segmentPickIcon[chkMode]);
		}
		else if (!chkRefresh && iconRefresh)
			m_btnSegmentPicker->setIcon(segmentPickIcon[chkMode]);
	}

	if (m_btnSegmentDraw)
	{
		if (chkRefresh = ((mode == ANAL_WORK_SEGMENT_FREEDRAW) != (chkMode = m_btnSegmentDraw->isChecked())))
		{
			chkMode = !chkMode;
			m_btnSegmentDraw->setChecked(chkMode);
			m_btnSegmentDraw->setIcon(segmentFreeDrawIcon[chkMode]);
		}
		else if (!chkRefresh && iconRefresh)
			m_btnSegmentDraw->setIcon(segmentFreeDrawIcon[chkMode]);
	}

	if (m_btnSegmentRect)
	{
		if (chkRefresh = ((mode == ANAL_WORK_SEGMENT_RECT)) != (chkMode = m_btnSegmentRect->isChecked()))
		{
			chkMode = !chkMode;
			m_btnSegmentRect->setChecked(chkMode);
			m_btnSegmentRect->setIcon(segmentRectIcon[chkMode]);
		}
		else if (!chkRefresh && iconRefresh)
			m_btnSegmentRect->setIcon(segmentRectIcon[chkMode]);
	}

	if (m_btnSegmentOval)
	{
		if (chkRefresh = ((mode == ANAL_WORK_SEGMENT_OVAL)) != (chkMode = m_btnSegmentOval->isChecked()))
		{
			chkMode = !chkMode;
			m_btnSegmentOval->setChecked(chkMode);
			m_btnSegmentOval->setIcon(segmentOvalIcon[chkMode]);
		}
		else if (!chkRefresh && iconRefresh)
			m_btnSegmentOval->setIcon(segmentOvalIcon[chkMode]);
	}

	if (m_btnProfileLine)
	{
		if (chkRefresh = ((mode == ANAL_WORK_ANNO_PROFILE_LINE) != (chkMode = m_btnProfileLine->isChecked())))
		{
			chkMode = !chkMode;
			m_btnProfileLine->setChecked(chkMode);
			m_btnProfileLine->setIcon(profileLineIcon[chkMode]);
		}
		else if (!chkRefresh && iconRefresh)
			m_btnProfileLine->setIcon(profileLineIcon[chkMode]);
	}

	if (m_btnAnnoArrow)
	{
		if (chkRefresh = ((mode == ANAL_WORK_ANNO_ARROW)) != (chkMode = m_btnAnnoArrow->isChecked()))
		{
			chkMode = !chkMode;
			m_btnAnnoArrow->setChecked(chkMode);
			m_btnAnnoArrow->setIcon(annoArrowIcon[chkMode]);
		}
		else if (!chkRefresh && iconRefresh)
			m_btnAnnoArrow->setIcon(annoArrowIcon[chkMode]);
	}

	if (m_btnAnnoRect)
	{
		if (chkRefresh = ((mode == ANAL_WORK_ANNO_RECTANGLE)) != (chkMode = m_btnAnnoRect->isChecked()))
		{
			chkMode = !chkMode;
			m_btnAnnoRect->setChecked(chkMode);
			m_btnAnnoRect->setIcon(annoRectIcon[chkMode]);
		}
		else if (!chkRefresh && iconRefresh)
			m_btnAnnoRect->setIcon(annoRectIcon[chkMode]);
	}

	if (m_btnAnnoText)
	{
		if (chkRefresh = ((mode == ANAL_WORK_ANNO_TEXT)) != (chkMode = m_btnAnnoText->isChecked()))
		{
			chkMode = !chkMode;
			m_btnAnnoText->setChecked(chkMode);
			m_btnAnnoText->setIcon(annoTextIcon[chkMode]);
		}
		else if (!chkRefresh && iconRefresh)
			m_btnAnnoText->setIcon(annoTextIcon[chkMode]);
	}

	if (m_btnAnnoLength)
	{
		if (chkRefresh = ((mode == ANAL_WORK_ANNO_LENGTH)) != (chkMode = m_btnAnnoLength->isChecked()))
		{
			chkMode = !chkMode;
			m_btnAnnoLength->setChecked(chkMode);
			m_btnAnnoLength->setIcon(annoLenIcon[chkMode]);
		}
		else if (!chkRefresh && iconRefresh)
			m_btnAnnoLength->setIcon(annoLenIcon[chkMode]);
	}

	if (m_btnAnnoAngle)
	{
		if (chkRefresh = ((mode == ANAL_WORK_ANNO_ANGLE)) != (chkMode = m_btnAnnoAngle->isChecked()))
		{
			chkMode = !chkMode;
			m_btnAnnoAngle->setChecked(chkMode);
			m_btnAnnoAngle->setIcon(annoAngleIcon[chkMode]);
		}
		else if (!chkRefresh && iconRefresh)
			m_btnAnnoAngle->setIcon(annoAngleIcon[chkMode]);
	}
}

void AnalMPRPlaneView::updateRightMenuButtonStatus_HoverOrNot(QEvent::Type type, QEvent* e, QObject* target)
{
	static QIcon annoLengthIcon[] = { RESOURCE_MANAGER->getIcon(ICON_NON_ANNO_LENGTH, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_ANNO_LENGTH_HOVER, m_nIconSize, m_nIconSize) };
	static QIcon annoTextIcon[] = { RESOURCE_MANAGER->getIcon(ICON_NON_ANNO_TEXT, m_nIconSize, m_nIconSize) ,
		RESOURCE_MANAGER->getIcon(ICON_ANNO_TEXT_HOVER, m_nIconSize, m_nIconSize) };
	static QIcon annoAngleIcon[] = { RESOURCE_MANAGER->getIcon(ICON_NON_ANNO_ANGLE, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_ANNO_ANGLE_HOVER, m_nIconSize, m_nIconSize) };
	static QIcon annoArrowIcon[] = { RESOURCE_MANAGER->getIcon(ICON_NON_ANNO_ARROW, m_nIconSize, m_nIconSize) ,
		RESOURCE_MANAGER->getIcon(ICON_ANNO_ARROW_HOVER, m_nIconSize, m_nIconSize) };
	static QIcon annoRectIcon[] = { RESOURCE_MANAGER->getIcon(ICON_NON_ANNO_RECT, m_nIconSize, m_nIconSize) ,
		RESOURCE_MANAGER->getIcon(ICON_ANNO_RECT_HOVER, m_nIconSize, m_nIconSize) };

	static QIcon segmentPolyIcon[] = { RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_POLYROI, m_nIconSize, m_nIconSize) ,
		RESOURCE_MANAGER->getIcon(ICON_VIEW_POLYROI_HOVER, m_nIconSize, m_nIconSize) };
	static QIcon segmentPickIcon[] = { RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_PICKERROI, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_VIEW_PICKERROI_HOVER, m_nIconSize, m_nIconSize) };
	static QIcon segmentFreeDrawIcon[] = { RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_FREEDRAWROI, m_nIconSize, m_nIconSize) ,
		RESOURCE_MANAGER->getIcon(ICON_VIEW_FREEDRAWROI_HOVER, m_nIconSize, m_nIconSize) };
	static QIcon segmentRectIcon[] = { RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_ANGLEDROI, m_nIconSize, m_nIconSize) ,
		RESOURCE_MANAGER->getIcon(ICON_VIEW_ANGLEDROI_HOVER, m_nIconSize, m_nIconSize) };
	static QIcon segmentOvalIcon[] = { RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_OVALROI, m_nIconSize, m_nIconSize) ,
		RESOURCE_MANAGER->getIcon(ICON_VIEW_OVALROI_HOVER, m_nIconSize, m_nIconSize) };

	static QIcon profileLineIcon[] = { RESOURCE_MANAGER->getIcon(ICON_NON_ANNO_PROF_LINE, m_nIconSize, m_nIconSize) ,
		RESOURCE_MANAGER->getIcon(ICON_ANNO_PROF_LINE_HOVER, m_nIconSize, m_nIconSize) };

	static QIcon fullIcon[] = { RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_FULL_SCREEN, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_VIEW_FULL_SCREEN_HOVER, m_nIconSize, m_nIconSize) };
	static QIcon axisIcon[] = { RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_2D_AXISES, m_nIconSize, m_nIconSize) ,
		RESOURCE_MANAGER->getIcon(ICON_VIEW_2D_AXISES_HOVER, m_nIconSize, m_nIconSize) };

	if (type == QEvent::HoverLeave ||
		type == QEvent::Leave || type == QEvent::Show)
	{
		if (m_HoverWidget)
		{
			if (!m_HoverWidget->isChecked())
			{
				m_HoverWidget->setIcon(m_LeaveIcon);
			}
		}
	}

	int res = 0;
	QWidget* w = dynamic_cast<QWidget*>(target);
	{
		QMouseEvent* evt = dynamic_cast<QMouseEvent*>(e);
		if (evt)
		{
			if (w)
			{
				if (w->rect().contains(evt->pos()))
					res = 1;
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
					res = 1;
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
					res = 1;
			}
		}
	}

	if (res == 1)
	{
		if (target == m_btnAnnoLength && !m_btnAnnoLength->isChecked())
		{
			m_btnAnnoLength->setIcon(annoLengthIcon[res]);
			m_HoverWidget = m_btnAnnoLength;
			m_LeaveIcon = annoLengthIcon[0];
		}
		else if (target == m_btnAnnoText && !m_btnAnnoText->isChecked())
		{
			m_btnAnnoText->setIcon(annoTextIcon[res]);
			m_HoverWidget = m_btnAnnoText;
			m_LeaveIcon = annoTextIcon[0];
		}
		else if (target == m_btnAnnoAngle && !m_btnAnnoAngle->isChecked())
		{
			m_btnAnnoAngle->setIcon(annoAngleIcon[res]);
			m_HoverWidget = m_btnAnnoAngle;
			m_LeaveIcon = annoAngleIcon[0];
		}
		else if (target == m_btnSegmentOval && !m_btnSegmentOval->isChecked())
		{
			m_btnSegmentOval->setIcon(segmentOvalIcon[res]);
			m_HoverWidget = m_btnSegmentOval;
			m_LeaveIcon = segmentOvalIcon[0];
		}
		else if (target == m_btnSegmentPicker && !m_btnSegmentPicker->isChecked())
		{
			m_btnSegmentPicker->setIcon(segmentPickIcon[res]);
			m_HoverWidget = m_btnSegmentPicker;
			m_LeaveIcon = segmentPickIcon[0];
		}
		else if (target == m_btnSegmentRect && !m_btnSegmentRect->isChecked())
		{
			m_btnSegmentRect->setIcon(segmentRectIcon[res]);
			m_HoverWidget = m_btnSegmentRect;
			m_LeaveIcon = segmentRectIcon[0];
		}
		else if (target == m_btnAnnoArrow && !m_btnAnnoArrow->isChecked())
		{
			m_btnAnnoArrow->setIcon(annoArrowIcon[res]);
			m_HoverWidget = m_btnAnnoArrow;
			m_LeaveIcon = annoArrowIcon[0];
		}
		else if (target == m_btnAnnoRect && !m_btnAnnoRect->isChecked())
		{
			m_btnAnnoRect->setIcon(annoRectIcon[res]);
			m_HoverWidget = m_btnAnnoRect;
			m_LeaveIcon = annoRectIcon[0];
		}
		else if (target == m_btnShowAxis && !m_btnShowAxis->isChecked())
		{
			m_btnShowAxis->setIcon(axisIcon[res]);
			m_HoverWidget = m_btnShowAxis;
			m_LeaveIcon = axisIcon[0];
		}
		else if (target == m_btnProfileLine && !m_btnProfileLine->isChecked())
		{
			m_btnProfileLine->setIcon(profileLineIcon[res]);
			m_HoverWidget = m_btnProfileLine;
			m_LeaveIcon = profileLineIcon[0];
		}
		else if (target == m_btnSegmentDraw && !m_btnSegmentDraw->isChecked())
		{
			m_btnSegmentDraw->setIcon(segmentFreeDrawIcon[res]);
			m_HoverWidget = m_btnSegmentDraw;
			m_LeaveIcon = segmentFreeDrawIcon[0];
		}
		else if (target == m_btnSegmentPoly && !m_btnSegmentPoly->isChecked())
		{
			m_btnSegmentPoly->setIcon(segmentPolyIcon[res]);
			m_HoverWidget = m_btnSegmentPoly;
			m_LeaveIcon = segmentPolyIcon[0];
		}
		else if (target == m_btnFullScreen && !m_fullscreen)
		{
			m_btnFullScreen->setIcon(fullIcon[res]);
			m_HoverWidget = m_btnFullScreen;
			m_LeaveIcon = fullIcon[0];
		}
	}
}

void AnalMPRPlaneView::updateMouseCursor_By_EventType(QEvent::Type type)
{
	if (type == QEvent::HoverEnter)
	{
		unsetCursor();
	}
	else if (type == QEvent::HoverLeave ||
		type == QEvent::Leave || type == QEvent::Show)
	{
		setCursor(m_cursor);
	}
}

void AnalMPRPlaneView::updateMouseCursor(ANAL_WORK_MODE mode, bool captureMode)
{
	/* TODO : 추후 커서 Icon 업데이트 될 경우 여기에서 Cursor 정보를 업데이트 한다.*/
	m_cursor = Qt::ArrowCursor;

	switch (mode)
	{
	case ANAL_WORK_PATH_3D_PLAY:
		break;
	case ANAL_WORK_PATH:
		m_cursor = Qt::CrossCursor;
		break;

	case ANAL_WORK_SEGMENT_PICKER:
		break;

	case ANAL_WORK_SEGMENT_RECT:
	case ANAL_WORK_SKETCHDRAWSEGMENTATION:
	case ANAL_WORK_SEGMENT_OVAL:
		m_cursor = getBitmapCursor();
		break;

	case ANAL_WORK_CAPTURE:
	{
		if (captureMode)
			m_cursor = Qt::PointingHandCursor;
		else
			m_cursor = Qt::CrossCursor;
	}
	break;
	case ANAL_WORK_SEGMENT_POLY:
		break;
	case ANAL_WORK_SEGMENT_FREEDRAW:
		break;
	case ANAL_WORK_ANNO_PROFILE_LINE:
		break;
	case ANAL_WORK_ANNO_ARROW:
		break;
	case ANAL_WORK_ANNO_RECTANGLE:
		break;
	case ANAL_WORK_ANNO_ANGLE:
		break;
	case ANAL_WORK_ANNO_LENGTH:
		break;
	case ANAL_WORK_ANNO_TEXT:
		break;
	case ANAL_WORK_NONE:
	default:
		break;
	}

	setCursor(m_cursor);
}

void AnalMPRPlaneView::clearSelectedAnnotation(ANAL_WORK_MODE mode, bool* pVolumeUpdate)
{
	if (mode != ANAL_WORK_NONE)
	{
		for (int i = 0; i < m_pWindowManager->anotationList.size(); i++)
		{
			Annotation* ano = m_pWindowManager->anotationList.at(i);

			if (ano->isSelected())
			{
				ano->clearSelect();
				*pVolumeUpdate = true;
				break;
			}
		}
	}
}

void AnalMPRPlaneView::updateVolumeUpdateStatus(ANAL_WORK_MODE mode, ANAL_WORK_MODE prevMode, bool* pVolumeUpdate)
{
	if (mode != prevMode)
	{
		switch (prevMode)
		{
		case ANAL_WORK_PATH:
		case ANAL_WORK_3D_CURVE_SPLIT:
			*pVolumeUpdate = true;
			break;
		default:
			break;
		}
	}

	switch (mode)
	{
	case ANAL_WORK_PATH_3D_PLAY:
		*pVolumeUpdate = true;
		break;

	default:
		break;
	}
}

bool AnalMPRPlaneView::isMousePointInBoundaryBox()
{
	mip::VECTOR3 point;
	if (getCurrentCameraToPlaneIntersectedPoint(&point) == false)
	{
		return false;
	}

	mip::AABB box3D = m_pDataContext->volume_data.getBoundingBox3DAABB();
	if (isBoxAABB_ContainPoint(box3D, point) == false)
	{
		return false;
	}
	return true;
}


/**
	충돌 Box 영역 체크
*/
bool AnalMPRPlaneView::isBoxAABB_ContainPoint(mip::AABB box3D, mip::VECTOR3 point)
{
	if (box3D.checkPoint(point) == false)
	{
		return false;
	}

	if (box3D.min.x > point.x)
	{
		return false;
	}
	else if (box3D.max.x < point.x)
	{
		return false;
	}

	if (box3D.min.y > point.y)
	{
		return false;
	}
	else if (box3D.max.y < point.y)
	{
		return false;
	}

	if (box3D.min.z > point.z)
	{
		return false;
	}

	else if (box3D.max.z < point.z)
	{
		return false;
	}

	return true;
}

bool AnalMPRPlaneView::isRenderable()
{
	return m_pDataContext->volume_data.isValidate()
		&& m_pWindowManager->getRenderable()
		&& (m_pWindowManager->IsCropOn() == false);
}

void AnalMPRPlaneView::drawUnloadedImage(QPainter* p)
{
	QColor background = RESOURCE_MANAGER->getUnloadBackGroundColor();

	if (m_labelUnloadImage == NULL)
	{
		m_labelUnloadImage = new QLabel(this);

		QImage* unloadImage = RESOURCE_MANAGER->getUnloadImage(m_windowType);

		if (unloadImage != NULL)
		{
			m_labelUnloadImage->setPixmap(QPixmap::fromImage(*unloadImage));
			m_size = QSize(unloadImage->width(), unloadImage->height());
		}
	}

	QSize size = (this->size() / 2) - (m_size / 2);
	m_labelUnloadImage->setGeometry(QRect(size.width(), size.height(), m_size.width(), m_size.height()));
	m_labelUnloadImage->show();

	p->fillRect(0, 0, width(), height(), background);
}


void AnalMPRPlaneView::moveDepth(int direction)
{
	bool res = false;
	if (m_pWindowManager->getWheelZoom())
	{
		if (!m_pDataContext->volume_data.checkRotatePlanes(m_windowType))
		{
			res = m_pDataContext->volume_data.getAllThickness(m_thickness, direction, m_windowType, true, true);
		}

		if (!res)
		{
			int depth = m_slidebar->value();
			if (m_slidebar)
			{
				m_slidebar->setValue(depth + direction);
			}
		}
	}
	else
	{
		if (!m_pDataContext->volume_data.checkRotatePlanes(m_windowType))
		{
			res = m_pDataContext->volume_data.getAllThickness(m_thickness, direction, m_windowType);
		}

		if (!res)
		{
			processSlideZ(direction);
			updatePlaneDatas();
		}
	}
}

void AnalMPRPlaneView::checkDepth()
{
	int currentPosition = -1;
	if (m_slidebar)
	{
		currentPosition = m_slidebar->value();
	}

	muint32 cz = m_pDataContext->volume_data.getCZ();

	int currentDepth = m_pDataContext->volume_data.getDepth(m_windowType);
	//int prevDepth = m_pDataContext->volume_data.getPrevDepth(m_windowType);

	if ((currentDepth == currentPosition) || !ACTION_MANAGER->isActionFinished())
	{
		return;
	}

	if (currentDepth != currentPosition)
	{
		if (m_windowType == WT_AXIAL)
		{
			m_slidebar->setValue(cz - currentDepth - 1);
		}
		else
		{
			m_slidebar->setValue(currentDepth);
		}
	}
}

void AnalMPRPlaneView::calcMesh2DOutline()
{
	Visualize2DTab* pVisualize2D = m_pWindowManager->GetTab()->get2DTab();
	if (!pVisualize2D || !pVisualize2D->isOutlineMode() || !m_bUpdateMeshOutline || !ACTION_MANAGER->isActionFinished())
	{
		return;
	}

	float spaceX = m_pDataContext->volume_data.getSpaceX();
	float spaceY = m_pDataContext->volume_data.getSpaceY();
	float spaceZ = m_pDataContext->volume_data.getSpaceZ();
	float halfSpaceX = spaceX * 0.5f;
	float halfSpaceY = spaceY * 0.5f;
	float halfSpaceZ = spaceZ * 0.5f;

	std::vector<mip::VECTOR3>	tempList;
	m_pDataContext->volume_data.getMPRPPlanes(m_windowType, tempList);

	switch (m_windowType)
	{
	case WT_CORONAL:
	{
		tempList[0].y += halfSpaceY * 0.5f;
		tempList[1].y += halfSpaceY * 0.5f;
		tempList[2].y += halfSpaceY * 0.5f;
	}
	break;
	case WT_SAGITTAL:
	{
		tempList[0].x += halfSpaceX * 0.5f;
		tempList[1].x += halfSpaceX * 0.5f;
		tempList[2].x += halfSpaceX * 0.5f;
	}
	break;
	case WT_AXIAL:
	default:
	{
		tempList[0].z += halfSpaceZ * 0.5f;
		tempList[1].z += halfSpaceZ * 0.5f;
		tempList[2].z += halfSpaceZ * 0.5f;
	}
	break;
	}

	mip::PLANE plane;
	plane.fromTri(tempList[1], tempList[2], tempList[0]);

	mip::VECTOR3 posCamera = m_sliceCamera.getPos() * 0.1f;

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

	m_bUpdateMeshOutline = false;
}

void AnalMPRPlaneView::slot_OnContextAnnoText()
{
	m_pWindowManager->bShowAnnoText = !m_pWindowManager->bShowAnnoText;

	m_pWindowManager->renderLater_AnalView();
}

void AnalMPRPlaneView::slot_OnContextAnnoLength()
{
	m_pWindowManager->bShowAnnoLength = !m_pWindowManager->bShowAnnoLength;

	m_pWindowManager->renderLater_AnalView();
}

void AnalMPRPlaneView::slot_OnContextAnnoAngle()
{
	m_pWindowManager->bShowAnnoAngle = !m_pWindowManager->bShowAnnoAngle;

	m_pWindowManager->renderLater_AnalView();
}


void AnalMPRPlaneView::slot_OnContextAnnoArrow()
{
	m_pWindowManager->bShowAnnoArrow = !m_pWindowManager->bShowAnnoArrow;

	m_pWindowManager->renderLater_AnalView();
}

void AnalMPRPlaneView::slot_OnContextAnnoRectangle()
{
	m_pWindowManager->bShowAnnoRectangle = !m_pWindowManager->bShowAnnoRectangle;

	m_pWindowManager->renderLater_AnalView();
}

void AnalMPRPlaneView::slot_OnContextAnnoList()
{
	mip::VECTOR3 vecWorld;
	QVector3D vecVolume;

	if (getCurrentCameraToPlaneIntersectedPoint(&vecWorld) == false)
	{
		return;
	}

	mip::AABB box3D = m_pDataContext->volume_data.getBoundingBox3DAABB();
	if (isBoxAABB_ContainPoint(box3D, vecWorld) == false)
	{
		return;
	}

	mip::VECTOR3 vecVolumeTemp = getWorldToVolume(vecWorld.x, vecWorld.y, vecWorld.z);
	vecVolume.setX(vecVolumeTemp.x);
	vecVolume.setY(vecVolumeTemp.y);
	vecVolume.setZ(vecVolumeTemp.z);

	AnnoControlDlg Condlg(&vecVolume, this, AT_ALL);
	Condlg.exec();

	if (Condlg.isAdd() == false)
	{
		return;
	}

	if (Condlg.getAddType() == AT_LEN)
	{
		m_pWindowManager->mainAnalWidget->setWorkMode(ANAL_WORK_ANNO_LENGTH);
		m_pWindowManager->renderLater_All();
	}
	else if (Condlg.getAddType() == AT_ANGLE)
	{
		m_pWindowManager->mainAnalWidget->setWorkMode(ANAL_WORK_ANNO_ANGLE);
		m_pWindowManager->renderLater_All();
	}
	else if (Condlg.getAddType() == AT_ARROW)
	{
		m_pWindowManager->mainAnalWidget->setWorkMode(ANAL_WORK_ANNO_ARROW);
		m_pWindowManager->renderLater_All();
	}
	else if (Condlg.getAddType() == AT_RECTANGLE)
	{
		m_pWindowManager->mainAnalWidget->setWorkMode(ANAL_WORK_ANNO_RECTANGLE);
		m_pWindowManager->renderLater_All();
	}
	else if (Condlg.getAddType() == AT_TEXT)
	{
		AnnoTextDlg dlg(this);
		if ((dlg.exec() == QDialog::Accepted) && (dlg.Text.isEmpty() == false))
		{
			AnnoString* pAnnoString = new AnnoString(
				vecWorld,
				Annotation::DRAWING_FINISHED,
				dlg.Text,
				dlg.FontSize,
				toCOLOR(dlg.Color)
			);

			ACTION_MANAGER->action_Annotation_Text_Add(pAnnoString);
			m_pWindowManager->renderLater_All();
		}
	}
}

void AnalMPRPlaneView::slot_OnDepthSlideChanged(int value)
{
	if (m_pWindowManager->getWheelZoom())
	{
		float dt;
		bool isRotate = m_pDataContext->volume_data.checkRotatePlanes(m_windowType);
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
		updatePlaneDatas();

		m_bUpdateMeshOutline = true;
	}
	else
	{
		m_sliceCamera.setZoom((m_Winsize * 3.0f) * ((100 - value) / 100.0f));
		if (m_cursor.shape() == Qt::BitmapCursor)
		{
			m_cursor = getBitmapCursor();
			this->setCursor(m_cursor);
		}
	}

	m_pWindowManager->renderLater_3DView();

	renderLater();
}

void AnalMPRPlaneView::slot_OnComboThicknessChanged(int value)
{
	if (!m_pWindowManager->IsLicensePass())
	{
		m_comboThickness->blockSignals(true);
		m_comboThickness->setCurrentIndex(0);
		m_comboThickness->blockSignals(false);
		QMessageBox::warning(this, tr("Not Supported"), tr("Supported features in current version.\n(Edu version or later is supported.)"));
		return;
	}

	if (!m_btnThickness->isHidden())
		m_btnThickness->hide();

	switch ((WINDOW2D_THICKNESS)value)
	{
	case W2PTK_00MM: m_thickness = 0.0f; break;
	case W2PTK_05MM: m_thickness = 0.5f; break;
	case W2PTK_10MM: m_thickness = 1.0f; break;
	case W2PTK_15MM: m_thickness = 1.5f; break;
	case W2PTK_20MM: m_thickness = 2.0f; break;
	case W2PTK_CUSTOM:
	{
		if (m_userThick != 0.0f)
		{
			m_thickness = m_userThick;

			m_comboThickness->setItemText(W2PTK_CUSTOM, QString("%1mm").arg(m_userThick * 10));
			if (m_btnThickness->isHidden())
				m_btnThickness->show();
		}
		else
		{
			ThickDialog dlg(m_userThick, this);
			if (QDialog::Accepted == dlg.exec())
			{
				m_thickness = dlg.getThickness();

				value = m_thickness / 0.5f;

				if ((value * 0.5f == m_thickness) && value < W2PTK_CUSTOM)
				{
					m_comboThickness->blockSignals(true);
					m_comboThickness->setCurrentIndex(value);
					m_comboThickness->blockSignals(false);
					m_comboThickness->setItemText(W2PTK_CUSTOM, QString("User-defined"));
				}
				else
				{
					m_userThick = m_thickness;
					m_comboThickness->setItemText(W2PTK_CUSTOM, QString("%1mm").arg(m_userThick * 10));
					if (m_btnThickness->isHidden())
						m_btnThickness->show();
				}
			}
			else
			{
				if (m_userThick == 0)
					m_comboThickness->setCurrentIndex(0);
			}
		}
	}
	break;
	}

	m_pWindowManager->renderLater_AnalView(false);
}

#ifdef _DEBUG
void AnalMPRPlaneView::slot_OnComboFilterChanged(int value)
{
	m_pWindowManager->setWindow2D_PostProcess_Filter((WINDOW2D_POSTPROCESS_FILTER)value);

	renderLater();
}
#endif


void AnalMPRPlaneView::slot_OnAnnoLength()
{
	if (m_btnAnnoLength->isChecked())
		setWorkMode(ANAL_WORK_ANNO_LENGTH, true);
	else
		setWorkMode(ANAL_WORK_NONE, true);

}

void AnalMPRPlaneView::slot_OnAnnoText()
{
	if (m_btnAnnoText->isChecked())
		setWorkMode(ANAL_WORK_ANNO_TEXT, true);
	else
		setWorkMode(ANAL_WORK_NONE, true);
}

void AnalMPRPlaneView::slot_OnAnnoAngle()
{
	if (m_btnAnnoAngle->isChecked())
		setWorkMode(ANAL_WORK_ANNO_ANGLE, true);
	else
		setWorkMode(ANAL_WORK_NONE, true);
}

void AnalMPRPlaneView::slot_OnInterOval()
{
	if (m_btnSegmentOval->isChecked())
		setWorkMode(ANAL_WORK_SEGMENT_OVAL, true);
	else
		setWorkMode(ANAL_WORK_NONE, true);
}

void AnalMPRPlaneView::slot_OnAnnoArrow()
{
	if (m_btnAnnoArrow->isChecked())
		setWorkMode(ANAL_WORK_ANNO_ARROW, true);
	else
		setWorkMode(ANAL_WORK_NONE, true);
}

void AnalMPRPlaneView::slot_OnAnnoRect()
{
	if (m_btnAnnoRect->isChecked())
		setWorkMode(ANAL_WORK_ANNO_RECTANGLE, true);
	else
		setWorkMode(ANAL_WORK_NONE, true);
}

void AnalMPRPlaneView::slot_OnShowAniPoint()
{
	m_pWindowManager->bShowAniPoint = !m_pWindowManager->bShowAniPoint;

	m_pWindowManager->renderLater_GridView(false);
}

void AnalMPRPlaneView::slot_OnInterPoly()
{
	if (m_btnSegmentPoly->isChecked())
		setWorkMode(ANAL_WORK_SEGMENT_POLY, true);
	else
		setWorkMode(ANAL_WORK_NONE, true);

}

mip::VECTOR3 AnalMPRPlaneView::getLocalToVolume(float x, float y, float z)
{
	mip::VECTOR3 basisVec;
	mip::VECTOR3 LocalVec;
	mip::VECTOR3 volumeVec;

	basisVec.set((-((float)m_pDataContext->volume_data.getCX() / 2)) * m_pDataContext->volume_data.getSpaceX(),
		(-((float)m_pDataContext->volume_data.getCY() / 2)) * m_pDataContext->volume_data.getSpaceY(),
		(-((float)m_pDataContext->volume_data.getCZ() / 2)) * m_pDataContext->volume_data.getSpaceZ());

	LocalVec.set(x, y, z);

	volumeVec.set((LocalVec - basisVec));

	volumeVec.x /= m_pDataContext->volume_data.getSpaceX();
	volumeVec.y /= m_pDataContext->volume_data.getSpaceY();
	volumeVec.z /= m_pDataContext->volume_data.getSpaceZ();

	return volumeVec;
}

QCursor AnalMPRPlaneView::getBitmapCursor()
{
	if (m_list.size() == 0)
		createCursorShape();

	bool bType = (m_pWindowManager->getAnalWorkMode() == ANAL_WORK_SEGMENT_RECT || 2 == m_pWindowManager->getDCutShape(true));
	bool res = bType ? m_pWindowManager->pencil_A : m_pWindowManager->oval_A;

	QCursor cur;
	if (res)
		cur.setShape(Qt::ArrowCursor);
	else
	{
		QRect rect;
		cur.setShape(Qt::BitmapCursor);
		auto coord1 = m_list.begin();
		auto coord2 = m_list.last();
		float factor;

		factor = m_sliceCamera.getZoomMax() - m_sliceCamera.getZoom(); /*0 ~ zoommax*/
		factor /= m_sliceCamera.getZoomMax();
		factor += 0.1f;

		rect.setCoords(m_MousePos.x() + (coord1->x() * factor), m_MousePos.y() + (coord1->y() * factor),
			m_MousePos.x() + (coord2.x() * factor), m_MousePos.y() + (coord2.y() * factor));

		QPixmap pix(rect.width() + 2, rect.height() + 2);
		pix.fill(Qt::transparent);
		QPainter p(&pix);
		//p.setPen(QPen(QColor(Qt::black)));
		//p.setBrush(QBrush(Qt::white));

		QPen pen;
		pen.setColor(QColor(Qt::black));
		pen.setWidth(m_pWindowManager->getBrushThickness());
		p.setPen(pen);
		p.setBrush(QBrush(QColor(255, 255, 255, m_pWindowManager->getBrushOpacity())));

		if (bType)
			p.drawRect(0, 0, rect.width() + 1, rect.height() + 1);
		else
			p.drawEllipse(0, 0, rect.width() + 1, rect.height() + 1);

		p.end();

		cur = (pix);
	}
	return cur;
}

void AnalMPRPlaneView::preDepth()
{
	int direction = -1;
	if (m_windowType == WT_AXIAL || m_windowType == WT_SAGITTAL)
	{
		direction = -1;
	}
	else if (m_windowType == WT_CORONAL)
	{
		direction = 1;
	}

	moveDepth(direction);
}

void AnalMPRPlaneView::nextDepth()
{
	int direction = -1;
	if (m_windowType == WT_AXIAL || m_windowType == WT_SAGITTAL)
	{
		direction = 1;
	}
	else if (m_windowType == WT_CORONAL)
	{
		direction = -1;
	}

	moveDepth(direction);
}

void AnalMPRPlaneView::updatePlaneDatas()
{
	SHADER_QUALITY shader_quality = m_pWindowManager->getShaderQuality();

	if (g_Renderer->isAvailableVolumeRender() == false ||
		shader_quality == SQ_INVISIBLE)
	{
		int n = 0;
		std::vector<mip::VECTOR3> tempList;
		m_pDataContext->volume_data.getMPRPPlanes(m_windowType, tempList);

		if (tempList.size() < 4) return;

		float xSpace, ySpace;
		uint xMax, yMax;
		mip::VECTOR3 xDir = (tempList[3] - tempList[0]).normalize();
		mip::VECTOR3 yDir = (tempList[1] - tempList[0]).normalize();
		mint16* planeData = 0;
		mip::VECTOR3 start;
		mip::VECTOR3 point;

		switch (m_windowType)
		{
		case WT_AXIAL:
			yDir = -yDir;
			xSpace = m_pDataContext->volume_data.getSpaceX();
			ySpace = m_pDataContext->volume_data.getSpaceY();
			xMax = m_pDataContext->volume_data.getCX();
			yMax = m_pDataContext->volume_data.getCY();
			planeData = m_pDataContext->volume_data.axialPlaneData[1];
			start = tempList[1] + (xDir * 0.5f * xSpace) + (yDir * 0.5f * ySpace);
			break;
		case WT_CORONAL:
			xSpace = m_pDataContext->volume_data.getSpaceX();
			ySpace = m_pDataContext->volume_data.getSpaceZ();
			xMax = m_pDataContext->volume_data.getCX();
			yMax = m_pDataContext->volume_data.getCZ();
			planeData = m_pDataContext->volume_data.coronalPlaneData[1];
			start = tempList[0] + (xDir * 0.5f * xSpace) + (yDir * 0.5f * ySpace);
			break;
		case WT_SAGITTAL:
			xSpace = m_pDataContext->volume_data.getSpaceY();
			ySpace = m_pDataContext->volume_data.getSpaceZ();
			xMax = m_pDataContext->volume_data.getCY();
			yMax = m_pDataContext->volume_data.getCZ();
			planeData = m_pDataContext->volume_data.sagittalPlaneData[1];
			start = tempList[0] + (xDir * 0.5f * xSpace) + (yDir * 0.5f * ySpace);
			break;
		default:
			return;
		}

		mip::AABB box3D = m_pDataContext->volume_data.getBoundingBox3DAABB();

		unsigned char _m = m_pWindowManager->getSelectedMask();
		int _m_index = m_pWindowManager->getSelectedMaskByteIndex();

		for (int y = 0; y < yMax; y++)
		{
			point = start;
			for (int x = 0; x < xMax; x++)
			{
				if (box3D.checkPoint(point) == true)
				{
					mip::VECTOR3 value = getWorldToVolume(point.x, point.y, point.z);
					//m_pDataContext->volume_data.setMaskBit(value.x, value.y, value.z, _m, _m_index);
					planeData[y * xMax + x] = m_pDataContext->volume_data.getData(value.x, value.y, value.z);
				}
				else
					planeData[y * xMax + x] = MIN_int16;

				point += xDir * xSpace;
			}
			start += yDir * ySpace;
		}

		m_pDataContext->volume_data.forceUpdateMPR();
	}
}

void AnalMPRPlaneView::updatePlaneDepth()
{
	if (m_slidebar)
	{
		int value = m_pDataContext->volume_data.getDepth(m_windowType);
		if (WT_AXIAL == m_windowType)
		{
			value = m_pDataContext->volume_data.getCZ() - value - 1;
		}

		m_slidebar->setValue(value);
	}
}

void AnalMPRPlaneView::syncFullscreen()
{
	slot_OnFullScreen();
}

QImage AnalMPRPlaneView::getCaptureImage(bool mode, bool type)
{
	QImage img = QImage(this->width(), this->height(), QImage::Format_RGBA8888);

	QPainter p(&img);
	p.fillRect(QRect(0, 0, this->width(), this->height()), Qt::black);
	p.drawImage(0, 0, m_img);

	if (type)
	{
		drawPlaneLine(&p);
		drawAnnotations(&p);
		drawRuler(&p);

		QFontMetrics fontMet(p.font());
		QString str = QString("Thickness(%1)").arg(m_comboThickness->currentText());
		QRect rect = fontMet.boundingRect(str);
		p.setPen(QPen(QColor(255, 255, 255)));
		p.drawText(QPoint(0, rect.height()), str);

		drawCoordHU(&p, rect.height());
	}

	if (mode)
	{
		/*if (!m_pWindowManager->IsLicensePass())
		{
			QImage * WaterImage = RESOURCE_MANAGER->getWaterMark();
			if (WaterImage != NULL)
			{
				QImage drawImg = WaterImage->scaled(QSize(this->width(), this->height() / 3), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
				p.drawImage(QPoint(0, this->height() / 4), drawImg);
			}
		}*/

		p.end();
		return img;
	}
	else //rectangle
	{
		p.end();
		int w, h;

		w = abs(m_polyLine.at(0).x() - m_polyLine.at(1).x());
		w++;

		h = abs(m_polyLine.at(0).y() - m_polyLine.at(1).y());
		h++;

		if ((1 >= w) && (1 >= h))
			return QImage();

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

		/*if (!m_pWindowManager->IsLicensePass())
		{
			QImage * WaterImage = RESOURCE_MANAGER->getWaterMark();
			if (WaterImage != NULL)
			{
				QImage drawImg = WaterImage->scaled(QSize(w, h / 3), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
				p2.drawImage(QPoint(0, h / 4), drawImg);
			}
		}*/

		p2.end();

		return rectImg;
	}

	return QImage();
}

void AnalMPRPlaneView::createCursorShape(bool _auto) //5 ~ 50
{
	QPolygon poly;
	int sizeRect = (m_pWindowManager->getAnalWorkMode() == ANAL_WORK_SEGMENT_RECT || 2 == m_pWindowManager->getDCutShape(true)) ?
		m_pWindowManager->AngularSize_A : m_pWindowManager->CirculSize_A;
	//	float rSize = this->width() / sizeRect;
// 	float rSize = this->width();
// 	rSize /= 10 + (30 - sizeRect);
// 	rSize /= 2.0f;
// 	poly.reserve(((rSize + 2)) * (rSize + 2));

	float rSize = sizeRect;// / 2;

	poly.reserve((sizeRect + 2) * (sizeRect + 2));

	//	rSize /= 2.0f;
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
		m_cursor = getBitmapCursor();
		this->setCursor(m_cursor);
	}
}

void AnalMPRPlaneView::slot_OnProfileLine()
{
	if (m_btnProfileLine->isChecked())
		setWorkMode(ANAL_WORK_ANNO_PROFILE_LINE, true);
	else
		setWorkMode(ANAL_WORK_NONE, true);
}

void AnalMPRPlaneView::slot_OnInterDraw()
{
	if (m_btnSegmentDraw->isChecked())
		setWorkMode(ANAL_WORK_SEGMENT_FREEDRAW, true);
	else
		setWorkMode(ANAL_WORK_NONE, true);

}

void AnalMPRPlaneView::slot_OnInterPicker()
{
	if (m_btnSegmentPicker->isChecked())
		setWorkMode(ANAL_WORK_SEGMENT_PICKER, true);
	else
		setWorkMode(ANAL_WORK_NONE, true);

}

void AnalMPRPlaneView::slot_OnInterRect()
{
	if (m_btnSegmentRect->isChecked())
		setWorkMode(ANAL_WORK_SEGMENT_RECT, true);
	else
		setWorkMode(ANAL_WORK_NONE, true);
}

void AnalMPRPlaneView::slot_OnContextProfPlane()
{
#ifdef DEV_VER

	if (!m_pWindowManager->IsLicensePass())
	{
		QMessageBox::warning(this, tr("Not Supported"), tr("Supported features in current version.\n(Edu version or later is supported.)"));
		return;
	}

	updatePlaneDatas();
#endif

}

void AnalMPRPlaneView::slot_OnShowAxises()
{
	static QIcon axisPress = RESOURCE_MANAGER->getIcon(ICON_VIEW_2D_AXISES, m_nIconSize, m_nIconSize);

	if (m_btnShowAxis)
	{
		if (m_btnShowAxis->isChecked())
		{
			m_btnShowAxis->setIcon(axisPress);
			m_pWindowManager->setShowViewAxises(true);
		}
		else
			m_pWindowManager->setShowViewAxises(false);
	}

	m_pWindowManager->renderLater_GridView(false);
}

void AnalMPRPlaneView::slot_OnFullScreen()
{
	if (!m_pDataContext->volume_data.isValidate())
	{
		return;
	}

	if (isWorkMode() == true)
	{
		return;
	}

	m_fullscreen = !m_fullscreen;

	static QIcon prIcon = RESOURCE_MANAGER->getIcon(ICON_VIEW_FULL_SCREEN, m_nIconSize, m_nIconSize);
	static QIcon reIcon = RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_FULL_SCREEN, m_nIconSize, m_nIconSize);

	if (m_fullscreen)
		m_btnFullScreen->setIcon(prIcon);
	else
		m_btnFullScreen->setIcon(reIcon);

	emit setFullScreen(m_fullscreen == true ? this : NULL);
}

void AnalMPRPlaneView::slot_OnThickness()
{
	if (m_btnThickness->isChecked())
		m_btnThickness->setChecked(false);

	ThickDialog dlg(m_userThick, this);
	if (QDialog::Accepted == dlg.exec())
	{
		m_thickness = dlg.getThickness();

		int index = m_thickness / 0.5f;

		if ((index * 0.5f == m_thickness) && index < W2PTK_CUSTOM)
		{
			m_comboThickness->setItemText(W2PTK_CUSTOM, QString("User-defined"));
		}
		else
		{
			index = W2PTK_CUSTOM;
			m_userThick = m_thickness;
			m_comboThickness->setItemText(W2PTK_CUSTOM, QString("%1mm").arg(m_userThick * 10));
			if (m_btnThickness->isHidden())
				m_btnThickness->show();
		}

		m_comboThickness->blockSignals(true);
		m_comboThickness->setCurrentIndex(index);
		m_comboThickness->blockSignals(false);
	}
}

void AnalMPRPlaneView::processMouseDoubleClick(QMouseEvent* e)
{
	if (e->buttons() & Qt::RightButton)
		m_RbuttonDown = true;
	else if (e->buttons() & Qt::LeftButton)
		m_LbuttonDown = true;
	else if (e->buttons() & Qt::MidButton)
		m_MbuttonDown = true;

	switch (m_pWindowManager->getAnalWorkMode())
	{
	case ANAL_WORK_NONE:
	case ANAL_WORK_3D_CURVE_SPLIT:
	case ANAL_WORK_3D_PLANE_SPLIT:
		mouseDblClick_Annotation();
		break;
	default:
		break;
	}
}


void AnalMPRPlaneView::processMousePress(QMouseEvent* e)
{
	m_MousePos = e->pos();
	m_MousePos_Pressed = e->pos();

	m_pWindowManager->setMoveFocus(true);
	this->setFocus();

	/* Mouse 초기 상태 업데이트 */
	if (e->buttons() & Qt::RightButton)
		m_RbuttonDown = true;
	else if (e->buttons() & Qt::LeftButton)
		m_LbuttonDown = true;
	else if (e->buttons() & Qt::MidButton)
		m_MbuttonDown = true;

	m_globalPos = e->globalPos();

	/* 모드별 Mouse Event Handling */
	ANAL_WORK_MODE analWorkMode = m_pWindowManager->getAnalWorkMode();
	switch (analWorkMode)
	{
		/* Capture */
	case ANAL_WORK_CAPTURE:
		mousePress_CaptureArea();
		break;

		/* Segmentation */
	case ANAL_WORK_SEGMENT_POLY:
		mousePress_SegmentPolyLine();
		break;

	case ANAL_WORK_SKETCHDRAWSEGMENTATION:
	case ANAL_WORK_SEGMENT_OVAL:
	case ANAL_WORK_SEGMENT_RECT:
		mousePress_Segmentation_MagicCut_or_SegmentRectOval();
		break;

	case ANAL_WORK_SEGMENT_PICKER:
	case ANAL_WORK_SEGMENT_FREEDRAW:
		mousePress_Segmentation_Pick_or_FreeDraw();
		break;

		/* Annotation */
	case ANAL_WORK_ANNO_LENGTH:
	case ANAL_WORK_ANNO_ARROW:
	case ANAL_WORK_ANNO_ANGLE:
	case ANAL_WORK_ANNO_RECTANGLE:
	case ANAL_WORK_ANNO_TEXT:
	case ANAL_WORK_ANNO_PROFILE_LINE:
		mousePress_Annotation_InDrawingMode(analWorkMode);
		break;

		/* 어떤 모드도 선택되어있지 않을 경우 */
	case ANAL_WORK_NONE:
	case ANAL_WORK_3D_PLANE_SPLIT:
	case ANAL_WORK_3D_CURVE_SPLIT:
		mousePress_InNormalMode();
		break;
	default:
		break;
	}

	renderLater();
}


void AnalMPRPlaneView::processMouseMove()
{
	bool isSelected = false;

	if (ANAL_WORK_NONE == m_pWindowManager->getAnalWorkMode())
	{
		isSelected = isSelectedView();
	}

	if (isSelected)
	{
		return;
	}

	switch (m_pWindowManager->getAnalWorkMode())
	{
	case ANAL_WORK_CAPTURE:
		mouseMove_CaptureArea();
		break;

	case ANAL_WORK_SEGMENT_PICKER:
	case ANAL_WORK_SEGMENT_FREEDRAW:
		mouseMove_Segmentation_Pick_or_FreeDraw();
		break;

	case ANAL_WORK_SEGMENT_OVAL:
	case ANAL_WORK_SKETCHDRAWSEGMENTATION:
	case ANAL_WORK_SEGMENT_RECT:
		mouseMove_Segmentation_MagicCut_or_RectOval();
		break;
	case ANAL_WORK_SEGMENT_POLY:
		mouseMove_SegmentPolyLine();
		break;
	case ANAL_WORK_ANNO_PROFILE_LINE:
	case ANAL_WORK_ANNO_LENGTH:
	case ANAL_WORK_ANNO_ARROW:
	case ANAL_WORK_ANNO_ANGLE:
	case ANAL_WORK_ANNO_RECTANGLE:
	case ANAL_WORK_ANNO_TEXT:
		mouseMove_Annotation_InDrawingMode();
		break;

	case ANAL_WORK_NONE:
	case ANAL_WORK_3D_PLANE_SPLIT:
	case ANAL_WORK_3D_CURVE_SPLIT:
		mouseMove_Annotation_NormalMode();
		break;
	default:
		break;
	}
}

void AnalMPRPlaneView::processMouseRelease()
{
	switch (m_pWindowManager->getAnalWorkMode())
	{
	case ANAL_WORK_PATH:
		mouseRelease_Animation();
		break;

	case ANAL_WORK_CAPTURE:
		mouseRelease_CaptureArea();
		break;

	case ANAL_WORK_SKETCHDRAWSEGMENTATION:
	case ANAL_WORK_SEGMENT_RECT:
	case ANAL_WORK_SEGMENT_OVAL:
		mouseRelease_Segmentation_MagicCut_or_RectOval();
		break;
	case ANAL_WORK_SEGMENT_PICKER:
	case ANAL_WORK_SEGMENT_FREEDRAW:
		mouseRelease_Segmentation_Pick_or_FreeDraw();
		break;

	case ANAL_WORK_NONE:
	case ANAL_WORK_3D_PLANE_SPLIT:
	case ANAL_WORK_3D_CURVE_SPLIT:
		if (m_RbuttonDown)
		{
			/* 현재 호출되지 않음 */
			if (!m_bMoveMouse)
			{
				setContextMenu();
				m_contextMenu->exec(m_globalPos);

				m_pWindowManager->renderLater_All();
			}
		}
		else if (m_LbuttonDown)
		{
			mouseRelease_EditSelectedAnnotation();
		}
		break;
	default:
		break;
	}

	m_RbuttonDown = false;
	m_LbuttonDown = false;
	m_MbuttonDown = false;
	m_bMoveMouse = false;
	m_translationMPR = false;
	m_rotateMPR = false;

	renderLater();
}

void AnalMPRPlaneView::processResize(muint32 width, muint32 height)
{
	m_sliceCamera.setScreenSize(width, height);
}

void AnalMPRPlaneView::focusInEvent(QFocusEvent*)
{
	setWorkMode(m_pWindowManager->getAnalWorkMode());
}

void AnalMPRPlaneView::slide(WINDOW_TYPE type, mip::VECTOR3& dir)
{
	mip::VECTOR3* planes = NULL;
	switch (type)
	{
	case WT_AXIAL:
		m_pWindowManager->analSlideOffsetSub[0] += dir;
		m_pWindowManager->analSlideOffset[0] += dir;
		planes = &m_pDataContext->volume_data.axialPPlane[0];
		break;
	case WT_CORONAL:
		m_pWindowManager->analSlideOffsetSub[1] += dir;
		m_pWindowManager->analSlideOffset[1] += dir;
		planes = &m_pDataContext->volume_data.coronalPPlane[0];
		break;
	case WT_SAGITTAL:
		m_pWindowManager->analSlideOffsetSub[2] += dir;
		m_pWindowManager->analSlideOffset[2] += dir;
		planes = &m_pDataContext->volume_data.saggitalPPlane[0];
		break;
	default:
		break;
	}

	if (planes == NULL)
		return;

	for (int n = 0; n < 4; ++n)
	{
		planes[n] += dir;
	}
}

bool AnalMPRPlaneView::processRotate()
{
	if (m_translationMPR)
	{
		mip::VECTOR3 v1 = m_sliceCamera.getWorldPoint(0.5f);
		mip::VECTOR3 v2 = m_sliceCamera.getPreWorldPoint(0.5f);

		mip::VECTOR3 v = v1 - v2;
		v *= 0.25f;

		for (int n = 0; n < 4; n++)
		{
			switch (m_windowType)
			{
			case WT_CORONAL:
				slide(WT_AXIAL, v);
				slide(WT_SAGITTAL, v);
				if (m_pWindowManager->mainAnalWidget)
				{
					m_pWindowManager->mainAnalWidget->updatePlaneData(WT_AXIAL);
					m_pWindowManager->mainAnalWidget->updatePlaneData(WT_SAGITTAL);
				}
				break;
			case WT_SAGITTAL:
				slide(WT_AXIAL, v);
				slide(WT_CORONAL, v);
				if (m_pWindowManager->mainAnalWidget)
				{
					m_pWindowManager->mainAnalWidget->updatePlaneData(WT_AXIAL);
					m_pWindowManager->mainAnalWidget->updatePlaneData(WT_CORONAL);
				}
				break;
			default:
			case WT_AXIAL:
				slide(WT_SAGITTAL, v);
				slide(WT_CORONAL, v);
				if (m_pWindowManager->mainAnalWidget)
				{
					m_pWindowManager->mainAnalWidget->updatePlaneData(WT_CORONAL);
					m_pWindowManager->mainAnalWidget->updatePlaneData(WT_SAGITTAL);
				}
				break;
			}
		}

		if (m_pWindowManager->mainAnalWidget)m_pWindowManager->mainAnalWidget->updateMeshOutline();

		m_pWindowManager->renderLater_All();

		return true;
	}
	else if (m_rotateMPR)
	{
		mip::VECTOR3 s = mip::VECTOR3(m_preMousePos.x(), m_preMousePos.y(), 0);
		mip::VECTOR3 e = mip::VECTOR3(m_MousePos.x(), m_MousePos.y(), 0);

		mip::MATRIX44 wvp = m_sliceCamera.getView() * m_sliceCamera.getProj();
		mip::VECTOR3 center(m_planeCenter.x, m_planeCenter.y, 0);

		mip::VECTOR3 vs = s - center;
		mip::VECTOR3 ve = e - center;

		mip::MATRIX44 matView = m_sliceCamera.getView();
		mip::MATRIX44 matProj = m_sliceCamera.getProj();
		mip::MATRIX44 matWVP = matView * matProj;
		matWVP.inverse();

		mip::MATRIX44 matInvCamera;
		matInvCamera = matView;
		matInvCamera.inverse();
		float ang = ::atan2(ve.y, ve.x) - ::atan2(vs.y, vs.x);

		mip::QUATERNION	q;
		mip::VECTOR3 Z = -matInvCamera.getScaledZaxis().normalize();

		q.setRotationAxis(Z, ang);

		mip::VECTOR3 pivot = mip::geom::Screen2World(m_planeCenter.x, m_planeCenter.y, this->width(), this->height(), matView, matProj);
		mip::MATRIX44 matRot = mip::math::MatrixAffineTransformation(1.0f, pivot, q, mip::VECTOR3(0, 0, 0));

		for (int n = 0; n < 4; n++)
		{
			switch (m_windowType)
			{
			case WT_CORONAL:
				m_pDataContext->volume_data.axialPPlane[n].transform(matRot);
				m_pDataContext->volume_data.saggitalPPlane[n].transform(matRot);

				if (m_pWindowManager->mainAnalWidget)
				{
					m_pWindowManager->mainAnalWidget->updatePlaneData(WT_AXIAL);
					m_pWindowManager->mainAnalWidget->updatePlaneData(WT_SAGITTAL);
				}

				m_pWindowManager->setSliderMid(WT_AXIAL);
				m_pWindowManager->setSliderMid(WT_SAGITTAL);
				break;
			case WT_SAGITTAL:
				m_pDataContext->volume_data.axialPPlane[n].transform(matRot);
				m_pDataContext->volume_data.coronalPPlane[n].transform(matRot);
				if (m_pWindowManager->mainAnalWidget)
				{
					m_pWindowManager->mainAnalWidget->updatePlaneData(WT_AXIAL);
					m_pWindowManager->mainAnalWidget->updatePlaneData(WT_CORONAL);
				}

				m_pWindowManager->setSliderMid(WT_AXIAL);
				m_pWindowManager->setSliderMid(WT_CORONAL);
				break;
			default:
			case WT_AXIAL:
				m_pDataContext->volume_data.saggitalPPlane[n].transform(matRot);
				m_pDataContext->volume_data.coronalPPlane[n].transform(matRot);
				if (m_pWindowManager->mainAnalWidget)
				{
					m_pWindowManager->mainAnalWidget->updatePlaneData(WT_CORONAL);
					m_pWindowManager->mainAnalWidget->updatePlaneData(WT_SAGITTAL);
				}

				m_pWindowManager->setSliderMid(WT_SAGITTAL);
				m_pWindowManager->setSliderMid(WT_CORONAL);
				break;
			}
		}

		switch (m_windowType)
		{
		case WT_CORONAL:
			m_pWindowManager->analSlideOffsetSub[0].transform(matRot);
			m_pWindowManager->analSlideOffsetSub[2].transform(matRot);
			break;
		case WT_SAGITTAL:
			m_pWindowManager->analSlideOffsetSub[0].transform(matRot);
			m_pWindowManager->analSlideOffsetSub[1].transform(matRot);
			break;
		default:
		case WT_AXIAL:
			m_pWindowManager->analSlideOffsetSub[1].transform(matRot);
			m_pWindowManager->analSlideOffsetSub[2].transform(matRot);
			break;
		}

		if (m_pWindowManager->mainAnalWidget)m_pWindowManager->mainAnalWidget->updateMeshOutline();

		m_pWindowManager->renderLater_All();

		return true;
	}

	return false;
}

bool AnalMPRPlaneView::isSelectedView()
{
	std::vector<mip::VECTOR2> lineList;
	std::vector<mip::VECTOR3> tempList;
	mip::MATRIX44 matView = m_sliceCamera.getView();
	mip::MATRIX44 matProj = m_sliceCamera.getProj();
	float closeLenth = 0.0f;

	bool preAxialSeleted = m_AxialSeleted;
	bool preCoronalSeleted = m_CoronalSeleted;
	bool preSaggitalSeleted = m_SaggitalSeleted;
	bool isSelected = false;
	m_AxialSeleted = false;
	m_CoronalSeleted = false;
	m_SaggitalSeleted = false;
	m_bMoveMouse = true;

	if (!m_pWindowManager->getShowViewAxises())
	{

		if (preAxialSeleted != m_AxialSeleted ||
			preCoronalSeleted != m_CoronalSeleted ||
			preSaggitalSeleted != m_SaggitalSeleted)
		{
			renderLater();
		}
		return false;
	}

	switch (m_windowType)
	{
	case WT_CORONAL:
		m_pDataContext->volume_data.getMPRPPlanes(WT_AXIAL, tempList);
		break;
	case WT_SAGITTAL:
		m_pDataContext->volume_data.getMPRPPlanes(WT_CORONAL, tempList);
		break;
	case WT_AXIAL:
	default:
		m_pDataContext->volume_data.getMPRPPlanes(WT_CORONAL, tempList);
		break;
	}
	if (tempList.size() > 1)
	{
		for (auto item = tempList.begin(); tempList.end() != item; ++item)
		{
			mip::VECTOR3 v = mip::geom::WorldToScreen(*item, this->width(), this->height(), matView, matProj);
			lineList.push_back(mip::VECTOR2(v.x, v.y));
		}

		if (lineList.size() >= 4)
		{
			float a = mip::VECTOR2(lineList[0] - lineList[2]).length();
			if (a < mip::VECTOR2(lineList[1] - lineList[3]).length())
			{
				closeLenth = mip::geom::GetClosestDist(lineList[1], lineList[3], mip::VECTOR2(m_MousePos.x(), m_MousePos.y()));
			}
			else
			{
				closeLenth = mip::geom::GetClosestDist(lineList[0], lineList[2], mip::VECTOR2(m_MousePos.x(), m_MousePos.y()));
			}

			if (closeLenth <= 5.0f)
			{
				switch (m_windowType)
				{
				case WT_CORONAL:
					isSelected = m_AxialSeleted = true;
					break;
				case WT_SAGITTAL:
					isSelected = m_CoronalSeleted = true;
					break;
				case WT_AXIAL:
				default:
					isSelected = m_CoronalSeleted = true;
					break;
				}
			}
		}
	}

	lineList.clear();
	tempList.clear();
	switch (m_windowType)
	{
	case WT_CORONAL:
		m_pDataContext->volume_data.getMPRPPlanes(WT_SAGITTAL, tempList);
		break;
	case WT_SAGITTAL:
		m_pDataContext->volume_data.getMPRPPlanes(WT_AXIAL, tempList);
		break;
	case WT_AXIAL:
	default:
		m_pDataContext->volume_data.getMPRPPlanes(WT_SAGITTAL, tempList);
		break;
	}
	if (tempList.size() > 1)
	{
		for (auto item = tempList.begin(); tempList.end() != item; ++item)
		{
			mip::VECTOR3 v = mip::geom::WorldToScreen(*item, this->width(), this->height(), matView, matProj);
			lineList.push_back(mip::VECTOR2(v.x, v.y));
		}

		if (lineList.size() >= 4)
		{
			float a = mip::VECTOR2(lineList[0] - lineList[2]).length();
			if (a < mip::VECTOR2(lineList[1] - lineList[3]).length())
				closeLenth = mip::geom::GetClosestDist(lineList[1], lineList[3], mip::VECTOR2(m_MousePos.x(), m_MousePos.y()));
			else
				closeLenth = mip::geom::GetClosestDist(lineList[0], lineList[2], mip::VECTOR2(m_MousePos.x(), m_MousePos.y()));

			if (closeLenth <= 5.0f)
			{
				switch (m_windowType)
				{
				case WT_CORONAL:
					isSelected = m_SaggitalSeleted = true;
					break;
				case WT_SAGITTAL:
					isSelected = m_AxialSeleted = true;
					break;
				default:
				case WT_AXIAL:
					isSelected = m_SaggitalSeleted = true;
					break;
				}
			}
		}
	}

	if (preAxialSeleted != m_AxialSeleted ||
		preCoronalSeleted != m_CoronalSeleted ||
		preSaggitalSeleted != m_SaggitalSeleted)
	{
		renderLater();
	}

	return isSelected;
}

void AnalMPRPlaneView::processSlideZ(float dt, bool isSlider, bool isRotate)
{
	if (!isSlider)
		dt = dt > 0 ? 1.0f : -1.0f;

	mip::VECTOR3* planes = NULL;
	mip::VECTOR3 slideOffset;
	switch (m_windowType)
	{
	case WT_AXIAL:
		if (isSlider && (!isRotate))
		{
			++dt;
			dt -= (float)m_pDataContext->volume_data.getCZ() / 2.f;
		}
		dt *= (float)m_pDataContext->volume_data.getSpaceZ();
		planes = &m_pDataContext->volume_data.axialPPlane[0];
		slideOffset = m_pWindowManager->analSlideOffset[0];
		break;
	case WT_CORONAL:
		if (isSlider && (!isRotate))
		{
			++dt;
			dt -= (float)m_pDataContext->volume_data.getCY() / 2.f;
		}
		dt *= (float)m_pDataContext->volume_data.getSpaceY();
		planes = &m_pDataContext->volume_data.coronalPPlane[0];
		slideOffset = m_pWindowManager->analSlideOffset[1];
		break;
	case WT_SAGITTAL:
		if (isSlider && (!isRotate))
			dt -= (float)m_pDataContext->volume_data.getCX() / 2.f;
		dt *= (float)m_pDataContext->volume_data.getSpaceX();
		planes = &m_pDataContext->volume_data.saggitalPPlane[0];
		slideOffset = m_pWindowManager->analSlideOffset[2];
		break;
	default:
		break;
	}

	if (planes == NULL)
	{
		return;
	}

	if (isSlider)
	{
		//m_pDataContext->volume_data.getAnal3DPlanes(m_windowType, planes, !isRotate);
		m_pDataContext->volume_data.getAnal3DPlanes(m_windowType, planes, false);
	}

	mip::VECTOR3 v = ((planes[0] - planes[1]) ^ (planes[2] - planes[1])).normalize();

	v *= dt;

	for (int n = 0; n < 4; ++n)
	{
		planes[n] += v;
	}

	if (isSlider)
	{
		if (slideOffset != slideOffset.Zero)
		{
			for (int i = 0; i < 4; i++)
				planes[i] += slideOffset;
		}
	}

	m_pWindowManager->renderLater_All(true);
}

#define CLAMP(v, min, max) if (v < min) { v = min; } else if (v > max) { v = max; } 

const mip::COLOR PixelClamped(mip::COLOR* data, muint32 width, muint32 height, int x, int y)
{
	CLAMP(x, 0, width - 1);
	CLAMP(y, 0, height - 1);
	return data[y * width + x];
}

// t is a value that goes from 0 to 1 to interpolate in a C1 continuous way across uniformly sampled data points.
// when t is 0, this will return B.  When t is 1, this will return C.  Inbetween values will return an interpolation
// between B and C.  A and B are used to calculate slopes at the edges.
float CubicHermite(float A, float B, float C, float D, float t)
{
	float a = -A / 2.0f + (3.0f * B) / 2.0f - (3.0f * C) / 2.0f + D / 2.0f;
	float b = A - (5.0f * B) / 2.0f + 2.0f * C - D / 2.0f;
	float c = -A / 2.0f + C / 2.0f;
	float d = B;

	return a * t * t * t + b * t * t + c * t + d;
}

float Lerp(float A, float B, float t)
{
	return A * (1.0f - t) + B * t;
}

QRgb SampleLinearI(mip::COLOR* data, muint32 width, muint32 height, muint32 x, muint32 y)
{
	float xfract = 0.5f;
	float yfract = 0.5f;

	// get pixels
	mip::COLOR p00 = PixelClamped(data, width, height, x + 0, y + 0);
	mip::COLOR p10 = PixelClamped(data, width, height, x + 1, y + 0);
	mip::COLOR p01 = PixelClamped(data, width, height, x + 0, y + 1);
	mip::COLOR p11 = PixelClamped(data, width, height, x + 1, y + 1);

	muint8 ret[3];
	for (int i = 0; i < 3; ++i)
	{
		float col0 = Lerp(p00.c[i], p10.c[i], xfract);
		float col1 = Lerp(p01.c[i], p11.c[i], xfract);
		float value = Lerp(col0, col1, yfract);
		CLAMP(value, 0.0f, 255.0f);
		ret[i] = muint8(value);
	}

	return qRgb(ret[2], ret[1], ret[0]);
}

QRgb SampleLinearF(mip::COLOR* data, muint32 width, muint32 height, float u, float v)
{
	float x = (u * width) - 0.5f;
	int xint = int(x);
	float xfract = x - floor(x);

	float y = (v * height) - 0.5f;
	int yint = int(y);
	float yfract = y - floor(y);

	// get pixels
	mip::COLOR p00 = PixelClamped(data, width, height, xint + 0, yint + 0);
	mip::COLOR p10 = PixelClamped(data, width, height, xint + 1, yint + 0);
	mip::COLOR p01 = PixelClamped(data, width, height, xint + 0, yint + 1);
	mip::COLOR p11 = PixelClamped(data, width, height, xint + 1, yint + 1);

	muint8 ret[3];
	for (int i = 0; i < 3; ++i)
	{
		float col0 = Lerp(p00.c[i], p10.c[i], xfract);
		float col1 = Lerp(p01.c[i], p11.c[i], xfract);
		float value = Lerp(col0, col1, yfract);
		CLAMP(value, 0.0f, 255.0f);
		ret[i] = muint8(value);
	}

	return qRgb(ret[2], ret[1], ret[0]);
}

QRgb SampleBicubicF(mip::COLOR* image, muint32 width, muint32 height, float u, float v)
{
	// calculate coordinates -> also need to offset by half a pixel to keep image from shifting down and left half a pixel
	float x = (u * width) - 0.5;
	int xint = int(x);
	float xfract = x - floor(x);

	float y = (v * height) - 0.5;
	int yint = int(y);
	float yfract = y - floor(y);

	// 1st row
	auto p00 = PixelClamped(image, width, height, xint - 1, yint - 1);
	auto p10 = PixelClamped(image, width, height, xint + 0, yint - 1);
	auto p20 = PixelClamped(image, width, height, xint + 1, yint - 1);
	auto p30 = PixelClamped(image, width, height, xint + 2, yint - 1);

	// 2nd row
	auto p01 = PixelClamped(image, width, height, xint - 1, yint + 0);
	auto p11 = PixelClamped(image, width, height, xint + 0, yint + 0);
	auto p21 = PixelClamped(image, width, height, xint + 1, yint + 0);
	auto p31 = PixelClamped(image, width, height, xint + 2, yint + 0);

	// 3rd row
	auto p02 = PixelClamped(image, width, height, xint - 1, yint + 1);
	auto p12 = PixelClamped(image, width, height, xint + 0, yint + 1);
	auto p22 = PixelClamped(image, width, height, xint + 1, yint + 1);
	auto p32 = PixelClamped(image, width, height, xint + 2, yint + 1);

	// 4th row
	auto p03 = PixelClamped(image, width, height, xint - 1, yint + 2);
	auto p13 = PixelClamped(image, width, height, xint + 0, yint + 2);
	auto p23 = PixelClamped(image, width, height, xint + 1, yint + 2);
	auto p33 = PixelClamped(image, width, height, xint + 2, yint + 2);

	muint8 ret[3];
	for (int i = 0; i < 3; ++i)
	{
		float col0 = CubicHermite(p00.c[i], p10.c[i], p20.c[i], p30.c[i], xfract);
		float col1 = CubicHermite(p01.c[i], p11.c[i], p21.c[i], p31.c[i], xfract);
		float col2 = CubicHermite(p02.c[i], p12.c[i], p22.c[i], p32.c[i], xfract);
		float col3 = CubicHermite(p03.c[i], p13.c[i], p23.c[i], p33.c[i], xfract);
		float value = CubicHermite(col0, col1, col2, col3, yfract);
		CLAMP(value, 0.0f, 255.0f);
		ret[i] = muint8(value);
	}

	return qRgb(ret[2], ret[1], ret[0]);
}


QRgb SampleBicubicI(mip::COLOR* image, muint32 width, muint32 height, muint32 xint, muint32 yint)
{
	float xfract = 0.5f;
	float yfract = 0.5f;

	// 1st row
	auto p00 = PixelClamped(image, width, height, xint - 1, yint - 1);
	auto p10 = PixelClamped(image, width, height, xint + 0, yint - 1);
	auto p20 = PixelClamped(image, width, height, xint + 1, yint - 1);
	auto p30 = PixelClamped(image, width, height, xint + 2, yint - 1);

	// 2nd row
	auto p01 = PixelClamped(image, width, height, xint - 1, yint + 0);
	auto p11 = PixelClamped(image, width, height, xint + 0, yint + 0);
	auto p21 = PixelClamped(image, width, height, xint + 1, yint + 0);
	auto p31 = PixelClamped(image, width, height, xint + 2, yint + 0);

	// 3rd row
	auto p02 = PixelClamped(image, width, height, xint - 1, yint + 1);
	auto p12 = PixelClamped(image, width, height, xint + 0, yint + 1);
	auto p22 = PixelClamped(image, width, height, xint + 1, yint + 1);
	auto p32 = PixelClamped(image, width, height, xint + 2, yint + 1);

	// 4th row
	auto p03 = PixelClamped(image, width, height, xint - 1, yint + 2);
	auto p13 = PixelClamped(image, width, height, xint + 0, yint + 2);
	auto p23 = PixelClamped(image, width, height, xint + 1, yint + 2);
	auto p33 = PixelClamped(image, width, height, xint + 2, yint + 2);

	muint8 ret[3];
	for (int i = 0; i < 3; ++i)
	{
		float col0 = CubicHermite(p00.c[i], p10.c[i], p20.c[i], p30.c[i], xfract);
		float col1 = CubicHermite(p01.c[i], p11.c[i], p21.c[i], p31.c[i], xfract);
		float col2 = CubicHermite(p02.c[i], p12.c[i], p22.c[i], p32.c[i], xfract);
		float col3 = CubicHermite(p03.c[i], p13.c[i], p23.c[i], p33.c[i], xfract);
		float value = CubicHermite(col0, col1, col2, col3, yfract);
		CLAMP(value, 0.0f, 255.0f);
		ret[i] = muint8(value);
	}

	return qRgb(ret[2], ret[1], ret[0]);
}

void AnalMPRPlaneView::setImageData(muint32 ip_mode, QImage& image, mip::COLOR* data)
{
	QRgb value;
	int ymax = image.height();
	int xmax = image.width();
	for (int y = 0; y < ymax; y++)
	{
		QRgb* qrgb = (QRgb*)image.scanLine(y);
		float v = float(y) / float(ymax - 1);
		for (int x = 0; x < xmax; x++)
		{
			float u = float(x) / float(xmax - 1);

			switch (ip_mode)
			{
			case 0:
			{
				int ind = (xmax * y) + (x);
				qrgb[x] = qRgb(data[ind].r, data[ind].g, data[ind].b);
			}
			break;
			case 1:
				qrgb[x] = SampleLinearF(data, xmax, ymax, u, v);
				break;
			case 2:
				qrgb[x] = SampleBicubicF(data, xmax, ymax, u, v);
				break;
			default:
				break;
			}
		}
	}
}

void AnalMPRPlaneView::updateMeshOutline()
{
	m_bUpdateMeshOutline = true;

	renderLater();
}

void AnalMPRPlaneView::updatePlaneState()
{
	if (m_slidebar)
	{
		int value = 0;

		if (m_pWindowManager->getWheelZoom())
			setWheelSliderFunc(true);
		else
			setWheelSliderFunc(false);

		value = m_slidebar->value();

		slot_OnDepthSlideChanged(value);

		renderLater();
	}

}
