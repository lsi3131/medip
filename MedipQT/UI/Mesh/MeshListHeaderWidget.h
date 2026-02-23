#pragma once

#include <array>
#include <QTreeWidget>
#include "defineMEDIP.h"
#include "UI/Mesh/MeshListColumnList.h"

class WindowManager;
class DataContext;

class MeshListHeaderWidget : public QTreeWidget
{
	Q_OBJECT

public:
	MeshListHeaderWidget(std::shared_ptr<MeshListColumnList> pColumnList, WindowManager* pWinManager, DataContext* pDataContext, QWidget* parent = nullptr);

public:
	void Init();

	void SetExpand(bool isExpand);
	bool IsExpand() const;

	bool ResetColumnAll();
	bool UpdateColumn(MESH_COLUMN_AKA column);
	bool ToggleColumn(MESH_COLUMN_AKA column);

	bool ItemClicked(QTreeWidgetItem* item, int column);

	void UpdateShow();
	void UpdateLock();
	void UpdateExpand();
	void UpdateMaterial();

	void ToggleShow();
	void ToggleLock();
	void ToggleExpand();

private:
	int getExpandArrowColumnIndex() const;
	bool isExpandColumn(MESH_COLUMN_AKA column) const;

Q_SIGNALS:
	void sig_columnStateChanged(int column);
	void sig_expandChanged(bool isExpand);

private:
	std::shared_ptr<MeshListColumnList> m_pColumnList;
	WindowManager* m_pWinManager;
	DataContext* m_pDataContext;

	std::array<QBrush, 2> m_brushes;
	std::array<QString, 2> m_textArrowes;

	bool m_isExpand;
};
