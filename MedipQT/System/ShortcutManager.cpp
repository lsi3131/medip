#include "stdafx.h"
#include "ShortcutManager.h"
#include "ProductManager.h"
#include "StringManager.h"
#include "ActionManager.h"
#include <qaction>
#include <qwidget>
#include <qkeysequence>

ShortcutManager::ShortcutManager() :
	m_mainWidget(nullptr),

	m_action_UndoStack_Undo_Main(nullptr),
	m_action_UndoStack_Redo_Main(nullptr),

	m_action_Import_Open(nullptr),
	m_action_Import_ReverseOpen(nullptr),
	m_action_Import_MultiOpen(nullptr),

	m_action_Export_Save(nullptr),
	m_action_Export_SaveAs(nullptr),

	m_action_Common_BreakLock(nullptr),
	m_action_Common_BeginScreen(nullptr),
	m_action_Common_Delete(nullptr),
	m_action_Common_ESC(nullptr),
	m_action_Common_PagePrev(nullptr),
	m_action_Common_PageNext(nullptr),
	m_action_Common_Info(nullptr),

	m_action_Layer_General_Clear(nullptr),
	m_action_Layer_General_New(nullptr),
	m_action_Layer_General_Rename(nullptr),
	m_action_Layer_General_Duplicate(nullptr),
	m_action_Layer_General_Delete(nullptr),
	m_action_Layer_General_Visible(nullptr),

	m_action_Layer_3DHoleFilling(nullptr),

	m_action_Layer_2DHoleFilling_AxialPlane(nullptr),
	m_action_Layer_2DHoleFilling_CoronalPlane(nullptr),
	m_action_Layer_2DHoleFilling_SagittalPlane(nullptr),
	m_action_Layer_2DHoleFilling_WholePlane(nullptr),

	m_action_Layer_Boolean_Intersection(nullptr),
	m_action_Layer_Boolean_Merge(nullptr),
	m_action_Layer_Boolean_FFS(nullptr),

	m_action_Layer_Split_Region(nullptr),
	m_action_Layer_Make_Polyhedron(nullptr),
	m_action_Layer_Erosion_Left(nullptr),
	m_action_Layer_Erosion_Right(nullptr),
	m_action_Layer_Erosion_Anterior(nullptr),
	m_action_Layer_Erosion_Posterior(nullptr),
	m_action_Layer_Erosion_Inferior(nullptr),
	m_action_Layer_Erosion_Superior(nullptr),
	m_action_Layer_Erosion_6_Connectivity(nullptr),

	m_action_Layer_Dilation_Left(nullptr),
	m_action_Layer_Dilation_Right(nullptr),
	m_action_Layer_Dilation_Posterior(nullptr),
	m_action_Layer_Dilation_Anterior(nullptr),
	m_action_Layer_Dilation_Superior(nullptr),
	m_action_Layer_Dilation_Inferior(nullptr),
	m_action_Layer_Dilation_6_Connectivity(nullptr),

	m_action_Layer_Image_Calculator(nullptr),

	m_action_Layer_Move_Mask_Left(nullptr),
	m_action_Layer_Move_Mask_Right(nullptr),
	m_action_Layer_Move_Mask_Posterior(nullptr),
	m_action_Layer_Move_Mask_Anterior(nullptr),
	m_action_Layer_Move_Mask_Superior(nullptr),
	m_action_Layer_Move_Mask_Inferior(nullptr),

	m_action_ManualDrawing_PolygonSelection(nullptr),
	m_action_ManualDrawing_RegionSelection(nullptr),
	m_action_ManualDrawing_PixelWisedSelection(nullptr),
	m_action_ManualDrawing_OvalSelection(nullptr),
	m_action_ManualDrawing_AngularSelection(nullptr),
	m_action_ManualDrawing_Less(nullptr),
	m_action_ManualDrawing_More(nullptr),


	m_action_VolumeProcessing_FlipLayer(nullptr),

	m_action_VolumeHomePosition_Anterior(nullptr),
	m_action_VolumeHomePosition_Posterior(nullptr),
	m_action_VolumeHomePosition_Right(nullptr),
	m_action_VolumeHomePosition_Left(nullptr),
	m_action_VolumeHomePosition_Superior(nullptr),
	m_action_VolumeHomePosition_Inferior(nullptr),

	m_action_WindowBasic_FullScreen(nullptr),
	m_action_WindowBasic_BoundaryViewer(nullptr),

	m_action_SemiAutoSeg_DrawCutMode(nullptr),
	m_action_SemiAutoSeg_DrawCutApply(nullptr),

	m_action_WorkingRegion(nullptr),

	m_action_Threshold_WithinBrush(nullptr),
	m_action_Threshold_Preview(nullptr)
{
}

void ShortcutManager::Remove_Action_UndoStack_Main()
{
	m_mainWidget->removeAction(m_action_UndoStack_Undo_Main);
	m_mainWidget->removeAction(m_action_UndoStack_Redo_Main);
}

void ShortcutManager::Add_Action_UndoStack_Main()
{
	m_mainWidget->addAction(m_action_UndoStack_Undo_Main);
	m_mainWidget->addAction(m_action_UndoStack_Redo_Main);
}

void ShortcutManager::Remove_All_Action()
{
	for (auto& pAction : m_actionList)
	{
		m_mainWidget->removeAction(pAction);
	}
}

void ShortcutManager::Add_All_Action()
{
	for (auto& pAction : m_actionList)
	{
		m_mainWidget->addAction(pAction);
	}
}

bool ShortcutManager::Initialize(QWidget* mainWidget)
{
	m_mainWidget = mainWidget;

	/* ======== UndoStack(Main) =========*/
	if (Create_And_AddAction_With_UndoStack(
		&m_action_UndoStack_Undo_Main,
		"&Undo",
		QKeySequence(Qt::CTRL + Qt::Key_Z),
		ACTION_MANAGER->getUndoStack(), true) == false)
	{
		return false;
	}

	if (Create_And_AddAction_With_UndoStack(
		&m_action_UndoStack_Redo_Main,
		"&Redo",
		QKeySequence(Qt::CTRL + Qt::Key_Y),
		ACTION_MANAGER->getUndoStack(), false) == false)
	{
		return false;
	}

	/* ======== Import =========*/
	if (Create_And_AddAction(
		&m_action_Import_Open,
		"&Open...",
		QKeySequence(Qt::CTRL + Qt::Key_O)) == false)
	{
		return false;
	}

	if (Create_And_AddAction(
		&m_action_Import_ReverseOpen,
		"&Reverse Open...",
		QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_O)) == false)
	{
		return false;
	}

	if (Create_And_AddAction(
		&m_action_Import_MultiOpen,
		"&Multi-Open...",
		QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_O)) == false)
	{
		return false;
	}
	/* ======== Export =========*/

	if (Create_And_AddAction(
		&m_action_Export_Save,
		"&Save...",
		QKeySequence(Qt::CTRL + Qt::Key_S)) == false)
	{
		return false;
	}

	if (Create_And_AddAction(
		&m_action_Export_SaveAs,
		"&Save As...",
		QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_S)) == false)
	{
		return false;
	}

	/* ======== Common =========*/
	if (Create_And_AddAction(
		&m_action_Common_BreakLock,
		"&Break lock...",
		QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_Q)) == false)
	{
		return false;
	}

	if (Create_And_AddAction(
		&m_action_Common_BeginScreen,
		"&Break lock...",
		QKeySequence(Qt::CTRL + Qt::Key_0)) == false)
	{
		return false;
	}

	if (Create_And_AddAction(
		&m_action_Common_Delete,
		"&Delete...",
		QKeySequence::Delete) == false)
	{
		return false;
	}

	if (Create_And_AddAction(
		&m_action_Common_ESC,
		"&Escape...",
		QKeySequence::Cancel) == false)
	{
		return false;
	}

	if (Create_And_AddAction(
		&m_action_Common_PagePrev,
		"&PagePrev...",
		QKeySequence(Qt::Key_PageUp)) == false)
	{
		return false;
	}

	if (Create_And_AddAction(
		&m_action_Common_PageNext,
		"&PageNext...",
		QKeySequence(Qt::Key_PageDown)) == false)
	{
		return false;
	}

	if (Create_And_AddAction(
		&m_action_Common_Info,
		"&Info...",
		QKeySequence(Qt::Key_F1)) == false)
	{
		return false;
	}


	/* ======== Layer(General) =========*/
	if (Create_And_AddAction_With_FunctionLevel(
		&m_action_Layer_General_Clear,
		STRING_MANAGER->getString(STR_CLR_ROI),
		QKeySequence(Qt::ALT + Qt::Key_C),
		MFL_Common_LayerOperation_GeneralFunction) == false)
	{
		return false;
	}

	if (Create_And_AddAction_With_FunctionLevel(
		&m_action_Layer_General_New,
		STRING_MANAGER->getString(STR_NEW),
		QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_N),
		MFL_Common_LayerOperation_GeneralFunction) == false)
	{
		return false;
	}

	if (Create_And_AddAction_With_FunctionLevel(
		&m_action_Layer_General_Rename,
		STRING_MANAGER->getString(STR_RENAME),
		QKeySequence(Qt::Key_F2),
		MFL_Common_LayerOperation_GeneralFunction) == false)
	{
		return false;
	}
	if (Create_And_AddAction_With_FunctionLevel(
		&m_action_Layer_General_Duplicate,
		STRING_MANAGER->getString(STR_COPY),
		QKeySequence(Qt::CTRL + Qt::Key_J),
		MFL_Common_LayerOperation_GeneralFunction) == false)
	{
		return false;
	}

	if (Create_And_AddAction_With_FunctionLevel(
		&m_action_Layer_General_Delete,
		STRING_MANAGER->getString(STR_DELETE),
		QKeySequence(),		/* Delete 단축키만 eventFilter에서 처리 중임. 확인될 때까지 비워둘 것 */
		MFL_Common_LayerOperation_GeneralFunction) == false)
	{
		return false;
	}

	if (Create_And_AddAction(
		&m_action_Layer_General_Visible,
		"&Visible layers...",
		QKeySequence(Qt::Key_Slash)) == false)
	{
		return false;
	}

	/* ======== Layer(3D Hole Filling) =========*/
	if (Create_And_AddAction_With_FunctionLevel(
		&m_action_Layer_3DHoleFilling,
		STRING_MANAGER->getString(STR_HOLEFILLING_ROI),
		QKeySequence(Qt::CTRL + Qt::Key_H),
		MFL_Common_LayerOperation_3DHoleFilling) == false)
	{
		return false;
	}

	/* ======== Layer(2D Hole Filling) =========*/
	if (Create_And_AddAction_With_FunctionLevel(
		&m_action_Layer_2DHoleFilling_AxialPlane,
		"Axial Plane",
		QKeySequence(Qt::SHIFT + Qt::Key_A),
		MFL_Common_LayerOperation_2DHoleFilling) == false)
	{
		return false;
	}

	if (Create_And_AddAction_With_FunctionLevel(
		&m_action_Layer_2DHoleFilling_CoronalPlane,
		"Coronal Plane",
		QKeySequence(Qt::SHIFT + Qt::Key_C),
		MFL_Common_LayerOperation_2DHoleFilling) == false)
	{
		return false;
	}

	if (Create_And_AddAction_With_FunctionLevel(
		&m_action_Layer_2DHoleFilling_SagittalPlane,
		"Sagittal Plane",
		QKeySequence(Qt::SHIFT + Qt::Key_S),
		MFL_Common_LayerOperation_2DHoleFilling) == false)
	{
		return false;
	}

	if (Create_And_AddAction_With_FunctionLevel(
		&m_action_Layer_2DHoleFilling_WholePlane,
		"Whole Combination",
		QKeySequence(Qt::SHIFT + Qt::Key_W),
		MFL_Common_LayerOperation_2DHoleFilling) == false)
	{
		return false;
	}

	/* ======== Layer(Boolean) =========*/
	if (Create_And_AddAction_With_FunctionLevel(
		&m_action_Layer_Boolean_Intersection,
		STRING_MANAGER->getString(STR_INTERSECTION_ROI),
		QKeySequence(Qt::SHIFT + Qt::Key_I),
		MFL_Common_LayerOperation_LayerBooleanFunction_Intersection) == false)
	{
		return false;
	}

	if (Create_And_AddAction_With_FunctionLevel(
		&m_action_Layer_Boolean_Merge,
		STRING_MANAGER->getString(STR_MERGE),
		QKeySequence(Qt::CTRL + Qt::Key_M),
		MFL_Common_LayerOperation_LayerBooleanFunction_merge) == false)
	{
		return false;
	}

	if (Create_And_AddAction_With_FunctionLevel(
		&m_action_Layer_Boolean_FFS,
		STRING_MANAGER->getString(STR_FFS_ROI),
		QKeySequence(Qt::SHIFT + Qt::Key_F),
		MFL_Common_LayerOperation_LayerBooleanFunction_First_First_Second) == false)
	{
		return false;
	}

	/* ======== Layer(Split Region) =========*/
	if (Create_And_AddAction_With_FunctionLevel(
		&m_action_Layer_Split_Region,
		STRING_MANAGER->getString(STR_SPLIT_REGION),
		QKeySequence(Qt::CTRL + Qt::Key_L),
		MFL_Common_LayerOperation_MaskSplitRegion) == false)
	{
		return false;
	}

#if SUPPORT_3D_INTERPOLATION == 1
	/* ======== Layer(3D Interpolation) =========*/
	if (Create_And_AddAction_With_FunctionLevel(
		&m_action_Layer_Make_Polyhedron,
		STRING_MANAGER->getString(STR_3D_INTERPOLATION),
		QKeySequence(),
		MFL_Common_LayerOperation_3DInterpolation) == false)
	{
		return false;
	}
#endif

	/* ======== Layer(Inverse) =========*/
	if (Create_And_AddAction_With_FunctionLevel(
		&m_action_Layer_Inverse,
		STRING_MANAGER->getString(STR_INVERSE),
		QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_I),
		MFL_Common_LayerOperation_Inverse) == false)
	{
		return false;
	}

	/* ======== Layer(Erosion) =========*/
	if (Create_And_AddAction_With_FunctionLevel(
		&m_action_Layer_Erosion_Left,
		"Left erosion",
		QKeySequence(Qt::ALT + Qt::Key_Left),
		MFL_Common_LayerOperation_ImageErosion) == false)
	{
		return false;
	}

	if (Create_And_AddAction_With_FunctionLevel(
		&m_action_Layer_Erosion_Right,
		"Right erosion",
		QKeySequence(Qt::ALT + Qt::Key_Right),
		MFL_Common_LayerOperation_ImageErosion) == false)
	{
		return false;
	}

	if (Create_And_AddAction_With_FunctionLevel(
		&m_action_Layer_Erosion_Posterior,
		"Posterior Erosion",
		QKeySequence(Qt::ALT + Qt::Key_Down),
		MFL_Common_LayerOperation_ImageErosion) == false)
	{
		return false;
	}

	if (Create_And_AddAction_With_FunctionLevel(
		&m_action_Layer_Erosion_Anterior,
		"Anterior Erosion",
		QKeySequence(Qt::ALT + Qt::Key_Up),
		MFL_Common_LayerOperation_ImageErosion) == false)
	{
		return false;
	}

	if (Create_And_AddAction_With_FunctionLevel(
		&m_action_Layer_Erosion_Superior,
		"Superior Erosion",
		QKeySequence(Qt::ALT + Qt::Key_PageUp),
		MFL_Common_LayerOperation_ImageErosion) == false)
	{
		return false;
	}

	if (Create_And_AddAction_With_FunctionLevel(
		&m_action_Layer_Erosion_Inferior,
		"Interior Erosion",
		QKeySequence(Qt::ALT + Qt::Key_PageDown),
		MFL_Common_LayerOperation_ImageErosion) == false)
	{
		return false;
	}

	if (Create_And_AddAction_With_FunctionLevel(
		&m_action_Layer_Erosion_6_Connectivity,
		"6-connectivity",
		QKeySequence(Qt::CTRL + Qt::Key_E),
		MFL_Common_LayerOperation_ImageErosion) == false)
	{
		return false;
	}

	/* ======== Layer(Dilation) =========*/
	if (Create_And_AddAction_With_FunctionLevel(
		&m_action_Layer_Dilation_Left,
		"Left dilation",
		QKeySequence(Qt::CTRL + Qt::Key_Left),
		MFL_Common_LayerOperation_ImageDilation) == false)
	{
		return false;
	}

	if (Create_And_AddAction_With_FunctionLevel(
		&m_action_Layer_Dilation_Right,
		"Right dilation",
		QKeySequence(Qt::CTRL + Qt::Key_Right),
		MFL_Common_LayerOperation_ImageDilation) == false)
	{
		return false;
	}

	if (Create_And_AddAction_With_FunctionLevel(
		&m_action_Layer_Dilation_Posterior,
		"Posterior dilation",
		QKeySequence(Qt::CTRL + Qt::Key_Down),
		MFL_Common_LayerOperation_ImageDilation) == false)
	{
		return false;
	}

	if (Create_And_AddAction_With_FunctionLevel(
		&m_action_Layer_Dilation_Anterior,
		"Anterior dilation",
		QKeySequence(Qt::CTRL + Qt::Key_Up),
		MFL_Common_LayerOperation_ImageDilation) == false)
	{
		return false;
	}

	if (Create_And_AddAction_With_FunctionLevel(
		&m_action_Layer_Dilation_Superior,
		"Superior dilation",
		QKeySequence(Qt::CTRL + Qt::Key_PageUp),
		MFL_Common_LayerOperation_ImageDilation) == false)
	{
		return false;
	}

	if (Create_And_AddAction_With_FunctionLevel(
		&m_action_Layer_Dilation_Inferior,
		"Interior dilation",
		QKeySequence(Qt::CTRL + Qt::Key_PageDown),
		MFL_Common_LayerOperation_ImageDilation) == false)
	{
		return false;
	}

	if (Create_And_AddAction_With_FunctionLevel(
		&m_action_Layer_Dilation_6_Connectivity,
		"6-connectivity",
		QKeySequence(Qt::CTRL + Qt::Key_D),
		MFL_Common_LayerOperation_ImageDilation) == false)
	{
		return false;
	}

	if (Create_And_AddAction(
		&m_action_Layer_Image_Calculator,
		"Image Calculator",
		QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_C)
	) == false)
	{
		return false;
	}

	/* ======== Layer(Move Mask) =========*/
	if (Create_And_AddAction_With_FunctionLevel(
		&m_action_Layer_Move_Mask_Left,
		"Left direction",
		QKeySequence(Qt::SHIFT + Qt::Key_Left),
		MFL_Common_LayerOperation_MoveMask) == false)
	{
		return false;
	}

	if (Create_And_AddAction_With_FunctionLevel(
		&m_action_Layer_Move_Mask_Right,
		"Right direction",
		QKeySequence(Qt::SHIFT + Qt::Key_Right),
		MFL_Common_LayerOperation_MoveMask) == false)
	{
		return false;
	}

	if (Create_And_AddAction_With_FunctionLevel(
		&m_action_Layer_Move_Mask_Posterior,
		"Posterior direction",
		QKeySequence(Qt::SHIFT + Qt::Key_PageDown),
		MFL_Common_LayerOperation_MoveMask) == false)
	{
		return false;
	}

	if (Create_And_AddAction_With_FunctionLevel(
		&m_action_Layer_Move_Mask_Anterior,
		"Anterior direction",
		QKeySequence(Qt::SHIFT + Qt::Key_PageUp),
		MFL_Common_LayerOperation_MoveMask) == false)
	{
		return false;
	}

	if (Create_And_AddAction_With_FunctionLevel(
		&m_action_Layer_Move_Mask_Superior,
		"Superior direction",
		QKeySequence(Qt::SHIFT + Qt::Key_Up),
		MFL_Common_LayerOperation_MoveMask) == false)
	{
		return false;
	}

	if (Create_And_AddAction_With_FunctionLevel(
		&m_action_Layer_Move_Mask_Inferior,
		"Inferior direction",
		QKeySequence(Qt::SHIFT + Qt::Key_Down),
		MFL_Common_LayerOperation_MoveMask) == false)
	{
		return false;
	}

	/* ======== Menutal Drawing ======== */
	if (Create_And_AddAction(
		&m_action_ManualDrawing_PolygonSelection,
		"Polygon Selection",
		QKeySequence(Qt::Key_1)) == false)
	{
		return false;
	}

	if (Create_And_AddAction(
		&m_action_ManualDrawing_RegionSelection,
		"Region Selection",
		QKeySequence(Qt::Key_2)) == false)
	{
		return false;
	}

	if (Create_And_AddAction(
		&m_action_ManualDrawing_PixelWisedSelection,
		"Pixel Wised Selection",
		QKeySequence(Qt::Key_3)) == false)
	{
		return false;
	}

	if (Create_And_AddAction(
		&m_action_ManualDrawing_OvalSelection,
		"Oval Selection",
		QKeySequence(Qt::Key_4)) == false)
	{
		return false;
	}

	if (Create_And_AddAction(
		&m_action_ManualDrawing_AngularSelection,
		"Angular Selection",
		QKeySequence(Qt::Key_5)) == false)
	{
		return false;
	}

	if (Create_And_AddAction(
		&m_action_ManualDrawing_Less,
		"&Less",
		QKeySequence(Qt::Key_BracketLeft)) == false)
	{
		return false;
	}

	if (Create_And_AddAction(
		&m_action_ManualDrawing_More,
		"&More",
		QKeySequence(Qt::Key_BracketRight)) == false)
	{
		return false;
	}


	/* ======== Volume Processing ======== */
	if (Create_And_AddAction(
		&m_action_VolumeProcessing_FlipLayer,
		"Flip Layer",
		QKeySequence(Qt::CTRL + Qt::Key_W)) == false)
	{
		return false;
	}

	/* ======== Volume Home Position ======== */
	if (Create_And_AddAction_With_FunctionLevel(
		&m_action_VolumeHomePosition_Anterior,
		"&Anterior",
		QKeySequence(Qt::Key_A),
		MFL_Common_Rendering_3DVolumeViewer) == false)
	{
		return false;
	}

	if (Create_And_AddAction_With_FunctionLevel(
		&m_action_VolumeHomePosition_Posterior,
		"&Posterior",
		QKeySequence(Qt::Key_P),
		MFL_Common_Rendering_3DVolumeViewer) == false)
	{
		return false;
	}

	if (Create_And_AddAction_With_FunctionLevel(
		&m_action_VolumeHomePosition_Right,
		"&Right",
		QKeySequence(Qt::Key_R),
		MFL_Common_Rendering_3DVolumeViewer) == false)
	{
		return false;
	}

	if (Create_And_AddAction_With_FunctionLevel(
		&m_action_VolumeHomePosition_Left,
		"&Left",
		QKeySequence(Qt::Key_L),
		MFL_Common_Rendering_3DVolumeViewer) == false)
	{
		return false;
	}

	if (Create_And_AddAction_With_FunctionLevel(
		&m_action_VolumeHomePosition_Superior,
		"&Superior",
		QKeySequence(Qt::Key_S),
		MFL_Common_Rendering_3DVolumeViewer) == false)
	{
		return false;
	}

	if (Create_And_AddAction_With_FunctionLevel(
		&m_action_VolumeHomePosition_Inferior,
		"&Inferior",
		QKeySequence(Qt::Key_I),
		MFL_Common_Rendering_3DVolumeViewer) == false)
	{
		return false;
	}


	/* ======== WindowBasic ======== */
	if (Create_And_AddAction(
		&m_action_WindowBasic_FullScreen,
		"Full Screen",
		QKeySequence(Qt::Key_Space)) == false)
	{
		return false;
	}

	if (Create_And_AddAction(
		&m_action_WindowBasic_BoundaryViewer,
		"Boundary Viewer",
		QKeySequence(Qt::Key_O)) == false)
	{
		return false;
	}


	/* ======== Semi-Auto Segmentation ======== */
	if (Create_And_AddAction(
		&m_action_SemiAutoSeg_DrawCutMode,
		"Draw Cut Mode",
		QKeySequence(Qt::Key_D)) == false)
	{
		return false;
	}

	if (Create_And_AddAction(
		&m_action_SemiAutoSeg_DrawCutApply,
		"Draw Cut Apply",
		QKeySequence(Qt::CTRL + Qt::Key_Return)) == false)
	{
		return false;
	}

	/* ======== Working Region ======== */
	if (Create_And_AddAction(
		&m_action_WorkingRegion,
		"Working Region",
		QKeySequence(Qt::Key_T)) == false)
	{
		return false;
	}

	/* ======== Threshold ======== */
	if (Create_And_AddAction(
		&m_action_Threshold_WithinBrush,
		"Within Brush",
		QKeySequence(Qt::Key_W)) == false)
	{
		return false;
	}

	if (Create_And_AddAction(
		&m_action_Threshold_Preview,
		"Show Preview",
		QKeySequence(Qt::Key_V)) == false)
	{
		return false;
	}


	return true;
}

QAction * ShortcutManager::Create_Action_UndoStack_Undo_Main()
{
	if (Create_And_AddAction_With_UndoStack(
		&m_action_UndoStack_Undo_Main,
		"&Undo",
		QKeySequence(Qt::CTRL + Qt::Key_Z),
		ACTION_MANAGER->getUndoStack(), true) == false)
	{
		return nullptr;
	}
	return m_action_UndoStack_Undo_Main;
}

QAction * ShortcutManager::Create_Action_UndoStack_Redo_Main()
{
	if (Create_And_AddAction_With_UndoStack(
		&m_action_UndoStack_Redo_Main,
		"&Redo",
		QKeySequence(Qt::CTRL + Qt::Key_Y),
		ACTION_MANAGER->getUndoStack(), false) == false)
	{
		return nullptr;
	}

	return m_action_UndoStack_Redo_Main;
}

bool ShortcutManager::Create_And_AddAction(QAction** ppAction, QString text, QKeySequence keysequnce)
{
	*ppAction = new QAction(m_mainWidget);
	if (*ppAction == nullptr)
	{
		/* Just Skip */
		return true;
	}
	return AddAction_To_MainWidget(*ppAction, text, keysequnce);
}

bool ShortcutManager::Create_And_AddAction_With_FunctionLevel(QAction** ppAction, QString text, QKeySequence keysequnce, eMEDIP_FUNCTION_LEVEL functionLevel)
{
	*ppAction = PRODUCT_FACTORY->createObject<QAction>(functionLevel, m_mainWidget);
	if (*ppAction == nullptr)
	{
		/* Just Skip */
		return true;
	}
	return AddAction_To_MainWidget(*ppAction, text, keysequnce);
}

bool ShortcutManager::Create_And_AddAction_With_UndoStack(QAction** ppAction, QString text, QKeySequence keysequnce, QUndoStack* undoStack, bool undo)
{
	if (undo)
	{
		*ppAction = undoStack->createUndoAction(m_mainWidget, text);
	}
	else
	{
		*ppAction = undoStack->createRedoAction(m_mainWidget, text);
	}

	if (*ppAction == nullptr)
	{
		/* Just Skip */
		return true;
	}

	return AddAction_To_MainWidget(*ppAction, text, keysequnce);
}


bool ShortcutManager::AddAction_To_MainWidget(QAction* pAction, QString text, QKeySequence keysequnce)
{
	pAction->setText(text);
	pAction->setShortcut(keysequnce);
	m_mainWidget->addAction(pAction);
	return true;
}

bool ShortcutManager::AddAllAction_To_ActionList()
{
	AddActionToActionList(m_action_Import_Open);
	AddActionToActionList(m_action_Import_ReverseOpen);
	AddActionToActionList(m_action_Import_MultiOpen);

	AddActionToActionList(m_action_Export_Save);
	AddActionToActionList(m_action_Export_SaveAs);

	AddActionToActionList(m_action_Common_BreakLock);
	AddActionToActionList(m_action_Common_BeginScreen);
	AddActionToActionList(m_action_Common_Delete);
	AddActionToActionList(m_action_Common_ESC);
	AddActionToActionList(m_action_Common_PagePrev);
	AddActionToActionList(m_action_Common_PageNext);
	AddActionToActionList(m_action_Common_Info);

	AddActionToActionList(m_action_Layer_General_Clear);
	AddActionToActionList(m_action_Layer_General_New);
	AddActionToActionList(m_action_Layer_General_Rename);
	AddActionToActionList(m_action_Layer_General_Duplicate);
	AddActionToActionList(m_action_Layer_General_Delete);
	AddActionToActionList(m_action_Layer_General_Visible);

	AddActionToActionList(m_action_Layer_3DHoleFilling);

	AddActionToActionList(m_action_Layer_2DHoleFilling_AxialPlane);
	AddActionToActionList(m_action_Layer_2DHoleFilling_CoronalPlane);
	AddActionToActionList(m_action_Layer_2DHoleFilling_SagittalPlane);
	AddActionToActionList(m_action_Layer_2DHoleFilling_WholePlane);

	AddActionToActionList(m_action_Layer_Boolean_Intersection);
	AddActionToActionList(m_action_Layer_Boolean_Merge);
	AddActionToActionList(m_action_Layer_Boolean_FFS);

	AddActionToActionList(m_action_Layer_Split_Region);

	AddActionToActionList(m_action_Layer_Inverse);


	AddActionToActionList(m_action_Layer_Erosion_Left);
	AddActionToActionList(m_action_Layer_Erosion_Right);
	AddActionToActionList(m_action_Layer_Erosion_Posterior);
	AddActionToActionList(m_action_Layer_Erosion_Anterior);
	AddActionToActionList(m_action_Layer_Erosion_Superior);
	AddActionToActionList(m_action_Layer_Erosion_Inferior);
	AddActionToActionList(m_action_Layer_Erosion_6_Connectivity);

	AddActionToActionList(m_action_Layer_Dilation_Left);
	AddActionToActionList(m_action_Layer_Dilation_Right);
	AddActionToActionList(m_action_Layer_Dilation_Posterior);
	AddActionToActionList(m_action_Layer_Dilation_Anterior);
	AddActionToActionList(m_action_Layer_Dilation_Superior);
	AddActionToActionList(m_action_Layer_Dilation_Inferior);
	AddActionToActionList(m_action_Layer_Dilation_6_Connectivity);

	AddActionToActionList(m_action_Layer_Image_Calculator);

	AddActionToActionList(m_action_Layer_Move_Mask_Left);
	AddActionToActionList(m_action_Layer_Move_Mask_Right);
	AddActionToActionList(m_action_Layer_Move_Mask_Posterior);
	AddActionToActionList(m_action_Layer_Move_Mask_Anterior);
	AddActionToActionList(m_action_Layer_Move_Mask_Superior);
	AddActionToActionList(m_action_Layer_Move_Mask_Inferior);

	AddActionToActionList(m_action_ManualDrawing_PolygonSelection);
	AddActionToActionList(m_action_ManualDrawing_RegionSelection);
	AddActionToActionList(m_action_ManualDrawing_PixelWisedSelection);
	AddActionToActionList(m_action_ManualDrawing_OvalSelection);
	AddActionToActionList(m_action_ManualDrawing_AngularSelection);
	AddActionToActionList(m_action_ManualDrawing_Less);
	AddActionToActionList(m_action_ManualDrawing_More);

	AddActionToActionList(m_action_VolumeProcessing_FlipLayer);

	AddActionToActionList(m_action_VolumeHomePosition_Anterior);
	AddActionToActionList(m_action_VolumeHomePosition_Posterior);
	AddActionToActionList(m_action_VolumeHomePosition_Right);
	AddActionToActionList(m_action_VolumeHomePosition_Left);
	AddActionToActionList(m_action_VolumeHomePosition_Superior);
	AddActionToActionList(m_action_VolumeHomePosition_Inferior);

	AddActionToActionList(m_action_WindowBasic_FullScreen);
	AddActionToActionList(m_action_WindowBasic_BoundaryViewer);

	AddActionToActionList(m_action_SemiAutoSeg_DrawCutMode);
	AddActionToActionList(m_action_SemiAutoSeg_DrawCutApply);

	AddActionToActionList(m_action_WorkingRegion);

	AddActionToActionList(m_action_Threshold_WithinBrush);
	AddActionToActionList(m_action_Threshold_Preview);

	return true;
}

bool ShortcutManager::AddActionToActionList(QAction* pAction)
{
	if (pAction)
	{
		m_actionList.push_back(pAction);
	}
	return true;
}

void SetEnable_Action(QAction* action, bool enabled)
{
	if (action)
	{
		action->setEnabled(enabled);
	}
}

