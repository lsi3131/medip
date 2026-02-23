#pragma once

#include "CollapseWidget.h"
#include "UI/VisualPrint/VisualPrintMeshListWidget.h"

class VisualPrintMeshTab : public CollapseWidget
{
	Q_OBJECT

public:
	explicit VisualPrintMeshTab(QWidget* parent = nullptr);
	virtual ~VisualPrintMeshTab();

	void Init(DataContext* pDataContext);

	void Update(bool refresh = true, int selIndex = -1);
	void UpdateHeader(MESH_COLUMN_AKA type = M_COL_COUNT);
	void UpdateSubWidget(muint8 UID);
	void ChangeSubUID(muint8 newUID);
	void Clear();
	void SetTabType(MAINTAB_TYPE);
	VisualPrintMeshListWidget* GetVisualPrintMeshListWidget();

protected:
	//bool eventFilter(QObject *watched, QEvent *e) override;

private slots:
	void slot_OnHeaderClick(QTreeWidgetItem* item, int column);
	void slot_OnColumnClick(QTreeWidgetItem* item, int column);
	void slot_OnColumnDoubleClick(QTreeWidgetItem* item, int column);
	void slot_OnColumnChanged(QTreeWidgetItem* item, int column);
	void slot_LoadMesh();

public slots:
	// void slot_DeleteMesh();
	void slot_AddMeshes(QStringList filename);

private:
	VisualPrintMeshListWidget* m_treeMesh;
	QTreeWidget* m_treeHeader;

	QToolButton* m_btnLoad; //stl,obj,... + sphere/cube/cylinder
	QPushButton* m_btnDel;

	QPushButton* m_btnUnion;
	QPushButton* m_btnDiff;
	QPushButton* m_btnIntersect;

	bool  m_bExpand;
	QAbstractButton* m_HoverWidget;
	QIcon m_LeaveIcon;

	DataContext* m_pDataContext;
};

