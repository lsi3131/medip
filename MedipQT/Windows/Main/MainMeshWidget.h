#ifndef MAINMESHWIDGET_H
#define MAINMESHWIDGET_H

#include "define.h"
#include "glwidget.h"
#include "MainTabWidget.h"

#include "Renderer/Mesh.h"

#include "DataContext.h"

class MEVolumeView;
class QVBoxLayout;
class QGridLayout;
class ImageListWidget;

class MainMeshWidget : public MainTabWidget
{
	Q_OBJECT

public:
	explicit MainMeshWidget(QWidget* parent = 0);
	virtual ~MainMeshWidget();

public:
	bool Init(DataContext* pDataContext);

	void activate();
	void deactivate();

	void	loadMesh(int uid, bool reset = false);
	mip::MeshTopology* getMainMesh();
	mip::MATRIX44* getMainMatrix();
	void renderLater();
	void setWorkMode(MESH_WORK_MODE mode);

	void resetUI();

	MEVolumeView* getMainView();

	QWidget* getWidgetMesh();
	QWidget* getWidgetVisualPrintMesh();

	void OnCaptureClicked(QListWidgetItem* item);
	void showMaterialTab(bool bShow);

private:
	bool createMeshWindow();
	void addTabItem(QString path, QString itemName);

private:
	QVBoxLayout* m_meshMainLayout;

	MEVolumeView* m_viewMesh;
	QWidget* m_widgetMesh;
	QWidget* m_pWidgetVisualPrintMesh;

	//Material Capture
	QTabWidget* m_tabImg; /*default : capture list, import list*/
	ImageListWidget* m_listCapture;		/*view mode : grid=icon(default), list*/
	ImageListWidget* m_listOther;		/*view mode : grid=icon(default), list*/

	bool m_dockHidden; //previous tab hidden
	DataContext* m_pDataContext;
};
#endif
