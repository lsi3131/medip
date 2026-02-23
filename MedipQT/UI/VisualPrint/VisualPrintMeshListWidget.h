#pragma once

#ifndef VISUALPRINT_MESHLIST_WIDGET_H
#define VISUALPRINT_MESHLIST_WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QDoubleSpinBox>

#include "DataContext.h"

class VisualPrintMeshSmoothWidget;
class VisualPrintMEVolumeView;

class VisualPrintMeshListWidget : public QTreeWidget
{
	Q_OBJECT

public:
	VisualPrintMeshListWidget(DataContext* pDataContext, QWidget* parent = nullptr);
	virtual ~VisualPrintMeshListWidget();

	QSize sizeForRow();

	void updateSubwidget(muint8 UID);
	void changeSubUID(muint8 newUID);
	void update(bool bCreate);
	void updateSize(bool);
	void updateColumn(MESH_COLUMN_AKA, bool res = true);
	void selectionUpdate(int index_dest);
	void selectionRefresh(int index_dest);
	void setTabType(MAINTAB_TYPE);
	VisualPrintMeshSmoothWidget* GetVisualPrintMeshSmoothWidget();

	void setSmoothWidgets(int _val_smooth, int _val_reduce);
	void setSmoothWidgetsSmoothVal(int _val_smooth);
	void setSmoothWidgetsReduceVal(int _val_reduce);

	void setEnable(bool _b_enable);
protected:
	void contextMenuEvent(QContextMenuEvent* event) override;
	void dropEvent(QDropEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* e) override;
	void keyReleaseEvent(QKeyEvent* e) override;
	void keyPressEvent(QKeyEvent* e) override;

private slots:
	void slot_ShiftPosition();

	void OnRename();

private:
	MAINTAB_TYPE	Tabtype;
	QAction* actRename;

	DataContext* m_pDataContext;

	VisualPrintMeshSmoothWidget* m_pVisualPrintMeshSmoothWidget;
public:
	bool editMode;
};
#endif