#include "stdafx.h"
#include "MaskList.h"
#include "MedipQT.h"
#include "Windows/windowManager.h"
#include "System/resourceManager.h"
#include "System/stringManager.h"
#include "System/ShortcutManager.h"
#include "System/FileManager.h"
#include "System/styleManager.h"

#include "Windows/Tabwindow.h"
#include "Windows/Main/MainSegmentWidget.h"
#include "Windows/Main/MainMeshWidget.h"

#include "UI/ClickLabel.h"

#include "algorithm/FeatureExtractor.h"
#include "algorithm/MagicCut.h"
#include "Dialogs/ConfusionMatrixInfoDialog.h"
#include "Dialogs/NumDialog.h"
#include "Dialogs/DrawcutDlg.h"
#include "Dialogs/BrushSculptDlg.h"
#include "Dialogs/Interpolation3dDlg.h"
#include "Dialogs/LayerHistogramDlg.h"
#include "Dialogs/LevelsetDialog.h"
#include "Dialogs/ImageCalculatorDlg.h"

#include "Renderer/model.h"
#include "ComponentDlg.h"
#include "AnalysisWidget.h"

#include "MEVolumeView.h"

#include "UI/VisualPrint/VisualPrintMeshInfoWidget.h"

#include "CVoxelCountRatioDlg.h"
#include "AlphaWidget.h"

#define TEMP_CODE

MaskListWidget::MaskListWidget(VOLUME_DATA* pVolumeData, ProductManager* pProductManager, QWidget* parent)
	: QTreeWidget(parent),
	m_pVolumeData(pVolumeData),
	m_pProductManager(pProductManager),
	m_pDlgConfusionInfo(nullptr),
	m_pDlgImageCalculator(nullptr)
{
	setContextMenuPolicy(Qt::DefaultContextMenu);
	setAnimated(true);
	m_context = NULL;
	//m_actClearMask = new QAction(STRING_MANAGER->getString(STR_CLR_ROI), this);
	m_actClearMask = SHORTCUT_MANAGER->Action_Layer_General_Clear();
	if (m_actClearMask)
	{
		connect(m_actClearMask, &QAction::triggered, this, &MaskListWidget::OnClearMask);
	}

	m_actNew = SHORTCUT_MANAGER->Action_Layer_General_New();
	if (m_actNew)
	{
		connect(m_actNew, &QAction::triggered, this, &MaskListWidget::OnNew);
	}

	//m_actRename = new QAction(STRING_MANAGER->getString(STR_RENAME), this);
	// 주석처리  220713 허 건 과장 
	// Mesh List Rename short cut 등록안됨
	// MedipQT로 구현이동
	m_actRename = SHORTCUT_MANAGER->Action_Layer_General_Rename();
	//if (m_actRename)
	//{
	//	connect(m_actRename, &QAction::triggered, this, &MaskListWidget::OnRename);
	//}

	//m_actDelete = new QAction(STRING_MANAGER->getString(STR_DELETE), this);
	m_actDelete = SHORTCUT_MANAGER->Action_Layer_General_Delete();
	if (m_actDelete)
	{
		connect(m_actDelete, &QAction::triggered, this, &MaskListWidget::slot_OnDelete);
	}

	//m_actCopy = new QAction(STRING_MANAGER->getString(STR_COPY), this);
	m_actCopy = SHORTCUT_MANAGER->Action_Layer_General_Duplicate();
	if (m_actCopy)
	{
		connect(m_actCopy, &QAction::triggered, this, &MaskListWidget::slot_OnCopy);
	}

	m_actInvert = SHORTCUT_MANAGER->Action_Layer_Inverse();
	if (m_actInvert)
	{
		connect(m_actInvert, &QAction::triggered, this, &MaskListWidget::OnInvert);
	}

	//m_actIntersectAB = new QAction(STRING_MANAGER->getString(STR_FFS_ROI), this);

	m_actIntersectAB = SHORTCUT_MANAGER->Action_Layer_Boolean_FFS();
	if (m_actIntersectAB)
	{
		connect(m_actIntersectAB, &QAction::triggered, this, &MaskListWidget::slot_OnDifferenceAB);
	}

	//m_actIntersection = new QAction(STRING_MANAGER->getString(STR_INTERSECTION_ROI), this);
	m_actIntersection = SHORTCUT_MANAGER->Action_Layer_Boolean_Intersection();
	if (m_actIntersection)
	{
		connect(m_actIntersection, &QAction::triggered, this, &MaskListWidget::slot_OnIntersection);
	}

	//m_actMerge = new QAction(STRING_MANAGER->getString(STR_MERGE), this);
	m_actMerge = SHORTCUT_MANAGER->Action_Layer_Boolean_Merge();
	if (m_actMerge)
	{
		connect(m_actMerge, &QAction::triggered, this, &MaskListWidget::slot_OnMerge);
	}

	m_actSplitRegion = SHORTCUT_MANAGER->Action_Layer_Split_Region();
	if (m_actSplitRegion)
	{
		connect(m_actSplitRegion, &QAction::triggered, this, &MaskListWidget::OnSplitRegion);
	}

#if SUPPORT_3D_INTERPOLATION == 1
	m_actMakePolyhedron = SHORTCUT_MANAGER->Action_Layer_3D_Interpolation();
	if (m_actMakePolyhedron)
	{
		connect(m_actMakePolyhedron, &QAction::triggered, this, &MaskListWidget::OnInterPolation3D);
	}
#endif


	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_LayerOperation_ImageErosion) == eAVAILABLE_STATE::CREATE)
		//if (PRODUCT_MANAGER->IsAvailableFunctionLevel(MFL_Common_LayerOperation_ImageErosion) == eAVAILABLE_STATE::CREATE)
	{
		m_actErosion_Left = SHORTCUT_MANAGER->Action_Layer_Erosion_Left();
		if (m_actErosion_Left)
		{
			connect(m_actErosion_Left, &QAction::triggered, this, &MaskListWidget::slot_OnErosionLeft);
		}

		m_actErosion_Right = SHORTCUT_MANAGER->Action_Layer_Erosion_Right();
		if (m_actErosion_Right)
		{
			connect(m_actErosion_Right, &QAction::triggered, this, &MaskListWidget::slot_OnErosionRight);
		}

		m_actErosion_Posterior = SHORTCUT_MANAGER->Action_Layer_Erosion_Posterior();
		if (m_actErosion_Posterior)
		{
			connect(m_actErosion_Posterior, &QAction::triggered, this, &MaskListWidget::slot_OnErosionPosterior);
		}

		m_actErosion_Anterior = SHORTCUT_MANAGER->Action_Layer_Erosion_Anterior();
		if (m_actErosion_Anterior)
		{
			connect(m_actErosion_Anterior, &QAction::triggered, this, &MaskListWidget::slot_OnErosionAnterior);
		}

		m_actErosion_Superior = SHORTCUT_MANAGER->Action_Layer_Erosion_Superior();
		if (m_actErosion_Superior)
		{
			connect(m_actErosion_Superior, &QAction::triggered, this, &MaskListWidget::slot_OnErosionSuperior);
		}

		m_actErosion_Inferior = SHORTCUT_MANAGER->Action_Layer_Erosion_Inferior();
		if (m_actErosion_Inferior)
		{
			connect(m_actErosion_Inferior, &QAction::triggered, this, &MaskListWidget::slot_OnErosionInferior);
		}

		m_actErosion_6_Connectivity = SHORTCUT_MANAGER->Action_Layer_Erosion_6_Connectivity();
		if (m_actErosion_6_Connectivity)
		{
			connect(m_actErosion_6_Connectivity, &QAction::triggered, this, &MaskListWidget::slot_OnErosion);
		}
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_LayerOperation_ImageDilation) == eAVAILABLE_STATE::CREATE)
	{
		m_actDilation_Left = SHORTCUT_MANAGER->Action_Layer_Dilation_Left();
		if (m_actDilation_Left)
		{
			connect(m_actDilation_Left, &QAction::triggered, this, &MaskListWidget::slot_OnDilationLeft);
		}

		m_actDilation_Right = SHORTCUT_MANAGER->Action_Layer_Dilation_Right();
		if (m_actDilation_Right)
		{
			connect(m_actDilation_Right, &QAction::triggered, this, &MaskListWidget::slot_OnDilationRight);
		}

		m_actDilation_Posterior = SHORTCUT_MANAGER->Action_Layer_Dilation_Posterior();
		if (m_actDilation_Posterior)
		{
			connect(m_actDilation_Posterior, &QAction::triggered, this, &MaskListWidget::slot_OnDilationPosterior);
		}

		m_actDilation_Anterior = SHORTCUT_MANAGER->Action_Layer_Dilation_Anterior();
		if (m_actDilation_Anterior)
		{
			connect(m_actDilation_Anterior, &QAction::triggered, this, &MaskListWidget::slot_OnDilationAnterior);
		}

		m_actDilation_Superior = SHORTCUT_MANAGER->Action_Layer_Dilation_Superior();
		if (m_actDilation_Superior)
		{
			connect(m_actDilation_Superior, &QAction::triggered, this, &MaskListWidget::slot_OnDilationSuperior);
		}

		m_actDilation_Inferior = SHORTCUT_MANAGER->Action_Layer_Dilation_Inferior();
		if (m_actDilation_Inferior)
		{
			connect(m_actDilation_Inferior, &QAction::triggered, this, &MaskListWidget::slot_OnDilationInferior);
		}

		m_actDilation_6_Connectivity = SHORTCUT_MANAGER->Action_Layer_Dilation_6_Connectivity();
		if (m_actDilation_6_Connectivity)
		{
			connect(m_actDilation_6_Connectivity, &QAction::triggered, this, &MaskListWidget::slot_OnDilation);
		}
	}

	/* TODO : 테스트 완료 후 권한에 따라 생성되도록 할 것 */
	m_actImageCalculator = SHORTCUT_MANAGER->Action_Layer_ImageCalculator();
	//if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_LayerOperation_ImageCalculator) == eAVAILABLE_STATE::CREATE)
	{
		connect(m_actImageCalculator, &QAction::triggered, this, &MaskListWidget::OnImageCalculator);
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_LayerOperation_MoveMask) == eAVAILABLE_STATE::CREATE)
	{
		m_actMove_Left = SHORTCUT_MANAGER->Action_Layer_Move_Mask_Left();
		if (m_actMove_Left)
		{
			connect(m_actMove_Left, &QAction::triggered, this, &MaskListWidget::OnMoveLeft);
		}

		m_actMove_Right = SHORTCUT_MANAGER->Action_Layer_Move_Mask_Right();
		if (m_actMove_Right)
		{
			connect(m_actMove_Right, &QAction::triggered, this, &MaskListWidget::OnMoveRight);
		}

		m_actMove_Posterior = SHORTCUT_MANAGER->Action_Layer_Move_Mask_Posterior();
		if (m_actMove_Posterior)
		{
			connect(m_actMove_Posterior, &QAction::triggered, this, &MaskListWidget::OnMovePosterior);
		}

		m_actMove_Anterior = SHORTCUT_MANAGER->Action_Layer_Move_Mask_Anterior();
		if (m_actMove_Anterior)
		{
			connect(m_actMove_Anterior, &QAction::triggered, this, &MaskListWidget::OnMoveAnterior);
		}

		m_actMove_Superior = SHORTCUT_MANAGER->Action_Layer_Move_Mask_Superior();
		if (m_actMove_Superior)
		{
			connect(m_actMove_Superior, &QAction::triggered, this, &MaskListWidget::OnMoveSuperior);
		}

		m_actMove_Inferior = SHORTCUT_MANAGER->Action_Layer_Move_Mask_Inferior();
		if (m_actMove_Inferior)
		{
			connect(m_actMove_Inferior, &QAction::triggered, this, &MaskListWidget::OnMoveInferior);
		}
	}

#ifdef DEV_VER
	///////////////////////////
	//m_pActSetSeed = new QAction(STRING_MANAGER->getString(STR_SET_SEED_POINT), this);
	m_pActSetSeed = PRODUCT_FACTORY->createObject<QAction>(MFL_Common_LayerOperation_GeneralFunction, this);
	if (m_pActSetSeed)
	{
		m_pActSetSeed->setText(STRING_MANAGER->getString(STR_SET_SEED_POINT));
		//m_pActSetSeed->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_Down));
		connect(m_pActSetSeed, &QAction::triggered, this, &MaskListWidget::OnSetSeedPoint);
		addAction(m_pActSetSeed);
	}
#endif	
	//////////////////////////

	//m_act3DFilling = new QAction(STRING_MANAGER->getString(STR_HOLEFILLING_ROI), this);
	m_act3DFilling = SHORTCUT_MANAGER->Action_Layer_3DHoleFilling();
	if (m_act3DFilling)
	{
		connect(m_act3DFilling, &QAction::triggered, this, &MaskListWidget::On3DFilling);
	}

	m_actComponentChoice = new QAction(STRING_MANAGER->getString(STR_COMPONENT_ROI), this);
	if (m_actComponentChoice)
	{
		connect(m_actComponentChoice, &QAction::triggered, this, &MaskListWidget::OnComponentChoice);
	}

#ifdef DEV_VER
	//m_actVesselness = new QAction(STRING_MANAGER->getString(STR_VESSELNESS_ROI), this);
	m_actVesselness = PRODUCT_FACTORY->createObject<QAction>(MFL_Common_LayerOperation_GeneralFunction, this);
	if (m_actVesselness)
	{
		m_actVesselness->setText(STRING_MANAGER->getString(STR_VESSELNESS_ROI));
		connect(m_actVesselness, &QAction::triggered, this, &MaskListWidget::OnVesselness);
	}

	//m_actFissureness = new QAction(STRING_MANAGER->getString(STR_FISSURENESS_ROI), this);
	m_actFissureness = PRODUCT_FACTORY->createObject<QAction>(MFL_Common_LayerOperation_GeneralFunction, this);
	if (m_actFissureness)
	{
		m_actFissureness->setText(STRING_MANAGER->getString(STR_FISSURENESS_ROI));
		connect(m_actFissureness, &QAction::triggered, this, &MaskListWidget::OnFissureness);
	}

#else
	m_actVesselness = NULL;
	m_actFissureness = NULL;
#endif

#ifdef MULTI_DRAWCUT_MODE
	m_actSeed = new QAction("To Drawcut seed", this);
	connect(m_actSeed, &QAction::triggered, this, &MaskListWidget::OnToDrawSeed);
#else
	m_actSeed = NULL;
#endif

	//m_actExportSurface = new QAction(STRING_MANAGER->getString(STR_PREVIEW_SURFACE), this);
	m_actExportSurface = PRODUCT_FACTORY->createObject<QAction>(MFL_Common_MeshEditing_FileExport, this);
	if (m_actExportSurface)
	{
		m_actExportSurface->setText(STRING_MANAGER->getString(STR_PREVIEW_SURFACE));
		connect(m_actExportSurface, &QAction::triggered, this, &MaskListWidget::OnPreviewSurface);
	}

	//actExportStlFile = new QAction("Stereolithography (.stl, current layer)", this);
	//connect(actExportStlFile, &QAction::triggered, this, &MaskListWidget::slot_OnExportSTLFile);
	//actExportObjFile = new QAction("Object (.obj, current layer)", this);
	//connect(actExportObjFile, &QAction::triggered, this, &MaskListWidget::slot_OnExportOBJFile);
	//actExportRawFile = new QAction("ROI Mask raw (.raw, current layer)", this);
	//connect(actExportRawFile, &QAction::triggered, this, &MaskListWidget::OnExportRAWFile);
	//actExportHURawFile = new QAction("HU raw (.raw, current layer)", this);
	//connect(actExportHURawFile, &QAction::triggered, this, &MaskListWidget::OnExportHURAWFile);
	//actExportVTKFile = new QAction("Visualization Toolkit (.vtk, current layer)", this);
	//connect(actExportVTKFile, &QAction::triggered, this, &MaskListWidget::slot_OnExportVTKFile);
	//actExportNIIFile = new QAction("NIfTI-1 (.nii, current layer)", this);
	//connect(actExportNIIFile, &QAction::triggered, this, &MaskListWidget::OnExportROINIIFile);
	//actExportTXTFile = new QAction("XYZ Coordinates (.txt, current layer)", this);
	//connect(actExportTXTFile, &QAction::triggered, this, &MaskListWidget::OnExportTXTFile);
#if 0
	actToForeSeed = new QAction(STRING_MANAGER->getString(STR_FORWARD_SEED), this);
	connect(actToForeSeed, &QAction::triggered, this, &MaskListWidget::slot_OnCopyToForeSeed);
	actToBackSeed = new QAction(STRING_MANAGER->getString(STR_BACKWARD_SEED), this);
	connect(actToBackSeed, &QAction::triggered, this, &MaskListWidget::slot_OnCopyToBackSeed);
#else
	//m_actToForeSeed = new QWidgetAction(this);
	m_actToForeSeed = PRODUCT_FACTORY->createObject<QWidgetAction>(MFL_Common_Segmentation_Semi_Autosegmentation_DrawCut, this);
	if (m_actToForeSeed)
	{
		connect(m_actToForeSeed, &QAction::triggered, this, &MaskListWidget::slot_OnCopyToForeSeed);
	}

	//m_actToBackSeed = new QWidgetAction(this);
	m_actToBackSeed = PRODUCT_FACTORY->createObject<QWidgetAction>(MFL_Common_Segmentation_Semi_Autosegmentation_DrawCut, this);
	if (m_actToBackSeed)
	{
		connect(m_actToBackSeed, &QAction::triggered, this, &MaskListWidget::slot_OnCopyToBackSeed);
	}
#endif
	//m_actXFlip = new QAction(STRING_MANAGER->getString(STR_X_FLIP), this);
	m_actXFlip = PRODUCT_FACTORY->createObject<QAction>(MFL_Common_LayerOperation_Flip, this);
	if (m_actXFlip)
	{
		m_actXFlip->setText(STRING_MANAGER->getString(STR_X_FLIP));
		connect(m_actXFlip, &QAction::triggered, this, &MaskListWidget::slot_OnXFlip);
	}

	//m_actYFlip = new QAction(STRING_MANAGER->getString(STR_Y_FLIP), this);
	m_actYFlip = PRODUCT_FACTORY->createObject<QAction>(MFL_Common_LayerOperation_Flip, this);
	if (m_actYFlip)
	{
		m_actYFlip->setText(STRING_MANAGER->getString(STR_Y_FLIP));
		connect(m_actYFlip, &QAction::triggered, this, &MaskListWidget::slot_OnYFlip);
	}

	//m_actZFlip = new QAction(STRING_MANAGER->getString(STR_Z_FLIP), this);
	m_actZFlip = PRODUCT_FACTORY->createObject<QAction>(MFL_Common_LayerOperation_Flip, this);
	if (m_actZFlip)
	{
		m_actZFlip->setText(STRING_MANAGER->getString(STR_Z_FLIP));
		connect(m_actZFlip, &QAction::triggered, this, &MaskListWidget::slot_OnZFlip);
	}

	// Levelset 삭제
	// 	actLevelset = new QAction(STRING_MANAGER->getString(STR_LEVEL_SET), this);
	// 	actLevelset->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_L));
	// 	connect(actLevelset, &QAction::triggered, this, &MaskListWidget::OnLevelset);
	// 	addAction(actLevelset);

#ifdef DEV_VER
	//m_actHausdorff = new QAction("Hausdorff Distance", this);
	m_actHausdorff = PRODUCT_FACTORY->createObject<QAction>(MFL_Common_LayerOperation_GeneralFunction, this);
	if (m_actHausdorff)
	{
		m_actHausdorff->setText(QString("Hausdorff Distance"));
		connect(m_actHausdorff, &QAction::triggered, this, &MaskListWidget::OnHausdorff);
	}

	//m_actRandomP = new QAction("Generate random points", this);
	m_actRandomP = PRODUCT_FACTORY->createObject<QAction>(MFL_Common_LayerOperation_GeneralFunction, this);
	if (m_actRandomP)
	{
		m_actRandomP->setText(QString("Generate random points"));
		connect(m_actRandomP, &QAction::triggered, this, &MaskListWidget::OnRandomPoint);
	}

	//m_actConfusionMtx = new QAction(QString("Compute Confusion Matrix"), this);
	m_actConfusionMtx = PRODUCT_FACTORY->createObject<QAction>(MFL_Common_LayerOperation_GeneralFunction, this);
	if (m_actConfusionMtx)
	{
		m_actConfusionMtx->setText(QString("Compute Confusion Matrix"));
		connect(m_actConfusionMtx, &QAction::triggered, this, &MaskListWidget::OnComputeCofusionMtx);
		addAction(m_actConfusionMtx);
	}

#else
	//	actLevelset = NULL;
	m_actHausdorff = NULL;
	m_actRandomP = NULL;
	m_actConfusionMtx = NULL;
#endif

	//m_actFeature = new QAction(STRING_MANAGER->getString(STR_TA), this);
	m_actFeature = PRODUCT_FACTORY->createObject<QAction>(MFL_Common_Radiomics, this);
	if (m_actFeature)
	{
		m_actFeature->setText(STRING_MANAGER->getString(STR_TA));
		connect(m_actFeature, &QAction::triggered, this, &MaskListWidget::OnFeatureExtractor);
	}

	//m_actSimilar = new QAction("Similarity Coefficient", this);
	m_actSimilar = PRODUCT_FACTORY->createObject<QAction>(MFL_Common_LayerOperation_GeneralFunction, this);
	if (m_actSimilar)
	{
		m_actSimilar->setText(QString("Similarity Coefficient"));
		connect(m_actSimilar, &QAction::triggered, this, &MaskListWidget::OnSimilarity);
	}

	m_act2DFilling_AxialPlane = SHORTCUT_MANAGER->Action_Layer_2DHoleFilling_AxialPlane();
	if (m_act2DFilling_AxialPlane)
	{
		connect(m_act2DFilling_AxialPlane, &QAction::triggered, this, &MaskListWidget::OnHoleFillingAxial);
	}

	m_act2DFilling_CoronalPlane = SHORTCUT_MANAGER->Action_Layer_2DHoleFilling_CoronalPlane();
	if (m_act2DFilling_CoronalPlane)
	{
		connect(m_act2DFilling_CoronalPlane, &QAction::triggered, this, &MaskListWidget::OnHoleFillingCoronal);
	}

	m_act2DFilling_SagittalPlane = SHORTCUT_MANAGER->Action_Layer_2DHoleFilling_SagittalPlane();
	if (m_act2DFilling_SagittalPlane)
	{
		connect(m_act2DFilling_SagittalPlane, &QAction::triggered, this, &MaskListWidget::OnHoleFillingSagittal);
	}

	m_act2DFilling_WholePlane = SHORTCUT_MANAGER->Action_Layer_2DHoleFilling_WholePlane();
	if (m_act2DFilling_WholePlane)
	{
		connect(m_act2DFilling_WholePlane, &QAction::triggered, this, &MaskListWidget::OnHoleFillingWhole);
	}

	/* TreeWidget 이벤트 처리*/
	//	connect(this, &QTreeWidget::currentItemChanged, this, &MaskListWidget::OnMaskItemChanged);

	editMode = false;
	setRootIsDecorated(false);
	setAcceptDrops(true);
	setDragEnabled(true);
	setDragDropMode(QAbstractItemView::DragDrop);
	setSelectionMode(QAbstractItemView::ExtendedSelection);
}

MaskListWidget::~MaskListWidget()
{

}

QSize MaskListWidget::SizeForRow()
{
	return QSize(sizeHintForRow(0), sizeHintForRow(0));
}

void MaskListWidget::contextMenuEvent(QContextMenuEvent* event)
{
	if (!m_pVolumeData->isValidate())
		return;

	if (editMode)
		return;

	//#if !defined(DEEP_CATCH_VER) && !defined(COVID19_VER)
	QMenu menu(this);
	m_context = &menu;
	QMenu* subMenu = NULL;
	if (WIN_MANAGER->mainTabType == MAINTAB_MEASUREMENT)
	{
		menu.addAction(m_actSimilar);
		menu.addAction(m_actHausdorff);
		menu.addAction(m_actFeature);

		menu.addAction(m_actConfusionMtx);

		subMenu = menu.addMenu("Segmentation");
	}
	else
		subMenu = &menu;

	subMenu->setStyleSheet(STYLE_MANAGER->m_MenuMaskROI);

	if (m_actClearMask) subMenu->addAction(m_actClearMask);
	if (m_actNew) subMenu->addAction(m_actNew);
	if (m_actRename) subMenu->addAction(m_actRename);
	if (m_actCopy) subMenu->addAction(m_actCopy);
	if (m_actDelete) subMenu->addAction(m_actDelete);
	if (m_actInvert) subMenu->addAction(m_actInvert);

	subMenu->addAction(subMenu->addSeparator());

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_LayerOperation_LayerBooleanFunction_First_First_Second) == eAVAILABLE_STATE::CREATE
		&& isEditableMask() == true)
	{
		if (m_actIntersectAB) subMenu->addAction(m_actIntersectAB);
	}
	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_LayerOperation_LayerBooleanFunction_Intersection) == eAVAILABLE_STATE::CREATE
		&& isEditableMask() == true)
	{
		if (m_actIntersection)
		{
			subMenu->addAction(m_actIntersection);
		}
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_LayerOperation_LayerBooleanFunction_merge) == eAVAILABLE_STATE::CREATE
		&& isEditableMask() == true)
	{
		if (m_actMerge)
		{
			subMenu->addAction(m_actMerge);
		}
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_LayerOperation_MaskSplitRegion) == eAVAILABLE_STATE::CREATE
		&& isEditableMask() == true)
	{
		if (m_actSplitRegion) subMenu->addAction(m_actSplitRegion);
	}

#if SUPPORT_3D_INTERPOLATION == 1
	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_LayerOperation_3DInterpolation) == eAVAILABLE_STATE::CREATE
		&& isEditableMask() == true)
	{
		if (m_actMakePolyhedron) subMenu->addAction(m_actMakePolyhedron);
	}
#endif

	subMenu->addAction(subMenu->addSeparator());
	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_LayerOperation_ImageErosion) == eAVAILABLE_STATE::CREATE)
	{
		QMenu* erosion = subMenu->addMenu(STRING_MANAGER->getString(STR_EROSION_ROI));

		erosion->addAction(m_actErosion_Left);
		erosion->addAction(m_actErosion_Right);
		erosion->addAction(m_actErosion_Posterior);
		erosion->addAction(m_actErosion_Anterior);
		erosion->addAction(m_actErosion_Superior);
		erosion->addAction(m_actErosion_Inferior);
		erosion->addAction(m_actErosion_6_Connectivity);
	}
	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_LayerOperation_ImageDilation) == eAVAILABLE_STATE::CREATE)
	{
		QMenu* dilation = subMenu->addMenu(STRING_MANAGER->getString(STR_DILATION_ROI));

		dilation->addAction(m_actDilation_Left);
		dilation->addAction(m_actDilation_Right);
		dilation->addAction(m_actDilation_Posterior);
		dilation->addAction(m_actDilation_Anterior);
		dilation->addAction(m_actDilation_Superior);
		dilation->addAction(m_actDilation_Inferior);
		dilation->addAction(m_actDilation_6_Connectivity);

		subMenu->addAction(subMenu->addSeparator());
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_LayerOperation_ImageCalculator) == eAVAILABLE_STATE::CREATE)
	{
		if (m_actImageCalculator)
			subMenu->addAction(m_actImageCalculator);
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_LayerOperation_MoveMask) == eAVAILABLE_STATE::CREATE)
	{
		QMenu* move = subMenu->addMenu(STRING_MANAGER->getString(STR_MOVE_MASK_ROI));

		move->addAction(m_actMove_Left);
		move->addAction(m_actMove_Right);
		move->addAction(m_actMove_Posterior);
		move->addAction(m_actMove_Anterior);
		move->addAction(m_actMove_Superior);
		move->addAction(m_actMove_Inferior);

		subMenu->addAction(subMenu->addSeparator());
	}

	if (m_pActSetSeed)
		subMenu->addAction(m_pActSetSeed);

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_LayerOperation_3DHoleFilling) == eAVAILABLE_STATE::CREATE)
	{
		if (m_act3DFilling)
			subMenu->addAction(m_act3DFilling);
	}

	subMenu->addAction(m_actRandomP);

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_LayerOperation_2DHoleFilling) == eAVAILABLE_STATE::CREATE)
	{
		QMenu* holeFilling = subMenu->addMenu(STRING_MANAGER->getString(STR_HOLEFILLING2D_ROI));

		holeFilling->addAction(m_act2DFilling_AxialPlane);
		holeFilling->addAction(m_act2DFilling_CoronalPlane);
		holeFilling->addAction(m_act2DFilling_SagittalPlane);
		holeFilling->addAction(m_act2DFilling_WholePlane);
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_LayerOperation_Component) == eAVAILABLE_STATE::CREATE)
	{
		subMenu->addAction(m_actComponentChoice);
	}

	if (m_actVesselness) subMenu->addAction(m_actVesselness);
	if (m_actFissureness) subMenu->addAction(m_actFissureness);

	//if (actLevelset) subMenu->addAction(actLevelset);
	subMenu->addAction(subMenu->addSeparator());
	QMenu* downloadAs = nullptr;
	QAction* actExport = nullptr;

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_MeshEditing_FileExport) == eAVAILABLE_STATE::CREATE
		|| m_pProductManager->IsAvailableFunctionLevel(MFL_Common_LayerOperation_MaskExport) == eAVAILABLE_STATE::CREATE)
	{
		if (WIN_MANAGER->mainTabType == MAINTAB_SEGMENTATION || WIN_MANAGER->mainTabType == MAINTAB_MESH_EDITING)
		{
			subMenu->addAction(m_actExportSurface);
		}
		downloadAs = subMenu->addMenu("Export as...");
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_MeshEditing_FileExport) == eAVAILABLE_STATE::CREATE)
	{
		actExport = downloadAs->addAction("Stereolithography (.stl, current layer)");
		connect(actExport, &QAction::triggered, this, &MaskListWidget::slot_OnExportSTLFile);
		//actExport = downloadAs->addAction("Stereolithography Patient Coordinate(.stl, current layer)");
		//connect(actExport, &QAction::triggered, this, &MaskListWidget::slot_OnExportSTLFilePatientCoordinate);
		actExport = downloadAs->addAction("Object (.obj, current layer)");
		connect(actExport, &QAction::triggered, this, &MaskListWidget::slot_OnExportOBJFile);
		//actExport = downloadAs->addAction("Object Patient Coordinate(.obj, current layer)");
		//connect(actExport, &QAction::triggered, this, &MaskListWidget::slot_OnExportOBJFilePatientCoordinate);
		actExport = downloadAs->addAction("Visualization Toolkit (.vtk, current layer)");
		connect(actExport, &QAction::triggered, this, &MaskListWidget::slot_OnExportVTKFile);
		//actExport = downloadAs->addAction("Visualization Toolkit Patient Coordinate(.vtk, current layer)");
		//connect(actExport, &QAction::triggered, this, &MaskListWidget::slot_OnExportVTKFilePatientCoordinate);
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_LayerOperation_MaskExport) == eAVAILABLE_STATE::CREATE
		&& isEditableMask() == true)
	{
		actExport = downloadAs->addAction("ROI Mask raw (.raw, current layer)");
		connect(actExport, &QAction::triggered, this, &MaskListWidget::OnExportRAWFile);
		actExport = downloadAs->addAction("HU raw (.raw, current layer)");
		connect(actExport, &QAction::triggered, this, &MaskListWidget::OnExportHURAWFile);
		actExport = downloadAs->addAction("NIfTI-1 (.nii, current layer)");
		connect(actExport, &QAction::triggered, this, &MaskListWidget::OnExportROINIIFile);
		actExport = downloadAs->addAction("NIfTI-1 (.nii, patient coordinate)");
		connect(actExport, &QAction::triggered, this, &MaskListWidget::OnExportCOORDINATENIIFile);
		actExport = downloadAs->addAction("NIfTI-1 (.nii, HU layer)");
		connect(actExport, &QAction::triggered, this, &MaskListWidget::OnExportHUNIIFile);
		actExport = downloadAs->addAction("XYZ Coordinates (.txt, current layer)");
		connect(actExport, &QAction::triggered, this, &MaskListWidget::OnExportTXTFile);
	}

	subMenu->addAction(subMenu->addSeparator());

	if (m_actToForeSeed && m_actToForeSeed->defaultWidget())
	{
		if (!m_actToForeSeed->defaultWidget()->isHidden())
			m_actToForeSeed->defaultWidget()->hide();
		m_actToForeSeed->defaultWidget()->deleteLater();
	}

	if (m_actToBackSeed && m_actToBackSeed->defaultWidget())
	{
		if (!m_actToBackSeed->defaultWidget()->isHidden())
			m_actToBackSeed->defaultWidget()->hide();
		m_actToBackSeed->defaultWidget()->deleteLater();
	}

	int indent = subMenu->style()->pixelMetric(QStyle::PM_SmallIconSize);
	int w = 0, h = 0;
	if (subMenu)
	{
		QRect _rect = subMenu->actionGeometry(m_actIntersectAB);

		w = _rect.width() * 0.33 * 2;
		h = _rect.height();
	}

	//ClickLabel *label = new ClickLabel(NULL); //ownership to actToforeseed
	ClickLabel* labelForeseed = PRODUCT_FACTORY->createWidget<ClickLabel>(MFL_Common_Segmentation_Semi_Autosegmentation_DrawCut, this);
	if (labelForeseed)
	{
		COLOR _col = WIN_MANAGER->getSeedColor(1);
		labelForeseed->setText(QString("<span> copy to </span><span style=\"color:rgb(%1, %2, %3);\">Foreseed</span> </a>").arg(_col.r).arg(_col.g).arg(_col.b));
		labelForeseed->setFixedSize(w, h);
		labelForeseed->setIndent(indent);

		m_actToForeSeed->setDefaultWidget(labelForeseed);
		connect(labelForeseed, &ClickLabel::clicked, m_actToForeSeed, &QAction::trigger);
	}
	//label = new ClickLabel(NULL); //ownership to actTobackseed
	ClickLabel* labelBackseed = PRODUCT_FACTORY->createWidget<ClickLabel>(MFL_Common_Segmentation_Semi_Autosegmentation_DrawCut, this);
	if (labelBackseed)
	{
		COLOR _col = WIN_MANAGER->getSeedColor(0);
		labelBackseed->setText(QString("<span> copy to </span><span style=\"color:rgb(%1, %2, %3);\"> Backseed</span> </a>").arg(_col.r).arg(_col.g).arg(_col.b));
		labelBackseed->setFixedSize(w, h);
		labelBackseed->setIndent(indent);
		m_actToBackSeed->setDefaultWidget(labelBackseed);
		connect(labelBackseed, &ClickLabel::clicked, m_actToBackSeed, &QAction::trigger);
	}

	if (m_actToForeSeed) subMenu->addAction(m_actToForeSeed);
	if (m_actToBackSeed) 	subMenu->addAction(m_actToBackSeed);

	if (m_actSeed) subMenu->addAction(m_actSeed);

	if (m_actXFlip) subMenu->addAction(m_actXFlip);
	if (m_actYFlip) subMenu->addAction(m_actYFlip);
	if (m_actZFlip) subMenu->addAction(m_actZFlip);

	menu.exec(event->globalPos());
	//#endif

}

void MaskListWidget::OnClearMask()
{
	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	QList<QTreeWidgetItem*>& list = selectedItems();

	if (list.size() != 1)
	{
		QMessageBox::warning(this, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(STR_ONE_SELECT) +
			STRING_MANAGER->getString(STR_CLR_ROI));
		return;
	}

	muint32 index = 0;
	QTreeWidgetItem* item = currentItem();
	if (currentItem()->childCount() == 0)
		item = currentItem()->parent();

	index = indexOfTopLevelItem(item);

	MaskInfo* info = m_pVolumeData->getMaskInfo(index);
	int mI = info->uid >= MASK_SECOND_MAX ? (info->uid - MASK_SECOND_MAX) / 8 + 1 : 0;

	ACTION_MANAGER->action_MaskList_clear_one(index, mI);
}

void MaskListWidget::OnMaskItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
	if (current)
	{
		mint32 index = indexOfTopLevelItem(current);

		if (index >= 0 && index < topLevelItemCount())
		{
			SelectionRefresh(index);
			m_pVolumeData->setCurrentMaskIndex(index);
		}
		else
		{
			SelectionRefresh(m_pVolumeData->getCurrentMaskInfoID());
		}

		WIN_MANAGER->renderLater_GridView();
		WIN_MANAGER->renderLater_3DView();
	}
}

void MaskListWidget::OnNew()
{
	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	ACTION_MANAGER->action_MaskList_add();
}

void MaskListWidget::Rename()
{
	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	QList<QTreeWidgetItem*>& list = selectedItems();

	if (list.size() != 1)
	{
		QMessageBox::warning(this, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(STR_ONE_SELECT) +
			STRING_MANAGER->getString(STR_RENAME));
		return;
	}
	muint32 index = 0;
	QTreeWidgetItem* item = currentItem();
	if (currentItem()->childCount() == 0)
		item = currentItem()->parent();

	index = indexOfTopLevelItem(item);

	//	MaskInfo * editMask = m_pVolumeData->getCurrentMaskInfo();
	editMode = true;
	WIN_MANAGER->setMoveFocus(false);
	item->setFlags(item->flags() | Qt::ItemIsEditable);
	editItem(item, L_COL_NAME);
	//	openPersistentEditor(list[0],2);

}

void MaskListWidget::slot_OnCopy()
{
	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	QList<QTreeWidgetItem*>& list = selectedItems();

	if (list.size() != 1)
	{
		QMessageBox::warning(this, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(STR_ONE_SELECT) +
			STRING_MANAGER->getString(STR_COPY));
		return;
	}

	muint32 index = 0;
	QTreeWidgetItem* item = currentItem();
	if (currentItem()->childCount() == 0)
		item = currentItem()->parent();

	index = indexOfTopLevelItem(item);

	ACTION_MANAGER->action_MaskList_copy(index);
}

void MaskListWidget::slot_OnDelete()
{
	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	QList<QTreeWidgetItem*>& list = selectedItems();
	QList<QTreeWidgetItem*> Dellist;

	for (int i = 0; i < list.size(); i++)
	{
		QTreeWidgetItem* item = list.at(i);
		int index = 0;
		if (item->childCount() == 0)
			item = item->parent();

		index = indexOfTopLevelItem(item);

		if (!Dellist.contains(item))
			Dellist.push_back(item);

	}

	if (Dellist.size() >= m_pVolumeData->getMaskInfoListCnt())
	{
		QMessageBox::warning(this, STRING_MANAGER->getString(STR_WARN), QString("Only %1 or fewer layers can be deleted.")
			.arg(m_pVolumeData->getMaskInfoListCnt() - 1));
		return;
	}

	if (Dellist.size() == 1)
	{
		muint32 index = m_pVolumeData->getCurrentMaskInfoID();
		MaskInfo* info = m_pVolumeData->getMaskInfo(index);
		int mI = info->uid >= MASK_SECOND_MAX ? (info->uid - MASK_SECOND_MAX) / 8 + 1 : 0;

		//ACTION_MANAGER->action_MaskList_del(index, mI);
		ACTION_MANAGER->action_MaskList_del_ex(m_pVolumeData, index, mI);
	}
	else if (Dellist.size() > 1)
	{
		std::vector<int> indeces;
		for (int cnt = 0; cnt < Dellist.size(); cnt++)
		{
			indeces.push_back(indexOfTopLevelItem(Dellist[cnt]));
		}

		ACTION_MANAGER->action_MaskList_del_list_ex(m_pVolumeData, indeces);
	}
}

void MaskListWidget::OnInvert()
{
	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	QList<QTreeWidgetItem*>& list = selectedItems();

	if (list.size() != 1)
	{
		QMessageBox::warning(this, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(STR_ONE_SELECT) +
			STRING_MANAGER->getString(STR_INVERSE));
		return;
	}


	muint32 index = 0;
	QTreeWidgetItem* item = currentItem();
	if (currentItem()->childCount() == 0)
		item = currentItem()->parent();

	index = indexOfTopLevelItem(item);

	MaskInfo* info = m_pVolumeData->getMaskInfo(index);
	int mI = info->uid >= MASK_SECOND_MAX ? (info->uid - MASK_SECOND_MAX) / 8 + 1 : 0;

	ACTION_MANAGER->action_MaskList_Invert(*info, mI);
}

void MaskListWidget::slot_OnDifferenceAB()
{
	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	QList<QTreeWidgetItem*>& list = selectedItems();
	QList<QTreeWidgetItem*> Difflist;

	for (int i = 0; i < list.size(); i++)
	{
		QTreeWidgetItem* item = list.at(i);

		if (item->childCount() == 0)
			item = item->parent();

		muint32 index = indexOfTopLevelItem(item);

		if (!Difflist.contains(item))
			Difflist.push_back(item);
	}

	if (Difflist.size() != 2)
	{
		QMessageBox::warning(this, STRING_MANAGER->getString(STR_FFS_ROI), STRING_MANAGER->getString(STR_SELECT_2_WARN));
		return;
	}

	int count = m_pVolumeData->getMaskInfoListCnt();
	if (count < 2)
		return;

	mint32 indeces[2];
	int cnt = 0;
	for (cnt = 0; cnt < Difflist.size(); cnt++)
	{
		indeces[cnt] = indexOfTopLevelItem(Difflist[cnt]);

		if (isVolumeDataMaskEditableByIndex(indeces[cnt]) == false)
			return;
	}

	MaskInfo* A = m_pVolumeData->getMaskInfo(indeces[0]);
	MaskInfo* B = m_pVolumeData->getMaskInfo(indeces[1]);

	if (A && B)
	{
		ACTION_MANAGER->action_MaskList_DifferenceAB(*A, *B);
	}
}

void MaskListWidget::slot_OnIntersection()
{
	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	QList<QTreeWidgetItem*>& list = selectedItems();
	QList<QTreeWidgetItem*> Difflist;

	for (int i = 0; i < list.size(); i++)
	{
		QTreeWidgetItem* item = list.at(i);

		if (item->childCount() == 0)
			item = item->parent();

		muint32 index = indexOfTopLevelItem(item);

		if (!Difflist.contains(item))
			Difflist.push_back(item);
	}

	if (Difflist.size() != 2)
	{
		QMessageBox::warning(this, STRING_MANAGER->getString(STR_INTERSECTION_ROI), STRING_MANAGER->getString(STR_SELECT_2_WARN));
		return;
	}

	int count = m_pVolumeData->getMaskInfoListCnt();
	if (count < 2)
		return;

	mint32 indeces[2];
	int cnt = 0;
	for (cnt = 0; cnt < Difflist.size(); cnt++)
	{
		indeces[cnt] = indexOfTopLevelItem(Difflist[cnt]);

		if (isVolumeDataMaskEditableByIndex(indeces[cnt]) == false)
			return;
	}

	MaskInfo* A = m_pVolumeData->getMaskInfo(indeces[0]);
	MaskInfo* B = m_pVolumeData->getMaskInfo(indeces[1]);

	if (A && B)
	{
		ACTION_MANAGER->action_MaskList_intersection(*A, *B);
	}
}

void MaskListWidget::mousePressEvent(QMouseEvent* event)
{
	//qDebug() << "mousePressEvent(QMouseEvent *event)" << endl;

	if (!m_pVolumeData->isValidate())
	{
		return;
	}

	QTreeWidget::mousePressEvent(event);

	QTreeWidgetItem* item = itemAt(event->pos());
	if (item == NULL || (Qt::NoModifier == event->modifiers() && (event->button() != Qt::RightButton)))
	{
		mint32 index = indexOfTopLevelItem(item);

		if (index >= 0 && index < topLevelItemCount())
		{
			SelectionRefresh(index);
			m_pVolumeData->setCurrentMaskIndex(index);
		}
		else
		{
			SelectionRefresh(m_pVolumeData->getCurrentMaskInfoID());
		}

		WIN_MANAGER->renderLater_GridView();
		WIN_MANAGER->renderLater_3DView();
		return;
	}

	mint32 index = indexOfTopLevelItem(item);

	if (index >= 0 && index < topLevelItemCount())
	{
		m_pVolumeData->setCurrentMaskIndex(index);
		SelectionUpdate(index);
	}
	else
	{
		SelectionUpdate(m_pVolumeData->getCurrentMaskInfoID());
	}

	/////////////////////////////////////
	// set multiSelectUID data
	QTreeWidgetItem* it;
	std::vector<muint32> vecMaskUIDtmp;
	for (int n = 0; n < topLevelItemCount(); n++)
	{
		it = topLevelItem(n);
		if (it->isSelected())
		{
			MaskInfo* info = m_pVolumeData->getMaskInfo(n);
			vecMaskUIDtmp.push_back(info->uid);
		}
	}
	m_pVolumeData->setMultiSelectUID(vecMaskUIDtmp);
	WIN_MANAGER->renderLater_3DView();
}

void MaskListWidget::mouseReleaseEvent(QMouseEvent* e)
{
	if (!m_pVolumeData->isValidate())
		return;

	QTreeWidgetItem* item = itemAt(e->pos());

	if (selectedItems().size() <= 1)
		QTreeWidget::mouseReleaseEvent(e);

	if (item == NULL)
	{
		if (selectedItems().size() <= 1)
		{
			SelectionRefresh(m_pVolumeData->getCurrentMaskInfoID());
			return;
		}
	}


	////////////////////////////////////////
	// 	MaskInfo *info = m_pVolumeData->getCurrentMaskInfo();
	// 
	// 	int nVecIdx = -1, nByteIndex, nMaskBit, nThreshold;
	// 
	// 	if (info)
	// 	{		
	// 		nByteIndex = m_pVolumeData->GetMaskByteIndex(info->uid);
	// 		nMaskBit = m_pVolumeData->getMask(info->uid);
	// 		//m_TAState = m_pVolumeData->getTAState(m_uid);
	// 		//m_orgbox[0] = m_pVolumeData->getBoundingBox(m_uid);
	// 		//m_orgbox[1].reset(true);
	// 
	// 		AISegTab * pAItab = WIN_MANAGER->GetTab()->getAITab();
	// 		if (pAItab && pAItab->m_tabSet)
	// 		{
	// 			nThreshold = pAItab->m_tabSet->getOutVal();
	// 		}
	// 				
	// 		for (int i = 0; i< m_pVolumeData->m_vec2dClassData.size(); ++i)
	// 		{
	// 			if (m_pVolumeData->m_vec2dClassData[i].first == info->uid)
	// 			{
	// 				nVecIdx = i;
	// 				break;
	// 			}
	// 		}
	// 	}
	// 	 
	// 	int nVoxelCount = -1;
	// 	if (nVecIdx < m_pVolumeData->m_vec2dClassData.size())
	// 	{
	// 		nVoxelCount = m_pVolumeData->applyTempMaskBitOutset(m_pVolumeData->m_vec2dClassData[nVecIdx].second
	// 			, nMaskBit, nByteIndex, nThreshold);
	// 	}
	// 
	// 	m_pVolumeData->forceUpdateMaskVolume();
	//  	m_pVolumeData->setBoundingBox(info->uid, m_pVolumeData->getBoundingBox(info->uid));
	//  	m_pVolumeData->setVoxelCount(info->uid, nVoxelCount, false);
	//  	WIN_MANAGER->renderLater_GridView(false);
	//  	m_pVolumeData->setTAState(info->uid, true);
	//  	WIN_MANAGER->applyVoxelToUI(info->uid);
	// 	
}
// 
// void MaskListWidget::mouseMoveEvent(QMouseEvent *e)
// {
// 	qDebug() << QString::number(e->pos().x());
// 	qDebug() << QString::number(e->pos().y());
// }

void MaskListWidget::keyReleaseEvent(QKeyEvent* e)
{
	if (!m_pVolumeData->isValidate())
		return;

	if ((Qt::Key_Down == e->key() || Qt::Key_Up == e->key()) && Qt::NoModifier == e->modifiers())
	{
		QTreeWidgetItem* item = currentItem();
		mint32 index = indexOfTopLevelItem(item);

		if (index >= 0 && index < topLevelItemCount())
		{
			SelectionRefresh(index);
			m_pVolumeData->setCurrentMaskIndex(index);
		}
		else
			SelectionRefresh(m_pVolumeData->getCurrentMaskInfoID());

		WIN_MANAGER->renderLater_GridView();
		WIN_MANAGER->renderLater_3DView();
		return;
	}

	if (Qt::Key_Enter == e->key())
	{
		editMode = false;
	}
	else if (Qt::Key_Delete == e->key())
	{
		if (!editMode)
		{
			slot_OnDelete();
			return;
		}
	}

	QTreeWidget::keyReleaseEvent(e);
}

void MaskListWidget::keyPressEvent(QKeyEvent* e)
{
	if (!editMode)
	{
		if (Qt::Key_Right == e->key())
		{
			e->ignore();
			return;
		}
		else if (Qt::Key_Left == e->key())
		{
			e->ignore();
			return;
		}
	}

	QTreeWidget::keyPressEvent(e);
}

void MaskListWidget::startDrag(Qt::DropActions supportedActions)
{
	if (WIN_MANAGER->mainTabType == MAINTAB_REPORT)
	{
		QList<QTreeWidgetItem*>& list = selectedItems();

		if (list.size() != 1 || list.size() == 0)
		{
			QMessageBox::warning(this, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(STR_ONE_SELECT) +
				STRING_MANAGER->getString(STR_INVERSE));
			return;
		}

		MaskInfo* info = m_pVolumeData->getCurrentMaskInfo();
		if (nullptr == info)
			return;
		int mI = info->uid >= MASK_SECOND_MAX ? (info->uid - MASK_SECOND_MAX) / 8 + 1 : 0;

		QMimeData* mimeData = new QMimeData;
		QByteArray cellData;
		char* pVal = reinterpret_cast<char*>(info->maskName);
		if (nullptr == pVal)
			return;
		cellData.append(pVal, sizeof(info->maskName));

		pVal = reinterpret_cast<char*>(&mI);
		if (nullptr == pVal)
			return;
		cellData.append(pVal, sizeof(int));

		pVal = reinterpret_cast<char*>(info);
		if (nullptr == pVal)
			return;
		cellData.append(pVal, sizeof(MaskInfo));

		mimeData->setData("Texture_Feature", cellData);
		QDrag* drag = new QDrag(this);
		drag->setMimeData(mimeData);
		drag->exec(Qt::CopyAction);
	}

	QTreeWidget::startDrag(supportedActions);
}

bool MaskListWidget::isEditableMask(void)
{
	if (m_pVolumeData->isValidate() == false)
		return false;

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_AIResearch_AIMaskExport))
		return true;

	QList<QTreeWidgetItem*>& list = selectedItems();
	QList<QTreeWidgetItem*> Difflist;

	for (int i = 0; i < list.size(); i++)
	{
		QTreeWidgetItem* item = list.at(i);

		if (item->childCount() == 0)
			item = item->parent();

		if (!Difflist.contains(item))
			Difflist.push_back(item);
	}

	//mint32 indeces[2];
	mint32 indeces;
	int cnt = 0;
	for (cnt = 0; cnt < Difflist.size(); cnt++)
	{
		indeces = indexOfTopLevelItem(Difflist[cnt]);

		if (m_pVolumeData->isMaskEditable(indeces) == false)
			return false;
	}
	return true;
}

bool MaskListWidget::isVolumeDataMaskEditableByIndex(int nIndex)
{
	if (m_pVolumeData->isValidate() == false)
		return false;

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_AIResearch_AIMaskExport))
		return true;

	return m_pVolumeData->isMaskEditable(nIndex);
}

QTreeWidgetItem* MaskListWidget::insertRowItem(int index, MaskInfo* info)
{
	QTreeWidgetItem* item = new QTreeWidgetItem(this);

	setRowItemDataByMaskInfo(item, info);

	QTreeWidgetItem* subItem = new QTreeWidgetItem(item);
	subItem->setFirstColumnSpanned(true);
	setItemWidget(subItem, 0, new AlphaWidget(m_pVolumeData, info->uid, this));

	item->addChild(subItem);
	item->setExpanded(false);

	insertTopLevelItem(index, item);

	if (MAINTAB_SEGMENTATION != WIN_MANAGER->mainTabType)
	{
		item->setText(L_COL_ALPHA, "*");
		subItem->setHidden(true);
	}

	return item;
}

void MaskListWidget::setRowItemDataByMaskInfo(QTreeWidgetItem* item, MaskInfo* info)
{
	bool needUpdate = (WIN_MANAGER->mainTabType == MAINTAB_TA) && !m_pVolumeData->getTAState(info->uid);

	item->setText(L_COL_ALPHA, item->isExpanded() ? "-" : "+");
	item->setBackground(L_COL_COLOR, QBrush(QColor(info->color.r, info->color.g, info->color.b, 255)));
	item->setIcon(L_COL_SHOW, RESOURCE_MANAGER->getIcon(info->show ? ICON_LIST_VISIBLE : ICON_LIST_INVISIBLE));
	item->setText(L_COL_NAME, QString::fromWCharArray(info->maskName));
	item->setTextColor(L_COL_NAME, needUpdate ? QColor(255, 0, 0) : QColor(255, 255, 255));
	item->setToolTip(L_COL_NAME, getVoxelInfoText(info));

	QString AIText;
	if (m_pVolumeData->IsAIMaskByUID(info->uid))
	{
		AIText = "AI";
	}
	item->setText(L_COL_AI, AIText);

	if (item->child(0))
	{
		if (item->child(0)->isHidden())
		{
			item->setText(L_COL_ALPHA, "*");
		}
	}
}

void MaskListWidget::setRowItemBackgroundColor_Selected(QTreeWidgetItem* item)
{
	QColor colorSelected = QColor(200, 200, 200, 255);
	setRowItemBackgroundColor(item, colorSelected);
}

void MaskListWidget::setRowItemBackgroundColor_UnSelected(QTreeWidgetItem* item)
{
	QColor colorUnselected = QColor(48, 48, 48, 48);
	setRowItemBackgroundColor(item, colorUnselected);
}

void MaskListWidget::setRowItemBackgroundColor(QTreeWidgetItem* item, QColor color)
{
	item->setBackground(L_COL_ALPHA, QBrush(color));
	item->setBackground(L_COL_SHOW, QBrush(color));
	item->setBackground(L_COL_AI, QBrush(color));
	item->setBackground(L_COL_NAME, QBrush(color));
}

QString MaskListWidget::getVoxelInfoText(MaskInfo* info) const
{
	int voxelCount = m_pVolumeData->getVoxelCount(info->uid);
	int voxelDataLength = m_pVolumeData->getVolumeDataLength();
	int per = (int)(((float)voxelCount / (float)voxelDataLength) * 10000);
	float val = (float)per * 0.01;

	return QString("Voxel count : %1(%2%)").arg(voxelCount).arg(QString::number(val, 'f', 2));
}

void MaskListWidget::dropEvent(QDropEvent* event)
{
	if (event->source() != this && !(event->possibleActions() & Qt::MoveAction))
		return;

	if (WIN_MANAGER->mainTabType == MAINTAB_REPORT)
		return;

	QTreeWidgetItem* target = itemAt(event->pos());
	mint32 index_dest = indexOfTopLevelItem(target);
	mint32 index_source = m_pVolumeData->getCurrentMaskInfoID();

	ACTION_MANAGER->action_MaskList_move(this, index_source, index_dest);
}

void MaskListWidget::UpdateByVolumeData()
{
	clear();

	if (!m_pVolumeData->isValidate())
		return;

	muint32 item_cnt = m_pVolumeData->getMaskInfoListCnt();
	QList<QTreeWidgetItem*> items;

	setItemsExpandable(false);
	for (int i = 0; i < item_cnt; ++i)
	{
		MaskInfo* info = m_pVolumeData->getMaskInfo(i);
		QTreeWidgetItem* item = insertRowItem(0, info);
	}

	if (item_cnt > 0)
	{
		QTreeWidgetItem* item = topLevelItem(m_pVolumeData->getCurrentMaskInfoID());
		if (item)
		{
			setRowItemBackgroundColor_Selected(item);
			setItemSelected(item, true);
		}
		else
		{
			m_pVolumeData->setCurrentMaskIndex(0);
			MaskInfo* info = m_pVolumeData->getMaskInfo(0);
			std::vector<muint32> vecSelect;
			vecSelect.push_back(info->uid);
			m_pVolumeData->setMultiSelectUID(vecSelect);
			item = topLevelItem(0);

			setRowItemBackgroundColor_Selected(item);
			setItemSelected(item, true);
		}

		if (item)
		{
			setCurrentItem(item);
		}
	}
}

void MaskListWidget::UpdateTAState(bool needUpdate, int uid)
{
	if (WIN_MANAGER->mainTabType != MAINTAB_TA)
		return;

	for (int i = 0; i < m_pVolumeData->getMaskInfoListCnt(); i++)
	{
		MaskInfo* info = m_pVolumeData->getMaskInfo(i);
		if (info)
		{
			if (uid == info->uid)
			{
				QTreeWidgetItem* item = topLevelItem(i);
				if (item)
					item->setTextColor(L_COL_NAME, needUpdate ? QColor(255, 0, 0) : QColor(255, 255, 255));
			}
		}
	}
}

void MaskListWidget::UpdateColumn(LAYER_COLUMN_AKA aka, bool res)
{
	if (aka == L_COL_SHOW)
	{
		for (int i = 0; i < m_pVolumeData->getMaskInfoListCnt(); i++)
		{
			MaskInfo* info = m_pVolumeData->getMaskInfo(i);
			if (info)
			{
				if (res != info->show)
				{
					info->show = res;
					QTreeWidgetItem* item = topLevelItem(i);
					if (item)
						item->setIcon(L_COL_SHOW, RESOURCE_MANAGER->getIcon(info->show ? ICON_LIST_VISIBLE : ICON_LIST_INVISIBLE));
				}

			}
		}
		WIN_MANAGER->forceUpdate2DViewData(false, true);
	}

	WIN_MANAGER->renderLater_GridView(true);
}

void MaskListWidget::UpdateSize(bool expand)
{
	const static QSize minSize = minimumSizeHint();

	if (expand)
	{
		setFixedHeight(SizeForRow().height() * (MASK_MAX + 1));
		setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	}
	else
	{
		setMinimumHeight(minSize.height());
		setMaximumHeight(SizeForRow().height() * (MASK_MAX + 1));
		setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	}
}

void MaskListWidget::SelectionUpdate(muint32 index_dest)
{
	QTreeWidgetItem* item;
	for (int n = 0; n < topLevelItemCount(); n++)
	{
		item = topLevelItem(n);
		if (index_dest == n || item->isSelected())
		{
			setRowItemBackgroundColor_Selected(item);
			if (!item->isSelected())
			{
				item->setSelected(true);
				setItemSelected(item, true);
			}
		}
		else
		{
			setRowItemBackgroundColor_UnSelected(item);

			item->setSelected(false);
			setItemSelected(item, false);
		}
	}

	if (currentItem() == NULL)
	{
		setCurrentItem(topLevelItem(index_dest));
	}

}

void MaskListWidget::SelectionMultiUpdate(const std::vector<muint32>& vecMaskUID)
{
	QTreeWidgetItem* item;
	for (int n = 0; n < topLevelItemCount(); n++)
	{
		MaskInfo* info = m_pVolumeData->getMaskInfo(n);

		bool bExist = false;
		for (auto i = 0; i < vecMaskUID.size(); ++i)
		{
			if (vecMaskUID[i] == info->uid)
			{
				bExist = true;
				break;
			}
		}

		item = topLevelItem(n);
		if (bExist)
		{
			setRowItemBackgroundColor_Selected(item);
			if (!item->isSelected())
			{
				item->setSelected(true);
				setItemSelected(item, true);
			}
			m_pVolumeData->setCurrentMaskIndex(n);
			WIN_MANAGER->settingOutset();
		}
		else
		{
			setRowItemBackgroundColor_UnSelected(item);
			item->setSelected(false);
			setItemSelected(item, false);
		}
	}
}

void MaskListWidget::SelectionRefresh(muint32 index_dest)
{
	muint32 item_cnt = m_pVolumeData->getMaskInfoListCnt();
	MaskInfo* info = m_pVolumeData->getMaskInfoByIndex(index_dest);
	if (NULL == info)
	{
		return;
	}

	QTreeWidgetItem* item = topLevelItem(index_dest);
	if (item == NULL)
	{
		item = insertRowItem(index_dest, info);
	}
	else
	{
		setRowItemDataByMaskInfo(item, info);
	}

	if (item_cnt > 0)
	{
		clearSelection();

		if (item)
		{
			setCurrentItem(item);
			setRowItemBackgroundColor_Selected(item);

			item->setSelected(true);
			setItemSelected(item, true);
			scrollToItem(item);
			SelectionUpdate(index_dest);
		}
		m_pVolumeData->setCurrentMaskIndex(index_dest);
		MaskInfo* info = m_pVolumeData->getMaskInfo(index_dest);
		std::vector<muint32> vecSelect;
		vecSelect.push_back(info->uid);
		m_pVolumeData->setMultiSelectUID(vecSelect);

		WIN_MANAGER->settingOutset();
	}
}

void MaskListWidget::VoxelRefresh(muint32 index)
{
	MaskInfo* info = m_pVolumeData->getMaskInfo(index, true);

	if (NULL == info)
		return;

	QTreeWidgetItem* item = topLevelItem(m_pVolumeData->getMaskIndex(index));

	if (NULL == item)
		return;

	item->setToolTip(L_COL_NAME, getVoxelInfoText(info));
}

void MaskListWidget::SetDisableAlpha(bool res)
{
	for (int i = 0; i < topLevelItemCount(); i++)
	{
		QTreeWidgetItem* item = topLevelItem(i);
		if (item->child(0))
		{
			item->child(0)->setHidden(res);
			if (res)
				item->setText(L_COL_ALPHA, QString("*"));
			else
			{
				if (item->isExpanded())
					item->setText(L_COL_ALPHA, QString("-"));
				else
					item->setText(L_COL_ALPHA, QString("+"));
			}
		}
	}
}

void MaskListWidget::SetDisplayTA(bool val)
{
	for (int i = 0; i < topLevelItemCount(); i++)
	{
		QTreeWidgetItem* item = topLevelItem(i);

		if (item->child(0))
		{
			muint32 uid = m_pVolumeData->getMaskInfo(i)->uid;
			bool needUpdate = val && (!m_pVolumeData->getTAState(uid));

			item->setTextColor(L_COL_NAME, needUpdate ? QColor(255, 0, 0) : QColor(255, 255, 255));
		}
	}
}

std::vector<muint32> MaskListWidget::GetSelectedItemIndices() const
{
	QList<QTreeWidgetItem*>& list = selectedItems();
	QList<QTreeWidgetItem*> selectedList;

	for (int i = 0; i < list.size(); i++)
	{
		QTreeWidgetItem* item = list.at(i);

		if (item->childCount() == 0)
			item = item->parent();

		muint32 index = indexOfTopLevelItem(item);

		if (!selectedList.contains(item))
			selectedList.push_back(item);
	}

	std::vector<muint32> selectedIndices;
	for (int i = 0; i < selectedList.size(); ++i)
	{
		int index = indexOfTopLevelItem(selectedList[i]);
		selectedIndices.push_back(index);
	}
	return selectedIndices;
}

void MaskListWidget::slot_OnMerge()
{
	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	QList<QTreeWidgetItem*>& list = selectedItems();
	QList<QTreeWidgetItem*> Mergelist;

	for (int i = 0; i < list.size(); i++)
	{
		QTreeWidgetItem* item = list.at(i);

		if (item->childCount() == 0)
			item = item->parent();

		muint32 index = indexOfTopLevelItem(item);

		if (!Mergelist.contains(item))
			Mergelist.push_back(item);
	}

	if (Mergelist.size() <= 1)
	{
		QMessageBox::warning(this, STRING_MANAGER->getString(STR_MERGE), STRING_MANAGER->getString(STR_SELECT_MORE));
		return;
	}

	std::vector<muint32> indeces;
	for (int cnt = 0; cnt < Mergelist.size(); cnt++)
	{
		int nIdx = indexOfTopLevelItem(Mergelist[cnt]);
		if (isVolumeDataMaskEditableByIndex(nIdx) == false)
			return;

		indeces.push_back(nIdx);
	}

	ACTION_MANAGER->action_MaskList_merge(DATA_CONTEXT, indeces);
}

void MaskListWidget::OnSplitRegion()
{
	if (isEditableMask() == false)
		return;

	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;

	}
	ACTION_MANAGER->m_qThreadNext.push_back(ActionThreadArgument(ACTP_MASK_LIST_SPLIT_REGION, THREAD_NONE, nullptr));
	ACTION_MANAGER->m_nCurrentThreadCount = ACTION_MANAGER->m_nMultiThreadTotalCount = ACTION_MANAGER->m_qThreadNext.size();
	ACTION_MANAGER->action_MaskList_splitRegion(/*WIN_MANAGER->getSelectedMask(), WIN_MANAGER->getSelectedMaskByteIndex()*/);
}

void MaskListWidget::OnInterPolation3D()
{
#if SUPPORT_3D_INTERPOLATION == 1
	if (isEditableMask() == false)
		return;

	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	Interpolation3dDlg dlg;

	if (dlg.exec() == QDialog::Accepted)
	{
	}
#endif
}

void MaskListWidget::slot_OnErosion()
{
	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	ACTION_MANAGER->action_ImageErosion(WIN_MANAGER->getSelectedMask(), WIN_MANAGER->getSelectedMaskByteIndex());
}

void MaskListWidget::slot_OnErosionLeft()
{
	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	ACTION_MANAGER->action_ImageErosion(WIN_MANAGER->getSelectedMask(), WIN_MANAGER->getSelectedMaskByteIndex(), DIRECTION_LEFT);
}
void MaskListWidget::slot_OnErosionRight()
{
	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	ACTION_MANAGER->action_ImageErosion(WIN_MANAGER->getSelectedMask(), WIN_MANAGER->getSelectedMaskByteIndex(), DIRECTION_RIGHT);
}
void MaskListWidget::slot_OnErosionPosterior()
{
	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	ACTION_MANAGER->action_ImageErosion(WIN_MANAGER->getSelectedMask(), WIN_MANAGER->getSelectedMaskByteIndex(), DIRECTION_POSTERIOR);
}
void MaskListWidget::slot_OnErosionAnterior()
{
	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	ACTION_MANAGER->action_ImageErosion(WIN_MANAGER->getSelectedMask(), WIN_MANAGER->getSelectedMaskByteIndex(), DIRECTION_ANTERIOR);
}
void MaskListWidget::slot_OnErosionSuperior()
{
	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	ACTION_MANAGER->action_ImageErosion(WIN_MANAGER->getSelectedMask(), WIN_MANAGER->getSelectedMaskByteIndex(), DIRECTION_SUPERIOR);
}
void MaskListWidget::slot_OnErosionInferior()
{
	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	ACTION_MANAGER->action_ImageErosion(WIN_MANAGER->getSelectedMask(), WIN_MANAGER->getSelectedMaskByteIndex(), DIRECTION_INFERIOR);
}

void MaskListWidget::slot_OnDilation()
{
	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	ACTION_MANAGER->action_ImageDilation(WIN_MANAGER->getSelectedMask(), WIN_MANAGER->getSelectedMaskByteIndex());
}
void MaskListWidget::slot_OnDilationLeft()
{
	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	ACTION_MANAGER->action_ImageDilation(WIN_MANAGER->getSelectedMask(), WIN_MANAGER->getSelectedMaskByteIndex(), DIRECTION_LEFT);
}
void MaskListWidget::slot_OnDilationRight()
{
	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	ACTION_MANAGER->action_ImageDilation(WIN_MANAGER->getSelectedMask(), WIN_MANAGER->getSelectedMaskByteIndex(), DIRECTION_RIGHT);
}
void MaskListWidget::slot_OnDilationPosterior()
{
	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	ACTION_MANAGER->action_ImageDilation(WIN_MANAGER->getSelectedMask(), WIN_MANAGER->getSelectedMaskByteIndex(), DIRECTION_POSTERIOR);
}
void MaskListWidget::slot_OnDilationAnterior()
{
	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	ACTION_MANAGER->action_ImageDilation(WIN_MANAGER->getSelectedMask(), WIN_MANAGER->getSelectedMaskByteIndex(), DIRECTION_ANTERIOR);
}
void MaskListWidget::slot_OnDilationSuperior()
{
	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	ACTION_MANAGER->action_ImageDilation(WIN_MANAGER->getSelectedMask(), WIN_MANAGER->getSelectedMaskByteIndex(), DIRECTION_SUPERIOR);
}
void MaskListWidget::slot_OnDilationInferior()
{
	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	ACTION_MANAGER->action_ImageDilation(WIN_MANAGER->getSelectedMask(), WIN_MANAGER->getSelectedMaskByteIndex(), DIRECTION_INFERIOR);
}

void MaskListWidget::OnImageCalculator()
{
	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	if (m_pDlgImageCalculator == nullptr)
	{
		m_pDlgImageCalculator = new ImageCalculatorDlg(this, this);
		setWindowFlags(Qt::WindowStaysOnTopHint);
	}

	std::vector<muint32> vecSelectedIdx = GetSelectedItemIndices();

	if (!vecSelectedIdx.empty())
	{
		m_pDlgImageCalculator->show();
		m_pDlgImageCalculator->raise();
		m_pDlgImageCalculator->activateWindow();
	}
	else
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), "Select at least 1 roi list");
		return;
	}
}

void MaskListWidget::OnMoveLeft()
{
	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	ACTION_MANAGER->action_ImageMove(WIN_MANAGER->getSelectedMask(), WIN_MANAGER->getSelectedMaskByteIndex(), DIRECTION_LEFT);
}

void MaskListWidget::OnMoveRight()
{
	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	ACTION_MANAGER->action_ImageMove(WIN_MANAGER->getSelectedMask(), WIN_MANAGER->getSelectedMaskByteIndex(), DIRECTION_RIGHT);
}

void MaskListWidget::OnMovePosterior()
{
	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	ACTION_MANAGER->action_ImageMove(WIN_MANAGER->getSelectedMask(), WIN_MANAGER->getSelectedMaskByteIndex(), DIRECTION_POSTERIOR);
}

void MaskListWidget::OnMoveAnterior()
{
	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	ACTION_MANAGER->action_ImageMove(WIN_MANAGER->getSelectedMask(), WIN_MANAGER->getSelectedMaskByteIndex(), DIRECTION_ANTERIOR);
}

void MaskListWidget::OnMoveSuperior()
{
	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	ACTION_MANAGER->action_ImageMove(WIN_MANAGER->getSelectedMask(), WIN_MANAGER->getSelectedMaskByteIndex(), DIRECTION_SUPERIOR);
}

void MaskListWidget::OnMoveInferior()
{
	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	ACTION_MANAGER->action_ImageMove(WIN_MANAGER->getSelectedMask(), WIN_MANAGER->getSelectedMaskByteIndex(), DIRECTION_INFERIOR);
}

void MaskListWidget::OnSetSeedPoint()
{
	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	//int nMaskIdx = m_pVolumeData->getCurrentMaskIndex();
	//MaskInfo* pMaskInfo = m_pVolumeData->getMaskInfo(nMaskIdx);
	MaskInfo* pMaskInfo = m_pVolumeData->getCurrentMaskInfo();
	mask8 maskBit = m_pVolumeData->getMask(pMaskInfo->uid);

	int byteIdx = m_pVolumeData->GetMaskByteIndex(pMaskInfo->uid);
	mask8* pMaskData = m_pVolumeData->getMaskDataPoint(byteIdx);

	mint16* pHUData = m_pVolumeData->getHUDataPoint();

	muint32 cx = m_pVolumeData->getCX();
	muint32 cy = m_pVolumeData->getCY();
	muint32 cz = m_pVolumeData->getCZ();
	muint32 length = m_pVolumeData->getVolumeDataLength();

	for (int z = 0; z < cz; ++z)
	{
		for (int y = 0; y < cy; ++y)
		{
			for (int x = 0; x < cx; ++x)
			{
				int index = z * cx * cy + y * cx + x;

				if (pMaskData[index] & maskBit)
				{
					WIN_MANAGER->seedLocation.push_back(QVector3D(x, y, z));
					int HU = m_pVolumeData->getData(index);
				}
			}
		}
	}

	if (WIN_MANAGER->GetTab())
	{
		ThreSholdTab* tab = WIN_MANAGER->GetTab()->getThreSholdTab();

		if (tab)
			//tab->m_rgTab->UpdateRGList();
			tab->UpdateRGList();
	}

	WIN_MANAGER->renderLater_GridView(false);
}

void MaskListWidget::On3DFilling()
{
	if (!WIN_MANAGER->IsLicensePass())
	{
		//		QMessageBox::warning(this, tr("Not Supported"), tr("Supported features in current version.\n(Edu version or later is supported.)"));
		//		return;
	}

	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	MaskInfo* info = m_pVolumeData->getCurrentMaskInfo();
	int uid = 0;
	if (info)
		uid = info->uid;

	ACTION_MANAGER->action_Image_Hole_Filling(uid, WIN_MANAGER->getSelectedMask(), WIN_MANAGER->getSelectedMaskByteIndex(), mip::HoleFilling::Mode::Mode3D);
}

void MaskListWidget::On2DFilling()
{
	if (!WIN_MANAGER->IsLicensePass())
	{
		//		QMessageBox::warning(this, tr("Not Supported"), tr("Supported features in current version.\n(Edu version or later is supported.)"));
		//		return;
	}

	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	MaskInfo* info = m_pVolumeData->getCurrentMaskInfo();
	int uid = 0;
	if (info)
		uid = info->uid;

	ACTION_MANAGER->action_Image_Hole_Filling(uid, WIN_MANAGER->getSelectedMask(), WIN_MANAGER->getSelectedMaskByteIndex(), mip::HoleFilling::Mode::Mode2DPlaneWhole);
}

void MaskListWidget::OnHoleFillingWhole()
{
	if (!WIN_MANAGER->IsLicensePass())
	{
		//		QMessageBox::warning(this, tr("Not Supported"), tr("Supported features in current version.\n(Edu version or later is supported.)"));
		//		return;
	}

	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	MaskInfo* info = m_pVolumeData->getCurrentMaskInfo();
	int uid = 0;
	if (info)
		uid = info->uid;

	ACTION_MANAGER->action_Image_Hole_Filling(uid, WIN_MANAGER->getSelectedMask(), WIN_MANAGER->getSelectedMaskByteIndex(), mip::HoleFilling::Mode::Mode2DPlaneWhole);
}

void MaskListWidget::OnHoleFillingAxial()
{
	if (!WIN_MANAGER->IsLicensePass())
	{
		// 		QMessageBox::warning(this, tr("Not Supported"), tr("Supported features in current version.\n(Edu version or later is supported.)"));
		// 		return;
	}

	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	MaskInfo* info = m_pVolumeData->getCurrentMaskInfo();
	int uid = 0;
	if (info)
		uid = info->uid;

	ACTION_MANAGER->action_Image_Hole_Filling(uid, WIN_MANAGER->getSelectedMask(), WIN_MANAGER->getSelectedMaskByteIndex(), mip::HoleFilling::Mode::Mode2DPlaneAxial);
}

void MaskListWidget::OnHoleFillingCoronal()
{
	if (!WIN_MANAGER->IsLicensePass())
	{
		// 		QMessageBox::warning(this, tr("Not Supported"), tr("Supported features in current version.\n(Edu version or later is supported.)"));
		// 		return;
	}

	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	MaskInfo* info = m_pVolumeData->getCurrentMaskInfo();
	int uid = 0;
	if (info)
		uid = info->uid;

	ACTION_MANAGER->action_Image_Hole_Filling(uid, WIN_MANAGER->getSelectedMask(), WIN_MANAGER->getSelectedMaskByteIndex(), mip::HoleFilling::Mode::Mode2DPlaneCoronal);
}

void MaskListWidget::OnHoleFillingSagittal()
{
	if (!WIN_MANAGER->IsLicensePass())
	{
		// 		QMessageBox::warning(this, tr("Not Supported"), tr("Supported features in current version.\n(Edu version or later is supported.)"));
		// 		return;
	}

	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	MaskInfo* info = m_pVolumeData->getCurrentMaskInfo();
	int uid = 0;
	if (info)
		uid = info->uid;

	ACTION_MANAGER->action_Image_Hole_Filling(uid, WIN_MANAGER->getSelectedMask(), WIN_MANAGER->getSelectedMaskByteIndex(), mip::HoleFilling::Mode::Mode2DPlaneSagittal);
}

void MaskListWidget::OnComponentChoice()
{
	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	ComponentDlg dlg;

	if (dlg.exec() == QDialog::Accepted)
	{
		int num = dlg.getClusterNum();

		MaskInfo* pMaskInfo = m_pVolumeData->getCurrentMaskInfo();

		ACTION_MANAGER->action_ImageComponentChoise(m_pVolumeData->getMask(pMaskInfo->uid)
			, m_pVolumeData->GetMaskByteIndex(pMaskInfo->uid), num);

	}

}

void MaskListWidget::OnVesselness()
{
	if (!WIN_MANAGER->IsLicensePass())
	{
		QMessageBox::warning(this, tr("Not Supported"), tr("Supported features in current version.\n(Edu version or later is supported.)"));
		return;
	}

	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	ACTION_MANAGER->action_ImageVesselness(WIN_MANAGER->getSelectedMask(), WIN_MANAGER->getSelectedMaskByteIndex());
}

void MaskListWidget::OnFissureness()
{
	if (!WIN_MANAGER->IsLicensePass())
	{
		QMessageBox::warning(this, tr("Not Supported"), tr("Supported features in current version.\n(Edu version or later is supported.)"));
		return;
	}

	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	ACTION_MANAGER->action_ImageFissureness(WIN_MANAGER->getSelectedMask(), WIN_MANAGER->getSelectedMaskByteIndex());
}

void MaskListWidget::OnSimilarity()
{
	if (!WIN_MANAGER->IsLicensePass())
	{
		QMessageBox::warning(this, tr("Not Supported"), tr("Supported features in current version.\n(Edu version or later is supported.)"));
		return;
	}

	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	QList<QTreeWidgetItem*>& list = selectedItems();
	QList<QTreeWidgetItem*> Simlist;

	for (int i = 0; i < list.size(); i++)
	{
		QTreeWidgetItem* item = list.at(i);

		if (item->childCount() == 0)
			item = item->parent();

		muint32 index = indexOfTopLevelItem(item);

		if (!Simlist.contains(item))
			Simlist.push_back(item);
	}

	if (Simlist.size() != 2)
	{
		QMessageBox::warning(this, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(STR_SELECT_2_WARN));
		return;
	}

	int count = m_pVolumeData->getMaskInfoListCnt();
	if (count < 2)
		return;

	mint32 indeces[2];
	int cnt = 0;

	for (cnt = 0; cnt < Simlist.size(); cnt++)
		indeces[cnt] = indexOfTopLevelItem(Simlist[cnt]);

	MaskInfo* A = m_pVolumeData->getMaskInfo(indeces[0]);
	MaskInfo* B = m_pVolumeData->getMaskInfo(indeces[1]);
	int mAI = A->uid >= MASK_SECOND_MAX ? (A->uid - MASK_SECOND_MAX) / 8 + 1 : 0;
	int mBI = B->uid >= MASK_SECOND_MAX ? (B->uid - MASK_SECOND_MAX) / 8 + 1 : 0;

	unsigned char mA = mAI != 0 ? A->mask_id2 : A->mask_id;
	unsigned char mB = mBI != 0 ? B->mask_id2 : B->mask_id;

	m_pVolumeData->createTempMaskData(true, -1, 0);
	m_pVolumeData->createTempMaskData(true, -1, 1);

	m_pVolumeData->setTempMaskBitFromData(mA, mAI);
	m_pVolumeData->setTempMaskBitFromData(mB, mBI, 1, 1);

	int width, height;

	width = m_pVolumeData->getCX();
	height = m_pVolumeData->getCY();
	cnt = m_pVolumeData->getCZ();

	float diceRes = mip::TA::GetDiceSimilarityCoefficient(m_pVolumeData->getMaskTempDataPoint(0), m_pVolumeData->getMaskTempDataPoint(1), width, height, cnt);
	int res = (int)((float)diceRes * 10000);
	diceRes = (float)res * 0.01;

	float TaniRes = mip::TA::GetTanimotoCoefficient(m_pVolumeData->getMaskTempDataPoint(0), m_pVolumeData->getMaskTempDataPoint(1), width, height, cnt);

	float VolRes = mip::TA::GetVolumeOverlapPercentage(m_pVolumeData->getMaskTempDataPoint(0), m_pVolumeData->getMaskTempDataPoint(1), width, height, cnt);


	QMessageBox::warning(NULL, "Result", QString("DICE similarity : %1%\nTanimoto : %2\nVolumeOverlap : %3").arg(diceRes).arg(TaniRes).arg(VolRes));


}


void MaskListWidget::OnHausdorff()
{
	if (!WIN_MANAGER->IsLicensePass())
	{
		QMessageBox::warning(this, tr("Not Supported"), tr("Supported features in current version.\n(Edu version or later is supported.)"));
		return;
	}

	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	QList<QTreeWidgetItem*>& list = selectedItems();
	QList<QTreeWidgetItem*> Simlist;

	for (int i = 0; i < list.size(); i++)
	{
		QTreeWidgetItem* item = list.at(i);

		if (item->childCount() == 0)
			item = item->parent();

		muint32 index = indexOfTopLevelItem(item);

		if (!Simlist.contains(item))
			Simlist.push_back(item);
	}

	if (Simlist.size() != 2)
	{
		QMessageBox::warning(this, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(STR_SELECT_2_WARN));
		return;
	}

	int count = m_pVolumeData->getMaskInfoListCnt();
	if (count < 2)
		return;

	mint32 indeces[2];
	int cnt = 0;

	for (cnt = 0; cnt < Simlist.size(); cnt++)
		indeces[cnt] = indexOfTopLevelItem(Simlist[cnt]);

	MaskInfo* A = m_pVolumeData->getMaskInfo(indeces[0]);
	MaskInfo* B = m_pVolumeData->getMaskInfo(indeces[1]);
	int mAI = A->uid >= MASK_SECOND_MAX ? (A->uid - MASK_SECOND_MAX) / 8 + 1 : 0;
	int mBI = B->uid >= MASK_SECOND_MAX ? (B->uid - MASK_SECOND_MAX) / 8 + 1 : 0;

	unsigned char mA = mAI != 0 ? A->mask_id2 : A->mask_id;
	unsigned char mB = mBI != 0 ? B->mask_id2 : B->mask_id;

	m_pVolumeData->createTempMaskData(true, -1, 0);
	m_pVolumeData->createTempMaskData(true, -1, 1);

	m_pVolumeData->setTempMaskBitFromData(mA, mAI);
	m_pVolumeData->setTempMaskBitFromData(mB, mBI, 1, 1);

	int width, height;

	width = m_pVolumeData->getCX();
	height = m_pVolumeData->getCY();
	cnt = m_pVolumeData->getCZ();
	// 
	// 	float diceRes = mip::TA::GetDiceSimilarityCoefficient(m_pVolumeData->getMaskTempDataPoint(0), m_pVolumeData->getMaskTempDataPoint(1), width, height, cnt);
	// 	int res = (int)((float)diceRes * 10000);
	// 	diceRes = (float)res * 0.01;
	// 
	// 	float TaniRes = mip::TA::GetTanimotoCoefficient(m_pVolumeData->getMaskTempDataPoint(0), m_pVolumeData->getMaskTempDataPoint(1), width, height, cnt);
	// 
	// 	float VolRes = mip::TA::GetVolumeOverlapPercentage(m_pVolumeData->getMaskTempDataPoint(0), m_pVolumeData->getMaskTempDataPoint(1), width, height, cnt);
	double hausdorff_euclidean = 0, hausdorff_spacing = 0;

	bool HausdorffDistanceRes = mip::TA::GetHausdorffDistance(m_pVolumeData->getMaskTempDataPoint(0), m_pVolumeData->getMaskTempDataPoint(1), width, height, cnt, m_pVolumeData->getSpaceX(true), m_pVolumeData->getSpaceY(true), m_pVolumeData->getSpaceZ(true), hausdorff_euclidean, hausdorff_spacing);

	QMessageBox::warning(NULL, "Result", QString("Hausdorff Distance(euclidean) : %1\nHausdorff Distance(spacing) : %2mm").arg(hausdorff_euclidean).arg(hausdorff_spacing));


}



void MaskListWidget::OnRandomPoint()
{
	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	QList<QTreeWidgetItem*>& list = selectedItems();

	if (list.size() != 1)
	{
		QMessageBox::warning(this, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(STR_ONE_SELECT));
		return;
	}


	muint32 index = indexOfTopLevelItem(list.at(0));
	MaskInfo* info = m_pVolumeData->getMaskInfo(index);

	if (info)
	{
		int cnt;
		if ((cnt = m_pVolumeData->getVoxelCount(info->uid)) < 2)
		{
			QMessageBox::warning(this, QString("Voxel count"), QString("This function works only on layers with 2 or more voxels."));
			return;
		}

		NumDialog dlg(this);

		if (dlg.exec() == QDialog::Accepted)
		{
			dlg.getNum();


			ACTION_MANAGER->action_RandomSelect(dlg.m_number_of_point, dlg.enablePointOnly, dlg.m_x_diameter_min, dlg.m_x_diameter_max, dlg.m_y_diameter_min, dlg.m_y_diameter_max, dlg.m_z_diameter_min, dlg.m_z_diameter_max, dlg.m_mean_min_HU, dlg.m_mean_max_HU, dlg.m_boundary_min_HU, dlg.m_boundary_max_HU, dlg.cavityMode, dlg.m_cavity_boundary, info->uid);

		}
	}
}

void MaskListWidget::OnFeatureExtractor()
{
	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	if (!WIN_MANAGER->IsLicensePass())
	{
		QMessageBox::warning(this, tr("Not Supported"), tr("Supported features in current version.\n(Edu version or later is supported.)"));
		return;
	}

	QList<QTreeWidgetItem*>& list = selectedItems();
	QList<QTreeWidgetItem*> Extrlist;

	for (int i = 0; i < list.size(); i++)
	{
		QTreeWidgetItem* item = list.at(i);

		if (item->childCount() == 0)
			item = item->parent();

		muint32 index = indexOfTopLevelItem(item);

		if (!Extrlist.contains(item))
			Extrlist.push_back(item);
	}

	MaskInfo* info = NULL;

	if (Extrlist.size() > 1)
	{
		WIN_MANAGER->exportList.clear();
		ACTION_MANAGER->SetAfterThread(THREAD_TA_UPDATE);
	}
	else if (Extrlist.size() < 1)
	{
		info = m_pVolumeData->getCurrentMaskInfo();
	}

	for (int i = 1; i < Extrlist.size(); i++)
	{
		MaskInfo* _info = m_pVolumeData->getMaskInfo(indexOfTopLevelItem(Extrlist[i]));

		if (_info)
		{
			WIN_MANAGER->exportList.push_back(_info->uid);
		}
	}

	if (Extrlist.size() != 0)
	{
		info = m_pVolumeData->getMaskInfo(indexOfTopLevelItem(Extrlist[0]));
	}

	int mI = info->uid >= MASK_SECOND_MAX ? (info->uid - MASK_SECOND_MAX) / 8 + 1 : 0;

	ACTION_MANAGER->action_ImageFeatureExtractor_Add(mI == 0 ? info->mask_id : info->mask_id2, mI, info->uid);

}

void MaskListWidget::OnPreviewSurface()
{
	if (m_pVolumeData->isValidate() == false)
	{
		return;
	}

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	MaskInfo* info = NULL;
	QList<QTreeWidgetItem*>& list = selectedItems();
	QList<QTreeWidgetItem*> Prelist;

	for (int i = 0; i < list.size(); i++)
	{
		QTreeWidgetItem* item = list.at(i);

		if (item->childCount() == 0)
			item = item->parent();

		muint32 index = indexOfTopLevelItem(item);

		if (!Prelist.contains(item))
			Prelist.push_back(item);
	}

	// 200828 허 건 대리
	//if (Prelist.size() > 1)
	//{
	//	WIN_MANAGER->exportList.clear();
	//	ACTION_MANAGER->bAfterThread = THREAD_PREVIEW_MESHES;
	//}
	//else if (Prelist.size() < 1)
	//{
	//	info = m_pVolumeData->getCurrentMaskInfo();
	//}

	//for (int i = 1; i < Prelist.size(); i++)
	//{
	//	MaskInfo *_info = m_pVolumeData->getMaskInfo(indexOfTopLevelItem(Prelist[i]));

	//	if (_info)
	//	{
	//		WIN_MANAGER->exportList.push_back(_info->uid);
	//	}
	//}

	//20201112_byPHS - Dlg reject
	WIN_MANAGER->rejectMEViewDlg();

	WIN_MANAGER->exportList.clear();

	for (int i = 0; i < Prelist.size(); i++)
	{
		int			pos_list = indexOfTopLevelItem(Prelist[i]);

		MaskInfo* _info = m_pVolumeData->getMaskInfo(pos_list);

		if (_info)
		{
			//WIN_MANAGER->exportList.push_back(_info->uid);
			WIN_MANAGER->exportList.push_back(pos_list);
		}
	}

	if (Prelist.size() != 0)
		info = m_pVolumeData->getMaskInfo(indexOfTopLevelItem(Prelist[0]));

	int mI = info->uid >= MASK_SECOND_MAX ? (info->uid - MASK_SECOND_MAX) / 8 + 1 : 0;

	bool r = WIN_MANAGER->previewSurface(info->meshConnected ? info->uid : -1, mI == 0 ? info->mask_id : info->mask_id2, mI);

	if (r == false)
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_PREVIEW_SURFACE), STRING_MANAGER->getString(STR_FAILED_TO)
			+ STRING_MANAGER->getString(STR_PREVIEW_SURFACE));
	}
}

void MaskListWidget::slot_OnExportSTLFile()
{
	ExportMaskLayerToFile(
		STRING_MANAGER->getString(STR_EXPORT_STL),
		EX_FILES_STL,
		tr("STL File(*.stl;*.STL)")
	);
}

void MaskListWidget::slot_OnExportSTLFilePatientCoordinate()
{
	ExportMaskLayerToFile(
		STRING_MANAGER->getString(STR_EXPORT_STL),
		EX_FILES_STL,
		tr("STL File(*.stl;*.STL)"),
		true
	);
}

void MaskListWidget::slot_OnExportOBJFile()
{
	ExportMaskLayerToFile(
		STRING_MANAGER->getString(STR_EXPORT_OBJ),
		EX_FILES_OBJ,
		tr("OBJ File(*.obj;*.OBJ)")
	);
}

void MaskListWidget::slot_OnExportOBJFilePatientCoordinate()
{
	ExportMaskLayerToFile(
		STRING_MANAGER->getString(STR_EXPORT_OBJ),
		EX_FILES_OBJ,
		tr("OBJ File(*.obj;*.OBJ)"),
		true
	);
}

void MaskListWidget::OnExportRAWFile()
{
	ExportMaskLayerToFile(
		STRING_MANAGER->getString(STR_EXPORT_RAW),
		EX_FILES_RAW,
		tr("Raw File(*.raw;*.RAW)")
	);
}

void MaskListWidget::OnExportHURAWFile()
{
	ExportMaskLayerToFile(
		STRING_MANAGER->getString(STR_EXPORT_HURAW),
		EX_FILES_HU_RAW,
		tr("HU Raw File(*.raw;*.RAW)")
	);
}

void MaskListWidget::slot_OnExportVTKFile()
{
	ExportMaskLayerToFile(
		STRING_MANAGER->getString(STR_EXPORT_VTK),
		EX_FILES_VTK,
		tr("VTK File(*.vtk;*.VTK)")
	);
}

void MaskListWidget::slot_OnExportVTKFilePatientCoordinate()
{
	ExportMaskLayerToFile(
		STRING_MANAGER->getString(STR_EXPORT_VTK),
		EX_FILES_VTK,
		tr("VTK File(*.vtk;*.VTK)"),
		true
	);
}

void MaskListWidget::OnExportROINIIFile()
{
	ExportMaskLayerToFile(
		STRING_MANAGER->getString(STR_EXPORT_NII),
		EX_FILES_NII,
		tr("ROI NII File(*.nii;*.NII)")
	);
}

void MaskListWidget::OnExportCOORDINATENIIFile()
{
	ExportMaskLayerToFile(
		STRING_MANAGER->getString(STR_EXPORT_NII),
		EX_FILES_COORDINATE_NII,
		tr("Coordnate NII File(*.nii;*.NII)")
	);
}

void MaskListWidget::OnExportHUNIIFile()
{
	ExportMaskLayerToFile(
		STRING_MANAGER->getString(STR_EXPORT_NII),
		EX_FILES_HUNII,
		tr("HU NII File(*.nii;*.NII)")
	);
}

void MaskListWidget::OnExportTXTFile()
{
	ExportMaskLayerToFile(
		STRING_MANAGER->getString(STR_EXPORT_TXT),
		EX_FILES_TXT,
		tr("TXT File(*.txt;*.TXT)")
	);
}

void MaskListWidget::ExportMaskLayerToFile(QString caption, EXPORT_FILES exportFileType, QString filter, bool bPatientCoordinate)
{
	if (m_pVolumeData->isValidate() == false)
	{
		//Skip
		return;
	}

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	if (WIN_MANAGER->IsLicensePass() == false)
	{
		QMessageBox::warning(NULL, caption,
			STRING_MANAGER->getString(STR_FAILED_TO) + caption +
			STRING_MANAGER->getString(STR_LICENSE_WARN));

		return;
	}

	QList<QTreeWidgetItem*> exportItemList = GetExportItemList();

	QString lastestDirPath;
	bool latest = WIN_MANAGER->lastestPathGet(lastestDirPath, true);
	if (!latest)
	{
		lastestDirPath = "";
	}

	bool exportResult = false;
	if (exportItemList.size() > 1)
	{
		exportResult = ExportMultiFile(caption, lastestDirPath, exportItemList, exportFileType, bPatientCoordinate);
	}
	else
	{
		exportResult = ExportSingleFile(caption, lastestDirPath, exportFileType, filter, bPatientCoordinate);
	}

	if (!exportResult)
	{
		QMessageBox::warning(NULL,
			caption,
			STRING_MANAGER->getString(STR_FAILED_TO) + caption);
	}
}

QList<QTreeWidgetItem*> MaskListWidget::GetExportItemList()
{
	QList<QTreeWidgetItem*>& selectedItemList = selectedItems();
	QList<QTreeWidgetItem*> exportItemList;

	for (int i = 0; i < selectedItemList.size(); i++)
	{
		QTreeWidgetItem* item = selectedItemList.at(i);

		if (item->childCount() == 0)
			item = item->parent();

		muint32 index = indexOfTopLevelItem(item);

		if (!exportItemList.contains(item))
			exportItemList.push_back(item);
	}

	return exportItemList;
}

bool MaskListWidget::ExportMultiFile(QString caption, QString lastestDirPath, QList<QTreeWidgetItem*> exportItemList, EXPORT_FILES exportFileType, bool bPatientCoordinate)
{
	/* export List, Thread 정보를 초기화 */
	WIN_MANAGER->exportList.clear();
	ACTION_MANAGER->SetAfterThread(THREAD_EXPORT_FILES);

	QString dirpath = ExportDirectoryDialog(
		this, caption, lastestDirPath
	);

	if (dirpath.isEmpty())
	{
		/* Skip */
		return true;
	}

	/* Thread 완료 후 진행할 Mask를 추가 */
	WIN_MANAGER->exportPath = dirpath;

	MaskInfo* pMaskInfo = NULL;
	for (int i = 1; i < exportItemList.size(); i++)
	{
		pMaskInfo = m_pVolumeData->getMaskInfo(indexOfTopLevelItem(exportItemList[i]));
		if (pMaskInfo)
		{
			WIN_MANAGER->exportList.push_back(pMaskInfo->uid);
		}
	}

	pMaskInfo = m_pVolumeData->getMaskInfo(indexOfTopLevelItem(exportItemList[0]));

	//%dirpath%/%filename.%으로 설정한 후 exportFileType extension Append 
	QString fileName = m_pVolumeData->getMaskName(pMaskInfo->uid, true);
	QString filePath = dirpath + "/" + fileName + ".";

	int mI = pMaskInfo->uid >= MASK_SECOND_MAX ? (pMaskInfo->uid - MASK_SECOND_MAX) / 8 + 1 : 0;
	bool result = WIN_MANAGER->saveFiles(filePath, pMaskInfo->uid, mI == 0 ? pMaskInfo->mask_id : pMaskInfo->mask_id2, mI, exportFileType, bPatientCoordinate);

	return result;
}

bool MaskListWidget::ExportSingleFile(QString caption, QString lastestDirPath, EXPORT_FILES exportFileType, QString filter, bool bPatientCoordinate)
{
	MaskInfo* pMaskInfo = m_pVolumeData->getCurrentMaskInfo();

	QString	filePath = ExportFileDialog(
		this,
		caption,
		m_pVolumeData->getMaskName(pMaskInfo->uid, true),
		lastestDirPath,
		filter,
		QFlag(QFileDialog::ShowDirsOnly)
	);

	if (filePath.isEmpty())
	{
		/* Skip */
		return true;
	}

	int mI = pMaskInfo->uid >= MASK_SECOND_MAX ? (pMaskInfo->uid - MASK_SECOND_MAX) / 8 + 1 : 0;
	bool result = WIN_MANAGER->saveFiles(filePath, pMaskInfo->uid, mI == 0 ? pMaskInfo->mask_id : pMaskInfo->mask_id2, mI, exportFileType, bPatientCoordinate, false);

	return result;
}


void MaskListWidget::slot_OnCopyToForeSeed()
{
	if (m_context)
	{
		m_context->hide();
		m_context = NULL;
	}

	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	muint32 index = this->indexOfTopLevelItem(currentItem()->childCount() == 0 ? currentItem()->parent() : currentItem());
	MaskInfo* info = m_pVolumeData->getMaskInfo(index);
	int mI = info->uid >= MASK_SECOND_MAX ? (info->uid - MASK_SECOND_MAX) / 8 + 1 : 0;
	unsigned char m = info->uid >= MASK_SECOND_MAX ? info->mask_id2 : info->mask_id;

	ACTION_MANAGER->action_DrawSeedFormMask(m, true, mI);
}

void MaskListWidget::slot_OnCopyToBackSeed()
{
	if (m_context)
	{
		m_context->hide();
		m_context = NULL;
	}

	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	muint32 index = this->indexOfTopLevelItem(currentItem()->childCount() == 0 ? currentItem()->parent() : currentItem());
	MaskInfo* info = m_pVolumeData->getMaskInfo(index);
	int mI = info->uid >= MASK_SECOND_MAX ? (info->uid - MASK_SECOND_MAX) / 8 + 1 : 0;
	unsigned char m = info->uid >= MASK_SECOND_MAX ? info->mask_id2 : info->mask_id;

	ACTION_MANAGER->action_DrawSeedFormMask(m, false, mI);
}

void MaskListWidget::OnToDrawSeed()
{
	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	muint32 index = this->indexOfTopLevelItem(currentItem()->childCount() == 0 ? currentItem()->parent() : currentItem());
	MaskInfo* info = m_pVolumeData->getMaskInfo(index);

	if (info)
		ACTION_MANAGER->action_MaskList_As_Drawseed(info->uid);

}

void MaskListWidget::slot_OnXFlip()
{
	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	QList<QTreeWidgetItem*>& list = selectedItems();

	if (list.size() != 1)
	{
		QMessageBox::warning(this, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(STR_ONE_SELECT) +
			STRING_MANAGER->getString(STR_INVERSE));
		return;
	}

	muint32 index = this->indexOfTopLevelItem(currentItem()->childCount() == 0 ? currentItem()->parent() : currentItem());
	MaskInfo* info = m_pVolumeData->getMaskInfo(index);
	int mI = info->uid >= MASK_SECOND_MAX ? (info->uid - MASK_SECOND_MAX) / 8 + 1 : 0;

	ACTION_MANAGER->action_MaskList_XFlip(*info, mI);
}


void MaskListWidget::slot_OnYFlip()
{
	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	QList<QTreeWidgetItem*>& list = selectedItems();

	if (list.size() != 1)
	{
		QMessageBox::warning(this, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(STR_ONE_SELECT) +
			STRING_MANAGER->getString(STR_INVERSE));
		return;
	}

	muint32 index = this->indexOfTopLevelItem(currentItem()->childCount() == 0 ? currentItem()->parent() : currentItem());
	MaskInfo* info = m_pVolumeData->getMaskInfo(index);
	int mI = info->uid >= MASK_SECOND_MAX ? (info->uid - MASK_SECOND_MAX) / 8 + 1 : 0;

	ACTION_MANAGER->action_MaskList_YFlip(*info, mI);
}



void MaskListWidget::slot_OnZFlip()
{
	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	QList<QTreeWidgetItem*>& list = selectedItems();

	if (list.size() != 1)
	{
		QMessageBox::warning(this, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(STR_ONE_SELECT) +
			STRING_MANAGER->getString(STR_INVERSE));
		return;
	}

	muint32 index = this->indexOfTopLevelItem(currentItem()->childCount() == 0 ? currentItem()->parent() : currentItem());
	MaskInfo* info = m_pVolumeData->getMaskInfo(index);
	int mI = info->uid >= MASK_SECOND_MAX ? (info->uid - MASK_SECOND_MAX) / 8 + 1 : 0;

	ACTION_MANAGER->action_MaskList_ZFlip(*info, mI);
}

void MaskListWidget::OnLevelset()
{
	if (!WIN_MANAGER->IsLicensePass())
	{
		QMessageBox::warning(this, tr("Not Supported"), tr("Supported features in current version.\n(Edu version or later is supported.)"));
		return;
	}

	if (m_pVolumeData->isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	QList<QTreeWidgetItem*>& list = selectedItems();

	if (list.size() != 1)
	{
		QMessageBox::warning(this, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(STR_ONE_SELECT) +
			STRING_MANAGER->getString(STR_INVERSE));
		return;
	}

	muint32 index = this->indexOfTopLevelItem(currentItem());
	MaskInfo* info = m_pVolumeData->getMaskInfo(index);
	int mI = info->uid >= MASK_SECOND_MAX ? (info->uid - MASK_SECOND_MAX) / 8 + 1 : 0;

	LevelsetDialog dlg(this);

	dlg.exec();

	if (dlg.isAccept())
		ACTION_MANAGER->action_MaskList_Levelset(mI == 0 ? info->mask_id : info->mask_id2, mI, dlg.getIter(), dlg.getLambda(), dlg.getRadious());
	else
		QMessageBox::warning(this, STRING_MANAGER->getString(STR_WARN), "Invalid format.");

}

void MaskListWidget::OnComputeCofusionMtx()
{
	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	QList<QTreeWidgetItem*>& list = selectedItems();
	if (list.size() <= 1)
	{
		QMessageBox::warning(this, QString("Compute Confusion Matrix"), STRING_MANAGER->getString(STR_SELECT_MORE));
		return;
	}
	std::vector<muint32> indeces;
	for (int cnt = 0; cnt < list.size(); cnt++)
	{
		indeces.push_back(indexOfTopLevelItem(list[cnt]));
	}

	if (m_pDlgConfusionInfo == nullptr)
		m_pDlgConfusionInfo = new ConfusionMatrixInfoDialog(this);

	ACTION_MANAGER->action_MaskList_compute_comfusionMtx(indeces, *m_pDlgConfusionInfo);
	m_pDlgConfusionInfo->calculateInfo();
	m_pDlgConfusionInfo->show();


}

/*
@brief		복셀 부피 비율 계산(211209 허 건 과장)
@return		없음
*/
void		MaskListWidget::OnCalculateVoxelRatio()
{
	CVoxelCountRatioDlg* dlg = new CVoxelCountRatioDlg(
		QString("Voxel Ratio Calculator"),
		WIN_MANAGER->mainWindow,
		WIN_MANAGER->GetTab()->getROITab(),
		m_pVolumeData
	);

	dlg->exec();
}


