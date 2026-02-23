#pragma once

#ifndef ROITAB2_H
#define ROITAB2_H

#include "CollapseWidget.h"
#include "UI/MaskList.h"
#include <QTreeWidget>
#include <QPushButton>
#include <QLabel>

class ROITab2 : public CollapseWidget
{
	Q_OBJECT

public:
	explicit ROITab2(ProductManager* pProductManager, QWidget* parent = nullptr);

	void	AddROIList();
	void	ClearROIList();
	void	updateROIList(bool isSelection = false, int selection = -1);
	void	updateROIListMultiMask(const std::vector<muint32>& vecMultiMaskUID);
	void	updateROIHeader(LAYER_COLUMN_AKA type = L_COL_COUNT); /*COL_COUNT:all, COL_SHOW:layer, COL_MESH:surface*/
	void	applyVoxel(int);
	void	setMaintabType(MAINTAB_TYPE);
	void	updateVoxelLabel();
	void	updateTAState(bool, int);
	void	invertShowState();

protected:
	bool	eventFilter(QObject* watched, QEvent* e) override;

private:
	void initDataFromConfigFile();
	void addMaskListWidget();
	void addButtonWidgets();
	void addVoxelInfoWidgets();
	void addLegacyWidgets();

private slots:
	void	slot_OnHeaderClick(QTreeWidgetItem* item, int column);
	void	slot_OnColumnClick(QTreeWidgetItem* item, int column);
	void	slot_OnColumnDoubleClick(QTreeWidgetItem* item, int column);
	void	slot_OnColumnChanged(QTreeWidgetItem* item, int column);

	void	slot_OnInverse();
	void	slot_OnFFS();
	void	slot_OnMerge();
	void	slot_OnErosion();
	void	slot_OnDilation();
	void	slot_OnAdd();
	void	slot_OnDup();
	void	slot_OnDel();
	void	slot_OnFitBoundingBox();
	void	slot_OnAxialArea();

public:
	MaskListWidget* ROIList;

private:
	ProductManager* m_pProductManager;
	int m_nRow;

	QTreeWidget* m_ROIheader;
	QPushButton* m_btnAnalyze;

	QPushButton* m_btnInverse;
	QPushButton* m_btnFFS;
	QPushButton* m_btnMerge;
	QPushButton* m_btnErosion;
	QPushButton* m_btnDilation;
	QLabel* m_labelVoxel = nullptr;
	QLabel* m_labelAxialArea;
	QLabel* m_labelBurdenWeight = nullptr;
	QPushButton* m_btnAdd = nullptr;
	QPushButton* m_btnDup = nullptr; //duplicate
	QPushButton* m_btnDel = nullptr;
	QPushButton* m_btnFitBox = nullptr;

	QPushButton* m_btnAxialArea;

	QPushButton* m_btnVoxelCountCalculator; // 211209 허 건 과장

	bool		m_bExpand;

	int		m_nIconSize = 0;
};
#endif