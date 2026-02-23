#include "stdafx.h"
#include "MainAnalWidget.h"

#include "Windows/AnalVolumeView.h"
#include "Windows/AnalMPRPlaneView.h"
#include "Windows/glwidget.h"
#include "Windows/windowManager.h"
#include "Windows/Main/MainSegmentWidget.h"
#include "Windows/Tabwindow.h"

#include "System/styleManager.h"
#include "System/resourceManager.h"
#include "System/stringManager.h"
#include "Actions/ActionManager.h"

#define ALPHACOLOR
#ifdef ALPHACOLOR
#include "UI/AlphaColorMap.h"
#endif

#include "UI/RangeWidget.h"
#include "UI/MaskList.h"
#include "UI/CustomHistogram.h"

#include "MedipQT.h"
#include "DataContext.h"

#include <qwt_plot_curve.h>

MainAnalWidget::MainAnalWidget(QWidget* parent)
	: MainTabWidget(parent)
{
	m_leftFullScreen = nullptr;
}

MainAnalWidget::~MainAnalWidget()
{
	if (m_leftFullScreen != nullptr)
	{
		SAFE_DELETE(m_leftGrid);
	}
}

void MainAnalWidget::activate()
{
	WIN_MANAGER->mainWindow->setDockWindowTitle(STRING_MANAGER->getString(STR_MEASUREMENT));

	WIN_MANAGER->mainHLayout->addWidget(this);
	setSharedWidget();
	if (WIN_MANAGER->mainSegmentWidget->getHistogramView())
		WIN_MANAGER->mainSegmentWidget->getHistogramView()->AfterFullScreen();

	hideControls();
	if (DATA_CONTEXT->volume_data.isValidate())
	{
		WIN_MANAGER->setSharedPrefer(MAINTAB_SEGMENTATION);
		showControls(WIN_MANAGER->getLatestActiveViewType());
	}

	show();

	WIN_MANAGER->SetEnableViewControls(WIN_MANAGER->IsPredictComplete());
	WIN_MANAGER->updatePlaneData_all();
	if (WIN_MANAGER->mainSegmentWidget->getHistogramView())
		WIN_MANAGER->mainSegmentWidget->getHistogramView()->AfterFullScreen();
}

void MainAnalWidget::deactivate()
{
	hide();
	WIN_MANAGER->mainHLayout->removeWidget(this);
}

bool MainAnalWidget::init(DataContext* pDataContext)
{
	this->m_analMainLayout = new QVBoxLayout(this);
	this->m_analMainLayout->setMargin(0);
	this->m_analMainLayout->setSpacing(0);

	// Left Windows
	this->m_leftGrid = nullptr;
	if (createLeftGridWindows(pDataContext) == false)
		return false;

	m_screenLayout = new QHBoxLayout;
	QVBoxLayout* histogramArea = new QVBoxLayout;

	m_histogramLayout = new QHBoxLayout;
	m_btnLayout = new QVBoxLayout;
	m_plotLayout = new QHBoxLayout;
	this->m_analMainLayout->addLayout(m_screenLayout);
	this->m_analMainLayout->addLayout(histogramArea);
	m_screenLayout->addWidget(this->m_leftGrid);
	m_histogramLayout->addLayout(m_btnLayout);
	m_histogramLayout->addLayout(m_plotLayout);
	histogramArea->addLayout(m_histogramLayout);

	return true;
}

void MainAnalWidget::reInit()
{
	if (this->m_viewAxial)
	{
		AnalMPRPlaneView* view = dynamic_cast<AnalMPRPlaneView*>(this->m_viewAxial);
		if (view)
			view->reInit();
	}
	if (this->m_viewSagittal)
	{
		AnalMPRPlaneView* view = dynamic_cast<AnalMPRPlaneView*>(this->m_viewSagittal);
		if (view)
			view->reInit();
	}
	if (this->m_viewCoronal)
	{
		AnalMPRPlaneView* view = dynamic_cast<AnalMPRPlaneView*>(this->m_viewCoronal);
		if (view)
			view->reInit();
	}
	if (this->m_viewVolume)
	{
		AnalVolumeView* view = dynamic_cast<AnalVolumeView*>(this->m_viewVolume);
		if (view)
			view->reInit();
	}

	if (WIN_MANAGER->GetTab())
	{
		AnnotationTab2* tab = WIN_MANAGER->GetTab()->getAnnoTab();
		if (tab)
			tab->UpdateAnnoList(WIN_MANAGER->mainTabType);
	}
}

void MainAnalWidget::setSharedWidget()
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

	if (this->m_viewVolume)
	{
		AnalVolumeView* vol = static_cast<AnalVolumeView*>(this->m_viewVolume);
		WIN_MANAGER->volumeLayerAlphaSet();
		vol->renderLater();
	}
}

void MainAnalWidget::removeSharedWidget()
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

void MainAnalWidget::setWorkMode(ANAL_WORK_MODE mode)
{
	AnalMPRPlaneView* win = getWindow(WIN_MANAGER->getLatestActiveViewType());

	if (nullptr != win)
	{
		win->setWorkMode(mode);
	}

	if (this->m_viewVolume)
	{
		AnalVolumeView* vView = static_cast<AnalVolumeView*> (this->m_viewVolume);
		vView->setWorkMode(mode);
	}
	if (this->m_viewAxial)
	{
		AnalMPRPlaneView* aView = static_cast<AnalMPRPlaneView*>(this->m_viewAxial);
		aView->setWorkMode(mode);
	}
	if (this->m_viewSagittal)
	{
		AnalMPRPlaneView* sView = static_cast<AnalMPRPlaneView*>(this->m_viewSagittal);
		sView->setWorkMode(mode);
	}
	if (this->m_viewCoronal)
	{
		AnalMPRPlaneView* cView = static_cast<AnalMPRPlaneView*>(this->m_viewCoronal);
		cView->setWorkMode(mode);
	}
}

void MainAnalWidget::changeShapeSize()
{
	AnalMPRPlaneView* win = getWindow(WIN_MANAGER->getLatestActiveViewType());

	if (nullptr != win)
	{
		win->createCursorShape(true);
	}

	if (win != this->m_viewAxial)
	{
		AnalMPRPlaneView* aView = static_cast<AnalMPRPlaneView*>(this->m_viewAxial);
		aView->createCursorShape(true);
	}
	if (win != this->m_viewSagittal)
	{
		AnalMPRPlaneView* sView = static_cast<AnalMPRPlaneView*>(this->m_viewSagittal);
		sView->createCursorShape(true);
	}
	if (win != this->m_viewCoronal)
	{
		AnalMPRPlaneView* cView = static_cast<AnalMPRPlaneView*>(this->m_viewCoronal);
		cView->createCursorShape(true);
	}

}

void MainAnalWidget::renderLater_All()
{
	renderLater_GridView();
}

void MainAnalWidget::renderLater_GridView(bool volumeupdate)
{
	if (this->m_viewAxial)
	{
		this->m_viewAxial->renderLater();
	}

	if (this->m_viewSagittal)
	{
		this->m_viewSagittal->renderLater();
	}

	if (this->m_viewCoronal)
	{
		this->m_viewCoronal->renderLater();
	}

	if (this->m_viewVolume && volumeupdate)
	{
		this->m_viewVolume->renderLater();
	}
}

void MainAnalWidget::hideControls()
{
	if (this->m_viewAxial)
	{
		AnalMPRPlaneView* view = dynamic_cast<AnalMPRPlaneView*>(this->m_viewAxial);

		if (view)
		{
			view->hideControls();
		}
	}
	if (this->m_viewSagittal)
	{
		AnalMPRPlaneView* view = dynamic_cast<AnalMPRPlaneView*>(this->m_viewSagittal);

		if (view)
		{
			view->hideControls();
		}
	}
	if (this->m_viewCoronal)
	{
		AnalMPRPlaneView* view = dynamic_cast<AnalMPRPlaneView*>(this->m_viewCoronal);

		if (view)
		{
			view->hideControls();
		}
	}
	if (this->m_viewVolume)
	{
		this->m_viewVolume->hideControls();
	}
}

void MainAnalWidget::updatePlaneData(WINDOW_TYPE windowType)
{
	if (!DATA_CONTEXT->volume_data.isValidate())
	{
		return;
	}

	AnalMPRPlaneView* view = nullptr;
	switch (windowType)
	{
	case WT_AXIAL:
		view = static_cast<AnalMPRPlaneView*>(this->m_viewAxial);
		break;
	case WT_SAGITTAL:
		view = static_cast<AnalMPRPlaneView*>(this->m_viewSagittal);
		break;
	case WT_CORONAL:
		view = static_cast<AnalMPRPlaneView*>(this->m_viewCoronal);
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

void MainAnalWidget::updatePlaneState()
{
	if (!DATA_CONTEXT->volume_data.isValidate())
	{
		return;
	}

	AnalMPRPlaneView* view = static_cast<AnalMPRPlaneView*>(this->m_viewAxial);
	if (view)
	{
		view->updatePlaneState();
	}

	view = static_cast<AnalMPRPlaneView*>(this->m_viewSagittal);
	if (view)
	{
		view->updatePlaneState();
	}

	view = static_cast<AnalMPRPlaneView*>(this->m_viewCoronal);
	if (view)
	{
		view->updatePlaneState();
	}
}

void MainAnalWidget::showControls(WINDOW_TYPE windowType)
{
	if (!DATA_CONTEXT->volume_data.isValidate() || !WIN_MANAGER->getRenderable())
	{
		return;
	}

	switch (windowType)
	{
	case WT_AXIAL:
		if (this->m_viewAxial)
		{
			AnalMPRPlaneView* view = dynamic_cast<AnalMPRPlaneView*>(this->m_viewAxial);

			if (view)
				view->showControls();
		}
		break;
	case WT_SAGITTAL:
		if (this->m_viewSagittal)
		{
			AnalMPRPlaneView* view = dynamic_cast<AnalMPRPlaneView*>(this->m_viewSagittal);

			if (view)
				view->showControls();
		}
		break;
	case WT_CORONAL:
		if (this->m_viewCoronal)
		{
			AnalMPRPlaneView* view = dynamic_cast<AnalMPRPlaneView*>(this->m_viewCoronal);

			if (view)
				view->showControls();
		}
		break;
	case WT_VOLUME:
		if (this->m_viewVolume) this->m_viewVolume->showControls();
		break;
	default:
		break;
	}

}

bool MainAnalWidget::createLeftGridWindows(DataContext* pDataContext)
{
	AnalVolumeView* volumeView = new AnalVolumeView(pDataContext, this);
	this->m_viewVolume = volumeView;
	this->m_widgetVolume = volumeView;
	this->m_widgetVolume->setFocusPolicy(Qt::StrongFocus);
	this->m_widgetVolume->setMouseTracking(true);
	volumeView->reserveInit((HWND)this->m_widgetVolume->winId());
	connect(this->m_viewVolume, SIGNAL(setFullScreen(OpenGLWidget*)), this, SLOT(slot_setFullScreen(OpenGLWidget*)));

	AnalMPRPlaneView* axialView = new AnalMPRPlaneView(WT_AXIAL, pDataContext, this);
	this->m_viewAxial = axialView;
	this->m_widgetAxial = axialView;
	this->m_widgetAxial->setFocusPolicy(Qt::StrongFocus);
	this->m_widgetAxial->setMouseTracking(true);
	connect(this->m_viewAxial, SIGNAL(setFullScreen(OpenGLWidget*)), this, SLOT(slot_setFullScreen(OpenGLWidget*)));

	AnalMPRPlaneView* coroView = new AnalMPRPlaneView(WT_CORONAL, pDataContext, this);
	this->m_viewCoronal = coroView;
	this->m_widgetCoronal = coroView;
	this->m_widgetCoronal->setFocusPolicy(Qt::StrongFocus);
	this->m_widgetCoronal->setMouseTracking(true);
	connect(this->m_viewCoronal, SIGNAL(setFullScreen(OpenGLWidget*)), this, SLOT(slot_setFullScreen(OpenGLWidget*)));

	AnalMPRPlaneView* sagittalView = new AnalMPRPlaneView(WT_SAGITTAL, pDataContext, this);
	this->m_viewSagittal = sagittalView;
	this->m_widgetSagittal = sagittalView;
	this->m_widgetSagittal->setFocusPolicy(Qt::StrongFocus);
	this->m_widgetSagittal->setMouseTracking(true);
	connect(this->m_viewSagittal, SIGNAL(setFullScreen(OpenGLWidget*)), this, SLOT(slot_setFullScreen(OpenGLWidget*)));

	if (!this->m_leftGrid)
	{
		this->m_leftGrid = new QWidget(this);
	}

	this->m_leftGrid->setContentsMargins(0, 0, 0, 0);
	m_gridlayout = new QGridLayout;
	m_gridlayout->setContentsMargins(0, 0, 0, 0);
	// gridlayout->setMargin(2);
	m_gridlayout->setSpacing(1);
	this->m_leftGrid->setLayout(m_gridlayout);

	m_gridlayout->addWidget(this->m_widgetVolume, 0, 0);
	m_gridlayout->addWidget(this->m_widgetAxial, 0, 1);
	m_gridlayout->addWidget(this->m_widgetCoronal, 1, 0);
	m_gridlayout->addWidget(this->m_widgetSagittal, 1, 1);

	return true;
}

void MainAnalWidget::slot_setFullScreen(OpenGLWidget* window)
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

void MainAnalWidget::nextPage()
{
	AnalMPRPlaneView* view = getWindow(WIN_MANAGER->getLatestActiveViewType());

	if (view)
	{
		view->nextDepth();
	}
}

void MainAnalWidget::prePage()
{
	AnalMPRPlaneView* view = getWindow(WIN_MANAGER->getLatestActiveViewType());

	if (view)
	{
		view->preDepth();
	}
}

void MainAnalWidget::setWindowsZoomFactor(float x, float y, float z)
{

}

void MainAnalWidget::updateUI(bool isSelection, int selection)
{

}

void MainAnalWidget::resetUI()
{
	setBeginScreen();
}

void MainAnalWidget::resetSlide()
{

}

void MainAnalWidget::setBeginScreen()
{
	if (this->m_viewVolume)
	{
		AnalVolumeView* view = dynamic_cast<AnalVolumeView*>(this->m_viewVolume);

		if (view)
		{
			view->resetUI();
		}
	}
	if (this->m_viewAxial)
	{
		AnalMPRPlaneView* view = dynamic_cast<AnalMPRPlaneView*>(this->m_viewAxial);

		if (view)
		{
			view->resetUI();
			view->updatePlaneDatas();
		}
	}
	if (this->m_viewCoronal)
	{
		AnalMPRPlaneView* view = dynamic_cast<AnalMPRPlaneView*>(this->m_viewCoronal);

		if (view)
		{
			view->resetUI();
			view->updatePlaneDatas();
		}
	}
	if (this->m_viewSagittal)
	{
		AnalMPRPlaneView* view = dynamic_cast<AnalMPRPlaneView*>(this->m_viewSagittal);

		if (view)
		{
			view->resetUI();
			view->updatePlaneDatas();
		}
	}

	this->renderLater_All();
}


AnalMPRPlaneView* MainAnalWidget::getWindow(WINDOW_TYPE windowType)
{
	switch (windowType)
	{
	case WT_VOLUME:
		return nullptr;
	case WT_AXIAL:
		return dynamic_cast<AnalMPRPlaneView*>(m_viewAxial);
	case WT_CORONAL:
		return dynamic_cast<AnalMPRPlaneView*>(m_viewCoronal);
	case WT_SAGITTAL:
		return dynamic_cast<AnalMPRPlaneView*>(m_viewSagittal);
	default:
		return nullptr;
	}
}

AnalVolumeView* MainAnalWidget::getViewVolume()
{
	return dynamic_cast<AnalVolumeView*>(m_viewVolume);
}

void MainAnalWidget::updateMeshOutline()
{
	if (this->m_viewAxial)
	{
		AnalMPRPlaneView* view = dynamic_cast<AnalMPRPlaneView*>(this->m_viewAxial);

		if (view)
		{
			view->updateMeshOutline();
		}
	}
	if (this->m_viewCoronal)
	{
		AnalMPRPlaneView* view = dynamic_cast<AnalMPRPlaneView*>(this->m_viewCoronal);

		if (view)
		{
			view->updateMeshOutline();
		}
	}
	if (this->m_viewSagittal)
	{
		AnalMPRPlaneView* view = dynamic_cast<AnalMPRPlaneView*>(this->m_viewSagittal);

		if (view)
		{
			view->updateMeshOutline();
		}
	}
}
