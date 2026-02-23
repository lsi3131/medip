#pragma once

#ifndef SHORTCUT_MANAGER_H
#define SHORTCUT_MANAGER_H

#define SHORTCUT_MANAGER ShortcutManager::GetInstance()

#include <qstring>
#include <qkeysequence>
#include "MedipType.h"

class QAction;
class QWidget;

class ShortcutManager
{
public:
	static ShortcutManager* GetInstance()
	{
		static ShortcutManager instance;
		return &instance;
	}

public:
	ShortcutManager();

public:
	bool Initialize(QWidget* mainWidget);

public:
	QAction* Create_Action_UndoStack_Undo_Main();
	QAction* Create_Action_UndoStack_Redo_Main();

	void Remove_Action_UndoStack_Main();
	void Add_Action_UndoStack_Main();

	void Remove_All_Action();
	void Add_All_Action();

	QAction* Action_UndoStack_Undo_Main() { return m_action_UndoStack_Undo_Main; }
	QAction* Action_UndoStack_Redo_Main() { return m_action_UndoStack_Redo_Main; }

	QAction* Action_Import_Open() { return m_action_Import_Open; }
	QAction* Action_Import_ReverseOpen() { return m_action_Import_ReverseOpen; }
	QAction* Action_Import_MultiOpen() { return m_action_Import_MultiOpen; }

	QAction* Action_Export_Save() { return m_action_Export_Save; }
	QAction* Action_Export_SaveAs() { return m_action_Export_SaveAs; }

	QAction* Action_Common_BreakLock() { return m_action_Common_BreakLock; }
	QAction* Action_Common_BeginScreen() { return m_action_Common_BeginScreen; }
	QAction* Action_Common_Delete() { return m_action_Common_Delete; }
	QAction* Action_Common_ESC() { return m_action_Common_ESC; }
	QAction* Action_Common_PagePrev() {return m_action_Common_PagePrev;}
	QAction* Action_Common_PageNext(){ return m_action_Common_PageNext; }
	QAction* Action_Common_Info(){ return m_action_Common_Info; }

	QAction* Action_Layer_General_Clear() { return m_action_Layer_General_Clear; }
	QAction* Action_Layer_General_New() { return m_action_Layer_General_New; }
	QAction* Action_Layer_General_Rename() { return m_action_Layer_General_Rename; }
	QAction* Action_Layer_General_Duplicate() { return m_action_Layer_General_Duplicate; }
	QAction* Action_Layer_General_Delete() { return m_action_Layer_General_Delete; }
	QAction* Action_Layer_General_Visible() { return m_action_Layer_General_Visible; }

	QAction* Action_Layer_3DHoleFilling() { return m_action_Layer_3DHoleFilling; }

	QAction* Action_Layer_2DHoleFilling_AxialPlane() { return m_action_Layer_2DHoleFilling_AxialPlane; }
	QAction* Action_Layer_2DHoleFilling_CoronalPlane() { return m_action_Layer_2DHoleFilling_CoronalPlane; }
	QAction* Action_Layer_2DHoleFilling_SagittalPlane() { return m_action_Layer_2DHoleFilling_SagittalPlane; }
	QAction* Action_Layer_2DHoleFilling_WholePlane() { return m_action_Layer_2DHoleFilling_WholePlane; }

	QAction* Action_Layer_Boolean_Intersection() { return m_action_Layer_Boolean_Intersection; }
	QAction* Action_Layer_Boolean_Merge() { return m_action_Layer_Boolean_Merge; }
	QAction* Action_Layer_Boolean_FFS() { return m_action_Layer_Boolean_FFS; }

	QAction* Action_Layer_Split_Region() { return m_action_Layer_Split_Region; }
	QAction* Action_Layer_3D_Interpolation() { return m_action_Layer_Make_Polyhedron; }

	QAction* Action_Layer_Inverse() { return m_action_Layer_Inverse; }

	QAction* Action_Layer_Erosion_Left() { return m_action_Layer_Erosion_Left; }
	QAction* Action_Layer_Erosion_Right() { return m_action_Layer_Erosion_Right; }
	QAction* Action_Layer_Erosion_Anterior() { return m_action_Layer_Erosion_Anterior; }
	QAction* Action_Layer_Erosion_Posterior() { return m_action_Layer_Erosion_Posterior; }
	QAction* Action_Layer_Erosion_Inferior() { return m_action_Layer_Erosion_Inferior; }
	QAction* Action_Layer_Erosion_Superior() { return m_action_Layer_Erosion_Superior; }
	QAction* Action_Layer_Erosion_6_Connectivity() { return m_action_Layer_Erosion_6_Connectivity; }

	QAction* Action_Layer_Dilation_Left() { return m_action_Layer_Dilation_Left; }
	QAction* Action_Layer_Dilation_Right() { return m_action_Layer_Dilation_Right; }
	QAction* Action_Layer_Dilation_Anterior() { return m_action_Layer_Dilation_Anterior; }
	QAction* Action_Layer_Dilation_Posterior() { return m_action_Layer_Dilation_Posterior; }
	QAction* Action_Layer_Dilation_Inferior() { return m_action_Layer_Dilation_Inferior; }
	QAction* Action_Layer_Dilation_Superior() { return m_action_Layer_Dilation_Superior; }
	QAction* Action_Layer_Dilation_6_Connectivity() { return m_action_Layer_Dilation_6_Connectivity; }

	QAction* Action_Layer_ImageCalculator() { return m_action_Layer_Image_Calculator; }

	QAction* Action_Layer_Move_Mask_Left() { return m_action_Layer_Move_Mask_Left; }
	QAction* Action_Layer_Move_Mask_Right() { return m_action_Layer_Move_Mask_Right; }
	QAction* Action_Layer_Move_Mask_Anterior() { return m_action_Layer_Move_Mask_Anterior; }
	QAction* Action_Layer_Move_Mask_Posterior() { return m_action_Layer_Move_Mask_Posterior; }
	QAction* Action_Layer_Move_Mask_Inferior() { return m_action_Layer_Move_Mask_Inferior; }
	QAction* Action_Layer_Move_Mask_Superior() { return m_action_Layer_Move_Mask_Superior; }

	QAction* Action_ManualDrawing_PolygonSelection() { return m_action_ManualDrawing_PolygonSelection; }
	QAction* Action_ManualDrawing_RegionSelection() { return m_action_ManualDrawing_RegionSelection; }
	QAction* Action_ManualDrawing_PixelWisedSelection() { return m_action_ManualDrawing_PixelWisedSelection; }
	QAction* Action_ManualDrawing_OvalSelection() { return m_action_ManualDrawing_OvalSelection; }
	QAction* Action_ManualDrawing_AngularSelection() { return m_action_ManualDrawing_AngularSelection; }
	QAction* Action_ManualDrawing_Less() { return m_action_ManualDrawing_Less; }
	QAction* Action_ManualDrawing_More() { return m_action_ManualDrawing_More; }

	QAction* Action_VolumeProcessing_FlipLayer() { return m_action_VolumeProcessing_FlipLayer; }

	QAction* Action_VolumeHomePosition_Anterior() { return m_action_VolumeHomePosition_Anterior; }
	QAction* Action_VolumeHomePosition_Posterior() { return m_action_VolumeHomePosition_Posterior; }
	QAction* Action_VolumeHomePosition_Right() { return m_action_VolumeHomePosition_Right; }
	QAction* Action_VolumeHomePosition_Left() { return m_action_VolumeHomePosition_Left; }
	QAction* Action_VolumeHomePosition_Superior() { return m_action_VolumeHomePosition_Superior; }
	QAction* Action_VolumeHomePosition_Inferior() { return m_action_VolumeHomePosition_Inferior; }

	QAction* Action_WindowBasic_FullScreen() { return m_action_WindowBasic_FullScreen; }
	QAction* Action_WindowBasic_BoundaryViewer() { return m_action_WindowBasic_BoundaryViewer; }

	QAction* Action_SemiAutoSeg_DrawCutMode() { return m_action_SemiAutoSeg_DrawCutMode; }
	QAction* Action_SemiAutoSeg_DrawCutApply() { return m_action_SemiAutoSeg_DrawCutApply; }

	QAction* Action_WorkingRegion() { return m_action_WorkingRegion; }

	QAction* Action_Threshold_WithinBrush() { return m_action_Threshold_WithinBrush; }
	QAction* Action_Threshold_Preview() { return m_action_Threshold_Preview; }

private:
	bool Create_And_AddAction(QAction** ppAction, QString text, QKeySequence keysequnce);
	bool Create_And_AddAction_With_FunctionLevel(QAction** ppAction, QString text, QKeySequence keysequnce, eMEDIP_FUNCTION_LEVEL functionLevel);
	bool Create_And_AddAction_With_UndoStack(QAction** ppAction, QString text, QKeySequence keysequnce, QUndoStack* undoStack, bool undo);
	bool AddAction_To_MainWidget(QAction* pAction, QString text, QKeySequence keysequnce);
	bool AddAllAction_To_ActionList();
	bool AddActionToActionList(QAction* pAction);

private:
	QWidget* m_mainWidget;

	std::vector<QAction*> m_actionList;

	QAction* m_action_UndoStack_Undo_Main;
	QAction* m_action_UndoStack_Redo_Main; 

	QAction* m_action_Import_Open;
	QAction* m_action_Import_ReverseOpen;
	QAction* m_action_Import_MultiOpen;

	QAction* m_action_Export_Save;
	QAction* m_action_Export_SaveAs;

	QAction* m_action_Common_BreakLock;
	QAction* m_action_Common_BeginScreen;
	QAction* m_action_Common_Delete;
	QAction* m_action_Common_ESC;
	QAction* m_action_Common_PagePrev;
	QAction* m_action_Common_PageNext;
	QAction* m_action_Common_Info;

	QAction* m_action_Layer_General_Clear;
	QAction* m_action_Layer_General_New;
	QAction* m_action_Layer_General_Rename;
	QAction* m_action_Layer_General_Duplicate;
	QAction* m_action_Layer_General_Delete;
	QAction* m_action_Layer_General_Visible;

	QAction* m_action_Layer_3DHoleFilling;

	QAction* m_action_Layer_2DHoleFilling_AxialPlane;
	QAction* m_action_Layer_2DHoleFilling_CoronalPlane;
	QAction* m_action_Layer_2DHoleFilling_SagittalPlane;
	QAction* m_action_Layer_2DHoleFilling_WholePlane;

	QAction* m_action_Layer_Boolean_Intersection;
	QAction* m_action_Layer_Boolean_Merge;
	QAction* m_action_Layer_Boolean_FFS;

	QAction* m_action_Layer_Split_Region;
	QAction* m_action_Layer_Make_Polyhedron;	

	QAction* m_action_Layer_Inverse;


	QAction* m_action_Layer_Erosion_Left;
	QAction* m_action_Layer_Erosion_Right;
	QAction* m_action_Layer_Erosion_Posterior;
	QAction* m_action_Layer_Erosion_Anterior;
	QAction* m_action_Layer_Erosion_Superior;
	QAction* m_action_Layer_Erosion_Inferior;
	QAction* m_action_Layer_Erosion_6_Connectivity;

	QAction* m_action_Layer_Dilation_Left;
	QAction* m_action_Layer_Dilation_Right;
	QAction* m_action_Layer_Dilation_Posterior;
	QAction* m_action_Layer_Dilation_Anterior;
	QAction* m_action_Layer_Dilation_Superior;
	QAction* m_action_Layer_Dilation_Inferior;
	QAction* m_action_Layer_Dilation_6_Connectivity;

	QAction* m_action_Layer_Image_Calculator;

	QAction* m_action_Layer_Move_Mask_Left;
	QAction* m_action_Layer_Move_Mask_Right;
	QAction* m_action_Layer_Move_Mask_Posterior;
	QAction* m_action_Layer_Move_Mask_Anterior;
	QAction* m_action_Layer_Move_Mask_Superior;
	QAction* m_action_Layer_Move_Mask_Inferior;

	QAction* m_action_ManualDrawing_PolygonSelection;
	QAction* m_action_ManualDrawing_RegionSelection;
	QAction* m_action_ManualDrawing_PixelWisedSelection;
	QAction* m_action_ManualDrawing_OvalSelection;
	QAction* m_action_ManualDrawing_AngularSelection;
	QAction* m_action_ManualDrawing_Less; 
	QAction* m_action_ManualDrawing_More; 

	QAction* m_action_VolumeProcessing_FlipLayer;

	QAction* m_action_VolumeHomePosition_Anterior;
	QAction* m_action_VolumeHomePosition_Posterior;
	QAction* m_action_VolumeHomePosition_Right;
	QAction* m_action_VolumeHomePosition_Left;
	QAction* m_action_VolumeHomePosition_Superior;
	QAction* m_action_VolumeHomePosition_Inferior;

	QAction* m_action_WindowBasic_FullScreen;
	QAction* m_action_WindowBasic_BoundaryViewer;

	QAction* m_action_SemiAutoSeg_DrawCutMode;
	QAction* m_action_SemiAutoSeg_DrawCutApply;

	QAction* m_action_WorkingRegion;

	QAction* m_action_Threshold_WithinBrush;
	QAction* m_action_Threshold_Preview;
};

//=========================================================
//				Utility
//=========================================================
void SetEnable_Action(QAction* action, bool enabled);
#endif