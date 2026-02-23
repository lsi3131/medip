#pragma once

#include "define.h"
#include "Windows/Main/MainTabWidget.h"
#include "VisualPrintMEVolumeView.h"

class VisualPrintMEVolumeView;

class MainVisualPrintMeshWidget : public MainTabWidget
{
	//	Q_OBJECT

public:
	explicit MainVisualPrintMeshWidget(QWidget* parent = 0);
	virtual ~MainVisualPrintMeshWidget();

	void activate();
	void deactivate();

	void	loadMesh(int uid, bool reset = false);
	mip::MeshCore* getMainMesh();
	mip::MATRIX44* getMainMatrix();
	void renderLater();
	void setWorkMode(VISUAL_PRINT_WORK_MODE mode);

	void resetUI();

	bool createMeshWindow(DataContext* pDataContext);

	bool init(DataContext* pDataContext);

	VisualPrintMEVolumeView* getMainView() { return m_viewMesh; }

	QWidget* getWidgetMesh() { return m_widgetMesh; }

	void setUpdateFinish(bool _b_flag) { m_bUpdateFinish = _b_flag; }
	bool getUpdateFinish() { return m_bUpdateFinish; }

private:
	QVBoxLayout* m_meshMainLayout;
	VisualPrintMEVolumeView* m_viewMesh;
	QWidget* m_widgetMesh;

	bool m_dockHidden; //previous tab hidden
	bool m_bUpdateFinish;
	DataContext* m_pDataContext;
};
