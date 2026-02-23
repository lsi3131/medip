#include "stdafx.h"
#include "Tabwindow.h"

#include "Actions/ActionManager.h"
#include "ActionCuda.h"
#include "ActionMaskList.h"

#include "UI/MaskList.h"
#include "UI/RangeWidget.h"

#include "System/styleManager.h"
#include "System/resourceManager.h"
#include "System/stringManager.h"
#include "System/ShortcutManager.h"
#include "System/VolumeCalculator.h"
#include "System/FileManager.h"
#include "System/LicenseManager.h"

#include "Main/MainSegmentWidget.h"
#include "Main/MainAnalWidget.h"

#include "MedipQT.h"
#include "windowManager.h"
#include "Windows/VolumeView.h"
#include "Windows/AnalVolumeView.h"
#include "Windows/AnalMPRPlaneView.h"

#include "UI/CustomHistogram.h"
#include "UI/AnalysisWidget.h"


#include "algorithm/FeatureExtractor.h"
#include "algorithm/Radiomics.h"
#include "algorithm/Radiomics.h"
#include "DeepInsthink.h"
#include "Renderer/Renderer.h"

#include "Dialogs/LevelsetDialog.h"
#include "Dialogs/AnnoPathUsageDlg.h"
#include "Dialogs/ProjectDialog.h"
#include "Dialogs/HCCPredictionDlg.h"
#include "Dialogs/PresetDlg.h"
#include "Dialogs/SizeDialog.h"
#include "Dialogs/Infomation/InfoDialog.h"
#include "Dialogs/VisualPrintDlg.h"
#include "Dialogs/InputMessageDlg.h"
#include "Dialogs/SelectSliceDlg.h"

#include "Network/Network.h"
#include "volumedata.h"
#include "CollapseDock.h"

#include "ApplicationManager.h"
#include "Config/ConfigManager.h"

#include "Windows/Main/MainMeshWidget.h"
#include "Windows/MEVolumeView.h"
#include "MeshEdit/CMeshModelViewManager.h"
#include "MeshEdit/CMeshWorkManager.h"
#include "MeshEdit/CPlaneManiplator.h"

#include "UI/VisualPrint/VisualPrintMeshListWidget.h"
#include "Omniverse/OmniverseContext.h"

#include "Tab/MeshTab.h"
#include "Tab/AISegTab.h"
#include "Tab/AISegTabDeepCatch.h"

#include <QColorDialog>
#include <ppltasks.h>

/////////////////////////////////////////////////////////////////////////////////////////

TabWindow::TabWindow(DataContext* pDataContext, QWidget* parent /*= NULL*/) :
	TabWindow(pDataContext, WIN_MANAGER, PRODUCT_MANAGER, ACTION_MANAGER, PRODUCT_FACTORY, PLANE_MANIPULATOR, parent)
{
}

TabWindow::TabWindow(
	DataContext* pDataContext, 
	WindowManager* pWindowManager,
	ProductManager* pProductManager, 
	ActionManager* pActionManager,
	Factory* pProductFactory, 
	CPlaneManiplator* pPlaneManipulator, 
	QWidget* parent) :
	QWidget(parent),
	m_tab2D(nullptr), m_tabImg(nullptr), m_tabSummary(nullptr), m_tab3D(nullptr),
	m_tabROI(nullptr), m_tabVol(nullptr), m_tabDraw(nullptr), m_tabExport(nullptr),
	m_tabAnno(nullptr), m_tabAnimation(nullptr), m_tabThre(nullptr), m_tabMesh2(nullptr),
	m_tabCapture(nullptr), m_tabOption(nullptr), m_tabPatchy(nullptr),
	m_tabAnalysis(nullptr), m_tabAISeg(nullptr), m_tabAITranslation(nullptr), m_tabVisualPrint(nullptr),
	m_tabVisualPrintMesh(nullptr), m_tabAIContrastSynthesis(nullptr), m_tabAIKernelConversion(nullptr),
	m_tabOmniverse(nullptr), m_tabAILowdoseRecon(nullptr)
{
	QVBoxLayout* mainLayout = new QVBoxLayout;

	m_pWindowManager = pWindowManager;
	m_pProductManager = pProductManager;
	m_pActionManager = pActionManager;
	m_pProductFactory = pProductFactory;
	m_pPlaneManipulator = pPlaneManipulator;

	if (!(m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(m_pProductManager->m_strMedipMDBox))
	{
		QHBoxLayout* MDboxBtnLayout = new QHBoxLayout;
		mainLayout->addLayout(MDboxBtnLayout);

		QPushButton* pMoveLeftDogTab = new QPushButton(this);
		pMoveLeftDogTab->setText("<<");
		connect(pMoveLeftDogTab, &QPushButton::clicked, this, &TabWindow::MoveLeft);

		QPushButton* pMoveRightDogTab = new QPushButton(this);
		pMoveRightDogTab->setText(">>");
		connect(pMoveRightDogTab, &QPushButton::clicked, this, &TabWindow::MoveRight);

		MDboxBtnLayout->addWidget(pMoveLeftDogTab);
		MDboxBtnLayout->addWidget(pMoveRightDogTab);
	}

	m_tab2D = new Visualize2DTab(m_pProductManager, this);
	m_tabSummary = new SummaryTab(this);
	m_tab3D = new Visualize3DTab(m_pProductManager, this);
	m_tabROI = new ROITab2(m_pProductManager, this);

#ifdef SUPPORT_PLANNING
	m_tabPlanningROI = new ROITab_Planning(this);
	m_tabPlanningROI->hide();
#endif

	//m_tabThre = PRODUCT_FACTORY->createWidget<ThreSholdTab>(MFL_Common_Segmentation_Threshold_Thresholdbasic, this);
	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Segmentation_Threshold_Thresholdbasic)
		|| m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Segmentation_Regiongrowing_RegionGrowing))
	{
		m_tabThre = new ThreSholdTab(m_pProductManager, this);
	}

	m_tabImg = m_pProductFactory->createWidget<IMGProcessTab>(MFL_Common_VolumeOperation_Filtration, this);
	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_VolumeOperation_Z_Isotropification)
		|| m_pProductManager->IsAvailableFunctionLevel(MFL_Common_VolumeOperation_VolumeFlipping)
		|| m_pProductManager->IsAvailableFunctionLevel(MFL_Common_VolumeOperation_VolumeDownscale)
		|| m_pProductManager->IsAvailableFunctionLevel(MFL_Common_VolumeOperation_VolumeCropping))
	{
		//m_tabVol = m_pProductFactory->createWidget<VOLProcessTab>(MS_COLLAPS_VOLUME_PROCESSING_TAB, this);
		m_tabVol = new VOLProcessTab(this);
	}
	m_tabDraw = m_pProductFactory->createWidget<DrawcutTab>(MFL_Common_Segmentation_Semi_Autosegmentation_DrawCut, this);
	m_tabAnno = m_pProductFactory->createWidget<AnnotationTab2>(MFL_Common_Annotation_Text, this);
	m_tabAnimation = m_pProductFactory->createWidget<AnimationTab>(MFL_Common_Annotation_PathAnimation, this);

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Rendering_MeshTabList))
	{
		m_tabMesh2 = new MeshTab(m_pProductManager, this);
	}
	m_tabCapture = m_pProductFactory->createWidget<CaptureTab>(MFL_Common_Report_ImageManagement_Capture, this);

	//m_tabOption = m_pProductFactory->createWidget<OptionalTab>(MS_COLLAPS_OPTION_TAB, this);
	m_tabOption = new OptionalTab(m_pProductManager, this);

	m_tabAnalysis = m_pProductFactory->createWidget<AnalysisTab>(MFL_Common_Radiomics, this);
	m_tabPatchy = m_pProductFactory->createWidget<WorkingRegionTab>(MFL_Common_Segmentation_Workingregion_Workingregiontablist, this);

	if (m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY) == PRODUCT_NAME_DEEPCATCH)
	{
		m_tabAISeg = new AISegTabDeepCatch(&pDataContext->volume_data, m_pWindowManager, m_pActionManager, m_pProductManager, this);
		//m_tabAISeg = m_pProductFactory->createWidget<AISegTabDeepCatch>(MFL_Product_DeepCatch, this);
	}
	else if (m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY) == PRODUCT_NAME_DEEPCATCH_V2)
	{
		m_tabAISeg = new AISegTabDeepCatch(&pDataContext->volume_data, m_pWindowManager, m_pActionManager, m_pProductManager, this);
		//m_tabAISeg = m_pProductFactory->createWidget<AISegTabDeepCatch>(MFL_Product_DeepCatch_DeepCatchV2, this);
	}
	else
	{
		//m_tabAISeg = m_pProductFactory->createWidget<AISegTab>(MFL_PLUGIN_DEEPDRAW_PACK, this);
		m_tabAISeg = new AISegTab(&pDataContext->volume_data, m_pWindowManager, m_pActionManager, m_pProductManager, this);
	}

#ifdef AI_TRANSLATION
	m_tabAITranslation = new AITranslationTab(this);
#endif	

	m_tabVisualPrint = m_pProductFactory->createWidget<VisualPrintTab>(MFL_Common_VisualPrinting_VisualPrintingservice, this);
	m_tabVisualPrintMesh = m_pProductFactory->createWidget<VisualPrintMeshTab>(MFL_Common_VisualPrinting_BasicMeshEditing, this);

#ifdef AI_IMAGE_ENHANCEMENT
	if (m_pProductManager->IsAvailableFunctionLevel(MFL_DEV_AIcontrastsynthesis))
	{
		m_tabAIContrastSynthesis = new AIContrastSynthesisTab(this);
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_DEV_AIkernelconversion))
	{
		m_tabAIKernelConversion = new AIKernelConversionTab(this);
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_DEV_AILow_doseCTReconstruction))
	{
		m_tabAILowdoseRecon = new AILowdoseCTReconstuctionTab(this);
	}
#endif

#ifdef SUPPORT_PLANNING
	//m_tabPlanning = m_pProductFactory->createWidget<PlannigTab>(MFL_PLANNING_DISPLAY, this);
	m_tabPlanning = new PlannigTab(this);
#endif

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_DEV_NvidiaOmniverse))
	{
		m_tabOmniverse = new OmniverseTab(pDataContext, this);
	}

	if (m_tabMesh2)
	{
		m_tabMesh2->Init(pDataContext, pDataContext->GetOmniverseContext(), m_pPlaneManipulator);
	}

	if (m_tabVisualPrint)
	{
		m_tabVisualPrint->Init(pDataContext, parent);
	}

	if (m_tabVisualPrintMesh)
	{
		m_tabVisualPrintMesh->Init(pDataContext);
	}

	for (auto& tab : GetAllTabList())
	{
		mainLayout->addWidget(tab);
	}

	m_mapCollapseTabList.insert({ MAINTAB_SEGMENTATION , std::vector<CollapseWidget*>() });
	m_mapCollapseTabList.insert({ MAINTAB_MEASUREMENT , std::vector<CollapseWidget*>() });
	m_mapCollapseTabList.insert({ MAINTAB_TA , std::vector<CollapseWidget*>() });
	m_mapCollapseTabList.insert({ MAINTAB_REPORT , std::vector<CollapseWidget*>() });
	m_mapCollapseTabList.insert({ MAINTAB_MESH_EDITING , std::vector<CollapseWidget*>() });

	addMainTabCollapsedTabList(MAINTAB_SEGMENTATION, {
		m_tab2D, m_tab3D, m_tabImg, m_tabVol,
		m_tabDraw, m_tabThre, m_tabROI, m_tabMesh2, m_tabOmniverse,
		m_tabVisualPrint, m_tabPatchy, m_tabAISeg, m_tabAITranslation,
		m_tabAIContrastSynthesis, m_tabAIKernelConversion, m_tabAILowdoseRecon,
		m_tabCapture, m_tabAnno, m_tabAnimation,
		m_tabSummary, m_tabOption
		});

	addMainTabCollapsedTabList(MAINTAB_MEASUREMENT, {
		m_tab2D, m_tab3D, m_tabDraw,
		m_tabROI, m_tabMesh2, m_tabAISeg, m_tabAITranslation,
		m_tabAIContrastSynthesis, m_tabAIKernelConversion, m_tabAILowdoseRecon,
		m_tabCapture, m_tabAnno, m_tabAnimation,
		m_tabSummary, m_tabOption
		});

	addMainTabCollapsedTabList(MAINTAB_TA, {
		m_tab2D, m_tab3D, m_tabROI, m_tabAnalysis, m_tabAITranslation,
		m_tabSummary, m_tabOption,
		});

	addMainTabCollapsedTabList(MAINTAB_REPORT, {
		m_tabROI, m_tabSummary, m_tabOption
		});

	addMainTabCollapsedTabList(MAINTAB_MESH_EDITING, {
		m_tabROI, m_tabMesh2, m_tabOmniverse, m_tabVisualPrint, m_tabOption,
		});


	QWidget* emptyBox0 = new QWidget(this);
	emptyBox0->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

	mainLayout->addWidget(emptyBox0, 1);

	SetMaintabType(m_pWindowManager->mainTabType);

	setLayout(mainLayout);
}

void TabWindow::SetMaintabType(MAINTAB_TYPE type)
{
	if (m_tab2D)
	{
		m_tab2D->showWidgetByTabType(type);
	}

	if (m_tabROI)
	{
		m_tabROI->setMaintabType(type);
	}

	if (m_tabMesh2)
	{
		m_tabMesh2->SetTabType(type);
	}

	for (CollapseWidget* tab : GetAllTabList())
	{
		if (tab)
		{
			tab->hide();
		}
	}

	for (CollapseWidget* tab : GetTabListByMainTabType(type))
	{
		if (tab)
		{
			tab->show();
		}
	}

	if (m_tabAnalysis)
	{
		m_tabAnalysis->updateUI();
	}

#ifdef _M_IX86
	m_tab3D->hide();
	m_tabVol->hide();
	m_tabImg->hide();
	m_tabDraw->hide();
	m_tabThre->hide();
	m_tabPatchy->hide();
	//m_tabROI->hide();

	m_tabAISeg->hide();
	m_tabAITranslation->hide();

#endif
}

void TabWindow::ResetUI()
{
	if (m_tab2D)
	{
		m_tab2D->ResetUI();
	}
}

std::vector<CollapseWidget*> TabWindow::GetAllTabList() const
{
	std::vector<CollapseWidget*> tabList = {
		m_tab2D,
		m_tab3D,
		m_tabImg,
		m_tabVol,
		m_tabAnno,
		m_tabROI,
		m_tabDraw,
		m_tabThre,
		m_tabMesh2,
		m_tabOmniverse,
		m_tabVisualPrint,
		m_tabVisualPrintMesh,
		m_tabAISeg,
		m_tabAITranslation,
		m_tabAIContrastSynthesis,
		m_tabAIKernelConversion,
		m_tabAILowdoseRecon,
		m_tabAnalysis,
		m_tabPatchy,
		m_tabCapture,
		m_tabAnimation,
		m_tabExport,
		m_tabSummary,
		m_tabOption
	};

	std::vector<CollapseWidget*> tabListNotNull;
	for (auto& tab : tabList)
	{
		if (tab)
		{
			tabListNotNull.push_back(tab);
		}
	}

	return tabListNotNull;
}

std::vector<CollapseWidget*> TabWindow::GetTabListByMainTabType(MAINTAB_TYPE type)
{
	auto it = m_mapCollapseTabList.find(type);
	if (it == m_mapCollapseTabList.end())
	{
		return std::vector<CollapseWidget*>();
	}
	return m_mapCollapseTabList[type];
}

Visualize2DTab* TabWindow::get2DTab() 
{
	return m_tab2D; 
}

IMGProcessTab* TabWindow::getImgTab() 
{
	return m_tabImg; 
}

VOLProcessTab* TabWindow::getVolTab()
{
	return m_tabVol; 
}

DrawcutTab* TabWindow::getDrawTab()
{
	return m_tabDraw; 
}

SummaryTab* TabWindow::getSummaryTab()
{
	return m_tabSummary; 
}

Visualize3DTab* TabWindow::get3DTab() 
{
	return m_tab3D; 
}

Export3DTab* TabWindow::getExport3DTab() 
{
	return m_tabExport; 
}

AnnotationTab2* TabWindow::getAnnoTab() 
{
	return m_tabAnno; 
}

AnimationTab* TabWindow::getAniTab() 
{
	return m_tabAnimation; 
}

ThreSholdTab* TabWindow::getThreSholdTab()
{
	return m_tabThre; 
}

ROITab2* TabWindow::getROITab() 
{
	return m_tabROI; 
}

WorkingRegionTab* TabWindow::getPatchyTab()
{
	return m_tabPatchy; 
}

CaptureTab* TabWindow::getCaptureTab() 
{
	return m_tabCapture; 
}

OptionalTab* TabWindow::getOptionalTab() 
{
	return m_tabOption; 
}

AnalysisTab* TabWindow::getAnalysisTab() 
{
	return m_tabAnalysis; 
}

AISegTab* TabWindow::getAITab()
{
	return m_tabAISeg;
}

AITranslationTab* TabWindow::getAITranslationTab() 
{
	return m_tabAITranslation; 
}

AIContrastSynthesisTab* TabWindow::getAIContrastSynthesisTab()
{
	return m_tabAIContrastSynthesis; 
}

AIKernelConversionTab* TabWindow::getAIKernelConversionTab() 
{
	return m_tabAIKernelConversion; 
}

AILowdoseCTReconstuctionTab* TabWindow::getAILowdoseCTReconTab() 
{
	return m_tabAILowdoseRecon;
}

MeshTab* TabWindow::getMeshTab() 
{
	return m_tabMesh2;
}

VisualPrintTab* TabWindow::getVisualPrintTab() 
{
	return m_tabVisualPrint; 
}

VisualPrintMeshTab* TabWindow::getVisualPrintMeshTab() 
{
	return m_tabVisualPrintMesh; 
}

OmniverseTab* TabWindow::getOmniverseTab()
{
	return m_tabOmniverse; 
}

void TabWindow::MoveLeft(void)
{
	CollapseDock* pDock = m_pWindowManager->mainWindow->getTabDock();
	QSize size = pDock->size();

	m_pWindowManager->mainWindow->resizeDocks({ pDock }, { size.width() + 10 }, Qt::Horizontal);
}

void TabWindow::MoveRight(void)
{
	CollapseDock* pDock = m_pWindowManager->mainWindow->getTabDock();
	QSize size = pDock->size();

	m_pWindowManager->mainWindow->resizeDocks({ pDock }, { size.width() - 10 }, Qt::Horizontal);
}

void TabWindow::addMainTabCollapsedTabList(MAINTAB_TYPE type, std::vector<CollapseWidget*> tabList)
{
	for (auto& tab : tabList)
	{
		if (tab)
		{
			m_mapCollapseTabList[type].push_back(tab);
		}
	}
}
