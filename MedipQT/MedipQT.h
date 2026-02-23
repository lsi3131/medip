#pragma once
#include "define.h"
#include "FileManager/net/defines.h"

#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2);
VTK_MODULE_INIT(vtkRenderingFreeType);
VTK_MODULE_INIT(vtkInteractionStyle);

#ifndef _M_IX86
#define vtkRenderingCore_AUTOINIT 4(vtkInteractionStyle,vtkRenderingFreeType,vtkRenderingFreeTypeOpenGL,vtkRenderingOpenGL)
#define vtkRenderingVolume_AUTOINIT 1(vtkRenderingVolumeOpenGL)
#endif

#include <QtWidgets/QMainWindow>
#include "ui_MedipQT.h"
#include "Windows/Main/MainTabWidget.h"
#include "Actions/ActionManager.h"

class QAction;
class QMenu;
class CollapseDock;
class ImageManagementTab;
class ProductManager;
class Factory;
class LicenseManager;
class WindowManager;
class ActionManager;

namespace fm
{
	class FileManagerImportData;
}

class MedipQT : public QMainWindow, public ActionObserver
{
	Q_OBJECT

public:
	MedipQT(QWidget* parent = Q_NULLPTR);
	~MedipQT();

public:
	int init(DataContext* pDataContext);
	void ExistFileSave(QString fileName, eAfterTHREAD type = THREAD_OPEN_FILE); //type -> after thread
	void ExistFileSaveAndOpenImportData();
	CollapseDock* getTabDock() { return m_pDockCollapseSeg; }
	bool createReportTab();
	void showImageManagementTab();
	void setDockWindowTitle(const QString& title);
	void setMainWindowTitle(QString subTitle, bool isMain = false);
	void RebootProgram();

	void DeleteRecentFile(int index);
	void AddRecentFile(QString FileName);
	void UpdateRecentFile();
	void SwapRecentFile(int, int newIndex, int exIndex);

	// deepcatch report operation function
	void deepcatchResultToReport(bool bUseSingleThreadProgress);
	void deepcatchResultToExpertReport();
	void moveToReportTab();
	//#ifdef SUPPORT_DEEPCATCH_V2_DOCTORANSWER		
	bool getChartImg(QImage& outImg);
	bool getHCCResult(std::vector<float>& vec);
	//#endif
	void CreateUndoRedoAction();

	void fileOpen(QString fileName, bool bReverse = true);
	void OpenImportData(bool isImportDicomCropping);
	void callMainTabchange(int tabType);

	bool MedipMacroFileOpen(QString filepath);
public:
	int IconSize = 0;
	int MainTabSize = 0;

	// MACRO MODE
	bool IgnoreCloseEvent; // 매크로를 위해 close 시 저장 여부 묻지 않게하는 변수

protected:
	virtual bool eventFilter(QObject* target, QEvent* e) override;
	virtual void closeEvent(QCloseEvent* e) override;
	virtual void moveEvent(QMoveEvent* event) override;
	virtual void onActionStarted(ACTION_PROCESSING id, void* pData) override;
	virtual void onActionFinished(ACTION_PROCESSING id, void* pData) override;

	virtual void resizeEvent(QResizeEvent* event) override;
private:
	bool createMainOpenGLContext();
	bool createMainMenu();
	bool createMainStatusBar();
	bool useCheck();
	void setHomePosition(HOME_POSITION index, bool bForce = false);

	template<typename T, typename ENUM>
	float getDeepCatchReportValue(T temp, ENUM _enum);
	QStringList getFileOpenExtensionFilter();
	QStringList GetImportFilePathListByFunctionLevel(int functionLevel, QString filter);
	void ImportMultiFile_Default(QStringList filePathList);
	void AddActionToMenu(QMenu* menu, QString text, const char* slotFunction);
	void AddActionToMenuWithFunctionLevel(QMenu* menu, QString text, const char* slotFunction, int functionLevel);

	bool saveAs_Logout();

	void replaceMenuIcon();
signals:
	void ProgramClose();
	bool progressCheck();
	void progressUpdate(unsigned int);

public slots:
	void OnOpenMIPFile();
	void OnOpenDcmFolder();
	void OnImportRawMaskFiles();
	void OnImportNIIMaskFiles();
	void OnImportSTLFiles();
	void OnImportObjFiles();
	void OnImportVTKFiles();
	void OnImport3MFFiles();
	void OnImportImageFiles();
	void OnImportCustomRenderingPreset();
	void OnCloseProject();
	void OnExit();

	////////project file open slot
	void OnStrProgress(QString);
	void OnProjClearVolume();
	void OnProjupdateAnno();
	void OnProjcreateHistogram();
	void OnProjresetRG();
	void OnProjresetwork();
	void OnProjsetDepth(int, float);
	void OnProjPreset(bool, int, int, int, int);
	void OnProjWheelzoom(bool);
	void OnProjsetGamma(bool, bool);
	void OnProjresetUI();
	void OnProjresetRC();
	void OnProjupdateUI();
	void OnProjupdateSummary();
	void OnNIIinitUI(int, int);

	void OnThreadEnd();
	void OnUpdateProgress(int value);
	void slot_OnUpdateProgress(int value, QString _str);
	void OnUpdateProgressMultiThread(int value, QString strWorkName = "");
	void OnThreadCancel();
	void OnHistoClick();
	void OnShowImageList();
	void OnUpdateReport();

	void OnMacroFileOpen(QString fileName, bool& isDone);
	void reportVolumeViewCapture();

	void slot_updateMeshUI(bool _b_refresh, int _selIndex, bool _b_clear);
	void showQMessageDlg(int _Rstring, QString _caseName);

	void slot_buildRenderBufferTopology(mip::MeshTopology* _p_mesh);
	void slot_renderLater();

	void slot_applyMaskToUI(int _uid);

	void slot_MoveMeshDlg();
private slots:
	void OnRecentFileOpen();

	void OnMenuFileOpen();

	void OnXrayFileOpen();
	void OnMenuFilesOpen();
	void OnMenuFileManagerOpen();
	void OnMenuCropping();
	void OnMenuReverseOpen();
	bool OnMenuFileSave(bool exit = false);
	bool OnMenuFileSaveAs(bool exit = false);
	bool OnMenuUploadToPACS();

	void OnLayerFlip();

	void OnMenuAbout();
	void OnServerConnect();
	void OnWebConnect();

	bool OnPrePage();
	bool OnNextPage();

	bool OnEsc();
	bool OnDelete();
	bool OnLess();
	bool OnMore();
	void OnHPA();
	void OnHPP();
	void OnHPL();
	void OnHPR();
	void OnHPS();
	void OnHPI();

	void OnMainTabChanged(int index);
	void OnTabSegment();
	void OnScreentoBegin();
	void OnShowTab();

	void OnUndoStateChange();
	void OnUndoClick();

	void OnRedoStateChange();
	void OnRedoClick();
	void OnInfoClick();
	void OnSViewClick();
	void OnLinkClick();
	void OnBarAreaChange();
	void OnBreakLock();
	void OnVisibleLayers();
	void OnSegDockFloating(bool floating);
	void OnSegDockAreaChange(Qt::DockWidgetArea area);

	void OnPolyROI();
	void OnFreeDrawROI();
	void OnPickerROI();
	void OnOvalROI();
	void OnRectROI();

	void OnShowBounding();
	void OnShowBoundaryEdge();

	void OnRename(); // 220713 F2 키 통한 Rename 

	void slot_actionFinished(int id);
	void slot_actionProcessFinished(int actionProcessID);

private:
	QString	m_strMainTitle;
	QString	m_StrOpen;
	QString	m_strRecent[RECENT_MAX];

	QToolButton* m_pToolBtnOpen;
	QPushButton* m_pBtnOpen;
	QPushButton* m_pBtnCrop;
	QPushButton* m_pBtnSave;
	QPushButton* m_pBtnSaveAs;
	QPushButton* m_pBtnUploadToPACS;
	QPushButton* m_pBtnServer = nullptr;
	QPushButton* m_pBtnUndo;
	QPushButton* m_pBtnRedo;
	QPushButton* m_pBtnInfo;

	QPushButton* m_pBtnResetView;
	QPushButton* m_pBtnHomepage;

	QPushButton* m_pBtnCollapseActive = nullptr;
	QPushButton* m_pBtnCrossSectionView = nullptr;
	QPushButton* m_pBtnHistogram = nullptr;
	QPushButton* m_pBtnImageList = nullptr;

	QMenu* m_pMenuOpen;

	QAction* m_pActOpen;
	QAction* m_pActMultiOpen;//*.txt, *.raw, *.nii(only roi), *.stl, *.prd, *.png, *.bmp, *.jpg
	QAction* m_pActCancel;
	QAction* m_pActCrossSection;
	QAction* m_pActHistogram;
	QAction* m_pActBreakLock; //hidden func
	QAction* m_pActRecent[RECENT_MAX] = {};


	QWidget* m_pWidgetMain;
	QPushButton* m_pTabBtnSegment;
	QPushButton* m_pTabBtnWeb;

	QTabBar* m_pTab;
	QToolBar* m_pToolBarFile;
	CollapseDock* m_pDockCollapseSeg;

	MainTabWidget* m_pMainTab[MAINTAB_COUNT] = {};
	CollapseDock* m_pDockImage;

	// deepcatch report volume view capture temp variables
	QTimer* m_pReportVolumeVeiwCaptureTimer;
	int m_nReportVolumeVeiwCapturesteps;
	int m_nReportTabIdx;

	fm::FileManagerImportData* m_pImportData;
	bool m_importDicomWithCropping;

	DataContext* m_pDataContext;
	WindowManager* m_pWindowManager;
	LicenseManager* m_pLicenseManager;
	ProductManager* m_pProductManager;
	Factory* m_pProductFactory;
	ActionManager* m_pActionManager;

	QWidget* m_emptySpace[10];
};
