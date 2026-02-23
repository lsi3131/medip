#include "stdafx.h"
#include "SegmentationView.h"
#include "windowManager.h"
#include "Windows/Tabwindow.h"
#include "Windows/Tab/AISegTabDeepCatch.h"
#include "System/resourceManager.h"
#include "System/ShortcutManager.h"
#include "System/styleManager.h"
#include "System/ProductManager.h"
#include "Dialogs/AnnoTextDlg.h"
#include "Main/MainSegmentWidget.h"
#include "DataContext.h"

SegmentationView::SegmentationView(QWidget* parent, WINDOW_TYPE eType, MPRViewSharedInfo* pViewerSharedInfo)
	: WindowBase(parent, pViewerSharedInfo)
{
	m_windowType = eType;
	if (m_windowType == WT_AXIAL)
	{
		m_flipY = false;
	}

	m_showRightMenu = true;

	setMouseTracking(true);

	initInScreenMenu();
	initInScreenRightTopMenu();
	initInScreenRightBottomMenu();
	initInScreenLeftBottomMenu();

	registerViewerButtonInfo(m_btnPolyROI,
		RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_POLYROI, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_VIEW_POLYROI_HOVER, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_VIEW_POLYROI, m_nIconSize, m_nIconSize)
	);

	registerViewerButtonInfo(m_btnRegionROI,
		RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_FREEDRAWROI, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_VIEW_FREEDRAWROI_HOVER, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_VIEW_FREEDRAWROI, m_nIconSize, m_nIconSize)
	);

	registerViewerButtonInfo(m_btnPickerROI,
		RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_PICKERROI, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_VIEW_PICKERROI_HOVER, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_VIEW_PICKERROI, m_nIconSize, m_nIconSize)
	);

	registerViewerButtonInfo(m_btnRectROI,
		RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_ANGLEDROI, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_VIEW_ANGLEDROI_HOVER, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_VIEW_ANGLEDROI, m_nIconSize, m_nIconSize)
	);

	registerViewerButtonInfo(m_btnOvalROI,
		RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_OVALROI, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_VIEW_OVALROI_HOVER, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_VIEW_OVALROI, m_nIconSize, m_nIconSize)
	);

	registerViewerButtonInfo(m_btnAnoText,
		RESOURCE_MANAGER->getIcon(ICON_NON_ANNO_TEXT, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_ANNO_TEXT_HOVER, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_ANNO_TEXT, m_nIconSize, m_nIconSize)
	);

	registerViewerButtonInfo(m_btnL3Modify,
		RESOURCE_MANAGER->getIcon(ICON_DEEPCATCH_VIEW_NON_L3, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_DEEPCATCH_VIEW_L3_HOVER, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_DEEPCATCH_VIEW_L3, m_nIconSize, m_nIconSize)
	);

	registerViewerButtonInfo(m_btnAbdominalWaistModify,
		RESOURCE_MANAGER->getIcon(ICON_DEEPCATCH_VIEW_NON_AW, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_DEEPCATCH_VIEW_AW_HOVER, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_DEEPCATCH_VIEW_AW, m_nIconSize, m_nIconSize)
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

SegmentationView::~SegmentationView()
{

}

void SegmentationView::initInScreenMenu()
{
	WindowBase::initInScreenMenu();
}

void SegmentationView::initInScreenRightTopMenu()
{
	WindowBase::initInScreenRightTopMenu();

	//m_btnPolyROI = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_POLYROI), "", this);
	m_btnPolyROI = PRODUCT_FACTORY->createWidget<QPushButton>(MFL_Common_Segmentation_Manualdrawingtools_2DViewManualselectionfunction, this);
	if (m_btnPolyROI)
	{
		m_btnPolyROI->setIcon(RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_POLYROI, m_nIconSize, m_nIconSize));
		m_btnPolyROI->setIconSize(QSize(m_nIconSize, m_nIconSize));
		m_btnPolyROI->setFixedSize(QSize(m_nIconSize, m_nIconSize));
		m_btnPolyROI->setCheckable(true);
		m_btnPolyROI->setMouseTracking(true);
		m_btnPolyROI->installEventFilter(this);
		m_btnPolyROI->setObjectName("SegButtonPoly");
		m_btnPolyROI->hide();
		m_btnPolyROI->setStyleSheet("color: black;");
		m_btnPolyROI->setToolTip("Polygon Selection");

		connect(m_btnPolyROI, &QPushButton::clicked, this, &SegmentationView::slot_OnPolyROI);
	}

	//m_btnFreeDrawROI = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_FREEDRAWROI), "", this);
	m_btnRegionROI = PRODUCT_FACTORY->createWidget<QPushButton>(MFL_Common_Segmentation_Manualdrawingtools_2DViewManualselectionfunction, this);
	if (m_btnRegionROI)
	{
		m_btnRegionROI->setIcon(RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_FREEDRAWROI, m_nIconSize, m_nIconSize));
		m_btnRegionROI->setIconSize(QSize(m_nIconSize, m_nIconSize));
		m_btnRegionROI->setFixedSize(QSize(m_nIconSize, m_nIconSize));
		m_btnRegionROI->setCheckable(true);
		m_btnRegionROI->setMouseTracking(true);
		m_btnRegionROI->installEventFilter(this);
		m_btnRegionROI->setObjectName("SegButtonFreeDraw");
		m_btnRegionROI->hide();
		m_btnRegionROI->setStyleSheet("color: black;");
		m_btnRegionROI->setToolTip("Region Selection");

		connect(m_btnRegionROI, &QPushButton::clicked, this, &SegmentationView::slot_OnFreeDrawROI);
	}

	//m_btnPickerROI = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_PICKERROI), "", this);
	m_btnPickerROI = PRODUCT_FACTORY->createWidget<QPushButton>(MFL_Common_Segmentation_Manualdrawingtools_2DViewManualselectionfunction, this);
	if (m_btnPickerROI)
	{
		m_btnPickerROI->setIcon(RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_PICKERROI, m_nIconSize, m_nIconSize));
		m_btnPickerROI->setIconSize(QSize(m_nIconSize, m_nIconSize));
		m_btnPickerROI->setFixedSize(QSize(m_nIconSize, m_nIconSize));
		m_btnPickerROI->setCheckable(true);
		m_btnPickerROI->setMouseTracking(true);
		m_btnPickerROI->installEventFilter(this);
		m_btnPickerROI->setObjectName("SegButtonPicker");
		m_btnPickerROI->hide();
		m_btnPickerROI->setStyleSheet("color: black;");
		m_btnPickerROI->setToolTip("Pixel-wised Selection");

		connect(m_btnPickerROI, &QPushButton::clicked, this, &SegmentationView::slot_OnPickerROI);
	}

	//m_btnOvalROI = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_OVALROI), "", this);
	m_btnOvalROI = PRODUCT_FACTORY->createWidget<QPushButton>(MFL_Common_Segmentation_Manualdrawingtools_2DViewManualselectionfunction, this);
	if (m_btnOvalROI)
	{
		m_btnOvalROI->setIcon(RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_OVALROI, m_nIconSize, m_nIconSize));
		m_btnOvalROI->setIconSize(QSize(m_nIconSize, m_nIconSize));
		m_btnOvalROI->setFixedSize(QSize(m_nIconSize, m_nIconSize));
		m_btnOvalROI->setCheckable(true);
		m_btnOvalROI->setMouseTracking(true);
		m_btnOvalROI->installEventFilter(this);
		m_btnOvalROI->setObjectName("SegButtonOval");
		m_btnOvalROI->hide();
		m_btnOvalROI->setStyleSheet("color: black;");
		m_btnOvalROI->setToolTip("Oval Selection");

		connect(m_btnOvalROI, &QPushButton::clicked, this, &SegmentationView::slot_OnOvalROI);
	}

	//m_btnRectROI = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_ANGLEDROI), "", this);
	m_btnRectROI = PRODUCT_FACTORY->createWidget<QPushButton>(MFL_Common_Segmentation_Manualdrawingtools_2DViewManualselectionfunction, this);
	if (m_btnRectROI)
	{
		m_btnRectROI->setIcon(RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_ANGLEDROI, m_nIconSize, m_nIconSize));
		m_btnRectROI->setIconSize(QSize(m_nIconSize, m_nIconSize));
		m_btnRectROI->setFixedSize(QSize(m_nIconSize, m_nIconSize));
		m_btnRectROI->setCheckable(true);
		m_btnRectROI->setMouseTracking(true);
		m_btnRectROI->installEventFilter(this);
		m_btnRectROI->setObjectName("SegButtonRect");
		m_btnRectROI->hide();
		m_btnRectROI->setStyleSheet("color: black;");
		m_btnRectROI->setToolTip("Angular Selection");

		connect(m_btnRectROI, &QPushButton::clicked, this, &SegmentationView::slot_OnRectROI);
	}

	//#if !defined(DEEP_CATCH_VER) && !defined(COVID19_VER)
	//m_btnAnoText = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_ANNO_TEXT), "", this);
	m_btnAnoText = PRODUCT_FACTORY->createWidget<QPushButton>(MFL_Common_Annotation_Text, this);
	if (m_btnAnoText)
	{
		m_btnAnoText->setIcon(RESOURCE_MANAGER->getIcon(ICON_NON_ANNO_TEXT, m_nIconSize, m_nIconSize));
		m_btnAnoText->setIconSize(QSize(m_nIconSize, m_nIconSize));
		m_btnAnoText->setFixedSize(QSize(m_nIconSize, m_nIconSize));
		m_btnAnoText->setCheckable(true);
		m_btnAnoText->setMouseTracking(true);
		m_btnAnoText->installEventFilter(this);
		m_btnAnoText->setObjectName("SegButtonText");
		m_btnAnoText->hide();
		m_btnAnoText->setStyleSheet("color: black;");
		m_btnAnoText->setToolTip("Annotation - text");
		connect(m_btnAnoText, &QPushButton::clicked, this, &SegmentationView::slot_OnAnotationText);
	}
	//#endif

	if (PRODUCT_MANAGER->IsAvailableFunctionLevel(MFL_Product_DeepCatch) || PRODUCT_MANAGER->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2))
	{
		m_btnL3Modify = new QPushButton(this);
		if (m_btnL3Modify)
		{
			m_btnL3Modify->setIcon(RESOURCE_MANAGER->getIcon(ICON_DEEPCATCH_VIEW_NON_L3, m_nIconSize, m_nIconSize));
			m_btnL3Modify->setIconSize(QSize(m_nIconSize, m_nIconSize));
			m_btnL3Modify->setFixedSize(QSize(m_nIconSize, m_nIconSize));
			m_btnL3Modify->setMouseTracking(true);
			m_btnL3Modify->installEventFilter(this);
			m_btnL3Modify->setObjectName("L3");
			m_btnL3Modify->hide();
			m_btnL3Modify->setStyleSheet("color: black;");
			m_btnL3Modify->setToolTip("L3 Modification");
			connect(m_btnL3Modify, &QPushButton::clicked, this, &SegmentationView::slot_OnL3Modify);
		}
		m_btnAbdominalWaistModify = new QPushButton(this);
		if (m_btnAbdominalWaistModify)
		{
			m_btnAbdominalWaistModify->setIcon(RESOURCE_MANAGER->getIcon(ICON_DEEPCATCH_VIEW_NON_AW, m_nIconSize, m_nIconSize));
			m_btnAbdominalWaistModify->setIconSize(QSize(m_nIconSize, m_nIconSize));
			m_btnAbdominalWaistModify->setFixedSize(QSize(m_nIconSize, m_nIconSize));
			m_btnAbdominalWaistModify->setMouseTracking(true);
			m_btnAbdominalWaistModify->installEventFilter(this);
			m_btnAbdominalWaistModify->setObjectName("AW");
			m_btnAbdominalWaistModify->hide();
			m_btnAbdominalWaistModify->setStyleSheet("color: black;");
			m_btnAbdominalWaistModify->setToolTip("AW Modification");
			connect(m_btnAbdominalWaistModify, &QPushButton::clicked, this, &SegmentationView::slot_OnAWModify);
		}
	}
}

void SegmentationView::initInScreenRightBottomMenu()
{
	WindowBase::initInScreenRightBottomMenu();
}

void SegmentationView::initInScreenLeftBottomMenu()
{
	WindowBase::initInScreenLeftBottomMenu();
}

QVector3D SegmentationView::getShaderSliderDepthPosition()
{
	QVector3D pos;

	//if (WIN_MANAGER->mainTabType == MAINTAB_SEGMENTATION)
	{
		WindowBase* pAxial = WIN_MANAGER->mainSegmentWidget->getWindow(WT_AXIAL);
		WindowBase* pCoronal = WIN_MANAGER->mainSegmentWidget->getWindow(WT_CORONAL);
		WindowBase* pSagittal = WIN_MANAGER->mainSegmentWidget->getWindow(WT_SAGITTAL);

		int _x_idx = pSagittal->getDepth();
		int _y_idx = pCoronal->getDepth();
		int _z_idx = pAxial->getDepth();

		pos = QVector3D(_x_idx, _y_idx, _z_idx);
	}

	return pos;
}

void SegmentationView::updateScreenMenu(int width, int height)
{
	WindowBase::updateScreenMenu(width, height);

	int h = 0;
	if (m_btnPolyROI)
	{
		m_btnPolyROI->move(width - m_btnPolyROI->width(), h);
		h += m_btnPolyROI->height();
		m_btnRegionROI->move(width - m_btnRegionROI->width(), h);
		h += m_btnRegionROI->height();
		m_btnPickerROI->move(width - m_btnPickerROI->width(), h);
		h += m_btnPickerROI->height();
		m_btnOvalROI->move(width - m_btnOvalROI->width(), h);
		h += m_btnOvalROI->height();
		m_btnRectROI->move(width - m_btnRectROI->width(), h);
		h += 5 + m_btnRectROI->height();
	}

	if (m_btnAnoText)
	{
		m_btnAnoText->move(width - m_btnAnoText->width(), h);
		h += 5 + m_btnAnoText->height();
	}

	if (m_btnL3Modify)
	{
		m_btnL3Modify->move(width - m_btnL3Modify->width(), h);
		h += m_btnL3Modify->height();
	}

	if (m_btnAbdominalWaistModify)
	{
		m_btnAbdominalWaistModify->move(width - m_btnAbdominalWaistModify->width(), h);
		h += m_btnAbdominalWaistModify->height();
	}

}


void SegmentationView::enableControls(bool bEnable)
{
	WindowBase::enableControls(bEnable);

	if (m_btnPolyROI) m_btnPolyROI->setEnabled(bEnable);
	if (m_btnRegionROI) m_btnRegionROI->setEnabled(bEnable);
	if (m_btnPickerROI) m_btnPickerROI->setEnabled(bEnable);
	if (m_btnRectROI) m_btnRectROI->setEnabled(bEnable);
	if (m_btnOvalROI) m_btnOvalROI->setEnabled(bEnable);
	if (m_btnAnoText) m_btnAnoText->setEnabled(bEnable);
	if (m_btnL3Modify) m_btnL3Modify->setEnabled(bEnable);
	if (m_btnAbdominalWaistModify) m_btnAbdominalWaistModify->setEnabled(bEnable);
}

void SegmentationView::resetUI()
{
	if (m_slidebar)
	{
		if (m_windowType == WT_AXIAL)
		{
			m_depth = DATA_CONTEXT->volume_data.getCZ() / 2;
		}
		else if (m_windowType == WT_CORONAL)
		{
			m_depth = DATA_CONTEXT->volume_data.getCY() / 2;
		}
		else if (m_windowType == WT_SAGITTAL)
		{
			m_depth = DATA_CONTEXT->volume_data.getCX() / 2;
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

		m_latestClickPos.setX(DATA_CONTEXT->volume_data.getCX() / 2);
		m_latestClickPos.setY(DATA_CONTEXT->volume_data.getCY() / 2);
		m_latestClickPos.setZ(DATA_CONTEXT->volume_data.getCZ() / 2);

		DATA_CONTEXT->volume_data.setDepth(m_windowType, m_depth);
	}

	enableControls(WIN_MANAGER->IsEnableViewControls());
}

void SegmentationView::setDepth(uint depth)
{
	if (m_slidebar != NULL)
	{
		if (WIN_MANAGER->getWheelZoom())
		{
			if (m_windowType == WT_AXIAL)
			{
#ifdef USE_RIGHT_HAND
				m_slidebar->setValue((DATA_CONTEXT->volume_data.getCZ() - 1) - depth);
#else
				m_slidebar->setValue(mSlice);
#endif
			}
			else
			{
				m_slidebar->setValue(depth);
			}
		}
		else
		{
			WindowBase::setDepth(depth);
			WIN_MANAGER->renderLater_GridView(true);
		}
	}
}


void SegmentationView::setWorkMode(WORK_MODE mode, bool iconRefresh)
{
	static QIcon polyIcon[] = { RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_POLYROI, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_VIEW_POLYROI, m_nIconSize, m_nIconSize) };
	static QIcon freeIcon[] = { RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_FREEDRAWROI, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_VIEW_FREEDRAWROI, m_nIconSize, m_nIconSize) };
	static QIcon pickIcon[] = { RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_PICKERROI, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_VIEW_PICKERROI, m_nIconSize, m_nIconSize) };
	static QIcon rectIcon[] = { RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_ANGLEDROI, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_VIEW_ANGLEDROI, m_nIconSize, m_nIconSize) };
	static QIcon anoIcon[] = { RESOURCE_MANAGER->getIcon(ICON_NON_ANNO_TEXT, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_ANNO_TEXT, m_nIconSize, m_nIconSize) };
	static QIcon ovalIcon[] = { RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_OVALROI, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_VIEW_OVALROI, m_nIconSize, m_nIconSize) };

	WORK_MODE prMode = WIN_MANAGER->getWorkMode();
	WIN_MANAGER->setWorkMode(mode);

	m_polyLine.clear();
	bool volUpdate = false;

	if (mode != prMode)
	{
		switch (prMode)
		{
		case WORK_RECTROI:
			break;
		case WORK_OVALROI:
			break;
		case WORK_CAPTURE:
			if (WIN_MANAGER->GetTab()->getCaptureTab())
				WIN_MANAGER->GetTab()->getCaptureTab()->cancelCapture();
			break;
		case WORK_WORKING_REGION_MOVE:
			if (WIN_MANAGER->GetTab()->getPatchyTab())
				WIN_MANAGER->GetTab()->getPatchyTab()->cancelMoveMode();
			break;
		case WORK_PATCHY_SELECT:
			if (WIN_MANAGER->GetTab()->getPatchyTab())
				WIN_MANAGER->GetTab()->getPatchyTab()->cancelSelectMode();
			break;
		case WORK_SEED_SELECT:
			if (WIN_MANAGER->GetTab())
				WIN_MANAGER->GetTab()->getThreSholdTab()->cancelSelectMode();
			break;
		case WORK_POLYROI:
			m_bWorkMode = false;
			break;
		case WORK_REGION_ROI:
			break;
		case WORK_PICKERROI:
			break;
		case WORK_ANNOTATION_TEXT:
			break;
		case WORK_ANNOTATION_PATH:
			WIN_MANAGER->bCreateAni = false;
			if (WIN_MANAGER->GetTab())
			{
				AnimationTab* tab = WIN_MANAGER->GetTab()->getAniTab();

				if (tab)
					tab->CompleteAnimation();
			}
			volUpdate = true;
			break;
		case WORK_ANNOTATION_PATH_PLAY:
			WIN_MANAGER->AniType = WT_NONE;
			WIN_MANAGER->bStartAni = false;
			break;
		case WORK_3D_PLANE_SPLIT:
			WIN_MANAGER->setClipOpt();
		case WORK_3D_CURVE_SPLIT:
			volUpdate = true;
		case WORK_NONE:
		default:
			WIN_MANAGER->layerRGLocation.clear();
			break;
		}
	}

	m_cursor = Qt::ArrowCursor;

	WIN_MANAGER->DrawCutOnOff(mode == WORK_SKETCHDRAWSEGMENTATION);

	/* Mode상태에 버튼 업데이트 */
	updateToolButtonIcon_As_WorkMode(m_btnPolyROI, WORK_POLYROI, mode, polyIcon, iconRefresh);
	updateToolButtonIcon_As_WorkMode(m_btnRegionROI, WORK_REGION_ROI, mode, freeIcon, iconRefresh);
	updateToolButtonIcon_As_WorkMode(m_btnPickerROI, WORK_PICKERROI, mode, pickIcon, iconRefresh);
	updateToolButtonIcon_As_WorkMode(m_btnAnoText, WORK_ANNOTATION_TEXT, mode, anoIcon, iconRefresh);
	updateToolButtonIcon_As_WorkMode(m_btnOvalROI, WORK_OVALROI, mode, ovalIcon, iconRefresh);
	updateToolButtonIcon_As_WorkMode(m_btnRectROI, WORK_RECTROI, mode, rectIcon, iconRefresh);

	switch (mode)
	{
	case WORK_RECTROI:
	case WORK_SKETCHDRAWSEGMENTATION:
		m_cursor = getBitmapCursor();
		break;
	case WORK_OVALROI:
		m_cursor = getBitmapCursor();
		break;
	case WORK_CAPTURE:
		m_bWorkMode = false;
		{
			bool _mode = WIN_MANAGER->GetTab()->getCaptureTab()->getCaptureMode();

			if (_mode)
				m_cursor = Qt::PointingHandCursor;
			else
				m_cursor = Qt::CrossCursor;
		}
		break;
	case WORK_WORKING_REGION_MOVE:
		m_cursor = Qt::OpenHandCursor;
		break;
	case WORK_PATCHY_SELECT:
	case WORK_SEED_SELECT:
	case WORK_ANNOTATION_PATH:
		m_cursor = Qt::CrossCursor;
		break;
	case WORK_POLYROI:
		break;
	case WORK_REGION_ROI:
		break;
	case WORK_PICKERROI:
		break;
	case WORK_ANNOTATION_TEXT:
		break;
	case WORK_ANNOTATION_PATH_PLAY:
		volUpdate = true;
		break;
	case WORK_NONE:
	default:
		m_bWorkMode = false;
		break;
	}
	this->setCursor(m_cursor);

	if (WIN_MANAGER->mainTabType == MAINTAB_SEGMENTATION)
		WIN_MANAGER->renderLater_GridView(volUpdate);
}

void SegmentationView::render(QPainter* p)
{
	WindowBase::render(p);

	if (p == NULL)
		return;

	if (DATA_CONTEXT->volume_data.isValidate() && WIN_MANAGER->getRenderable() == true && !WIN_MANAGER->IsCropOn())
	{
		p->fillRect(0, 0, width(), height(), QColor(0, 0, 0));

		QVector3D vec;
		mint16 HU = SHRT_MIN;

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

		/* Draw Slice 이전에 Volume Calculator Preview 계산을 진행한다.*/
		if (WIN_MANAGER->mainTabType == MAINTAB_SEGMENTATION)
		{
			calcVolumePreview();
		}

		calcMesh2DOutline();

		drawSlice(p);

		p->setPen(QPen(QColor(255, 255, 255)));

		QRect drawTextRect;

		if (WIN_MANAGER->mainTabType == MAINTAB_SEGMENTATION)
		{
			if (WIN_MANAGER->bThrePreview)
				drawPreview(p);
			else
			{
				if (m_bIsMaskDraw)
				{
					drawMaskAll(p, m_dutyCheck);
					m_dutyCheck = false;
				}
			}
#ifdef SEGMENTATION_COLOR_CODING
			renderHeatMap(p);
#endif

			drawMouseWork(p);

			drawAnnotations(p);

			if (WIN_MANAGER->getDirectionMode())
			{
				drawDirection(m_slidebar->size().width(), p);
			}

			QPoint ptCoord(10, 0);
			QString unitText = WIN_MANAGER->getUnitString();
			drawCoordAndUnitTypeText(p, ptCoord, Qt::white, vec, HU, unitText, &drawTextRect);

			QPoint ptWordMode(10, drawTextRect.top() + 5);
			drawWorkModeText(p, ptWordMode, QColor(62, 137, 219), &drawTextRect);

			QPoint ptThresholdMode(10, drawTextRect.top() + 5);
			drawThresholdModeText(p, ptThresholdMode, QColor(62, 137, 219), &drawTextRect);

			drawBoundLine(p);

			drawCoordLine(p);

#ifdef DEV_VER
			drawBoundPoints(p);
#endif

		}

		muint32 downscaledCnt = DATA_CONTEXT->volume_data.getDownScaledCnt();
		if (downscaledCnt > 0)
		{
			QString text = QString("1/%1 Downscaled").arg(downscaledCnt == 1 ? 2 : 4);
			QPoint ptDownScaleCount(10, drawTextRect.top() + 5);
			drawText(p, ptDownScaleCount, QColor(62, 137, 219), text, &drawTextRect);
		}

		drawWaterMark(p);

		drawMeshOutline(p);
	}
	else
	{
		drawUnloaded(p);
	}

	// 220802 허 건 과장
	// Redmine 결함 #1320 Heatmap이 계속 활성화 되어있는 현상 수정
	checkColorMap();
}

void SegmentationView::wheelEvent(QWheelEvent* ev)
{
	this->setFocus();

	if (DATA_CONTEXT->volume_data.isValidate() == false) return;

	if (m_touchZoomEvent == true) return;

	if (m_bWorkMode == true) return;

	if (WIN_MANAGER->getWheelZoom())
	{
		if (m_WheelTimer)
		{
			m_WheelTimer->stop();
			m_WheelTimer->start(500);
		}
		zoom(ev->delta(), m_fZoomStepX, m_fZoomStepY);
	}
	else
	{
		if (m_windowType == WT_AXIAL)
		{
			if (ev->delta() > 0)
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
		}
		else // coronal, sagittal
		{
			if (ev->delta() > 0)
				WindowBase::setDepth(m_depth + 1);
			else
			{
				int val = m_depth;
				val--;
				if (val < 0)
					WindowBase::setDepth(0);
				else
					WindowBase::setDepth(m_depth - 1);
			}
		}

		updateViewerShaderedInfo();

		WIN_MANAGER->renderLater_GridView(false);
	}
	renderLater();
}

void SegmentationView::mousePressEvent(QMouseEvent* e)
{
	WindowBase::mousePressEvent(e);
	processWork_MousePress();
}

void SegmentationView::mouseMoveEvent(QMouseEvent* e)
{
	checkModifiers(e);

	if (WIN_MANAGER->getMoveFocus())
	{
		this->setFocus();
	}

	WIN_MANAGER->setLatestActiveViewType(m_windowType);
	//WIN_MANAGER->setLatestActiveViewDepth(m_depth);
	//WIN_MANAGER->setLatestActiveViewZoomX(m_zoomFactorX);
	//WIN_MANAGER->setLatestActiveViewZoomY(m_zoomFactorY);

	if (DATA_CONTEXT->volume_data.isValidate() == false) return;

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
			// 			WIN_MANAGER->setWindowLevel(WIN_MANAGER->getWindowLevel() + (m_preMousePos.y() - m_MousePos.y()));
			// 			WIN_MANAGER->setWindowWidth(WIN_MANAGER->getWindowWidth() + (m_MousePos.x() - m_preMousePos.x()));
			// 			WIN_MANAGER->renderLater_GridView(false);
			// 			WIN_MANAGER->renderLater_SubView();
			break;
		default:
			break;
		}
	}

	processWork_MouseMove();

	updateMouseActionMode();

	WindowBase::mouseMoveEvent(e);
}

void SegmentationView::mouseReleaseEvent(QMouseEvent* e)
{
	processWork_MouseRelease();

	WindowBase::mouseReleaseEvent(e);
}

void SegmentationView::mouseDoubleClickEvent(QMouseEvent* e)
{
	processWork_MouseDoubleClick(e);
}

void SegmentationView::focusInEvent(QFocusEvent* e)
{
	setWorkMode(WIN_MANAGER->getWorkMode());
	WindowBase::focusInEvent(e);
}

void SegmentationView::focusOutEvent(QFocusEvent* e)
{
	WindowBase::focusOutEvent(e);
}

void SegmentationView::preDepth()
{
	WindowBase::preDepth();

}

void SegmentationView::nextDepth()
{
	WindowBase::nextDepth();

}

void SegmentationView::hideControls()
{
	WindowBase::hideControls();

	if (m_btnPolyROI) m_btnPolyROI->hide();
	if (m_btnRegionROI) m_btnRegionROI->hide();
	if (m_btnPickerROI) m_btnPickerROI->hide();
	if (m_btnOvalROI) m_btnOvalROI->hide();
	if (m_btnRectROI) m_btnRectROI->hide();
	if (m_btnAnoText) m_btnAnoText->hide();
}

void SegmentationView::showControls()
{
	WindowBase::showControls();
	setWorkMode(WIN_MANAGER->getWorkMode());

	if (m_btnPolyROI) m_btnPolyROI->show();
	if (m_btnRegionROI) m_btnRegionROI->show();
	if (m_btnPickerROI) m_btnPickerROI->show();
	if (m_btnOvalROI) m_btnOvalROI->show();
	if (m_btnRectROI) m_btnRectROI->show();
	if (m_btnAnoText) m_btnAnoText->show();

}

bool SegmentationView::eventFilter(QObject* target, QEvent* e)
{
	if (target == NULL)
		return QWidget::eventFilter(target, e);

	if (!target->inherits("QPushButton") && !target->inherits("QSlider"))
		return QWidget::eventFilter(target, e);


	QEvent::Type _type = e->type();

	if (_type == QEvent::HoverEnter)
		this->unsetCursor();
	else if (_type == QEvent::HoverLeave)
		this->setCursor(m_cursor);


	return WindowBase::eventFilter(target, e);
}

void SegmentationView::updateToolButtonIcon_As_WorkMode(QPushButton* button, WORK_MODE targetWorkMode, WORK_MODE mode, QIcon icon[], bool iconRefresh)
{
	bool chkModeNotChanged = false;

	if (button)
	{
		/* WORK_NONE 모드일 경우 버튼을 초기상태로 변경*/
		if (mode == WORK_NONE)
		{
			button->setChecked(false);
			button->setIcon(icon[false]);
		}
		else
		{
			chkModeNotChanged = (mode == targetWorkMode);
			if (chkModeNotChanged)
			{
				button->setChecked(true);
				button->setIcon(icon[true]);
			}
			else if ((chkModeNotChanged == false) && iconRefresh)
			{
				button->setChecked(false);
				button->setIcon(icon[false]);
			}
		}
	}
}


void SegmentationView::processWork_MouseMove()
{
	if (DATA_CONTEXT->volume_data.isValidate() == false) return;

	if (m_touchZoomEvent == true) return;

	if (m_mouse_action_mode == MAM_NONE)
	{
		switch (WIN_MANAGER->getWorkMode())
		{
		case WORK_SKETCHDRAWSEGMENTATION:
		case WORK_RECTROI:
		case WORK_OVALROI:
			if (m_LbuttonDown || m_RbuttonDown)
			{
				bool bType = (WIN_MANAGER->getWorkMode() == WORK_RECTROI || WIN_MANAGER->getDCutShape() == DRAW_CUT_SHAPE_ANGULAR); //rect/angular type -> true
				bool bDraw = bType ? WIN_MANAGER->pencil_S : WIN_MANAGER->oval_S;

				if (bDraw)
				{
					if (m_LbuttonDown || m_RbuttonDown)
					{
						if (bType)
						{
							m_polyLine.append(m_MousePos);
						}
						else
						{
							if (m_polyLine.size() >= 2)
								m_polyLine.replace(1, m_MousePos);
							else
								m_polyLine.append(m_MousePos);
						}
					}
				}
				else
				{
					auto coord1 = m_list.begin();
					auto coord2 = m_list.last();

					if (m_polyLine.size() <= 0)
					{
						QRect rect;
						float zoomLow = m_fZoomLow;
						rect.setCoords(m_MousePos.x() + (coord1->x() * zoomLow), m_MousePos.y() + (coord1->y() * zoomLow),
							m_MousePos.x() + (coord2.x() * zoomLow), m_MousePos.y() + (coord2.y() * zoomLow));

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
								float zoomLow = m_fZoomLow;
								rect.setCoords(pV.x() + (coord1->x() * zoomLow), pV.y() + (coord1->y() * zoomLow),
									pV.x() + (coord2.x() * zoomLow), pV.y() + (coord2.y() * zoomLow));

								if (bType)
									m_rectRegion.addRect(rect);
								else
									m_rectRegion.addEllipse(rect);
							}
							m_polyLine.append(m_MousePos);
						}
					}
				}
			}
			break;
		case WORK_CAPTURE:
			if (m_LbuttonDown)
			{
				bool mode = WIN_MANAGER->GetTab()->getCaptureTab()->getCaptureMode();

				if (!mode) //rectangle
				{
					if (m_polyLine.count() >= 2)
					{
						m_polyLine.replace(1, m_MousePos);
					}
					else
						m_polyLine.push_back(m_MousePos);
				}
			}
			break;
		case WORK_WORKING_REGION_MOVE:
		{
			if (m_LbuttonDown && (Qt::ClosedHandCursor == this->cursor().shape()))
			{
				QPoint pos = screenToVolumePosition(&(m_MousePos - m_moveOffset));
				BoundingBoxI _box = DATA_CONTEXT->volume_data.getBoundingBoxForScreen(m_windowType);

				QVector3D vec;

				switch (m_windowType)
				{
				case WT_CORONAL:
					vec = QVector3D(pos.x(), m_depth, pos.y());
					break;
				case WT_SAGITTAL:
					vec = QVector3D(m_depth, pos.x(), pos.y());
					break;
				case WT_AXIAL:
				default:
					vec = QVector3D(pos.x(), pos.y(), m_depth);
					break;
				}

				checkVolumeArea(vec);


				bool _ratio = true;

				if (WIN_MANAGER->GetTab()->getPatchyTab())
					_ratio = WIN_MANAGER->GetTab()->getPatchyTab()->useRatio();

				BoundingBoxI newBox, orgBox;

				int x, y, z;

				newBox = orgBox = DATA_CONTEXT->volume_data.getBoundingBox();

				x = orgBox.maxX - orgBox.minX;
				y = orgBox.maxY - orgBox.minY;
				z = orgBox.maxZ - orgBox.minZ;

				newBox.minX = vec.x() - (x / 2);
				newBox.maxX = vec.x() + (x / 2);

				newBox.minY = vec.y() - (y / 2);
				newBox.maxY = vec.y() + (y / 2);

				newBox.minZ = vec.z() - (z / 2);
				newBox.maxZ = vec.z() + (z / 2);

				if (newBox != orgBox)
				{
					bool res = false;
					orgBox = newBox;

					newBox.validateCheck();

					if (_ratio) // 비율 유지
					{
						if (orgBox == newBox)
						{
							DATA_CONTEXT->volume_data.setBoundingBox(newBox);
							res = true;
						}
					}
					else
					{
						DATA_CONTEXT->volume_data.setBoundingBox(newBox);
						res = true;
					}

					if (res)
					{
						WIN_MANAGER->forceUpdate2DViewData(true, true);
						WIN_MANAGER->renderLater_GridView(false);
					}

				}
				//				
			}
		}
		break;
		case WORK_POLYROI:
			if (m_polyLine.size() >= 3)
			{
				QPoint q = m_polyLine[0] - m_MousePos;
				if (q.manhattanLength() < 10)
				{
					if (m_polyProcessCheck == false)
					{
						m_polyProcessCheck = true;
						renderLater();
					}
				}
				else if (m_polyProcessCheck == true)
				{
					m_polyProcessCheck = false;
					renderLater();
				}
			}
			break;
		case WORK_PICKERROI:
		case WORK_REGION_ROI:
			if (m_mouseDown || m_RbuttonDown)
			{
				m_polyLine.append(m_MousePos);
			}
			break;
		case WORK_NONE:
		case WORK_3D_CURVE_SPLIT:
		case WORK_3D_PLANE_SPLIT:
		{
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
		}

		if (m_mouse_action_mode == MAM_NONE)
		{
			if (m_LbuttonDown)
			{
				if (m_annoEditMode == true)
				{
					for (auto a = WIN_MANAGER->anotationList.begin(); a != WIN_MANAGER->anotationList.end(); ++a)
					{
						if ((*a)->getType() != AT_TEXT)
							continue;

						if ((*a)->isSelected() == true)
						{
							AnnoString* annoStr = (AnnoString*)(*a);
							annoStr->movePosition_Window(this, m_MousePos.x(), m_MousePos.y());
							break;
						}
					}
				}
				else
				{
					QVector3D pos = screenToVolumePosition3(&m_MousePos);

					if (m_latestClickPos != pos)
					{
						m_latestClickPos = pos;

						if (m_latestClickDepth != m_depth)
						{
							m_latestClickDepth = m_depth;
							WIN_MANAGER->setLatestActiveViewDepth(m_latestClickDepth);
						}

						WIN_MANAGER->setLatestActiveViewZoomX(m_zoomFactorX);
						WIN_MANAGER->setLatestActiveViewZoomY(m_zoomFactorY);
						WIN_MANAGER->renderLater_SubView();
					}

					setOtherMPRPlaneDepth_InMouseEvent(MAINTAB_SEGMENTATION);
#ifdef DEV_VER
					if (m_shift)
					{
						QVector3D vec = screenToVolumePosition3(&m_MousePos);
						checkVolumeArea(vec);

						if (WIN_MANAGER->boundPoints.size() == 2)
							WIN_MANAGER->boundPoints.replace(1, vec);
						//	else
						//		WIN_MANAGER->boundPoints.push_back(vec);

						WIN_MANAGER->renderLater_GridView(false);
					}
#endif
				}

				renderLater();
			}
			else if (m_RbuttonDown)
			{
				m_MovePos = m_MousePos;
				int xGap, yGap;
				int widthVal, levelVal;

				xGap = abs(m_MovePos.x() - m_StartPos.x());
				yGap = abs(m_MovePos.y() - m_StartPos.y());


				if (xGap >= yGap)
				{
					widthVal = WIN_MANAGER->getWindowWidth();
					WIN_MANAGER->getWidthLevelValue(false, widthVal, m_StartPos, m_MovePos);

					ACTION_MANAGER->action_Window_Width(widthVal);
				}
				else
				{
					levelVal = WIN_MANAGER->getWindowLevel();

					WIN_MANAGER->getWidthLevelValue(true, levelVal, m_StartPos, m_MovePos);

					ACTION_MANAGER->action_Window_Level(levelVal);
				}
				m_StartPos = m_MovePos;

			}
		}
		break;
		default:
			break;
		}

		if (WIN_MANAGER->getWorkMode() != WORK_NONE && (m_mouseDown || m_RbuttonDown))
			renderLater();
	}
}

void SegmentationView::processWork_MousePress()
{
	m_StartPos = m_MousePos;

	if (DATA_CONTEXT->volume_data.isValidate() == false)
		return;

	if (m_touchZoomEvent == true)
		return;

	if (m_mouse_action_mode == MAM_NONE)
	{
		switch (WIN_MANAGER->getWorkMode())
		{
		case WORK_SKETCHDRAWSEGMENTATION:
		case WORK_RECTROI:
		case WORK_OVALROI:
			if (m_LbuttonDown || m_RbuttonDown)
			{
				bool bType = (WIN_MANAGER->getWorkMode() == WORK_RECTROI || WIN_MANAGER->getDCutShape() == DRAW_CUT_SHAPE_ANGULAR);
				bool bDraw = bType ? WIN_MANAGER->pencil_S : WIN_MANAGER->oval_S;

				m_bWorkMode = true;
				m_polyLine.clear();
				m_polyLine.append(m_MousePos);
				if (!bDraw)
				{
					m_rectRegion = QPainterPath();
					QRect rect;
					auto coord1 = m_list.begin();
					auto coord2 = m_list.last();
					float zoomLow = m_fZoomLow;
					rect.setCoords(m_MousePos.x() + (coord1->x() * zoomLow), m_MousePos.y() + (coord1->y() * zoomLow),
						m_MousePos.x() + (coord2.x() * zoomLow), m_MousePos.y() + (coord2.y() * zoomLow));

					if (bType)
						m_rectRegion.addRect(rect);
					else
						m_rectRegion.addEllipse(rect);
				}
			}
			break;
		case WORK_POLYROI:
			if (m_LbuttonDown || m_RbuttonDown)
			{
				m_polyLine.append(m_MousePos);

				if (m_polyLine.size() > 1)
				{
					QPoint q = m_polyLine[0] - m_polyLine[m_polyLine.size() - 1];
					if (q.manhattanLength() < 10)
					{
						if (m_LbuttonDown)
						{
							ACTION_MANAGER->action_PolyROI(m_polyLine, this, false, WIN_MANAGER->getSelectedMask(),
								WIN_MANAGER->getSelectedMaskByteIndex());
						}
						else
						{
							ACTION_MANAGER->action_PolyROI_Del(m_polyLine, this, false, WIN_MANAGER->getSelectedMask(),
								WIN_MANAGER->getSelectedMaskByteIndex());
						}

						m_polyLine.clear();
					}
				}
				if (m_polyLine.size() == 0)
					m_bWorkMode = false;
				else
					m_bWorkMode = true;
			}
			break;
		case WORK_PICKERROI:
		case WORK_REGION_ROI:
			m_bWorkMode = true;
			if (m_LbuttonDown || m_RbuttonDown)
			{
				m_polyLine.append(m_MousePos);
			}
			break;
		case WORK_PATCHY_SELECT:
		{
			if (m_LbuttonDown)
			{
				WIN_MANAGER->patchyPoint = screenToVolumePosition3(&m_MousePos);
				WIN_MANAGER->setPatchyPoint();

				WIN_MANAGER->mainSegmentWidget->setWorkMode(WORK_NONE);
				WIN_MANAGER->renderLater_GridView(false);
			}
		}
		break;
		case WORK_SEED_SELECT:
		{
			if (m_LbuttonDown)
			{
				//non select 로 설정 (seedshow value 변경 x)
				if (WIN_MANAGER->getSeedShow() & RT_SHOW)
					WIN_MANAGER->setSeedShow(RT_SHOW);
				else
					WIN_MANAGER->setSeedShow(RT_NONE);

				QPoint pos = screenToVolumePosition(&m_MousePos);
				mint16 HU = getData(m_MousePos, m_depth);
				WIN_MANAGER->seedLocation.push_back(screenToVolumePosition3(&m_MousePos));
				//WIN_MANAGER->seedLocation.push_back(screenToCoord3(&m_MousePos));
				switch (m_windowType)
				{
				case WT_CORONAL:
					WIN_MANAGER->setRegionGrowingSeed(pos.x(), m_depth, pos.y(), HU);
					break;
				case WT_SAGITTAL:
					WIN_MANAGER->setRegionGrowingSeed(m_depth, pos.x(), pos.y(), HU);
					break;
				case WT_AXIAL:
					WIN_MANAGER->setRegionGrowingSeed(pos.x(), pos.y(), m_depth, HU);
					break;
				default:
					break;;
				}

				WIN_MANAGER->mainSegmentWidget->setWorkMode(WORK_NONE);
				WIN_MANAGER->renderLater_GridView(false);
			}
		}
		break;
		case WORK_ANNOTATION_TEXT:
			if (m_LbuttonDown)
			{
				m_LbuttonDown = false;
				m_mouseDown = false;
				QVector3D vecVolume = screenToVolumePosition3(&(m_MousePos));
				AnnoTextDlg dlg(this);
				if ((dlg.exec() == QDialog::Accepted) && (dlg.Text.isEmpty() == false))
				{
					mip::VECTOR3 v_world = getVolumeToWorld(&DATA_CONTEXT->volume_data, vecVolume.x(), vecVolume.y(), vecVolume.z());

					AnnoString* pAnnoString = new AnnoString(
						v_world,
						Annotation::DRAWING_FINISHED,
						dlg.Text,
						dlg.FontSize,
						toCOLOR(dlg.Color)
					);

					checkVolumeArea(vecVolume);
					ACTION_MANAGER->action_Annotation_Text_Add(pAnnoString);
				}
				setWorkMode(WORK_NONE);
			}
			break;
		case WORK_ANNOTATION_PATH:
			if (m_mouse_action_mode == MAM_NONE)
			{
				if (m_LbuttonDown || m_RbuttonDown)
				{
					WindowBase* winY;
					WindowBase* winX;

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

					if (winY != NULL && winX != NULL)
					{
						QPoint pos = screenToVolumePosition(&m_MousePos);
						//	m_latestClickPos = pos;
						winX->setDepth(pos.x());
						winY->setDepth(pos.y());

						OpenGLWidget* volumeView = WIN_MANAGER->mainSegmentWidget->getViewVolume();
						bool state = volumeView->getUpdateFrameState();

						if (state == true) // VR On
						{
							VOLUME_DATA& vd = DATA_CONTEXT->volume_data;

							float cx = 0, cy = 0, cz = 0;

							switch (m_windowType)
							{
							case WT_CORONAL:
							{
								cx = vd.getSpaceX() * pos.x() - vd.getSizeX() * 0.5f; // sagital
								cz = vd.getSpaceZ() * pos.y() - vd.getSizeZ() * 0.5f; // axial
								cy = vd.getSpaceY() * m_depth - vd.getSizeY() * 0.5f; // coronal
							}
							break;
							case WT_SAGITTAL:
							{
								cx = vd.getSpaceX() * m_depth - vd.getSizeX() * 0.5f;
								cz = vd.getSpaceZ() * pos.y() - vd.getSizeZ() * 0.5f; // axial
								cy = vd.getSpaceY() * pos.x() - vd.getSizeY() * 0.5f;
							}
							break;
							case WT_AXIAL:
							default:
							{
								cx = vd.getSpaceX() * pos.x() - vd.getSizeX() * 0.5f;
								cy = vd.getSpaceY() * pos.y() - vd.getSizeY() * 0.5f;
								cz = vd.getSpaceZ() * m_depth - vd.getSizeZ() * 0.5f;
							}
							break;
							}
							WIN_MANAGER->mainSegmentWidget->getViewVolume()->resetUI();
							WIN_MANAGER->matMoveCameraPos.set(cx, cy, cz);
						}

						if (m_LbuttonDown)
							WIN_MANAGER->renderLater_SubView();
						else
						{
							WIN_MANAGER->renderLater_GridView(false);
							return;
						}
					}
				}
			}
			break;
		case WORK_WORKING_REGION_MOVE:
		{
			QPoint pos = screenToVolumePosition(&m_MousePos);
			BoundingBoxI _box = DATA_CONTEXT->volume_data.getBoundingBoxForScreen(m_windowType);
			bool res = false;
			if (_box.minX <= pos.x() && pos.x() <= _box.maxX)
			{
				if (_box.minY <= pos.y() && pos.y() <= _box.maxY)
					res = true;
			}

			if (m_LbuttonDown && res)
			{
				m_cursor = Qt::ClosedHandCursor;
				this->setCursor(m_cursor);

				m_bWorkMode = true;

				int y = (_box.maxY);
				y -= (_box.minY);
				y /= 2;
				y += (_box.minY);
				/*	int y = _box.maxY;
				y -= _box.minY;
				y /= 2;
				y += _box.minY;*/

				int x = _box.maxX;
				x -= _box.minX;
				x /= 2;
				x += _box.minX;

				QVector3D vec = volumeToScreenPositionAuto(x, y, m_depth);

				m_moveOffset = m_MousePos - QPoint(vec.x(), vec.y());
				m_preMovePos = m_MousePos;
				m_stOrgBox = DATA_CONTEXT->volume_data.getBoundingBox();
				DATA_CONTEXT->volume_data.setPreBoundingBox();
			}
		}
		break;
		case WORK_CAPTURE:
		{
			bool mode = WIN_MANAGER->GetTab()->getCaptureTab()->getCaptureMode();
			bool type = WIN_MANAGER->GetTab()->getCaptureTab()->getCaptureType();

			m_polyLine.clear();
			if (mode)	//specific window
			{
				//TODO:: data & ROI
				/*QImage *img = getCaptureImage(mode, type);

				if(img)
				WIN_MANAGER->captureList.push_back(img);
				*/
				QImage img = getCaptureImage(mode, type);

				if (img != QImage())
					ACTION_MANAGER->action_Capture_image_Add(img);

				setWorkMode(WORK_NONE);
			}
			else //rectangle
			{
				//TODO:: data & ROI
				if (m_LbuttonDown)
				{
					m_bWorkMode = true;
					m_polyLine.append(m_MousePos);
				}
			}
		}
		break;
		case WORK_NONE:
		case WORK_3D_PLANE_SPLIT:
		case WORK_3D_CURVE_SPLIT:
			if (m_mouse_action_mode == MAM_NONE)
			{
				if (m_LbuttonDown)
				{
					bool chkRG = false;

					if (m_ctrl || m_alt || m_shift)
					{
						if (WIN_MANAGER->getRGType() == LAYER_RG_NONE)
						{
							if (m_ctrl)
								WIN_MANAGER->setRGType(LAYER_RG_CTRL);
							else if (m_alt)
								WIN_MANAGER->setRGType(LAYER_RG_ALT);
							else if (m_shift)
								WIN_MANAGER->setRGType(LAYER_RG_SHIFT);

							chkRG = true;
						}
						else if (WIN_MANAGER->getRGType() == LAYER_RG_CTRL)
						{
							if (m_ctrl)
								chkRG = true;
						}
						else if (WIN_MANAGER->getRGType() == LAYER_RG_ALT)
						{
							if (m_alt)
								chkRG = true;
						}
						else if (WIN_MANAGER->getRGType() == LAYER_RG_SHIFT)
						{
							if (m_shift)
								chkRG = true;
						}
					}
					else
						WIN_MANAGER->setRGType(LAYER_RG_NONE);

					if (chkRG)
					{
						//non select 로 설정 (seedshow value 변경 x)
						if (WIN_MANAGER->getSeedShow() & RT_SHOW)
							WIN_MANAGER->setSeedShow(RT_SHOW);
						else
							WIN_MANAGER->setSeedShow(RT_NONE);

						QPoint pos = screenToVolumePosition(&m_MousePos);
						mint16 HU = getData(m_MousePos, m_depth);
						WIN_MANAGER->layerRGLocation.push_back(screenToVolumePosition3(&m_MousePos));
						switch (m_windowType)
						{
						case WT_CORONAL:
							WIN_MANAGER->setRegionGrowingSeed(pos.x(), m_depth, pos.y(), HU, true);
							break;
						case WT_SAGITTAL:
							WIN_MANAGER->setRegionGrowingSeed(m_depth, pos.x(), pos.y(), HU, true);
							break;
						case WT_AXIAL:
							WIN_MANAGER->setRegionGrowingSeed(pos.x(), pos.y(), m_depth, HU, true);
							break;
						default:
							break;;
						}

						//	WIN_MANAGER->mainSegmentWidget->setWorkMode(WORK_NONE);
						WIN_MANAGER->renderLater_GridView();
						break;
					}

					m_annoEditMode = false;
					bool textEdit = false;
					muint32 annoIndex = 0;
					for (int n = 0; n < WIN_MANAGER->anotationList.size(); n++)
					{
						Annotation* anno = WIN_MANAGER->anotationList[n];

						if (anno->getType() != AT_TEXT)
							continue;

						AnnoString* annoStr = (AnnoString*)anno;

						if (annoStr->setSelect_Window(this, m_MousePos.x(), m_MousePos.y()))
						{
							if (annoStr->isSelected() == true)
							{
								textEdit = true;
								annoIndex = n;
							}

							//annoStr->clearSelect();
							m_annoEditMode = true;

							mip::VECTOR3 volumePos = annoStr->getVolumePos();

							QVector3D v = volumeToScreenPosition(
								volumePos.x,
								volumePos.y,
								volumePos.z);

							m_moveOffset = QPoint(v.x(), v.y()) - m_MousePos;
							m_preMovePos = m_MousePos;
						}
						else
							anno->clearSelect();
					}

					if (!m_annoEditMode)
					{
#ifdef DEV_VER
						if (m_shift)
						{
							QVector3D vec = screenToVolumePosition3(&m_MousePos);
							checkVolumeArea(vec);

							WIN_MANAGER->boundPoints.push_back(vec);

							WIN_MANAGER->renderLater_GridView(false);
						}
#endif
						QVector3D pos = screenToVolumePosition3(&m_MousePos);

						m_latestClickPos = pos;
						m_latestClickDepth = m_depth;
						WIN_MANAGER->setLatestActiveViewZoomX(m_zoomFactorX);
						WIN_MANAGER->setLatestActiveViewZoomY(m_zoomFactorY);
						WIN_MANAGER->setLatestActiveViewDepth(m_latestClickDepth);


						OpenGLWidget* volumeView = WIN_MANAGER->mainSegmentWidget->getViewVolume();
						bool state = volumeView->getUpdateFrameState();

						if (state == true) // VR On
						{
							QPoint pos = screenToVolumePosition(&m_MousePos);
							VOLUME_DATA& vd = DATA_CONTEXT->volume_data;

							float cx = 0, cy = 0, cz = 0;

							switch (m_windowType)
							{
							case WT_CORONAL:
							{
								cx = vd.getSpaceX() * pos.x() - vd.getSizeX() * 0.5f; // sagital
								cz = vd.getSpaceZ() * pos.y() - vd.getSizeZ() * 0.5f; // axial
								cy = vd.getSpaceY() * m_depth - vd.getSizeY() * 0.5f; // coronal
							}
							break;
							case WT_SAGITTAL:
							{
								cx = vd.getSpaceX() * m_depth - vd.getSizeX() * 0.5f;
								cz = vd.getSpaceZ() * pos.y() - vd.getSizeZ() * 0.5f; // axial
								cy = vd.getSpaceY() * pos.x() - vd.getSizeY() * 0.5f;
							}
							break;
							case WT_AXIAL:
							default:
							{
								cx = vd.getSpaceX() * pos.x() - vd.getSizeX() * 0.5f;
								cy = vd.getSpaceY() * pos.y() - vd.getSizeY() * 0.5f;
								cz = vd.getSpaceZ() * m_depth - vd.getSizeZ() * 0.5f;
							}
							break;
							}
							WIN_MANAGER->mainSegmentWidget->getViewVolume()->resetUI();
							WIN_MANAGER->matMoveCameraPos.set(cx, cy, cz);
						}

						if (WIN_MANAGER->getRepositioningMode() == true)
						{
							setOtherMPRPlaneDepth_InMouseEvent(MAINTAB_SEGMENTATION);
						}
					}

					WIN_MANAGER->renderLater_All();
					return;
				}
			}
			break;
		default:
			break;
		}

		renderLater();
	}

}

void SegmentationView::processWork_MouseRelease()
{
	if (DATA_CONTEXT->volume_data.isValidate())
	{
		switch (WIN_MANAGER->getWorkMode())
		{
		case WORK_OVALROI:
		case WORK_SKETCHDRAWSEGMENTATION:
		case WORK_RECTROI:
		{
			bool bType = (WIN_MANAGER->getWorkMode() == WORK_RECTROI || WIN_MANAGER->getDCutShape() == DRAW_CUT_SHAPE_ANGULAR);
			bool bDraw = bType ? WIN_MANAGER->pencil_S : WIN_MANAGER->oval_S;

			m_bWorkMode = false;
			if (bDraw)
			{
				if (bType)
				{
					if (WIN_MANAGER->getWorkMode() == WORK_SKETCHDRAWSEGMENTATION)
					{
#ifndef MULTI_DRAWCUT_MODE
						ACTION_MANAGER->action_Marking(m_polyLine, this, true, false, m_RbuttonDown ? VM_MASK0 : VM_MASK1);
#else
						ACTION_MANAGER->action_Marking(m_polyLine, this, true, false, WIN_MANAGER->getDrawingSeedMask(m_LbuttonDown),
							WIN_MANAGER->getDrawingSeedIndex(m_LbuttonDown));
#endif
					}
					else if (m_LbuttonDown)
						ACTION_MANAGER->action_Marking(m_polyLine, this, false, false, WIN_MANAGER->getSelectedMask(), WIN_MANAGER->getSelectedMaskByteIndex());
					else if (m_RbuttonDown)
						ACTION_MANAGER->action_UnMarking(m_polyLine, this, false, WIN_MANAGER->getSelectedMask(), WIN_MANAGER->getSelectedMaskByteIndex());
				}
				else
				{
					bool res = false;

					if (m_RbuttonDown)
						res = true;

					if (m_polyLine.size() >= 2)
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
						if (WIN_MANAGER->getWorkMode() == WORK_SKETCHDRAWSEGMENTATION)
						{
#ifndef MULTI_DRAWCUT_MODE
							ACTION_MANAGER->action_MarkingSquare(region, this, m_RbuttonDown ? VM_MASK0 : VM_MASK1, 0, false, true);
#else
							uchar drawM = WIN_MANAGER->getDrawingSeedMask(m_LbuttonDown);
							int drawIndex = WIN_MANAGER->getDrawingSeedIndex(m_LbuttonDown);
							bool chkDefault = (drawM <= VM_MASK1) && (drawIndex == 0);

							ACTION_MANAGER->action_MarkingSquare(region, this, drawM, drawIndex, false, chkDefault);
#endif
						}
						else if (m_LbuttonDown)
						{



							ACTION_MANAGER->action_MarkingSquare(region, this, WIN_MANAGER->getSelectedMask(), WIN_MANAGER->getSelectedMaskByteIndex());

						}
						else if (m_RbuttonDown)
							ACTION_MANAGER->action_UnMarkingSquare(region, this, WIN_MANAGER->getSelectedMask(), WIN_MANAGER->getSelectedMaskByteIndex());
					}
				}

			}
			else
			{
				if (WIN_MANAGER->getWorkMode() == WORK_SKETCHDRAWSEGMENTATION)
				{
#ifndef MULTI_DRAWCUT_MODE
					ACTION_MANAGER->action_MarkingSquare(m_rectRegion, this, m_RbuttonDown ? VM_MASK0 : VM_MASK1, 0, false, true);
#else
					uchar drawM = WIN_MANAGER->getDrawingSeedMask(m_LbuttonDown);
					int drawIndex = WIN_MANAGER->getDrawingSeedIndex(m_LbuttonDown);
					bool chkDefault = (drawM <= VM_MASK1) && (drawIndex == 0);

					ACTION_MANAGER->action_MarkingSquare(m_rectRegion, this, drawM, drawIndex, false, chkDefault);
#endif
				}
				else
				{
					if (m_LbuttonDown)
					{
						if (WIN_MANAGER->GetTab()->getThreSholdTab() != NULL)
						{
							int start_HU = WIN_MANAGER->GetTab()->getThreSholdTab()->getStartHU();
							int end_HU = WIN_MANAGER->GetTab()->getThreSholdTab()->getEndHU();
							bool brush = WIN_MANAGER->GetTab()->getThreSholdTab()->getBrush();

							ACTION_MANAGER->action_MarkingSquare(m_rectRegion, this, WIN_MANAGER->getSelectedMask()
								, WIN_MANAGER->getSelectedMaskByteIndex(), false, false, brush, start_HU, end_HU);
						}
						else
						{
							int start_HU = 0;
							int end_HU = 0;
							bool brush = false;

							ACTION_MANAGER->action_MarkingSquare(m_rectRegion, this, WIN_MANAGER->getSelectedMask()
								, WIN_MANAGER->getSelectedMaskByteIndex(), false, false, brush, start_HU, end_HU);
						}
					}
					else if (m_RbuttonDown)
					{
						ACTION_MANAGER->action_UnMarkingSquare(m_rectRegion, this, WIN_MANAGER->getSelectedMask(), WIN_MANAGER->getSelectedMaskByteIndex());
					}
				}
				m_rectRegion = QPainterPath();
			}
			m_polyLine.clear();
			renderLater();
			//WIN_MANAGER->renderLater_3DView();

		}
		break;
		case WORK_CAPTURE:
			if (m_LbuttonDown && m_polyLine.count() >= 2)
			{
				bool type = WIN_MANAGER->GetTab()->getCaptureTab()->getCaptureType();
				m_bWorkMode = false;
				//TODO
				/*	QImage *img = getCaptureImage(false, type);

				if (img)
				WIN_MANAGER->captureList.push_back(img);*/

				QImage img = getCaptureImage(false, type);

				if (img != QImage())
					ACTION_MANAGER->action_Capture_image_Add(img);

				m_polyLine.clear();
			}
			setWorkMode(WORK_NONE);
			break;
		case WORK_WORKING_REGION_MOVE:
			if (m_LbuttonDown && m_bWorkMode)
			{
				m_bWorkMode = false;
				ACTION_MANAGER->action_BoundingBox_Modify(DATA_CONTEXT->volume_data.getBoundingBox());
				WIN_MANAGER->mainSegmentWidget->setWorkMode(WORK_NONE);
				WIN_MANAGER->renderLater_GridView(false);
			}
			break;
		case WORK_POLYROI:
		{
		}
		break;
		case WORK_PICKERROI:
		{
			m_bWorkMode = false;
			if (m_LbuttonDown)
				ACTION_MANAGER->action_Marking(m_polyLine, this, false, false, WIN_MANAGER->getSelectedMask(), WIN_MANAGER->getSelectedMaskByteIndex());
			else if (m_RbuttonDown)
				ACTION_MANAGER->action_UnMarking(m_polyLine, this, false, WIN_MANAGER->getSelectedMask(), WIN_MANAGER->getSelectedMaskByteIndex());

			m_polyLine.clear();
		}
		break;
		case WORK_REGION_ROI:
		{
			m_bWorkMode = false;
			if (m_LbuttonDown)
				ACTION_MANAGER->action_FreeDrawROI(m_polyLine, this, false, WIN_MANAGER->getSelectedMask(), WIN_MANAGER->getSelectedMaskByteIndex());
			else if (m_RbuttonDown)
				ACTION_MANAGER->action_FreeDrawROI_Del(m_polyLine, this, false, WIN_MANAGER->getSelectedMask(), WIN_MANAGER->getSelectedMaskByteIndex());

			m_polyLine.clear();
		}
		break;


		/*{
		m_bWorkMode = false;
		if (m_RbuttonDown)
		ACTION_MANAGER->action_Marking(m_polyLine, this, true, false, VM_MASK0);
		else if ( m_LbuttonDown )
		ACTION_MANAGER->action_Marking(m_polyLine, this, true, false, VM_MASK1);

		m_polyLine.clear();
		}
		break;*/
		case WORK_ANNOTATION_PATH:
			if (m_mouse_action_mode == MAM_NONE)
			{
				int group = WIN_MANAGER->getAnimationGroup();
				if (m_LbuttonDown)
				{
					QVector3D vec = screenToVolumePosition3(&m_MousePos);
					if (!WIN_MANAGER->isContainAniPoint(vec, group))
						ACTION_MANAGER->action_Annotation_Path_Add(vec, group);
				}
				else if (m_RbuttonDown)
				{
					if (WIN_MANAGER->aniCount.at(group) > 0)
						ACTION_MANAGER->action_Annotation_Path_Clear(group);
				}
			}
			break;
		case WORK_NONE:
		case WORK_3D_PLANE_SPLIT:
		case WORK_3D_CURVE_SPLIT:
		{
			if (m_LbuttonDown)
			{
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

#ifdef DEV_VER
				if (m_shift)
				{
					WIN_MANAGER->renderLater_GridView(false);

					m_shift = false;
					/*	QMessageBox::warning(NULL, QString("DEV_VER ONLY"), QString("start (%1,%2,%3)\nend (%4,%5,%6)\n")
					.arg(WIN_MANAGER->boundPoints.at(0).x()).arg(WIN_MANAGER->boundPoints.at(0).y()).arg(WIN_MANAGER->boundPoints.at(0).z())
					.arg(WIN_MANAGER->boundPoints.at(1).x()).arg(WIN_MANAGER->boundPoints.at(1).y()).arg(WIN_MANAGER->boundPoints.at(1).z()));*/

					// boxing segmentation test
					if (WIN_MANAGER->boundPoints.size() == 2)
					{
						BoundingBoxI		boundingBoxROI;
						boundingBoxROI.reset(DATA_CONTEXT->volume_data.getCX(), DATA_CONTEXT->volume_data.getCY(), DATA_CONTEXT->volume_data.getCZ());
						double distance = mip::TA::EuclideanDistance(WIN_MANAGER->boundPoints.at(0).x(), WIN_MANAGER->boundPoints.at(0).y(), WIN_MANAGER->boundPoints.at(0).z(), WIN_MANAGER->boundPoints.at(1).x(), WIN_MANAGER->boundPoints.at(1).y(), WIN_MANAGER->boundPoints.at(1).z());
						int cog_x = (int)((WIN_MANAGER->boundPoints.at(0).x() + WIN_MANAGER->boundPoints.at(1).x()) / 2. + 0.5);
						int cog_y = (int)((WIN_MANAGER->boundPoints.at(0).y() + WIN_MANAGER->boundPoints.at(1).y()) / 2. + 0.5);
						int cog_z = (int)((WIN_MANAGER->boundPoints.at(0).z() + WIN_MANAGER->boundPoints.at(1).z()) / 2. + 0.5);

						int width = DATA_CONTEXT->volume_data.getCX();
						int height = DATA_CONTEXT->volume_data.getCY();
						int slice = DATA_CONTEXT->volume_data.getCZ();

						boundingBoxROI.minX = cog_x - distance / 2;
						boundingBoxROI.minY = cog_y - distance / 2;
						boundingBoxROI.minZ = cog_z - (distance / 2 * DATA_CONTEXT->volume_data.getSpaceX() / DATA_CONTEXT->volume_data.getSpaceZ());
						boundingBoxROI.maxX = cog_x + distance / 2;
						boundingBoxROI.maxY = cog_y + distance / 2;
						boundingBoxROI.maxZ = cog_z + (distance / 2 * DATA_CONTEXT->volume_data.getSpaceX() / DATA_CONTEXT->volume_data.getSpaceZ());

						if (boundingBoxROI.minX < 1) boundingBoxROI.minX = 1;
						if (boundingBoxROI.maxX > width - 2) boundingBoxROI.maxX = width - 2;
						if (boundingBoxROI.minY < 1) boundingBoxROI.minY = 1;
						if (boundingBoxROI.maxY > height - 2) boundingBoxROI.maxY = height - 2;
						if (boundingBoxROI.minZ < 1) boundingBoxROI.minZ = 1;
						if (boundingBoxROI.maxZ > slice - 2) boundingBoxROI.maxZ = slice - 2;


						DATA_CONTEXT->volume_data.setBoundingBox(boundingBoxROI);
						WIN_MANAGER->renderLater_GridView();


						ACTION_MANAGER->action_ImageEnhance(2);
						uchar* resMask = DATA_CONTEXT->volume_data.getMaskDataPoint(0);

						for (int z = boundingBoxROI.getMinZ(); z <= boundingBoxROI.getMaxZ(); z++)
						{
							for (int y = boundingBoxROI.getMinY(); y <= boundingBoxROI.getMaxY(); y++)
							{
								for (int x = boundingBoxROI.getMinX(); x <= boundingBoxROI.getMaxX(); x++)
								{
									if (
										x == boundingBoxROI.getMinX() || x == boundingBoxROI.getMaxX() ||
										y == boundingBoxROI.getMinY() || y == boundingBoxROI.getMaxY() ||
										z == boundingBoxROI.getMinZ() || z == boundingBoxROI.getMaxZ()
										)

										resMask[z * width * height + y * width + x] = VM_MASK0;

								}
							}
						}

						unsigned char* outputData = new unsigned char[width * height * slice];
						memset(outputData, 0, width * height * slice);
						mip::TA::Bresenham3D(WIN_MANAGER->boundPoints.at(0).x(), WIN_MANAGER->boundPoints.at(0).y(), WIN_MANAGER->boundPoints.at(0).z(),
							WIN_MANAGER->boundPoints.at(1).x(), WIN_MANAGER->boundPoints.at(1).y(), WIN_MANAGER->boundPoints.at(1).z(),
							outputData, 1, width, height, slice);

						QStack<mip::TA::PointHU> line_stack;

						int line_max = -1024;
						int line_min = 1024;

						double std_dev = 0.0, xsq = 0.0, y = 0.0;


						int mask_count = 0;
						double sum = 0;
						for (int z = boundingBoxROI.getMinZ(); z <= boundingBoxROI.getMaxZ(); z++)
						{
							for (int y = boundingBoxROI.getMinY(); y <= boundingBoxROI.getMaxY(); y++)
							{
								for (int x = boundingBoxROI.getMinX(); x <= boundingBoxROI.getMaxX(); x++)
								{
									if (outputData[z * width * height + y * width + x] != 0)
									{
										mask_count++;
										mip::TA::PointHU pt;
										pt.x = x;
										pt.y = y;
										pt.z = z;
										pt.value = DATA_CONTEXT->volume_data.getData(z * width * height + y * width + x);
										sum = sum + pt.value;

										line_stack.push(pt);
										if (pt.value > line_max) line_max = pt.value;
										if (pt.value < line_min) line_min = pt.value;

										//										qDebug() << x << "\t" << y << "\t" << z << "\t" << pt.value << endl;
									}
								}
							}
						}

						double mean = sum / mask_count;


						int i = 0;
						short* input_array = new short[line_stack.size()];
						for (int z = boundingBoxROI.getMinZ(); z <= boundingBoxROI.getMaxZ(); z++)
						{
							for (int y = boundingBoxROI.getMinY(); y <= boundingBoxROI.getMaxY(); y++)
							{
								for (int x = boundingBoxROI.getMinX(); x <= boundingBoxROI.getMaxX(); x++)
								{
									if (outputData[z * width * height + y * width + x] != 0)
									{
										input_array[i++] = DATA_CONTEXT->volume_data.getData(z * width * height + y * width + x);
										std_dev += (DATA_CONTEXT->volume_data.getData(z * width * height + y * width + x) - mean) * (DATA_CONTEXT->volume_data.getData(z * width * height + y * width + x) - mean);
									}
								}
							}
						}

						std_dev = (double)sqrt((double)std_dev / (double)mask_count);


						int threshold = (line_min + line_max) / 2; // otsu(1, line_stack.size(), input_array);;

						qDebug() << line_stack.size() << endl;
						qDebug() << 1 << endl;
						qDebug() << threshold << endl;

						BOOL sign = FALSE;
						if (line_stack.top().value < threshold) sign = TRUE;
						int foreseed_mean = 0;
						int foreseed_count = 0;
						while (!line_stack.empty())
						{
							if (sign)
							{
								if (line_stack.top().value > threshold)
								{
									foreseed_mean = foreseed_mean + line_stack.top().value;
									foreseed_count++;
									resMask[line_stack.top().z * width * height + line_stack.top().y * width + line_stack.top().x] = VM_MASK1;
									/*		else if (line_stack.top().value < threshold - std_dev)
									resMask[line_stack.top().z * width*height + line_stack.top().y *width + line_stack.top().x] = VM_MASK0;*/
								}
							}
							else
							{

								if (line_stack.top().value < threshold)
								{
									foreseed_mean = foreseed_mean + line_stack.top().value;
									foreseed_count++;
									resMask[line_stack.top().z * width * height + line_stack.top().y * width + line_stack.top().x] = VM_MASK1;
									/*else if (line_stack.top().value > threshold + std_dev)
									resMask[line_stack.top().z * width*height + line_stack.top().y *width + line_stack.top().x] = VM_MASK0;*/
								}
							}

							line_stack.pop();
						}
						foreseed_mean = foreseed_mean / foreseed_count;

						int histo_width = (boundingBoxROI.getMaxX() - boundingBoxROI.getMinX()) + 1;
						int histo_height = (boundingBoxROI.getMaxY() - boundingBoxROI.getMinY()) + 1;
						int histo_slice = (boundingBoxROI.getMaxZ() - boundingBoxROI.getMinZ()) + 1;

						unsigned char* histo_img = new unsigned char[histo_width * histo_height * histo_slice];
						unsigned char* histo_img_out = new unsigned char[histo_width * histo_height * histo_slice];
						for (int z = boundingBoxROI.getMinZ(); z <= boundingBoxROI.getMaxZ(); z++)
						{
							for (int y = boundingBoxROI.getMinY(); y <= boundingBoxROI.getMaxY(); y++)
							{
								for (int x = boundingBoxROI.getMinX(); x <= boundingBoxROI.getMaxX(); x++)
								{
									int normalized_value = 255 * (DATA_CONTEXT->volume_data.getData(z * width * height + y * width + x) - (mean - (line_max - line_min) / 2)) / (line_max - line_min);
									if (normalized_value > 255) normalized_value = 255;
									if (normalized_value < 0) normalized_value = 0;

									//if (normalized_value <= mean - (line_max - line_min) / 2)
									//	normalized_value = line_min;
									//else if (normalized_value >= mean + (line_max - line_min) / 2)
									//	normalized_value = line_max;


									histo_img[(z - boundingBoxROI.getMinZ()) * histo_width * histo_height + (y - boundingBoxROI.getMinY()) * histo_width + (x - boundingBoxROI.getMinX())] = normalized_value;
									//									DATA_CONTEXT->volume_data.setData(z*width*height + y*width + x, normalized_value);
									//									outputData[z*width*height + y*width + x] = normalized_value;

								}
							}
						}

						// compute the histogram of the sliceImg
						int HISTOGRAM_SIZE = 256;
						//						unsigned int *equalHistogram = new unsigned int[HISTOGRAM_SIZE];
						//						unsigned int *sumEqualHistogram = new unsigned int[HISTOGRAM_SIZE];

						mip::TA::equalizeHistogram(histo_img, histo_width, histo_height, histo_slice, histo_img_out);
						//						getHistogram(histo_img_out, histo_width, histo_height, histo_slice, equalHistogram, HISTOGRAM_SIZE);
						//						getSumHistogram(histo_img_out, histo_width, histo_height, histo_slice, sumEqualHistogram, HISTOGRAM_SIZE);

						for (int z = boundingBoxROI.getMinZ(); z <= boundingBoxROI.getMaxZ(); z++)
						{
							for (int y = boundingBoxROI.getMinY(); y <= boundingBoxROI.getMaxY(); y++)
							{
								for (int x = boundingBoxROI.getMinX(); x <= boundingBoxROI.getMaxX(); x++)
								{
									DATA_CONTEXT->volume_data.setData(z * width * height + y * width + x, histo_img_out[(z - boundingBoxROI.getMinZ()) * histo_width * histo_height + (y - boundingBoxROI.getMinY()) * histo_width + (x - boundingBoxROI.getMinX())]);

								}
							}
						}

						OffsetDlg dlg(this);

						dlg.exec();

						if (dlg.isAccept())
							ACTION_MANAGER->action_SketchDrawSegmentation(this, false, WIN_MANAGER->getSelectedMask(), WIN_MANAGER->getSelectedMaskByteIndex(), dlg.getDoubleVal());


						delete[]input_array;
						delete[]outputData;
						//						delete[]equalHistogram;
						//						delete[]sumEqualHistogram;
						delete[]histo_img;
						delete[]histo_img_out;
						//						ACTION_MANAGER->action_ImageEnhance(0);
						QString clip_board_str;

						WIN_MANAGER->boundPoints.clear();
					}
				}
				else
				{
					if (WIN_MANAGER->boundPoints.size() == 2)
					{
						WIN_MANAGER->boundPoints.clear();
					}
				}
#endif
				if (m_mouse_action_mode == MAM_NONE && !m_annoEditMode)
				{
					QVector3D pos = screenToVolumePosition3(&m_MousePos);

					m_latestClickPos = pos;

					if (m_latestClickDepth != m_depth)
					{
						m_latestClickDepth = m_depth;
						WIN_MANAGER->setLatestActiveViewDepth(m_latestClickDepth);
					}

					WIN_MANAGER->setLatestActiveViewZoomX(m_zoomFactorX);
					WIN_MANAGER->setLatestActiveViewZoomY(m_zoomFactorY);


					WIN_MANAGER->renderLater_SubView();
				}
			}

			if (m_RbuttonDown)
			{
				if (!m_bMoveMouse)
				{
					setContextMenu();
					m_pContextMenu->exec(m_globalPos);

					WIN_MANAGER->renderLater_GridView(false);

					return;
				}

			}
			else if (m_annoEditMode == true)
			{
				for (int n = 0; n < WIN_MANAGER->anotationList.size(); n++)
				{
					Annotation* anno = WIN_MANAGER->anotationList[n];

					if (anno->getType() != AT_TEXT) continue;

					if (anno->isSelected() == true && m_MousePos != m_preMovePos)
					{
						QVector3D vec_prev = screenToVolumePosition3(&(m_moveOffset + m_preMovePos));
						QVector3D vec_new = screenToVolumePosition3(&(m_moveOffset + m_MousePos));
						checkVolumeArea(vec_new);
						checkVolumeArea(vec_prev);

						AnnoString* pAnnoStr = (AnnoString*)anno;
						AnnoString annoPrev = *pAnnoStr;
						AnnoString annoNew = *pAnnoStr;

						annoPrev.setVolumePos(vec_prev.x(), vec_prev.y(), vec_prev.z());
						annoNew.setVolumePos(vec_new.x(), vec_new.y(), vec_new.z());

						ACTION_MANAGER->action_Annotation_Text_Edit(this, n, &annoPrev, &annoNew);

						m_preMovePos = m_MousePos;
						break;
					}
				}
			}

		}
		break;
		default:
			break;
		}

		renderLater();
	}
}

void SegmentationView::processWork_MouseDoubleClick(QMouseEvent* e)
{
	if (e->buttons() & Qt::RightButton)
		m_RbuttonDown = true;
	else if (e->buttons() & Qt::LeftButton)
		m_LbuttonDown = true;
	else if (e->buttons() & Qt::MidButton)
		m_MbuttonDown = true;

	switch (WIN_MANAGER->getWorkMode())
	{
	case WORK_NONE:
	case WORK_3D_PLANE_SPLIT:
	case WORK_3D_CURVE_SPLIT:
		if (m_LbuttonDown)
		{
			m_annoEditMode = false;
			bool textEdit = false;
			muint32 annoIndex = 0;
			for (int n = 0; n < WIN_MANAGER->anotationList.size(); n++)
			{
				Annotation* anno = WIN_MANAGER->anotationList[n];

				if (anno->getType() != AT_TEXT)
					continue;

				AnnoString* annoStr = (AnnoString*)anno;

				if (annoStr->setSelect_Window(this, m_MousePos.x(), m_MousePos.y()))
				{
					if (annoStr->isSelected() == true)
					{
						textEdit = true;
						annoIndex = n;
					}

					m_preMovePos = m_MousePos;
				}
				else
					annoStr->clearSelect();
			}

			if (textEdit == true) // annoText Edit
			{
				AnnoString* anno = static_cast<AnnoString*>(WIN_MANAGER->anotationList[annoIndex]);
				if (anno)
				{
					AnnoTextDlg dlg(this, anno->getText(), anno->getFontSize(), anno->getColor());
					if (dlg.exec() == QDialog::Accepted && (dlg.Text.isEmpty() == false))
					{
						AnnoString* pAnnoStr = (AnnoString*)anno;
						AnnoString annoPrev = *pAnnoStr;
						AnnoString annoNew = *pAnnoStr;

						annoNew.setColor(toCOLOR(dlg.Color));
						annoNew.setText(dlg.Text);
						annoNew.setFontSize(dlg.FontSize);

						ACTION_MANAGER->action_Annotation_Text_Edit(this, annoIndex, &annoPrev, &annoNew);
					}
					anno->clearSelect();
					renderLater();
				}
			}
			else
				WIN_MANAGER->setCoordType(REDRAW_TYPE);
		}
		break;
	default:
		break;
	}
}

void SegmentationView::checkColorMap()
{
	if (WIN_MANAGER->GetTab())
	{
		AISegTab* tab = WIN_MANAGER->GetTab()->getAITab();

		if (tab)
		{
			if (!tab->IsHeatMapEnabled() || !tab->IsHeatMapMode())
			{
				if (m_pColorTable)
				{
					m_pColorTable->hide();
					m_pColorTable->clearCategoryMap();
				}
			}
		}
	}
}

void SegmentationView::slot_OnPolyROI()
{
	WIN_MANAGER->setSegmentationWorkMode(WORK_POLYROI);
}

void SegmentationView::slot_OnFreeDrawROI()
{
	WIN_MANAGER->setSegmentationWorkMode(WORK_REGION_ROI);
}

void SegmentationView::slot_OnPickerROI()
{
	WIN_MANAGER->setSegmentationWorkMode(WORK_PICKERROI);
}

void SegmentationView::slot_OnAnotationText()
{
	WIN_MANAGER->setSegmentationWorkMode(WORK_ANNOTATION_TEXT);
}

void SegmentationView::slot_OnOvalROI()
{
	WIN_MANAGER->setSegmentationWorkMode(WORK_OVALROI);
}

void SegmentationView::slot_OnRectROI()
{
	WIN_MANAGER->setSegmentationWorkMode(WORK_RECTROI);
}

