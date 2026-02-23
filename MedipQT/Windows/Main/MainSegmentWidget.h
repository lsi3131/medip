#pragma once

#include "define.h"
#include "WindowBase.h"
#include "MainTabWidget.h"

class QGroupBox;
class QTabWidget;
class WindowBase;
class QHBoxLayout;
class QVBoxLayout;
class QDockWidget;
class CustomHistogram;

class OpenGLWidget;
class QTimer;
class QSlider;
class QComboBox;
class QPushButton;
class RangeWidget;
class QwtPlotCurve;
class QGridLayout;
class CollapseDock;

class MainSegmentWidget : public MainTabWidget
{
	Q_OBJECT

public:
	explicit MainSegmentWidget(QWidget* parent = nullptr);
	virtual ~MainSegmentWidget();

public:
	virtual void activate() override;
	virtual void deactivate() override;

public:
	bool init(DataContext* pDataContext);
	OpenGLWidget* getViewVolume();
	void setVRUpdate(bool value);
	WindowBase* getWindow(WINDOW_TYPE windowType);
	WINDOW_TYPE getFullScreen();

	QVBoxLayout* getTabLayout();

	void renderLater_All();
	void renderLater_SubView();
	void renderLater_GridView(bool volumeupdate = true);
	void updatePlaneData(WINDOW_TYPE windowType);
	void updateSliderPosition();
	void forceUpdate2DViewData();

	void delMaskView();
	void hideControls();
	void showControls(WINDOW_TYPE windowType);
	void initWindows();
	void resetResource();
	void updateLayerState();

	void setSharedWidget();

	void nextPage();
	void prePage();
	void enableTOIAll();
	void setWorkMode(WORK_MODE mode);
	void changeShapeSize();
	void setWindowsZoomFactor(float x, float y, float z);

	float getAutoScaleSlope();
	void setAutoScaleSlope(float value);

	void resetUI(bool isIsotro = false);

	SLICE_PRESET	getPresetType();
	void setBeginScreen(bool isIsotro = false);
	void createHUHisto();
	void SetWidthLine();

	QWidget* getRoiTab();
	CustomHistogram* getHistogramView();
	QPushButton* getHisZoomBtn();
	QPushButton* getHisCopyBtn();
	QPushButton* getHisSaveBtn();
	void SetWindowWidthLine(bool changePreset = false);
	void SetVolumeWidthLine(bool changePreset = false);
	void SetWindowRGSlider();
	bool createCenterWindows();
	bool deleteCenterWindows();
	bool isExistSView();

	bool isVisibleHistogram();
	void setVisibleHistogram(bool val);
#ifdef DEV_VER
	bool isHighLight() { return bHighlight; }
#endif

	bool createLeftGridWindows();
	void deleteLeftGridWindows();

	void updateMeshOutline();
	void setMPRViewInfo(QVector3D volumePos, mint16 HU);
	void updateSharedInfo();

private slots:
	void slot_setFullScreen(WindowBase* window);
	void slot_setFullScreen(OpenGLWidget* window);
	void slot_OnRenderTypeChanged(int index);

	void slot_OnComboChanged(int index);
	void slot_OnWidthChanged();
	void slot_OnLevelChanged();


	void slot_OnHistogramPressed();
	void slot_OnHistogramHide();

	void slot_OnZoomClicked();
	void slot_OnHistogramCopy();
	void slot_OnHistogramSave();

	void slot_OnHighlightClicked();

protected:
	MPRViewSharedInfo m_viewerSharedInfo;

private:
	DataContext* m_pDataContext;

	WINDOW_TYPE m_tempType;

	QVBoxLayout* m_segmentMainLayout;
	QWidget* m_leftGrid;
	QWidget* m_centerWidgets;
	QVBoxLayout* m_centerLayout;
	QHBoxLayout* m_screenLayout;
	QHBoxLayout* m_histogramLayout;
	QVBoxLayout* m_btnLayout;
	QHBoxLayout* m_plotLayout;
	QWidget* m_leftFullScreen;
	QWidget* m_rightTabWidget;

	WindowBase* m_viewSegment;
	WindowBase* m_viewResult;
	WindowBase* m_viewAxial;
	WindowBase* m_viewCoronal;
	WindowBase* m_viewSagittal;

	SLICE_PRESET m_preset;
	SLICE_PRESET m_prePreset;
	QSlider* m_slideLevel;
	QSlider* m_slideWidth;

	QWidget* widgetVolume;
	OpenGLWidget* m_viewVolume;

	QTimer* m_vrUpdateTimer;

	QComboBox* m_presetCombo;

	WindowBase* m_viewSub1;
	WindowBase* m_viewSub2;
	WindowBase* m_viewSub3;
	WindowBase* m_viewSub4;
	WindowBase* m_viewSub5;
	WindowBase* m_viewSub6;
	WindowBase* m_viewSub7;
	WindowBase* m_viewSub8;
	QVBoxLayout* m_rightBox;
	QWidget* m_tabInfo;
	QWidget* m_tabROI;
	QWidget* m_tabEnhance;
	QTabWidget* m_tabWidget;
	QSlider* m_sliderSmooth;

	QPushButton* m_btnZoom;
	QPushButton* m_btnCopy;
	QPushButton* m_btnSave;
	bool m_bCreate;
	bool m_bHistogram;
	bool m_bSubView;
#ifdef DEV_VER
	bool bHighlight;
#endif
	CustomHistogram* m_viewPlot;
#ifdef DEV_VER
	QPushButton* btnHighlight;
	//short* emptyMemory[8];
#endif
	QGridLayout* m_gridlayout;
};