#pragma once

#ifndef MESHTAB_H
#define MESHTAB_H

#include "defineMEDIP.h"
#include "CollapseWidget.h"
#include "UI/Mesh/MeshListWidget.h"
#include "UI/Mesh/MeshListHeaderWidget.h"
#include "UI/Mesh/MeshListColumnList.h"

#include "DataContext.h"
#include <QPushButton>
#include <array>

class WindowManager;
class ActionManager;
class ProductManager;
class OmniverseContext;
class CPlaneManiplator;
class ProductManager;
class CMeshModelViewManager;
class CMeshManipulator;
class ShortcutManager;
class Factory;

enum class eMeshTabButtonType
{
	Normal = 0,
	Selected = 1,
	Hovered = 2,
};

class MeshTab : public CollapseWidget
{
	Q_OBJECT

public:
	explicit MeshTab(ProductManager* pProductManager, QWidget* parent = nullptr);
	explicit MeshTab(
		WindowManager* pWinManager,
		ActionManager* pActionManager,
		ProductManager* pProductManager,
		CMeshModelViewManager* pMeshModelViewManager,
		CMeshManipulator* pMeshManipulator,
		ShortcutManager* pShorcutManager,
		QWidget* parent = nullptr);
	virtual ~MeshTab();

public:
	void Init(DataContext* pDataContext, OmniverseContext* pOmniverse, CPlaneManiplator* pPlaneManipulator);

	void Update(bool isRefresh = true, int selectIndex = -1, bool isClear = true, bool isCloseDialog = true);
	void UpdateSubWidget(muint8 UID);
	void ChangeSubUID(muint8 newUID);
	void Clear();
	void SetTabType(MAINTAB_TYPE);
	void ClearSelection();

	MeshListWidget* GetMeshList() const;
	MeshListHeaderWidget* GetMeshHeader() const;

protected:
	bool eventFilter(QObject* watched, QEvent* e) override;

private:
	void setButtonStyle(QAbstractButton* button, const std::array<QIcon, 3>& iconList, eMeshTabButtonType type);

	QList<muint32> getSelectedIndexList() const;
	void doBooleanWork(QPushButton* button, const std::array<QIcon, 3>& iconList, MESH_WORK_MODE mode, const QString& warningMessageTitle);

private slots:
	void slot_MeshHeaderClick(QTreeWidgetItem* item, int column);
	void slot_MeshHeaderColumnStateChanged(int column);
	void slot_MeshHeaderExpanedChanged(bool isExpand);

	void slot_MeshListItemClick(QTreeWidgetItem* item, int column);
	void slot_MeshListItemDoubleClick(QTreeWidgetItem* item, int column);
	void slot_MeshListItemChanged(QTreeWidgetItem* item, int column);
	void slot_MeshListColumnStateChanged(int column);

	void slot_LoadMesh();

	void slot_OnUnion();
	void slot_OnDiff();
	void slot_OnIntersect();

	void slot_OnSphere();
	void slot_OnCube();
	void slot_OnCylinder();

public slots:
	void slot_DeleteMesh();
	void slot_AddMesh(QString filename);
	void slot_AddMeshes(QStringList filenames);

signals:
	void sig_OnUpdateProgress(int, QString);

private:
	MeshListWidget* m_treeMeshList;
	MeshListHeaderWidget* m_treeMeshHeader;

	DataContext* m_pDataContext;

	std::array<QIcon, 3> ICON_NEW_LIST;
	std::array<QIcon, 3> ICON_DIFF_LIST;
	std::array<QIcon, 3> ICON_UNION_LIST;
	std::array<QIcon, 3> ICON_INTERSECT_LIST;
	std::array<QIcon, 3> ICON_DELETE_LIST;

	WindowManager* m_pWinManager;
	ActionManager* m_pActionManager;
	ProductManager* m_pProductManager;
	Factory* m_pProductFactory;
	CMeshModelViewManager* m_pMeshModelViewManager;
	CMeshManipulator* m_pMeshManipulator;
	ShortcutManager* m_pShorcutManager;

	QToolButton* m_btnLoad;	//stl,obj,... + sphere/cube/cylinder
	QPushButton* m_btnDel;

	QPushButton* m_btnUnion;
	QPushButton* m_btnDiff;
	QPushButton* m_btnIntersect;

	QAbstractButton* m_HoverWidget;
	QIcon m_LeaveIcon;

	std::shared_ptr<MeshListColumnList> m_pColumnList_Header;
	std::shared_ptr<MeshListColumnList> m_pColumnList_Widget;
};
#endif