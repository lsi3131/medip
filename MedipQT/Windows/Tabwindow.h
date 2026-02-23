#pragma once

#ifndef TABWINDOW_H
#define TABWINDOW_H

#include "define.h"
#include <qwidget.h>
#include <qicon.h>
#include "UI/CollapseWidget.h"
#include "graphics/BoundingBox.h"
#include "graphics/color.h"

#include "System/MedipType.h"
#include "System/ProductManager.h"

#include "volumedata.h"

#include "VisualPrintDlg.h"
#include "MaskList.h"

#include "Tab/AIContrastSynthesisTab.h"
#include "Tab/AIKernelConversionTab.h"
#include "Tab/AILowdoseCTReconstuctionTab.h"
#include "Tab/AISetTab.h"
#include "Tab/CaptureTab.h"
#include "Tab/WorkingRegionTab.h"
#include "Tab/Visualize2DTab.h"
#include "Tab/AnalysisTab.h"
#include "Tab/Visualize3DTab.h"
#include "Tab/VOLProcessTab.h"
#include "Tab/IMGProcessTab.h"
#include "Tab/AITranslationTab.h"
#include "Tab/ROITab2.h"
#include "Tab/SummaryTab.h"
#include "Tab/DrawcutTab.h"
#include "Tab/Export3DTab.h"
#include "Tab/AnnotationTab2.h"
#include "Tab/AnimationTab.h"
#include "Tab/ThreSholdTab.h"
#include "Tab/VisualPrintMeshTab.h"
#include "Tab/VisualPrintTab.h"
#include "Tab/OptionalTab.h"
#include "Tab/OmniverseTab.h"

#include "DataContext.h"

class QLabel;
class QPlainTextEdit;
class QSlider;
class QLineEdit;
class QDoubleSpinBox;
class QTreeWidget;
class QTreeWidgetItem;
class QComboBox;
class QCheckBox;
class QPushButton;
class QToolButton;
class QButtonGroup;
class QTabWidget;
class QGroupBox;
class QAbstractButton;

class MaskListWidget;
class VisualPrintMeshListWidget;
class RangeWidget;
class VisualPrintDlg;
class Factory;
class ProductManager;
class WindowManager;
class ActionManager;

class CPlaneManiplator;

class MeshTab;
class AISegTab;
class AISegTabDeepCatch;

class TabWindow : public QWidget
{
	Q_OBJECT

public:
	TabWindow(DataContext* pDataContext, QWidget* parent = nullptr);
	TabWindow(
		DataContext* pDataContext, 
		WindowManager* pWindowManager, 
		ProductManager* pProductManager, 
		ActionManager* pActionManager,
		Factory* pProductFactory, 
		CPlaneManiplator* pPlaneManipulator, 
		QWidget* parent = nullptr);

public:
	void SetMaintabType(MAINTAB_TYPE type);
	void ResetUI();

	std::vector<CollapseWidget*> GetAllTabList() const;
	std::vector<CollapseWidget*> GetTabListByMainTabType(MAINTAB_TYPE type);

	Visualize2DTab* get2DTab();
	IMGProcessTab* getImgTab();
	VOLProcessTab* getVolTab();
	DrawcutTab* getDrawTab();
	SummaryTab* getSummaryTab();
	Visualize3DTab* get3DTab();
	Export3DTab* getExport3DTab();
	AnnotationTab2* getAnnoTab();
	AnimationTab* getAniTab();
	ThreSholdTab* getThreSholdTab();
	ROITab2* getROITab();
	WorkingRegionTab* getPatchyTab();
	CaptureTab* getCaptureTab();
	OptionalTab* getOptionalTab();
	AnalysisTab* getAnalysisTab();
	AISegTab* getAITab();
	AITranslationTab* getAITranslationTab();
	AIContrastSynthesisTab* getAIContrastSynthesisTab();
	AIKernelConversionTab* getAIKernelConversionTab();
	AILowdoseCTReconstuctionTab* getAILowdoseCTReconTab();
	MeshTab* getMeshTab();
	VisualPrintTab* getVisualPrintTab();
	VisualPrintMeshTab* getVisualPrintMeshTab();
	OmniverseTab* getOmniverseTab();

	void MoveLeft(void);
	void MoveRight(void);

private:
	void addMainTabCollapsedTabList(MAINTAB_TYPE type, std::vector<CollapseWidget*> tabList);

private:
	Visualize2DTab* m_tab2D;
	IMGProcessTab* m_tabImg;
	VOLProcessTab* m_tabVol;
	DrawcutTab* m_tabDraw;
	SummaryTab* m_tabSummary;
	Visualize3DTab* m_tab3D;
	Export3DTab* m_tabExport;
	AnnotationTab2* m_tabAnno;
	AnimationTab* m_tabAnimation;
	ThreSholdTab* m_tabThre;
	ROITab2* m_tabROI;
	CaptureTab* m_tabCapture;
	WorkingRegionTab* m_tabPatchy;
	OptionalTab* m_tabOption;
	AnalysisTab* m_tabAnalysis;
	AISegTab* m_tabAISeg;
	AITranslationTab* m_tabAITranslation;
	AIContrastSynthesisTab* m_tabAIContrastSynthesis;
	AIKernelConversionTab* m_tabAIKernelConversion;
	AILowdoseCTReconstuctionTab* m_tabAILowdoseRecon;
	MeshTab* m_tabMesh2;
	VisualPrintTab* m_tabVisualPrint;
	VisualPrintMeshTab* m_tabVisualPrintMesh;
	OmniverseTab* m_tabOmniverse;

private:
	std::map<MAINTAB_TYPE, std::vector<CollapseWidget*>> m_mapCollapseTabList;

	WindowManager* m_pWindowManager;
	ProductManager* m_pProductManager;
	ActionManager* m_pActionManager;
	Factory* m_pProductFactory;
	CPlaneManiplator* m_pPlaneManipulator;
};
#endif


