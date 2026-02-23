#pragma once

#include "defineMEDIP.h"
#include "graphics/MeshInfo.h"
#include "DataContext.h"
#include "UI/Mesh/MeshListColumnList.h"

#include <QWidget>
#include <QPainter>
#include <QStyle>
#include <QMouseEvent>
#include <QTreeWidget>
#include <QBrush>

class WindowManager;
class ActionManager;
class ProductManager;
class MEVolumeView;
class OmniverseContext;
class CMeshModelViewManager;
class CPlaneManiplator;
class CMeshManipulator;
class ShortcutManager;

enum class eMeshActionType
{
	Export,
	Export_STL,
	Export_Obj,
	Export_VTK,
	Export_3MF,
	Export_USD,
	Transform,
	Transform_Image_Plus,
	Transform_Image_Minus,
	Boolean,
	Boolean_Difference,
	Boolean_Intersection,
	Boolean_Union,
	Duplicate,
	Attatch,
	Mesh2Mask,
	Solid,
	Hollow,
	Remesh,
	Reduction,
	Smooth,
	Subdivision,
	IslandFilter,
	SendToOmniverse,
	ReceiveFromOmniverse,
};

class MeshListWidget;

struct MeshActionInfo
{
	eMeshActionType Type;
	QString Text;
	int FunctionLevel;
	void (MeshListWidget::* SlotFunction)(void);
	std::vector<MeshActionInfo> Children;
};

class MeshListWidget : public QTreeWidget
{
	Q_OBJECT

public:
	MeshListWidget(
		std::shared_ptr<MeshListColumnList> pColumnList,
		WindowManager* pWinManager,
		ActionManager* pActionManager, 
		ProductManager* pProductManager,
		DataContext* pDataContext, 
		CMeshModelViewManager* pMeshModelViewManager,
		CPlaneManiplator* pPlaneManipulator, 
		CMeshManipulator* pMeshManipulator,
		ShortcutManager* pShortcutManager,
		QWidget* parent = nullptr);
	virtual ~MeshListWidget();

public:
	bool Init();

	QSize sizeForRow();

	void UpdateSubwidget(muint8 UID);
	void ChangeSubUID(muint8 newUID);
	void Update();
	void updateSize(bool);
	void SetAllColumnVisible(bool visible = true);
	void SetAllColumnLock(eOmniverseStatus status);

	void SelectionUpdate(int index_dest, bool b_close_dlg = true);
	void SelectionRefresh(int index_dest, bool b_clear = true, bool b_close_dlg = true);
	void rename();

	void UpdateItemByLayerIndex(int layerIndex);
	void UpdateItemList();

	void ToggleShowState();
	void ToggleOmnivserStatusState();

	void ExportMeshLayerToFile(QString caption, EXPORT_FILES exportFileType, QString filter, bool bPatientCoordinate = false);
	QList<QTreeWidgetItem*> GetExportItemList();
	bool ExportMultiFile(QString caption, QString lastestDirPath, QList<QTreeWidgetItem*> exportItemList, EXPORT_FILES exportFileType, bool bPatientCoordinate);
	bool ExportSingleFile(QString caption, QString lastestDirPath, EXPORT_FILES exportFileType, QString filter, bool bPatientCoordinate);

	bool IsEditMode() const;
	void SetEditMode(bool value);

	std::vector<MeshActionInfo> GetActionInfoList() const;
	std::shared_ptr<QMenu> Get_QMenu();

	bool ItemClicked(QTreeWidgetItem* item, int column);
	bool ItemDoubleClicked(QTreeWidgetItem* item, int column);

protected:
	void contextMenuEvent(QContextMenuEvent* event) override;
	void dropEvent(QDropEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* e) override;
	void keyReleaseEvent(QKeyEvent* e) override;
	void keyPressEvent(QKeyEvent* e) override;

protected:
	QList<muint32> getSeletedIndexList() const;

private:
	QList<QTreeWidgetItem*> getSelectedTargetItem();

private:
	bool getImageMatrix(mip::MATRIX44& imageMatrix);

	void setItemByMeshInfo(QTreeWidgetItem* outItem, const MeshInfo& info);
	QString getConnectedMaterialNameByMeshInfo(const MeshInfo& info);

	QVector<int> getExportIndexList() const;
	void setItemBackgroundColor(QTreeWidgetItem* item, const QBrush& brush);

	QBrush getBrush_Unselected() const;
	QBrush getBrush_Selected() const;

	QIcon getIcon_Invisible() const;
	QIcon getIcon_Visible() const;

Q_SIGNALS:
	void sig_columnStateChanged(int column);

public slots:
	void slot_OnExportSTLFile();
	void slot_OnExportSTLFilePatientCoordinate();
	void slot_OnExportOBJFile();
	void slot_OnExportOBJFilePatientCoordinate();
	void slot_OnExportVTKFile();
	void slot_OnExportVTKFilePatientCoordinate();

	void slot_OnExport3MFFile();
	void slot_OnExport3MFFilePatientCoordinate();

	void slot_ExportUSD();

	// Transform
	void slot_ImageMatrixPlus();
	void slot_ImageMatrixMinus();

	void slot_ShiftPosition();

	void slot_DeleteMeshes();

	void slot_OnDiff();
	void slot_OnIntersect();
	void slot_OnUnion();

	void slot_Attach();
	void slot_OnDuplicate();
	void slot_Mesh2Mask();

	void slot_Solid();
	void slot_Hollow();
	void slot_Remesh();
	void slot_Smooth();
	void slot_Reduction();//collapse
	void slot_SubDivision();
	void slot_IslandFilter();
	void slot_SendToOmniverse();
	void slot_ReceiveFromOmniverse();
#if SUPPORT_MESHOFFSET == 1
	void OnMeshOffset();
#endif

	void slot_meshInfoChanged(MeshInfo* pInfo);
	void slot_OmniverseStageChanged();
	void slot_OmniverseStagePresetChanged();

private:
	bool m_isEditMode;

	WindowManager* m_pWinManager;
	ActionManager* m_pActionManager;
	ProductManager* m_pProductManager;

	DataContext* m_pDataContext;
	OmniverseContext* m_pOmniverse;

	CMeshModelViewManager* m_pMeshModelViewManager;
	CPlaneManiplator* m_pPlaneManipulator;
	CMeshManipulator* m_pMeshManipulator;

	ShortcutManager* m_pShortcutManager;
	std::shared_ptr<MeshListColumnList> m_pColumnList;

	std::array<QBrush, 2> m_brushes;
	std::array<QIcon, 2> m_visibleIcons;
};
