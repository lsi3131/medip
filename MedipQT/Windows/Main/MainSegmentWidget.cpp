#include "stdafx.h"
#include "MainSegmentWidget.h"
#include "MedipQT.h"

#include "Windows/VolumeView.h"
#include "Windows/SegmentationView.h"
#include "Windows/ResultView.h"
#include "Windows/SubView.h"
#include "Windows/glwidget.h"
#include "Windows/Tabwindow.h"
#include "Windows/Tab/AISegTabDeepCatch.h"
#include "Windows/windowManager.h"
#include "Windows/Main/MainAnalWidget.h"
#include "Windows/Main/MainTAWidget.h"

#include "System/stringManager.h"
#include "System/FileManager.h"
#include "System/styleManager.h"
#include "System/resourceManager.h"
#include "Actions/ActionManager.h"

#include "graphics/anotation.h"

#include "UI/CollapseDock.h"
#include "UI/MaskList.h"

#define ALPHACOLOR
#ifdef ALPHACOLOR
#include "UI/AlphaColorMap.h"
#endif

#include "UI/CustomHistogram.h"
#include "UI/RangeWidget.h"


#include "DataContext.h"

#include <qwt_plot_curve.h>

MainSegmentWidget::MainSegmentWidget(QWidget* parent) :
	MainTabWidget(parent),
	m_preset(SP_DEFAULT),
	m_prePreset(SP_DEFAULT)
{
	m_viewAxial = nullptr;
	m_viewCoronal = nullptr;
	m_viewSagittal = nullptr;
	m_leftFullScreen = nullptr;
	m_tabEnhance = 0;
	m_tabInfo = 0;
	m_tabROI = 0;
	m_sliderSmooth = 0;
	m_viewPlot = nullptr;
	m_vrUpdateTimer = 0;
	m_rightBox = nullptr;
	m_viewSub1 = nullptr;
	m_viewSub2 = nullptr;
	m_viewSub3 = nullptr;
	m_viewSub4 = nullptr;
	m_viewSub5 = nullptr;
	m_viewSub6 = nullptr;
	m_viewSub7 = nullptr;
	m_viewSub8 = nullptr;
	m_btnZoom = nullptr;
	m_btnCopy = nullptr;
	m_btnSave = nullptr;
	m_viewPlot = nullptr;
	m_bHistogram = false;
	m_bSubView = false;
#ifdef DEV_VER
	bHighlight = false;
	//for (int i = 0; i < 8; ++i)
	//{
	//	emptyMemory[i] = new short[1024 * 1024 * 1024];
	//	memset(emptyMemory[i], 0, sizeof(short) * 1024 * 1024 * 1024);
	//}
#endif
}

MainSegmentWidget::~MainSegmentWidget()
{
	if (m_leftFullScreen != nullptr)
	{
		SAFE_DELETE(m_leftGrid);
		SAFE_DELETE(m_centerWidgets);
	}
}

void MainSegmentWidget::activate()
{
	WIN_MANAGER->mainHLayout->addWidget(this);
	setSharedWidget();
	show();
	showControls(WIN_MANAGER->getLatestActiveViewType());

	WIN_MANAGER->mainWindow->setDockWindowTitle(STRING_MANAGER->getString(STR_SEGMENT));

	if (getHistogramView())
	{
		getHistogramView()->AfterFullScreen();
	}
}

void MainSegmentWidget::deactivate()
{
	//	setWorkMode(WORK_NONE);
	hideControls();
	deleteCenterWindows();
	hide();

	WIN_MANAGER->mainHLayout->removeWidget(this);
}

bool MainSegmentWidget::init(DataContext* pDataContext)
{
	m_pDataContext = pDataContext;

	m_segmentMainLayout = new QVBoxLayout(this);
	m_segmentMainLayout->setMargin(0);
	m_segmentMainLayout->setSpacing(0);

	// Left Windows
	m_leftGrid = nullptr;
	if (createLeftGridWindows() == false)
	{
		return false;
	}

	m_centerWidgets = new QWidget(this);
	m_centerWidgets->setMaximumWidth(120);
	m_centerWidgets->setContentsMargins(1, 1, 1, 1);
	m_centerWidgets->hide();
	m_centerLayout = new QVBoxLayout(m_centerWidgets);
	m_centerLayout->setMargin(1);
	m_centerLayout->setSpacing(1);

	m_screenLayout = new QHBoxLayout();
	QVBoxLayout* histogramArea = new QVBoxLayout();
	m_histogramLayout = new QHBoxLayout;
	m_btnLayout = new QVBoxLayout;
	m_plotLayout = new QHBoxLayout;
	m_segmentMainLayout->addLayout(m_screenLayout);
	m_segmentMainLayout->addLayout(histogramArea);
	m_screenLayout->addWidget(m_leftGrid);
	m_screenLayout->addWidget(m_centerWidgets);
	m_histogramLayout->addLayout(m_btnLayout);
	m_histogramLayout->addLayout(m_plotLayout);
	histogramArea->addLayout(m_histogramLayout);

	m_btnZoom = new QPushButton(this);
	m_btnZoom->setObjectName("m_btnZoom");
	m_btnZoom->setStyleSheet(STYLE_MANAGER->buttonBehind);
	m_btnZoom->setText(tr("Log"));
	m_btnZoom->setCheckable(true);
	m_btnZoom->hide();
	connect(m_btnZoom, &QPushButton::clicked, this, &MainSegmentWidget::slot_OnZoomClicked);

	m_btnCopy = new QPushButton(this);
	m_btnCopy->setObjectName("m_btnCopy");
	m_btnCopy->setStyleSheet(STYLE_MANAGER->buttonBehind);
	m_btnCopy->setText(tr("Copy"));
	m_btnCopy->setToolTip("Copy histogram to csv format.");
	m_btnCopy->hide();
	connect(m_btnCopy, &QPushButton::clicked, this, &MainSegmentWidget::slot_OnHistogramCopy);

	m_btnSave = new QPushButton(this);
	m_btnSave->setObjectName("m_btnSave");
	m_btnSave->setStyleSheet(STYLE_MANAGER->buttonBehind);
	m_btnSave->setText(tr("Save"));
	m_btnSave->setToolTip("Save histogram to *.csv file.");
	m_btnSave->hide();
	connect(m_btnSave, &QPushButton::clicked, this, &MainSegmentWidget::slot_OnHistogramSave);

	return true;
}

void MainSegmentWidget::enableTOIAll()
{
	SegmentationView* aView = static_cast<SegmentationView*> (m_viewAxial);
	if (aView)
	{
		aView->setShowBounding(true);
	}

	SegmentationView* sView = static_cast<SegmentationView*> (m_viewSagittal);
	if (sView)
	{
		sView->setShowBounding(true);
	}

	SegmentationView* cView = static_cast<SegmentationView*> (m_viewCoronal);
	if (cView)
	{
		cView->setShowBounding(true);
	}
}

void MainSegmentWidget::setWorkMode(WORK_MODE mode)
{
	SegmentationView* win = (SegmentationView*)getWindow(WIN_MANAGER->getLatestActiveViewType());

	if (nullptr != win)
	{
		win->setWorkMode(mode);
	}

	if (m_viewVolume)
	{
		VolumeView* vView = static_cast<VolumeView*> (m_viewVolume);
		vView->setWorkMode(mode);
	}

	if (win != m_viewAxial)
	{
		((SegmentationView*)m_viewAxial)->setWorkMode(mode);
	}

	if (win != m_viewSagittal)
	{
		((SegmentationView*)m_viewSagittal)->setWorkMode(mode);
	}

	if (win != m_viewCoronal)
	{
		((SegmentationView*)m_viewCoronal)->setWorkMode(mode);
	}
}

void MainSegmentWidget::changeShapeSize()
{
	WindowBase* win = getWindow(WIN_MANAGER->getLatestActiveViewType());

	if (nullptr != win)
		win->createCursorShape(true);

	if (win != m_viewAxial)
		m_viewAxial->createCursorShape(true);

	if (win != m_viewSagittal)
		m_viewSagittal->createCursorShape(true);

	if (win != m_viewCoronal)
		m_viewCoronal->createCursorShape(true);
}

void MainSegmentWidget::renderLater_All()
{
	renderLater_GridView();
	renderLater_SubView();
}

void MainSegmentWidget::updateLayerState()
{
	if (m_viewAxial)
		m_viewAxial->setDutyCheck();

	if (m_viewSagittal)
		m_viewSagittal->setDutyCheck();

	if (m_viewCoronal)
		m_viewCoronal->setDutyCheck();
}

void MainSegmentWidget::renderLater_GridView(bool volumeupdate)
{
	if (m_viewAxial)
	{
		m_viewAxial->renderLater();
	}

	if (m_viewSagittal)
	{
		m_viewSagittal->renderLater();
	}

	if (m_viewCoronal)
	{
		m_viewCoronal->renderLater();
	}

	if (m_viewVolume && volumeupdate)
	{
		m_viewVolume->renderLater();
	}
}

void MainSegmentWidget::forceUpdate2DViewData()
{
	if (m_viewAxial)
		m_viewAxial->forceUpdateSliceColorData();

	if (m_viewSagittal)
		m_viewSagittal->forceUpdateSliceColorData();

	if (m_viewCoronal)
		m_viewCoronal->forceUpdateSliceColorData();
}

void MainSegmentWidget::delMaskView()
{
	m_pDataContext->volume_data.forceUpdateMaskVolume();

	WIN_MANAGER->renderLater_GridView();
}

void MainSegmentWidget::renderLater_SubView()
{
	if (isExistSView())
	{
		if (m_viewSub1)
			m_viewSub1->renderLater();

		if (m_viewSub2)
			m_viewSub2->renderLater();

		if (m_viewSub3)
			m_viewSub3->renderLater();

		if (m_viewSub4)
			m_viewSub4->renderLater();

		if (m_viewSub5)
			m_viewSub5->renderLater();

		if (m_viewSub6)
			m_viewSub6->renderLater();

		if (m_viewSub7)
			m_viewSub7->renderLater();

		if (m_viewSub8)
			m_viewSub8->renderLater();
	}
}

void MainSegmentWidget::hideControls()
{
	if (m_viewAxial)
	{
		m_viewAxial->hideControls();
	}

	if (m_viewSagittal)
	{
		m_viewSagittal->hideControls();
	}

	if (m_viewCoronal)
	{
		m_viewCoronal->hideControls();
	}

	if (m_viewVolume)
	{
		m_viewVolume->hideControls();
	}
}

void MainSegmentWidget::showControls(WINDOW_TYPE windowType)
{
	if (!m_pDataContext->volume_data.isValidate() || !WIN_MANAGER->getRenderable())
	{
		return;
	}

	switch (windowType)
	{
	case WT_AXIAL:
		if (m_viewAxial)
		{
			m_viewAxial->showControls();
		}
		break;
	case WT_SAGITTAL:
		if (m_viewSagittal)
		{
			m_viewSagittal->showControls();
		}
		break;
	case WT_CORONAL:
		if (m_viewCoronal)
		{
			m_viewCoronal->showControls();
		}
		break;
	case WT_VOLUME:
		if (m_viewVolume)
		{
			m_viewVolume->showControls();
		}
		break;
	default:
		break;
	}
}

void MainSegmentWidget::updatePlaneData(WINDOW_TYPE windowType)
{
	if (!m_pDataContext->volume_data.isValidate())
	{
		return;
	}

	WindowBase* view = nullptr;
	switch (windowType)
	{
	case WT_AXIAL:
		view = m_viewAxial;
		break;
	case WT_SAGITTAL:
		view = m_viewSagittal;
		break;
	case WT_CORONAL:
		view = m_viewCoronal;
		break;
	case WT_VOLUME:
		break;
	default:
		break;
	}

	if (view == nullptr)
	{
		return;
	}

	view->updatePlaneDatas();
}

void MainSegmentWidget::updateSliderPosition()
{
	if (!m_pDataContext->volume_data.isValidate())
	{
		return;
	}

	if (m_viewAxial)
	{
		m_viewAxial->updateSliderPosition();
	}

	if (m_viewCoronal)
	{
		m_viewCoronal->updateSliderPosition();
	}

	if (m_viewSagittal)
	{
		m_viewSagittal->updateSliderPosition();
	}
}

void MainSegmentWidget::setSharedWidget()
{
	if (m_btnZoom)
	{
		m_btnLayout->addWidget(m_btnZoom);
	}
	if (m_btnCopy)
	{
		m_btnLayout->addWidget(m_btnCopy);
	}
	if (m_btnSave)
	{
		m_btnLayout->addWidget(m_btnSave);
	}
	if (m_viewPlot)
	{
		m_plotLayout->addWidget(m_viewPlot);
	}

	if (m_viewVolume)
	{
		VolumeView* vol = static_cast<VolumeView*>(m_viewVolume);
		WIN_MANAGER->volumeLayerAlphaSet();
		vol->renderLater();
	}
}

bool MainSegmentWidget::createLeftGridWindows()
{
	VolumeView* volumeView = new VolumeView(this);
	m_viewVolume = volumeView;
	widgetVolume = volumeView;
	widgetVolume->setFocusPolicy(Qt::StrongFocus);
	widgetVolume->setMouseTracking(true);
	volumeView->reserveInit((HWND)widgetVolume->winId());
	connect(m_viewVolume, SIGNAL(setFullScreen(OpenGLWidget*)), this, SLOT(slot_setFullScreen(OpenGLWidget*)));

	//m_viewAxial = new AxialView(this);
	m_viewAxial = new SegmentationView(this, WT_AXIAL, &m_viewerSharedInfo);
	m_viewAxial->setFocusPolicy(Qt::StrongFocus);
	m_viewAxial->setMouseTracking(true);
	connect(m_viewAxial, SIGNAL(setFullScreen(WindowBase*)), this, SLOT(slot_setFullScreen(WindowBase*)));

	//m_viewCoronal = new CoronalView(this);
	m_viewCoronal = new SegmentationView(this, WT_CORONAL, &m_viewerSharedInfo);
	m_viewCoronal->setFocusPolicy(Qt::StrongFocus);
	m_viewCoronal->setMouseTracking(true);
	connect(m_viewCoronal, SIGNAL(setFullScreen(WindowBase*)), this, SLOT(slot_setFullScreen(WindowBase*)));

	//viewSaggital = new SaggitalView(this);
	m_viewSagittal = new SegmentationView(this, WT_SAGITTAL, &m_viewerSharedInfo);
	m_viewSagittal->setFocusPolicy(Qt::StrongFocus);
	m_viewSagittal->setMouseTracking(true);
	connect(m_viewSagittal, SIGNAL(setFullScreen(WindowBase*)), this, SLOT(slot_setFullScreen(WindowBase*)));

	if (!m_leftGrid)
	{
		m_leftGrid = new QWidget(this);
	}
	m_leftGrid->setContentsMargins(0, 0, 0, 0);
	m_gridlayout = new QGridLayout;
	m_gridlayout->setContentsMargins(0, 0, 0, 0);
	//	m_m_gridlayout->setMargin(2);
	m_gridlayout->setSpacing(1);
	m_leftGrid->setLayout(m_gridlayout);

	m_gridlayout->addWidget(widgetVolume, 0, 0);
	m_gridlayout->addWidget(m_viewAxial, 0, 1);
	m_gridlayout->addWidget(m_viewCoronal, 1, 0);
	m_gridlayout->addWidget(m_viewSagittal, 1, 1);

	initWindows();

	return true;
}

void MainSegmentWidget::deleteLeftGridWindows()
{
	disconnect(m_viewVolume, SIGNAL(setFullScreen(OpenGLWidget*)), this, SLOT(slot_setFullScreen(OpenGLWidget*)));
	m_viewVolume->hide();
	widgetVolume->hide();
	m_gridlayout->removeWidget(widgetVolume);
	m_viewVolume->deleteLater();
	m_viewVolume = nullptr;
	widgetVolume = nullptr;

	disconnect(m_viewAxial, SIGNAL(setFullScreen(WindowBase*)), this, SLOT(slot_setFullScreen(WindowBase*)));
	m_viewAxial->hide();
	m_gridlayout->removeWidget(m_viewAxial);
	m_viewAxial->deleteLater();
	m_viewAxial = nullptr;

	disconnect(m_viewCoronal, SIGNAL(setFullScreen(WindowBase*)), this, SLOT(slot_setFullScreen(WindowBase*)));
	m_viewCoronal->hide();
	m_gridlayout->removeWidget(m_viewCoronal);
	m_viewCoronal->deleteLater();
	m_viewCoronal = nullptr;

	disconnect(m_viewSagittal, SIGNAL(setFullScreen(WindowBase*)), this, SLOT(slot_setFullScreen(WindowBase*)));
	m_viewSagittal->hide();
	m_gridlayout->removeWidget(m_viewSagittal);
	m_viewSagittal->deleteLater();
	m_viewSagittal = nullptr;

	m_gridlayout->deleteLater();
	m_gridlayout = nullptr;
}

void MainSegmentWidget::setMPRViewInfo(QVector3D volumePos, mint16 HU)
{
	m_viewerSharedInfo.VolumeVoxelPosition = volumePos;
	m_viewerSharedInfo.HU = HU;
}

void MainSegmentWidget::updateSharedInfo()
{
	mint16& huVal = m_viewerSharedInfo.HU;

	if (huVal == SHRT_MIN)
	{
		return;
	}

	QVector3D& volumePosition = m_viewerSharedInfo.VolumeVoxelPosition;
	QVector3D screenPosition;

	switch (m_viewerSharedInfo.winType)
	{
	case WT_CORONAL:
		screenPosition = m_viewCoronal->volumeToScreenPositionAuto(volumePosition.x(), volumePosition.y(), volumePosition.z());
		huVal = m_viewCoronal->getData(QPoint(screenPosition.x(), screenPosition.y()), m_viewerSharedInfo.depth);
		m_viewCoronal->renderLater();
		break;
	case WT_SAGITTAL:
		screenPosition = m_viewSagittal->volumeToScreenPositionAuto(volumePosition.x(), volumePosition.y(), volumePosition.z());
		huVal = m_viewSagittal->getData(QPoint(screenPosition.x(), screenPosition.y()), m_viewerSharedInfo.depth);
		m_viewSagittal->renderLater();
		break;
	case WT_AXIAL:
	default:
		screenPosition = m_viewAxial->volumeToScreenPositionAuto(volumePosition.x(), volumePosition.y(), volumePosition.z());
		huVal = m_viewAxial->getData(QPoint(screenPosition.x(), screenPosition.y()), m_viewerSharedInfo.depth);
		break;
	}

	m_viewCoronal->renderLater();
	m_viewSagittal->renderLater();
	m_viewAxial->renderLater();
}

bool MainSegmentWidget::createCenterWindows()
{
	m_viewSub1 = new SubView(-4, this);
	m_viewSub1->setContentsMargins(1, 1, 1, 1);

	m_viewSub2 = new SubView(-3, this);
	m_viewSub2->setContentsMargins(1, 1, 1, 1);

	m_viewSub3 = new SubView(-2, this);
	m_viewSub3->setContentsMargins(1, 1, 1, 1);

	m_viewSub4 = new SubView(-1, this);
	m_viewSub4->setContentsMargins(1, 1, 1, 1);

	m_viewSub5 = new SubView(1, this);
	m_viewSub5->setContentsMargins(1, 1, 1, 1);

	m_viewSub6 = new SubView(2, this);
	m_viewSub6->setContentsMargins(1, 1, 1, 1);

	m_viewSub7 = new SubView(3, this);
	m_viewSub7->setContentsMargins(1, 1, 1, 1);

	m_viewSub8 = new SubView(4, this);
	m_viewSub8->setContentsMargins(1, 1, 1, 1);

	m_centerLayout->addWidget(m_viewSub1);
	m_centerLayout->addWidget(m_viewSub2);
	m_centerLayout->addWidget(m_viewSub3);
	m_centerLayout->addWidget(m_viewSub4);
	m_centerLayout->addWidget(m_viewSub5);
	m_centerLayout->addWidget(m_viewSub6);
	m_centerLayout->addWidget(m_viewSub7);
	m_centerLayout->addWidget(m_viewSub8);
	m_centerWidgets->show();
	m_bSubView = true;

	return m_bSubView;
}

bool MainSegmentWidget::deleteCenterWindows()
{
	m_centerWidgets->hide();

	if (isExistSView())
	{
		m_centerLayout->removeWidget(m_viewSub1);
		m_centerLayout->removeWidget(m_viewSub2);
		m_centerLayout->removeWidget(m_viewSub3);
		m_centerLayout->removeWidget(m_viewSub4);
		m_centerLayout->removeWidget(m_viewSub5);
		m_centerLayout->removeWidget(m_viewSub6);
		m_centerLayout->removeWidget(m_viewSub7);
		m_centerLayout->removeWidget(m_viewSub8);

		m_viewSub1->deleteLater();
		m_viewSub2->deleteLater();
		m_viewSub3->deleteLater();
		m_viewSub4->deleteLater();
		m_viewSub5->deleteLater();
		m_viewSub6->deleteLater();
		m_viewSub7->deleteLater();
		m_viewSub8->deleteLater();

		m_viewSub1 = nullptr;
		m_viewSub2 = nullptr;
		m_viewSub3 = nullptr;
		m_viewSub4 = nullptr;
		m_viewSub5 = nullptr;
		m_viewSub6 = nullptr;
		m_viewSub7 = nullptr;
		m_viewSub8 = nullptr;
		m_bSubView = false;
	}
	return true;
}

bool MainSegmentWidget::isExistSView() 
{
	if (m_viewSub1)
	{
		return true; 
	}
	return false; 
}

bool MainSegmentWidget::isVisibleHistogram() 
{
	return m_bHistogram; 
}

void MainSegmentWidget::initWindows()
{
	WindowBase* window = nullptr;

	window = static_cast<WindowBase*>(m_viewAxial);
	if (window)
	{
		window->init(m_pDataContext);
	}

	window = static_cast<WindowBase*>(m_viewCoronal);
	if (window)
	{
		window->init(m_pDataContext);
	}

	window = static_cast<WindowBase*>(m_viewSagittal);
	if (window)
	{
		window->init(m_pDataContext);
	}
	}

void MainSegmentWidget::resetResource()
{
	WindowBase* window = nullptr;

	window = static_cast<WindowBase*>(m_viewAxial);
	if (window)
	{
		window->resetResource();
	}

	window = static_cast<WindowBase*>(m_viewCoronal);
	if (window)
	{
		window->resetResource();
	}

	window = static_cast<WindowBase*>(m_viewSagittal);
	if (window)
	{
		window->resetResource();
	}
}

void MainSegmentWidget::setVisibleHistogram(bool val)
{
	m_bHistogram = val;

	if (m_bHistogram)
	{
		if (m_viewPlot)
		{
			m_btnZoom->show();
			m_btnCopy->show();
			m_btnSave->show();
			m_viewPlot->show();
		}

		if (m_bCreate)
		{
			m_bCreate = false;
			//	SetWindowWidthLine();
#ifdef LEVEL_LINE
			SetWindowLevelLine();
#endif
			//	SetVolumeWidthLine();

			SetWidthLine();

			SetWindowRGSlider();

		}
	}
	else
	{
		if (m_btnZoom)
		{
			m_btnZoom->hide();
			m_btnCopy->hide();
			m_btnSave->hide();
			m_viewPlot->hide();
		}
	}
}


void MainSegmentWidget::createHUHisto()
{
	QVector<QPointF> points;
	QVector<QPointF> logpoints;
	static int ComboIndex;

	m_bCreate = true;
	if (m_viewPlot)
	{
		ComboIndex = m_viewPlot->m_WindowCombo->currentIndex();
		m_viewPlot->hide();
		m_btnZoom->hide();
		m_btnCopy->hide();
		m_btnSave->hide();

		disconnect(m_viewPlot->HuRangeSlider, &RangeWidget::sliderPressed, this, &MainSegmentWidget::slot_OnHistogramPressed);
		disconnect(m_viewPlot->HuRangeSlider, &RangeWidget::firstValueChanged, this, &MainSegmentWidget::slot_OnHistogramPressed);
		disconnect(m_viewPlot->HuRangeSlider, &RangeWidget::secondValueChanged, this, &MainSegmentWidget::slot_OnHistogramPressed);
		disconnect(m_viewPlot->actHidden, &QPushButton::clicked, this, &MainSegmentWidget::slot_OnHistogramHide);

		SAFE_DELETE(m_viewPlot->plotArea);
		SAFE_DELETE(m_viewPlot->plotArea2);
		SAFE_DELETE(m_viewPlot->EmptyDark);
		SAFE_DELETE(m_viewPlot->EmptyLight);
		SAFE_DELETE(m_viewPlot->lineWidth);
		SAFE_DELETE(m_viewPlot->HuRangeSlider);
		SAFE_DELETE(m_viewPlot->actHidden);
		SAFE_DELETE(m_viewPlot->lineVolumeWidth);
		SAFE_DELETE(m_viewPlot->lineRange);

		if (m_btnLayout->count() != 0)
		{
			m_btnLayout->removeWidget(m_btnZoom);
			m_btnLayout->removeWidget(m_btnCopy);
			m_btnLayout->removeWidget(m_btnSave);
		}

		if (m_plotLayout->count() != 0)
		{
			m_plotLayout->removeWidget(m_viewPlot);
		}

		if (WIN_MANAGER->mainAnalWidget)
		{
			WIN_MANAGER->mainAnalWidget->removeSharedWidget();
		}

		if (WIN_MANAGER->mainTAWidget)
		{
			WIN_MANAGER->mainTAWidget->removeSharedWidget();
		}
	}
	else
	{
		ComboIndex = COMBO_LIST::CL_3D;
	}


	mint32* AllCount = nullptr;
	mint32* AllCount2 = nullptr;
	int HUMin, HUMax;
	int size;
	float maxCount = 0;
	float maxLogCount = 0;
	int Hu;

	HUMin = m_pDataContext->volume_data.getHuMin();
	HUMax = m_pDataContext->volume_data.getHuMax();
	size = m_pDataContext->volume_data.getVolumeDataLength();

	int AllSize = (HUMax - HUMin);
	AllSize++;

	if (AllSize > 30000)
	{
		AllCount = new mint32[AllSize / 2];
		AllCount2 = new mint32[AllSize / 2];
		memset(AllCount, 0, sizeof(mint32) * (AllSize / 2));
		memset(AllCount2, 0, sizeof(mint32) * (AllSize / 2));
	}
	else
	{
		AllCount = new mint32[AllSize];
		memset(AllCount, 0, sizeof(mint32) * (AllSize));
	}

	for (int i = 0; i < size; i++)
	{
		Hu = m_pDataContext->volume_data.getData(i);

		Hu -= HUMin;

		if (Hu >= 0 && AllSize > Hu)
		{
			if (AllSize > 30000)
			{
				if ((AllSize / 2) > Hu)
					AllCount[Hu]++;
				else
				{
					if ((Hu - (AllSize / 2)) < (AllSize / 2))
						AllCount2[Hu - (AllSize / 2)]++;
				}
			}
			else
			{
				AllCount[Hu]++;
			}
		}
	} //count ÃøÁ¤

	QString _tmpFile = STRING_MANAGER->cacheFilePath + "/Hu.tmp";

	QFile file(_tmpFile);

	if (file.open(QIODevice::WriteOnly))
	{
		if (AllSize > 30000)
		{
			file.write((char*)AllCount, sizeof(mint32) * (AllSize / 2));
			file.write((char*)AllCount2, sizeof(mint32) * (AllSize / 2));
		}
		else
		{
			file.write((char*)AllCount, sizeof(mint32) * AllSize);
		}

		file.close();
	} //hu tmp file write

	SAFE_DELETES(AllCount);
	SAFE_DELETES(AllCount2);
	const int maxrange = 255;
	float count[maxrange + 1];

	memset(count, 0, sizeof(float) * (maxrange + 1));

	float HURange = (float)(HUMax - HUMin) / maxrange;

	for (int i = 0; i < size; i++)
	{
		Hu = m_pDataContext->volume_data.getData(i);

		Hu -= HUMin;

		int index = maxrange * (float)((float)Hu / (HUMax - HUMin));

		if ((index <= maxrange) && (index >= 0))
			count[index]++;
	}

	SAFE_DELETE(m_viewPlot);
	m_viewPlot = new CustomHistogram(this);
	m_viewPlot->plotArea = new QwtPlotCurve();
	m_viewPlot->plotArea2 = new QwtPlotCurve();

	m_viewPlot->setMaxCount(0);


	points.reserve(maxrange + 1);
	logpoints.reserve(maxrange + 1);

	for (int i = 0; i <= maxrange; i++)
	{
		m_viewPlot->addMaxCount(count[i]);

		if (maxCount < count[i])
			maxCount = count[i];

		points.push_back(QPointF((HUMin + (int)(HURange * i)), count[i]));

		if (count[i] != 0)
			count[i] = log10f(count[i]);

		if (maxLogCount < count[i])
			maxLogCount = count[i];
	}

	for (int i = 0; i <= maxrange; i++)
	{
		float val = count[i] / maxLogCount * maxCount;

		logpoints.push_back(QPointF((HUMin + (int)(HURange * i)), val));
	}

	QLinearGradient gradient(QPointF(0, 0), QPointF(1, 0));
	gradient.setColorAt(0.0, QColor(200, 200, 200));
	gradient.setCoordinateMode(QGradient::ObjectBoundingMode);

	m_viewPlot->setChart(points, logpoints);
	points.clear();
	logpoints.clear();

	m_viewPlot->initPlot();

	if (m_btnZoom->isChecked())
	{
		m_viewPlot->setCheckZoom(true);
	}

	QPushButton* btn = nullptr;
	if ((btn = getHisZoomBtn()))
	{
		m_btnLayout->addWidget(btn);
	}

	if ((btn = getHisCopyBtn()))
	{
		m_btnLayout->addWidget(btn);
	}

	if ((btn = getHisSaveBtn()))
	{
		m_btnLayout->addWidget(btn);
	}

	if (getHistogramView())
	{
		m_plotLayout->addWidget(getHistogramView());
	}


	if (WIN_MANAGER->mainTabType == MAINTAB_MEASUREMENT)
	{
		WIN_MANAGER->mainAnalWidget->setSharedWidget();
	}
	else if (WIN_MANAGER->mainTabType == MAINTAB_TA)
	{
		WIN_MANAGER->mainTAWidget->setSharedWidget();
	}
#ifdef LEVEL_LINE
	view->lineLevel = new QWidget(view);
#endif

	m_viewPlot->EmptyDark = new QWidget(m_viewPlot);
	m_viewPlot->EmptyLight = new QWidget(m_viewPlot);
	m_viewPlot->lineWidth = new QWidget(m_viewPlot);
	m_viewPlot->HuRangeSlider = new RangeWidget(Qt::Horizontal, m_viewPlot, true);
	m_viewPlot->actHidden = new QPushButton(m_viewPlot);
	m_viewPlot->actHidden->setStyleSheet(STYLE_MANAGER->buttonNormal);
	m_viewPlot->actHidden->setText("X");
	m_viewPlot->CreateWindowCombo();
	m_viewPlot->lineVolumeWidth = new AlphaColorMap(m_viewPlot, ComboIndex);
	m_viewPlot->lineRange = new QWidget(m_viewPlot);
	m_viewPlot->m_WindowCombo->setCurrentIndex(ComboIndex);

	m_pDataContext->volume_data.updateColorTable();
	WIN_MANAGER->updatePresetTexture(WIN_MANAGER->getSelectedVolumePreset(), WIN_MANAGER->getSelectedCustomPreset(CL_3D));
	WIN_MANAGER->update2DPresetTexture(WIN_MANAGER->getSelectedPreset(), WIN_MANAGER->getSelectedCustomPreset(CL_2D));

	connect(m_viewPlot->HuRangeSlider, &RangeWidget::sliderPressed, this, &MainSegmentWidget::slot_OnHistogramPressed);
	connect(m_viewPlot->HuRangeSlider, &RangeWidget::firstValueChanged, this, &MainSegmentWidget::slot_OnHistogramPressed);
	connect(m_viewPlot->HuRangeSlider, &RangeWidget::secondValueChanged, this, &MainSegmentWidget::slot_OnHistogramPressed);
	connect(m_viewPlot->actHidden, &QPushButton::clicked, this, &MainSegmentWidget::slot_OnHistogramHide);

	if (!m_bHistogram)
	{
		m_btnZoom->hide();
		m_btnCopy->hide();
		m_btnSave->hide();
		m_viewPlot->hide();
	}
	else// if (nullptr == m_leftFullScreen)
	{
		m_btnZoom->show();
		m_btnCopy->show();
		m_btnSave->show();
		m_viewPlot->show();

		SetWidthLine();
		SetWindowRGSlider();
	}
	m_viewPlot->lineVolumeWidth->setAttribute(Qt::WA_TransparentForMouseEvents);
}

void MainSegmentWidget::SetWidthLine()
{
	if (m_viewPlot)
	{
		m_viewPlot->SetWidthLine();
	}
}

QWidget* MainSegmentWidget::getRoiTab() 
{
	return m_tabROI; 
}

CustomHistogram* MainSegmentWidget::getHistogramView() 
{
	if (m_viewPlot)
	{
		return m_viewPlot; 
	}
	else
	{
		return nullptr; 
	}
}
QPushButton* MainSegmentWidget::getHisZoomBtn() 
{
	if (m_btnZoom)
	{
		return m_btnZoom; 
	}
	else
	{
		return nullptr; 
	}
}
QPushButton* MainSegmentWidget::getHisCopyBtn() 
{
	if (m_btnCopy)
	{
		return m_btnCopy; 
	}
	else
	{
		return nullptr; 
	}
}
QPushButton* MainSegmentWidget::getHisSaveBtn() 
{
	if (m_btnSave)
	{
		return m_btnSave; 
	}
	else
	{
		return nullptr; 
	}
}

void MainSegmentWidget::SetWindowWidthLine(bool changePreset)
{
	if (m_viewPlot)
	{
		m_viewPlot->SetWindowWidthLine(changePreset);
	}
}

void MainSegmentWidget::SetVolumeWidthLine(bool changePreset)
{
	if (m_viewPlot)
	{
		m_viewPlot->SetVolumeWidthLine(changePreset);
	}
}


void MainSegmentWidget::SetWindowRGSlider()
{
	if (m_viewPlot)
	{
		int HURange = (m_pDataContext->volume_data.getHuMax() - m_pDataContext->volume_data.getHuMin()) / 2;

		HURange += (m_pDataContext->volume_data.getHuMin());

		QPointF start = m_viewPlot->getGeometry((float)m_pDataContext->volume_data.getHuMin(), 0.0f);
		QPointF end = m_viewPlot->getGeometry((float)m_pDataContext->volume_data.getHuMax(), (float)m_viewPlot->getMaxCount());

		m_viewPlot->HuRangeSlider->setGeometry(start.x(), 0, end.x(), end.y());
		m_viewPlot->HuRangeSlider->setRange(m_pDataContext->volume_data.getHuMin(), m_pDataContext->volume_data.getHuMax());

		if (m_viewPlot->getComboType() == CL_2D)
		{
			SetWindowWidthLine();
		}
		else
		{
			SetVolumeWidthLine();
		}

		m_viewPlot->actHidden->setGeometry(QRect(m_viewPlot->width() - 30, 0, 25, 25));
		m_viewPlot->m_WindowCombo->setGeometry(QRect(m_viewPlot->width() - 125, 0, 90, 25));

		int nLower = 0;
		int nUpper = 0;
		if (WIN_MANAGER->GetTab())
		{
			ThreSholdTab* tab = WIN_MANAGER->GetTab()->getThreSholdTab();

			if (tab)
			{
				nLower = tab->getStartHU();
				nUpper = tab->getEndHU();
			}
		}
		if (nLower < nUpper)
		{
			m_viewPlot->HuRangeSlider->setFirstValue(nLower);
			m_viewPlot->HuRangeSlider->setSecondValue(nUpper);
		}
		else
		{
			if ((HURange / 2) < 30)
			{
				m_viewPlot->HuRangeSlider->setFirstValue(HURange - 30);
				m_viewPlot->HuRangeSlider->setSecondValue(HURange + 30);
			}
			else
			{
				m_viewPlot->HuRangeSlider->setFirstValue(HURange - (HURange / 2));
				m_viewPlot->HuRangeSlider->setSecondValue(HURange + (HURange / 2));
			}
		}
	}
}

void MainSegmentWidget::slot_OnHistogramPressed()
{
	if (WIN_MANAGER->GetTab())
	{
		ThreSholdTab* tab = WIN_MANAGER->GetTab()->getThreSholdTab();
		if (tab)
		{
			if (m_viewPlot->HuRangeSlider->GetFirstPressed())
			{
				tab->setStartHU(m_viewPlot->HuRangeSlider->firstValue());
			}
			else if (m_viewPlot->HuRangeSlider->GetSecondPressed())
			{
				tab->setEndHU(m_viewPlot->HuRangeSlider->secondValue());
			}
		}
	}
}

void MainSegmentWidget::slot_OnHistogramHide()
{
	WIN_MANAGER->mainWindow->OnHistoClick();
}

void MainSegmentWidget::slot_OnZoomClicked()
{
	if (m_btnZoom->isChecked())
	{
		m_btnZoom->setText(tr("Non-log"));
		m_viewPlot->setCheckZoom(true);
	}
	else
	{
		m_btnZoom->setText(tr("Log"));
		m_viewPlot->setCheckZoom(false);
	}
}
void MainSegmentWidget::slot_OnHistogramCopy()
{
	QString valText;

	mint16 HuMin, HuMax;
	mint32 count = 0;

	QString _tmpFile = STRING_MANAGER->cacheFilePath + "/Hu.tmp";

	QFile file(_tmpFile);

	if (!file.open(QIODevice::ReadOnly))
	{
		QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DR_0001)).exec();
		return;
	}

	HuMin = m_pDataContext->volume_data.getHuMin();
	HuMax = m_pDataContext->volume_data.getHuMax();

	valText = QString("%1\tHistogram\tAccumlated Histogram").arg(WIN_MANAGER->getUnitString());

	for (mint16 index = HuMin; index <= HuMax; index++)
	{
		mint32 dt = 0;
		file.read((char*)&dt, sizeof(mint32));

		if (dt > 0)
		{
			count += dt;
		}

		valText.append(QString("\r\n%1\t%2\t%3").arg(index).arg(dt).arg(count));
	}

	auto clip = QApplication::clipboard();
	clip->clear();
	clip->setText(valText);

	file.close();
}

void MainSegmentWidget::slot_OnHistogramSave()
{
	QString strLatest;
	bool latest = WIN_MANAGER->lastestPathGet(strLatest);

	strLatest.remove(strLatest.section(".", -1));
	strLatest.remove(strLatest.size() - 1, 1);

	QFileInfo fileInfo(strLatest);

	QString fileName = ExportFileDialog(
		this,
		STRING_MANAGER->getString(STR_MEDIP_FILE_SAVE),
		"histogram",
		fileInfo.dir().path(),
		tr("CSV File(*.csv;*.CSV)")
	);

	if (!fileName.isEmpty())
	{
		QString valText;

		mint16 HuMin, HuMax;
		mint32 count = 0;

		QString _tmpFile = STRING_MANAGER->cacheFilePath + "/Hu.tmp";

		QFile file(_tmpFile);

		if (!file.open(QIODevice::ReadOnly))
		{
			QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DR_0001)).exec();
			return;
		}

		QFile newFile(fileName);

		if (!newFile.open(QIODevice::WriteOnly))
		{
			file.close();
			QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DU_1002)).exec();
			return;
		}

		HuMin = m_pDataContext->volume_data.getHuMin();
		HuMax = m_pDataContext->volume_data.getHuMax();

		valText = QString("%1,Histogram,Accumlated Histogram\r\n").arg(WIN_MANAGER->getUnitString());
		newFile.write((const char*)valText.toStdWString().c_str(), sizeof(WCHAR) * valText.size());
		for (mint16 index = HuMin; index <= HuMax; index++)
		{
			mint32 dt = 0;
			file.read((char*)&dt, sizeof(mint32));

			if (dt > 0)
				count += dt;

			valText = QString("%1,%2,%3\r\n").arg(index).arg(dt).arg(count);
			newFile.write((const char*)valText.toStdWString().c_str(), sizeof(WCHAR) * valText.size());
		}

		newFile.close();
		file.close();
}
}
void MainSegmentWidget::slot_OnHighlightClicked()
{
#ifdef DEV_VER

	bHighlight = !bHighlight;

	if (!bHighlight)
		btnHighlight->setText("High light Off");
	else
		btnHighlight->setText("High light On");

	if (m_pDataContext->volume_data.isValidate())
		WIN_MANAGER->renderLater_GridView(true);
#endif
}

void MainSegmentWidget::slot_setFullScreen(WindowBase* window)
{
	if (!m_pDataContext->volume_data.isValidate())
	{
		return;
	}

	if (window != nullptr) // Full Screen
	{
		m_tempType = window->getType();

		QWidget* w = window;
		if (w == nullptr)
		{
			return;
		}

		m_screenLayout->removeWidget(m_leftGrid);

		m_screenLayout->removeWidget(m_centerWidgets);
		m_leftGrid->layout()->removeWidget(w);
		m_leftFullScreen = w;
		m_leftGrid->hide();
		m_centerWidgets->hide();

		m_screenLayout->addWidget(m_leftFullScreen);
		m_leftFullScreen->show();
	}
	else
	{
		m_screenLayout->removeWidget(m_leftFullScreen);

		switch (m_tempType)
		{
		case WT_VOLUME:
			((QGridLayout*)m_leftGrid->layout())->addWidget(m_leftFullScreen, 0, 0);
			break;
		case WT_AXIAL:
			((QGridLayout*)m_leftGrid->layout())->addWidget(m_leftFullScreen, 0, 1);
			break;
		case WT_CORONAL:
			((QGridLayout*)m_leftGrid->layout())->addWidget(m_leftFullScreen, 1, 0);
			break;
		case WT_SAGITTAL:
			((QGridLayout*)m_leftGrid->layout())->addWidget(m_leftFullScreen, 1, 1);
			break;
		}

		m_screenLayout->addWidget(m_leftGrid);
		m_screenLayout->addWidget(m_centerWidgets);
		m_leftGrid->show();
		if (m_bSubView)
		{
			m_centerWidgets->show();
		}
		else
		{
			m_centerWidgets->hide();
		}
		m_leftFullScreen = nullptr;
		m_tempType = WT_NONE;
	}
}

void MainSegmentWidget::slot_setFullScreen(OpenGLWidget* window)
{
	if (!m_pDataContext->volume_data.isValidate())
	{
		return;
	}

	if (window != nullptr) // Full Screen
	{
		m_tempType = WT_VOLUME;
		m_screenLayout->removeWidget(m_leftGrid);
		m_screenLayout->removeWidget(m_centerWidgets);
		m_leftGrid->layout()->removeWidget(window);
		m_leftFullScreen = window;
		m_leftGrid->hide();
		m_centerWidgets->hide();

		m_screenLayout->addWidget(m_leftFullScreen);
		m_leftFullScreen->show();
	}
	else
	{
		m_screenLayout->removeWidget(m_leftFullScreen);

		((QGridLayout*)m_leftGrid->layout())->addWidget(m_leftFullScreen, 0, 0);

		m_screenLayout->addWidget(m_leftGrid);
		m_screenLayout->addWidget(m_centerWidgets);

		m_leftGrid->show();
		if (m_bSubView)
		{
			m_centerWidgets->show();
		}
		else
		{
			m_centerWidgets->hide();
		}

		m_leftFullScreen = nullptr;
		m_tempType = WT_NONE;
	}
}

WindowBase* MainSegmentWidget::getWindow(WINDOW_TYPE windowType)
{
	switch (windowType)
	{
	case WT_VOLUME:
		return nullptr;
	case WT_SEGMENT:
		return nullptr;
	case WT_RESULT:
		return nullptr;
	case WT_AXIAL:
		return m_viewAxial;
	case WT_CORONAL:
		return m_viewCoronal;
	case WT_SAGITTAL:
		return m_viewSagittal;
	default:
		return nullptr;
	}
}

WINDOW_TYPE MainSegmentWidget::getFullScreen() 
{
	return m_tempType; 
}

QVBoxLayout* MainSegmentWidget::getTabLayout()
{
	if (m_rightBox)
	{
		return m_rightBox;
	}
	else
	{
		return nullptr;
	}
}

void MainSegmentWidget::slot_OnComboChanged(int index)
{
	if (m_pDataContext->volume_data.isValidate() == false)
	{
		return;
	}

	int preLevel = WIN_MANAGER->getVolumeLevel();
	int preWidth = WIN_MANAGER->getVolumeWidth();
	m_prePreset = m_preset;
	ACTION_MANAGER->action_VolumePreset(preLevel, preWidth, m_prePreset, (SLICE_PRESET)index);
}

void MainSegmentWidget::slot_OnWidthChanged()
{
	if (m_pDataContext->volume_data.isValidate() == false)
	{
		return;
	}

	if (m_slideWidth)
	{
		ACTION_MANAGER->action_Volume_Width(m_slideWidth->value());
	}
}

void MainSegmentWidget::slot_OnLevelChanged()
{
	if (m_pDataContext->volume_data.isValidate() == false)
	{
		return;
	}

	if (m_slideLevel)
	{
		ACTION_MANAGER->action_Volume_Level(m_slideLevel->value());
	}
}

void MainSegmentWidget::slot_OnRenderTypeChanged(int index)
{
	WIN_MANAGER->setRenderType(index);
	WIN_MANAGER->renderLater_GridView(true);
}

void MainSegmentWidget::nextPage()
{
	if (m_pDataContext->volume_data.isValidate() == false)
	{
		return;
	}

	WindowBase* win = getWindow(WIN_MANAGER->getLatestActiveViewType());
	if (win)
	{
		if (win->getType() == WT_AXIAL)
		{
			win->nextDepth();
		}
		else
		{
			win->preDepth();
		}

		win->renderLater();
	}
}

void MainSegmentWidget::prePage()
{
	if (m_pDataContext->volume_data.isValidate() == false)
	{
		return;
	}

	WindowBase* win = getWindow(WIN_MANAGER->getLatestActiveViewType());
	if (win)
	{
		if (win->getType() == WT_AXIAL)
		{
			win->preDepth();
		}
		else
		{
			win->nextDepth();
		}

		win->renderLater();
	}
}

void MainSegmentWidget::setWindowsZoomFactor(float x, float y, float z)
{
	SegmentationView* aView = static_cast<SegmentationView*> (m_viewAxial);
	if (aView)
	{
		aView->InitZoomFactor(x, y);
	}

	SegmentationView* sView = static_cast<SegmentationView*> (m_viewSagittal);
	if (sView)
	{
		sView->InitZoomFactor(y, z);
	}

	SegmentationView* cView = static_cast<SegmentationView*> (m_viewCoronal);
	if (cView)
	{
		cView->InitZoomFactor(x, z);
	}
}

void MainSegmentWidget::resetUI(bool isIsotro)
{
	if (!isIsotro)
	{
		SegmentationView* aView = static_cast<SegmentationView*> (m_viewAxial);
		if (aView)
		{
			aView->resetUI();
		}

		SegmentationView* sView = static_cast<SegmentationView*> (m_viewSagittal);
		if (sView)
		{
			sView->resetUI();
		}

		SegmentationView* cView = static_cast<SegmentationView*> (m_viewCoronal);
		if (cView)
		{
			cView->resetUI();
		}

		VolumeView* volumeView = static_cast<VolumeView*>(m_viewVolume);
		if (volumeView)
		{
			volumeView->resetUI();
		}

		if (WIN_MANAGER->GetTab())
		{
			Visualize2DTab* tab = WIN_MANAGER->GetTab()->get2DTab();
			if (tab)
			{
				tab->resetSlide();
			}

			AITranslationTab* pTransTab = WIN_MANAGER->GetTab()->getAITranslationTab();
			if (pTransTab)
			{
				pTransTab->sig_disablePredictBtn(false);
				pTransTab->sig_checkedPredictBtn(false);
			}
		}

	}
	else
	{
		setBeginScreen(true);
	}

	if (WIN_MANAGER->GetTab())
	{
		SummaryTab* tab = WIN_MANAGER->GetTab()->getSummaryTab();

		if (tab)
		{
			tab->setSummary();
		}

		AISegTab* pAITab = WIN_MANAGER->GetTab()->getAITab();
		if (pAITab)
		{
			AISegTabDeepCatch* pAITabDeepCatch = dynamic_cast<AISegTabDeepCatch*>(pAITab);
			if (pAITabDeepCatch)
			{
				pAITabDeepCatch->SettingFromDicomInfo();
			}
		}
	}

}

float MainSegmentWidget::getAutoScaleSlope()
{
	if (WIN_MANAGER->GetTab())
	{
		Visualize2DTab* tab = WIN_MANAGER->GetTab()->get2DTab();

		if (tab)
			return	tab->getAutoScaleSlope();
	}

	return 0.0f;
}

void MainSegmentWidget::setAutoScaleSlope(float value)
{
	if (WIN_MANAGER->GetTab())
	{
		Visualize2DTab* tab = WIN_MANAGER->GetTab()->get2DTab();

		if (tab)
		{
			tab->setAutoScaleSlope(value);
		}
	}
}

SLICE_PRESET	MainSegmentWidget::getPresetType()
{
	return m_preset; 
}

void MainSegmentWidget::setBeginScreen(bool isIsotro)
{
	VolumeView* vView = static_cast<VolumeView*> (m_viewVolume);

	if (vView)
	{
		vView->resetUI();
	}

	setWindowsZoomFactor(m_pDataContext->volume_data.getSpaceX(true),
		m_pDataContext->volume_data.getSpaceY(true),
		m_pDataContext->volume_data.getSpaceZ(true));

	SegmentationView* aView = static_cast<SegmentationView*> (m_viewAxial);
	if (aView)
	{
		aView->resetSlicePosition();
		aView->resetUI();

		aView->updatePlaneDatas();
	}

	SegmentationView* sView = static_cast<SegmentationView*> (m_viewSagittal);
	if (sView)
	{
		sView->resetSlicePosition();
		sView->resetUI();

		aView->updatePlaneDatas();
	}

	SegmentationView* cView = static_cast<SegmentationView*> (m_viewCoronal);
	if (cView)
	{
		cView->resetSlicePosition();
		cView->resetUI();

		aView->updatePlaneDatas();
	}

	if (!isIsotro)
	{
		if (WIN_MANAGER->GetTab())
		{
			Visualize2DTab* tab = WIN_MANAGER->GetTab()->get2DTab();

			if (tab)
			{
				tab->resetSlide();
			}

			Visualize3DTab* tab2 = WIN_MANAGER->GetTab()->get3DTab();

			if (tab2)
			{
				tab2->resetSlide();
			}
		}
	}
	WIN_MANAGER->forceUpdate2DViewData(true, true);
	renderLater_All();
}

OpenGLWidget* MainSegmentWidget::getViewVolume() 
{
	return m_viewVolume; 
}

void MainSegmentWidget::setVRUpdate(bool value)
{
	if (value == true)
	{
		if (m_vrUpdateTimer == nullptr)
		{
			m_vrUpdateTimer = new QTimer(this);
			connect(m_vrUpdateTimer, &QTimer::timeout, getViewVolume(), QOverload<>::of(&OpenGLWidget::update));
		}

		m_vrUpdateTimer->start(15);
	}
	else
	{
		if (m_vrUpdateTimer != nullptr)
		{
			m_vrUpdateTimer->stop();
		}
	}

	static_cast<VolumeView*>(getViewVolume())->setUpdateFrame(value);
}

void MainSegmentWidget::updateMeshOutline()
{
	SegmentationView* aView = static_cast<SegmentationView*> (m_viewAxial);
	if (aView)
	{
		aView->updateMeshOutline();
	}

	SegmentationView* sView = static_cast<SegmentationView*> (m_viewSagittal);
	if (sView)
	{
		sView->updateMeshOutline();
	}

	SegmentationView* cView = static_cast<SegmentationView*> (m_viewCoronal);
	if (cView)
	{
		cView->updateMeshOutline();
	}

	renderLater_GridView(false);
}
