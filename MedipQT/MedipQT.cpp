#pragma once
#include "stdafx.h"
#include "MedipQT.h"

#include "Actions/ActionManager.h"
#include "System/resourceManager.h"
#include "System/stringManager.h"
#include "System/styleManager.h"
#include "System/LicenseManager.h"
#include "System/FileManager.h"
#include "System/ShortcutManager.h"
#include "System/VolumeCalculator.h"
#include "System/ApplicationManager.h"

#include "Windows/AnalMPRPlaneView.h"
#include "Windows/AnalVolumeView.h"

#include "Windows/VolumeView.h"
#include "Windows/SegmentationView.h"

#include "Windows/MEVolumeView.h"
#include "Windows/ResultView.h"

#include "Windows/Tabwindow.h"
#include "Windows/Tab/MeshTab.h"
#include "Windows/Tab/AISegTabDeepCatch.h"

#include "Windows/windowManager.h"
#include "Windows/Main/MainSegmentWidget.h"
#include "Windows/Main/MainMeshWidget.h"
#include "Windows/Main/MainAnalWidget.h"
#include "Windows/Main/MainTAWidget.h"
#include "Windows/ImageManagementTab.h"
#include "Windows/ReportWidget.h"
//#include "System/SelfDump.h"

#include "Network/FTPProtocolOutput.h"
#include "Network/Network.h"

#include "Dialogs/Infomation/InfoDialog.h"
#include "Dialogs/PasswordDialog.h"
#include "Dialogs/SelectSliceDlg.h"

#include "Renderer/Renderer.h"

#include "version.h"

#include "UI/CustomHistogram.h"
#include "UI/CollapseDock.h"
#include "UI/CollapseWidget.h"
#include "UI/MaskList.h"
#include "UI/AnalysisWidget.h"
#include "UI/CustomUI.h"

#include "medipmipencoder.h"
#include "ActionMaskList.h"
#include "ActionVertebra.h"
#include "LogManager.h"

#include "MeshEdit/CMeshViewRenderManager.h"

#include "MeshEdit/CMeshDlgManager.h"
#include "MeshEdit/CMeshWorkManager.h"

#include "DataContext.h"

#include "Omniverse/OmniverseContext.h"
#include "Omniverse/Usd/mipUsdPresetManager.h"

#include <QScrollArea>

MedipQT::MedipQT(QWidget* parent)
	: QMainWindow(parent)
	, m_pDataContext(nullptr)
	, m_pTab(nullptr)
	, m_pToolBtnOpen(nullptr)
	, m_pBtnOpen(nullptr)
	, m_pBtnCrop(nullptr)
	, m_pBtnSave(nullptr)
	, m_pBtnSaveAs(nullptr)
	, m_pBtnServer(nullptr)
	, m_pBtnUndo(nullptr)
	, m_pBtnRedo(nullptr)
	, m_pBtnInfo(nullptr)
	, m_pBtnResetView(nullptr)
	, m_pBtnHomepage(nullptr)
	, m_pBtnCollapseActive(nullptr)
	, m_pBtnCrossSectionView(nullptr)
	, m_pBtnHistogram(nullptr)
	, m_pBtnImageList(nullptr)
	, m_pMenuOpen(nullptr)
	, m_pActOpen(nullptr)
	, m_pActMultiOpen(nullptr)
	, m_pActCancel(nullptr)
	, m_pActCrossSection(nullptr)
	, m_pActHistogram(nullptr)
	, m_pActBreakLock(nullptr)
	, m_pWidgetMain(nullptr)
	, m_pTabBtnSegment(nullptr)
	, m_pTabBtnWeb(nullptr)
	, m_pToolBarFile(nullptr)
	, m_pDockCollapseSeg(nullptr)
	, m_pDockImage(nullptr)
	, m_pReportVolumeVeiwCaptureTimer(nullptr)
	, m_nReportVolumeVeiwCapturesteps(0)
	, m_nReportTabIdx(-1)
{
	m_pWindowManager = WIN_MANAGER;
	m_pLicenseManager = LICENSE_DATA;
	m_pProductManager = PRODUCT_MANAGER;
	m_pProductFactory = PRODUCT_FACTORY;
	m_pActionManager = ACTION_MANAGER;

	/*
	22.02.15 이상일 대리
	MDBOX 납품을 위해 임시로 작성한 코드.
	config.dat 텍스트 파일을 수동으로 수정하여 관리한다.
	** MDBOX 이슈가 생길 경우 define으로 정식으로 관리할 것 **
	*/
	QString MDBOX_mode;
	bool result = m_pWindowManager->getConfigValue(ELEMENT_FILE, STRING_MANAGER->config_MDBOX_Mode, MDBOX_mode);
	if (result == false)
	{
		m_pWindowManager->setConfigValue(ELEMENT_FILE, STRING_MANAGER->config_MDBOX_Mode, "0");
	}
	else
	{
		if (MDBOX_mode.toInt() >= 1)
		{
			QStatusBar* statusBar = new QStatusBar(this);
			statusBar->setSizeGripEnabled(true);

			QString sizeGripImagePath = STRING_MANAGER->resourceDir + "/MEDIP_Icon_StatusBar_GrabResize.png";
			QString sizeGripStyleSheet = QString("QSizeGrip { image: url(%1); width: 48px; height: 48px;}").arg(sizeGripImagePath);
			statusBar->setStyleSheet(sizeGripStyleSheet);
			setStatusBar(statusBar);
		}
	}

	setAcceptDrops(true);
	setStyleSheet(STYLE_MANAGER->mainFrame);
	memset(m_pMainTab, 0, sizeof(MainTabWidget*) * MAINTAB_COUNT);

	m_pImportData = new fm::FileManagerImportData();

	if (!(m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(m_pProductManager->m_strMedipMDBox))
	{
		IconSize = m_pWindowManager->ScreenWidth / 50;
		MainTabSize = m_pWindowManager->ScreenWidth / 70;
	}
	else
	{
		IconSize = m_pWindowManager->ScreenWidth / 60;	// 32
		MainTabSize = m_pWindowManager->ScreenWidth / 76;
	}

#ifdef DEV_MENU_INTERFACE
	QMenu* menuFile = menuBar()->addMenu("&File");

	AddActionToMenuWithFunctionLevel(menuFile, "Open &MIP", SLOT(OnOpenMIPFile()), MFL_Common_Import_MIP);
	AddActionToMenuWithFunctionLevel(menuFile, "Open &DICOM files", SLOT(OnOpenDcmFolder()), MFL_Common_Import_DICOM);
	menuFile->addSeparator();

	AddActionToMenu(menuFile, "Save", SLOT(OnMenuFileSave()));
	AddActionToMenu(menuFile, "Save As", SLOT(OnMenuFileSaveAs()));

	menuFile->addSeparator();
	QMenu* menuImport = menuFile->addMenu("&Import");
	AddActionToMenuWithFunctionLevel(menuImport, "Raw Mask(.raw)", SLOT(OnImportRawMaskFiles()), MFL_Common_Import_NII_RAW_Mask);
	AddActionToMenuWithFunctionLevel(menuImport, "NII Mask(.nii)", SLOT(OnImportNIIMaskFiles()), MFL_Common_Import_NII_RAW_Mask);
	AddActionToMenuWithFunctionLevel(menuImport, "Mesh STL (.stl)", SLOT(OnImportSTLFiles()), MFL_Common_MeshEditing_FileImport);
	AddActionToMenuWithFunctionLevel(menuImport, "Mesh OBJ (.obj)", SLOT(OnImportObjFiles()), MFL_Common_MeshEditing_FileImport);
	AddActionToMenuWithFunctionLevel(menuImport, "Mesh VTK (.vtk)", SLOT(OnImportVTKFiles()), MFL_Common_MeshEditing_FileImport);
	AddActionToMenuWithFunctionLevel(menuImport, "Mesh 3mf (.3mf)", SLOT(OnImport3MFFiles()), MFL_Common_MeshEditing_FileImport);
	AddActionToMenuWithFunctionLevel(menuImport, "Image (.png, .bmp, .jpg, .txt)", SLOT(OnImportImageFiles()), MFL_Common_Report_ImageManagement_ImageImport);
	AddActionToMenuWithFunctionLevel(menuImport, "Rendering Preset (.prd)", SLOT(OnImportCustomRenderingPreset()), MFL_Common_Rendering_2D_3DHistogram_Presetcustomising);

	menuFile->addSeparator();

	//AddActionToMenu(menuFile, "Close Project", SLOT(OnCloseProject())); //TODO : 추후 구현 예정
	AddActionToMenu(menuFile, "Exit", SLOT(OnExit()));

	/* Menu - Edit */
	QMenu* menuEdit = menuBar()->addMenu("&Edit");
	AddActionToMenu(menuEdit, "&Undo", SLOT(OnUndoClick()));
	AddActionToMenu(menuEdit, "&Redo", SLOT(OnRedoClick()));

#endif
}

MedipQT::~MedipQT()
{
	disconnect(SHORTCUT_MANAGER->Action_UndoStack_Undo_Main(), &QAction::changed, this, &MedipQT::OnUndoStateChange);
	disconnect(m_pBtnUndo, &QPushButton::clicked, this, &MedipQT::OnUndoClick);
	disconnect(SHORTCUT_MANAGER->Action_UndoStack_Redo_Main(), &QAction::changed, this, &MedipQT::OnRedoStateChange);
	disconnect(m_pBtnRedo, &QPushButton::clicked, this, &MedipQT::OnRedoClick);

	delete m_pImportData;
	m_pImportData = nullptr;
}

bool MedipQT::createMainMenu()
{
	QString str_val;
	if (m_pWindowManager->getConfigValue(ELEMENT_FILE, STRING_MANAGER->config_undoLimit, str_val))
	{
		m_pWindowManager->setUndoLimit(str_val.toInt());
	}

	m_pActionManager->setUndoStack(new QUndoStack(this));
	connect(m_pActionManager, &ActionManager::sig_actionFinished, this, &MedipQT::slot_actionFinished);
	connect(m_pActionManager, &ActionManager::sig_actionProcessFinished, this, &MedipQT::slot_actionProcessFinished);

	QString areaVal;
	int nArea;

	if (!m_pWindowManager->getConfigValue(ELEMENT_FILE, STRING_MANAGER->config_ToolArea, areaVal))
	{
		areaVal = QString::number(Qt::ToolBarArea::LeftToolBarArea);
	}
	const QIcon winIcon = QIcon::fromTheme("Medip", RESOURCE_MANAGER->getIcon(ICON_APP));
	this->setWindowIcon(winIcon);
	this->setWindowTitle(STRING_MANAGER->getString(STR_MEDIPWINDOW_TITLE));

	nArea = areaVal.toInt();

	setContextMenuPolicy(Qt::NoContextMenu);

	m_pToolBarFile = new QToolBar(this);
	addToolBar((Qt::ToolBarArea)(nArea), m_pToolBarFile);
	m_pToolBarFile->setContextMenuPolicy(Qt::NoContextMenu);
	m_pToolBarFile->setMinimumWidth(40);
	m_pToolBarFile->setOrientation((nArea == Qt::BottomToolBarArea)
		|| (nArea == Qt::TopToolBarArea) ? Qt::Horizontal : Qt::Vertical);
	m_pToolBarFile->setStyleSheet("color: black; border: 0px;");
	connect(m_pToolBarFile, &QToolBar::topLevelChanged, this, &MedipQT::OnBarAreaChange);

#ifndef DEV_FILE_MANAGER
	m_pActOpen = SHORTCUT_MANAGER->Action_Import_Open();
	connect(m_pActOpen, &QAction::triggered, this, &MedipQT::OnMenuFileOpen);
#endif

	if (!m_pLicenseManager->getProductName().compare(PRODUCT_NAME_MEDIP))
	{
		m_pActMultiOpen = SHORTCUT_MANAGER->Action_Import_MultiOpen();
		connect(m_pActMultiOpen, &QAction::triggered, this, &MedipQT::OnMenuFilesOpen);
	}

	m_pActBreakLock = SHORTCUT_MANAGER->Action_Common_BreakLock();
	connect(m_pActBreakLock, &QAction::triggered, this, &MedipQT::OnBreakLock);

	QAction* actVisible = SHORTCUT_MANAGER->Action_Layer_General_Visible();
	connect(actVisible, &QAction::triggered, this, &MedipQT::OnVisibleLayers);

	m_pMenuOpen = new QMenu(this);
	m_pMenuOpen->installEventFilter(this);

	UpdateRecentFile();

	//m_pToolBtnOpen = new QToolButton(this);
#ifdef DEV_FILE_MANAGER
	m_pBtnOpen = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_FILE_NON_OPEN_FILEMANAGER, IconSize, IconSize), "", this);
	m_pBtnOpen->setIconSize(QSize(IconSize, IconSize));
	m_pBtnOpen->setFixedSize(QSize(IconSize, IconSize));
	m_pBtnOpen->setToolTip(STRING_MANAGER->getString(STR_OPEN));
	m_pBtnOpen->setMouseTracking(true);
	m_pBtnOpen->installEventFilter(this);
	m_pBtnOpen->setStyleSheet(STYLE_MANAGER->mainToolBarBtn);
	connect(m_pBtnOpen, &QPushButton::clicked, this, &MedipQT::OnMenuFileManagerOpen);
	m_pToolBarFile->addWidget(m_pBtnOpen);

	m_pActOpen = SHORTCUT_MANAGER->Action_Import_Open();
	connect(m_pActOpen, &QAction::triggered, this, &MedipQT::OnMenuFileManagerOpen);
#else
	m_pToolBtnOpen = new CustomToolButton(this);
	m_pToolBtnOpen->setIcon(RESOURCE_MANAGER->getIcon(ICON_FILE_NON_OPEN, IconSize, IconSize));
	m_pToolBtnOpen->setIconSize(QSize(IconSize, IconSize));
	m_pToolBtnOpen->setFixedSize(QSize(IconSize, IconSize));
	m_pToolBtnOpen->setMenu(m_pMenuOpen);
	m_pToolBtnOpen->setPopupMode(QToolButton::InstantPopup);
	m_pToolBtnOpen->setToolTip(STRING_MANAGER->getString(STR_OPEN));
	m_pToolBtnOpen->setMouseTracking(true);
	m_pToolBtnOpen->installEventFilter(this);
	//	openBtn->setStyleSheet(STYLE_MANAGER->mainToolBarBtn);
	//	connect(openBtn, &QPushButton::clicked, this, &MedipQT::OnMenuFileOpen);
	m_pToolBarFile->addWidget(m_pToolBtnOpen);
#endif

	QAction* reverseAct = SHORTCUT_MANAGER->Action_Import_ReverseOpen();
	connect(reverseAct, &QAction::triggered, this, &MedipQT::OnMenuReverseOpen);

	QAction* flipLayerAct = SHORTCUT_MANAGER->Action_VolumeProcessing_FlipLayer();
	connect(flipLayerAct, &QAction::triggered, this, &MedipQT::OnLayerFlip);

	m_pBtnCrop = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_CROP_NON_OPEN, IconSize, IconSize), "", this);
	m_pBtnCrop->setIconSize(QSize(IconSize, IconSize));
	m_pBtnCrop->setFixedSize(QSize(IconSize, IconSize));
	m_pBtnCrop->setToolTip(STRING_MANAGER->getString(STR_CROP));
	m_pBtnCrop->setMouseTracking(true);
	m_pBtnCrop->installEventFilter(this);
	m_pBtnCrop->setStyleSheet(STYLE_MANAGER->mainToolBarBtn);
	connect(m_pBtnCrop, &QPushButton::clicked, this, &MedipQT::OnMenuCropping);
	m_pToolBarFile->addWidget(m_pBtnCrop);

	m_pBtnSave = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_FILE_NON_SAVE, IconSize, IconSize), "", this);
	m_pBtnSave->setIconSize(QSize(IconSize, IconSize));
	m_pBtnSave->setFixedSize(QSize(IconSize, IconSize));
	m_pBtnSave->setToolTip(STRING_MANAGER->getString(STR_SAVE_FILE));
	m_pBtnSave->setMouseTracking(true);
	m_pBtnSave->installEventFilter(this);
	m_pBtnSave->setCheckable(false);
	m_pBtnSave->setChecked(false);
	m_pBtnSave->setStyleSheet(STYLE_MANAGER->mainToolBarBtn);
	connect(m_pBtnSave, &QPushButton::clicked, this, &MedipQT::OnMenuFileSave);
	m_pToolBarFile->addWidget(m_pBtnSave);

	QAction* saveAct = SHORTCUT_MANAGER->Action_Export_Save();
	connect(saveAct, &QAction::triggered, this, &MedipQT::OnMenuFileSave);

	m_pBtnSaveAs = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_FILE_NON_SAVE_AS, IconSize, IconSize), "", this);
	m_pBtnSaveAs->setIconSize(QSize(IconSize, IconSize));
	m_pBtnSaveAs->setFixedSize(QSize(IconSize, IconSize));
	m_pBtnSaveAs->setToolTip(STRING_MANAGER->getString(STR_SAVE_AS));
	m_pBtnSaveAs->setMouseTracking(true);
	m_pBtnSaveAs->installEventFilter(this);
	m_pBtnSaveAs->setCheckable(false);
	m_pBtnSaveAs->setChecked(false);
	m_pBtnSaveAs->setStyleSheet(STYLE_MANAGER->mainToolBarBtn);
	connect(m_pBtnSaveAs, &QPushButton::clicked, this, &MedipQT::OnMenuFileSaveAs);
	m_pToolBarFile->addWidget(m_pBtnSaveAs);

	QAction* saveAsAct = SHORTCUT_MANAGER->Action_Export_SaveAs();
	connect(saveAsAct, &QAction::triggered, this, &MedipQT::OnMenuFileSaveAs);

#ifdef DEV_FILE_MANAGER
	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Export_PACSUpload))
	{
		m_pBtnUploadToPACS = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_PACS_NON_UPLOAD, IconSize, IconSize), "", this);
		m_pBtnUploadToPACS->setIconSize(QSize(IconSize, IconSize));
		m_pBtnUploadToPACS->setFixedSize(QSize(IconSize, IconSize));
		m_pBtnUploadToPACS->setToolTip(STRING_MANAGER->getString(STR_UPLOAD_PACS));
		m_pBtnUploadToPACS->setMouseTracking(true);
		m_pBtnUploadToPACS->installEventFilter(this);
		m_pBtnUploadToPACS->setCheckable(false);
		m_pBtnUploadToPACS->setChecked(false);
		m_pBtnUploadToPACS->setStyleSheet(STYLE_MANAGER->mainToolBarBtn);
		connect(m_pBtnUploadToPACS, &QPushButton::clicked, this, &MedipQT::OnMenuUploadToPACS);
		m_pToolBarFile->addWidget(m_pBtnUploadToPACS);
	}
#endif

	// 	QAction *saveAsHUAct = new QAction(QIcon(), tr("HU Save &As..."), this);
	// 	saveAsHUAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_3));
	// 	connect(saveAsHUAct, &QAction::triggered, this, &MedipQT::OnMenuFileHUSaveAs);
	// 	addAction(saveAsHUAct);

	m_pBtnUndo = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_UNDO, IconSize, IconSize), "", this);
	m_pBtnUndo->setIconSize(QSize(IconSize, IconSize));
	m_pBtnUndo->setFixedSize(QSize(IconSize, IconSize));
	m_pBtnUndo->setCheckable(false);
	m_pBtnUndo->setMouseTracking(true);
	m_pBtnUndo->installEventFilter(this);

	m_pBtnRedo = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_REDO, IconSize, IconSize), "", this);
	m_pBtnRedo->setIconSize(QSize(IconSize, IconSize));
	m_pBtnRedo->setFixedSize(QSize(IconSize, IconSize));
	m_pBtnRedo->setCheckable(false);
	m_pBtnRedo->setMouseTracking(true);
	m_pBtnRedo->installEventFilter(this);

	SHORTCUT_MANAGER->Create_Action_UndoStack_Undo_Main();
	SHORTCUT_MANAGER->Create_Action_UndoStack_Redo_Main();

	connect(SHORTCUT_MANAGER->Action_UndoStack_Undo_Main(), &QAction::changed, this, &MedipQT::OnUndoStateChange);
	connect(m_pBtnUndo, &QPushButton::clicked, this, &MedipQT::OnUndoClick);
	connect(SHORTCUT_MANAGER->Action_UndoStack_Redo_Main(), &QAction::changed, this, &MedipQT::OnRedoStateChange);
	connect(m_pBtnRedo, &QPushButton::clicked, this, &MedipQT::OnRedoClick);
	m_pToolBarFile->addWidget(m_pBtnUndo);
	m_pToolBarFile->addWidget(m_pBtnRedo);

	m_pBtnResetView = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_RESET, IconSize, IconSize), "", this);
	m_pBtnResetView->setIconSize(QSize(IconSize, IconSize));
	m_pBtnResetView->setFixedSize(QSize(IconSize, IconSize));
	m_pBtnResetView->setCheckable(false);
	m_pBtnResetView->setMouseTracking(true);
	m_pBtnResetView->installEventFilter(this);
	m_pBtnResetView->setToolTip("Reset view");
	m_pBtnResetView->setStyleSheet(STYLE_MANAGER->mainToolBarBtn);

	QAction* beginScreenAct = SHORTCUT_MANAGER->Action_Common_BeginScreen();
	connect(beginScreenAct, &QAction::triggered, this, &MedipQT::OnScreentoBegin);
	connect(m_pBtnResetView, &QPushButton::clicked, this, &MedipQT::OnScreentoBegin);
	m_pToolBarFile->addWidget(m_pBtnResetView);

	// 갯수 만큼 거리벌림
	int nDistanceNum = 10;
	for (int i = 0; i < nDistanceNum; i++)
	{
		m_emptySpace[i] = new QWidget(m_pToolBarFile);
		m_emptySpace[i]->setFixedSize(IconSize, IconSize);
		m_emptySpace[i]->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
		m_pToolBarFile->addWidget(m_emptySpace[i]);
	}

	QAction* pagePreAct = SHORTCUT_MANAGER->Action_Common_PagePrev();
	connect(pagePreAct, &QAction::triggered, this, &MedipQT::OnPrePage);

	QAction* pageNextAct = SHORTCUT_MANAGER->Action_Common_PageNext();
	connect(pageNextAct, &QAction::triggered, this, &MedipQT::OnNextPage);

	QAction* deleteAct = SHORTCUT_MANAGER->Action_Common_Delete();
	connect(deleteAct, &QAction::triggered, this, &MedipQT::OnDelete);

	QAction* escAct = SHORTCUT_MANAGER->Action_Common_ESC();
	connect(escAct, &QAction::triggered, this, &MedipQT::OnEsc);

	QAction* lessAct = SHORTCUT_MANAGER->Action_ManualDrawing_Less();
	connect(lessAct, &QAction::triggered, this, &MedipQT::OnLess);

	QAction* moreAct = SHORTCUT_MANAGER->Action_ManualDrawing_More();
	connect(moreAct, &QAction::triggered, this, &MedipQT::OnMore);

	//#if !defined(DEEP_CATCH_VER) && !defined(COVID19_VER)
	//m_pBtnServer = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_FILE_NON_UPLOAD, IconSize, IconSize), "", this);
	m_pBtnServer = PRODUCT_FACTORY->createWidget<QPushButton>(MFL_Common_Export_MIP_MIPDFileuploader, this);
	if (m_pBtnServer)
	{
		m_pBtnServer->setIcon(RESOURCE_MANAGER->getIcon(ICON_FILE_NON_UPLOAD, IconSize, IconSize));
		m_pBtnServer->setIconSize(QSize(IconSize, IconSize));
		m_pBtnServer->setFixedSize(QSize(IconSize, IconSize));
		m_pBtnServer->setToolTip(STRING_MANAGER->getString(STR_UPLOAD));
		m_pBtnServer->setMouseTracking(true);
		m_pBtnServer->installEventFilter(this);
		m_pBtnServer->setStyleSheet(STYLE_MANAGER->mainToolBarBtn);
		connect(m_pBtnServer, &QPushButton::clicked, this, &MedipQT::OnServerConnect);
		m_pToolBarFile->addWidget(m_pBtnServer);
	}
	m_pBtnCollapseActive = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_TAB, IconSize, IconSize), "", this);
	m_pBtnCollapseActive->setIconSize(QSize(IconSize, IconSize));
	m_pBtnCollapseActive->setFixedSize(QSize(IconSize, IconSize));
	m_pBtnCollapseActive->setCheckable(true);
	m_pBtnCollapseActive->setChecked(true);
	m_pBtnCollapseActive->setMouseTracking(true);
	m_pBtnCollapseActive->installEventFilter(this);
	m_pBtnCollapseActive->setToolTip(STRING_MANAGER->getString(STR_TABLIST_BTN_TOOLTIP));
	m_pBtnCollapseActive->setStyleSheet(STYLE_MANAGER->mainToolBarBtn);
#if !defined(COVID19_VER)
	connect(m_pBtnCollapseActive, &QPushButton::clicked, this, &MedipQT::OnShowTab);
	m_pToolBarFile->addWidget(m_pBtnCollapseActive);
#else
	m_pBtnCollapseActive->setCheckable(false);
	m_pBtnCollapseActive->setChecked(false);
	m_pBtnCollapseActive->hide();
#endif


	//Factory factory	
	//m_pBtnCrossSectionView = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_SVIEW, IconSize, IconSize), "", this);
	m_pBtnCrossSectionView = m_pProductFactory->createWidget<QPushButton>(MFL_Common_Rendering_Windowbasicfunction_CrossectionViewer, this);//new QPushButton(this);
	if (m_pBtnCrossSectionView)
	{
		m_pBtnCrossSectionView->setIcon(RESOURCE_MANAGER->getIcon(ICON_NON_SVIEW, IconSize, IconSize));
		m_pBtnCrossSectionView->setIconSize(QSize(IconSize, IconSize));
		m_pBtnCrossSectionView->setFixedSize(QSize(IconSize, IconSize));
		m_pBtnCrossSectionView->setToolTip(STRING_MANAGER->getString(STR_SVIEW));
		m_pBtnCrossSectionView->setMouseTracking(true);
		m_pBtnCrossSectionView->installEventFilter(this);
		m_pBtnCrossSectionView->setStyleSheet(STYLE_MANAGER->mainToolBarBtn);
#if !defined(COVID19_VER)
		connect(m_pBtnCrossSectionView, &QPushButton::clicked, this, &MedipQT::OnSViewClick);
		m_pActCrossSection = m_pToolBarFile->addWidget(m_pBtnCrossSectionView);
#else
		m_pBtnCrossSectionView->setCheckable(false);
		m_pBtnCrossSectionView->setChecked(false);
		m_pBtnCrossSectionView->hide();
#endif
	}
	//m_pProductFactory->createWidget<QPushButton>(MS_MAIN_TOOL_CROSECTION);
	m_pBtnHistogram = m_pProductFactory->createWidget<QPushButton>(MFL_Common_Rendering_2D_3DHistogram_2D_3DHistogrambasic, this);// new QPushButton(this);
	if (m_pBtnHistogram)
	{
		m_pBtnHistogram->setIcon(RESOURCE_MANAGER->getIcon(ICON_NON_HISTOGRAM, IconSize, IconSize));
		m_pBtnHistogram->setIconSize(QSize(IconSize, IconSize));
		m_pBtnHistogram->setFixedSize(QSize(IconSize, IconSize));
		m_pBtnHistogram->setToolTip(STRING_MANAGER->getString(STR_HISTOGRAM_DESC));
		m_pBtnHistogram->setMouseTracking(true);
		m_pBtnHistogram->installEventFilter(this);
		m_pBtnHistogram->setStyleSheet(STYLE_MANAGER->mainToolBarBtn);
#if !defined(COVID19_VER)
		connect(m_pBtnHistogram, &QPushButton::clicked, this, &MedipQT::OnHistoClick);
		m_pActHistogram = m_pToolBarFile->addWidget(m_pBtnHistogram);
#else
		m_pBtnHistogram->setCheckable(false);
		m_pBtnHistogram->setChecked(false);
		m_pBtnHistogram->hide();
#endif
	}

	m_pBtnImageList = m_pProductFactory->createWidget<QPushButton>(MFL_Common_Report_ImageManagement, this); //new QPushButton(this);
	if (m_pBtnImageList)
	{
		m_pBtnImageList->setIcon(RESOURCE_MANAGER->getIcon(ICON_IMAGE_MANAGEMENT_TAB, IconSize, IconSize));
		m_pBtnImageList->setIconSize(QSize(IconSize, IconSize));
		m_pBtnImageList->setFixedSize(QSize(IconSize, IconSize));
		m_pBtnImageList->setCheckable(true);
		m_pBtnImageList->setChecked(true);
		m_pBtnImageList->setMouseTracking(true);
		m_pBtnImageList->installEventFilter(this);
		m_pBtnImageList->setToolTip(STRING_MANAGER->getString(STR_IMAGELIST_BTN_TOOLTIP));
		m_pBtnImageList->setStyleSheet(STYLE_MANAGER->mainToolBarBtn);
#if !defined(COVID19_VER)
		connect(m_pBtnImageList, &QPushButton::clicked, this, &MedipQT::OnShowImageList);
		m_pToolBarFile->addWidget(m_pBtnImageList);
#else
		m_pBtnImageList->setCheckable(false);
		m_pBtnImageList->setChecked(false);
		m_pBtnImageList->hide();
#endif
	}

	QWidget* empty02 = new QWidget(m_pToolBarFile);
	//empty02->setFixedSize(IconSize, m_nEmpty02Height);
	empty02->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	m_pToolBarFile->addWidget(empty02);

	//#endif // COVID19

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Rendering_3DVolumeViewer))
	{
		QAction* hpAAct = SHORTCUT_MANAGER->Action_VolumeHomePosition_Anterior();
		connect(hpAAct, &QAction::triggered, this, &MedipQT::OnHPA);

		QAction* hpPAct = SHORTCUT_MANAGER->Action_VolumeHomePosition_Posterior();
		connect(hpPAct, &QAction::triggered, this, &MedipQT::OnHPP);

		QAction* hpRAct = SHORTCUT_MANAGER->Action_VolumeHomePosition_Right();
		connect(hpRAct, &QAction::triggered, this, &MedipQT::OnHPR);

		QAction* hpLAct = SHORTCUT_MANAGER->Action_VolumeHomePosition_Left();
		connect(hpLAct, &QAction::triggered, this, &MedipQT::OnHPL);

		QAction* hpSAct = SHORTCUT_MANAGER->Action_VolumeHomePosition_Superior();
		connect(hpSAct, &QAction::triggered, this, &MedipQT::OnHPS);

		QAction* hpIAct = SHORTCUT_MANAGER->Action_VolumeHomePosition_Inferior();
		connect(hpIAct, &QAction::triggered, this, &MedipQT::OnHPI);
	}

	QAction* InfoAct = SHORTCUT_MANAGER->Action_Common_Info();
	connect(InfoAct, &QAction::triggered, this, &MedipQT::OnInfoClick);
	//fileToolBar->addAction(webAct);

	m_pBtnHomepage = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_LINK, IconSize, IconSize), "", this);
	m_pBtnHomepage->setIconSize(QSize(IconSize, IconSize));
	m_pBtnHomepage->setFixedSize(QSize(IconSize, IconSize));
	m_pBtnHomepage->setToolTip(STRING_MANAGER->getString(STR_INFO_DESC));
	m_pBtnHomepage->setMouseTracking(true);
	m_pBtnHomepage->installEventFilter(this);
	m_pBtnHomepage->setToolTip(STRING_MANAGER->getString(STR_CONNECT_HOMEPAGE));
	m_pBtnHomepage->setStyleSheet(STYLE_MANAGER->mainToolBarBtn);
	connect(m_pBtnHomepage, &QPushButton::clicked, this, &MedipQT::OnLinkClick);
	m_pToolBarFile->addWidget(m_pBtnHomepage);

	m_pBtnInfo = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_APP_INFO, IconSize, IconSize), "", this);
	m_pBtnInfo->setIconSize(QSize(IconSize, IconSize));
	m_pBtnInfo->setFixedSize(QSize(IconSize, IconSize));
	m_pBtnInfo->setToolTip(STRING_MANAGER->getString(STR_INFO_DESC));
	m_pBtnInfo->setMouseTracking(true);
	m_pBtnInfo->installEventFilter(this);
	m_pBtnInfo->setStyleSheet(STYLE_MANAGER->mainToolBarBtn);
	connect(m_pBtnInfo, &QPushButton::clicked, this, &MedipQT::OnInfoClick);
	m_pToolBarFile->addWidget(m_pBtnInfo);

	connect(SHORTCUT_MANAGER->Action_ManualDrawing_PolygonSelection(), &QAction::triggered, this, &MedipQT::OnPolyROI);
	connect(SHORTCUT_MANAGER->Action_ManualDrawing_RegionSelection(), &QAction::triggered, this, &MedipQT::OnFreeDrawROI);
	connect(SHORTCUT_MANAGER->Action_ManualDrawing_PixelWisedSelection(), &QAction::triggered, this, &MedipQT::OnPickerROI);
	connect(SHORTCUT_MANAGER->Action_ManualDrawing_OvalSelection(), &QAction::triggered, this, &MedipQT::OnOvalROI);
	connect(SHORTCUT_MANAGER->Action_ManualDrawing_AngularSelection(), &QAction::triggered, this, &MedipQT::OnRectROI);

	connect(SHORTCUT_MANAGER->Action_WorkingRegion(), &QAction::triggered, this, &MedipQT::OnShowBounding);
	connect(SHORTCUT_MANAGER->Action_WindowBasic_BoundaryViewer(), &QAction::triggered, this, &MedipQT::OnShowBoundaryEdge);

	mip::DcmtkVolumeReader::SetEnableCP246Support(false);

	QAction* pRenameAction = SHORTCUT_MANAGER->Action_Layer_General_Rename();
	if (pRenameAction)
	{
		connect(pRenameAction, &QAction::triggered, this, &MedipQT::OnRename);
	}

	return true;
}

bool MedipQT::createMainStatusBar()
{
	QStatusBar* statusBar = new QStatusBar(this);
	statusBar->setObjectName(QStringLiteral("statusBar"));
	statusBar->setContentsMargins(1, 1, 1, 1);
	statusBar->setMinimumHeight(21);

	QProgressBar* bar = new QProgressBar(statusBar);
	bar->setGeometry(800, 0, 200, 20);
	bar->setMaximum(100);
	bar->setMinimum(0);
	bar->setValue(50);
	bar->setOrientation(Qt::Horizontal);

	m_pWindowManager->statusBar = statusBar;
	statusBar->showMessage(STRING_MANAGER->getString(STR_MESSAGE_FILE_LOAD_FAILED));
	setStatusBar(statusBar);

	return true;
}

bool MedipQT::createMainOpenGLContext()
{
	QSurfaceFormat format;
	format.setDepthBufferSize(24);
	format.setStencilBufferSize(8);
	QSurfaceFormat::setDefaultFormat(format);

	return true;
}

int MedipQT::init(DataContext* pDataContext)
{
	m_pDataContext = pDataContext;

	OmniverseContext* pOmnverse = m_pDataContext->GetOmniverseContext();
	pOmnverse->GetStage()->SetMeshData(&m_pDataContext->m_MeshData);

	if (createMainOpenGLContext() == false)
	{
		return FALSE;
	}

	m_pWindowManager->setAdvancedConfig();

	if (objectName().isEmpty())
	{
		setObjectName(QStringLiteral("MedipQTClass"));
	}
	resize(1024, 768);
	//this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
	QApplication::setFont(RESOURCE_MANAGER->fontNormal);
	QApplication::setWindowIcon(RESOURCE_MANAGER->getIcon(ICON_APP));
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

	if (m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY) == PRODUCT_NAME_DEEPCATCH
		|| m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY) == PRODUCT_NAME_DEEPCATCH_V2)
	{
		QString expertReportPath;
		if (m_pWindowManager->getConfigValue(ELEMENT_FILE, STRING_MANAGER->config_expertReportPath, expertReportPath))
		{
			if (STRING_MANAGER->expertReportFilePath != expertReportPath)
			{
				STRING_MANAGER->expertReportFilePath = expertReportPath;
			}
		}
	}

	//todo recent files
	HKEY recentKey = 0;
	LONG ret;
	if (!m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY).compare(m_pProductManager->m_strMedipDeepCatch)
		|| !m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY).compare(m_pProductManager->m_strMedipDeepCatchV2))

	{
		ret = RegCreateKeyEx(HKEY_CURRENT_USER, (L"Software\\Medicalip\\DeepCatch\\Recent files"),
			0, nullptr, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr, &recentKey, nullptr);
	}
	else // MEDIP
	{
		ret = RegCreateKeyEx(HKEY_CURRENT_USER, (L"Software\\Medicalip\\MEDIP\\Recent files"),
			0, nullptr, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr, &recentKey, nullptr);
	}

	if (ret == ERROR_SUCCESS)
	{
		wchar_t filePath[MAX_PATH];
		DWORD pathSize;
		for (int i = 1; i <= RECENT_MAX; i++)
		{
			m_strRecent[i - 1] = QString();

			QString recentFile = QString("File%1\0").arg(i);
			pathSize = sizeof(filePath);
			memset(filePath, 0, pathSize);

			ret = RegGetValue(recentKey, nullptr, (const wchar_t*)recentFile.utf16(),
				RRF_RT_REG_SZ, nullptr, filePath, &pathSize);

			if (ret == ERROR_SUCCESS)
				m_strRecent[i - 1] = QString::fromWCharArray(filePath);
		}

		RegCloseKey(recentKey);
	}

	/* 단축키 초기화 전, ActionManager Undo Stack 초기화(추후 의존성 변경할 것)*/
	m_pActionManager->setUndoStack(new QUndoStack(this));

	/* 단축키 관련 Action 초기화*/
	if (SHORTCUT_MANAGER->Initialize(this) == false)
	{
		return FALSE;
	}

	if (createMainMenu() == false)
	{
		return FALSE;
	}

	/*if (createMainStatusBar() == false)
	{
	return false;
	}*/
	m_pWindowManager->mainWindow = this;
	m_pWindowManager->mainCenterWidget = new QWidget(this);
	m_pWindowManager->mainHLayout = new QHBoxLayout(m_pWindowManager->mainCenterWidget);
	m_pWindowManager->mainHLayout->setMargin(0);
	m_pWindowManager->mainHLayout->setSpacing(0);
	if (1) // add tab
	{
		int nTabIdx = -1;
		m_pTab = new QTabBar(m_pWindowManager->mainCenterWidget);
		m_pTab->setShape(QTabBar::RoundedWest);

		//if (m_pProductManager->IsAvailableFunctionLevel(eMEDIP_FUNCTION_LEVEL::MS_MAIN_TAB_SEGMENTATION) == eAVAILABLE_STATE::CREATE)
		{
			m_pTab->addTab(STRING_MANAGER->getString(STR_SEGMENT));
			m_pTab->setTabData(++nTabIdx, QVariant(MAINTAB_SEGMENTATION));
		}

		if (m_pProductManager->IsAvailableFunctionLevel(eMEDIP_FUNCTION_LEVEL::MFL_Common_Measurement_GeneralFunction) == eAVAILABLE_STATE::CREATE)
		{
			m_pTab->addTab(STRING_MANAGER->getString(STR_MEASUREMENT));
			m_pTab->setTabData(++nTabIdx, QVariant(MAINTAB_MEASUREMENT));
		}

		if (m_pProductManager->IsAvailableFunctionLevel(eMEDIP_FUNCTION_LEVEL::MFL_Common_Radiomics) == eAVAILABLE_STATE::CREATE)
		{
			m_pTab->addTab(STRING_MANAGER->getString(STR_TA));
			m_pTab->setTabData(++nTabIdx, QVariant(MAINTAB_TA));
		}

		if (m_pProductManager->IsAvailableFunctionLevel(eMEDIP_FUNCTION_LEVEL::MFL_Common_Report_GeneralFunction) == eAVAILABLE_STATE::CREATE)
		{
			m_pTab->addTab(STRING_MANAGER->getString(STR_REPORT));
			m_pTab->setTabData(++nTabIdx, QVariant(MAINTAB_REPORT));
			m_nReportTabIdx = nTabIdx;
		}

		if (m_pProductManager->IsAvailableFunctionLevel(eMEDIP_FUNCTION_LEVEL::MFL_Common_MeshEditing_GeneralFunction) == eAVAILABLE_STATE::CREATE)
		{
			m_pTab->addTab(STRING_MANAGER->getString(STR_MESH_EDITING));
			m_pTab->setTabData(++nTabIdx, QVariant(MAINTAB_MESH_EDITING));
		}

#ifdef SUPPORT_PLANNING
		//20211101_byPHS_Planning
		//if (m_pProductManager->IsAvailableFunctionLevel(eMEDIP_FUNCTION_LEVEL::MFL_PLANNING_GENEREL) == eAVAILABLE_STATE::CREATE)
		{
			m_pTab->addTab(STRING_MANAGER->getString(STR_PLANNING));
			m_pTab->setTabData(++nTabIdx, QVariant(MAINTAB_PLANNING));

			m_nPlanningTabIdx = nTabIdx;
		}
#endif

		//tab->addTab(STRING_MANAGER->getString(STR_REPORT));
		m_pTab->setLayoutDirection(Qt::LayoutDirectionAuto);
		m_pTab->setTabButton(0, QTabBar::LeftSide, nullptr);
		m_pTab->setTabButton(1, QTabBar::LeftSide, nullptr);
		//tab->setTabButton(2, QTabBar::LeftSide, nullptr);
		//tab->setTabButton(3, QTabBar::LeftSide, nullptr);
		//	tab->setMaximumHeight(500);
		m_pTab->setMaximumWidth(20);
		m_pTab->setStyleSheet(STYLE_MANAGER->leftTabBar + QString("QTabBar:tab::selected { width:%1;} QTabBar:tab::!selected{ width : %1; }").arg(MainTabSize));
		m_pTab->setFixedWidth(MainTabSize);
		m_pTab->setCurrentIndex(MAINTAB_SEGMENTATION);
		m_pTab->installEventFilter(this);
		m_pTab->setObjectName("Maintab");

		connect(m_pTab, &QTabBar::currentChanged, this, &MedipQT::OnMainTabChanged);
		m_pWindowManager->mainHLayout->addWidget(m_pTab);

#ifdef _M_IX86
		m_pTab->setTabEnabled(MAINTAB_MEASUREMENT, false);
		m_pTab->setTabEnabled(MAINTAB_TA, false);
#endif
	}

	MainSegmentWidget* segmentMain = new MainSegmentWidget(this);
	//MainSegmentWidget * segmentMain = m_pProductFactory->createWidget<MainSegmentWidget>(MS_MAIN_TAB_SEGMENTATION, this);
	m_pWindowManager->mainSegmentWidget = segmentMain;
	m_pMainTab[MAINTAB_SEGMENTATION] = segmentMain;
	setCentralWidget(m_pWindowManager->mainCenterWidget);

	QString dockArea, dock2Area;
	QString dockTop;
	QString dockShow;
	QString imgShow;

	if (!m_pWindowManager->getConfigValue(ELEMENT_FILE, STRING_MANAGER->config_SegDockArea, dockArea))
		dockArea = QString::number(Qt::DockWidgetArea::RightDockWidgetArea);

	if (!m_pWindowManager->getConfigValue(ELEMENT_FILE, STRING_MANAGER->config_RenderDockArea, dock2Area))
		dock2Area = QString::number(Qt::DockWidgetArea::RightDockWidgetArea);

	if (!m_pWindowManager->getConfigValue(ELEMENT_FILE, STRING_MANAGER->config_DockTop, dockTop))
		dockTop = QString::number(TRUE);

	if (!m_pWindowManager->getConfigValue(ELEMENT_FILE, STRING_MANAGER->config_DockShow, dockShow))
		dockShow = QString::number(TRUE);

	if (!m_pWindowManager->getConfigValue(ELEMENT_FILE, STRING_MANAGER->config_imageListShow, imgShow))
		imgShow = QString::number(TRUE);

	// 기본으로 수정
	m_pDockCollapseSeg = new CollapseDock(this); // "Tab list"
	//m_pDockCollapseSeg = m_pProductFactory->createWidget<CollapseDock>(MS_MAIN_TOOL_DOCK_COLLAPSE, this); //new CollapseDock(this);
	if (m_pDockCollapseSeg)
	{
		m_pDockCollapseSeg->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
		m_pDockCollapseSeg->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
		m_pDockCollapseSeg->setStyleSheet(STYLE_MANAGER->rightDockingWidget);
		m_pDockCollapseSeg->setWindowTitle(STRING_MANAGER->getString(STR_SEGMENT));
		m_pDockCollapseSeg->setAttribute(Qt::WA_AcceptTouchEvents);

		addDockWidget((Qt::DockWidgetArea)dockArea.toInt(), m_pDockCollapseSeg);
		QRect rect = QDesktopWidget().availableGeometry(this);
		resizeDocks({ m_pDockCollapseSeg }, { rect.width() / 5 }, Qt::Horizontal);

		connect(m_pDockCollapseSeg, &QDockWidget::topLevelChanged, this, &MedipQT::OnSegDockFloating);
		connect(m_pDockCollapseSeg, &QDockWidget::dockLocationChanged, this, &MedipQT::OnSegDockAreaChange);
	}

	if (segmentMain && segmentMain->init(m_pDataContext) == false)
	{
		SAFE_DELETE(segmentMain);
		return FALSE;
	}

	if (segmentMain)
	{
		m_pWindowManager->mainHLayout->addWidget(segmentMain);
	}

	m_pWindowManager->mainTabType = MAINTAB_SEGMENTATION;

	if (m_pWindowManager->mainAnalWidget == nullptr)
	{
		m_pWindowManager->mainAnalWidget = m_pProductFactory->createWidget<MainAnalWidget>(MFL_Common_Measurement_GeneralFunction, this);
		m_pMainTab[MAINTAB_MEASUREMENT] = m_pWindowManager->mainAnalWidget;
		if (m_pWindowManager->mainAnalWidget)
		{
			m_pWindowManager->mainAnalWidget->init(m_pDataContext);
			m_pWindowManager->mainAnalWidget->setBeginScreen();
			m_pMainTab[MAINTAB_MEASUREMENT]->deactivate();
		}
	}

	if (m_pWindowManager->mainTAWidget == nullptr)
	{
		m_pWindowManager->mainTAWidget = m_pProductFactory->createWidget<MainTAWidget>(MFL_Common_Radiomics, this);
		m_pMainTab[MAINTAB_TA] = m_pWindowManager->mainTAWidget;

		if (m_pWindowManager->mainTAWidget)
		{
			m_pWindowManager->mainTAWidget->init(m_pDataContext);
			m_pMainTab[MAINTAB_TA]->deactivate();
		}
	}

	if (m_pWindowManager->mainMeshWidget == nullptr)
	{
		//m_pWindowManager->mainMeshWidget = new MainMeshWidget(this);
		m_pWindowManager->mainMeshWidget = m_pProductFactory->createWidget<MainMeshWidget>(MFL_Common_MeshEditing_GeneralFunction, this);
		m_pMainTab[MAINTAB_MESH_EDITING] = m_pWindowManager->mainMeshWidget;
		if (m_pWindowManager->mainMeshWidget)
		{
			m_pWindowManager->mainMeshWidget->Init(m_pDataContext);
			m_pMainTab[MAINTAB_MESH_EDITING]->deactivate();
		}
	}

	QString strVal;

	int index = 0;
	if (m_pWindowManager->getConfigValue(ELEMENT_FILE, STRING_MANAGER->config_cusPreset, strVal))
	{
		QStringList names = strVal.split(":");

		for (index = 0; index < names.size(); index++)
		{
			m_pWindowManager->setPresetName(names.at(index), index, true);
		}
	}

#ifdef DEV_USE_APPDATA_PATH
	if (m_pWindowManager->getConfigValue(ELEMENT_FILE, STRING_MANAGER->config_cusPreset_AppData, strVal))
	{
		QStringList names = strVal.split(":");

		for (int i = 0; i < names.size(); i++)
		{
			m_pWindowManager->setPresetName(names.at(i), index++, true, true);
		}
	}
#endif

	if (nullptr == m_pWindowManager->GetTab())
	{
		QScrollArea* scrollarea = new QScrollArea(this);
		scrollarea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		scrollarea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		scrollarea->setWidgetResizable(true);
		m_pWindowManager->SetTab(new TabWindow(m_pDataContext, this));
		m_pWindowManager->GetTab()->setAttribute(Qt::WA_AcceptTouchEvents);
		scrollarea->setWidget(m_pWindowManager->GetTab());
		scrollarea->setAttribute(Qt::WA_AcceptTouchEvents);
		scrollarea->setStyleSheet(STYLE_MANAGER->rightDockingScrollArea);
		if (m_pDockCollapseSeg) m_pDockCollapseSeg->setWidget(scrollarea);

		//if (m_pWindowManager->GetTab()->getVisualPrintTab()->CheckVisualPrintDlgCreated() == false)
		//	//		if(m_pWindowManager->GetTab()->getVisualPrintTab()->GetVisualPrintDlg()->GetVisualPrintMeshTabCreate())
		//{
		//	// For CT Plane Preview			
		//	m_pWindowManager->GetTab()->getVisualPrintTab()->CreateVisualPrintDlg(this);
		//}

		/* Region Growing Connectivity 초기화*/
		QString strVal;
		REGION_GROWING_CONNECTIVITY connectivity;
		if (m_pWindowManager->getConfigValue(
			ELEMENT_FILE,
			STRING_MANAGER->config_RegionGrowingConnectivity,
			strVal) == true)
		{
			connectivity = (REGION_GROWING_CONNECTIVITY)strVal.toInt();
			m_pWindowManager->GetTab()->getThreSholdTab()->setRegionGrowingConnectivity(connectivity);
		}

		/* Draw Cut Shape 초기화*/
		DRAW_CUT_SHAPE drawCutMode;
		if (m_pWindowManager->getConfigValue(
			ELEMENT_FILE,
			STRING_MANAGER->config_DrawCutShape,
			strVal) == true)
		{
			drawCutMode = (DRAW_CUT_SHAPE)strVal.toInt();
			m_pWindowManager->GetTab()->getDrawTab()->setDrawCutMode(drawCutMode);
		}

	}


#ifdef PASSWORD_VER
	{
		bool res = false;
		PasswordDialog dlg(this);

		dlg.exec();

		if (dlg.isAccept())
		{
			if (QString::compare(dlg.getPass(), QString("yoon0131")) == 0)
			{
				res = true;
			}
		}
		if (!res) return -2;

	}
#endif
#if defined(COVID19_VER)
	m_strMainTitle = QString("%1").arg(QString(VER_PRODUCTNAME_STR) + " " + QString::fromUtf8(m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY).c_str()) + QString(VER_ADD_STR));
#else

	QString productType = m_pLicenseManager->getProductType();

	if (!(productType.compare(PRODUCT_NAME_MEDIP)))
	{
		std::string productInfo = m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY);
		if (m_pLicenseManager->getTrialVer() == false)
		{
			m_strMainTitle = QString("%1 %2")
				.arg(QString(productType) + " " + QString::fromUtf8(productInfo.c_str())
					+ QString(VER_ADD_STR))
				.arg(STRING_MANAGER->getString(STR_PROGRAM_TITLE_CONTENT));
		}
		else
		{
			m_strMainTitle = QString("%1 %2")
				.arg(QString(productType) + " " + QString::fromUtf8(productInfo.c_str())
					+ /*QString(" Demo") +*/ QString(VER_ADD_STR))
				.arg(STRING_MANAGER->getString(STR_PROGRAM_TITLE_CONTENT));
		}
	}
	else if ((!productType.compare(PRODUCT_NAME_DEEPCATCH) || !productType.compare(PRODUCT_NAME_DEEPCATCH_V2)))
	{
		m_strMainTitle = QString("%1 %2")
			.arg(QString::fromUtf8(PRODUCT_NAME_DEEPCATCH) + QString(VER_ADD_STR), STRING_MANAGER->getString(STR_PROGRAM_TITLE_CONTENT));
	}
	else if (!(productType.compare(PRODUCT_NAME_MEDIP_AI)))
	{
		std::string productInfo = m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY);
		m_strMainTitle = QString("%1 %2")
			.arg(QString::fromUtf8(productInfo.c_str()) + QString(VER_ADD_STR), STRING_MANAGER->getString(STR_PROGRAM_TITLE_CONTENT));
	}

#endif
#ifdef _M_IX86
	title += " (32bit)";
#endif

	//Apha
	QString version;
#if defined ALPHA_VERSION
	version = " - " + QString(VER_FILE_VERSION_STR) + " Alpha";
#elif defined BETA_VERSION
	version = " - " + QString(VER_FILE_VERSION_STR) + " Beta";
#else
	version = " - " + QString(VER_PRODUCT_VERSION_STR);
#endif
	m_strMainTitle += version;

	setWindowTitle(QApplication::translate("MedipQTClass", m_strMainTitle.toStdString().c_str(), Q_NULLPTR));
	setMinimumSize(QSize(1024, 768));

	//CSelfDump::RegisterExceptionFilter();

	m_pDockImage = m_pProductFactory->createWidget<CollapseDock>(MFL_Common_Report_ImageManagement, this);//new CollapseDock("Image Management", this);
	if (m_pDockImage)
	{
		m_pDockImage->setObjectName(QString("Image Management"));
		m_pDockImage->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
		m_pDockImage->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
		m_pDockImage->setStyleSheet(STYLE_MANAGER->rightDockingWidget);
		m_pDockImage->setWindowTitle("Image Management");

		dockArea = QString::number(Qt::DockWidgetArea::LeftDockWidgetArea);
		addDockWidget((Qt::DockWidgetArea)dockArea.toInt(), m_pDockImage);

		if (m_pWindowManager->imgTabList == nullptr)
		{
			QScrollArea* scrollarea = new QScrollArea(this);
			scrollarea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
			scrollarea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
			scrollarea->setWidgetResizable(true);

			m_pWindowManager->imgTabList = new ImageManagementTab(this);

			scrollarea->setWidget(m_pWindowManager->imgTabList);
			scrollarea->setStyleSheet(STYLE_MANAGER->rightDockingScrollArea);
			m_pDockImage->setWidget(scrollarea);
		}

		if (imgShow.toInt() == false && m_pBtnImageList != nullptr)
		{
			m_pBtnImageList->setChecked(false);
			OnShowImageList();
		}
	}


	// 	if (!useCheck())
	// 		return -1;



	if (FALSE == dockShow.toInt() && m_pBtnCollapseActive != nullptr)
	{
		m_pBtnCollapseActive->setChecked(false);
		OnShowTab();
	}

	//#if defined(COVID19_VER)
	if (!(m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare("COVID19"))
	{
		bool isExistPreset = false;
		for (int i = 0; i < m_pWindowManager->getPresetCount(); ++i)
		{
			QString strName = m_pWindowManager->getPresetName(i);

			if (!strName.compare("COVID19"))
			{
				isExistPreset = true;
				break;
			}
		}

		if (!isExistPreset)
		{
			QString strFileName = STRING_MANAGER->presetFilePath + "/COVID19.prd";
			m_pWindowManager->loadPRD(strFileName);
		}
	}
	//#endif

	m_pWindowManager->mainWindow->connect(m_pWindowManager->mainWindow, &MedipQT::ProgramClose, m_pWindowManager->mainWindow, &MedipQT::OnThreadCancel);
	IgnoreCloseEvent = false;

	m_pActionManager->addObserver(this);

	return TRUE;
}

void MedipQT::ExistFileSave(QString fileName, eAfterTHREAD type)
{
	m_pWindowManager->bExit = false;
	m_pActionManager->SetAfterThread(type);
	if (type == THREAD_OPEN_FILE)
	{
		m_StrOpen = fileName;
	}

	OnMenuFileSave();
}

void MedipQT::ExistFileSaveAndOpenImportData()
{
	m_pWindowManager->bExit = false;
	m_pActionManager->SetAfterThread(THREAD_OPEN_IMPORT_DATA);

	OnMenuFileSave();
}

bool MedipQT::createReportTab()
{
	//m_pWindowManager->mainReportWidget = new ReportWidget(this);
	m_pWindowManager->mainReportWidget = m_pProductFactory->createWidget<ReportWidget>(MFL_Common_Report_GeneralFunction, this);
	if (nullptr == m_pWindowManager->mainReportWidget)
	{
		return false;
	}
	m_pMainTab[MAINTAB_REPORT] = m_pWindowManager->mainReportWidget;

	return true;
}

void MedipQT::showImageManagementTab()
{
	if (nullptr == m_pBtnImageList)
	{
		return;
	}
	m_pBtnImageList->setChecked(true);
	OnShowImageList();
}

void MedipQT::setDockWindowTitle(const QString& title)
{
	if (m_pDockCollapseSeg)
	{
		m_pDockCollapseSeg->setWindowTitle(title);
	}
}

void MedipQT::setMainWindowTitle(QString subTitle, bool isMain)
{
	QString title = m_strMainTitle;

	if (isMain)
		title = subTitle;
	else
		title.append(subTitle);

	setWindowTitle(QApplication::translate("MedipQTClass", title.toStdString().c_str(), Q_NULLPTR));
}

/*@function OnRecentFileOpen
*@brief recent file open
* file exist 유무 확인 및 open
*/
void MedipQT::OnRecentFileOpen()
{
	QString filename = QString(); //within path
	int index = 0;
	for (int i = 0; i < RECENT_MAX; i++)
	{
		if (m_pActRecent[i] && m_pActRecent[i]->isChecked())
		{
			m_pActRecent[i]->setChecked(false);

			filename = m_strRecent[i];
			index = i;
			break;
		}
	}

	if (!filename.isEmpty())
	{
		QFile file(filename);

		if (file.exists())
			fileOpen(filename);
		else
		{
			//QMessageBox dlg(this);
			//dlg.setWindowFlags(dlg.windowFlags() & ~(Qt::WindowContextHelpButtonHint));
			//dlg.setWindowTitle("Keep none existing file");
			//dlg.setText(QString("The file '%1' doesn't exist anymore.\n Keep this reference in menu?")
			//	.arg(filename));
			//QPushButton *btnNo = dlg.addButton(QString("No"), QMessageBox::RejectRole);
			//dlg.addButton(QString("Yes"), QMessageBox::AcceptRole);

			//dlg.exec();

			//if (btnNo == dlg.clickedButton())
			//	DeleteRecentFile(index);

			if (QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DU_1012)
				, (QMessageBox::Ok | QMessageBox::No | QMessageBox::Cancel)).exec() == QMessageBox::No)
			{
				DeleteRecentFile(index);
			}
		}
	}
}

void MedipQT::OnMenuFileOpen()
{
	QString strLatest;
	bool latest = m_pWindowManager->lastestPathGet(strLatest);

	const QFileDialog::Options options = QFlag(QFileDialog::ShowDirsOnly);
	QString selectedFilter;

	QString strFilter = QString("All Files (*);;DICOM File (*.dcm;*.DCM);;");

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Import_MIP))
	{
		strFilter += QString("MEDIP File(*.mip; *.MIP);;");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Import_MIPD))
	{
		strFilter += QString("DeepCatch File(*.mipd; *.MIPD);;");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Import_MIPA))
	{
		strFilter += QString("MEDIP AI File(*.mipa; *.MIPA);;");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Import_NII_RAW_Mask))
	{
		strFilter += QString("RAW File(*.raw;*.RAW);;ROI NII / NII File(*.nii;*.NII);;");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_MeshEditing_FileImport))
	{
		strFilter += QString("3MF File(*.3mf;*.3MF);;");
		strFilter += QString("STL File(*.stl;*.STL);;");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Rendering_2D_3DHistogram_Presetcustomising) && m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Rendering_2D_3DHistogram_Presetfileimport))
	{
		strFilter += QString("Preset File(*.prd;*.PRD);;");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Report_ImageManagement_ImageImport))
	{
		strFilter += QString("PNG File(*.png;*.PNG);;BMP File(*.bmp;*.BMP);;JPG File(*.jpg;*.JPG);;TXT File(*.txt;*.TXT);;");
	}

	QFileInfo fileInfo(latest ? strLatest : QDir::homePath());
	QString fileName = ImportFileDialog(
		this,
		STRING_MANAGER->getString(STR_MEDIP_FILE_OPEN),
		fileInfo.fileName(),
		fileInfo.dir().absolutePath(),
		strFilter
	);

	if (!fileName.isEmpty())
	{
		fileOpen(fileName);
	}
}

void MedipQT::fileOpen(QString fileName, bool bReverse)
{
	if (!fileName.isEmpty())
	{
		bool bResult = false;
		bResult = m_pWindowManager->fileOpen(fileName, bReverse);
		if (!(m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare("COVID19"))
		{
			// preset 셋팅
			for (int i = 0; i < m_pWindowManager->getPresetCount(); ++i)
			{
				QString strName = m_pWindowManager->getPresetName(i);
				if (!strName.compare("COVID19"))
				{
					int preLevel = m_pWindowManager->getVolumeLevel();
					int preWidth = m_pWindowManager->getVolumeWidth();

					m_pActionManager->action_Preset(preLevel, preWidth, (SLICE_PRESET)-1, (SLICE_PRESET)i, i);
					m_pActionManager->action_VolumePreset(preLevel, preWidth, (SLICE_PRESET)-1, (SLICE_PRESET)i, i);
					break;
				}
			}
		}
		if (bResult)
		{
			QString ext = fileName.section('.', -1);
			if (ext.contains(QRegularExpression(EXT_FILE_LIST)))
				return;

			m_pWindowManager->lastestPathSave(fileName);
		}
		std::vector<pair<int, std::vector<unsigned char>>>().swap(m_pDataContext->volume_data.m_vecAIResultData);
		std::vector<pair<int, int>>().swap(m_pDataContext->volume_data.m_vecAIOutset);
	}
}

void MedipQT::OpenImportData(bool isImportDicomCropping)
{
	fm::EFileManagerImportDataType importDataType = m_pImportData->GetImportDataType();

	/* MIP, DICOM일 경우 Save 체크*/
	if (
		(importDataType == fm::EFileManagerImportDataType::mip_project) ||
		(importDataType == fm::EFileManagerImportDataType::dicom_data)
		)
	{
		if (m_pWindowManager->ShouldCheckSaveStatus())
		{
			int res = QMessageBox::warning(this,
				STRING_MANAGER->getString(STR_SAVE_FILE),
				STRING_MANAGER->getString(STR_SAVE_FILE_DESC),
				QMessageBox::Ok | QMessageBox::No | QMessageBox::Cancel);
			if (res == QMessageBox::Ok)
			{
				/* 저장 후 OpenImportData() 재호출 */
				ExistFileSaveAndOpenImportData();
				return;
			}
			else if (res == QMessageBox::No)
			{
				/* Save 상태를 true로 초기화 */
				m_pWindowManager->setSaveState(true);
			}
			else if (res == QMessageBox::Cancel)
			{
				/* Skip */
				return;
			}
		}
	}

	switch (importDataType)
	{
	case fm::EFileManagerImportDataType::mip_project:
	{
		fileOpen(m_pImportData->GetMIPProjectFilePath());
		break;
	}
	case fm::EFileManagerImportDataType::dicom_data:
	{
		/* 마지막 경로 저장 */
		QString dcmFilePath = m_pImportData->GetDcmFilePath();
		m_pWindowManager->lastestPathSave(dcmFilePath);
		/* 상태 초기화 진행 */
		m_pWindowManager->InitViewControlsState();
		bool result = false;

		if (isImportDicomCropping == false)
		{
			result = m_pWindowManager->loadDicomData(
				m_pImportData->Data(),
				*m_pImportData->GetDcmSeriesInfo(),
				*m_pImportData->GetDcmVolumeInfo());
		}
		else
		{
			result = m_pWindowManager->loadCroppingDicomData(
				m_pImportData->Data(),
				*m_pImportData->GetDcmSeriesInfo(),
				*m_pImportData->GetDcmVolumeInfo());
		}
		if (result)
		{
			std::vector<pair<int, std::vector<unsigned char>>>().swap(m_pDataContext->volume_data.m_vecAIResultData);
			std::vector<pair<int, int>>().swap(m_pDataContext->volume_data.m_vecAIOutset);
		}
		break;
	}
	case fm::EFileManagerImportDataType::import_file_list:
	{
		/* 상태 초기화 진행 */
		m_pWindowManager->InitViewControlsState();

		QStringList filePathList = m_pImportData->GetImportFilePathList();
		if (filePathList.isEmpty() == false)
		{
			QFileInfo fileInfo(filePathList[0]);
			QString fileSuffx = fileInfo.suffix().toLower();
			if (fileSuffx == "stl")
			{
				m_pActionManager->action_FileWork_Import_STL(m_pDataContext, filePathList);
			}
			else if (fileSuffx == "obj")
			{
				m_pActionManager->action_FileWork_Import_OBJ(m_pDataContext, filePathList);
			}
			else if (fileSuffx == "vtk")
			{
				m_pActionManager->action_FileWork_Import_VTK(m_pDataContext, filePathList);
			}
			else if (fileSuffx == "3mf")
			{
				m_pActionManager->action_FileWork_Import_3MF(m_pDataContext, filePathList);
			}
			else if (fileSuffx == "usd")
			{
				m_pActionManager->action_FileWork_Import_USD(m_pDataContext, filePathList[0]);
			}
			else if (fileSuffx == "raw")
			{
				ImportMultiFile_Default(filePathList);
			}
			else if (fileSuffx == "nii")
			{
				ImportMultiFile_Default(filePathList);
			}
			else if (fileSuffx == "prd")
			{
				ImportMultiFile_Default(filePathList);
			}
			else if (
				fileSuffx == "png" ||
				fileSuffx == "bmp" ||
				fileSuffx == "jpg" ||
				fileSuffx == "txt"
				)
			{
				ImportMultiFile_Default(filePathList);
			}
		}
		break;
	}
	default:
		qCritical() << "invalid import data type : " << (int)importDataType;
		break;
	}
}

void MedipQT::callMainTabchange(int tabType)
{
	int cnt = m_pTab->count();
	for (int ii = 0; ii < cnt; ii++)
	{
		if (m_pTab->tabText(ii) == STRING_MANAGER->getString(STR_MESH_EDITING))
		{
			m_pTab->setCurrentIndex(ii);
			return;
		}
	}
}

bool MedipQT::MedipMacroFileOpen(QString filepath)
{
	if (filepath.isEmpty())
	{
		return false;
	}

	if (m_pWindowManager->fileOpen(filepath) == false)
	{
		return false;
	}

	QString ext = filepath.section('.', -1);
	if (ext.contains(QRegularExpression(EXT_FILE_LIST)))
	{
		return false;
	}

	m_pWindowManager->lastestPathSave(filepath);
	return true;
}

void MedipQT::OnMacroFileOpen(QString fileName, bool& isDone)
{
	QString strLatest;
	bool latest = m_pWindowManager->lastestPathGet(strLatest);

	if (!fileName.isEmpty())
	{
		latest = m_pWindowManager->fileOpen(fileName);

		if (latest)
		{
			QString ext = fileName.section('.', -1);
			if (ext.contains(QRegularExpression(EXT_FILE_LIST)))
			{
				return;
			}

			m_pWindowManager->lastestPathSave(fileName);
		}
		else
		{
			if (!ACTION_MANAGER->m_IsMultiSeries)
			{
				while (ACTION_MANAGER->m_qThreadNext.front().eNextThread != THREAD_DEEPCATCH_NEXT_LOOP)
				{
					ACTION_MANAGER->m_qThreadNext.pop_front();
					--ACTION_MANAGER->m_nCurrentThreadCount;
				}
			}
		}

#if defined(DEEPCATCH_VER_MACRO)
		WindowBase* pView = m_pWindowManager->mainSegmentWidget->getWindow(WT_AXIAL);
		if (pView)
			pView->m_btnShowBounding->click();
#endif
	}

	isDone = latest;
}

void MedipQT::OnXrayFileOpen()
{
	QString strLatest;
	bool latest = m_pWindowManager->lastestPathGet(strLatest);

	const QFileDialog::Options options = QFlag(QFileDialog::ShowDirsOnly);
	QString selectedFilter;

	QString strFilter = QString("All Files (*);;DICOM File (*.dcm;*.DCM);;");

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Import_MIP))
	{
		strFilter += QString("MEDIP File(*.mip; *.MIP);;");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Import_MIPD))
	{
		strFilter += QString("DeepCatch File(*.mipd; *.MIPD);;");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Import_MIPA))
	{
		strFilter += QString("MEDIP AI File(*.mipa; *.MIPA);;");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Import_NII_RAW_Mask))
	{
		strFilter += QString("RAW File(*.raw;*.RAW);;ROI NII / NII File(*.nii;*.NII);;");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_MeshEditing_FileImport))
	{
		strFilter += QString("STL File(*.stl;*.STL);;");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Rendering_2D_3DHistogram_Presetcustomising) && m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Rendering_2D_3DHistogram_Presetcustomising))
	{
		strFilter += QString("Preset File(*.prd;*.PRD);;");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Report_ImageManagement_ImageImport))
	{
		strFilter += QString("PNG File(*.png;*.PNG);;BMP File(*.bmp;*.BMP);;JPG File(*.jpg;*.JPG);;TXT File(*.txt;*.TXT);;");
	}

	QString fileName = QFileDialog::getOpenFileName(this,
		STRING_MANAGER->getString(STR_MEDIP_FILE_OPEN),
		latest ? strLatest : QDir::homePath(), // const QString & dir = QString(),
		strFilter, //const QString & filter = QString()
		&selectedFilter, // QString * selectedFilter = 0,
		options); // Options options = 0)

	fileOpen(fileName);
}


void MedipQT::OnMenuFilesOpen()
{
	if (!m_pDataContext->volume_data.isValidate())
	{
		QMessageBox::warning(this, QString("files open"), STRING_MANAGER->getString(STR_LOAD_FIRST));
		return;
	}

	QString latestFilePath;
	bool isLatestFilePathExist = m_pWindowManager->lastestPathGet(latestFilePath, true);

	const QFileDialog::Options options = QFlag(QFileDialog::ShowDirsOnly);

	QString filter = QString("All Files (*);;DICOM File (*.dcm;*.DCM);;");

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Import_MIP))
	{
		filter += QString("MEDIP File(*.mip; *.MIP);;");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Import_MIPD))
	{
		filter += QString("DeepCatch File(*.mipd; *.MIPD);;");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Import_MIPA))
	{
		filter += QString("MEDIP AI File(*.mipa; *.MIPA);;");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Import_NII_RAW_Mask))
	{
		filter += QString("RAW File(*.raw;*.RAW);;ROI NII / NII File(*.nii;*.NII);;");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_MeshEditing_FileImport))
	{
		filter += QString("STL File(*.stl;*.STL);;");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Rendering_2D_3DHistogram_Presetcustomising) && m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Rendering_2D_3DHistogram_Presetcustomising))
	{
		filter += QString("Preset File(*.prd;*.PRD);;");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Report_ImageManagement_ImageImport))
	{
		filter += QString("PNG File(*.png;*.PNG);;BMP File(*.bmp;*.BMP);;JPG File(*.jpg;*.JPG);;TXT File(*.txt;*.TXT);;");
	}

	QString selectedFilter;
	QStringList fileNameList = QFileDialog::getOpenFileNames(this,
		QString("Medip resource files open"),
		isLatestFilePathExist ? latestFilePath : QDir::homePath(), // const QString & dir = QString(),
		filter, //const QString & filter = QString()
		&selectedFilter, // QString * selectedFilter = 0,
		options); // Options options = 0)

	m_pWindowManager->fileMultiOpen(fileNameList);
}

void MedipQT::OnMenuReverseOpen()
{
	QString strLatest;
	bool latest = m_pWindowManager->lastestPathGet(strLatest);

	const QFileDialog::Options options = QFlag(QFileDialog::ShowDirsOnly);

	QString strFilter = QString("All Files (*);;DICOM File (*.dcm;*.DCM);;");

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Import_MIP))
	{
		strFilter += QString("MEDIP File(*.mip; *.MIP);;");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Import_MIPD))
	{
		strFilter += QString("DeepCatch File(*.mipd; *.MIPD);;");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Import_MIPA))
	{
		strFilter += QString("MEDIP AI File(*.mipa; *.MIPA);;");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Import_NII_RAW_Mask))
	{
		strFilter += QString("RAW File(*.raw;*.RAW);;ROI NII / NII File(*.nii;*.NII);;");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_MeshEditing_FileImport))
	{
		strFilter += QString("STL File(*.stl;*.STL);;");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Rendering_2D_3DHistogram_Presetcustomising) && m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Rendering_2D_3DHistogram_Presetcustomising))
	{
		strFilter += QString("Preset File(*.prd;*.PRD);;");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Report_ImageManagement_ImageImport))
	{
		strFilter += QString("PNG File(*.png;*.PNG);;BMP File(*.bmp;*.BMP);;JPG File(*.jpg;*.JPG);;TXT File(*.txt;*.TXT);;");
	}

	QString selectedFilter;
	QString fileName = QFileDialog::getOpenFileName(this,
		STRING_MANAGER->getString(STR_MEDIP_FILE_OPEN),
		latest ? strLatest : QDir::homePath(), // const QString & dir = QString(),
		strFilter, //const QString & filter = QString()
		&selectedFilter, // QString * selectedFilter = 0,
		options); // Options options = 0)

	fileOpen(fileName, false);
}

void MedipQT::OnMenuFileManagerOpen()
{
	if (!m_pActionManager->isActionFinished())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

#ifdef DEV_FILE_MANAGER
	QString mipFileFilters;
	QString importFileFilters;

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Import_MIP))
	{
		mipFileFilters += QString("MEDIP File(*.mip; *.MIP);;");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Import_MIPD))
	{
		mipFileFilters += QString("DeepCatch File(*.mipd; *.MIPD);;");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Import_MIPA))
	{
		mipFileFilters += QString("MEDIP AI File(*.mipa; *.MIPA);;");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Import_NII_RAW_Mask))
	{
		importFileFilters += QString("RAW File(*.raw;*.RAW);;ROI NII / NII File(*.nii;*.NII);;");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_MeshEditing_FileImport))
	{
		importFileFilters += QString("STL File(*.stl;*.STL);;");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_MeshEditing_FileImport))
	{
		importFileFilters += QString("VTK File(*.vtk;*.VTK);;");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_MeshEditing_FileImport))
	{
		importFileFilters += QString("OBJ File(*.obj;*.OBJ);;");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_MeshEditing_FileImport))
	{
		importFileFilters += QString("USD File(*.usd;*.USD);;");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_MeshEditing_FileImport))
	{
		importFileFilters += QString("3mf File(*.3mf;*.3MF);;");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Rendering_2D_3DHistogram_Presetcustomising) && m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Rendering_2D_3DHistogram_Presetcustomising))
	{
		importFileFilters += QString("Preset File(*.prd;*.PRD);;");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Report_ImageManagement_ImageImport))
	{
		importFileFilters += QString("PNG File(*.png;*.PNG);;BMP File(*.bmp;*.BMP);;JPG File(*.jpg;*.JPG);;TXT File(*.txt;*.TXT)");
	}

	QString defaultSelectFilter_Text;
	fm::FileManagerDialog* pDlg;
	if (FILE_MANAGER->App.GetDialog(pDlg))
	{
		if (pDlg->GetImportDefaultData(m_pImportData, mipFileFilters, importFileFilters, defaultSelectFilter_Text) == false)
		{
			return;
		}

		OpenImportData(false);
	}

#endif
}

void MedipQT::OnMenuCropping()
{
	if (!m_pActionManager->isActionFinished())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	fm::FileManagerDialog* pDlg;
	if (FILE_MANAGER->App.GetDialog(pDlg))
	{
		if (pDlg->GetImportDicomData(m_pImportData) == false)
		{
			return;
		}

		OpenImportData(true);
	}
}

bool MedipQT::OnMenuFileSave(bool exit)
{
	if (!m_pActionManager->isActionFinished())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return true;
	}

	if (m_pDataContext->volume_data.isValidate() == false)
	{
		return false;
	}

	QString strLatest;
	bool latest = m_pWindowManager->lastestPathGet(strLatest);

	QFileInfo file(strLatest);

	if (!file.exists() || !file.absoluteDir().exists())
	{
		return OnMenuFileSaveAs(exit);
	}

	QString ext = strLatest.section(".", -1).toLower();

	if ((ext == "nii") || (m_pWindowManager->fileSave(strLatest, exit) == false))
	{
		return OnMenuFileSaveAs(exit);
	}

	return true;
}

bool MedipQT::OnMenuFileSaveAs(bool exit)
{
	if (!m_pActionManager->isActionFinished())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return true;
	}

	if (m_pDataContext->volume_data.isValidate() == false) return false;

	QString strLatest;
	bool latest = m_pWindowManager->lastestPathGet(strLatest);

	strLatest = strLatest.section(".", 0, 0);

	QFileInfo file(strLatest);

	if (!file.absoluteDir().exists())
		latest = false;

	int i = 0;
	const QFileDialog::Options options = QFlag(QFileDialog::ShowDirsOnly);
	QString selectedFilter;

	QString saveFormat = "";
	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Export_MIP)
		&& !m_pLicenseManager->getProductType().compare(PRODUCT_NAME_MEDIP))
	{
		saveFormat += tr("MEDIP File(*.mip;*.MIP);;");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Export_MIPD)
		&& (!m_pLicenseManager->getProductType().compare(PRODUCT_NAME_DEEPCATCH)
			|| !m_pLicenseManager->getProductType().compare(PRODUCT_NAME_DEEPCATCH_V2)))
	{
		saveFormat += tr("DeepCatch File(*.mipd;*.MIPD);;");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Export_MIPA)
		&& !m_pLicenseManager->getProductType().compare(PRODUCT_NAME_MEDIP_AI))
	{
		saveFormat += tr("MEDIP AI File(*.mipa;*.MIPA);;");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Export_NII_wholeHU))
	{
		saveFormat += tr("NII File(*.nii; *.NII);;");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Export_jsonExport))
	{
		saveFormat += tr("JSON File(*.json; *.JSON);;");
	}

	// #if !defined(DEEP_CATCH_VER) && !defined(COVID19_VER)
	// 	QString saveFormat = tr("MEDIP File(*.mip;*.MIP);;NII File(*.nii;*.NII)");
	// #else
	// 	QString saveFormat = tr("MEDIP File(*.mip;*.MIP)");
	// #endif
	QFileInfo fileInfo(strLatest);
	QString fileName = ExportFileDialog(
		this,
		STRING_MANAGER->getString(STR_MEDIP_FILE_SAVE),
		fileInfo.fileName(),
		fileInfo.dir().path(),
		saveFormat,
		QFileDialog::ShowDirsOnly
	);

	if (!fileName.isEmpty())
		return m_pWindowManager->fileSave(fileName, exit);

	return false;
}

bool MedipQT::OnMenuUploadToPACS()
{
	if (!m_pActionManager->isActionFinished())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return false;
	}

	if (m_pDataContext->volume_data.isValidate() == false)
	{
		return false;
	}

#ifdef DEV_FILE_MANAGER
	/* 필수 DICOM 정보가 업데이트 되지 않았습니다. */
	if (m_pWindowManager->isDicomSeriesInfoShouldBeUpdated())
	{
		QString text = QString::fromLocal8Bit("DICOM 정보 업데이트가 필요합니다.");
		QMessageBox::information(this, STRING_MANAGER->getString(STR_INFO), text);
		if (m_pWindowManager->importDicomFile_And_UpdateSummary() == false)
		{
			return false;
		}
	}

	fm::FileManagerDialog* pDlg;
	if (FILE_MANAGER->App.GetDialog(pDlg))
	{
		fm::FileManagerExportData exportData;
		DcmtkSeriesInfo* pDcmInfo = m_pWindowManager->GetDicomInfo();
		exportData.SetExportDicomInfo(pDcmInfo);

		ImageManagementTab* pImageManagementTab = m_pWindowManager->imgTabList;
		if (pImageManagementTab)
		{
			ImageListTab* pImageProcessTab = pImageManagementTab->getImgTab();
			std::map<QString, QImage> captureImageList = pImageProcessTab->GetCaptureImageList();
			std::map<QString, QImage> importImageList = pImageProcessTab->GetImportImageList();

			for (auto it = captureImageList.begin(); it != captureImageList.end(); ++it)
			{
				exportData.AddImage((*it).second);
			}

			for (auto it = importImageList.begin(); it != importImageList.end(); ++it)
			{
				exportData.AddImage((*it).second);
			}
		}


		if (m_pWindowManager->mainReportWidget)
		{
			QTextDocument* document = m_pWindowManager->mainReportWidget->GetReportDocument();
			if (document)
			{
				exportData.AddPdfDocument(document);
			}
		}

		pDlg->ExportToPACS(&exportData);
	}
#endif

	return true;
}

void MedipQT::OnLayerFlip()
{
	m_pWindowManager->setFlipMode();
}

bool MedipQT::OnPrePage()
{
	if (m_pWindowManager->mainSegmentWidget && m_pWindowManager->mainTabType == MAINTAB_SEGMENTATION)
		m_pWindowManager->mainSegmentWidget->prePage();

	if (m_pWindowManager->mainAnalWidget && m_pWindowManager->mainTabType == MAINTAB_MEASUREMENT)
		m_pWindowManager->mainAnalWidget->prePage();

	if (m_pWindowManager->mainTAWidget && m_pWindowManager->mainTabType == MAINTAB_TA)
		m_pWindowManager->mainTAWidget->prePage();

	return true;
}

bool MedipQT::OnNextPage()
{
	if (m_pWindowManager->mainSegmentWidget && m_pWindowManager->mainTabType == MAINTAB_SEGMENTATION)
		m_pWindowManager->mainSegmentWidget->nextPage();

	if (m_pWindowManager->mainAnalWidget && m_pWindowManager->mainTabType == MAINTAB_MEASUREMENT)
		m_pWindowManager->mainAnalWidget->nextPage();

	if (m_pWindowManager->mainTAWidget && m_pWindowManager->mainTabType == MAINTAB_TA)
		m_pWindowManager->mainTAWidget->nextPage();

	return true;
}

bool MedipQT::OnDelete()
{
	m_pWindowManager->deleteKeyProcess();
	return true;
}

bool MedipQT::OnLess()
{
	m_pWindowManager->openBracketProcess();
	return true;
}

bool MedipQT::OnMore()
{
	m_pWindowManager->closeBracketProcess();
	return true;
}

void MedipQT::OnHPA()
{
	setHomePosition(HP_ANTERIOR);
}

void MedipQT::OnHPP()
{
	setHomePosition(HP_POSTERIOR);
}

void MedipQT::OnHPL()
{
	setHomePosition(HP_LEFT);
}

void MedipQT::OnHPR()
{
	setHomePosition(HP_RIGHT);
}

void MedipQT::OnHPS()
{
	setHomePosition(HP_SUPERIOR);
}

void MedipQT::OnHPI()
{
	setHomePosition(HP_INFERIOR);
}

void MedipQT::setHomePosition(HOME_POSITION index, bool bForce)
{
	if (m_pWindowManager->mainTabType == MAINTAB_MESH_EDITING && m_pWindowManager->mainMeshWidget != nullptr)
	{
		MEVolumeView* view = dynamic_cast<MEVolumeView*>(m_pWindowManager->mainMeshWidget->getMainView());

		if (view)
			MESH_RENDER_MANAGER->setHomePosition(index);
	}
	else
	{
		{
			VolumeView* view = dynamic_cast<VolumeView*>(m_pWindowManager->mainSegmentWidget->getViewVolume());

			if (view)
				view->setHomePosition(index, bForce);
		}

		if (m_pWindowManager->mainAnalWidget)
		{
			AnalVolumeView* view = dynamic_cast<AnalVolumeView*>(m_pWindowManager->mainAnalWidget->getViewVolume());

			if (view)
				view->setHomePosition(index);
		}

		if (m_pWindowManager->mainTAWidget)
		{
			AnalVolumeView* view = dynamic_cast<AnalVolumeView*>(m_pWindowManager->mainTAWidget->getViewVolume());

			if (view)
				view->setHomePosition(index);
		}
	}
}

void MedipQT::moveEvent(QMoveEvent* event)
{
	//QMainWindow::moveEvent(event);
	m_pWindowManager->MoveMeshDialog();
}

QStringList MedipQT::getFileOpenExtensionFilter()
{
	QStringList strFilterList;
	strFilterList << QString("*.dcm");

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Import_MIP))
	{
		strFilterList << QString("*.mip");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Import_MIPD))
	{
		strFilterList << QString("*.mipd");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Import_MIPA))
	{
		strFilterList << QString("*.mipa");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Import_NII_RAW_Mask))
	{
		strFilterList << QString("*.raw");
		strFilterList << QString("*.nii");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_MeshEditing_FileImport))
	{
		strFilterList << QString("*.3mf");
		strFilterList << QString("*.stl");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Rendering_2D_3DHistogram_Presetcustomising) && m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Rendering_2D_3DHistogram_Presetfileimport))
	{
		strFilterList << QString("*.prd");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Report_ImageManagement_ImageImport))
	{
		strFilterList << QString("*.png");
		strFilterList << QString("*.bmp");
		strFilterList << QString("*.jpg");
		strFilterList << QString("*.txt");
	}

	return strFilterList;
}

QStringList MedipQT::GetImportFilePathListByFunctionLevel(int functionLevel, QString filter)
{
	QStringList filePathList;

	QString filterTextList;
	if (m_pProductManager->IsAvailableFunctionLevel((eMEDIP_FUNCTION_LEVEL)functionLevel) == false)
	{
		return filePathList;
	}

	filterTextList += filter;

	filePathList = ImportFileListDialog(
		this,
		STRING_MANAGER->getString(STR_MEDIP_FILE_OPEN),
		"",
		filterTextList
	);

	return filePathList;
}

void MedipQT::ImportMultiFile_Default(QStringList filePathList)
{
	m_pWindowManager->fileMultiImport(filePathList);
}

void MedipQT::AddActionToMenu(QMenu* menu, QString text, const char* slotFunction)
{
	QAction* action = new QAction(text);
	connect(action, SIGNAL(triggered()), this, slotFunction);
	menu->addAction(action);

}

void MedipQT::AddActionToMenuWithFunctionLevel(QMenu* menu, QString text, const char* slotFunction, int functionLevel)
{
	if (m_pProductManager->IsAvailableFunctionLevel((eMEDIP_FUNCTION_LEVEL)functionLevel))
	{
		AddActionToMenu(menu, text, slotFunction);
	}
}

bool MedipQT::saveAs_Logout()
{
	if (!m_pActionManager->isActionFinished())
	{
		return false;
	}

	if (m_pDataContext->volume_data.isValidate() == false)
	{
		return false;
	}

	QString strLatest;
	bool latest = m_pWindowManager->lastestPathGet(strLatest);

	strLatest = strLatest.section(".", 0, 0);

	QFileInfo file(strLatest);

	if (!file.absoluteDir().exists())
		latest = false;

	QString saveFormat = "";
	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Export_MIP)
		&& !m_pLicenseManager->getProductType().compare(PRODUCT_NAME_MEDIP))
	{
		saveFormat += tr("MEDIP File(*.mip;*.MIP);;");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Export_MIPD)
		&& !m_pLicenseManager->getProductType().compare(PRODUCT_NAME_DEEPCATCH))
	{
		saveFormat += tr("DeepCatch File(*.mipd;*.MIPD);;");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Export_MIPA)
		&& !m_pLicenseManager->getProductType().compare(PRODUCT_NAME_MEDIP_AI))
	{
		saveFormat += tr("MEDIP AI File(*.mipa;*.MIPA);;");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Export_NII_wholeHU))
	{
		saveFormat += tr("NII File(*.nii; *.NII);;");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Export_jsonExport))
	{
		saveFormat += tr("JSON File(*.json; *.JSON);;");
	}

	QFileInfo fileInfo(strLatest);
	QString fileName = ExportFileDialog(
		this,
		STRING_MANAGER->getString(STR_MEDIP_FILE_SAVE),
		fileInfo.fileName(),
		fileInfo.dir().path(),
		saveFormat,
		QFileDialog::ShowDirsOnly
	);

	bool exit = true;
	if (!fileName.isEmpty())
		return m_pWindowManager->fileSave(fileName, exit);

	return false;
}

void MedipQT::RebootProgram()
{
	// 	 QProcess::startDetached(QApplication::applicationFilePath());
	// 	 qApp->exit(1337);

	SHELLEXECUTEINFO shInfo;

	QString strFile = QCoreApplication::applicationFilePath();
	memset(&shInfo, 0, sizeof(SHELLEXECUTEINFO));

	shInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	shInfo.hwnd = nullptr;
	//	shInfo.lpVerb = L"runas";
	shInfo.lpVerb = L"open";
	shInfo.lpFile = (const wchar_t*)strFile.utf16();
#if defined(DEEP_CATCH_VER)
	QString strParam = " --Reboot --DeepCatch";
#elif defined(MEDIP_AI)
	QString strParam = " --Reboot --MEDIP_AI";
#else
	QString strParam = " --Reboot --Medip";
#endif

	shInfo.lpParameters = (const wchar_t*)strParam.utf16();
	shInfo.lpDirectory = (const wchar_t*)(STRING_MANAGER->programPath.utf16());
	shInfo.nShow = SW_NORMAL;

	ShellExecuteEx(&shInfo);

	emit ProgramClose();
	QMainWindow::close();
	m_pWindowManager->bExit = false;
}


/*@function DeleteRecentFile
*@brief 참조 파일 삭제 (reg, menu, action, etc)
*@param index recent file index(not reg, array start 0)
*/
void MedipQT::DeleteRecentFile(int index)
{
	if (index < 0 || index >= RECENT_MAX) return;

	HKEY recentKey = 0;
	LONG ret;
	if (!m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY).compare(m_pProductManager->m_strMedipDeepCatch)
		|| !m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY).compare(m_pProductManager->m_strMedipDeepCatchV2))
	{
		ret = RegCreateKeyEx(HKEY_CURRENT_USER, (L"Software\\Medicalip\\DeepCatch\\Recent files"),
			0, nullptr, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr, &recentKey, nullptr);
	}
	else
	{
		ret = RegCreateKeyEx(HKEY_CURRENT_USER, (L"Software\\Medicalip\\MEDIP\\Recent files"),
			0, nullptr, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr, &recentKey, nullptr);
	}

	if (ERROR_SUCCESS == ret)
	{
		bool needSwap = (index < (RECENT_MAX - 1)) && (m_pActRecent[index + 1] != nullptr);
		int delIndex = index;

		if (needSwap)
		{
			DWORD pathSize;
			for (int i = delIndex; i < (RECENT_MAX - 1); i++)
			{
				if (m_pActRecent[i + 1])
				{
					m_strRecent[i] = m_strRecent[i + 1];
					QString recentFile = QString("File%1\0").arg(i + 1);
					pathSize = (m_strRecent[i].size() + 2) * sizeof(QChar);

					ret = RegSetValueEx(recentKey, (const wchar_t*)recentFile.utf16(), 0, REG_SZ,
						(const BYTE*)m_strRecent[i].constData(), pathSize);
				}
				else
				{
					delIndex = i;
					for (int j = delIndex; j < RECENT_MAX; j++)
						m_strRecent[j] = QString();
					break;
				}
			}
		}
		else
			m_strRecent[delIndex] = QString();

		ret = RegDeleteValue(recentKey, (const wchar_t*)QString("File%1\0").arg(delIndex + 1).utf16());

		RegCloseKey(recentKey);

		if (ERROR_SUCCESS == ret)
			UpdateRecentFile();
	}
}

void MedipQT::AddRecentFile(QString FileName)
{
	bool needSwap = false;
	int swapIndex = 0; //oldIndex
	for (int i = 0; i < RECENT_MAX; i++)
	{
		if (m_strRecent[i].compare(FileName) == 0)
		{
			needSwap = true;
			swapIndex = i;
		}
	}

	if (needSwap)
	{
		if (swapIndex != 0)
			SwapRecentFile(swapIndex, 0, swapIndex);
		return;
	}

	HKEY recentKey = 0;
	LONG ret;
	if (!m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY).compare(m_pProductManager->m_strMedipDeepCatch)
		|| !m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY).compare(m_pProductManager->m_strMedipDeepCatchV2))
	{
		ret = RegCreateKeyEx(HKEY_CURRENT_USER, (L"Software\\Medicalip\\DeepCatch\\Recent files"),
			0, nullptr, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr, &recentKey, nullptr);
	}
	else
	{
		ret = RegCreateKeyEx(HKEY_CURRENT_USER, (L"Software\\Medicalip\\MEDIP\\Recent files"),
			0, nullptr, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr, &recentKey, nullptr);
	}

	if (ERROR_SUCCESS == ret)
	{
		bool swapFirst = m_pActRecent[0] != nullptr;

		DWORD pathSize;
		QString prePath;
		int i = 0;
		for (; i < RECENT_MAX; i++)
		{
			if (m_pActRecent[i])
			{
				prePath = m_strRecent[i];
				m_strRecent[i] = FileName;
				FileName = prePath;
				QString recentFile = QString("File%1\0").arg(i + 1);

				pathSize = (m_strRecent[i].size() + 2) * sizeof(QChar);
				ret = RegSetValueEx(recentKey, (const wchar_t*)recentFile.utf16(), 0, REG_SZ,
					(const BYTE*)m_strRecent[i].constData(), pathSize);
			}
			else
				break;
		}

		if (RECENT_MAX > i)
		{
			m_strRecent[i] = FileName;
			FileName = prePath;
			QString recentFile = QString("File%1\0").arg(i + 1);

			pathSize = (m_strRecent[i].size() + 2) * sizeof(QChar);
			ret = RegSetValueEx(recentKey, (const wchar_t*)recentFile.utf16(), 0, REG_SZ,
				(const BYTE*)m_strRecent[i].constData(), pathSize);
		}


		RegCloseKey(recentKey);

		if (ERROR_SUCCESS == ret)
			UpdateRecentFile();
	}
}

/*@function UpdateRecentFile
*@brief recent file list update (menu, action)
*/
void MedipQT::UpdateRecentFile()
{
	m_pMenuOpen->setStyleSheet(STYLE_MANAGER->m_MenuMaskROI);
	m_pMenuOpen->clear();
	m_pMenuOpen->addAction(m_pActOpen);
	if (m_pActMultiOpen)
		m_pMenuOpen->addAction(m_pActMultiOpen);
	m_pMenuOpen->addAction(m_pMenuOpen->addSeparator());
	QMenu* subMenu = m_pMenuOpen->addMenu("Recent files...");

	for (int i = 0; i < RECENT_MAX; i++)
	{
		if (m_strRecent[i].length() <= 1)
			m_pActRecent[i] = nullptr;
		else
		{
			QString filename = m_strRecent[i].section('/', -1);

			if (filename == m_strRecent[i])
				filename = m_strRecent[i].section('\\', -1);

			m_pActRecent[i] = subMenu->addAction(QString("%2 .../%1").arg(filename).arg(i + 1));
			subMenu->connect(m_pActRecent[i], &QAction::triggered, this, &MedipQT::OnRecentFileOpen);
			m_pActRecent[i]->setToolTip(m_strRecent[i]);
			m_pActRecent[i]->setCheckable(true);
			m_pActRecent[i]->setChecked(false);
		}
	}

	if (nullptr == m_pActRecent[0])
		subMenu->addAction(QString("Haven't opened any files recently."));

}

/*@function SwapRecentFile *recursive function*
*@brief 최근에 참조한 파일 인덱스 변경 (reg, menu, action,,,)
*@param oldIndex 기존 index
*@param newIndex 변경할 index
*@param exIndex recursive 제외 index 구분 (==oldIndex 일 경우, org swap)
*/
void MedipQT::SwapRecentFile(int oldIndex, int newIndex, int exIndex)
{
	if (oldIndex >= RECENT_MAX) return;

	static bool indexEmpty[RECENT_MAX] = { false, };

	for (int i = 0; i < RECENT_MAX; i++)
	{
		if (!indexEmpty[i])
		{
			indexEmpty[i] = true;
			newIndex = i;
			break;
		}
	}

	if (newIndex > exIndex)
		return;

	QString tmpName = m_strRecent[oldIndex];
	bool needRecur = (newIndex < RECENT_MAX) && (m_pActRecent[newIndex] != nullptr);

	if (needRecur)
		SwapRecentFile(newIndex, newIndex + 1, exIndex);
	else if (newIndex >= RECENT_MAX)
	{
		DeleteRecentFile(oldIndex);
		return;
	}

	HKEY recentKey = 0;
	LONG ret;
	if (!m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY).compare(m_pProductManager->m_strMedipDeepCatch)
		|| !m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY).compare(m_pProductManager->m_strMedipDeepCatchV2))
	{
		ret = RegCreateKeyEx(HKEY_CURRENT_USER, (L"Software\\Medicalip\\DeepCatch\\Recent files"),
			0, nullptr, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr, &recentKey, nullptr);
	}
	else
	{
		ret = RegCreateKeyEx(HKEY_CURRENT_USER, (L"Software\\Medicalip\\MEDIP\\Recent files"),
			0, nullptr, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr, &recentKey, nullptr);
	}

	if (ERROR_SUCCESS == ret)
	{
		DWORD pathSize;
		QString prePath;

		m_strRecent[newIndex] = tmpName;
		QString recentFile = QString("File%1\0").arg(newIndex + 1);
		pathSize = (m_strRecent[newIndex].size() + 2) * sizeof(QChar);
		ret = RegSetValueEx(recentKey, (const wchar_t*)recentFile.utf16(), 0, REG_SZ,
			(const BYTE*)m_strRecent[newIndex].constData(), pathSize);

		RegCloseKey(recentKey);

		if (ERROR_SUCCESS == ret && oldIndex == exIndex)
		{
			for (int i = 0; i < RECENT_MAX; i++)
				indexEmpty[i] = false;
			UpdateRecentFile();
		}
	}
}

bool MedipQT::OnEsc()
{
	m_pWindowManager->escKeyProcess();
	return true;
}

void MedipQT::OnServerConnect()
{
	if (m_pDataContext->volume_data.isValidate() == false) return;

	// 	if (QMessageBox::warning(this, STRING_MANAGER->getString(STR_FILE_TRANSFER),
	// 		STRING_MANAGER->getString(STR_TRANSFER_DESC),
	// 		QMessageBox::Ok | QMessageBox::Cancel) != QMessageBox::Ok)
	// 		return;
	QMessageBox messageBox;
	QString path = STRING_MANAGER->programPath + "/agree.jpg";
	messageBox.setIconPixmap(QPixmap(path));
	messageBox.setWindowTitle(STRING_MANAGER->getString(STR_FILE_TRANSFER));

	messageBox.addButton(QMessageBox::Ok);
	messageBox.addButton(QMessageBox::Cancel);
	messageBox.setDefaultButton(QMessageBox::Cancel);

	int ret = messageBox.exec();

	if (ret == QMessageBox::Ok)
	{
		QDateTime tmpdate;
		QString server_str = "time.google.com";
		if (!network::GetNTPDateTime(tmpdate, server_str, 123))
		{
			QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DU_1015)).exec();
			return;
		}

		QString strLatest;
		bool latest = m_pWindowManager->lastestPathGet(strLatest);

		if (m_pWindowManager->fileSave(strLatest) == false)
		{
			if (OnMenuFileSaveAs() == false)
			{
			}
				return;
		}

		latest = m_pWindowManager->lastestPathGet(strLatest);

		if (latest)
		{
			m_pActionManager->SetAfterThread(THREAD_UPLOAD);
			m_pActionManager->action_ftpUpload(strLatest);
		}
	}
}

void MedipQT::OnWebConnect()
{

}


bool MedipQT::useCheck()
{
#ifndef _DEBUG
	if (m_pWindowManager->IsLicensePass())
	{
		std::string mac_address;
		bool bCancel;
		int res;
		if ((bCancel = network::GetMacAddress(mac_address)) == true)
		{
			nsFTP::CFTPClient ftpClient(nsSocket::CreateDefaultBlockingSocketInstance(), 30);
			nsFTP::CLogonInfo logonInfo(tstring(L"log.medicalip.net"), nsFTP::DEFAULT_FTP_PORT, tstring(L"medip_edu"), tstring(L"mmmdc!!!")); //check 기본 홈
			//nsFTP::CLogonInfo logonInfo(tstring(L"147.46.191.217"), nsFTP::DEFAULT_FTP_PORT, tstring(L"medip_edu"), tstring(L"mmmdc!!!")); //check 기본 홈
			ftpClient.Login(logonInfo);
			/*허용되지 않는 Mac address 폴더 존재 시, return 처리*/
			tstring wmac_address;
			wmac_address.assign(mac_address.begin(), mac_address.end());
			res = ftpClient.ChangeWorkingDirectory(wmac_address);

			if (nsFTP::FTP_OK == res)
			{
				return false;
			}
			ftpClient.Logout();
		}
	}
	else
	{/*로그 기록용*/
		nsFTP::CFTPClient ftpClient(nsSocket::CreateDefaultBlockingSocketInstance(), 30);
		nsFTP::CLogonInfo logonInfo(tstring(L"log.medicalip.net"), nsFTP::DEFAULT_FTP_PORT, tstring(L"medipcheck"), tstring(L"mdc!!!")); //check 기본 홈
		//nsFTP::CLogonInfo logonInfo(tstring(L"147.46.191.217"), nsFTP::DEFAULT_FTP_PORT, tstring(L"medipcheck"), tstring(L"mdc!!!")); //check 기본 홈
		ftpClient.Login(logonInfo);
		ftpClient.Logout();
	}
#endif	
	return true;
}

void MedipQT::OnMenuAbout()
{
	QMessageBox::about(this, tr("About Application"),
		tr("The <b>Application</b> example demonstrates how to "
			"write modern GUI applications using Qt, with a menu bar, "
			"toolbars, and a status bar."));
}

void MedipQT::OnMainTabChanged(int index)
{
	int nTabIndexData = m_pTab->tabData(index).toInt();

	if (!m_pDataContext->volume_data.isValidate() && (nTabIndexData != MAINTAB_MESH_EDITING) && (nTabIndexData != MAINTAB_SEGMENTATION))
	{
		m_pTab->setCurrentIndex((int)m_pWindowManager->mainTabType);
		QMessageBox::warning(this, QString("files open"), STRING_MANAGER->getString(STR_LOAD_FIRST));
		return;
	}

	MAINTAB_TYPE preType = m_pWindowManager->mainTabType;

	if (nTabIndexData == MAINTAB_MEASUREMENT)
	{
		if (nullptr == m_pWindowManager->mainAnalWidget)
		{
			//m_pWindowManager->mainAnalWidget = new MainAnalWidget(this);
			m_pWindowManager->mainAnalWidget = m_pProductFactory->createWidget<MainAnalWidget>(MFL_Common_Measurement_GeneralFunction, this);
			m_pMainTab[MAINTAB_MEASUREMENT] = m_pWindowManager->mainAnalWidget;
			if (m_pWindowManager->mainAnalWidget)
			{
				m_pWindowManager->mainAnalWidget->init(m_pDataContext);
				m_pWindowManager->mainAnalWidget->setBeginScreen();
			}

			if (m_pWindowManager->mainAnalWidget != nullptr && m_pDataContext->volume_data.isValidate())
			{
				if (m_pWindowManager->getWorkMode() == WORK_CAPTURE)
					m_pWindowManager->mainAnalWidget->setWorkMode(ANAL_WORK_CAPTURE);
				else if (m_pWindowManager->getWorkMode() == WORK_SKETCHDRAWSEGMENTATION)
					m_pWindowManager->mainAnalWidget->setWorkMode(ANAL_WORK_SKETCHDRAWSEGMENTATION);
				else if (m_pWindowManager->getWorkMode() == WORK_ANNOTATION_PATH)
					m_pWindowManager->mainAnalWidget->setWorkMode(ANAL_WORK_PATH);
				else if (m_pWindowManager->getWorkMode() == WORK_ANNOTATION_PATH_PLAY)
					m_pWindowManager->mainAnalWidget->setWorkMode(ANAL_WORK_PATH_3D_PLAY);

				//if ((nullptr == m_pWindowManager->mainTAWidget) && m_pWindowManager->getWheelZoom())
				//	m_pWindowManager->setWheelZoom(m_pWindowManager->getWheelZoom());
			}
		}

		m_pDataContext->volume_data.forceUpdateMaskVolume();
		m_pWindowManager->updatePlaneData_all();
		m_pWindowManager->renderLater_GridView();
	}

	if (nTabIndexData == MAINTAB_TA)
	{
		if (nullptr == m_pWindowManager->mainTAWidget)
		{
			if (!m_pWindowManager->IsLicensePass())
			{
				QMessageBox::warning(this, tr("Not Supported"), tr("Do not Supported in current version.\n(Edu version or later is supported.)"));
				m_pTab->setCurrentIndex(preType);
				return;
			}

			//m_pWindowManager->mainTAWidget = new MainTAWidget(this);
			m_pWindowManager->mainTAWidget = m_pProductFactory->createWidget<MainTAWidget>(MFL_Common_Radiomics, this);
			m_pMainTab[MAINTAB_TA] = m_pWindowManager->mainTAWidget;
			if (m_pWindowManager->mainTAWidget)
			{
				m_pWindowManager->mainTAWidget->init(m_pDataContext);
				//m_pWindowManager->mainTAWidget->resetUI();
			}
		}

		m_pWindowManager->mainTAWidget->setWorkMode(RADIOMICS_WORK_NONE);

	}

	if (m_pWindowManager->mainTabType == MAINTAB_MESH_EDITING && (m_pActionManager != nullptr && !m_pActionManager->isActionFinished()))
	{
		m_pTab->setCurrentIndex(MAINTAB_MESH_EDITING);
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}


	if (nTabIndexData == MAINTAB_MESH_EDITING && nullptr == m_pWindowManager->mainMeshWidget)
	{
		//m_pWindowManager->mainMeshWidget = new MainMeshWidget(this);
		m_pWindowManager->mainMeshWidget = m_pProductFactory->createWidget<MainMeshWidget>(MFL_Common_MeshEditing_GeneralFunction, this);
		m_pMainTab[MAINTAB_MESH_EDITING] = m_pWindowManager->mainMeshWidget;
		if (m_pWindowManager->mainMeshWidget)
		{
			m_pWindowManager->mainMeshWidget->Init(m_pDataContext);
		}
	}

	if (nTabIndexData == MAINTAB_REPORT)
	{
		if (m_pMainTab[MAINTAB_REPORT] == nullptr)
		{
			if (!m_pWindowManager->IsLicensePass())
			{
				QMessageBox::warning(this, tr("Not Supported"), tr("Do not Supported in current version.\n(Edu version or later is supported.)"));
				m_pTab->setCurrentIndex(preType);
				return;
			}
			createReportTab();
		}
	}

	//#if defined(COVID19_VER)
	if (!(m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare("COVID19"))
	{
		m_pActionManager->action_COVID19_FeatureExtract();
	}
	//#endif

	m_pWindowManager->setSharedPrefer(preType);

	if (m_pMainTab[preType])
		m_pMainTab[preType]->deactivate();

	m_pWindowManager->mainTabType = (MAINTAB_TYPE)nTabIndexData;

	if (m_pMainTab[nTabIndexData])
		m_pMainTab[nTabIndexData]->activate();

	if (m_pWindowManager->GetTab())
	{
		AnnotationTab2* tab = m_pWindowManager->GetTab()->getAnnoTab();

		if (tab)
		{
			tab->updateOpenState(preType);
			tab->setOpenState(m_pWindowManager->mainTabType);
		}
	}

#if !defined(COVID19_VER)
	if (m_pWindowManager->mainTabType != MAINTAB_SEGMENTATION)
	{
		if (m_pActCrossSection) m_pActCrossSection->setVisible(false);
		if (m_pActHistogram) m_pActHistogram->setVisible(false);
	}
	else
	{
		if (m_pActCrossSection) m_pActCrossSection->setVisible(true);
		if (m_pActHistogram) m_pActHistogram->setVisible(true);

		m_pWindowManager->updatePlaneData_all();
	}
#endif

	switch (m_pWindowManager->mainTabType)
	{
	case MAINTAB_SEGMENTATION:
	{
		if (m_pWindowManager->mainSegmentWidget)
		{
			m_pWindowManager->mainSegmentWidget->updateSliderPosition();
			m_pWindowManager->mainSegmentWidget->updateMeshOutline();
		}
	}
	break;
	case MAINTAB_TA:
	{
		if (m_pWindowManager->mainTAWidget)
		{
			m_pWindowManager->mainTAWidget->updateSliderPosition();
		}
	}
	break;
	case MAINTAB_MEASUREMENT:
	{
		if (m_pWindowManager->mainAnalWidget)
		{
			m_pWindowManager->mainAnalWidget->updatePlaneState();
			m_pWindowManager->mainAnalWidget->updateMeshOutline();
		}
	}
	break;
	default:	break;
	}

	m_pWindowManager->GetTab()->SetMaintabType(m_pWindowManager->mainTabType);
	if (m_pWindowManager->mainTabType == MAINTAB_MEASUREMENT ||
		m_pWindowManager->mainTabType == MAINTAB_TA)
		if (m_pActHistogram) m_pActHistogram->setVisible(true);

	//if (m_pWindowManager->mainTabType == MAINTAB_TA)
	//	OnScreentoBegin();

	m_pWindowManager->renderLater_All();

	//20201112_byPHS_tab넘어가면 Dlg 종료
	if (m_pWindowManager->mainTabType != MAINTAB_MESH_EDITING)
	{
		//m_pWindowManager->rejectMEViewDlg();
		MESH_DIALOG_MANAGER->closeDialog();
	}
	else
	{
		// mode 에 따라 다이얼로그 생성
		MESH_WORK_MODE mode = MESH_WORK_MANAGER->getWorkMode();
		MESH_DIALOG_MANAGER->makeMeshDialog(mode);
	}
}

void MedipQT::moveToReportTab()
{
	if (m_nReportTabIdx != -1)
		m_pTab->setCurrentIndex(m_nReportTabIdx);
}

//#ifdef SUPPORT_DEEPCATCH_V2_DOCTORANSWER
bool MedipQT::getChartImg(QImage& outImg)
{

	if (m_pActionManager->getDeepCatchAnalysisVals() == nullptr)
		return false;

	outImg = m_pActionManager->getDeepCatchAnalysisVals()->chartImg;
	return true;
}

bool MedipQT::getHCCResult(std::vector<float>& vec)
{
	if (m_pActionManager->getDeepCatchAnalysisVals() == nullptr || m_pActionManager->getDeepCatchAnalysisVals()->HCC_Result_Percentage.size() < 1)
		return false;

	vec.swap(m_pActionManager->getDeepCatchAnalysisVals()->HCC_Result_Percentage);
	return true;
}

void MedipQT::OnTabSegment()
{
	MAINTAB_TYPE preType = m_pWindowManager->mainTabType;

	m_pWindowManager->mainHLayout->removeWidget(m_pWidgetMain);

	m_pWindowManager->mainHLayout->addWidget(m_pWidgetMain);
	m_pWindowManager->mainHLayout->addWidget(m_pWindowManager->mainSegmentWidget);
	m_pWindowManager->mainSegmentWidget->show();

	m_pWindowManager->mainTabType = MAINTAB_SEGMENTATION;
}

void MedipQT::OnScreentoBegin()
{
	if (!m_pDataContext->volume_data.isValidate()) return;

	if (m_pWindowManager->mainTabType == MAINTAB_SEGMENTATION || m_pWindowManager->mainTabType == MAINTAB_TA)
	{
		BoundingBoxI bound;
		bound.reset(m_pDataContext->volume_data.getCX(), m_pDataContext->volume_data.getCY(), m_pDataContext->volume_data.getCZ());

		if (bound != m_pDataContext->volume_data.getBoundingBox())
		{
			m_pDataContext->volume_data.setPreBoundingBox();
			m_pActionManager->action_BoundingBox_Modify(bound);
		}

		if (m_pWindowManager->mainTabType == MAINTAB_SEGMENTATION)
		{
			if (m_pWindowManager->mainSegmentWidget)
			{
				m_pWindowManager->mainSegmentWidget->setBeginScreen();
			}
		}
		else
		{
			if (m_pWindowManager->mainTAWidget)
			{
				m_pWindowManager->mainTAWidget->setBeginScreen();
			}
		}
	}
	if (m_pWindowManager->mainTabType == MAINTAB_MEASUREMENT)
		//else if (m_pWindowManager->mainTabType == MAINTAB_MEASUREMENT ||
		//	m_pWindowManager->mainTabType == MAINTAB_TA)
	{
		//if (m_pWindowManager->mainAnalWidget)
		m_pWindowManager->mainAnalWidget->setBeginScreen();
		//if (m_pWindowManager->mainTAWidget)
		//	m_pWindowManager->mainTAWidget->setBeginScreen();

		m_pDataContext->volume_data.init3DPlanes();
		m_pDataContext->volume_data.forceUpdateVolume();
	}
	else if (m_pWindowManager->mainTabType == MAINTAB_MESH_EDITING)
	{
		if (m_pWindowManager->mainMeshWidget)
			m_pWindowManager->mainMeshWidget->resetUI();
	}

	if (m_pWindowManager->GetTab())
	{
		ThreSholdTab* tab = m_pWindowManager->GetTab()->getThreSholdTab();

		if (tab)
			tab->Reset();
	}
}

void MedipQT::OnShowTab()
{
	static QIcon actIcon = RESOURCE_MANAGER->getIcon(ICON_TAB, IconSize, IconSize);
	static QIcon inactIcon = RESOURCE_MANAGER->getIcon(ICON_NON_TAB, IconSize, IconSize);
	BOOL res = FALSE;
	if (m_pBtnCollapseActive->isChecked())
	{
		m_pBtnCollapseActive->setIcon(actIcon);
		m_pDockCollapseSeg->show();
		res = TRUE;
	}
	else
	{
		m_pBtnCollapseActive->setIcon(inactIcon);
		m_pDockCollapseSeg->hide();
		res = FALSE;
	}

	m_pWindowManager->setConfigValue(ELEMENT_FILE, STRING_MANAGER->config_DockShow, QString::number(res));

}

void MedipQT::OnUndoStateChange()
{
	static QIcon undoLeave = RESOURCE_MANAGER->getIcon(ICON_UNDO, IconSize, IconSize);
	static QIcon undoInactive = RESOURCE_MANAGER->getIcon(ICON_NON_UNDO, IconSize, IconSize);

	if (m_pActionManager->getUndoStack()->canUndo())
	{
		m_pBtnUndo->setCheckable(true);
		m_pBtnUndo->setIcon(undoLeave);
	}
	else
	{
		m_pBtnUndo->setCheckable(false);
		m_pBtnUndo->setIcon(undoInactive);
	}
}

void MedipQT::OnUndoClick()
{
	if (!m_pActionManager->isActionFinished())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	if (m_pBtnUndo->isCheckable())
	{
		m_pActionManager->getUndoStack()->undo();
	}
}

void MedipQT::OnRedoStateChange()
{
	static QIcon redoLeave = RESOURCE_MANAGER->getIcon(ICON_REDO, IconSize, IconSize);
	static QIcon redoInactive = RESOURCE_MANAGER->getIcon(ICON_NON_REDO, IconSize, IconSize);

	if (m_pActionManager->getUndoStack()->canRedo())
	{
		m_pBtnRedo->setCheckable(true);
		m_pBtnRedo->setIcon(redoLeave);
	}
	else
	{
		m_pBtnRedo->setCheckable(false);
		m_pBtnRedo->setIcon(redoInactive);
	}
}

void MedipQT::OnRedoClick()
{
	if (!m_pActionManager->isActionFinished())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	if (m_pBtnRedo->isCheckable())
	{
		m_pActionManager->getUndoStack()->redo();
	}
}

void MedipQT::OnInfoClick()
{
	InfoDialog info(this);

	int r = info.exec();

	if (r == LINK_CLICKED)
	{
		if (m_pTab)
			OnLinkClick();
		else
			m_pTabBtnWeb->click();
	}
	else if (r == MAIL_LINK_CLICKED)
	{
		QDesktopServices::openUrl(QUrl(QString("mailto:contact@medicalip.com")));
	}

}

void MedipQT::OnHistoClick()
{
	if (!m_pDataContext->volume_data.isValidate()) return;

	if (m_pWindowManager->mainTabType != MAINTAB_SEGMENTATION &&
		m_pWindowManager->mainTabType != MAINTAB_MEASUREMENT) return;

	static QIcon histoPress = RESOURCE_MANAGER->getIcon(ICON_HISTOGRAM, IconSize, IconSize);
	static QIcon histoLeave = RESOURCE_MANAGER->getIcon(ICON_NON_HISTOGRAM, IconSize, IconSize);

	bool bHisto = m_pWindowManager->mainSegmentWidget->isVisibleHistogram();
	m_pWindowManager->mainSegmentWidget->setVisibleHistogram(!bHisto);

	if (bHisto) //현재 상태 : hide
	{
		if (m_pBtnHistogram)
			m_pBtnHistogram->setIcon(histoLeave);

		m_pWindowManager->GetTab()->getThreSholdTab()->setPreviewThreshold(false);
	}
	else
	{
		if (m_pBtnHistogram)
			m_pBtnHistogram->setIcon(histoPress);

		m_pWindowManager->GetTab()->getThreSholdTab()->setPreviewThreshold(true);
	}

}

void MedipQT::OnShowImageList()
{
	static QIcon imageListPress = RESOURCE_MANAGER->getIcon(ICON_IMAGE_MANAGEMENT_TAB, IconSize, IconSize);
	static QIcon imageListLeave = RESOURCE_MANAGER->getIcon(ICON_NON_IMAGE_MANAGEMENT_TAB, IconSize, IconSize);

	bool res = false;
	if (m_pBtnImageList)
	{
		if (m_pBtnImageList->isChecked())
		{
			m_pBtnImageList->setIcon(imageListPress);
			m_pDockImage->show();
			res = true;
		}
		else
		{
			m_pBtnImageList->setIcon(imageListLeave);
			m_pDockImage->hide();
			res = false;
		}
	}

	m_pWindowManager->setConfigValue(ELEMENT_FILE, STRING_MANAGER->config_imageListShow, QString::number(res));
}

void MedipQT::OnUpdateReport()
{
	if (!m_pWindowManager->tempReportData.isEmpty())
	{
		if (nullptr == m_pWindowManager->mainReportWidget) return;

		m_pWindowManager->mainReportWidget->loadFromMip(m_pWindowManager->tempReportData.getFileName(),
			m_pWindowManager->tempReportData.getImageList(), m_pWindowManager->tempReportData.getHTML());
		m_pWindowManager->tempReportData.clear();
	}
}

void MedipQT::OnSViewClick()
{
	if (!m_pDataContext->volume_data.isValidate()) return;

	if (m_pWindowManager->mainTabType != MAINTAB_SEGMENTATION) return;

	static QIcon sviewPress = RESOURCE_MANAGER->getIcon(ICON_SVIEW, IconSize, IconSize);
	static QIcon sviewLeave = RESOURCE_MANAGER->getIcon(ICON_NON_SVIEW, IconSize, IconSize);

	bool isDelete = m_pWindowManager->mainSegmentWidget->isExistSView();

	if (isDelete)
	{
		m_pWindowManager->mainSegmentWidget->deleteCenterWindows();
		if (m_pBtnCrossSectionView) m_pBtnCrossSectionView->setIcon(sviewLeave);
	}
	else
	{
		m_pWindowManager->mainSegmentWidget->createCenterWindows();
		if (m_pBtnCrossSectionView) m_pBtnCrossSectionView->setIcon(sviewPress);
	}
}

void MedipQT::OnLinkClick()
{
	QDesktopServices::openUrl(QUrl(QString("http://www.medicalip.com/")));
}

void MedipQT::OnBarAreaChange()
{
	QString basicVal;
	static QString value = m_pWindowManager->getConfigValue(ELEMENT_FILE, STRING_MANAGER->config_ToolArea, basicVal) == true ? basicVal : QString::number(Qt::ToolBarArea::LeftToolBarArea);
	static Qt::ToolBarArea area = (Qt::ToolBarArea)value.toInt();

	if (area != this->toolBarArea(m_pToolBarFile))
	{
		area = this->toolBarArea(m_pToolBarFile);
		value = QString::number(area);
		m_pWindowManager->setConfigValue(ELEMENT_FILE, STRING_MANAGER->config_ToolArea, value);
	}

}

void MedipQT::OnBreakLock()
{
	m_pActionManager->BreakLock();
}

void MedipQT::OnVisibleLayers()
{
	if (!m_pDataContext->volume_data.isValidate())
	{
		return;
	}

	MeshTab* tabMesh = m_pWindowManager->GetTab()->getMeshTab();
	ROITab2* tabROI = m_pWindowManager->GetTab()->getROITab();

	/* Mesh Editing Tab에서는 항상 Mesh 리스트 Show/Hide 상태 Toggle*/
	if (m_pWindowManager->mainTabType == MAINTAB_MESH_EDITING)
	{
		if (tabMesh)
		{
			MeshListWidget* meshList = tabMesh->GetMeshList();
			if (meshList)
			{
				meshList->ToggleShowState();
			}
		}
	}
	else
	{
		bool bMEDIP = !m_pLicenseManager->getProductType().compare(PRODUCT_NAME_MEDIP);

		if (bMEDIP)
		{
			/* Mesh 리스트가 활성화되어 있으면 Mesh Show/Hide 상태 Toggle*/
			if (tabMesh == nullptr)
			{
				return;
			}

			MeshListWidget* meshList = tabMesh->GetMeshList();
			if (meshList)
			{
				if (meshList->hasFocus())
				{
					meshList->ToggleShowState();
				}
				else
				{
					/* 그외에는 Mask(ROI) Show/Hide 상태 Toggle*/
					if (tabROI)
					{
						tabROI->invertShowState();
					}
				}
			}
		}
		else
		{
			/* 그외에는 Mask(ROI) Show/Hide 상태 Toggle*/
			if (tabROI)
			{
				tabROI->invertShowState();
			}
		}
	}
}

void MedipQT::OnSegDockFloating(bool floating)
{
	int Right = 1;
	if (m_pDockCollapseSeg->isFloating())
	{
		m_pDockCollapseSeg->setMinimumToScreen();
	}
	m_pWindowManager->MoveMeshDialog();
}

void MedipQT::OnSegDockAreaChange(Qt::DockWidgetArea changeArea)
{
	QString basicVal;
	static QString AreaVal = m_pWindowManager->getConfigValue(ELEMENT_FILE, STRING_MANAGER->config_SegDockArea, basicVal) == true ? basicVal : QString::number(Qt::DockWidgetArea::RightDockWidgetArea);
	static Qt::DockWidgetArea area = (Qt::DockWidgetArea)(AreaVal.toInt());
	static QString TopVal = m_pWindowManager->getConfigValue(ELEMENT_FILE, STRING_MANAGER->config_DockTop, basicVal) == true ? basicVal : QString::number(TRUE);
	static bool top = TopVal.toInt();

	if (area != changeArea)
	{
		area = changeArea;
		AreaVal = QString::number(area);
		m_pWindowManager->setConfigValue(ELEMENT_FILE, STRING_MANAGER->config_SegDockArea, AreaVal);
	}
	m_pWindowManager->MoveMeshDialog();
}

void MedipQT::slot_updateMeshUI(bool _b_refresh, int _selIndex, bool _b_clear)
{
	int nMesh = m_pDataContext->m_MeshData.GetMeshCount();

	//if ((m_pWindowManager->vt_pckID.size() < 1) || (m_pWindowManager->vt_pckID.size() <= _selIndex))
	if ((nMesh < 1) || (nMesh <= _selIndex))
	{
		return;
	}

	//m_pWindowManager->vt_pckID[_selIndex] = true;
	m_pDataContext->m_MeshData.SetMeshInfoModeSelectMode(_selIndex, true);

	m_pWindowManager->updateMeshUI(_b_refresh, _selIndex, _b_clear);

	m_pWindowManager->moveMeshCenterScene(_selIndex);

	m_pWindowManager->renderLater_3DView();
}

// 20201012_byPHS
void MedipQT::showQMessageDlg(int _Rstring, QString _caseName)
{
	QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString((RSTRING)_Rstring) + " - " + _caseName);
}

bool MedipQT::eventFilter(QObject* target, QEvent* e)
{
	static QIcon openHover = RESOURCE_MANAGER->getIcon(ICON_FILE_OPEN_HOVER, IconSize, IconSize);
	static QIcon openLeave = RESOURCE_MANAGER->getIcon(ICON_FILE_NON_OPEN, IconSize, IconSize);
	static QIcon openPress = RESOURCE_MANAGER->getIcon(ICON_FILE_OPEN, IconSize, IconSize);
#ifdef DEV_FILE_MANAGER
	static QIcon openFileManagerHover = RESOURCE_MANAGER->getIcon(ICON_FILE_OPEN_FILEMANAGER_HOVER, IconSize, IconSize);
	static QIcon openFileManagerLeave = RESOURCE_MANAGER->getIcon(ICON_FILE_NON_OPEN_FILEMANAGER, IconSize, IconSize);
	static QIcon openFileManagerPress = RESOURCE_MANAGER->getIcon(ICON_FILE_OPEN_FILEMANAGER, IconSize, IconSize);
#endif

	static QIcon cropHover = RESOURCE_MANAGER->getIcon(ICON_CROP_OPEN_HOVER, IconSize, IconSize);
	static QIcon cropLeave = RESOURCE_MANAGER->getIcon(ICON_CROP_NON_OPEN, IconSize, IconSize);
	static QIcon cropPress = RESOURCE_MANAGER->getIcon(ICON_CROP_OPEN, IconSize, IconSize);

	static QIcon saveHover = RESOURCE_MANAGER->getIcon(ICON_FILE_SAVE_HOVER, IconSize, IconSize);
	static QIcon saveLeave = RESOURCE_MANAGER->getIcon(ICON_FILE_NON_SAVE, IconSize, IconSize);
	static QIcon savePress = RESOURCE_MANAGER->getIcon(ICON_FILE_SAVE, IconSize, IconSize);

	static QIcon saveasHover = RESOURCE_MANAGER->getIcon(ICON_FILE_SAVE_AS_HOVER, IconSize, IconSize);
	static QIcon saveasLeave = RESOURCE_MANAGER->getIcon(ICON_FILE_NON_SAVE_AS, IconSize, IconSize);
	static QIcon saveasPress = RESOURCE_MANAGER->getIcon(ICON_FILE_SAVE_AS, IconSize, IconSize);

	static QIcon pacsUploadHover = RESOURCE_MANAGER->getIcon(ICON_PACS_UPLOAD_HOVER, IconSize, IconSize);
	static QIcon pacsUploadLeave = RESOURCE_MANAGER->getIcon(ICON_PACS_NON_UPLOAD, IconSize, IconSize);
	static QIcon pacsUploadPress = RESOURCE_MANAGER->getIcon(ICON_PACS_UPLOAD, IconSize, IconSize);

	static QIcon serverHover = RESOURCE_MANAGER->getIcon(ICON_FILE_UPLOAD_HOVER, IconSize, IconSize);
	static QIcon serverLeave = RESOURCE_MANAGER->getIcon(ICON_FILE_NON_UPLOAD, IconSize, IconSize);
	static QIcon serverPress = RESOURCE_MANAGER->getIcon(ICON_FILE_UPLOAD, IconSize, IconSize);

	static QIcon undoHover = RESOURCE_MANAGER->getIcon(ICON_UNDO_HOVER, IconSize, IconSize);
	static QIcon undoPress = RESOURCE_MANAGER->getIcon(ICON_UNDO_PRESS, IconSize, IconSize);
	static QIcon undoLeave = RESOURCE_MANAGER->getIcon(ICON_UNDO, IconSize, IconSize);

	static QIcon redoHover = RESOURCE_MANAGER->getIcon(ICON_REDO_HOVER, IconSize, IconSize);
	static QIcon redoPress = RESOURCE_MANAGER->getIcon(ICON_REDO_PRESS, IconSize, IconSize);
	static QIcon redoLeave = RESOURCE_MANAGER->getIcon(ICON_REDO, IconSize, IconSize);

	static QIcon infoHover = RESOURCE_MANAGER->getIcon(ICON_APP_INFO_HOVER, IconSize, IconSize);
	static QIcon infoPress = RESOURCE_MANAGER->getIcon(ICON_APP_INFO, IconSize, IconSize);
	static QIcon infoLeave = RESOURCE_MANAGER->getIcon(ICON_NON_APP_INFO, IconSize, IconSize);

	static QIcon linkHover = RESOURCE_MANAGER->getIcon(ICON_LINK_HOVER, IconSize, IconSize);
	static QIcon linkPress = RESOURCE_MANAGER->getIcon(ICON_LINK, IconSize, IconSize);
	static QIcon linkLeave = RESOURCE_MANAGER->getIcon(ICON_NON_LINK, IconSize, IconSize);

	static QIcon histoHover = RESOURCE_MANAGER->getIcon(ICON_HISTOGRAM_HOVER, IconSize, IconSize);
	static QIcon histoPress = RESOURCE_MANAGER->getIcon(ICON_HISTOGRAM, IconSize, IconSize);
	static QIcon histoLeave = RESOURCE_MANAGER->getIcon(ICON_NON_HISTOGRAM, IconSize, IconSize);

	static QIcon sviewHover = RESOURCE_MANAGER->getIcon(ICON_SVIEW_HOVER, IconSize, IconSize);
	static QIcon sviewPress = RESOURCE_MANAGER->getIcon(ICON_SVIEW, IconSize, IconSize);
	static QIcon sviewLeave = RESOURCE_MANAGER->getIcon(ICON_NON_SVIEW, IconSize, IconSize);

	static QIcon rstHover = RESOURCE_MANAGER->getIcon(ICON_RESET_HOVER, IconSize, IconSize);
	static QIcon rstPress = RESOURCE_MANAGER->getIcon(ICON_RESET, IconSize, IconSize);
	static QIcon rstLeave = RESOURCE_MANAGER->getIcon(ICON_NON_RESET, IconSize, IconSize);

	static QIcon hideHover = RESOURCE_MANAGER->getIcon(ICON_TAB_HOVER, IconSize, IconSize);
	static QIcon hideLeave = RESOURCE_MANAGER->getIcon(ICON_NON_TAB, IconSize, IconSize);

	static QIcon imageListHover = RESOURCE_MANAGER->getIcon(ICON_IMAGE_MANAGEMENT_TAB_HOVER, IconSize, IconSize);
	static QIcon imageListLeave = RESOURCE_MANAGER->getIcon(ICON_NON_IMAGE_MANAGEMENT_TAB, IconSize, IconSize);

	// #721 Open 메뉴 오류 수정
	if (m_pMenuOpen == target)
	{
		if (e->type() == QEvent::HideToParent)
		{
			m_pToolBtnOpen->setIcon(openLeave);
		}
		//qDebug() << "QEvent" << QString::number(e->type());		
	}

	if (target == nullptr)
		return QWidget::eventFilter(target, e);

	if (target->objectName() == QString("Maintab"))
	{
		if (e->type() == QEvent::Resize ||
			e->type() == QEvent::ApplicationFontChange)
		{
			QFontMetrics fontMet(this->font());
			QString str = STRING_MANAGER->getString(STR_SEGMENT);
			QRect rect = fontMet.boundingRect(str);
			int w = rect.height() + 5;
			w = w > 20 ? w : 20;
			m_pTab->setMaximumWidth(w);
		}

		if (e->type() == QEvent::Scroll ||
			e->type() == QEvent::Wheel)
			return true;

	}

	if (!(e->type() == QEvent::MouseMove ||
		e->type() == QEvent::MouseButtonPress ||
		e->type() == QEvent::HoverLeave ||
		e->type() == QEvent::MouseButtonRelease))
		return QWidget::eventFilter(target, e);

	if (m_pToolBtnOpen == target)
	{
#ifdef DEV_FILE_MANAGER
		if (e->type() == QEvent::MouseMove)
			m_pToolBtnOpen->setIcon(openFileManagerHover);
		else if (e->type() == QEvent::MouseButtonPress)
			m_pToolBtnOpen->setIcon(openFileManagerPress);
		else if (e->type() == QEvent::HoverLeave ||
			e->type() == QEvent::MouseButtonRelease)
			m_pToolBtnOpen->setIcon(openFileManagerLeave);
#else
		if (e->type() == QEvent::MouseMove)
			m_pToolBtnOpen->setIcon(openHover);
		else if (e->type() == QEvent::MouseButtonPress)
			m_pToolBtnOpen->setIcon(openPress);
		else if (e->type() == QEvent::HoverLeave ||
			e->type() == QEvent::MouseButtonRelease)
			m_pToolBtnOpen->setIcon(openLeave);
#endif
	}
	else if (m_pBtnOpen == target)
	{
#ifdef DEV_FILE_MANAGER
		if (e->type() == QEvent::MouseMove)
			m_pBtnOpen->setIcon(openFileManagerHover);
		else if (e->type() == QEvent::MouseButtonPress)
			m_pBtnOpen->setIcon(openFileManagerPress);
		else if (e->type() == QEvent::HoverLeave ||
			e->type() == QEvent::MouseButtonRelease)
			m_pBtnOpen->setIcon(openFileManagerLeave);
#else
		if (e->type() == QEvent::MouseMove)
			m_pBtnOpen->setIcon(openHover);
		else if (e->type() == QEvent::MouseButtonPress)
			m_pBtnOpen->setIcon(openPress);
		else if (e->type() == QEvent::HoverLeave ||
			e->type() == QEvent::MouseButtonRelease)
			m_pBtnOpen->setIcon(openLeave);
#endif
	}
	else if (m_pBtnCrop == target)
	{
		if (m_pBtnCrop->isEnabled())
		{
			if (e->type() == QEvent::MouseMove)
				m_pBtnCrop->setIcon(cropHover);
			else if (e->type() == QEvent::MouseButtonPress)
				m_pBtnCrop->setIcon(cropPress);
			else if (e->type() == QEvent::HoverLeave ||
				e->type() == QEvent::MouseButtonRelease)
				m_pBtnCrop->setIcon(cropLeave);
		}
		else
		{
			m_pBtnCrop->setIcon(cropLeave);
		}
	}
	else if (m_pBtnSave == target)
	{
		if (e->type() == QEvent::MouseMove)
			m_pBtnSave->setIcon(saveHover);
		else if (e->type() == QEvent::MouseButtonPress)
			m_pBtnSave->setIcon(savePress);
		else if (e->type() == QEvent::HoverLeave ||
			e->type() == QEvent::MouseButtonRelease)
			m_pBtnSave->setIcon(saveLeave);
	}
	else if (m_pBtnSaveAs == target)
	{
		if (e->type() == QEvent::MouseMove)
			m_pBtnSaveAs->setIcon(saveasHover);
		else if (e->type() == QEvent::MouseButtonPress)
			m_pBtnSaveAs->setIcon(saveasPress);
		else if (e->type() == QEvent::HoverLeave ||
			e->type() == QEvent::MouseButtonRelease)
			m_pBtnSaveAs->setIcon(saveasLeave);
	}
	else if (m_pBtnUploadToPACS == target)
	{
		if (e->type() == QEvent::MouseMove)
			m_pBtnUploadToPACS->setIcon(pacsUploadHover);
		else if (e->type() == QEvent::MouseButtonPress)
			m_pBtnUploadToPACS->setIcon(pacsUploadPress);
		else if (e->type() == QEvent::HoverLeave ||
			e->type() == QEvent::MouseButtonRelease)
			m_pBtnUploadToPACS->setIcon(pacsUploadLeave);
	}
	else if (m_pBtnServer == target)
	{
		if (e->type() == QEvent::MouseMove)
			m_pBtnServer->setIcon(serverHover);
		else if (e->type() == QEvent::MouseButtonPress)
			m_pBtnServer->setIcon(serverPress);
		else if (e->type() == QEvent::HoverLeave ||
			e->type() == QEvent::MouseButtonRelease)
			m_pBtnServer->setIcon(serverLeave);
	}
	else if (m_pBtnUndo == target && m_pBtnUndo->isCheckable())
	{
		if (e->type() == QEvent::MouseMove)
			m_pBtnUndo->setIcon(undoHover);
		else if (e->type() == QEvent::MouseButtonPress)
			m_pBtnUndo->setIcon(undoPress);
		else if (e->type() == QEvent::HoverLeave ||
			e->type() == QEvent::MouseButtonRelease)
			m_pBtnUndo->setIcon(undoLeave);
	}
	else if (m_pBtnRedo == target && m_pBtnRedo->isCheckable())
	{
		if (e->type() == QEvent::MouseMove)
			m_pBtnRedo->setIcon(redoHover);
		else if (e->type() == QEvent::MouseButtonPress)
			m_pBtnRedo->setIcon(redoPress);
		else if (e->type() == QEvent::HoverLeave ||
			e->type() == QEvent::MouseButtonRelease)
			m_pBtnRedo->setIcon(redoLeave);
	}
	else if (m_pBtnInfo == target)
	{
		if (e->type() == QEvent::MouseMove)
			m_pBtnInfo->setIcon(infoHover);
		else if (e->type() == QEvent::MouseButtonPress)
			m_pBtnInfo->setIcon(infoPress);
		else if (e->type() == QEvent::HoverLeave ||
			e->type() == QEvent::MouseButtonRelease)
			m_pBtnInfo->setIcon(infoLeave);
	}
	else if (m_pBtnHomepage == target)
	{
		if (e->type() == QEvent::MouseMove)
			m_pBtnHomepage->setIcon(linkHover);
		else if (e->type() == QEvent::MouseButtonPress)
			m_pBtnHomepage->setIcon(linkPress);
		else if (e->type() == QEvent::HoverLeave ||
			e->type() == QEvent::MouseButtonRelease)
			m_pBtnHomepage->setIcon(linkLeave);
	}
	else if (m_pBtnHistogram != nullptr && m_pBtnHistogram == target)
	{
		if (!m_pWindowManager->mainSegmentWidget->isVisibleHistogram())
		{
			if (e->type() == QEvent::HoverLeave ||
				e->type() == QEvent::MouseButtonRelease)
				m_pBtnHistogram->setIcon(histoLeave);
			else if (e->type() == QEvent::MouseMove)
				m_pBtnHistogram->setIcon(histoHover);
		}
	}
	else if (m_pBtnCrossSectionView != nullptr && m_pBtnCrossSectionView == target)
	{
		if (!m_pWindowManager->mainSegmentWidget->isExistSView())
		{
			if (e->type() == QEvent::HoverLeave)
				m_pBtnCrossSectionView->setIcon(sviewLeave);
			else if (e->type() == QEvent::MouseMove)
				m_pBtnCrossSectionView->setIcon(sviewHover);
		}
	}
	else if (m_pBtnResetView == target)
	{
		if (e->type() == QEvent::MouseMove)
			m_pBtnResetView->setIcon(rstHover);
		else if (e->type() == QEvent::MouseButtonPress)
			m_pBtnResetView->setIcon(rstPress);
		else if (e->type() == QEvent::HoverLeave ||
			e->type() == QEvent::MouseButtonRelease)
			m_pBtnResetView->setIcon(rstLeave);
	}
	else if (m_pBtnCollapseActive == target && !m_pBtnCollapseActive->isChecked())
	{
		if (e->type() == QEvent::MouseMove)
			m_pBtnCollapseActive->setIcon(hideHover);
		else if (e->type() == QEvent::HoverLeave)
			m_pBtnCollapseActive->setIcon(hideLeave);
	}
	else if (m_pBtnImageList != nullptr && m_pBtnImageList == target && m_pBtnImageList->isChecked() == false)
	{
		if (e->type() == QEvent::MouseMove)
			m_pBtnImageList->setIcon(imageListHover);
		else if (e->type() == QEvent::HoverLeave)
			m_pBtnImageList->setIcon(imageListLeave);
	}

	return QWidget::eventFilter(target, e);
}

void MedipQT::closeEvent(QCloseEvent* e)
{
	m_pActionManager->SetAfterThread(THREAD_NONE);

	QString strFile;
	m_pWindowManager->lastestPathGet(strFile);

	if (!m_pLicenseManager->isForceLogOutCase())
	{
		if ((!strFile.isEmpty()) && m_pDataContext->volume_data.isValidate())
		{
			QFileInfo file(strFile);

			if (!file.exists())
				m_pWindowManager->setSaveState(false);
		}
		// IgnoreCloseEvent 변수는 macro를 위해서 저장 여부 묻지 않게 하기위함 : 이두희
		if (IgnoreCloseEvent == false && (!m_pWindowManager->getSaveState()) && (!m_pWindowManager->bExit))
		{
			int res;
			if ((res = QMessageBox::warning(this, STRING_MANAGER->getString(STR_SAVE_FILE),
				STRING_MANAGER->getString(STR_SAVE_FILE_DESC),
				QMessageBox::Ok | QMessageBox::No | QMessageBox::Cancel)) == QMessageBox::Ok)
			{
				if (OnMenuFileSave(true))
				{
					m_pWindowManager->bExit = true;
				}
				e->ignore();
				return;
			}
			else if (res == QMessageBox::Cancel)
			{
				e->ignore();
				return;
			}
		}
	}
	else
	{
		if (!IgnoreCloseEvent && m_pDataContext->volume_data.isValidate() && !m_pWindowManager->bExitForceLogoutCase)  // 220705 중복로그인 종료 전 저장루틴 추가
		{
			if (m_pActionManager->isActionFinished())
			{
				int res;
				if ((res = QMessageBox::warning(this, STRING_MANAGER->getString(STR_SAVE_FILE),
					STRING_MANAGER->getString(STR_SAVE_FILE_DESC),
					QMessageBox::Ok | QMessageBox::No | QMessageBox::Cancel)) == QMessageBox::Ok)
				{
					if (saveAs_Logout())
					{
						m_pWindowManager->bExitForceLogoutCase = true;
						e->ignore();
						return;
					}
				}
			}
		}
	}

	emit ProgramClose();
	//m_pDataContext->volume_data.clear();
	m_pDataContext->Clear();

	LOG_MNG->checkLogFiles();	// jhc [2020.12.30] - log files 용량 체크.

#ifdef DEV_FILE_MANAGER
	FILE_MANAGER->App.Deinitialize();
#endif

	// 시스템 종료시에는 SSE UnSubscribe.
	m_pLicenseManager->sseUnSubscribe();

	QMainWindow::closeEvent(e);
}

void MedipQT::onActionStarted(ACTION_PROCESSING id, void* pData)
{
	if (id == ACTION_PROCESSING::ACTP_NONE)
	{
		//skip
	}
	else
	{
		//if (id >= ACTION_PROCESSING::ACTP_MESH_PLANE_CUT && id <= ACTION_PROCESSING::ACTP_MESH_SMOOTH)
		//{
		//	//skip
		//	
		//}
		//else
		//{
		//}
	}
	m_pTab->setEnabled(false);
}

void MedipQT::onActionFinished(ACTION_PROCESSING id, void* pData)
{
	if (id == ACTION_PROCESSING::ACTP_NONE)
	{
		//skip
	}
	else
	{
		//if (id >= ACTION_PROCESSING::ACTP_MESH_PLANE_CUT && id <= ACTION_PROCESSING::ACTP_MESH_SMOOTH)
		//{
		//	//skip
		//}
		//else
		//{
		//}
	}

	m_pTab->setEnabled(true);
}

void MedipQT::resizeEvent(QResizeEvent* event)
{
	replaceMenuIcon();
}

void MedipQT::replaceMenuIcon() //Menu Icon사이 공백 조절 #1405
{
	if (m_pToolBarFile->height() < IconSize * 26)
	{
		m_emptySpace[0]->setFixedSize(IconSize, 0);
		m_emptySpace[1]->setFixedSize(IconSize, 0);
	}
	else
	{
		m_emptySpace[0]->setFixedSize(IconSize, IconSize);
		m_emptySpace[1]->setFixedSize(IconSize, IconSize);
	}
}


void MedipQT::OnProjClearVolume()
{
	//m_pDataContext->volume_data.clear();
	m_pDataContext->Clear();
}

void MedipQT::OnProjupdateAnno()
{
	m_pWindowManager->updateAnnotation();
	m_pWindowManager->updateSeed();
	m_pWindowManager->clearFeatureList();
}

void MedipQT::OnProjcreateHistogram()
{
	m_pWindowManager->mainSegmentWidget->createHUHisto();

	m_pWindowManager->mainSegmentWidget->SetWidthLine();
	m_pWindowManager->mainSegmentWidget->SetWindowRGSlider();
}

void MedipQT::OnProjresetRG()
{
	m_pWindowManager->resetRGValue();
	m_pWindowManager->patchyPoint = QVector3D(-1, -1, -1);

	m_pWindowManager->setPatchyPoint();
}

void MedipQT::OnProjresetwork()
{
	m_pWindowManager->mainSegmentWidget->setWorkMode(WORK_NONE);
	if (m_pWindowManager->mainAnalWidget)
	{
		m_pWindowManager->mainAnalWidget->reInit();
		m_pWindowManager->mainAnalWidget->setWorkMode(ANAL_WORK_NONE);
	}

	if (m_pWindowManager->mainTAWidget)
	{
		m_pWindowManager->mainTAWidget->setWorkMode(RADIOMICS_WORK_NONE);
	}

	BoundingBoxI _box = m_pDataContext->volume_data.getBoundingBox();
	m_pWindowManager->setAIRange();
	m_pWindowManager->setAIDepth(_box);
}

void MedipQT::OnProjsetDepth(int type, float depth)
{
	WindowBase* win = m_pWindowManager->mainSegmentWidget->getWindow((WINDOW_TYPE)type);
	if (win)
		win->setDepth(depth);

	if (m_pWindowManager->mainTAWidget)
	{
		win = m_pWindowManager->mainTAWidget->getWindow((WINDOW_TYPE)type);
		win->setDepth(depth);
	}
}

void MedipQT::OnProjPreset(bool is3D, int _preset, int _cusPreset, int width, int level)
{
	if (is3D)
	{
		m_pWindowManager->setVolumePreset((SLICE_PRESET)_preset, false, _cusPreset);
		m_pWindowManager->setVolumeWidthLevel(width, level);
	}
	else
	{
		m_pWindowManager->setPreset((SLICE_PRESET)_preset, false, _cusPreset);
		m_pWindowManager->setWindowWidthLevel(width, level);
	}
}

void MedipQT::OnProjWheelzoom(bool set)
{
	m_pWindowManager->setWheelZoom(set);
}

void MedipQT::OnProjsetGamma(bool val, bool rst)
{
	m_pWindowManager->setGammaMode(val, rst);
}

void MedipQT::OnProjresetUI()
{
	m_pWindowManager->resetUI();
}

void MedipQT::OnProjresetRC()
{
	if (m_pWindowManager->mainSegmentWidget)
	{
		VolumeView* vView = dynamic_cast<VolumeView*>(m_pWindowManager->mainSegmentWidget->getViewVolume());

		if (vView)
			vView->resetDisplayMode();
	}

	if (m_pWindowManager->mainSegmentWidget)
	{
		m_pWindowManager->mainSegmentWidget->resetResource();
	}

	if (m_pWindowManager->mainTAWidget)
	{
		m_pWindowManager->mainTAWidget->resetResource();
	}

	m_pWindowManager->resetResource();

	if (m_pWindowManager->GetTab())
	{
		{
			ROITab2* tab = m_pWindowManager->GetTab()->getROITab();

			if (tab)
				tab->ClearROIList();
		}

		{
			MeshTab* tab = m_pWindowManager->GetTab()->getMeshTab();

			if (tab)
				tab->Clear();
		}
	}
}

void MedipQT::OnOpenMIPFile()
{
	QString strFilter;
	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Import_MIP))
	{
		strFilter += QString("MEDIP File(*.mip; *.MIP);;");
	}

	QString filepath = ImportFileDialog(this,
		STRING_MANAGER->getString(STR_MEDIP_FILE_OPEN), "", "",
		strFilter
	);
	if (!filepath.isEmpty())
	{
		fileOpen(filepath);
	}
}

void MedipQT::OnOpenDcmFolder()
{
}

void MedipQT::OnImportRawMaskFiles()
{
	if (m_pDataContext->volume_data.isValidate() == false)
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_FAILED_TO) + STRING_MANAGER->getString(STR_LOAD)
			, STRING_MANAGER->getString(STR_LOAD_FIRST));
		return;
	}
	QString filter = "RAW File(*.raw;*.RAW)";
	QStringList filePathList = GetImportFilePathListByFunctionLevel(MFL_Common_Import_NII_RAW_Mask, filter);
	ImportMultiFile_Default(filePathList);
}

void MedipQT::OnImportNIIMaskFiles()
{
	if (m_pDataContext->volume_data.isValidate() == false)
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_FAILED_TO) + STRING_MANAGER->getString(STR_LOAD)
			, STRING_MANAGER->getString(STR_LOAD_FIRST));
		return;
	}

	QString filter = "ROI NII / NII File(*.nii;*.NII)";
	QStringList filePathList = GetImportFilePathListByFunctionLevel(MFL_Common_Import_NII_RAW_Mask, filter);
	ImportMultiFile_Default(filePathList);
}

void MedipQT::OnImportSTLFiles()
{
	QString filter = "STL File(*.stl;*.STL)";
	QStringList filePathList = GetImportFilePathListByFunctionLevel(MFL_Common_MeshEditing_FileImport, filter);
	m_pActionManager->action_FileWork_Import_STL(m_pDataContext, filePathList);
}

void MedipQT::OnImportObjFiles()
{
	QString filter = "Obj File(*.obj;*.OBJ)";
	QStringList filePathList = GetImportFilePathListByFunctionLevel(MFL_Common_MeshEditing_FileImport, filter);
	m_pActionManager->action_FileWork_Import_OBJ(m_pDataContext, filePathList);
}

void MedipQT::OnImportVTKFiles()
{
	QString filter = "VTK File(*.vtk;*.VTK)";
	QStringList filePathList = GetImportFilePathListByFunctionLevel(MFL_Common_MeshEditing_FileImport, filter);
	m_pActionManager->action_FileWork_Import_VTK(m_pDataContext, filePathList);
}

void MedipQT::OnImport3MFFiles()
{
	QString filter = "3mf File(*.3mf;*.3MF)";
	QStringList filePathList = GetImportFilePathListByFunctionLevel(MFL_Common_MeshEditing_FileImport, filter);
	m_pActionManager->action_FileWork_Import_3MF(m_pDataContext, filePathList);
}

void MedipQT::OnImportImageFiles()
{
	if (m_pDataContext->volume_data.isValidate() == false)
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_FAILED_TO) + STRING_MANAGER->getString(STR_LOAD)
			, STRING_MANAGER->getString(STR_LOAD_FIRST));
		return;
	}

	QString filter = "PNG File(*.png;*.PNG);;BMP File(*.bmp;*.BMP);;JPG File(*.jpg;*.JPG);;TXT File(*.txt;*.TXT)";
	QStringList filePathList = GetImportFilePathListByFunctionLevel(MFL_Common_Report_ImageManagement_ImageImport, filter);
	ImportMultiFile_Default(filePathList);
}

void MedipQT::OnImportCustomRenderingPreset()
{
	if (m_pDataContext->volume_data.isValidate() == false)
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_FAILED_TO) + STRING_MANAGER->getString(STR_LOAD)
			, STRING_MANAGER->getString(STR_LOAD_FIRST));
		return;
	}

	QString filter = "Preset File(*.prd;*.PRD)";
	QStringList filePathList = GetImportFilePathListByFunctionLevel(MFL_Common_Rendering_2D_3DHistogram_Presetcustomising, filter);
	ImportMultiFile_Default(filePathList);
}

void MedipQT::OnCloseProject()
{
}

void MedipQT::OnExit()
{
	close();
}


void MedipQT::OnStrProgress(QString str)
{
	QMessageBox box;
	box.setText(str);
	box.exec();
}

void MedipQT::OnProjupdateUI()
{
	m_pWindowManager->updateUI();
	m_pWindowManager->updateMeshUI();
	m_pWindowManager->applyVoxelToUI(0, true);

	m_pWindowManager->volume_renderer.initVolumeSizeInfo(m_pDataContext->volume_data.getSpaceX(), m_pDataContext->volume_data.getSpaceY(), m_pDataContext->volume_data.getSpaceZ()
		, m_pDataContext->volume_data.getCX(), m_pDataContext->volume_data.getCY(), m_pDataContext->volume_data.getCZ());

	/* Volume 원본 초기화 */
	VOLUME_CALCULATOR->Initialize(&m_pDataContext->volume_data);
}

void MedipQT::OnProjupdateSummary()
{
	m_pWindowManager->updateSummary();
}

void MedipQT::OnNIIinitUI(int ww, int wl)
{
	m_pWindowManager->initMedipUIData(ww, wl);
}

void MedipQT::OnThreadEnd()
{
	qDebug() << "m_pWindowManager->bAfterThread  : " << m_pActionManager->GetAfterThread();

	if (m_pDataContext->volume_data.threadStop && (m_pActionManager->GetAfterThread() >= THREAD_PREVIEW_MESHES))
	{
		m_pActionManager->SetAfterThread(THREAD_NONE);
		m_pWindowManager->ClearFileData();
	}

	m_pActionManager->action_ThreadEnd(m_pDataContext);

	/* ERROR Message 처리 추가 */
	if (m_pActionManager->actionResult.IsErrorExist())
	{
		QMessageBox::warning(this,
			m_pActionManager->actionResult.GetErrorTitle(),
			m_pActionManager->actionResult.GetErrorResult());

		m_pActionManager->actionResult.Clear();
		m_pActionManager->SetAfterThread(THREAD_NONE);
	}

	if (m_pActionManager->GetAfterThread() == THREAD_EXPORT_FILES ||
		m_pActionManager->GetAfterThread() == THREAD_BCKUP_TO_RAWS ||
		m_pActionManager->GetAfterThread() == THREAD_EXPORT_MESH_FILES) //export files
	{
		if (m_pWindowManager->exportList.isEmpty())
		{
			m_pActionManager->SetAfterThread(THREAD_NONE);
		}
		else if (m_pActionManager->GetAfterThread() == THREAD_EXPORT_MESH_FILES)
		{
			int _uid = m_pWindowManager->exportList.takeFirst();
			MeshInfo* info = m_pDataContext->m_MeshData.GetMeshInfo(_uid);
			QString filename = m_pWindowManager->exportPath;
			filename.append("/");
			filename.append(QString("%1.").arg(m_pDataContext->m_MeshData.GetMeshName(_uid)));
			m_pWindowManager->SaveMeshFiles(filename, _uid, info->uid);
			return;
		}
		else
		{
			int _uid = m_pWindowManager->exportList.takeFirst();
			MaskInfo* info = m_pDataContext->volume_data.getMaskInfo(_uid, true);
			int mI = _uid >= MASK_SECOND_MAX ? (_uid - MASK_SECOND_MAX) / 8 + 1 : 0;
			QString filename = m_pWindowManager->exportPath;
			filename.append("/");
			filename.append(QString("%1.").arg(m_pDataContext->volume_data.getMaskName(info->uid, true)));
			m_pWindowManager->saveFiles(filename, _uid, mI == 0 ? info->mask_id : info->mask_id2, mI, (m_pActionManager->GetAfterThread() == THREAD_BCKUP_TO_RAWS) ? EX_FILES_RAW : EX_FILES_NONE);
			return;
		}
	}

	if (
		m_pActionManager->GetAfterThread() == THREAD_MEDIP_MACRO_EXPORT_MASK_RAW_FILES ||
		m_pActionManager->GetAfterThread() == THREAD_MEDIP_MACRO_EXPORT_MASK_NII_FILES
		)
	{
		EXPORT_FILES exportFileType = EX_FILES_RAW;
		if (m_pActionManager->GetAfterThread() == THREAD_MEDIP_MACRO_EXPORT_MASK_RAW_FILES)
		{
			exportFileType = EX_FILES_RAW;
		}
		else if (m_pActionManager->GetAfterThread() == THREAD_MEDIP_MACRO_EXPORT_MASK_NII_FILES)
		{
			exportFileType = EX_FILES_NII;
		}

		if (m_pWindowManager->exportList.isEmpty())
		{
			m_pActionManager->NextThreadSetting();
			m_pActionManager->action_EmptyStart(ACTP_NONE);
		}
		else
		{
			int uid = m_pWindowManager->exportList.takeFirst();
			MaskInfo* info = m_pDataContext->volume_data.getMaskInfo(uid, true);
			int mI = uid >= MASK_SECOND_MAX ? (uid - MASK_SECOND_MAX) / 8 + 1 : 0;
			QString filepath = m_pWindowManager->exportPath;
			filepath.append("/");
			filepath.append(QString("%1.").arg(m_pDataContext->volume_data.getMaskName(info->uid, true)));

			m_pWindowManager->saveFiles(filepath, info->uid, mI == 0 ? info->mask_id : info->mask_id2, mI, exportFileType);
		}
	}

	if (m_pWindowManager->bExit)
	{
		emit ProgramClose();
		QMainWindow::close();
		return;
	}

	if (m_pWindowManager->bExitForceLogoutCase)
	{
		emit ProgramClose();
		QMainWindow::close();
		return;
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_MEDIP_Plugin_AIPack))
	{
		if (THREAD_COMPONENT_CHOICE == m_pActionManager->GetAfterThread())
		{
			m_pActionManager->NextThreadSetting();

			OnUpdateProgressMultiThread(0, QString("DeepDraw - Component"));

			MaskInfo* pMaskInfo = m_pDataContext->volume_data.getAtLastMaskInfo();

			int nComponent = 1;
			m_pActionManager->action_ImageComponentChoise(m_pDataContext->volume_data.getMask(pMaskInfo->uid)
				, m_pDataContext->volume_data.GetMaskByteIndex(pMaskInfo->uid), nComponent, false);
		}
		else if (THREAD_DEEPDRAW_PREDICT == m_pActionManager->GetAfterThread())
		{
			m_pActionManager->NextThreadSetting();

			OnUpdateProgressMultiThread(0, QString("DeepDraw - predict"));

			QString strInputPath = STRING_MANAGER->LocalAISegPath + "/predict";
			QString strWeightPath = STRING_MANAGER->AISegmentationPath + "/weight";
			auto strData = std::static_pointer_cast<std::string>(m_pActionManager->m_qThreadNext.front().pTempData);
			QString strFileName = QString::fromStdString(*strData);

			BoundingBoxI box = m_pDataContext->volume_data.getBoundingBox();
			//MaskInfo *maskInfo = m_pDataContext->volume_data.getCurrentMaskInfo();
			int nFilterIdx = m_pActionManager->GetReservationWorkList().size() - 1;

			eMEDIP_FUNCTION_LEVEL AIWeightType = MFL_Common_AI_PredictUsableCount_Credit;
			eDeepPredictAICopyMask copyMaskType = eDeepPredictAICopyMask::none_copy;

			AISegTab* tab = m_pWindowManager->GetTab()->getAITab();
			if (tab)
			{
				AIWeightType = tab->getWeightType();
				copyMaskType = tab->GetCopyMaskType();
			}

			if (m_pActionManager->m_qThreadNext.size() > 1)
			{
				qInfo() << "thread next exist. mask none copy";
				copyMaskType = eDeepPredictAICopyMask::none_copy;
			}

			m_pActionManager->action_Deepdraw_predict(
				strInputPath, strWeightPath, AIWeightType, strFileName, box, true, copyMaskType, false, nFilterIdx);
		}
		else if (THREAD_GMM_EXRACTMASK == m_pActionManager->GetAfterThread())
		{
			m_pActionManager->NextThreadSetting();

			std::list<std::vector<MaskInfo>> generatedNewMaskInfos;
			std::vector<QUndoCommand*> reservationWorkList = m_pActionManager->GetReservationWorkList();

			for (int i = reservationWorkList.size() - 1; i >= 0; --i)
			{
				((ActionPredictAddEx*)(reservationWorkList[i]))->redo();
				generatedNewMaskInfos.push_front(((ActionPredictAddEx*)(reservationWorkList[i]))->getNewMaskInfo());
			}

			m_pActionManager->ClearReservationWorkList();

			AISegTab* tab = m_pWindowManager->GetTab()->getAITab();

			if (tab)
			{
				QString projectName = tab->getProjectName();

				// GMM Mask Extraction 수행
				//m_pActionManager->action_MaskExtractGMM(vecMaskInfo);
				m_pActionManager->action_MaskExtractGMM(projectName, tab->GetClusterNumber());
			}
		}
	}

	//#if defined(COVID19_VER)
	/////////////////////////// thread COVID19 매크로2
	// ACTP_IMAGE_COMPONENT_CHOISE
	if (!((m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(PRODUCT_NAME_MEDIP_COVID19)))
	{
		if (THREAD_COMPONENT_CHOICE == m_pActionManager->GetAfterThread())
		{
			m_pActionManager->NextThreadSetting();

			MaskInfo* pMaskInfo = m_pDataContext->volume_data.getAtLastMaskInfo();

			int nComponent = 2;
			m_pActionManager->action_ImageComponentChoise(m_pDataContext->volume_data.getMask(pMaskInfo->uid)
				, m_pDataContext->volume_data.GetMaskByteIndex(pMaskInfo->uid), nComponent);
		}
		// ACTP_COVID19_PREDICT 
		else if (THREAD_PREDICT_PNEUMONIA == m_pActionManager->GetAfterThread())
		{
			m_pActionManager->NextThreadSetting();

			const QString strInputPath = STRING_MANAGER->LocalAISegPath + "/predict";
			const QString strWeightPath = STRING_MANAGER->AISegmentationPath + "/weight_covid19";

			int start, end;
			m_pWindowManager->GetTab()->getAITab()->GetDepth(&start, &end);

			QString projName = "Pneumonia";

			m_pActionManager->action_COVID19_predict(strInputPath, strWeightPath, projName, start, end, true);
		}
		// ACTP_COVID19_FEATURE_TO_REPORT 
		else if (THREAD_CALCULATE_FEATURE == m_pActionManager->GetAfterThread())
		{
			m_pActionManager->NextThreadSetting();

			// ROI 셋팅
			// default roi 삭제
			MaskInfo* pMaskInfo = m_pDataContext->volume_data.getMaskInfo(0, false);
			std::wstring ws(pMaskInfo->maskName);
			if (ws.find(L"Layer") != string::npos)
				m_pDataContext->volume_data.delMaskInfo(pMaskInfo->uid);

			// ROI show 셋팅		
			ROITab2* pRoiTab = m_pWindowManager->GetTab()->getROITab();

			int nItemCnt = pRoiTab->ROIList->topLevelItemCount();

			for (int i = 0; i < nItemCnt - 2; ++i)
			{
				MaskInfo* info = m_pDataContext->volume_data.getMaskInfo(i);
				if (info)
				{
					QTreeWidgetItem* item = pRoiTab->ROIList->topLevelItem(i);
					info->show = false;
					item->setIcon(L_COL_SHOW, RESOURCE_MANAGER->getIcon(info->show ? ICON_LIST_VISIBLE : ICON_LIST_INVISIBLE));
					pRoiTab->updateROIHeader(L_COL_SHOW);
					m_pWindowManager->forceUpdate2DViewData(false, true);
					m_pWindowManager->renderLater_GridView(true);
				}
			}

			MaskInfo* lungInfo = m_pDataContext->volume_data.getMaskInfo(nItemCnt - 2);
			if (lungInfo)
			{
				lungInfo->layerAlpha = 33;
				lungInfo->color = COLOR(170, 0, 255, 33);
			}

			MaskInfo* pneumoniaInfo = m_pDataContext->volume_data.getMaskInfo(nItemCnt - 1);
			if (pneumoniaInfo)
			{
				pneumoniaInfo->layerAlpha = 130;
				pneumoniaInfo->color = COLOR(50, 217, 48, 130);
			}

			// 마지막 roi selection
			mint32 index = nItemCnt - 1;
			m_pDataContext->volume_data.setCurrentMaskIndex(index);
			pRoiTab->ROIList->SelectionUpdate(index);
			MaskInfo* info = m_pDataContext->volume_data.getMaskInfo(index);
			std::vector<muint32> vecSelect;
			vecSelect.push_back(info->uid);
			m_pDataContext->volume_data.setMultiSelectUID(vecSelect);

			// update
			pRoiTab->updateROIHeader(L_COL_SHOW);
			pRoiTab->ROIList->UpdateByVolumeData();

			m_pWindowManager->forceUpdate2DViewData(false, true);
			m_pWindowManager->renderLater_GridView(true);

			// volume x mask
			Visualize3DTab* p3DTab = WIN_MANAGER->GetTab()->get3DTab();
			p3DTab->slot_OnChangeMode(1);

			// volume view 회전
			setHomePosition(HP_ANTERIOR, true);

			VolumeView* pVolumeView = static_cast<VolumeView*>(WIN_MANAGER->mainSegmentWidget->getViewVolume());
			pVolumeView->slot_OnLeftDisplay();

			// 		/////////////////////////////////////
			// 		// report 			
			m_pActionManager->action_COVID19_FeatureExtract();
		}
		else if (THREAD_VOLUME_PRESET == m_pActionManager->GetAfterThread())
		{
			m_pActionManager->NextThreadSetting();
			// mpr 화면 셋팅
			COVID_REPORT* pReport = m_pActionManager->getCOVID19AnalysisVals();

			if (pReport)
			{
				SegmentationView* pAxialView = static_cast<SegmentationView*>(m_pWindowManager->mainSegmentWidget->getWindow(WT_AXIAL));
				pAxialView->setDepth(pReport->nAxialDepth);

				SegmentationView* pCoronalView = static_cast<SegmentationView*>(m_pWindowManager->mainSegmentWidget->getWindow(WT_CORONAL));
				pCoronalView->setDepth(pReport->nCoronalDepth);

				SegmentationView* pSaggitalView = static_cast<SegmentationView*>(m_pWindowManager->mainSegmentWidget->getWindow(WT_SAGITTAL));
				pSaggitalView->setDepth(pReport->nSaggitalDepth);

				m_pWindowManager->forceUpdate2DViewData(false, true);
				m_pWindowManager->renderLater_GridView(true);
			}

			// volume 셋팅
			for (int i = 0; i < m_pWindowManager->getPresetCount(); ++i)
			{
				QString strName = m_pWindowManager->getPresetName(i);

				if (!strName.compare("COVID19"))
				{
					int preLevel = m_pWindowManager->getVolumeLevel();
					int preWidth = m_pWindowManager->getVolumeWidth();
					//qDebug() << "preset num:" << QString::number(i);

					m_pActionManager->action_Preset(preLevel, preWidth, (SLICE_PRESET)-1, (SLICE_PRESET)i, i);
					m_pActionManager->action_VolumePreset(preLevel, preWidth, (SLICE_PRESET)-1, (SLICE_PRESET)i, i);

					break;
				}
			}

#if defined(COVID19_VER)
			// save csv

			m_pWindowManager->bAfterThread = ACTP_NONE;


			QString InputPath = m_pWindowManager->m_strMacroInputPath;

			QString strTmp = InputPath.left(InputPath.lastIndexOf("\\"));
			QString strDcmFolder = strTmp.right(strTmp.lastIndexOf("\\"));

			QString filePath = m_pWindowManager->m_strMacroOutputPath;

			if (!filePath.isEmpty())
			{
				QString copyStr = "";

				copyStr.append(strDcmFolder);
				copyStr.append("\r\n");
				copyStr.append(",");
				copyStr.append(QString::fromStdString(m_pActionManager->m_pCovid_Report->vecFeature[0].strROIName));
				copyStr.append(",");
				copyStr.append(QString::number(m_pActionManager->m_pCovid_Report->vecFeature[0].fObjectVolume));

				copyStr.append("\r\n");
				copyStr.append(",");
				copyStr.append(QString::fromStdString(m_pActionManager->m_pCovid_Report->vecFeature[1].strROIName));
				copyStr.append(",");
				copyStr.append(QString::number(m_pActionManager->m_pCovid_Report->vecFeature[1].fObjectVolume));
				copyStr.append(",");
				copyStr.append(QString::number(m_pActionManager->m_pCovid_Report->vecFeature[1].fPneumoniaBurden));
				copyStr.append(",");
				copyStr.append(QString::number(m_pActionManager->m_pCovid_Report->vecFeature[1].fRatio));
				copyStr.append("\r\n");


				QFile newFile(filePath);

				if (!newFile.open(QIODevice::WriteOnly | QIODevice::Append))
				{
					QMessageBox::warning(nullptr, "Feature file save fail.", "csv file create fail.");
					return;
				}

				newFile.write((const char*)copyStr.toStdWString().c_str(), sizeof(WCHAR) * copyStr.size());

				//			QMessageBox::warning(this, filePath, "here.");
				newFile.close();
			}
			QApplication::closeAllWindows();

			return;


#endif
				}
			}

	if (!(m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(PRODUCT_NAME_DEEPCATCH)
		|| !(m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(PRODUCT_NAME_DEEPCATCH_V2))
	{
		if (THREAD_COMPONENT_CHOICE == m_pActionManager->GetAfterThread())
		{
			qDebug() << "THREAD_COMPONENT_CHOICE";

			// deepdraw predict 후 ROI색상 변경 가능성 있음
			// 1) Axial축에서 Width의 40 % 에서 60 % 에서 Bone 마스크 남기고 나머지는 0으로 바꾸기
			// 1-1) Bone Mask 복사
			// 1-2) Mask clipping
			// 2) action_component, // 2) Connectivity (가장 큰 마스크 남기기)

			// 			auto dResult = std::static_pointer_cast<int>(m_pActionManager->m_qThreadNext.front().pThreadResult);
			// 			nL3SliceNum = *dResult;
			// 
			// 			dResult.reset();
			// 			m_pActionManager->m_qThreadNext.front().pThreadResult.reset();
			m_pActionManager->NextThreadSetting();

			OnUpdateProgressMultiThread(0, QString("DeepCatch - L3 predict (Input data making)"));

			MaskInfo* pMaskInfo = m_pDataContext->volume_data.findMaskInfo("Bone");
			int nUIDBone = pMaskInfo->uid;

			// 1-1) Bone Mask copy
			int nUIIdxBone = m_pDataContext->volume_data.getMaskIndex(nUIDBone);

			// jhc [2020.12.24] - DeepCatch Predict시 undo/redo 제외 처리.
			//	m_pActionManager->action_MaskList_copy(nUIIdxBone);
			if (m_pDataContext->volume_data.getMaskInfoListCnt() < MASK_MAX && m_pDataContext->volume_data.getMaskInfoListCnt() > nUIIdxBone)
			{
				ActionMaskListCopyAdd maskListCopyAdd(&m_pDataContext->volume_data, nUIIdxBone);
				maskListCopyAdd.redo();
			}

			MaskInfo* pMaskInfoCopy = m_pDataContext->volume_data.getCurrentMaskInfo();
			int nMaskByteIdx = m_pDataContext->volume_data.GetMaskByteIndex(pMaskInfoCopy->uid);
			mask8* pMaskDataCopy = m_pDataContext->volume_data.getMaskDataPoint(nMaskByteIdx);
			mask8 maskBit = m_pDataContext->volume_data.getMask(pMaskInfoCopy->uid);

			// 1-2) Mask clipping
			int cx, cy, cz;
			cx = m_pDataContext->volume_data.getCX();
			cy = m_pDataContext->volume_data.getCY();
			cz = m_pDataContext->volume_data.getCZ();

			int nLeft = 0, nRight = 0;
			nLeft = cx * 0.4;
			nRight = cx * 0.6;

			for (int z = 0; z < cz; ++z)
			{
				for (int y = 0; y < cy; ++y)
				{
					for (int x = 0; x < nLeft; ++x)
					{
						int idx = z * cx * cy + y * cx + x;
						pMaskDataCopy[idx] &= ~maskBit;
					}

					for (int x = nRight; x < cx; ++x)
					{
						int idx = z * cx * cy + y * cx + x;
						pMaskDataCopy[idx] &= ~maskBit;
					}
				}
			}

			int nComponent = 1;
			m_pActionManager->action_ImageComponentChoise(m_pDataContext->volume_data.getMask(pMaskInfoCopy->uid)
				, m_pDataContext->volume_data.GetMaskByteIndex(pMaskInfoCopy->uid), nComponent, false);

		}
		else if (THREAD_L3_USERSELECT == m_pActionManager->GetAfterThread())
		{
			m_pActionManager->NextThreadSetting();
			qDebug() << "THREAD_L3_PREDICT";
			OnUpdateProgressMultiThread(0, QString("DeepCatch - User Selected"));

			//AISegTabDeepCatch* pDeepCatchtab = (AISegTabDeepCatch*)m_pWindowManager->GetTab()->getAITab();
			bool bAutoL3 = false;
			if (m_pActionManager->m_IsMacroMode) // L3
			{
				QMap<QString, sDeepcatchMacroColumnData>::const_iterator iterMap = m_pActionManager->m_ListMacroCommandLine.first().rowMap.find(DEEPCATCH_MACRO_CATEGORY_SLICE);
				if (iterMap != m_pActionManager->m_ListMacroCommandLine.first().rowMap.end())
				{
					sDeepcatchMacroColumnData data = iterMap.value();
					QString strSliceType = data.data.at(0);
					QString strSliceNum = data.data.at(1);

					if (strSliceType.compare(QString("L3")))
						bAutoL3 = true;

					if (!strSliceType.compare(QString("L3")) && !strSliceNum.compare(QString("A")))
						bAutoL3 = true;
				}
			}
			else if (!m_pActionManager->m_IsMacroMode)
				bAutoL3 = true;

			if (bAutoL3)	// L3
			{
				// 3) 하단 clipping
				// 4) Sagittal Projection (평균 값을 계산하기). 조건: // L3 predict의 입력 만들기
				// 5) action_L3_Predict
				if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2))
				{
					//DEEPCATCH_REPORT_PREDICT_INFO* pPredictedInfo = m_pActionManager->DeepcatchReportPredictedInfoSafeCreate();
					AISegTabDeepCatch* pDeepCatchtab = (AISegTabDeepCatch*)m_pWindowManager->GetTab()->getAITab();
					int btnState = pDeepCatchtab->getBoneAnalysis(); // 0: Vertebra Network, 1: nothing				

					if (btnState == eUseVertebraNetwork /*&& THREAD_L3_PREDICT == m_pActionManager->GetAfterThread()*/)
					{
						OnUpdateProgressMultiThread(0, QString("calculate_slice_position"));

						int num = pDeepCatchtab->getCurrentVBComboIndex(eSST_Single);

						std::vector<int> VbNumList;
						VbNumList.push_back(num);
						m_pActionManager->action_calculate_slice_position(VbNumList, ST_SINGLE);
					}
				}
			}
			else
			{
				// Window 화면에서 내부 index 처리를 0~ 부터 시작하기 때문에 -1 해줘야 함.
				int nL3Num = -1;
				QMap<QString, sDeepcatchMacroColumnData>::const_iterator iterMap = m_pActionManager->m_ListMacroCommandLine.first().rowMap.find(DEEPCATCH_MACRO_CATEGORY_SLICE);
				if (iterMap != m_pActionManager->m_ListMacroCommandLine.first().rowMap.end())
				{
					sDeepcatchMacroColumnData data = iterMap.value();
					QString strSliceNum = data.data.at(1);

					nL3Num = strSliceNum.toInt() - 1;
				}
				m_pActionManager->m_qThreadNext.front().pTempData = std::make_shared<int>(nL3Num);
				qDebug() << "marcro L3 : " << nL3Num;
				m_pActionManager->action_EmptyStart(ACTP_L3_USERSELECT);
			}

		}
		else if (THREAD_L3_PREDICT == m_pActionManager->GetAfterThread())
		{
			m_pActionManager->NextThreadSetting();
			qDebug() << "THREAD_L3_PREDICT";
			OnUpdateProgressMultiThread(0, QString("DeepCatch - L3 predict"));

			//AISegTabDeepCatch* pDeepCatchtab = (AISegTabDeepCatch*)m_pWindowManager->GetTab()->getAITab();
			bool bAutoL3 = false;
			if (m_pActionManager->m_IsMacroMode) // L3
			{
				QMap<QString, sDeepcatchMacroColumnData>::const_iterator iterMap = m_pActionManager->m_ListMacroCommandLine.first().rowMap.find(DEEPCATCH_MACRO_CATEGORY_SLICE);
				if (iterMap != m_pActionManager->m_ListMacroCommandLine.first().rowMap.end())
				{
					sDeepcatchMacroColumnData data = iterMap.value();
					QString strSliceType = data.data.at(0);
					QString strSliceNum = data.data.at(1);

					if (strSliceType.compare(QString("L3")))
						bAutoL3 = true;

					if (!strSliceType.compare(QString("L3")) && !strSliceNum.compare(QString("A")))
						bAutoL3 = true;
				}
			}
			else if (!m_pActionManager->m_IsMacroMode)
				bAutoL3 = true;

			if (bAutoL3)	// L3
			{
				// 3) 하단 clipping
				// 4) Sagittal Projection (평균 값을 계산하기). 조건: // L3 predict의 입력 만들기
				// 5) action_L3_Predict

				if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2))
				{
					AISegTabDeepCatch* pDeepCatchtab = (AISegTabDeepCatch*)m_pWindowManager->GetTab()->getAITab();
					int btnState = pDeepCatchtab->getBoneAnalysis(); // 0: Vertebra Network, 1: nothing

					if (btnState == eUseVertebraNetwork && pDeepCatchtab->getSingleSliceNum() != eDCVB_L3)
					{
						OnUpdateProgressMultiThread(0, QString("calculate_slice_position"));

						int num = pDeepCatchtab->getCurrentVBComboIndex(eSST_Single);

						std::vector<int> VbNumList;
						VbNumList.push_back(num);
						m_pActionManager->action_calculate_slice_position(VbNumList, ST_SINGLE);
					}
					else
					{
						//OnUpdateProgressMultiThread(0, QString("L3 Predict"));

						// 3) 하단 clipping, 마지막 mask+10 이후 clipping

						MaskInfo* pMaskInfoCopy = m_pDataContext->volume_data.findMaskInfo("Bone copy");
						const QString dataPath = STRING_MANAGER->LocalAISegPath + "/predict/L3Input";
						QString rawName = QString("/L3_predict_input.raw");

						int nLastSlice = 0;
						if (pMaskInfoCopy != nullptr)
							nLastSlice = m_pActionManager->CreateL3AWPredictInput(pMaskInfoCopy->uid, WT_SAGITTAL, dataPath, rawName);


						BoundingBoxI box = m_pDataContext->volume_data.getBoundingBox();
						box.minZ = nLastSlice;

						const QString weightPath = STRING_MANAGER->AISegmentationPath + "/weight_deepcatch";
						const QString strProjectName = "L3";
						m_pActionManager->action_DeepCatch_L3_predict(dataPath, weightPath, MFL_Product_DeepCatch_DeepCatchV2, strProjectName, box, true, false);
					}
				}
				else
				{
					// 3) 하단 clipping, 마지막 mask+10 이후 clipping

					MaskInfo* pMaskInfoCopy = m_pDataContext->volume_data.findMaskInfo("Bone copy");
					const QString dataPath = STRING_MANAGER->LocalAISegPath + "/predict/L3Input";
					QString rawName = QString("/L3_predict_input.raw");

					int nLastSlice = m_pActionManager->CreateL3AWPredictInput(pMaskInfoCopy->uid, WT_SAGITTAL, dataPath, rawName);


					BoundingBoxI box = m_pDataContext->volume_data.getBoundingBox();
					box.minZ = nLastSlice;

					const QString weightPath = STRING_MANAGER->AISegmentationPath + "/weight_deepcatch";
					const QString strProjectName = "L3";
					m_pActionManager->action_DeepCatch_L3_predict(dataPath, weightPath, MFL_Product_DeepCatch, strProjectName, box, true, false);
				}
			}
			else
			{
				// Window 화면에서 내부 index 처리를 0~ 부터 시작하기 때문에 -1 해줘야 함.
				int nL3Num = -1;
				QMap<QString, sDeepcatchMacroColumnData>::const_iterator iterMap = m_pActionManager->m_ListMacroCommandLine.first().rowMap.find(DEEPCATCH_MACRO_CATEGORY_SLICE);
				if (iterMap != m_pActionManager->m_ListMacroCommandLine.first().rowMap.end())
				{
					sDeepcatchMacroColumnData data = iterMap.value();
					QString strSliceNum = data.data.at(1);

					nL3Num = strSliceNum.toInt() - 1;
				}
				m_pActionManager->m_qThreadNext.front().pTempData = std::make_shared<int>(nL3Num);
				qDebug() << "marcro L3 : " << nL3Num;
				m_pActionManager->action_EmptyStart(ACTP_L3_PREDICT);
			}
		}
		else if (THREAD_ABDOMIAL_WAIST_USERSELECT == m_pActionManager->GetAfterThread())
		{
			qDebug() << "THREAD_ABDOMIAL_WAIST_RANGE";
			m_pActionManager->NextThreadSetting();

			OnUpdateProgressMultiThread(0, QString("DeepCatch - Abdominal waist User Selection"));

			bool bAutoAW = false;
			if (m_pActionManager->m_IsMacroMode)
			{
				QMap<QString, sDeepcatchMacroColumnData>::const_iterator iterMap = m_pActionManager->m_ListMacroCommandLine.first().rowMap.find(DEEPCATCH_MACRO_CATEGORY_SLICE);
				if (iterMap != m_pActionManager->m_ListMacroCommandLine.first().rowMap.end())
				{
					sDeepcatchMacroColumnData data = iterMap.value();
					QString strSliceType = data.data.at(0);
					QString strSliceNum = data.data.at(1);

					if (strSliceType.compare(QString("AW")))
						bAutoAW = true;

					if (!strSliceType.compare(QString("AW")) && !strSliceNum.compare(QString("A")))
						bAutoAW = true;
				}
			}
			else if (!m_pActionManager->m_IsMacroMode)
				bAutoAW = true;

			if (bAutoAW)	// AW			
			{
				if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2))
				{
					AISegTabDeepCatch* pDeepCatchtab = (AISegTabDeepCatch*)m_pWindowManager->GetTab()->getAITab();
					int btnState = pDeepCatchtab->getBoneAnalysis(); // 0: Vertebra Network, 1: nothing

					if (btnState == eUseVertebraNetwork)
					{
						OnUpdateProgressMultiThread(0, QString("calculate_slice_position"));
						//DEEPCATCH_REPORT_PREDICT_INFO* pPredictedInfo = m_pActionManager->DeepcatchReportPredictedInfoSafeCreate();

						AISegTabDeepCatch* pDeepCatchtab = (AISegTabDeepCatch*)m_pWindowManager->GetTab()->getAITab();
						int lowComboNum = pDeepCatchtab->getCurrentVBComboIndex(eSST_MultiLow);
						int upComboNum = pDeepCatchtab->getCurrentVBComboIndex(eSST_MultiUp);

						std::vector<int> VbNumList;
						VbNumList.push_back(lowComboNum);
						VbNumList.push_back(upComboNum);
						m_pActionManager->action_calculate_slice_position(VbNumList, ST_MULTIPLE);
					}
					else
					{
						m_pActionManager->action_EmptyStart(ACTP_ABDOMIAL_WAIST_USERSELECT);
					}
				}

			}
			else
			{
				// Window 화면에서 내부 index 처리를 0~ 부터 시작하기 때문에 -1 해줘야 함.
				std::vector<int> vecSlice;
				int sliceNum1 = -1;
				int sliceNum2 = -1;

				QMap<QString, sDeepcatchMacroColumnData>::const_iterator iterMap = m_pActionManager->m_ListMacroCommandLine.first().rowMap.find(DEEPCATCH_MACRO_CATEGORY_SLICE);
				if (iterMap != m_pActionManager->m_ListMacroCommandLine.first().rowMap.end())
				{
					sDeepcatchMacroColumnData data = iterMap.value();
					QString strSliceNum1 = data.data.at(1);
					QString strSliceNum2 = data.data.at(2);

					sliceNum1 = strSliceNum1.toInt() - 1;
					sliceNum2 = strSliceNum2.toInt() - 1;
				}

				vecSlice.push_back(sliceNum1);
				vecSlice.push_back(sliceNum2);

				m_pActionManager->m_qThreadNext.front().pTempData = std::make_shared<std::vector<int>>(vecSlice);
				qDebug() << "marcro AW : " << vecSlice[0];
				qDebug() << "marcro AW : " << vecSlice[1];

				m_pActionManager->action_EmptyStart(ACTP_ABDOMIAL_WAIST_USERSELECT);
			}

			MaskInfo* pDelMaskInfo = m_pDataContext->volume_data.findMaskInfo(QString("Bone copy"));
			if (pDelMaskInfo)
			{
				m_pDataContext->volume_data.delMaskInfo(pDelMaskInfo->uid);
			}
			// 6) L3 표시
			// 7) Abdomial waist range 알고리즘 - action_abdominal
		}
		else if (THREAD_ABDOMIAL_WAIST_RANGE == m_pActionManager->GetAfterThread())
		{
			qDebug() << "THREAD_ABDOMIAL_WAIST_RANGE";
			m_pActionManager->NextThreadSetting();

			OnUpdateProgressMultiThread(0, QString("DeepCatch - Abdominal waist range"));

			bool bAutoAW = false;
			if (m_pActionManager->m_IsMacroMode)
			{
				QMap<QString, sDeepcatchMacroColumnData>::const_iterator iterMap = m_pActionManager->m_ListMacroCommandLine.first().rowMap.find(DEEPCATCH_MACRO_CATEGORY_SLICE);
				if (iterMap != m_pActionManager->m_ListMacroCommandLine.first().rowMap.end())
				{
					sDeepcatchMacroColumnData data = iterMap.value();
					QString strSliceType = data.data.at(0);
					QString strSliceNum = data.data.at(1);

					if (strSliceType.compare(QString("AW")))
						bAutoAW = true;

					if (!strSliceType.compare(QString("AW")) && !strSliceNum.compare(QString("A")))
						bAutoAW = true;
				}
			}
			else if (!m_pActionManager->m_IsMacroMode)
				bAutoAW = true;

			if (bAutoAW)	// AW			
			{
				if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2))
				{
					int nCz = m_pDataContext->volume_data.getCZ();
					AISegTabDeepCatch* pDeepCatchtab = (AISegTabDeepCatch*)m_pWindowManager->GetTab()->getAITab();
					int btnState = pDeepCatchtab->getBoneAnalysis(); // 0: Vertebra Network, 1: nothing

					if (btnState == eUseVertebraNetwork && (pDeepCatchtab->getMultiSliceUpNum() != eDCVB_Abdominal_waist && pDeepCatchtab->getMultiSliceLowNum() != eDCVB_Abdominal_waist))
					{
						OnUpdateProgressMultiThread(0, QString("calculate_slice_position"));

						AISegTabDeepCatch* pDeepCatchtab = (AISegTabDeepCatch*)m_pWindowManager->GetTab()->getAITab();
						int lowComboNum = pDeepCatchtab->getCurrentVBComboIndex(eSST_MultiLow);
						int upComboNum = pDeepCatchtab->getCurrentVBComboIndex(eSST_MultiUp);

						std::vector<int> VbNumList;
						VbNumList.push_back(lowComboNum);
						VbNumList.push_back(upComboNum);
						m_pActionManager->action_calculate_slice_position(VbNumList, ST_MULTIPLE);
					}
					else if (nCz > 1)
					{
						MaskInfo* pMaskInfoCopy = m_pDataContext->volume_data.findMaskInfo("Bone");
						const QString dataPath = STRING_MANAGER->LocalAISegPath + "/predict/AWInput";
						QString rawName = QString("/AW_predict_input.raw");

						printf_s("\n[  ----- CreateAWPredictInput start ----  ] \n");
						m_pActionManager->CreateAWPredictInput(pMaskInfoCopy->uid, WT_CORONAL, dataPath, rawName);
						printf_s("\n[  ----- CreateAWPredictInput end ----  ] \n");

						BoundingBoxI box = m_pDataContext->volume_data.getBoundingBox();
						//box.minZ = nLastSlice;

						const QString weightPath = STRING_MANAGER->AISegmentationPath + "/weight_deepcatch";

						QString strProjectName;
						strProjectName = "AWSegmentation";

						m_pActionManager->action_DeepCatch_Abdominal_waist_range_Predict(dataPath, weightPath, MFL_Product_DeepCatch_DeepCatchV2, strProjectName, box, true, false);
						// 	5) Segmentation 네트워크로 ROI 마스크를 얻고 Connectivity를 적용합니다(가장 큰 덩어리를 남긴겁니다)
						// 		6) ROI의 Min Y 및 Max Y으로 Waist Upper 및 Waist Lower를 계산합니다
						// 		Waist Upper = Min Y / 512 * Total Slice
						// 		Waist Lower = Max Y / 512 * Total Slice
					}
					else
					{
						std::vector<int> vecSlice;
						vecSlice.push_back(1);
						vecSlice.push_back(1);

						m_pActionManager->m_qThreadNext.front().pTempData = std::make_shared<std::vector<int>>(vecSlice);
						qDebug() << "marcro AW : " << vecSlice[0];
						qDebug() << "marcro AW : " << vecSlice[1];

						m_pActionManager->action_EmptyStart(ACTP_ABDOMIAL_WAIST_RANGE);
					}
				}
				else
				{
					int nCz = m_pDataContext->volume_data.getCZ();
					if (nCz > 1)
					{
						MaskInfo* pMaskInfoCopy = m_pDataContext->volume_data.findMaskInfo("Bone");
						const QString dataPath = STRING_MANAGER->LocalAISegPath + "/predict/AWInput";
						QString rawName = QString("/AW_predict_input.raw");

						printf_s("\n[  ----- CreateAWPredictInput start ----  ] \n");
						m_pActionManager->CreateAWPredictInput(pMaskInfoCopy->uid, WT_CORONAL, dataPath, rawName);
						printf_s("\n[  ----- CreateAWPredictInput end ----  ] \n");

						BoundingBoxI box = m_pDataContext->volume_data.getBoundingBox();
						//box.minZ = nLastSlice;

						const QString weightPath = STRING_MANAGER->AISegmentationPath + "/weight_deepcatch";

						QString strProjectName;
						strProjectName = "AWSegmentation";

						m_pActionManager->action_DeepCatch_Abdominal_waist_range_Predict(dataPath, weightPath, MFL_Product_DeepCatch, strProjectName, box, true, false);
						// 	5) Segmentation 네트워크로 ROI 마스크를 얻고 Connectivity를 적용합니다(가장 큰 덩어리를 남긴겁니다)
						// 		6) ROI의 Min Y 및 Max Y으로 Waist Upper 및 Waist Lower를 계산합니다
						// 		Waist Upper = Min Y / 512 * Total Slice
						// 		Waist Lower = Max Y / 512 * Total Slice
					}
					else
					{
						std::vector<int> vecSlice;
						vecSlice.push_back(1);
						vecSlice.push_back(1);

						m_pActionManager->m_qThreadNext.front().pTempData = std::make_shared<std::vector<int>>(vecSlice);
						qDebug() << "marcro AW : " << vecSlice[0];
						qDebug() << "marcro AW : " << vecSlice[1];

						m_pActionManager->action_EmptyStart(ACTP_ABDOMIAL_WAIST_RANGE);
					}
				}
			}
			else
			{
				// Window 화면에서 내부 index 처리를 0~ 부터 시작하기 때문에 -1 해줘야 함.
				std::vector<int> vecSlice;
				int sliceNum1 = -1;
				int sliceNum2 = -1;

				QMap<QString, sDeepcatchMacroColumnData>::const_iterator iterMap = m_pActionManager->m_ListMacroCommandLine.first().rowMap.find(DEEPCATCH_MACRO_CATEGORY_SLICE);
				if (iterMap != m_pActionManager->m_ListMacroCommandLine.first().rowMap.end())
				{
					sDeepcatchMacroColumnData data = iterMap.value();
					QString strSliceNum1 = data.data.at(1);
					QString strSliceNum2 = data.data.at(2);

					sliceNum1 = strSliceNum1.toInt() - 1;
					sliceNum2 = strSliceNum2.toInt() - 1;
				}

				vecSlice.push_back(sliceNum1);
				vecSlice.push_back(sliceNum2);

				m_pActionManager->m_qThreadNext.front().pTempData = std::make_shared<std::vector<int>>(vecSlice);
				qDebug() << "marcro AW : " << vecSlice[0];
				qDebug() << "marcro AW : " << vecSlice[1];

				m_pActionManager->action_EmptyStart(ACTP_ABDOMIAL_WAIST_RANGE);
			}

			MaskInfo* pDelMaskInfo = m_pDataContext->volume_data.findMaskInfo(QString("Bone copy"));
			if (pDelMaskInfo)
			{
				m_pDataContext->volume_data.delMaskInfo(pDelMaskInfo->uid);
			}
			// 6) L3 표시
			// 7) Abdomial waist range 알고리즘 - action_abdominal
		}
		else if (THREAD_ABDOMIAL_WAIST_CONFIRM == m_pActionManager->GetAfterThread())
		{
			m_pActionManager->NextThreadSetting();
			// 8) 시작 라인 컨펌
			// 9) 끝 라인 컨펌
		}
		else if (THREAD_SAVE_3D_VOLUME == m_pActionManager->GetAfterThread())
		{
			m_pActionManager->NextThreadSetting();
			// 10) 
			// 11) action_preset 1~7까지 이미지 저장
		}
		else if (THREAD_DEEPCATCH_REPORT == m_pActionManager->GetAfterThread())
		{
			auto dResult = std::static_pointer_cast<double>(m_pActionManager->m_qThreadNext.front().pTempData);
			float fAbdomialCircumference = *dResult;
			dResult.reset();
			(m_pActionManager->m_qThreadNext.front().pTempData).reset();

			m_pActionManager->NextThreadSetting();

			deepcatchResultToReport(false);

			// result(l3 circumference) save
			DEEPCATCH_REPORT* pReport = m_pActionManager->getDeepCatchAnalysisVals();
			pReport->fAbdomialCircumference = fAbdomialCircumference;

		}
		else if (THREAD_TRUNK_PREDICT == m_pActionManager->GetAfterThread())
		{
			m_pActionManager->NextThreadSetting();
			qDebug() << "THREAD_TRUNK_PREDICT";
			OnUpdateProgressMultiThread(0, QString("DeepCatch - Trunk predict"));

			AISegTabDeepCatch* pAITab = (AISegTabDeepCatch*)m_pWindowManager->GetTab()->getAITab();

			const QString dataPath = STRING_MANAGER->LocalAISegPath + "/predict/TrunkInput";
			BoundingBoxI box = m_pDataContext->volume_data.getBoundingBox();

			const QString weightPath = STRING_MANAGER->AISegmentationPath + "/weight_deepcatch";

			DEEPCATCH_REPORT_PREDICT_INFO* pPredictedInfo = m_pActionManager->getDeepCatch_PredictedInfo();
			const QString strProjectName = (pPredictedInfo->stPredictOpt.UNETType == eDUT2D) ? "ArmTrunk2_2D178T-32" : "ArmTrunk";
			//	m_pActionManager->action_DeepCatch_Trunk_Predict(dataPath, weightPath, MFL_Product_DeepCatch, strProjectName, box, true, false);
			m_pActionManager->action_Deepdraw_predict(
				dataPath, weightPath, MFL_Product_DeepCatch, strProjectName, box, pAITab->IsUseGPU(), eDeepPredictAICopyMask::none_copy, false);
		}
		else if (THREAD_TRUNK_PREDICT_APPLY == m_pActionManager->GetAfterThread())
		{
			m_pActionManager->NextThreadSetting();
			qDebug() << "THREAD_TRUNK_PREDICT_APPLY";
			OnUpdateProgressMultiThread(0, QString("DeepCatch - Trunk predict apply"));

			// 1. 생성된 trunk mask에 대해 3d hole filling 적용.
			// trunk mask 선택.
			int uid = 0;
			unsigned char trunkMask;
			int trunkMaskByteIndex = 0;
			MaskInfo* info = m_pDataContext->volume_data.findMaskInfo(DEEPCATCH_MASKNAME_TRUNK);
			if (info)
			{
				uid = info->uid;
				if (info->uid >= MASK_SECOND_MAX)
				{
					trunkMask = info->mask_id2;
					trunkMaskByteIndex = (info->uid - MASK_SECOND_MAX) / 8 + 1;
				}
				else
					trunkMask = info->mask_id;
			}
			else
				trunkMask = VM_MASK2;

			m_pActionManager->action_Image_Hole_Filling(uid, trunkMask, trunkMaskByteIndex, mip::HoleFilling::Mode::Mode3D, false);
		}

		else if (THREAD_DEEPCATCH_DEEPDRAW_PREDICT == m_pActionManager->GetAfterThread())
		{
			m_pActionManager->NextThreadSetting();
			qDebug() << "THREAD_DEEPCATCH_DEEPDRAW_PREDICT";
			OnUpdateProgressMultiThread(0, QString("DeepCatch - Segmentation"));

			AISegTabDeepCatch* pAITab = (AISegTabDeepCatch*)m_pWindowManager->GetTab()->getAITab();

			const QString dataPath = STRING_MANAGER->LocalAISegPath + "/predict";
			//	BoundingBoxI box = m_pDataContext->volume_data.getBoundingBox();
			BoundingBoxI box = pAITab->getValidRegion();
			const QString weightPath = STRING_MANAGER->AISegmentationPath + "/weight_deepcatch";

			m_pActionManager->action_Deepdraw_predict(
				dataPath, weightPath, MFL_Product_DeepCatch, pAITab->GetFileName(), box, pAITab->IsUseGPU(), eDeepPredictAICopyMask::none_copy, false);
		}

		else if (THREAD_IO_CLASSIFICATION_PREDICT == m_pActionManager->GetAfterThread())
		{
			m_pActionManager->NextThreadSetting();
			qDebug() << "THREAD_IO_CLASSIFICATION_PREDICT";
			OnUpdateProgressMultiThread(0, QString("DeepCatch - IO Classification"));

			AISegTabDeepCatch* pAITab = (AISegTabDeepCatch*)m_pWindowManager->GetTab()->getAITab();

			const QString dataPath = STRING_MANAGER->LocalAISegPath + "/predict/IOClassificationInput";
			//	BoundingBoxI box = m_pDataContext->volume_data.getBoundingBox();
			BoundingBoxI box = pAITab->getValidRegion();
			const QString weightPath = STRING_MANAGER->AISegmentationPath + "/weight_deepcatch";

			DEEPCATCH_REPORT_PREDICT_INFO* pPredictedInfo = m_pActionManager->getDeepCatch_PredictedInfo();
			QString strProjectName = (pPredictedInfo->stPredictOpt.UNETType == eDUT2D) ? "IO_2D" : "IO_3D";
			qDebug() << "strProjectName" << strProjectName;

			m_pActionManager->action_Deepdraw_predict(
				dataPath, weightPath, MFL_Product_DeepCatch, strProjectName, box, pAITab->IsUseGPU(), eDeepPredictAICopyMask::none_copy, false);
		}

		else if (THREAD_IO_CLASSIFICATION_PREDICT2 == m_pActionManager->GetAfterThread())
		{
			m_pActionManager->NextThreadSetting();
			qDebug() << "THREAD_IO_CLASSIFICATION_PREDICT_Pancreas";
			OnUpdateProgressMultiThread(0, QString("DeepCatch - IO Classification"));

			AISegTabDeepCatch* pAITab = (AISegTabDeepCatch*)m_pWindowManager->GetTab()->getAITab();

			const QString dataPath = STRING_MANAGER->LocalAISegPath + "/predict/IOClassificationInput";
			//	BoundingBoxI box = m_pDataContext->volume_data.getBoundingBox();
			BoundingBoxI box = pAITab->getValidRegion();
			const QString weightPath = STRING_MANAGER->AISegmentationPath + "/weight_deepcatch";

			DEEPCATCH_REPORT_PREDICT_INFO* pPredictedInfo = m_pActionManager->getDeepCatch_PredictedInfo();
			QString strProjectName = (pPredictedInfo->stPredictOpt.UNETType == eDUT2D) ? "Pancreas_2D" : "Pancreas_3D";
			qDebug() << "strProjectName" << strProjectName;

			m_pActionManager->action_Deepdraw_predict(
				dataPath, weightPath, MFL_Product_DeepCatch, strProjectName, box, pAITab->IsUseGPU(), eDeepPredictAICopyMask::none_copy, false);
		}

		// 1. Muscle mask copy 4개하고 이름 변경.
		//  - Muscle_adp_tissue (cyan 색상- 0,255,255): -190 ~ -30 HU
		//  - Muscle_abnormal1(blue 색상 - 0, 0, 255) : -29 ~ 0 HU
		//  - Muscle_abnormal2(yellow 색상 255, 255, 0) : +1 ~ +29 HU
		//  - Muscle_normal(red 색상 - 255, 0, 0) : +30 ~ +150 HU
		else if (THREAD_MUSCLE_CLASSIFICATION_ADP_TISSUE == m_pActionManager->GetAfterThread())
		{
			m_pActionManager->NextThreadSetting();
			qDebug() << "THREAD_MUSCLE_CLASSIFICATION_ADP_TISSUE";
			OnUpdateProgressMultiThread(0, QString("DeepCatch - Adipose Tissue"));

			MaskInfo* pMaskInfo = nullptr;
			pMaskInfo = m_pDataContext->volume_data.findMaskInfo(DEEPCATCH_WHOLEBODY_MASKNAME_MUSCLE_ADP_TISSUE);
			if (pMaskInfo == nullptr)
			{
				MaskInfo* pMuscleMaskInfo = m_pDataContext->volume_data.findMaskInfo(DEEPCATCH_WHOLEBODY_MASKNAME_MUSCLE);
				if (pMuscleMaskInfo && m_pDataContext->volume_data.getMaskInfoListCnt() < MASK_MAX)
				{
					int nUIDMuscle = pMuscleMaskInfo->uid;
					int nUIIdxMuscle = m_pDataContext->volume_data.getMaskIndex(nUIDMuscle);

					ActionMaskListCopyAdd maskListCopyAdd(&m_pDataContext->volume_data, nUIIdxMuscle);
					maskListCopyAdd.redo();

					MaskInfo* tempInfo = m_pDataContext->volume_data.getCurrentMaskInfo();
					m_pDataContext->volume_data.setMaskName(DEEPCATCH_WHOLEBODY_MASKNAME_MUSCLE_ADP_TISSUE, tempInfo->uid, true);
					tempInfo->color = COLOR(0, 255, 255, 255);
				}
			}
			else
			{
				int maskIndex = -1;
				maskIndex = m_pDataContext->volume_data.getMaskIndex(pMaskInfo->uid);
				ROITab2* pRoiTab = m_pWindowManager->GetTab()->getROITab();
				m_pDataContext->volume_data.setCurrentMaskIndex(maskIndex);
				std::vector<muint32> vecSelect;
				vecSelect.push_back(pMaskInfo->uid);
				m_pDataContext->volume_data.setMultiSelectUID(vecSelect);
				pRoiTab->ROIList->SelectionUpdate(maskIndex);
			}
			// 2. 각 mask를 선택한 후 slot_OnApplyThreshold()를 수행하여 해당 영역만 남도록 한다.
			int rangeMin = -190;
			int rangeMax = -30;
			AISegTab* pAITab = m_pWindowManager->GetTab()->getAITab();
			if (pAITab)
			{
				AISegTabDeepCatch* pAITabDeepCatch = dynamic_cast<AISegTabDeepCatch*>(pAITab);
				if (pAITabDeepCatch)
				{
					rangeMin = pAITabDeepCatch->getMuslceQualityMapStartHU(eMQMCAptTissue);
					rangeMax = pAITabDeepCatch->getMuslceQualityMapEndHU(eMQMCAptTissue);
				}
			}
			int nUpper, nLower;
			m_pWindowManager->setThreLower(rangeMin);
			m_pWindowManager->setThreUpper(rangeMax);
			nUpper = m_pWindowManager->getThreUpper();
			nLower = m_pWindowManager->getThreLower();
			m_pActionManager->action_ThresholdSelect(nUpper, nLower, true, m_pWindowManager->getSelectedMask(), m_pWindowManager->getSelectedMaskByteIndex(), false);
		}
		// Muscle_abnormal1(blue 색상 - 0, 0, 255) : -29 ~ 0 HU
		else if (THREAD_MUSCLE_CLASSIFICATION_ABNORMAL1 == m_pActionManager->GetAfterThread())
		{
			m_pActionManager->NextThreadSetting();
			qDebug() << "THREAD_MUSCLE_CLASSIFICATION_ABNORMAL1";
			OnUpdateProgressMultiThread(0, QString("DeepCatch - Abnormal Muscle1"));

			MaskInfo* pMaskInfo = nullptr;
			pMaskInfo = m_pDataContext->volume_data.findMaskInfo(DEEPCATCH_WHOLEBODY_MASKNAME_MUSCLE_ABNORMAL1);
			if (pMaskInfo == nullptr)
			{
				MaskInfo* pMuscleMaskInfo = m_pDataContext->volume_data.findMaskInfo(DEEPCATCH_WHOLEBODY_MASKNAME_MUSCLE);
				if (pMuscleMaskInfo && m_pDataContext->volume_data.getMaskInfoListCnt() < MASK_MAX)
				{
					int nUIDMuscle = pMuscleMaskInfo->uid;
					int nUIIdxMuscle = m_pDataContext->volume_data.getMaskIndex(nUIDMuscle);

					ActionMaskListCopyAdd maskListCopyAdd(&m_pDataContext->volume_data, nUIIdxMuscle);
					maskListCopyAdd.redo();

					MaskInfo* tempInfo = m_pDataContext->volume_data.getCurrentMaskInfo();
					m_pDataContext->volume_data.setMaskName(DEEPCATCH_WHOLEBODY_MASKNAME_MUSCLE_ABNORMAL1, tempInfo->uid, true);
					tempInfo->color = COLOR(0, 0, 255, 255);
				}
			}
			else
			{
				int maskIndex = -1;
				maskIndex = m_pDataContext->volume_data.getMaskIndex(pMaskInfo->uid);
				ROITab2* pRoiTab = m_pWindowManager->GetTab()->getROITab();
				m_pDataContext->volume_data.setCurrentMaskIndex(maskIndex);
				std::vector<muint32> vecSelect;
				vecSelect.push_back(pMaskInfo->uid);
				m_pDataContext->volume_data.setMultiSelectUID(vecSelect);
				pRoiTab->ROIList->SelectionUpdate(maskIndex);
			}
			// 2. 각 mask를 선택한 후 slot_OnApplyThreshold()를 수행하여 해당 영역만 남도록 한다.
			int rangeMin = -29;
			int rangeMax = 0;
			AISegTab* pAITab = m_pWindowManager->GetTab()->getAITab();
			if (pAITab)
			{
				AISegTabDeepCatch* pAITabDeepCatch = dynamic_cast<AISegTabDeepCatch*>(pAITab);
				if (pAITabDeepCatch)
				{
					rangeMin = pAITabDeepCatch->getMuslceQualityMapStartHU(eMQMCAbnormal1);
					rangeMax = pAITabDeepCatch->getMuslceQualityMapEndHU(eMQMCAbnormal1);
				}
			}
			int nUpper, nLower;
			m_pWindowManager->setThreLower(rangeMin);
			m_pWindowManager->setThreUpper(rangeMax);
			nUpper = m_pWindowManager->getThreUpper();
			nLower = m_pWindowManager->getThreLower();
			m_pActionManager->action_ThresholdSelect(nUpper, nLower, true, m_pWindowManager->getSelectedMask(), m_pWindowManager->getSelectedMaskByteIndex(), false);
		}
		// Muscle_abnormal2(yellow 색상 255, 255, 0) : +1 ~ +29 HU
		else if (THREAD_MUSCLE_CLASSIFICATION_ABNORMAL2 == m_pActionManager->GetAfterThread())
		{
			m_pActionManager->NextThreadSetting();
			qDebug() << "THREAD_MUSCLE_CLASSIFICATION_ABNORMAL2";
			OnUpdateProgressMultiThread(0, QString("DeepCatch - Abnormal Muscle2"));

			MaskInfo* pMaskInfo = nullptr;
			pMaskInfo = m_pDataContext->volume_data.findMaskInfo(DEEPCATCH_WHOLEBODY_MASKNAME_MUSCLE_ABNORMAL2);
			if (pMaskInfo == nullptr)
			{
				MaskInfo* pMuscleMaskInfo = m_pDataContext->volume_data.findMaskInfo(DEEPCATCH_WHOLEBODY_MASKNAME_MUSCLE);
				if (pMuscleMaskInfo && m_pDataContext->volume_data.getMaskInfoListCnt() < MASK_MAX)
				{
					int nUIDMuscle = pMuscleMaskInfo->uid;
					int nUIIdxMuscle = m_pDataContext->volume_data.getMaskIndex(nUIDMuscle);

					ActionMaskListCopyAdd maskListCopyAdd(&m_pDataContext->volume_data, nUIIdxMuscle);
					maskListCopyAdd.redo();

					MaskInfo* tempInfo = m_pDataContext->volume_data.getCurrentMaskInfo();
					m_pDataContext->volume_data.setMaskName(DEEPCATCH_WHOLEBODY_MASKNAME_MUSCLE_ABNORMAL2, tempInfo->uid, true);
					tempInfo->color = COLOR(255, 255, 0, 255);
				}
			}
			else
			{
				int maskIndex = -1;
				maskIndex = m_pDataContext->volume_data.getMaskIndex(pMaskInfo->uid);
				ROITab2* pRoiTab = m_pWindowManager->GetTab()->getROITab();
				m_pDataContext->volume_data.setCurrentMaskIndex(maskIndex);
				std::vector<muint32> vecSelect;
				vecSelect.push_back(pMaskInfo->uid);
				m_pDataContext->volume_data.setMultiSelectUID(vecSelect);
				pRoiTab->ROIList->SelectionUpdate(maskIndex);
			}
			// 2. 각 mask를 선택한 후 slot_OnApplyThreshold()를 수행하여 해당 영역만 남도록 한다.
			int rangeMin = 1;
			int rangeMax = 29;
			AISegTab* pAITab = m_pWindowManager->GetTab()->getAITab();
			if (pAITab)
			{
				AISegTabDeepCatch* pAITabDeepCatch = dynamic_cast<AISegTabDeepCatch*>(pAITab);
				if (pAITabDeepCatch)
				{
					rangeMin = pAITabDeepCatch->getMuslceQualityMapStartHU(eMQMCAbnormal2);
					rangeMax = pAITabDeepCatch->getMuslceQualityMapEndHU(eMQMCAbnormal2);
				}
			}
			int nUpper, nLower;
			m_pWindowManager->setThreLower(rangeMin);
			m_pWindowManager->setThreUpper(rangeMax);
			nUpper = m_pWindowManager->getThreUpper();
			nLower = m_pWindowManager->getThreLower();
			m_pActionManager->action_ThresholdSelect(nUpper, nLower, true, m_pWindowManager->getSelectedMask(), m_pWindowManager->getSelectedMaskByteIndex(), false);
		}
		//  Muscle_normal(red 색상 - 255, 0, 0) : +30 ~ +150 HU
		else if (THREAD_MUSCLE_CLASSIFICATION_NORMAL == m_pActionManager->GetAfterThread())
		{
			m_pActionManager->NextThreadSetting();
			qDebug() << "THREAD_MUSCLE_CLASSIFICATION_NORMAL";
			OnUpdateProgressMultiThread(0, QString("DeepCatch - Normal Muscle"));

			MaskInfo* pMaskInfo = nullptr;
			pMaskInfo = m_pDataContext->volume_data.findMaskInfo(DEEPCATCH_WHOLEBODY_MASKNAME_MUSCLE_NORMAL);
			if (pMaskInfo == nullptr)
			{
				MaskInfo* pMuscleMaskInfo = m_pDataContext->volume_data.findMaskInfo(DEEPCATCH_WHOLEBODY_MASKNAME_MUSCLE);
				if (pMuscleMaskInfo && m_pDataContext->volume_data.getMaskInfoListCnt() < MASK_MAX)
				{
					int nUIDMuscle = pMuscleMaskInfo->uid;
					int nUIIdxMuscle = m_pDataContext->volume_data.getMaskIndex(nUIDMuscle);

					ActionMaskListCopyAdd maskListCopyAdd(&m_pDataContext->volume_data, nUIIdxMuscle);
					maskListCopyAdd.redo();

					MaskInfo* tempInfo = m_pDataContext->volume_data.getCurrentMaskInfo();
					m_pDataContext->volume_data.setMaskName(DEEPCATCH_WHOLEBODY_MASKNAME_MUSCLE_NORMAL, tempInfo->uid, true);
					tempInfo->color = COLOR(255, 0, 0, 255);
				}
			}
			else
			{
				int maskIndex = -1;
				maskIndex = m_pDataContext->volume_data.getMaskIndex(pMaskInfo->uid);
				ROITab2* pRoiTab = m_pWindowManager->GetTab()->getROITab();
				m_pDataContext->volume_data.setCurrentMaskIndex(maskIndex);
				std::vector<muint32> vecSelect;
				vecSelect.push_back(pMaskInfo->uid);
				m_pDataContext->volume_data.setMultiSelectUID(vecSelect);
				pRoiTab->ROIList->SelectionUpdate(maskIndex);
			}
			// 2. 각 mask를 선택한 후 slot_OnApplyThreshold()를 수행하여 해당 영역만 남도록 한다.
			int rangeMin = 30;
			int rangeMax = 150;
			AISegTab* pAITab = m_pWindowManager->GetTab()->getAITab();
			if (pAITab)
			{
				AISegTabDeepCatch* pAITabDeepCatch = dynamic_cast<AISegTabDeepCatch*>(pAITab);
				if (pAITabDeepCatch)
				{
					rangeMin = pAITabDeepCatch->getMuslceQualityMapStartHU(eMQMCNormal);
					rangeMax = pAITabDeepCatch->getMuslceQualityMapEndHU(eMQMCNormal);
				}
			}
			int nUpper, nLower;
			m_pWindowManager->setThreLower(rangeMin);
			m_pWindowManager->setThreUpper(rangeMax);
			nUpper = m_pWindowManager->getThreUpper();
			nLower = m_pWindowManager->getThreLower();
			m_pActionManager->action_ThresholdSelect(nUpper, nLower, true, m_pWindowManager->getSelectedMask(), m_pWindowManager->getSelectedMaskByteIndex(), false);
		}

		else if (THREAD_CIRCUMFERENCE == m_pActionManager->GetAfterThread())
		{
			m_pActionManager->NextThreadSetting();
			qDebug() << "THREAD_CIRCUMFERENCE";
			OnUpdateProgressMultiThread(0, QString("DeepCatch - L3 Circumference"));

			// Abdominal circumference
			DEEPCATCH_REPORT_PREDICT_INFO* predictInfo = m_pActionManager->getDeepCatch_PredictedInfo();
			MaskInfo* pNewMaskInfo = nullptr;
			if (m_pDataContext->volume_data.findMaskInfo(DEEPCATCH_WHOLEBODY_MASKNAME_AC) == nullptr)
			{
				m_pDataContext->volume_data.createMaskInfo();
				pNewMaskInfo = m_pDataContext->volume_data.getCurrentMaskInfo();
				m_pDataContext->volume_data.setMaskName(DEEPCATCH_WHOLEBODY_MASKNAME_AC, pNewMaskInfo->uid, true);
				pNewMaskInfo->color = COLOR(5, 144, 189, 255);

				if (predictInfo)
					predictInfo->mapDeepCatchPredictResultRoi.insert(DEEPCATCH_WHOLEBODY_MASKNAME_AC, pNewMaskInfo->uid);
			}
			int nSlicenum = -1;
			if (predictInfo->stPredictOpt.preferenceType == eDPTSingleSlice)
				nSlicenum = predictInfo->nAxialDepth;
			else if (predictInfo->stPredictOpt.preferenceType == eDPTMultiSlice)
				nSlicenum = (predictInfo->nStartAxialDepth + predictInfo->nEndAxialDepth) / 2;

			qDebug() << "pPredictedInfo->nAxialDepth" << predictInfo->nAxialDepth;
			qDebug() << "pPredictedInfo->nStartAxialDepth" << predictInfo->nStartAxialDepth;
			qDebug() << "pPredictedInfo->nEndAxialDepth" << predictInfo->nEndAxialDepth;
			qDebug() << "nSliceNum" << nSlicenum;

			m_pWindowManager->CopyAbdominalSkin(DEEPCATCH_WHOLEBODY_MASKNAME_SKIN, DEEPCATCH_WHOLEBODY_MASKNAME_AC, nSlicenum);

			mask8 mastBit = 0;
			int nByteIdx = -1;
			mask8* pMaskDataPoint = nullptr;
			mask8* pMaskDataPointTmp = nullptr;
			MaskInfo* pMaskInfo = m_pDataContext->volume_data.findMaskInfo(DEEPCATCH_WHOLEBODY_MASKNAME_SKIN);

			mastBit = m_pDataContext->volume_data.getMask(pMaskInfo->uid);
			nByteIdx = m_pDataContext->volume_data.GetMaskByteIndex(pMaskInfo->uid);
			pMaskDataPoint = m_pDataContext->volume_data.getMaskDataPoint(nByteIdx);
			m_pDataContext->volume_data.createTempMaskData();
			pMaskDataPointTmp = m_pDataContext->volume_data.getMaskTempDataPoint(0);

			m_pActionManager->action_ImageCircumference(pMaskDataPointTmp, pMaskDataPoint, mastBit, nSlicenum, false);
		}

		else if (THREAD_DEEPCATCH_PREDICT_OPTION == m_pActionManager->GetAfterThread())
		{
			m_pActionManager->NextThreadSetting();

			AISegTabDeepCatch* pAITab = (AISegTabDeepCatch*)m_pWindowManager->GetTab()->getAITab();

			pAITab->SettingPredictOpt(m_pActionManager->m_ListMacroCommandLine.first().rowMap);

			pAITab->slot_OnPredict();
		}
		else if (THREAD_DEEPCATCH_REPORT_START == m_pActionManager->GetAfterThread())
		{
			m_pActionManager->NextThreadSetting();
			OnUpdateProgressMultiThread(0, QString("DeepCatch - Report"));

			AISegTabDeepCatch* pAITab = (AISegTabDeepCatch*)WIN_MANAGER->GetTab()->getAITab();
			pAITab->slot_OnMakeReport();

		}
		else if (THREAD_DEEPCATCH_SAVE_MIPD == m_pActionManager->GetAfterThread())
		{
			m_pActionManager->NextThreadSetting();
			OnUpdateProgressMultiThread(0, STRING_MANAGER->getString(STR_MEDIP_FILE_SAVE));
			QString strPath = STRING_MANAGER->expertReportFilePath;
			QString strReportFolder = strPath.left(strPath.lastIndexOf("/"));

			QString lastOpenFilePath = m_strRecent[0];
			QString lastOpenFileExt = lastOpenFilePath.section(".", -1);
			QString usingFileName = "";
			if (!lastOpenFileExt.compare("mip", Qt::CaseInsensitive) || !lastOpenFileExt.compare("mipd", Qt::CaseInsensitive))
			{
				QString lastOpenFileName = lastOpenFilePath.section("\\", -1);

				// 디렉토리 구분자가 "\\"가 아니라 "/"인 경우.
				if (!lastOpenFileName.compare(lastOpenFilePath, Qt::CaseInsensitive))
					lastOpenFileName = lastOpenFilePath.section("/", -1);

				usingFileName = lastOpenFileName.left(lastOpenFileName.lastIndexOf("."));
			}
			// mip, mipd가 아니면 DICOM 파일이라고 가정.
			else
			{
				usingFileName = lastOpenFilePath.section("\\", -2, -2);

				// 디렉토리 구분자가 "\\"가 아니라 "/"인 경우.
				if (!usingFileName.compare(lastOpenFilePath, Qt::CaseInsensitive))
					usingFileName = lastOpenFilePath.section("/", -2, -2);
			}

			QString strFilePath = strReportFolder + QString("/%1_%2.mipd").arg(m_pActionManager->m_ListMacroCommandLine.first().rowNum).arg(usingFileName);
			m_pActionManager->action_FileWork_Export_MipFile(strFilePath, MIP_ENCODER::PT_MIPD, true, false);

		}
		else if (THREAD_DEEPCATCH_NEXT_LOOP == m_pActionManager->GetAfterThread())
		{
			m_pActionManager->NextThreadSetting();
			m_pActionManager->m_ListMacroCommandLine.pop_front();

			if (m_pActionManager->m_qThreadNext.empty() || m_pActionManager->m_ListMacroCommandLine.isEmpty())
			{
				QApplication::closeAllWindows();
			}
			else
			{
				m_pActionManager->action_DeepCatch_MACRO_Start();
			}
		}
		//SUPPORT_DEEPCATCH_VERSION_2
		else if (THREAD_BONE2VB_PREDICT == m_pActionManager->GetAfterThread() ||
			THREAD_VB2IND_PREDICT == m_pActionManager->GetAfterThread() ||
			THREAD_VB2BMCB_PREDICT == m_pActionManager->GetAfterThread() ||
			THREAD_IO_LIVER_SPLEEN_PREDICT == m_pActionManager->GetAfterThread())	//202204_byPHHS
		{
			qDebug() << "THREAD_BONE2AB_PREDICT";
			eAfterTHREAD thread = m_pActionManager->GetAfterThread();
			m_pActionManager->NextThreadSetting();

			AISegTabDeepCatch* pAITab = (AISegTabDeepCatch*)m_pWindowManager->GetTab()->getAITab();
			const QString dataPath = STRING_MANAGER->LocalAISegPath + "/predict";
			BoundingBoxI box = pAITab->getValidRegion();
			const QString weightPath = STRING_MANAGER->AISegmentationPath + "/weight_deepcatch";

			switch (thread)
			{
			case THREAD_BONE2VB_PREDICT:
			{
				printf_s("\n[  ----- THREAD_BONE2VB_PREDICT ----  ] \n");
				OnUpdateProgressMultiThread(0, QString("Bone to Vertebra"));
				m_pActionManager->action_VB_IO_predict(dataPath, weightPath, MFL_Product_DeepCatch_DeepCatchV2, PROJNAME_BONE2VB, "Bone", ACTP_DEEPCATCH_BONE2VB_PREDICT, box, pAITab->IsUseGPU(), false);

			}
			break;
			case THREAD_VB2IND_PREDICT:
			{
				MaskInfo* pDelMaskInfo = m_pDataContext->volume_data.findMaskInfo(QString("Bone copy"));
				if (pDelMaskInfo)
				{
					m_pDataContext->volume_data.delMaskInfo(pDelMaskInfo->uid);
				}
				printf_s("\n[  ----- THREAD_VB2IND_PREDICT ----  ] \n");
				OnUpdateProgressMultiThread(0, QString("Vertebra to Individual Vertebra"));
				m_pActionManager->action_VB_IO_predict(dataPath, weightPath, MFL_Product_DeepCatch_DeepCatchV2, PROJNAME_VB2IND, DEEPCATCH_VERTEBRA_MASKNAME_BONE2VB, ACTP_DEEPCATCH_VB2IND_PREDICT, box, pAITab->IsUseGPU(), false);
			}
			break;
			case THREAD_VB2BMCB_PREDICT:
			{
				printf_s("\n[  ----- THREAD_VB2BMCB_PREDICT ----  ] \n");
				OnUpdateProgressMultiThread(0, QString("Bone to Bone Mallow, cortical Bone"));
				m_pActionManager->action_VB_IO_predict(dataPath, weightPath, MFL_Product_DeepCatch_DeepCatchV2, PROJNAME_VB2BMCB, DEEPCATCH_VERTEBRA_MASKNAME_BONE2VB, ACTP_DEEPCATCH_VB2BMCB_PREDICT, box, pAITab->IsUseGPU(), false);
			}
			break;
			case THREAD_IO_LIVER_SPLEEN_PREDICT:
			{
				printf_s("\n[  ----- THREAD_IO_CLASS_PREDICT ----  ] \n");
				OnUpdateProgressMultiThread(0, QString("IO Classification"));
				m_pActionManager->action_VB_IO_predict(dataPath, weightPath, MFL_Product_DeepCatch_DeepCatchV2, PROJNAME_IO_CALSS, DEEPCATCH_WHOLEBODY_MASKNAME_IO, ACTP_DEEPCATCH_IO_LIVER_SPLEEN, box, pAITab->IsUseGPU(), false);
			}
			break;
			default:
				break;
			}
		}
		else if (THREAD_IO_AORTA_PREDICT == m_pActionManager->GetAfterThread())	//202207_byPHHS
		{
			qDebug() << "THREAD_BONE2AB_PREDICT";
			eAfterTHREAD thread = m_pActionManager->GetAfterThread();
			m_pActionManager->NextThreadSetting();

			AISegTabDeepCatch* pAITab = (AISegTabDeepCatch*)m_pWindowManager->GetTab()->getAITab();
			const QString dataPath = STRING_MANAGER->LocalAISegPath + "/predict";
			BoundingBoxI box = pAITab->getValidRegion();
			const QString weightPath = STRING_MANAGER->AISegmentationPath + "/weight_deepcatch";

			switch (thread)
			{
			case THREAD_IO_AORTA_PREDICT:
				printf_s("\n[  ----- CT_VesselIO_3DNN ----  ] \n");
				OnUpdateProgressMultiThread(0, QString("Aorta Predict"));
				m_pActionManager->action_IO_Aorta_predict(dataPath, weightPath, MFL_Product_DeepCatch_DeepCatchV2, PROJNAME_IO_AORTA_CALSS, ACTP_DEEPCATCH_IO_AORTA, box, pAITab->IsUseGPU());
				break;
			default:
				break;
			}
		}
		else if (THREAD_L3_AORTA_NONE == m_pActionManager->GetAfterThread())
		{
			m_pActionManager->NextThreadSetting();
			m_pActionManager->action_EmptyStart(ACTP_NONE);
			m_pActionManager->action_ProgressEnd();
			return;
		}
	}

	//#endif
	////////////////////////////
	// deepCatch custom
#if defined(DEEPCATCH_VER_MACRO)
	/////////////////////////// thread DEEPCATCH_VER_MACRO 매크로2
	// ACTP_IMAGE_COMPONENT_CHOISE
	// 	if (THREAD_MACRO_STEP1 == m_pWindowManager->bAfterThread)
	// 	{
	// 		m_pWindowManager->bAfterThread = ACTP_NONE;
	// 
	// 		MaskInfo* pMaskInfo = m_pDataContext->volume_data.findMaskInfo("Bone");
	// 		// action_ImageComponentChoise 에서 현재 선택된 마스크를 기준으로 하기 때문에 미리 선택해줘야 함.
	// 		m_pDataContext->volume_data.setCurrentMaskIndex(pMaskInfo->uid);
	// 
	// 		int nComponent = 1;		
	// 		m_pActionManager->action_ImageComponentChoise(m_pDataContext->volume_data.getMask(pMaskInfo->uid)
	// 			, m_pDataContext->volume_data.GetMaskByteIndex(pMaskInfo->uid), nComponent);
	// 	}
	// ACTP_CLASSIFICATION_PREDICT 
	// 	else if (THREAD_MACRO_STEP2 == m_pWindowManager->bAfterThread)
	// 	{
	// 		m_pWindowManager->bAfterThread = ACTP_NONE;
	// 		const QString projPath = STRING_MANAGER->AIClassificationPath + "/weight";
	// 		QString copyPath = STRING_MANAGER->AIClassificationPath + "/predict_result";
	// 
	// 		int start, end;
	// 		m_pWindowManager->GetTab()->getAITab()->m_tabSet->getDepth(start, end);
	// 		
	// 		QString projName = "Bone473_235";
	// 		QString filePath = projPath + QString("/%1.mipx").arg(projName);
	// 
	// 		m_pActionManager->action_Classification_predict(projName, start, end, true);
	// 	}	
	else if (THREAD_CIRCUMFERENCE == m_pWindowManager->bAfterThread)
	{
		m_pWindowManager->bAfterThread = ACTP_NONE;

		int nL3SliceNum = 0.0;
		auto dResult = std::static_pointer_cast<int>(m_pActionManager->m_qThreadNext.front().pTempData);
		nL3SliceNum = *dResult;

		dResult.reset();
		m_pActionManager->m_qThreadNext.front().pTempData.reset();
		m_pActionManager->m_qThreadNext.pop_front();
		--m_pActionManager->m_nCurrentThreadNum;


		MaskInfo* pMaskInfo = nullptr;
		mask8 mastBit = 0;
		int nByteIdx = -1;
		mask8* pMaskDataPoint = nullptr;
		mask8* pMaskDataPointTmp = nullptr;

		// skin
		pMaskInfo = m_pDataContext->volume_data.findMaskInfo("Skin");

		mastBit = m_pDataContext->volume_data.getMask(pMaskInfo->uid);
		nByteIdx = m_pDataContext->volume_data.GetMaskByteIndex(pMaskInfo->uid);
		pMaskDataPoint = m_pDataContext->volume_data.getMaskDataPoint(nByteIdx);
		m_pDataContext->volume_data.createTempMaskData();
		pMaskDataPointTmp = m_pDataContext->volume_data.getMaskTempDataPoint(0);

		m_pActionManager->action_ImageCircumference(pMaskDataPointTmp, pMaskDataPoint, mastBit, nL3SliceNum);
	}
	else if (THREAD_SAVE_CSV == m_pWindowManager->bAfterThread)
	{
		auto dResult = std::static_pointer_cast<double>(m_pActionManager->m_qThreadNext.front().pTempData);

		float fMuscleVol = 0.0;
		float fVisFatVol = 0.0;
		float fSubFatVol = 0.0;
		float fL3MuscleArea = 0.0;
		float fL3VisFatArea = 0.0;
		float fL3SubFatArea = 0.0;
		float fAbdomialCircumference = *dResult;
		dResult.reset();
		(m_pActionManager->m_qThreadNext.front().pTempData).reset();
		m_pActionManager->NextThreadSetting();

		float fL3SliceNum = 0.0;
		const std::string strL3Num = "L3Number";

		auto iterHash = m_pActionManager->m_hashThreadResult.find(strL3Num);
		if (iterHash != m_pActionManager->m_hashThreadResult.end())
		{
			auto nTmpL3 = std::static_pointer_cast<int>(iterHash->second);
			fL3SliceNum = *(nTmpL3);
			nTmpL3.reset();
			iterHash->second.reset();
			m_pActionManager->m_hashThreadResult.erase(iterHash);
			//m_pDataContext->volume_data.m_L3SliceNum.reset();

		}

		// 		if (m_pDataContext->volume_data.m_L3SliceNum != nullptr)
		// 		{
		// 			fL3SliceNum = *m_pDataContext->volume_data.m_L3SliceNum;
		// 			m_pDataContext->volume_data.m_L3SliceNum.reset();
		// 		}

		int nVoxelCnt = 0;
		float x_spacing = m_pDataContext->volume_data.getSpaceX(false);
		float y_spacing = m_pDataContext->volume_data.getSpaceY(false);
		float z_spacing = m_pDataContext->volume_data.getSpaceZ(false);

		int cx = m_pDataContext->volume_data.getCX();
		int cy = m_pDataContext->volume_data.getCY();
		int cz = m_pDataContext->volume_data.getCZ();

		MaskInfo* pMaskInfo = nullptr;
		mask8 mastBit = 0;
		int nByteIdx = -1;
		mask8* pMaskDataPoint = nullptr;


		// muscle volume(cm3)
		pMaskInfo = m_pDataContext->volume_data.findMaskInfo("Muscle");
		nVoxelCnt = m_pDataContext->volume_data.getVoxelCount(pMaskInfo->uid);
		fMuscleVol = nVoxelCnt * x_spacing * y_spacing * z_spacing;

		// L3 muscle volume(cm2)
		mastBit = m_pDataContext->volume_data.getMask(pMaskInfo->uid);
		nByteIdx = m_pDataContext->volume_data.GetMaskByteIndex(pMaskInfo->uid);
		pMaskDataPoint = m_pDataContext->volume_data.getMaskDataPoint(nByteIdx);

		nVoxelCnt = 0;
		for (int y = 0; y < cy; ++y)
		{
			for (int x = 0; x < cx; ++x)
			{
				int idx = fL3SliceNum * cy * cx + y * cx + x;
				if (pMaskDataPoint[idx] & mastBit)
				{
					++nVoxelCnt;
				}
			}
		}
		fL3MuscleArea = nVoxelCnt * x_spacing * y_spacing;

		// visfat volume(cm3)
		pMaskInfo = m_pDataContext->volume_data.findMaskInfo("AVF");
		nVoxelCnt = m_pDataContext->volume_data.getVoxelCount(pMaskInfo->uid);
		fVisFatVol = nVoxelCnt * x_spacing * y_spacing * z_spacing;

		// L3 visfat volume(cm2)
		mastBit = m_pDataContext->volume_data.getMask(pMaskInfo->uid);
		nByteIdx = m_pDataContext->volume_data.GetMaskByteIndex(pMaskInfo->uid);
		pMaskDataPoint = m_pDataContext->volume_data.getMaskDataPoint(nByteIdx);

		nVoxelCnt = 0;
		for (int y = 0; y < cy; ++y)
		{
			for (int x = 0; x < cx; ++x)
			{
				int idx = fL3SliceNum * cy * cx + y * cx + x;
				if (pMaskDataPoint[idx] & mastBit)
				{
					++nVoxelCnt;
				}
			}
		}
		fL3VisFatArea = nVoxelCnt * x_spacing * y_spacing;

		// subfat volume(cm3)
		pMaskInfo = m_pDataContext->volume_data.findMaskInfo("OF");
		nVoxelCnt = m_pDataContext->volume_data.getVoxelCount(pMaskInfo->uid);
		fSubFatVol = nVoxelCnt * x_spacing * y_spacing * z_spacing;

		// L3 subfat volume(cm2)
		mastBit = m_pDataContext->volume_data.getMask(pMaskInfo->uid);
		nByteIdx = m_pDataContext->volume_data.GetMaskByteIndex(pMaskInfo->uid);
		pMaskDataPoint = m_pDataContext->volume_data.getMaskDataPoint(nByteIdx);

		nVoxelCnt = 0;
		for (int y = 0; y < cy; ++y)
		{
			for (int x = 0; x < cx; ++x)
			{
				int idx = fL3SliceNum * cy * cx + y * cx + x;
				if (pMaskDataPoint[idx] & mastBit)
				{
					++nVoxelCnt;
				}
			}
		}
		fL3SubFatArea = nVoxelCnt * x_spacing * y_spacing;

		qDebug() << ">>>>>>>>result";
		qDebug() << fMuscleVol;
		qDebug() << fVisFatVol;
		qDebug() << fSubFatVol;
		qDebug() << fL3MuscleArea;
		qDebug() << fL3VisFatArea;
		qDebug() << fL3SubFatArea;
		qDebug() << fAbdomialCircumference;
		qDebug() << fL3SliceNum;

		QString fileDir = "c:\\MedipTemp";
		QString fileName = "result.csv";
		QString filePath = fileDir + QString("\\") + fileName;

		QDir dir(fileDir);
		if (!dir.exists())
			dir.mkdir(fileDir);

		QFile newFile(filePath);

		QString copyStr = "";

		if (!newFile.exists())
		{
			copyStr.append(QString("ID"));
			copyStr.append(",");
			copyStr.append(QString("muscle volume (cm3)"));
			copyStr.append(",");
			copyStr.append(QString("visfat volume (cm3)"));
			copyStr.append(",");
			copyStr.append(QString("subfat volume (cm3)"));
			copyStr.append(",");
			copyStr.append(QString("L3 muscle area (cm2)"));
			copyStr.append(",");
			copyStr.append(QString("L3 visfat area (cm2)"));
			copyStr.append(",");
			copyStr.append(QString("L3 subfat area (cm2)"));
			copyStr.append(",");
			copyStr.append(QString("abdominal circumference (cm)"));
			copyStr.append(",");
			copyStr.append(QString("L3 slice number"));
			copyStr.append("\r\n");
		}

		QString InputPath = m_pWindowManager->m_strMacroInputPath;

		QStringList strTmpList = InputPath.split("\\\\");
		QString strDcmFolder = strTmpList.at(strTmpList.size() - 2);

		copyStr.append(strDcmFolder);
		copyStr.append(",");
		copyStr.append(QString::number(fMuscleVol));
		copyStr.append(",");
		copyStr.append(QString::number(fVisFatVol));
		copyStr.append(",");
		copyStr.append(QString::number(fSubFatVol));
		copyStr.append(",");
		copyStr.append(QString::number(fL3MuscleArea));
		copyStr.append(",");
		copyStr.append(QString::number(fL3VisFatArea));
		copyStr.append(",");
		copyStr.append(QString::number(fL3SubFatArea));
		copyStr.append(",");
		copyStr.append(QString::number(fAbdomialCircumference));
		copyStr.append(",");
		copyStr.append(QString::number(m_pDataContext->volume_data.getCZ() - fL3SliceNum - 1));
		copyStr.append("\r\n");


		if (!newFile.open(QIODevice::WriteOnly | QIODevice::Append))
		{
			QMessageBox::warning(nullptr, "Feature file save fail.", "csv file create fail.");
			QApplication::closeAllWindows();
			return;
		}

		newFile.write((const char*)copyStr.toStdWString().c_str(), sizeof(WCHAR) * copyStr.size());

		//			QMessageBox::warning(this, filePath, "here.");
		newFile.close();

		QApplication::closeAllWindows();
		}

	return;

#endif




#if !defined(COVID19_VER) && !defined(DEEPCATCH_VER_MACRO)
	/////////////////////////// thread 매크로
	// 반드시 각 action 함수에서 connect 로 OnThreadEnd가 연결되어있어야 다음 STEP 으로 넘어감.
	// 아래는 딥러닝 predict 추출을 위한 스텝(전은주 교수) 2020.02.19 - 이두희 차장.
	if (THREAD_MACRO_STEP0 == m_pActionManager->GetAfterThread())
	{
		m_pDataContext->volume_data.setCurrentMaskIndex(0);
		MaskInfo* info = m_pDataContext->volume_data.getMaskInfo(0);
		std::vector<muint32> vecSelect;
		vecSelect.push_back(info->uid);
		m_pDataContext->volume_data.setMultiSelectUID(vecSelect);
		QString filename;
		m_pWindowManager->lastestPathGet(filename);
		filename.replace("*.*", "");

		QString parentname;
		parentname = filename;

		QStringList Name = parentname.split("\\");
		int listCount = parentname.split("\\").length();


		QString mip_filename = filename + Name[listCount - 3] + ".mip";


		//m_pActionManager->action_FileWork_Export_MipFile(mip_filename, true);
		m_pActionManager->action_FileWork_Export_MipFile(mip_filename, MIP_ENCODER::PT_MIP);
		//		m_pActionManager->action_FileWork_Export_RawFile(filename, m_pWindowManager->getSelectedMask(), m_pWindowManager->getSelectedMaskIndex());
		m_pActionManager->SetAfterThread(THREAD_MACRO_STEP1);

	}


	else if (THREAD_MACRO_STEP1 == m_pActionManager->GetAfterThread())
	{//  끝
		emit ProgramClose();
		QMainWindow::close();
		return;
	}

#endif
	///////////////////////////////////////////////////////////////////////////////////////////////

	if (THREAD_OPEN_FILE == m_pActionManager->GetAfterThread()) //open after save mip file
	{
		m_pActionManager->SetAfterThread(THREAD_NONE);
		fileOpen(m_StrOpen);
	}
	else if (THREAD_OPEN_IMPORT_DATA == m_pActionManager->GetAfterThread())
	{
		m_pActionManager->SetAfterThread(THREAD_NONE);
		OpenImportData(false);
	}
	else if (THREAD_UPLOAD == m_pActionManager->GetAfterThread()) //ftpupload after save mip file
	{
		QString strLatest;
		bool latest = m_pWindowManager->lastestPathGet(strLatest);

		m_pActionManager->SetAfterThread(THREAD_NONE);

		if (latest)
		{
			m_pActionManager->action_ftpUpload(strLatest);
		}
	}
	else if (THREAD_PREVIEW_MESHES == m_pActionManager->GetAfterThread()) //preview layers
	{
		if (m_pWindowManager->exportList.isEmpty())
		{
			m_pActionManager->SetAfterThread(THREAD_NONE);
			return;
		}

	}
	else if (THREAD_TA_UPDATE == m_pActionManager->GetAfterThread()) //add/modify layers
	{
		if (m_pWindowManager->exportList.isEmpty())
		{
			m_pActionManager->SetAfterThread(THREAD_NONE);
			return;
		}

		int _uid = m_pWindowManager->exportList.takeFirst();
		MaskInfo* info = m_pDataContext->volume_data.getMaskInfo(_uid, true);
		int mI = info->uid >= MASK_SECOND_MAX ? (info->uid - MASK_SECOND_MAX) / 8 + 1 : 0;

		m_pActionManager->action_ImageFeatureExtractor_Add(mI == 0 ? info->mask_id : info->mask_id2, mI, info->uid);
	}
	else if (m_pActionManager->GetAfterThread() == THREAD_SPLIT_TO_RAWS)
	{
		m_pActionManager->SetAfterThread(THREAD_NONE);
		m_pActionManager->action_FileWork_Split_ToFile();
	}
	else if (m_pActionManager->GetAfterThread() == THREAD_IMPORT_FILES)
	{
		if (m_pWindowManager->IsImportListEmpty())
		{
			m_pActionManager->SetAfterThread(THREAD_NONE);
		}
		else
		{
			m_pWindowManager->loadNextFiles();
		}
	}
	else if (THREAD_VISUAL_PRINT_DIALOG == m_pActionManager->GetAfterThread())	// Visual Print Dialog 실행
	{
		m_pActionManager->SetAfterThread(THREAD_NONE);
		m_pActionManager->m_qThreadNext.pop_front();
		--m_pActionManager->m_nCurrentThreadCount;

		m_pWindowManager->roiSaveMeshList.clear();

		// 201005 프로그레스바 종료
		m_pActionManager->action_ProgressEnd();

		m_pWindowManager->enableMeshList(false);
		m_pWindowManager->enableROIList(false);

		// Visual Print Popup Dialog 실행		
		if (m_pWindowManager->GetTab()->getVisualPrintTab()->CheckVisualPrintDlgCreated() == false)
		{
			// For CT Plane Preview			
			m_pWindowManager->GetTab()->getVisualPrintTab()->CreateVisualPrintDlg(m_pDataContext, this);
			m_pWindowManager->GetTab()->getVisualPrintTab()->GetVisualPrintDlg()->Update();
			m_pWindowManager->GetTab()->getVisualPrintTab()->GetVisualPrintDlg()->exec();
		}
		else
		{
			m_pWindowManager->GetTab()->getVisualPrintTab()->GetVisualPrintDlg()->Update();
			m_pWindowManager->GetTab()->getVisualPrintTab()->GetVisualPrintDlg()->exec();
		}
	}
	else if (THREAD_VISUAL_PRINT_UPLOAD == m_pActionManager->GetAfterThread()) // upload stl for visual print
	{
		m_pActionManager->SetAfterThread(THREAD_NONE);

		auto dResult = std::static_pointer_cast<tstring>(m_pActionManager->m_qThreadNext.front().pTempData);

		if (dResult)
		{
			dResult.reset();
		}
		m_pActionManager->m_qThreadNext.front().pTempData.reset();

		m_pActionManager->NextThreadSetting();

		if (m_pWindowManager->roiUploadList.size() == 0)
		{
			return;
		}

		m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_VISUAL_PRINT_UPLOAD, THREAD_VISUAL_PRINT_UPLOAD_ROI, nullptr));
		QString strLatest = m_pWindowManager->roiUploadList.at(0);
		m_pActionManager->action_VisualPrintFTPUpload(m_pDataContext, strLatest, true);
	}
	else if (THREAD_VISUAL_PRINT_UPLOAD_ROI == m_pActionManager->GetAfterThread()) // upload stl for visual print
	{
		m_pActionManager->NextThreadSetting();

		m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_VISUAL_PRINT_UPLOAD_ROI, THREAD_VISUAL_PRINT_RUN, nullptr));

		QString current_path = STRING_MANAGER->m_strAppDataLocalPath; //20210319_byPHS_관리자권한으로 인한 수정
		QString strLatest = current_path + "\\" + "Temp" + "\\" + "files.js";
		m_pActionManager->action_VisualPrintFTPUploadROI(m_pDataContext, strLatest);
	}
	else if (THREAD_VISUAL_PRINT_RUN == m_pActionManager->GetAfterThread()) // upload stl for visual print
	{
		m_pActionManager->NextThreadSetting();

		m_pWindowManager->roiDeleteList.clear();
		m_pWindowManager->roiUploadList.clear();


		// Visual Print Popup Dialog 실행		
		if (m_pWindowManager->GetTab()->getVisualPrintTab()->CheckVisualPrintDlgCreated())
		{
			m_pWindowManager->GetTab()->getVisualPrintTab()->GetVisualPrintDlg()->close();
		}

		m_pWindowManager->VisualPrintingWebLink(m_pDataContext);
	}
	else if (THREAD_MEDIP_MACRO_END == m_pActionManager->GetAfterThread())
	{
		QApplication::closeAllWindows();
	}

	if (DATA_CONTEXT->GetOmniverseContext()->GetStage()->IsLiveSessionMode())
	{
		qInfo() << "push omniverse update event";
		DATA_CONTEXT->GetOmniverseContext()->GetStage()->PushMeshWriteEvent();
	}


	if (m_pActionManager->m_qThreadNext.empty() /*|| m_pActionManager->m_qThreadNext.front().pWorkThread == nullptr*/)
	{
		m_pActionManager->action_ProgressEnd();
		qDebug() << "action_ProgressEnd";
	}
	}

void MedipQT::OnUpdateProgress(int value)
{
	m_pActionManager->action_ProgressUpdate(value);
}

void MedipQT::slot_OnUpdateProgress(int value, QString _str)
{
	m_pActionManager->action_ProgressUpdate(value, _str);
}

void MedipQT::OnUpdateProgressMultiThread(int value, QString strWorkName)
{
	//int nCurrentThreadNum = m_pActionManager->m_qThreadNext.size();
	int nCurrentThreadNum = m_pActionManager->m_nCurrentThreadCount;
	if (nCurrentThreadNum < 0)
		return;

	int nTotalThreadNum = m_pActionManager->m_nMultiThreadTotalCount;

	int nPercentValue = (float)(nTotalThreadNum - nCurrentThreadNum) / (float)nTotalThreadNum * 100.f +
		(float)value / (float)nTotalThreadNum;

	qDebug() << "value: " << value;
	qDebug() << "nTotalThreadNum : " << nTotalThreadNum;
	qDebug() << "current thread Num : " << nCurrentThreadNum;
	qDebug() << "nPercentValue: " << nPercentValue;

	m_pActionManager->action_ProgressUpdate(nPercentValue, strWorkName);
}

void MedipQT::OnThreadCancel()
{
	m_pDataContext->volume_data.threadStop = true;
	qInfo() << "request thread stop : " << m_pDataContext->volume_data.threadStop;
	//	ActionManager::action_ThreadExit();
}

// deepcatch report operation function
void MedipQT::deepcatchResultToReport(bool bUseSingleThreadProgress)
{

#ifdef SUPPORT_DEEPCATCH_V2_DOCTORANSWER	
	std::vector<float> hccResult;
	QImage chartImg;
	bool bIsChratImg = getChartImg(chartImg);
	bool bIsHccResult = getHCCResult(hccResult);
#endif

	DEEPCATCH_REPORT* pReport = m_pActionManager->DeepcatchReportInfoSafeCreate();
	pReport->tempInfo.bUseSingleThreadProgress = bUseSingleThreadProgress;
#ifndef DEEPCATCH_REPORT_150PPI_INTEGRATE	// 해상도에 따른 report res type 설정.
	QScreen* screen = QGuiApplication::primaryScreen();
	QRect  screenGeometry = screen->geometry();
	int height = screenGeometry.height();
	int width = screenGeometry.width();
	QVector<QPair<QSizeF, QString>> vecA4Size;
	vecA4Size.append(QPair<QSizeF, QString>(QSizeF(595.0f, 842.0f), QString("595x842")));			// 72 ppi
	//	vecA4Size.append(QPair<QSizeF, QString>(QSizeF(794.0f, 1123.0f), QString("794x1123")));			// 96 ppi
	vecA4Size.append(QPair<QSizeF, QString>(QSizeF(1240.0f, 1754.0f), QString("1240x1754")));		// 150 ppi
	//	vecA4Size.append(QPair<QSizeF, QString>(QSizeF(2480.0f, 3508.0f), QString("2480x3508")));		// 300 ppi
	int findIndex = -1;
	for (int i = 0; i < vecA4Size.size(); i++)
	{
		if (vecA4Size.at(i).first.width() <= width && vecA4Size.at(i).first.height() <= height)
			findIndex = i;
	}
	if (findIndex != -1)
		pReport->curA4Res = (eA4ResolutionType)findIndex;
	else
		pReport->curA4Res = eART72PPI;
#else	// 150ppi로 고정 건.
	pReport->curA4Res = eART150PPI;
#endif
#ifdef SUPPORT_DEEPCATCH_V2_DOCTORANSWER
	if (bIsChratImg && bIsHccResult)
	{
		pReport->chartImg = chartImg;
		hccResult.swap(pReport->HCC_Result_Percentage);
	}
#endif

	//	if (m_pActionManager->m_IsMacroMode == false)
	{
		OnUpdateProgressMultiThread(0, QString("DeepCatch - Report"));

		// volume view capture timer start.
		m_pReportVolumeVeiwCaptureTimer = new QTimer(this);
		connect(m_pReportVolumeVeiwCaptureTimer, SIGNAL(timeout()), this, SLOT(reportVolumeViewCapture()));
		m_pReportVolumeVeiwCaptureTimer->start(100);
	}
	//	else
	//		m_pActionManager->action_DeepCatch_FeatureExtract(pReport->tempInfo.bUseSingleThreadProgress);
}

void MedipQT::deepcatchResultToExpertReport()
{
	QString filePath = STRING_MANAGER->expertReportFilePath;
	// 파일 경로가 잘못된 경우 예외 처리.
	QString fileExtension = filePath.right(4);
	bool bValid = false;
	if (!fileExtension.compare(".csv", Qt::CaseInsensitive))
		bValid = true;
	if (!bValid)
	{
		QMessageBox::warning(this, "Expert report", "Expert report file path is invalid.(the file path was set in option tab). please check the file path.");
		return;
	}

	QFileInfo info(filePath);
	QDir dir(info.dir());
	if (!dir.exists())
		dir.mkpath(".");

	QFile newFile(filePath);
	QString copyStr = "";
	QString numberLine = "1";

	DEEPCATCH_REPORT* pReport = m_pActionManager->getDeepCatchAnalysisVals();
	DEEPCATCH_REPORT_PREDICT_INFO* pPredictedInfo = m_pActionManager->getDeepCatch_PredictedInfo();
	sReportOtherInfo* pOtherInfo = m_pActionManager->getReportOtherInfo();

	if (!newFile.exists())
	{
		QString mm2 = QString::fromLocal8Bit("mm²");
		QString mm3 = QString::fromLocal8Bit("mm³");
		QString gm2 = QString::fromLocal8Bit("g/height(㎡)");
		if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2))
		{
			QStringList firstRowList{
				"", "", "", "Predict option", "", "", "", "Slice selection",
				"", "", "", "", "",
				"", "", "Metadata", "", "",
				"", "", "", "", "",
				"", "", "", "Mask volume in the entire CT images", "",
				"", "", "", "", "",
				"Anatomic location of abdominal waist", "", "", "", "Anatomic location of L3 slice",
				"Mask volume in the abdominal waist", "", "", "", "",
				"", "", "Average mask area in the abdominal waist(AK ~ AQ / AI)", "", "",
				"", "", "", "", "Mask area in the L3 single slice", // 45~50
				"", "", "", "", "",
				"", "Mean mask CT attenuation in the abdominal waist", "", "", "",
				"", "", "", "Mean mask CT attenuation at the L3 single slice", "",
				"", "", "", "", "",
				"abdominal circumference", "", "", "Mask mass in the entire CT images", "",
				"", "Mask mass in the abdominal waist", "", "", "Mask mass in the L3",
				"", "", "Muscle volume by interval in the entire CT images", "", "",
				"", "Muscle volume by interval in the abdominal waist", "", "", "",
				"Muscle area by interval in the L3 single slice", "", "", "", "",
				"Volunme for Bone Analysis",   "", "", "", "", "",    "", "", "", "", "",   "", "", "", "", "",   "",
				"Mean HU for Bone Analysis",   "", "", "", "", "",    "", "", "", "", "",   "", "", "", "", "",   "",
				"T-score for Bone Analysis", "Mask volunme (liver)", "HU in mask volume (liver)", "mean HU in mask volume (liver)", "Mask volunme (spleen)",
				"HU in mask volume (spleen)", "mean HU in mask volume (spleen)", "Mask area (liver)  in the single slice", "Mask area (spleen)  in the single slice", "Aorta Calcification",
			};
			QStringList secondRowList{
				"", "", "",  "Contrast", "Body type", "Bone Analysis","IO Classification", "L3 single slice",
				"Abdominal waist", "Gender", "", "", "",
				"Preference for reporting", "", "Matrix", "Matrix", "Matrix",
				"mm", "mm", "mm", mm2, mm3,
				"", "", "", mm3, mm3,
				mm3, mm3, mm3, mm3, mm3,
				"slice number", "slice number", "array index", "mm", "slice number",
				mm3, mm3, mm3, mm3, mm3,
				mm3, mm3, mm2, mm2, mm2,
				mm2, mm2, mm2, mm2, mm2,  // 45~50
				mm2, mm2, mm2, mm2, mm2,
				mm2, "HU", "HU", "HU", "HU",
				"HU", "HU", "HU", "HU", "HU",
				"HU", "HU", "HU", "HU", "HU",
				"mm", "mm", "mm", gm2, gm2,
				gm2, gm2, gm2, gm2, gm2,
				gm2, gm2, mm3, mm3, mm3,
				mm3, mm3, mm3, mm3, mm3,
				mm2, mm2, mm2, mm2, "",
				mm3,   mm3, mm3, mm3, mm3, mm3,    mm3, mm3, mm3, mm3, mm3,   mm3, mm3, mm3, mm3, mm3,   mm3,
				"HU",  "HU", "HU", "HU", "HU", "HU",    "HU", "HU", "HU", "HU", "HU",   "HU", "HU", "HU", "HU", "HU",   "HU",
				"T-Score", mm3, mm3, mm3, mm3,
				mm3, mm3, mm3, mm3, mm3,
			};

			QString adiposeTissueStr = QString("adipose tissue(%1 ~ %2 HU)").arg(pPredictedInfo->stPredictOpt.nMuslceQualityMapStartHU[eMQMCAptTissue]).arg(pPredictedInfo->stPredictOpt.nMuslceQualityMapEndHU[eMQMCAptTissue]);
			QString abnormalMuscle1Str = QString("abnormal muscle 1(%1 ~ %2 HU)").arg(pPredictedInfo->stPredictOpt.nMuslceQualityMapStartHU[eMQMCAbnormal1]).arg(pPredictedInfo->stPredictOpt.nMuslceQualityMapEndHU[eMQMCAbnormal1]);
			QString abnormalMuscle2Str = QString("abnormal muscle 2(%1 ~ %2 HU)").arg(pPredictedInfo->stPredictOpt.nMuslceQualityMapStartHU[eMQMCAbnormal2]).arg(pPredictedInfo->stPredictOpt.nMuslceQualityMapEndHU[eMQMCAbnormal2]);
			QString normalMuscleStr = QString("normal muscle(%1 ~ %2 HU)").arg(pPredictedInfo->stPredictOpt.nMuslceQualityMapStartHU[eMQMCNormal]).arg(pPredictedInfo->stPredictOpt.nMuslceQualityMapEndHU[eMQMCNormal]);
			QStringList thirdRowList{
				"No", "File name", "DICOM Series Description", "Noncontrast or Contrast", "WholeBody or Chest", "Use or none", "liver/spleen/aorta or none",
				"Auto or user define", "Auto or user define", "Female or Male", "height", "weight",
				"BMI", "L3 or Abdominal waist", "Predict date", "x size", "y size",
				"z size", "space X", "space Y", "space Z", "space 2D",
				"space 3D", "Kernel", "Kvp", "mAs", "Skin",
				"Bone", "Muscle", "AVF", "SF", "IO",
				"CNS", "start position", "end position", "length", "length",
				"position", "Skin", "Bone", "Muscle", "AVF",
				"SF", "IO", "CNS", "Skin", "Bone",
				"Muscle", "AVF", "SF", "IO", "CNS",
				"Skin", "Bone", "Muscle", "AVF", "SF",
				"IO", "CNS", "Skin", "Bone", "Muscle",
				"AVF", "SF", "IO", "CNS", "Skin",
				"Bone", "Muscle", "AVF", "SF", "IO",
				"CNS", "Circumference", "max x length", "max y length", "Muscle",
				"AVF", "SF", "Muscle", "AVF", "SF",
				"Muscle", "AVF", "SF", adiposeTissueStr, abnormalMuscle1Str,
				abnormalMuscle2Str, normalMuscleStr, adiposeTissueStr, abnormalMuscle1Str, abnormalMuscle2Str,
				normalMuscleStr, adiposeTissueStr, abnormalMuscle1Str, abnormalMuscle2Str, normalMuscleStr,
				"-", "L1",   "L2", "L3", "L4", "L5", "T1",    "T2", "T3", "T4", "T5", "T6",   "T7", "T8", "T9", "T10", "T11",   "T12",
				"L1",   "L2", "L3", "L4", "L5", "T1",    "T2", "T3", "T4", "T5", "T6",   "T7", "T8", "T9", "T10", "T11",   "T12",
				"L1", "liver", "liver", "liver", "spleen",
				"spleen", "spleen", "liver", "spleen", "",
			};

			for (int i = 0; i < firstRowList.size(); i++)
			{
				copyStr += firstRowList.at(i);
				if (i == (firstRowList.size() - 1))	// 마지막에는 개행 문자.
					copyStr += "\r\n";
				else
					copyStr += ",";
			}
			for (int i = 0; i < secondRowList.size(); i++)
			{
				copyStr += secondRowList.at(i);
				if (i == (secondRowList.size() - 1))	// 마지막에는 개행 문자.
					copyStr += "\r\n";
				else
					copyStr += ",";
			}
			for (int i = 0; i < thirdRowList.size(); i++)
			{
				copyStr += thirdRowList.at(i);
				if (i == (thirdRowList.size() - 1))		// 마지막에는 개행 문자.
					copyStr += "\r\n";
				else
					copyStr += ",";
			}
		}
		else
		{
			QStringList firstRowList{
				"", "", "Predict option", "", "Slice selection",
				"", "", "", "", "",
				"", "", "Metadata", "", "",
				"", "", "", "", "",
				"", "", "", "Mask volume in the entire CT images", "",
				"", "", "", "", "",
				"Anatomic location of abdominal waist", "", "", "", "Anatomic location of L3 slice",
				"Mask volume in the abdominal waist", "", "", "", "",
				"", "", "Average mask area in the abdominal waist(AK ~ AQ / AI)", "", "",
				"", "", "", "", "Mask area in the L3 single slice", // 45~50
				"", "", "", "", "",
				"", "Mean mask CT attenuation in the abdominal waist", "", "", "",
				"", "", "", "Mean mask CT attenuation at the L3 single slice", "",
				"", "", "", "", "",
				"abdominal circumference", "", "", "Mask mass in the entire CT images", "",
				"", "Mask mass in the abdominal waist", "", "", "Mask mass in the L3",
				"", "", "Muscle volume by interval in the entire CT images", "", "",
				"", "Muscle volume by interval in the abdominal waist", "", "", "",
				"Muscle area by interval in the L3 single slice", "", "", "", "",
			};
			QStringList secondRowList{
				"", "", "UNET", "Contrast", "L3 single slice",
				"Abdominal waist", "Gender", "", "", "",
				"Preference for reporting", "", "Matrix", "Matrix", "Matrix",
				"mm", "mm", "mm", mm2, mm3,
				"", "", "", mm3, mm3,
				mm3, mm3, mm3, mm3, mm3,
				"slice number", "slice number", "array index", "mm", "slice number",
				mm3, mm3, mm3, mm3, mm3,
				mm3, mm3, mm2, mm2, mm2,
				mm2, mm2, mm2, mm2, mm2,  // 45~50
				mm2, mm2, mm2, mm2, mm2,
				mm2, "HU", "HU", "HU", "HU",
				"HU", "HU", "HU", "HU", "HU",
				"HU", "HU", "HU", "HU", "HU",
				"mm", "mm", "mm", gm2, gm2,
				gm2, gm2, gm2, gm2, gm2,
				gm2, gm2, mm3, mm3, mm3,
				mm3, mm3, mm3, mm3, mm3,
				mm2, mm2, mm2, mm2, "",
			};

			QString adiposeTissueStr = QString("adipose tissue(%1 ~ %2 HU)").arg(pPredictedInfo->stPredictOpt.nMuslceQualityMapStartHU[eMQMCAptTissue]).arg(pPredictedInfo->stPredictOpt.nMuslceQualityMapEndHU[eMQMCAptTissue]);
			QString abnormalMuscle1Str = QString("abnormal muscle 1(%1 ~ %2 HU)").arg(pPredictedInfo->stPredictOpt.nMuslceQualityMapStartHU[eMQMCAbnormal1]).arg(pPredictedInfo->stPredictOpt.nMuslceQualityMapEndHU[eMQMCAbnormal1]);
			QString abnormalMuscle2Str = QString("abnormal muscle 2(%1 ~ %2 HU)").arg(pPredictedInfo->stPredictOpt.nMuslceQualityMapStartHU[eMQMCAbnormal2]).arg(pPredictedInfo->stPredictOpt.nMuslceQualityMapEndHU[eMQMCAbnormal2]);
			QString normalMuscleStr = QString("normal muscle(%1 ~ %2 HU)").arg(pPredictedInfo->stPredictOpt.nMuslceQualityMapStartHU[eMQMCNormal]).arg(pPredictedInfo->stPredictOpt.nMuslceQualityMapEndHU[eMQMCNormal]);
			QStringList thirdRowList{
				"No", "File name", "DICOM Series Description", "2D or 3D", "Noncontrast or Contrast",
				"Auto or user define", "Auto or user define", "Female or Male", "height", "weight",
				"BMI", "L3 or Abdominal waist", "Predict date", "x size", "y size",
				"z size", "space X", "space Y", "space Z", "space 2D",
				"space 3D", "Kernel", "Kvp", "mAs", "Skin",
				"Bone", "Muscle", "AVF", "SF", "IO",
				"CNS", "start position", "end position", "length", "length",
				"position", "Skin", "Bone", "Muscle", "AVF",
				"SF", "IO", "CNS", "Skin", "Bone",
				"Muscle", "AVF", "SF", "IO", "CNS",
				"Skin", "Bone", "Muscle", "AVF", "SF",
				"IO", "CNS", "Skin", "Bone", "Muscle",
				"AVF", "SF", "IO", "CNS", "Skin",
				"Bone", "Muscle", "AVF", "SF", "IO",
				"CNS", "Circumference", "max x length", "max y length", "Muscle",
				"AVF", "SF", "Muscle", "AVF", "SF",
				"Muscle", "AVF", "SF", adiposeTissueStr, abnormalMuscle1Str,
				abnormalMuscle2Str, normalMuscleStr, adiposeTissueStr, abnormalMuscle1Str, abnormalMuscle2Str,
				normalMuscleStr, adiposeTissueStr, abnormalMuscle1Str, abnormalMuscle2Str, normalMuscleStr,
			};

			for (int i = 0; i < firstRowList.size(); i++)
			{
				copyStr += firstRowList.at(i);
				if (i == (firstRowList.size() - 1))	// 마지막에는 개행 문자.
					copyStr += "\r\n";
				else
					copyStr += ",";
			}
			for (int i = 0; i < secondRowList.size(); i++)
			{
				copyStr += secondRowList.at(i);
				if (i == (secondRowList.size() - 1))	// 마지막에는 개행 문자.
					copyStr += "\r\n";
				else
					copyStr += ",";
			}
			for (int i = 0; i < thirdRowList.size(); i++)
			{
				copyStr += thirdRowList.at(i);
				if (i == (thirdRowList.size() - 1))		// 마지막에는 개행 문자.
					copyStr += "\r\n";
				else
					copyStr += ",";
			}
		}
	}
	else
	{
		// 마지막 넘버를 읽어와 현재 등록할 넘버 계산.
		QStringList strList;
		if (newFile.open(QIODevice::ReadOnly))
		{
			QString line = "";
			QTextStream textStream(&newFile);
			while (textStream.readLineInto(&line))
				strList << line;
			newFile.close();
		}
		QString lastStr = "";
		QString firstStr = "";
		for (int i = strList.size() - 1; i >= 0; i--)
		{
			lastStr = strList.at(i);
			QStringList strListCellSting = lastStr.split(",", QString::KeepEmptyParts);
			if (!strListCellSting.isEmpty())
			{
				firstStr = strListCellSting.first();
				if (!firstStr.isEmpty())
				{
					bool ok = false;
					int curval = firstStr.toInt(&ok);
					if (ok)
					{
						numberLine = QString::number(++curval);
						break;
					}
				}
			}
		}
	}

	// input data 
	//	QFileInfo fi(m_strRecent[0]);
	//	QString curOpenedFileName = fi.fileName();
	QString curOpenedFileName = m_strRecent[0];

	QString strSelectedDicomSeriesDescription = "";
#if 0
	if (m_pActionManager->m_IsMacroMode)
	{
		if (pOtherInfo && !pOtherInfo->strSelectedSeriesDescription.isEmpty())
			strSelectedDicomSeriesDescription = pOtherInfo->strSelectedSeriesDescription;
	}
	else
	{
		// 파일 매니저로 다이콤 오픈시.
		if (!m_pImportData->GetDcmSeriesInfo()->description_.empty())
			strSelectedDicomSeriesDescription = QString::fromStdString(m_pImportData->GetDcmSeriesInfo()->description_);
}
#else
	DcmtkSeriesInfo* dicomFileInfo = m_pWindowManager->GetDicomInfo();
	if (dicomFileInfo)
	{
		strSelectedDicomSeriesDescription = QString::fromStdString(dicomFileInfo->description_);
	}
#endif

	QString strUNETType = "";
	QString strContrastType = "";
	QString strCompositionType = "";
	QString strVBNetworkType = "";
	QString strIONetWorkType = "";
	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2))
	{
		switch (pPredictedInfo->stPredictOpt.contrastType)
		{
		case eDCTNoneContrast:
			strContrastType = "None Contrast";
			break;
		case eDCTContrast:
		default:
			strContrastType = "Contrast";
			break;
		}

		switch (pPredictedInfo->stPredictOpt.BodyCompositionType)
		{
		case eDCBTAbdomenWholeBody:
			strCompositionType = "WholeBody";
			break;
		case eDCBTChest:
		default:
			strCompositionType = "Chest";
			break;
		}

		switch (pPredictedInfo->stPredictOpt.useVBNetwork)
		{
		case eUseVertebraNetwork:
			strVBNetworkType = "Use";
			break;
		case eDCVTNone:
		default:
			strVBNetworkType = "none";
			break;
		}

		switch (pPredictedInfo->stPredictOpt.useIONetwork)
		{
		case eDCIO_Liver_Spleen:
			strIONetWorkType = "liver/spleen";
			break;
		case eDCIO_Aorta:
			strIONetWorkType = "aorta";
			break;
		case eDCIO_Both:
			strIONetWorkType = "liver/spleen/aorta";
			break;
		case eDCIOTNone:
		default:
			strIONetWorkType = "none";
			break;
		}
	}
	else
	{
		switch (pPredictedInfo->stPredictOpt.UNETType)
		{
		case eDUT3D:
			strUNETType = "3D";
			break;
		case eDUT2D:
		default:
			strUNETType = "2D";
			break;
		}

		switch (pPredictedInfo->stPredictOpt.contrastType)
		{
		case eDCTNoneContrast:
			strContrastType = "None Contrast";
			break;
		case eDCTContrast:
		default:
			strContrastType = "Contrast";
			break;
		}
	}

	QString strSingleSliceType = "";
	switch (pPredictedInfo->stPredictOpt.singleSliceType)
	{
	case eDSliceSelectAuto:
		strSingleSliceType = "Auto";
		break;
	case eDSliceSelectUserDefine:
	default:
		strSingleSliceType = "User define";
		break;
	}

	QString strMultiSliceType = "";
	switch (pPredictedInfo->stPredictOpt.multiSliceType)
	{
	case eDSliceSelectAuto:
		strMultiSliceType = "Auto";
		break;
	case eDSliceSelectUserDefine:
	default:
		strMultiSliceType = "User define";
		break;
	}

	QString strGendersType = "";
	switch (pPredictedInfo->stPredictOpt.genderType)
	{
	case eDGTUnknown:
		strGendersType = "Unknown";
		break;
	case eDGTFemale:
		strGendersType = "Female";
		break;
	case eDGTMale:
		strGendersType = "Male";
		break;
	}

	QString strBMI = "";
	if (!pReport->expertReportInfo.bInvalidBMI)
		strBMI = QString::number(pReport->expertReportInfo.BMI);
	QString strReportPreferenceType = "";
	switch (pPredictedInfo->stPredictOpt.preferenceType)
	{
	case eDPTSingleSlice:
		strReportPreferenceType = "L3";
		break;
	case eDPTMultiSlice:
		strReportPreferenceType = "Abdominal waist";
		break;
	}

	VOLUME_DATA& volumeData = m_pDataContext->volume_data;

	// data insert.
	QString strNA = "N/A";
	QStringList dataList;
	if (m_pActionManager->m_IsMacroMode)
		dataList << QString::number(m_pActionManager->m_ListMacroCommandLine.first().rowNum);										// numbering
	else
		dataList << numberLine;									// numbering
	dataList << curOpenedFileName;								// file name

	dataList << strSelectedDicomSeriesDescription;				// DICOM Series Description
	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2))
	{
		dataList << strContrastType;								// contrast
		dataList << strCompositionType;								// 0: Wholebody & Abdomen, 1:Chest
		dataList << strVBNetworkType;								// 0: use Vertebra Network, 1: nothing
		dataList << strIONetWorkType;								// 0: use IO Network, 1: nothing
	}
	else
	{
		dataList << strUNETType;									// 2D or 3D
		dataList << strContrastType;								// contrast
	}

	dataList << strSingleSliceType;								// single slice type
	dataList << strMultiSliceType;								// multi slice type
	dataList << strGendersType;									// gender
	dataList << pPredictedInfo->stPredictOpt.strHeight;			// height
	dataList << pPredictedInfo->stPredictOpt.strWeight;			// weight
	dataList << strBMI;											// BMI
	dataList << strReportPreferenceType;						// preference for reporting
	dataList << pPredictedInfo->stPredictOpt.strReportingDate;	// reporting date.
	dataList << QString::number(volumeData.getCX());			// x size.
	dataList << QString::number(volumeData.getCY());			// y size.
	dataList << QString::number(volumeData.getCZ());			// z size.
	dataList << QString::number(volumeData.getSpaceX(true));	// space x.
	dataList << QString::number(volumeData.getSpaceY(true));	// space y.
	dataList << QString::number(volumeData.getSpaceZ(true));	// space z.
	dataList << QString::number(volumeData.getSpace2D(true));	// space 2d.
	dataList << QString::number(volumeData.getSpace3D(true));	// space 3d.

	if (dicomFileInfo)
	{
		QString strTemp = tr("");
		if (!dicomFileInfo->convolutionkernel.empty())
		{
			strTemp = QString::fromStdString(dicomFileInfo->convolutionkernel);
			dataList << strTemp;										// Kernel
		}
		else
			dataList << strNA;
		if (!dicomFileInfo->kvp_.empty())
		{
			strTemp = QString::fromStdString(dicomFileInfo->kvp_);
			dataList << strTemp;										// Kvp
		}
		else
			dataList << strNA;
		if (!dicomFileInfo->ma_.empty())
		{
			strTemp = QString::fromStdString(dicomFileInfo->ma_);
			dataList << strTemp;										// mAs
		}
		else
			dataList << strNA;
	}
	else
	{
		dataList << strNA;
		dataList << strNA;
		dataList << strNA;
	}

	// Mask volume in the entire CT images
	QVector<QPair<int, double>> maskVolumeInEntireCTImage;
	double fTempVolumeSize = 0.0f;
	for (int i = 0; i < eDWMMaxSize; i++)
	{
		if (pReport->expertReportInfo.mask_Uid[i] != -1)
		{
			fTempVolumeSize = volumeData.getVoxelCount(pReport->expertReportInfo.mask_Uid[i]) * volumeData.getSpace3D(true);
			dataList << QString::number(fTempVolumeSize, 'f');
			maskVolumeInEntireCTImage.push_back(QPair<int, double>(i, fTempVolumeSize));
		}
		else
			dataList << strNA;
	}

	// Anatomic location of abdominal waist
	int startAxialDepth = pPredictedInfo->nStartAxialDepth;
	int endAxialDepth = pPredictedInfo->nEndAxialDepth;
	if (pPredictedInfo->nEndAxialDepth < pPredictedInfo->nStartAxialDepth)
	{
		endAxialDepth = pPredictedInfo->nStartAxialDepth;
		startAxialDepth = pPredictedInfo->nEndAxialDepth;
	}

	dataList << QString::number(startAxialDepth + 1);		// start pos slice number
	dataList << QString::number(endAxialDepth + 1);		// end pos slice number
	int abdominaWaistLength = qAbs(endAxialDepth - startAxialDepth) + 1;
	dataList << QString::number(abdominaWaistLength);
	dataList << QString::number(abdominaWaistLength * volumeData.getSpaceZ(true));

	// Anatomic location of L3 slice
	dataList << QString::number(pPredictedInfo->nAxialDepth + 1);

	QVector<double> maskVolumeInAbdominalWaist;
	QMap<eDeepCatchWholebodyClasses, int>& mapAbdominalWaistVoxelCount = pReport->expertReportInfo.mapAbdominalWaistVoxelCount;
	int mapSize = mapAbdominalWaistVoxelCount.size();
	if (!mapAbdominalWaistVoxelCount.isEmpty())
	{
		int sumVoxelCountSkin = 0;
		int sumVoxelCountBone = 0;
		int sumVoxelCountMiscle = 0;
		int sumVoxelCountAVF = 0;
		int sumVoxelCountOf = 0;
		int sumVoxelCountIo = 0;
		int sumVoxelCountBsc = 0;
		QMap<eDeepCatchWholebodyClasses, int>::const_iterator iterMap;
		iterMap = mapAbdominalWaistVoxelCount.find(eDWMSkin);
		if (iterMap != mapAbdominalWaistVoxelCount.end())
			sumVoxelCountSkin = iterMap.value();
		iterMap = mapAbdominalWaistVoxelCount.find(eDWMBone);
		if (iterMap != mapAbdominalWaistVoxelCount.end())
			sumVoxelCountBone = iterMap.value();
		iterMap = mapAbdominalWaistVoxelCount.find(eDWMMuscle);
		if (iterMap != mapAbdominalWaistVoxelCount.end())
			sumVoxelCountMiscle = iterMap.value();
		iterMap = mapAbdominalWaistVoxelCount.find(eDWMAVF);
		if (iterMap != mapAbdominalWaistVoxelCount.end())
			sumVoxelCountAVF = iterMap.value();
		iterMap = mapAbdominalWaistVoxelCount.find(eDWMOF);
		if (iterMap != mapAbdominalWaistVoxelCount.end())
			sumVoxelCountOf = iterMap.value();
		iterMap = mapAbdominalWaistVoxelCount.find(eDWMIO);
		if (iterMap != mapAbdominalWaistVoxelCount.end())
			sumVoxelCountIo = iterMap.value();
		iterMap = mapAbdominalWaistVoxelCount.find(eDWMBSC);
		if (iterMap != mapAbdominalWaistVoxelCount.end())
			sumVoxelCountBsc = iterMap.value();

		// Mask volume in the abdominal waist
		dataList << QString::number(sumVoxelCountSkin * volumeData.getSpace3D(true), 'f');		// skin
		dataList << QString::number(sumVoxelCountBone * volumeData.getSpace3D(true), 'f');		// bone
		dataList << QString::number(sumVoxelCountMiscle * volumeData.getSpace3D(true), 'f');		// muscle
		dataList << QString::number(sumVoxelCountAVF * volumeData.getSpace3D(true), 'f');			// avf
		dataList << QString::number(sumVoxelCountOf * volumeData.getSpace3D(true), 'f');			// of
		dataList << QString::number(sumVoxelCountIo * volumeData.getSpace3D(true), 'f');			// io
		dataList << QString::number(sumVoxelCountBsc * volumeData.getSpace3D(true), 'f');			// bsc

		maskVolumeInAbdominalWaist.push_back(sumVoxelCountSkin * volumeData.getSpace3D(true));
		maskVolumeInAbdominalWaist.push_back(sumVoxelCountBone * volumeData.getSpace3D(true));
		maskVolumeInAbdominalWaist.push_back(sumVoxelCountMiscle * volumeData.getSpace3D(true));
		maskVolumeInAbdominalWaist.push_back(sumVoxelCountAVF * volumeData.getSpace3D(true));
		maskVolumeInAbdominalWaist.push_back(sumVoxelCountOf * volumeData.getSpace3D(true));
		maskVolumeInAbdominalWaist.push_back(sumVoxelCountIo * volumeData.getSpace3D(true));
		maskVolumeInAbdominalWaist.push_back(sumVoxelCountBsc * volumeData.getSpace3D(true));

		// Average mask area in the abdominal waist(AJ ~ AP / AH)
#if 0
		dataList << QString::number((sumVoxelCountSkin / mapSize) * volumeData.getSpace2D(true), 'f');		// skin
		dataList << QString::number((sumVoxelCountBone / mapSize) * volumeData.getSpace2D(true), 'f');		// bone
		dataList << QString::number((sumVoxelCountMiscle / mapSize) * volumeData.getSpace2D(true), 'f');	// muscle
		dataList << QString::number((sumVoxelCountAVF / mapSize) * volumeData.getSpace2D(true), 'f');		// avf
		dataList << QString::number((sumVoxelCountOf / mapSize) * volumeData.getSpace2D(true), 'f');		// of
		dataList << QString::number((sumVoxelCountIo / mapSize) * volumeData.getSpace2D(true), 'f');		// io
		dataList << QString::number((sumVoxelCountBsc / mapSize) * volumeData.getSpace2D(true), 'f');		// bsc
#else
		float zlength = abdominaWaistLength * volumeData.getSpaceZ(true);
		dataList << QString::number((sumVoxelCountSkin * volumeData.getSpace3D(true)) / zlength, 'f');
		dataList << QString::number((sumVoxelCountBone * volumeData.getSpace3D(true)) / zlength, 'f');
		dataList << QString::number((sumVoxelCountMiscle * volumeData.getSpace3D(true)) / zlength, 'f');
		dataList << QString::number((sumVoxelCountAVF * volumeData.getSpace3D(true)) / zlength, 'f');
		dataList << QString::number((sumVoxelCountOf * volumeData.getSpace3D(true)) / zlength, 'f');
		dataList << QString::number((sumVoxelCountIo * volumeData.getSpace3D(true)) / zlength, 'f');
		dataList << QString::number((sumVoxelCountBsc * volumeData.getSpace3D(true)) / zlength, 'f');
#endif

	}
	else
	{
		// Mask volume in the abdominal waist
		dataList << strNA;
		dataList << strNA;
		dataList << strNA;
		dataList << strNA;
		dataList << strNA;
		dataList << strNA;
		dataList << strNA;

		// Average mask area in the abdominal waist(AJ ~ AP / AH)
		dataList << strNA;
		dataList << strNA;
		dataList << strNA;
		dataList << strNA;
		dataList << strNA;
		dataList << strNA;
		dataList << strNA;
	}

	QVector<double> maskVolumeInL3;
	QMap<eDeepCatchWholebodyClasses, int>& mapL3VoxelCount = pReport->expertReportInfo.mapL3VoxelCount;
	if (!mapL3VoxelCount.isEmpty())
	{
		int voxelCountSkin = 0;
		int voxelCountBone = 0;
		int voxelCountMiscle = 0;
		int voxelCountAVF = 0;
		int voxelCountOf = 0;
		int voxelCountIo = 0;
		int voxelCountBsc = 0;
		QMap<eDeepCatchWholebodyClasses, int>::const_iterator iterMap;
		iterMap = mapL3VoxelCount.find(eDWMSkin);
		if (iterMap != mapL3VoxelCount.end())
			voxelCountSkin = iterMap.value();
		iterMap = mapL3VoxelCount.find(eDWMBone);
		if (iterMap != mapL3VoxelCount.end())
			voxelCountBone = iterMap.value();
		iterMap = mapL3VoxelCount.find(eDWMMuscle);
		if (iterMap != mapL3VoxelCount.end())
			voxelCountMiscle = iterMap.value();
		iterMap = mapL3VoxelCount.find(eDWMAVF);
		if (iterMap != mapL3VoxelCount.end())
			voxelCountAVF = iterMap.value();
		iterMap = mapL3VoxelCount.find(eDWMOF);
		if (iterMap != mapL3VoxelCount.end())
			voxelCountOf = iterMap.value();
		iterMap = mapL3VoxelCount.find(eDWMIO);
		if (iterMap != mapL3VoxelCount.end())
			voxelCountIo = iterMap.value();
		iterMap = mapL3VoxelCount.find(eDWMBSC);
		if (iterMap != mapL3VoxelCount.end())
			voxelCountBsc = iterMap.value();

		// Mask area in the L3 single slice
		dataList << QString::number(voxelCountSkin * volumeData.getSpace2D(true), 'f');		// skin
		dataList << QString::number(voxelCountBone * volumeData.getSpace2D(true), 'f');		// bone
		dataList << QString::number(voxelCountMiscle * volumeData.getSpace2D(true), 'f');		// muscle
		dataList << QString::number(voxelCountAVF * volumeData.getSpace2D(true), 'f');		// avf
		dataList << QString::number(voxelCountOf * volumeData.getSpace2D(true), 'f');			// of
		dataList << QString::number(voxelCountIo * volumeData.getSpace2D(true), 'f');			// io
		dataList << QString::number(voxelCountBsc * volumeData.getSpace2D(true), 'f');		// bsc

		// Mask mass(g) in the L3
		maskVolumeInL3.push_back(voxelCountSkin * volumeData.getSpace3D(true));
		maskVolumeInL3.push_back(voxelCountBone * volumeData.getSpace3D(true));
		maskVolumeInL3.push_back(voxelCountMiscle * volumeData.getSpace3D(true));
		maskVolumeInL3.push_back(voxelCountAVF * volumeData.getSpace3D(true));
		maskVolumeInL3.push_back(voxelCountOf * volumeData.getSpace3D(true));
		maskVolumeInL3.push_back(voxelCountIo * volumeData.getSpace3D(true));
		maskVolumeInL3.push_back(voxelCountBsc * volumeData.getSpace3D(true));
	}
	else
	{
		// Mask area in the L3 single slice
		dataList << strNA;
		dataList << strNA;
		dataList << strNA;
		dataList << strNA;
		dataList << strNA;
		dataList << strNA;
		dataList << strNA;
	}

	QMap<eDeepCatchWholebodyClasses, float>& mapAbdominalWaistMeanHU = pReport->expertReportInfo.mapAbdominalWaistMeanHU;
	if (!mapAbdominalWaistMeanHU.isEmpty())
	{
		float meanHuSkin = 0;
		float meanHuBone = 0;
		float meanHuMiscle = 0;
		float meanHuAVF = 0;
		float meanHuOf = 0;
		float meanHuIo = 0;
		float meanHuBsc = 0;
		QMap<eDeepCatchWholebodyClasses, float>::const_iterator iterMap;
		iterMap = mapAbdominalWaistMeanHU.find(eDWMSkin);
		if (iterMap != mapAbdominalWaistMeanHU.end())
			meanHuSkin = iterMap.value();
		iterMap = mapAbdominalWaistMeanHU.find(eDWMBone);
		if (iterMap != mapAbdominalWaistMeanHU.end())
			meanHuBone = iterMap.value();
		iterMap = mapAbdominalWaistMeanHU.find(eDWMMuscle);
		if (iterMap != mapAbdominalWaistMeanHU.end())
			meanHuMiscle = iterMap.value();
		iterMap = mapAbdominalWaistMeanHU.find(eDWMAVF);
		if (iterMap != mapAbdominalWaistMeanHU.end())
			meanHuAVF = iterMap.value();
		iterMap = mapAbdominalWaistMeanHU.find(eDWMOF);
		if (iterMap != mapAbdominalWaistMeanHU.end())
			meanHuOf = iterMap.value();
		iterMap = mapAbdominalWaistMeanHU.find(eDWMIO);
		if (iterMap != mapAbdominalWaistMeanHU.end())
			meanHuIo = iterMap.value();
		iterMap = mapAbdominalWaistMeanHU.find(eDWMBSC);
		if (iterMap != mapAbdominalWaistMeanHU.end())
			meanHuBsc = iterMap.value();

		// Mean mask CT attenuation in the abdominal waist
		if (meanHuSkin)
			dataList << QString::number(meanHuSkin);		// skin
		else
			dataList << strNA;
		if (meanHuBone)
			dataList << QString::number(meanHuBone);		// bone
		else
			dataList << strNA;
		if (meanHuMiscle)
			dataList << QString::number(meanHuMiscle);		// muscle
		else
			dataList << strNA;
		if (meanHuAVF)
			dataList << QString::number(meanHuAVF);			// avf
		else
			dataList << strNA;
		if (meanHuOf)
			dataList << QString::number(meanHuOf);			// of
		else
			dataList << strNA;
		if (meanHuIo)
			dataList << QString::number(meanHuIo);			// io
		else
			dataList << strNA;
		if (meanHuBsc)
			dataList << QString::number(meanHuBsc);			// bsc
		else
			dataList << strNA;

	}
	else
	{
		// Mean mask CT attenuation in the abdominal waist
		dataList << strNA;
		dataList << strNA;
		dataList << strNA;
		dataList << strNA;
		dataList << strNA;
		dataList << strNA;
		dataList << strNA;

	}

	QMap<eDeepCatchWholebodyClasses, float>& mapL3MeanHU = pReport->expertReportInfo.mapL3MeanHU;
	if (!mapL3MeanHU.isEmpty())
	{
		float meanHuSkin = 0;
		float meanHuBone = 0;
		float meanHuMiscle = 0;
		float meanHuAVF = 0;
		float meanHuOf = 0;
		float meanHuIo = 0;
		float meanHuBsc = 0;
		QMap<eDeepCatchWholebodyClasses, float>::const_iterator iterMap;
		iterMap = mapL3MeanHU.find(eDWMSkin);
		if (iterMap != mapL3MeanHU.end())
			meanHuSkin = iterMap.value();
		iterMap = mapL3MeanHU.find(eDWMBone);
		if (iterMap != mapL3MeanHU.end())
			meanHuBone = iterMap.value();
		iterMap = mapL3MeanHU.find(eDWMMuscle);
		if (iterMap != mapL3MeanHU.end())
			meanHuMiscle = iterMap.value();
		iterMap = mapL3MeanHU.find(eDWMAVF);
		if (iterMap != mapL3MeanHU.end())
			meanHuAVF = iterMap.value();
		iterMap = mapL3MeanHU.find(eDWMOF);
		if (iterMap != mapL3MeanHU.end())
			meanHuOf = iterMap.value();
		iterMap = mapL3MeanHU.find(eDWMIO);
		if (iterMap != mapL3MeanHU.end())
			meanHuIo = iterMap.value();
		iterMap = mapL3MeanHU.find(eDWMBSC);
		if (iterMap != mapL3MeanHU.end())
			meanHuBsc = iterMap.value();

		// Mean mask CT attenuation at the L3 single slice
		if (meanHuSkin)
			dataList << QString::number(meanHuSkin);		// skin
		else
			dataList << strNA;
		if (meanHuBone)
			dataList << QString::number(meanHuBone);		// bone
		else
			dataList << strNA;
		if (meanHuMiscle)
			dataList << QString::number(meanHuMiscle);		// muscle
		else
			dataList << strNA;
		if (meanHuAVF)
			dataList << QString::number(meanHuAVF);			// avf
		else
			dataList << strNA;
		if (meanHuOf)
			dataList << QString::number(meanHuOf);			// of
		else
			dataList << strNA;
		if (meanHuIo)
			dataList << QString::number(meanHuIo);			// io
		else
			dataList << strNA;
		if (meanHuBsc)
			dataList << QString::number(meanHuBsc);			// bsc
		else
			dataList << strNA;
	}
	else
	{
		// Mean mask CT attenuation at the L3 single slice
		dataList << strNA;
		dataList << strNA;
		dataList << strNA;
		dataList << strNA;
		dataList << strNA;
		dataList << strNA;
		dataList << strNA;
	}

	// abdominal circumference (mm)
	if (pReport->fAbdomialCircumference != -1.0f && pReport->fAbdomialCircumference != 0.0f)
		dataList << QString::number(pReport->fAbdomialCircumference * 10.0f);
	else
		dataList << strNA;

	MaskInfo* pCircumferenceMask = nullptr;
	pCircumferenceMask = m_pDataContext->volume_data.findMaskInfo(DEEPCATCH_WHOLEBODY_MASKNAME_AC);
	if (pCircumferenceMask)
	{
		BoundingBoxI circumferenceBoundingBox = m_pDataContext->volume_data.getBoundingBox(pCircumferenceMask->uid);
		// abdominal circumference cx (mm)
		dataList << QString::number((circumferenceBoundingBox.maxX - circumferenceBoundingBox.minX) * volumeData.getSpaceX(true));

		// abdominal circumference cy (mm)
		dataList << QString::number((circumferenceBoundingBox.maxY - circumferenceBoundingBox.minY) * volumeData.getSpaceY(true));
	}
	else
	{
		dataList << strNA;
		dataList << strNA;
	}

	// Mask mass(g) in the entire CT images
	QString strHeight = pPredictedInfo->stPredictOpt.strHeight;
	float fHeight = 0.0f;
	bool bRet = false;
	fHeight = strHeight.toFloat(&bRet);
	if (bRet && (fHeight != 0.0f))
	{
		fHeight /= 100;
		fHeight *= fHeight;
		int count = 0;
		for (int i = 0; i < maskVolumeInEntireCTImage.size(); i++)
		{
			if (maskVolumeInEntireCTImage.at(i).first == eDWMMuscle)
			{
				count++;
				dataList << QString::number(((maskVolumeInEntireCTImage.at(i).second / 1000) * 1.06) / fHeight, 'f');
			}
			else if (maskVolumeInEntireCTImage.at(i).first == eDWMAVF)
			{
				count++;
				dataList << QString::number(((maskVolumeInEntireCTImage.at(i).second / 1000) * 0.92) / fHeight, 'f');
			}
			else if (maskVolumeInEntireCTImage.at(i).first == eDWMOF)
			{
				count++;
				dataList << QString::number(((maskVolumeInEntireCTImage.at(i).second / 1000) * 0.92) / fHeight, 'f');
			}
		}

		if (!count)
		{
			dataList << strNA;
			dataList << strNA;
			dataList << strNA;
		}
		else if (count == 1)
		{
			dataList << strNA;
			dataList << strNA;
		}
		else if (count == 2)
			dataList << strNA;

		// Mask mass(g) in the abdominal waist
		if (!maskVolumeInAbdominalWaist.isEmpty())
		{
			dataList << QString::number(((maskVolumeInAbdominalWaist.at(eDWMMuscle) / 1000) * 1.06) / fHeight, 'f');
			dataList << QString::number(((maskVolumeInAbdominalWaist.at(eDWMAVF) / 1000) * 0.92) / fHeight, 'f');
			dataList << QString::number(((maskVolumeInAbdominalWaist.at(eDWMOF) / 1000) * 0.92) / fHeight, 'f');
		}
		else
		{
			dataList << strNA;
			dataList << strNA;
			dataList << strNA;
		}

		// Mask mass(g) in the L3
		if (!maskVolumeInL3.isEmpty())
		{
			dataList << QString::number(((maskVolumeInL3.at(eDWMMuscle) / 1000) * 1.06) / fHeight, 'f');
			dataList << QString::number(((maskVolumeInL3.at(eDWMAVF) / 1000) * 0.92) / fHeight, 'f');
			dataList << QString::number(((maskVolumeInL3.at(eDWMOF) / 1000) * 0.92) / fHeight, 'f');
		}
		else
		{
			dataList << strNA;
			dataList << strNA;
			dataList << strNA;
		}
	}
	else
	{
		dataList << strNA;
		dataList << strNA;
		dataList << strNA;
		dataList << strNA;
		dataList << strNA;
		dataList << strNA;
		dataList << strNA;
		dataList << strNA;
		dataList << strNA;
	}

	// Muscle volume by interval in the entire CT images
	double fTempMuslceVolumeSize = 0.0f;
	for (int i = 0; i < eDMQMCMaxSize; i++)
	{
		if (pReport->expertReportInfo.mask_Uid_muscleQualityMap[i] != -1)
		{
			fTempMuslceVolumeSize = volumeData.getVoxelCount(pReport->expertReportInfo.mask_Uid_muscleQualityMap[i]) * volumeData.getSpace3D(true);
			dataList << QString::number(fTempMuslceVolumeSize, 'f');
		}
		else
			dataList << strNA;
	}

	// Muscle volume by interval in the abdominal waist
	QMap<eDeepCatchMuscleQualityMapClasses, int>& mapAbdominalWaistVoxelCount_muscleQualityMap = pReport->expertReportInfo.mapAbdominalWaistVoxelCount_muscleQualityMap;
	if (!mapAbdominalWaistVoxelCount_muscleQualityMap.isEmpty())
	{
		int sumVoxelCountMuscleAdpTissue = 0;
		int sumVoxelCountMuscleAbnormal1 = 0;
		int sumVoxelCountMuscleAbnormal2 = 0;
		int sumVoxelCountMuscleNormal = 0;
		QMap<eDeepCatchMuscleQualityMapClasses, int>::const_iterator iterMap;
		iterMap = mapAbdominalWaistVoxelCount_muscleQualityMap.find(eDMQMCMuscleAdpTissue);
		if (iterMap != mapAbdominalWaistVoxelCount_muscleQualityMap.end())
			sumVoxelCountMuscleAdpTissue = iterMap.value();
		iterMap = mapAbdominalWaistVoxelCount_muscleQualityMap.find(eDMQMCMuscleAbnormal1);
		if (iterMap != mapAbdominalWaistVoxelCount_muscleQualityMap.end())
			sumVoxelCountMuscleAbnormal1 = iterMap.value();
		iterMap = mapAbdominalWaistVoxelCount_muscleQualityMap.find(eDMQMCMuscleAbnormal2);
		if (iterMap != mapAbdominalWaistVoxelCount_muscleQualityMap.end())
			sumVoxelCountMuscleAbnormal2 = iterMap.value();
		iterMap = mapAbdominalWaistVoxelCount_muscleQualityMap.find(eDMQMCMuscleNormal);
		if (iterMap != mapAbdominalWaistVoxelCount_muscleQualityMap.end())
			sumVoxelCountMuscleNormal = iterMap.value();

		// Mask volume in the abdominal waist
		if (pReport->expertReportInfo.mask_Uid_muscleQualityMap[eDMQMCMuscleAdpTissue] != -1)
			dataList << QString::number(sumVoxelCountMuscleAdpTissue * volumeData.getSpace3D(true), 'f');
		else
			dataList << strNA;
		if (pReport->expertReportInfo.mask_Uid_muscleQualityMap[eDMQMCMuscleAbnormal1] != -1)
			dataList << QString::number(sumVoxelCountMuscleAbnormal1 * volumeData.getSpace3D(true), 'f');
		else
			dataList << strNA;
		if (pReport->expertReportInfo.mask_Uid_muscleQualityMap[eDMQMCMuscleAbnormal2] != -1)
			dataList << QString::number(sumVoxelCountMuscleAbnormal2 * volumeData.getSpace3D(true), 'f');
		else
			dataList << strNA;
		if (pReport->expertReportInfo.mask_Uid_muscleQualityMap[eDMQMCMuscleNormal] != -1)
			dataList << QString::number(sumVoxelCountMuscleNormal * volumeData.getSpace3D(true), 'f');
		else
			dataList << strNA;
	}
	else
	{
		dataList << strNA;
		dataList << strNA;
		dataList << strNA;
		dataList << strNA;
	}

	// Muscle area by interval in the L3 single slice
	QMap<eDeepCatchMuscleQualityMapClasses, int>& mapL3VoxelCount_muscleQualityMap = pReport->expertReportInfo.mapL3VoxelCount_muscleQualityMap;
	if (!mapL3VoxelCount_muscleQualityMap.isEmpty())
	{
		int voxelCountMuscleAdpTissue = 0;
		int voxelCountMuscleAbnormal1 = 0;
		int voxelCountMuscleAbnormal2 = 0;
		int voxelCountMuscleNormal = 0;
		QMap<eDeepCatchMuscleQualityMapClasses, int>::const_iterator iterMap;
		iterMap = mapL3VoxelCount_muscleQualityMap.find(eDMQMCMuscleAdpTissue);
		if (iterMap != mapL3VoxelCount_muscleQualityMap.end())
			voxelCountMuscleAdpTissue = iterMap.value();
		iterMap = mapL3VoxelCount_muscleQualityMap.find(eDMQMCMuscleAbnormal1);
		if (iterMap != mapL3VoxelCount_muscleQualityMap.end())
			voxelCountMuscleAbnormal1 = iterMap.value();
		iterMap = mapL3VoxelCount_muscleQualityMap.find(eDMQMCMuscleAbnormal2);
		if (iterMap != mapL3VoxelCount_muscleQualityMap.end())
			voxelCountMuscleAbnormal2 = iterMap.value();
		iterMap = mapL3VoxelCount_muscleQualityMap.find(eDMQMCMuscleNormal);
		if (iterMap != mapL3VoxelCount_muscleQualityMap.end())
			voxelCountMuscleNormal = iterMap.value();

		// Mask area in the L3 single slice
		if (pReport->expertReportInfo.mask_Uid_muscleQualityMap[eDMQMCMuscleAdpTissue] != -1)
			dataList << QString::number(voxelCountMuscleAdpTissue * volumeData.getSpace2D(true), 'f');
		else
			dataList << strNA;
		if (pReport->expertReportInfo.mask_Uid_muscleQualityMap[eDMQMCMuscleAbnormal1] != -1)
			dataList << QString::number(voxelCountMuscleAbnormal1 * volumeData.getSpace2D(true), 'f');
		else
			dataList << strNA;
		if (pReport->expertReportInfo.mask_Uid_muscleQualityMap[eDMQMCMuscleAbnormal2] != -1)
			dataList << QString::number(voxelCountMuscleAbnormal2 * volumeData.getSpace2D(true), 'f');
		else
			dataList << strNA;
		if (pReport->expertReportInfo.mask_Uid_muscleQualityMap[eDMQMCMuscleNormal] != -1)
			dataList << QString::number(voxelCountMuscleNormal * volumeData.getSpace2D(true), 'f');
		else
			dataList << strNA;

		dataList << strNA;
	}
	else
	{
		dataList << strNA;
		dataList << strNA;
		dataList << strNA;
		dataList << strNA;
		dataList << strNA;
	}


	// Volunme for Bone Analysis "L1" ~ "T12" ---------------------------------------------
	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2))
	{

		for (int ii = 0; ii < eDVertebraSize; ii++)
		{
			float VoxelCnt = 0.0f;
			float value = 0.0f;
			if (!pReport->expertReportInfo.mapVertebraVoxelCount.isEmpty())
			{
				VoxelCnt = getDeepCatchReportValue<QMap<eDeepCatchVertebraMapClasses, int>, eDeepCatchVertebraMapClasses>(pReport->expertReportInfo.mapVertebraVoxelCount, (eDeepCatchVertebraMapClasses)ii);
			}
			else
			{
				dataList << strNA;
				continue;
			}
			value = VoxelCnt * volumeData.getSpace3D(true);
			dataList << QString::number(value);		// Volunme for Bone Analysis "L1" ~ "T12"
		}


		//  Mean HU for Bone Analysis "L1" ~ "T12" ---------------------------------------------

		for (int ii = 0; ii < eDVertebraSize; ii++)
		{
			float value = 0.0f;
			if (!pReport->expertReportInfo.mapVertebraMeanHU.isEmpty())
			{
				value = getDeepCatchReportValue<QMap<eDeepCatchVertebraMapClasses, float>, eDeepCatchVertebraMapClasses>(pReport->expertReportInfo.mapVertebraMeanHU, (eDeepCatchVertebraMapClasses)ii);
			}
			else
			{
				dataList << strNA;
				continue;
			}
			dataList << QString::number(value);		// Mean HU for Bone Analysis "L1" ~ "T12"
		}

		//  T-score for Bone Analysis ---------------------------------------------

		QMap<eDeepCatchVertebraMapClasses, float>& mapVBTScore = pReport->expertReportInfo.mapVertebraTScore;
		if (!mapVBTScore.isEmpty())
		{
			float vertebra_L1_T_Score = 0;
			QMap<eDeepCatchVertebraMapClasses, float>::const_iterator iterMap;
			iterMap = mapVBTScore.find(eDVertebra_L1);
			if (iterMap != mapVBTScore.end())
				vertebra_L1_T_Score = iterMap.value();

			if (vertebra_L1_T_Score)
				dataList << QString::number(vertebra_L1_T_Score);		// Vertebra area L1 T-Score
			else
				dataList << strNA;
		}
		else
			dataList << strNA;		// Vertebra area L1 T-Score


		if (!pReport->expertReportInfo.mapIOClassVoxelCount.isEmpty())
		{
			float value = getDeepCatchReportValue<QMap<eDeepCatchIOClassMapClasses, int>, eDeepCatchIOClassMapClasses>
				(pReport->expertReportInfo.mapIOClassVoxelCount, eDIOClass_Liver);

			if (value)
				dataList << QString::number(value);		//Mask volunme (liver)
			else
				dataList << strNA;

			dataList << strNA; // HU in mask volume (liver)

			value = getDeepCatchReportValue<QMap<eDeepCatchIOClassMapClasses, float>, eDeepCatchIOClassMapClasses>
				(pReport->expertReportInfo.mapIOClassMeanHU, eDIOClass_Liver);

			if (value)
				dataList << QString::number(value);		//HU in mask volume(liver)
			else
				dataList << strNA;

			//
			value = getDeepCatchReportValue<QMap<eDeepCatchIOClassMapClasses, int>, eDeepCatchIOClassMapClasses>
				(pReport->expertReportInfo.mapIOClassVoxelCount, eDIOClass_Spleen);

			if (value)
				dataList << QString::number(value);		//Mask volunme (spleen)
			else
				dataList << strNA;

			dataList << strNA; // HU in mask volume (spleen)

			value = getDeepCatchReportValue<QMap<eDeepCatchIOClassMapClasses, float>, eDeepCatchIOClassMapClasses>
				(pReport->expertReportInfo.mapIOClassMeanHU, eDIOClass_Spleen);

			if (value)
				dataList << QString::number(value);		//mean HU in mask volume(spleen)
			else
				dataList << strNA;
		}
		else
		{
			dataList << strNA; // Mask volunme(liver)
			dataList << strNA; // HU in mask volume (liver)
			dataList << strNA; // mean HU in mask volume (liver)
			dataList << strNA; // Mask volunme (spleen)
			dataList << strNA; // HU in mask volume (spleen)
			dataList << strNA; // mean HU in mask volume (spleen)
		}

		dataList << strNA; // Mask area (liver)  in the single slice
		dataList << strNA; // Mask area (spleen)  in the single slice
		dataList << strNA; // Aorta Calcification
	}

	// csv 형식 만들어 write.
	QString curString = "";
	for (int i = 0; i < dataList.size(); i++)
	{
		// ,들어간 문자가 있는지 검사. 문자열에 ,가 이미 들어가 있으면 _로 변경.
		curString = dataList.at(i);
		curString.replace(',', '_');

		copyStr += curString;
		if (i == (dataList.size() - 1))		// 마지막에는 개행 문자.
			copyStr += "\r\n";
		else
			copyStr += ",";
	}

	if (!newFile.open(QIODevice::WriteOnly | QIODevice::Append))
	{
		QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DU_1013)).exec();
		return;
	}

	QTextStream textStream(&newFile);
	textStream << copyStr;
	newFile.close();

	// 완료 후 expertReport.csv 파일 오픈.
	//	QDesktopServices::openUrl(QUrl(filePath));
				}

template<typename T, typename ENUM>
inline float MedipQT::getDeepCatchReportValue(T temp, ENUM _enum)
{
	auto& _map = temp;
	if (!_map.isEmpty())
	{
		float value = 0;
		T::const_iterator iterMap;
		iterMap = _map.find(_enum);
		if (iterMap != _map.end())
			value = iterMap.value();

		if (value)
			return value;
	}
	return 0.0f;
}

void MedipQT::reportVolumeViewCapture()
{
	DEEPCATCH_REPORT* pReport = m_pActionManager->getDeepCatchAnalysisVals();
	switch (m_nReportVolumeVeiwCapturesteps)
	{
	case 0:
	{
		//	QString title = QString("DeepCatch Volume View Capture") + QString(" to report.");
		//	m_pActionManager->action_ProgressBegin(title);

		DEEPCATCH_REPORT_PREDICT_INFO* predictInfo = m_pActionManager->getDeepCatch_PredictedInfo();
		// muscle classification이면 muscle classification 정보 1번 더 캡쳐.
		if (predictInfo->stPredictOpt.bMuscleQualityMap)
			pReport->tempInfo.reportVolumeVeiwCaptureLastStepCount = 9;
		else
			pReport->tempInfo.reportVolumeVeiwCaptureLastStepCount = 7;

		// 캡쳐전 마스크 상태 저장.
		pReport->tempInfo.tempMaskShowInfoVec.clear();

		//	ROITab2* pRoiTab = m_pWindowManager->GetTab()->getROITab();
		//	int nItemCnt = pRoiTab->ROIList->topLevelItemCount();
		int nItemCnt = m_pDataContext->volume_data.getMaskInfoListCnt();
		for (int i = 0; i < nItemCnt; ++i)
		{
			MaskInfo* info = m_pDataContext->volume_data.getMaskInfo(i);
			if (info)
				pReport->tempInfo.tempMaskShowInfoVec.push_back(info->show);
		}

		VolumeView* vView = dynamic_cast<VolumeView*>(m_pWindowManager->mainSegmentWidget->getViewVolume());
		if (vView)
		{
			// full screen 상태면 해제 후 캡쳐 진행.
			if (vView->getFullscreen())
			{
				pReport->tempInfo.bVolumeViewFullScreenState = true;
				vView->slot_OnFullScreen();
			}

			// reset ui
			vView->resetUI();
		}
		if (!pReport->tempInfo.bVolumeViewFullScreenState)
		{
			// axial, saggital, cornal view full screen이면 해제 후 캡쳐 진행.
			WindowBase* pAxialView = m_pWindowManager->mainSegmentWidget->getWindow(WT_AXIAL);
			WindowBase* pCoronalView = m_pWindowManager->mainSegmentWidget->getWindow(WT_CORONAL);
			WindowBase* pSagittalView = m_pWindowManager->mainSegmentWidget->getWindow(WT_SAGITTAL);
			if (pAxialView->isFullScreen())
			{
				pReport->tempInfo.bMPRViewFullScreenState[WT_AXIAL - WT_AXIAL] = true;
				pAxialView->slot_OnFullScreen();
			}
			else if (pCoronalView->isFullScreen())
			{
				pReport->tempInfo.bMPRViewFullScreenState[WT_CORONAL - WT_AXIAL] = true;
				pCoronalView->slot_OnFullScreen();
			}
			else if (pSagittalView->isFullScreen())
			{
				pReport->tempInfo.bMPRViewFullScreenState[WT_SAGITTAL - WT_AXIAL] = true;
				pSagittalView->slot_OnFullScreen();
			}
		}

		setHomePosition(HP_ANTERIOR, true);

		Visualize3DTab* p3DTab = m_pWindowManager->GetTab()->get3DTab();
		p3DTab->deepcatch3DTabSetting(pReport);
	}
	case 2:
	case 4:
	case 6:
	case 8:
	{
		// ROI show 셋팅	
		ROITab2* pRoiTab = m_pWindowManager->GetTab()->getROITab();
		//	int nItemCnt = pRoiTab->ROIList->topLevelItemCount();
		int nItemCnt = m_pDataContext->volume_data.getMaskInfoListCnt();
		for (int i = 0; i < nItemCnt; ++i)
		{
			MaskInfo* info = m_pDataContext->volume_data.getMaskInfo(i);
			if (info)
				info->show = false;
		}

		// 캡쳐할 마스크 이름 결정.
		QStringList curCapturedMaskNames;
		switch (m_nReportVolumeVeiwCapturesteps)
		{
		case 0:
			curCapturedMaskNames << DEEPCATCH_WHOLEBODY_MASKNAME_OF;
			break;
		case 2:
			curCapturedMaskNames << DEEPCATCH_WHOLEBODY_MASKNAME_MUSCLE;
			break;
		case 4:
			curCapturedMaskNames << DEEPCATCH_WHOLEBODY_MASKNAME_AVF;
			break;
		case 6:
		{
			curCapturedMaskNames << DEEPCATCH_WHOLEBODY_MASKNAME_BONE;
			curCapturedMaskNames << DEEPCATCH_WHOLEBODY_MASKNAME_SKIN;
		}
		break;
		case 8:
		{
			curCapturedMaskNames << DEEPCATCH_WHOLEBODY_MASKNAME_MUSCLE_ADP_TISSUE;
			curCapturedMaskNames << DEEPCATCH_WHOLEBODY_MASKNAME_MUSCLE_ABNORMAL1;
			curCapturedMaskNames << DEEPCATCH_WHOLEBODY_MASKNAME_MUSCLE_ABNORMAL2;
			curCapturedMaskNames << DEEPCATCH_WHOLEBODY_MASKNAME_MUSCLE_NORMAL;
		}
		break;
		}

		for (int i = 0; i < curCapturedMaskNames.size(); i++)
		{
#ifdef DEEPCATCH_REPORT_MASK_UID_USE
			int curUid = -1;
			DEEPCATCH_REPORT_PREDICT_INFO* pPredictedInfo = m_pActionManager->getDeepCatch_PredictedInfo();
			QMap<QString, int>& mapDeepCatchPredictResultRoi = pPredictedInfo->mapDeepCatchPredictResultRoi;
			if (!mapDeepCatchPredictResultRoi.isEmpty())
			{
				QMap<QString, int>::const_iterator iterMap = mapDeepCatchPredictResultRoi.find(curCapturedMaskNames.at(i));
				if (iterMap != mapDeepCatchPredictResultRoi.end())
					curUid = iterMap.value();
			}
			// muscle roi selection
			MaskInfo* pMaskInfo = nullptr;
			if (curUid != -1)
				pMaskInfo = m_pDataContext->volume_data.getMaskInfo(curUid, true);
#else 
			MaskInfo* pMaskInfo = m_pDataContext->volume_data.findMaskInfo(curCapturedMaskNames.at(i));
#endif
			if (pMaskInfo)
			{
				pMaskInfo->show = true;
				muint32 maskIndex = 0;
				maskIndex = m_pDataContext->volume_data.getMaskIndex(pMaskInfo->uid);
				if (maskIndex != -1)
				{
					m_pDataContext->volume_data.setCurrentMaskIndex(maskIndex);
					MaskInfo* info = m_pDataContext->volume_data.getMaskInfo(maskIndex);
					std::vector<muint32> vecSelect;
					vecSelect.push_back(info->uid);
					m_pDataContext->volume_data.setMultiSelectUID(vecSelect);
					pRoiTab->ROIList->SelectionUpdate(maskIndex);
				}
				else
					QMessageBox::warning(this, "error", "mask index is invalid.");
			}
		}

		// update
		pRoiTab->updateROIHeader(L_COL_SHOW);
		pRoiTab->ROIList->UpdateByVolumeData();
		m_pWindowManager->forceUpdate2DViewData(false, true);
		//	m_pWindowManager->renderLater_GridView(true);
		m_pWindowManager->mainSegmentWidget->renderLater_GridView(true);

		OnUpdateProgressMultiThread((m_nReportVolumeVeiwCapturesteps / 2) * 10);
		//	m_pActionManager->action_ProgressUpdate(m_nReportVolumeVeiwCapturesteps*20);
	}
	break;
	case 1:
	case 3:
	case 5:
	case 7:
	case 9:
	{
		// volume view 이미지 캡쳐 후 테이블에 이미지 삽입 테스트.
		QImage volumeViewImg;
		VolumeView* vView = dynamic_cast<VolumeView*>(m_pWindowManager->mainSegmentWidget->getViewVolume());
		if (vView)
		{
#if 0
			volumeViewImg = vView->getCaptureImage(true, false);
#else
			//	BoundingBoxI box1 = m_pDataContext->volume_data.getBoundingBox3D();
			//	mip::AABB box2 = m_pDataContext->volume_data.getBoundingBox3DAABB();
			std::vector<mip::VECTOR3> tempList;
			m_pDataContext->volume_data.getMPRPPlanes(WT_CORONAL, tempList);
			mip::VECTOR3 leftTop = vView->getLocalToScreen(tempList[0].x, tempList[0].y, tempList[0].z);
			mip::VECTOR3 rightBottom = vView->getLocalToScreen(tempList[2].x, tempList[2].y, tempList[2].z);
			//	mip::VECTOR3 b = getLocalToVolume(tempList[0].x, tempList[0].y, tempList[0].z);
			//	mip::VECTOR3 c = getScreenToLocal(tempList[0].x, tempList[0].y, tempList[0].z);
			//	m_polyLine.push_back(tempList[0].x, tempList[0].z);
			//	m_polyLine.push_back(tempList[2].x, tempList[2].z);
			QPolygon polyLine;
			polyLine.push_back(QPoint(leftTop.x, leftTop.y));
			polyLine.push_back(QPoint(rightBottom.x, rightBottom.y));
			volumeViewImg = vView->getCaptureImage(polyLine);
#endif
			//	pReport->scaledVolumeViewImg[(int)(m_nReportVolumeVeiwCapturesteps/2)] = volumeViewImg.scaled(QSize(volumeViewImg.width() / 5, volumeViewImg.height() / 5), Qt::AspectRatioMode::KeepAspectRatioByExpanding, Qt::TransformationMode::SmoothTransformation);
			switch (pReport->curA4Res)
			{
				//	eART96PPI:
				//	break;
			case eART150PPI:
			{
				//	pReport->scaledVolumeViewImg[(int)(m_nReportVolumeVeiwCapturesteps / 2)] = volumeViewImg.scaled(QSize(360, 400), Qt::AspectRatioMode::KeepAspectRatio, Qt::TransformationMode::SmoothTransformation);
				pReport->scaledVolumeViewImg[(int)(m_nReportVolumeVeiwCapturesteps / 2)] = volumeViewImg;
			}
			break;
			//	eART300PPI:
			//	break;
			case eART72PPI:
			default:
			{
				//	pReport->scaledVolumeViewImg[(int)(m_nReportVolumeVeiwCapturesteps / 2)] = volumeViewImg.scaled(QSize(174, 208), Qt::AspectRatioMode::KeepAspectRatio, Qt::TransformationMode::SmoothTransformation);
				pReport->scaledVolumeViewImg[(int)(m_nReportVolumeVeiwCapturesteps / 2)] = volumeViewImg;
			}
			break;
			}
		}

		if (m_nReportVolumeVeiwCapturesteps == pReport->tempInfo.reportVolumeVeiwCaptureLastStepCount)
		{
			// 캡쳐전 마스크 상태로 복귀.
			ROITab2* pRoiTab = m_pWindowManager->GetTab()->getROITab();
			//	int nItemCnt = pRoiTab->ROIList->topLevelItemCount();
			int nItemCnt = m_pDataContext->volume_data.getMaskInfoListCnt();
			for (int i = 0; i < nItemCnt; ++i)
			{
				MaskInfo* info = m_pDataContext->volume_data.getMaskInfo(i);
				if (info)
				{
					info->show = pReport->tempInfo.tempMaskShowInfoVec.at(i);
					if (info->show)
					{
						muint32 maskIndex = 0;
						maskIndex = m_pDataContext->volume_data.getMaskIndex(info->uid);
						if (maskIndex != -1)
						{
							m_pDataContext->volume_data.setCurrentMaskIndex(maskIndex);
							MaskInfo* info = m_pDataContext->volume_data.getMaskInfo(maskIndex);
							std::vector<muint32> vecSelect;
							vecSelect.push_back(info->uid);
							m_pDataContext->volume_data.setMultiSelectUID(vecSelect);
							pRoiTab->ROIList->SelectionUpdate(maskIndex);
						}
						else
							QMessageBox::warning(this, "error", "mask index is invalid.");
					}
				}
			}

			// updateF
			pRoiTab->updateROIHeader(L_COL_SHOW);
			pRoiTab->ROIList->UpdateByVolumeData();
			m_pWindowManager->forceUpdate2DViewData(false, true);
			m_pWindowManager->renderLater_GridView(true);

			OnUpdateProgressMultiThread((m_nReportVolumeVeiwCapturesteps / 2) * 10);
			//	m_pActionManager->action_ProgressUpdate(m_nReportVolumeVeiwCapturesteps * 20);
			//	m_pActionManager->action_ProgressEnd();

			m_pReportVolumeVeiwCaptureTimer->stop();
			m_nReportVolumeVeiwCapturesteps = 0;
			if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2))
			{
				m_pActionManager->action_DeepCatch_FeatureExtract(MFL_Product_DeepCatch_DeepCatchV2, pReport->tempInfo.bUseSingleThreadProgress);
			}
			else
			{
				m_pActionManager->action_DeepCatch_FeatureExtract(MFL_Product_DeepCatch, pReport->tempInfo.bUseSingleThreadProgress);
			}
			return;
		}
	}
	break;
	}
	m_nReportVolumeVeiwCapturesteps++;
}

// 201028 허 건 대리
// Qthread 렌더링 오류로 인한 signal slot 이용
////////////////////////////////////////////////////////////////////////////
void MedipQT::slot_buildRenderBufferTopology(mip::MeshTopology* _p_mesh)
{
	m_pWindowManager->buildRenderBufferTopology(_p_mesh);
}

void MedipQT::slot_renderLater()
{
	m_pWindowManager->renderLater_3DView();
}

void MedipQT::slot_MoveMeshDlg()
{
	m_pWindowManager->MoveMeshDialog();
}

// 210824 허 건 과장
// Mask List 갱신
void MedipQT::slot_applyMaskToUI(int _uid)
{
	m_pWindowManager->applyVoxelToUI(_uid);

	m_pWindowManager->updatePlaneData_all();

	m_pWindowManager->forceUpdate2DViewData(false, true);

	m_pWindowManager->updateUI(true, _uid);
	m_pWindowManager->renderLater_GridView(false);
	m_pWindowManager->setSaveState(false);
}

////////////////////////////////////////////////////////////////////////////
// 201103 허 건 대리
// Cretae Undo Redo Action
void MedipQT::CreateUndoRedoAction()
{
	QAction* p_act_undo = SHORTCUT_MANAGER->Action_UndoStack_Undo_Main();
	QAction* p_act_redo = SHORTCUT_MANAGER->Action_UndoStack_Redo_Main();

	disconnect(p_act_undo, &QAction::changed, this, &MedipQT::OnUndoStateChange);
	disconnect(m_pBtnUndo, &QPushButton::clicked, this, &MedipQT::OnUndoClick);
	disconnect(p_act_redo, &QAction::changed, this, &MedipQT::OnRedoStateChange);
	disconnect(m_pBtnRedo, &QPushButton::clicked, this, &MedipQT::OnRedoClick);
	removeAction(p_act_undo);
	removeAction(p_act_redo);
	p_act_redo = nullptr;
	p_act_undo = nullptr;

	SHORTCUT_MANAGER->Create_Action_UndoStack_Redo_Main();
	SHORTCUT_MANAGER->Create_Action_UndoStack_Undo_Main();

	p_act_undo = SHORTCUT_MANAGER->Action_UndoStack_Undo_Main();
	p_act_redo = SHORTCUT_MANAGER->Action_UndoStack_Redo_Main();

	connect(p_act_undo, &QAction::changed, this, &MedipQT::OnUndoStateChange);
	connect(m_pBtnUndo, &QPushButton::clicked, this, &MedipQT::OnUndoClick);
	connect(p_act_redo, &QAction::changed, this, &MedipQT::OnRedoStateChange);
	connect(m_pBtnRedo, &QPushButton::clicked, this, &MedipQT::OnRedoClick);
}

void MedipQT::OnPolyROI()
{
	if (m_pWindowManager->mainTabType == MAINTAB_SEGMENTATION)
	{
		m_pWindowManager->setSegmentationWorkMode(WORK_POLYROI);
	}
	else if (m_pWindowManager->mainTabType == MAINTAB_MEASUREMENT)
	{
		m_pWindowManager->setMeasurementWorkMode(ANAL_WORK_SEGMENT_POLY);
	}
	m_pWindowManager->renderLater_All();
}

void MedipQT::OnFreeDrawROI()
{
	if (m_pWindowManager->mainTabType == MAINTAB_SEGMENTATION)
	{
		m_pWindowManager->setSegmentationWorkMode(WORK_REGION_ROI);
	}
	else if (m_pWindowManager->mainTabType == MAINTAB_MEASUREMENT)
	{
		m_pWindowManager->setMeasurementWorkMode(ANAL_WORK_SEGMENT_FREEDRAW);
	}

	m_pWindowManager->renderLater_All();
}

void MedipQT::OnPickerROI()
{
	if (m_pWindowManager->mainTabType == MAINTAB_SEGMENTATION)
	{
		m_pWindowManager->setSegmentationWorkMode(WORK_PICKERROI);
	}
	else if (m_pWindowManager->mainTabType == MAINTAB_MEASUREMENT)
	{
		m_pWindowManager->setMeasurementWorkMode(ANAL_WORK_SEGMENT_PICKER);
	}

	m_pWindowManager->renderLater_All();
}

void MedipQT::OnOvalROI()
{
	if (m_pWindowManager->mainTabType == MAINTAB_SEGMENTATION)
	{
		m_pWindowManager->setSegmentationWorkMode(WORK_OVALROI);
	}
	else if (m_pWindowManager->mainTabType == MAINTAB_MEASUREMENT)
	{
		m_pWindowManager->setMeasurementWorkMode(ANAL_WORK_SEGMENT_OVAL);
	}

	m_pWindowManager->renderLater_All();
}

void MedipQT::OnRectROI()
{
	if (m_pWindowManager->mainTabType == MAINTAB_SEGMENTATION)
	{
		m_pWindowManager->setSegmentationWorkMode(WORK_RECTROI);
	}
	else if (m_pWindowManager->mainTabType == MAINTAB_MEASUREMENT)
	{
		m_pWindowManager->setMeasurementWorkMode(ANAL_WORK_SEGMENT_RECT);
	}

	m_pWindowManager->renderLater_All();
}

void MedipQT::OnShowBounding()
{
	if (
		m_pWindowManager->mainTabType == MAINTAB_SEGMENTATION ||
		m_pWindowManager->mainTabType == MAINTAB_TA
		)
	{
		bool mode = m_pWindowManager->getShowClippingBox();
		mode = !mode;	//toggle

		/* 버튼 및 데이터 업데이트 */
		m_pWindowManager->setShowBoundingBox(mode);
	}
}

void MedipQT::OnShowBoundaryEdge()
{
	if (
		m_pWindowManager->mainTabType == MAINTAB_SEGMENTATION ||
		m_pWindowManager->mainTabType == MAINTAB_TA
		)
	{
		bool mode = m_pWindowManager->getEdgeMode();
		mode = !mode;	//toggle

		/* 버튼 및 데이터 업데이트 */
		m_pWindowManager->setBoundingBoxEdge(mode);
	}
}

void MedipQT::OnRename()
{
	TabWindow* pTabList = m_pWindowManager->GetTab();
	if (pTabList)
	{
		MeshTab* pMeshTab = pTabList->getMeshTab();
		ROITab2* pROITab = pTabList->getROITab();

		if (pROITab)
		{
			if (pROITab->ROIList && pROITab->ROIList->hasFocus())
			{
				pROITab->ROIList->Rename();
			}
		}

		if (pMeshTab)
		{
			MeshListWidget* pMeshList = pMeshTab->GetMeshList();

			if (pMeshList && pMeshList->hasFocus())
			{
				pMeshList->rename();
			}
		}
	}
}

void MedipQT::slot_actionFinished(int id)
{
	qInfo() << "action finished. id : " << id;

	DATA_CONTEXT->GetOmniverseContext()->GetStage()->PushMeshWriteEvent();
	//g_Omniverse.UpdateMeshDataFromVolumeData();
}

void MedipQT::slot_actionProcessFinished(int actionProcessID)
{
	qInfo() << "action process finished. id : " << actionProcessID;

	DATA_CONTEXT->GetOmniverseContext()->GetStage()->PushMeshWriteEvent();
	//g_Omniverse.UpdateMeshDataFromVolumeData();
}



