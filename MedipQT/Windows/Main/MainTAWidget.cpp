#include "stdafx.h"
#include "MainTAWidget.h"
#include "MedipQT.h"

#include "Windows/glwidget.h"
#include "Windows/windowManager.h"
#include "Windows/Main/MainSegmentWidget.h"
#include "Windows/AnalVolumeView.h"
#include "Windows/RadiomicsView.h"

#include "System/stringManager.h"
#include "UI/CustomHistogram.h"
#include "DataContext.h"

MainTAWidget::MainTAWidget(QWidget* parent /* = 0 */) : 
	MainTabWidget(parent)
{
	m_TAMainLayout = nullptr;
	m_gridlayout = nullptr;
	m_screenLayout = nullptr;
	m_histogramLayout = nullptr;
	m_btnLayout = nullptr;
	m_plotLayout = nullptr;

	m_leftGrid = nullptr;
	m_leftFullScreen = nullptr;
	m_viewVolume = nullptr;
	m_viewAxial = nullptr;
	m_viewCoronal = nullptr;
	m_viewSagittal = nullptr;
}

bool MainTAWidget::init(DataContext* pDataContext)
{
	if (pDataContext == nullptr)
	{
		return false;
	}

	m_pDataContext = pDataContext;

	if (nullptr == m_TAMainLayout)
	{
		m_TAMainLayout = new QVBoxLayout(this);
		m_TAMainLayout->setMargin(0);
		m_TAMainLayout->setSpacing(0);

		// Left Windows
		m_leftGrid = nullptr;
		if (createLeftGridWindows(pDataContext) == false)
		{
			return false;
		}

		m_screenLayout = new QHBoxLayout;
		QVBoxLayout* histogramArea = new QVBoxLayout;

		m_histogramLayout = new QHBoxLayout;
		m_btnLayout = new QVBoxLayout;
		m_plotLayout = new QHBoxLayout;
		m_TAMainLayout->addLayout(m_screenLayout);
		m_TAMainLayout->addLayout(histogramArea);
		m_screenLayout->addWidget(m_leftGrid);
		m_histogramLayout->addLayout(m_btnLayout);
		m_histogramLayout->addLayout(m_plotLayout);
		histogramArea->addLayout(m_histogramLayout);
	}
	else
	{
		if (!m_pDataContext->volume_data.isValidate())
		{
			return true;
		}

		initWindows();

		if (m_viewVolume)
		{
			AnalVolumeView* view = dynamic_cast<AnalVolumeView*>(m_viewVolume);
			if (view)
			{
				view->reInit();
			}
		}
	}

	return true;
}

void MainTAWidget::activate()
{
	WIN_MANAGER->mainWindow->setDockWindowTitle(STRING_MANAGER->getString(STR_TA));

	WIN_MANAGER->mainHLayout->addWidget(this);

	setSharedWidget();
	if (WIN_MANAGER->mainSegmentWidget->getHistogramView())
	{
		WIN_MANAGER->mainSegmentWidget->getHistogramView()->AfterFullScreen();
	}

	hideControls();
	if (DATA_CONTEXT->volume_data.isValidate())
	{
		showControls(WIN_MANAGER->getLatestActiveViewType());
	}

	show();


	if (WIN_MANAGER->mainSegmentWidget->getHistogramView())
	{
		WIN_MANAGER->mainSegmentWidget->getHistogramView()->AfterFullScreen();
	}
}

void MainTAWidget::deactivate()
{
	hide();
	WIN_MANAGER->mainHLayout->removeWidget(this);
}

void MainTAWidget::renderLater_All()
{
	renderLater_GridView();
}

void MainTAWidget::renderLater_GridView(bool volumeupdate)
{
	if (m_viewAxial) 
		m_viewAxial->renderLater();

	if (m_viewSagittal) 
		m_viewSagittal->renderLater();

	if (m_viewCoronal)
		m_viewCoronal->renderLater();

	if (m_viewVolume && volumeupdate) 
		m_viewVolume->renderLater();
}

void MainTAWidget::hideControls()
{
	if (m_viewAxial)
	{
		RadiomicsView* view = dynamic_cast<RadiomicsView*>(m_viewAxial);

		if (view)
			view->hideControls();
	}
	if (m_viewSagittal)
	{
		RadiomicsView* view = dynamic_cast<RadiomicsView*>(m_viewSagittal);

		if (view)
			view->hideControls();
	}
	if (m_viewCoronal)
	{
		RadiomicsView* view = dynamic_cast<RadiomicsView*>(m_viewCoronal);

		if (view)
			view->hideControls();
	}
	if (m_viewVolume) m_viewVolume->hideControls();
}

void MainTAWidget::showControls(WINDOW_TYPE windowType)
{
	if (!DATA_CONTEXT->volume_data.isValidate() || !WIN_MANAGER->getRenderable()) 
		return;

	switch (windowType)
	{
	case WT_AXIAL:
		if (m_viewAxial)
		{
			RadiomicsView* view = dynamic_cast<RadiomicsView*>(m_viewAxial);

			if (view)
				view->showControls();
		}
		break;
	case WT_SAGITTAL:
		if (m_viewSagittal)
		{
			RadiomicsView* view = dynamic_cast<RadiomicsView*>(m_viewSagittal);

			if (view)
				view->showControls();
		}
		break;
	case WT_CORONAL:
		if (m_viewCoronal)
		{
			RadiomicsView* view = dynamic_cast<RadiomicsView*>(m_viewCoronal);

			if (view)
				view->showControls();
		}
		break;
	case WT_VOLUME:
		if (m_viewVolume)
			m_viewVolume->showControls();
		break;
	default:
		break;
	}
}

void MainTAWidget::updatePlaneData(WINDOW_TYPE windowType)
{
	if (!DATA_CONTEXT->volume_data.isValidate())
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

void MainTAWidget::updateSliderPosition()
{
	if (!DATA_CONTEXT->volume_data.isValidate())
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

void MainTAWidget::nextPage()
{
	WindowBase* view = getWindow(WIN_MANAGER->getLatestActiveViewType());

	if (view)
		view->nextDepth();
}

void MainTAWidget::prePage()
{
	WindowBase* view = getWindow(WIN_MANAGER->getLatestActiveViewType());

	if (view)
		view->preDepth();
}

WINDOW_TYPE MainTAWidget::getFullScreen()
{
	return m_tempType;
}

void MainTAWidget::resetUI()
{
	setBeginScreen();
}

void MainTAWidget::setWindowsZoomFactor(float x, float y, float z)
{
	RadiomicsView* aView = static_cast<RadiomicsView*> (m_viewAxial);
	if (aView)
	{
		aView->InitZoomFactor(x, y);
	}

	RadiomicsView* sView = static_cast<RadiomicsView*> (m_viewSagittal);
	if (sView)
	{
		sView->InitZoomFactor(y, z);
	}

	RadiomicsView* cView = static_cast<RadiomicsView*> (m_viewCoronal);
	if (cView)
	{
		cView->InitZoomFactor(x, z);
	}
}

void MainTAWidget::setBeginScreen()
{
	if (m_viewVolume)
	{
		AnalVolumeView* view = dynamic_cast<AnalVolumeView*>(m_viewVolume);

		if (view)
		{
			view->resetUI();
		}
	}

	setWindowsZoomFactor(
		DATA_CONTEXT->volume_data.getSpaceX(true),
		DATA_CONTEXT->volume_data.getSpaceY(true),
		DATA_CONTEXT->volume_data.getSpaceZ(true));

	if (m_viewAxial)
	{
		RadiomicsView* view = dynamic_cast<RadiomicsView*>(m_viewAxial);

		if (view)
		{
			view->resetSlicePosition();
			view->resetUI();

			view->updatePlaneDatas();
		}
	}
	if (m_viewCoronal)
	{
		RadiomicsView* view = dynamic_cast<RadiomicsView*>(m_viewCoronal);

		if (view)
		{
			view->resetSlicePosition();
			view->resetUI();

			view->updatePlaneDatas();
		}
	}
	if (m_viewSagittal)
	{
		RadiomicsView* view = dynamic_cast<RadiomicsView*>(m_viewSagittal);

		if (view)
		{
			view->resetSlicePosition();
			view->resetUI();

			view->updatePlaneDatas();
		}
	}

	WIN_MANAGER->forceUpdate2DViewData(true, true);
	renderLater_All();
}

void MainTAWidget::initWindows()
{
	WindowBase* window;

	window = static_cast<WindowBase*>(m_viewAxial);
	if (window) 
		window->init(m_pDataContext);

	window = static_cast<WindowBase*>(m_viewCoronal);
	if (window) 
		window->init(m_pDataContext);

	window = static_cast<WindowBase*>(m_viewSagittal);
	if (window) 
		window->init(m_pDataContext);
}

void MainTAWidget::resetResource()
{
	WindowBase* window;

	window = static_cast<WindowBase*>(m_viewAxial);
	if (window) 
		window->resetResource();

	window = static_cast<WindowBase*>(m_viewCoronal);
	if (window) 
		window->resetResource();

	window = static_cast<WindowBase*>(m_viewSagittal);
	if (window) 
		window->resetResource();
}

void MainTAWidget::updateLayerState()
{
	if (m_viewAxial) 
		m_viewAxial->setDutyCheck();

	if (m_viewSagittal) 
		m_viewSagittal->setDutyCheck();

	if (m_viewCoronal) 
		m_viewCoronal->setDutyCheck();
}

bool MainTAWidget::createLeftGridWindows(DataContext* pDataContext)
{
	AnalVolumeView* volumeView = new AnalVolumeView(pDataContext, this);
	m_viewVolume = volumeView;
	m_viewVolume->setFocusPolicy(Qt::StrongFocus);
	m_viewVolume->setMouseTracking(true);
	volumeView->reserveInit((HWND)m_viewVolume->winId());
	connect(m_viewVolume, SIGNAL(setFullScreen(OpenGLWidget*)), this, SLOT(slot_setFullScreen(OpenGLWidget*)));

	//RadimicsView *axialView = new RadimicsView(WT_AXIAL, this);
	RadiomicsView* axialView = new RadiomicsView(this, WT_AXIAL, pDataContext, &m_viewerSharedInfo);
	m_viewAxial = axialView;
	m_viewAxial->setFocusPolicy(Qt::StrongFocus);
	m_viewAxial->setMouseTracking(true);
	connect(m_viewAxial, SIGNAL(setFullScreen(WindowBase*)), this, SLOT(slot_setFullScreen(WindowBase*)));

	RadiomicsView* coroView = new RadiomicsView(this, WT_CORONAL, pDataContext, &m_viewerSharedInfo);
	m_viewCoronal = coroView;
	m_viewCoronal->setFocusPolicy(Qt::StrongFocus);
	m_viewCoronal->setMouseTracking(true);
	connect(m_viewCoronal, SIGNAL(setFullScreen(WindowBase*)), this, SLOT(slot_setFullScreen(WindowBase*)));

	RadiomicsView* sagittalView = new RadiomicsView(this, WT_SAGITTAL, pDataContext, &m_viewerSharedInfo);
	m_viewSagittal = sagittalView;
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
	m_gridlayout->setSpacing(1);
	m_leftGrid->setLayout(m_gridlayout);

	m_gridlayout->addWidget(m_viewVolume, 0, 0);
	m_gridlayout->addWidget(m_viewAxial, 0, 1);
	m_gridlayout->addWidget(m_viewCoronal, 1, 0);
	m_gridlayout->addWidget(m_viewSagittal, 1, 1);

	initWindows();

	return true;
}

void MainTAWidget::setWorkMode(RADIOMICS_WORK_MODE mode)
{
	RadiomicsView* win = (RadiomicsView*)getWindow(WIN_MANAGER->getLatestActiveViewType());

	/* TODO : Radiomics에서 처리 필요 */
	if (nullptr != win)
	{
		win->setWorkMode(mode);
	}

	if (win != m_viewAxial)
	{
		((RadiomicsView*)m_viewAxial)->setWorkMode(mode);
	}

	if (win != m_viewSagittal)
	{
		((RadiomicsView*)m_viewSagittal)->setWorkMode(mode);
	}

	if (win != m_viewCoronal)
	{
		((RadiomicsView*)m_viewCoronal)->setWorkMode(mode);
	}
}

void MainTAWidget::setSharedWidget()
{
	QPushButton* btn = nullptr;
	if ((btn = WIN_MANAGER->mainSegmentWidget->getHisZoomBtn()))
	{
		m_btnLayout->addWidget(btn);
	}

	if ((btn = WIN_MANAGER->mainSegmentWidget->getHisCopyBtn()))
	{
		m_btnLayout->addWidget(btn);
	}

	if ((btn = WIN_MANAGER->mainSegmentWidget->getHisSaveBtn()))
	{
		m_btnLayout->addWidget(btn);
	}

	if (WIN_MANAGER->mainSegmentWidget->getHistogramView())
	{
		m_plotLayout->addWidget(WIN_MANAGER->mainSegmentWidget->getHistogramView());
	}

	if (m_viewVolume)
	{
		AnalVolumeView* vol = static_cast<AnalVolumeView*>(m_viewVolume);
		WIN_MANAGER->volumeLayerAlphaSet();
		vol->renderLater();
	}
}

void MainTAWidget::removeSharedWidget()
{
	if (m_btnLayout->count() != 0)
	{
		m_btnLayout->removeWidget(WIN_MANAGER->mainSegmentWidget->getHisZoomBtn());
		m_btnLayout->removeWidget(WIN_MANAGER->mainSegmentWidget->getHisCopyBtn());
		m_btnLayout->removeWidget(WIN_MANAGER->mainSegmentWidget->getHisSaveBtn());
	}

	if (m_plotLayout->count() != 0)
	{
		m_plotLayout->removeWidget(WIN_MANAGER->mainSegmentWidget->getHistogramView());
	}
}

WindowBase* MainTAWidget::getWindow(WINDOW_TYPE windowType)
{
	switch (windowType)
	{
	case WT_AXIAL:
		return dynamic_cast<WindowBase*>(m_viewAxial);
	case WT_CORONAL:
		return dynamic_cast<WindowBase*>(m_viewCoronal);
	case WT_SAGITTAL:
		return dynamic_cast<WindowBase*>(m_viewSagittal);
	case WT_VOLUME:
	default:
		return nullptr;
	}
}

AnalVolumeView* MainTAWidget::getViewVolume()
{
	return dynamic_cast<AnalVolumeView*>(m_viewVolume);
}

void MainTAWidget::slot_setFullScreen(WindowBase* window)
{
	if (!DATA_CONTEXT->volume_data.isValidate())
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

		m_screenLayout->removeWidget(m_leftFullScreen);
		m_leftGrid->layout()->removeWidget(w);
		m_leftFullScreen = w;
		m_leftGrid->hide();
		m_leftFullScreen->hide();

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

		m_leftGrid->show();
		m_leftFullScreen = nullptr;
		m_tempType = WT_NONE;
	}
}

void MainTAWidget::slot_setFullScreen(OpenGLWidget* window)
{
	if (!DATA_CONTEXT->volume_data.isValidate())
	{
		return;
	}

	if (window != nullptr)
	{
		m_tempType = window->getType();
		m_screenLayout->removeWidget(m_leftGrid);
		m_leftGrid->layout()->removeWidget(window);
		m_leftFullScreen = window;
		m_leftGrid->hide();
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
		case WT_CORONAL:
			((QGridLayout*)m_leftGrid->layout())->addWidget(m_leftFullScreen, 1, 0);
			break;
		case WT_SAGITTAL:
			((QGridLayout*)m_leftGrid->layout())->addWidget(m_leftFullScreen, 1, 1);
			break;
		case WT_AXIAL:
		default:
			((QGridLayout*)m_leftGrid->layout())->addWidget(m_leftFullScreen, 0, 1);
			break;
		}

		m_screenLayout->addWidget(m_leftGrid);

		m_leftGrid->show();

		m_leftFullScreen = nullptr;
		m_tempType = WT_NONE;
	}
}

void MainTAWidget::setMPRViewInfo(QVector3D volumePos, mint16 HU)
{
	m_viewerSharedInfo.VolumeVoxelPosition = volumePos;
	m_viewerSharedInfo.HU = HU;
}

void MainTAWidget::updateMeshOutline()
{
	RadiomicsView* aView = static_cast<RadiomicsView*> (m_viewAxial);
	if (aView)
	{
		aView->updateMeshOutline();
	}

	RadiomicsView* sView = static_cast<RadiomicsView*> (m_viewSagittal);
	if (sView)
	{
		sView->updateMeshOutline();
	}

	RadiomicsView* cView = static_cast<RadiomicsView*> (m_viewCoronal);
	if (cView)
	{
		cView->updateMeshOutline();
	}

	renderLater_GridView(false);
}
