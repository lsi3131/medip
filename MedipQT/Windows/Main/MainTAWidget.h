#pragma once

#include "define.h"
#include "AnalMPRPlaneView.h"
#include "MainTabWidget.h"
#include "WindowBase.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;

class AnalMPRPlaneView;
class AnalVolumeView;

//Texture Analysis
class MainTAWidget : public MainTabWidget
{
	Q_OBJECT

public:
	MainTAWidget(QWidget* parent = nullptr);

	bool init(DataContext* pDataContext);

	void activate() override;
	void deactivate() override;

	void renderLater_All();
	void renderLater_GridView(bool volumeupdate = true);
	void hideControls();
	void showControls(WINDOW_TYPE windowType);
	void updatePlaneData(WINDOW_TYPE windowType);
	void updateSliderPosition();

	void nextPage();
	void prePage();

	WINDOW_TYPE getFullScreen();
	void resetUI();

	void setWindowsZoomFactor(float x, float y, float z);
	void setBeginScreen();

	void initWindows();
	void resetResource();
	void updateLayerState();

	bool createLeftGridWindows(DataContext* pDataContext);

	void setWorkMode(RADIOMICS_WORK_MODE mode);

	void setSharedWidget();
	void removeSharedWidget();

	WindowBase* getWindow(WINDOW_TYPE windowType);
	AnalVolumeView* getViewVolume();

	void updateMeshOutline();
	void setMPRViewInfo(QVector3D volumePos, mint16 HU);

private slots:
	void slot_setFullScreen(WindowBase* window);
	void slot_setFullScreen(OpenGLWidget* window);

protected:
	MPRViewSharedInfo m_viewerSharedInfo;

private:
	WINDOW_TYPE m_tempType;

	QVBoxLayout* m_TAMainLayout;
	QGridLayout* m_gridlayout;
	QHBoxLayout* m_screenLayout;
	QHBoxLayout* m_histogramLayout;
	QVBoxLayout* m_btnLayout;
	QHBoxLayout* m_plotLayout;

	QWidget* m_leftGrid;
	QWidget* m_leftFullScreen;
	OpenGLWidget* m_viewVolume;
	//OpenGLWidget * viewAxial;
	WindowBase* m_viewAxial;
	WindowBase* m_viewCoronal;
	WindowBase* m_viewSagittal;

	DataContext* m_pDataContext;
};

