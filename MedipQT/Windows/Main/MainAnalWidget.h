#pragma once

#include "define.h"
#include "glwidget.h"
#include "MainTabWidget.h"
#include "DataContext.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;

class AnalMPRPlaneView;
class AnalVolumeView;
class MainAnalWidget : public MainTabWidget
{
	Q_OBJECT

public:
	explicit MainAnalWidget(QWidget* parent = 0);
	virtual ~MainAnalWidget();

public:
	virtual void activate() override;
	virtual void deactivate() override;

public:
	void changeShapeSize();

	void renderLater_All();
	void renderLater_GridView(bool volumeupdate = true);
	void updatePlaneData(WINDOW_TYPE windowType);
	void hideControls();
	void showControls(WINDOW_TYPE windowType);
	WINDOW_TYPE getFullScreen() { return m_tempType; }
	void nextPage();
	void prePage();

	void setWindowsZoomFactor(float x, float y, float z);

	void resetSlide();

	void updateUI(bool isSelection = false, int selection = -1);
	void resetUI();

	void setBeginScreen();

	bool createLeftGridWindows(DataContext* pDataContext);

	bool init(DataContext* pDataContext);
	void reInit();
	void setSharedWidget();
	void removeSharedWidget();

	void setWorkMode(ANAL_WORK_MODE mode);
	AnalMPRPlaneView* getWindow(WINDOW_TYPE windowType);
	AnalVolumeView* getViewVolume();

	void updatePlaneState();
	void updateMeshOutline();

private slots:
	void slot_setFullScreen(OpenGLWidget* window);

private:
	WINDOW_TYPE m_tempType;

	QVBoxLayout* m_analMainLayout;
	QGridLayout* m_gridlayout;
	QWidget* m_leftGrid;
	QHBoxLayout* m_screenLayout;
	QHBoxLayout* m_histogramLayout;
	QVBoxLayout* m_btnLayout;
	QHBoxLayout* m_plotLayout;
	QWidget* m_leftFullScreen;

	OpenGLWidget* m_viewVolume;
	OpenGLWidget* m_viewAxial;
	OpenGLWidget* m_viewCoronal;
	OpenGLWidget* m_viewSagittal;

	QWidget* m_widgetVolume;
	QWidget* m_widgetAxial;
	QWidget* m_widgetCoronal;
	QWidget* m_widgetSagittal;

};
