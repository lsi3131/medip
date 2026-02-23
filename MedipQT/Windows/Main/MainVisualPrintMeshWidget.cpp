#include "stdafx.h"
#include "MainVisualPrintMeshWidget.h"

#include "Windows/MEVolumeView.h"
#include "Windows/glwidget.h"
#include "Windows/Tabwindow.h"
#include "Windows/windowManager.h"

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
#include "UI/CollapseDock.h"
#include "UI/CustomHistogram.h"

#include "MedipQT.h"
#include "DataContext.h"
#include <qwt_plot_curve.h>

MainVisualPrintMeshWidget::MainVisualPrintMeshWidget(QWidget* parent)
	: MainTabWidget(parent),
	m_widgetMesh(0),
	m_viewMesh(0),
	m_meshMainLayout(0),
	m_pDataContext(nullptr)
{
	m_bUpdateFinish = false;
}

MainVisualPrintMeshWidget::~MainVisualPrintMeshWidget()
{
	SAFE_DELETE(m_viewMesh)
}

void MainVisualPrintMeshWidget::activate()
{
	WIN_MANAGER->mainWindow->setDockWindowTitle(STRING_MANAGER->getString(STR_MESH_EDITING));

	WIN_MANAGER->mainHLayout->addWidget(this);
	show();
}

void MainVisualPrintMeshWidget::deactivate()
{
	if (!m_dockHidden)
	{
		CollapseDock* dock = WIN_MANAGER->mainWindow->getTabDock();

		if (dock)
			dock->setMinimum(false);
	}

	hide();
	WIN_MANAGER->mainHLayout->removeWidget(this);
}

void MainVisualPrintMeshWidget::loadMesh(int uid, bool reset)
{
	const bool chkRender = WIN_MANAGER->getRenderable();

	if (chkRender)
	{
		WIN_MANAGER->setRenderable(false);
	}

	if (chkRender)
	{
		WIN_MANAGER->setRenderable(true);
		renderLater();
	}
}

mip::MeshCore* MainVisualPrintMeshWidget::getMainMesh()
{
	return nullptr;
}

mip::MATRIX44* MainVisualPrintMeshWidget::getMainMatrix()
{
	return nullptr;
}

bool MainVisualPrintMeshWidget::init(DataContext* pDataContext)
{
	m_pDataContext = pDataContext;

	this->m_meshMainLayout = new QVBoxLayout(this);
	this->m_meshMainLayout->setContentsMargins(0, 0, 0, 0);
	this->m_meshMainLayout->setSpacing(1);

	// Left Windows
	if (createMeshWindow(m_pDataContext) == false)
		return false;

	if (this->m_widgetMesh)
	{
		this->m_meshMainLayout->addWidget(this->m_widgetMesh);

		if (DATA_CONTEXT->volume_data.isValidate())
		{
			//			loadMesh(DATA_CONTEXT->volume_data.GetCurrentMeshIndex(), false);
			loadMesh(DATA_CONTEXT->m_VisualPrinting_MeshData.GetCurrentMeshIndex(), false);
		}

		return true;
	}

	return false;
}

void MainVisualPrintMeshWidget::setWorkMode(VISUAL_PRINT_WORK_MODE mode)
{
	//TODO
	//if (m_viewMesh)
	//	m_viewMesh->setWorkMode(mode);
}

void MainVisualPrintMeshWidget::renderLater()
{
	if (this->m_viewMesh)
		this->m_viewMesh->renderLater();
}

bool MainVisualPrintMeshWidget::createMeshWindow(DataContext* pDataContext)
{
	this->m_viewMesh = new VisualPrintMEVolumeView(pDataContext, this);
	this->m_widgetMesh = m_viewMesh;
	this->m_widgetMesh->setFocusPolicy(Qt::StrongFocus);
	this->m_widgetMesh->setMouseTracking(true);
	//m_viewMesh->reserveInit((HWND)this->m_widgetMesh->winId());

	return true;
}


void MainVisualPrintMeshWidget::resetUI()
{
	if (this->m_viewMesh)
		m_viewMesh->resetUI();

	renderLater();
}

