#pragma once

#ifndef WORKINGREGIONTAB_H
#define WORKINGREGIONTAB_H

#include "CollapseWidget.h"
#include "graphics/BoundingBox.h"
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>

class WorkingRegionTab : public CollapseWidget
{
	Q_OBJECT

public:
	WorkingRegionTab(QWidget* parent = NULL);

	void	SelectPoint();
	void	cancelSelectMode();
	void	cancelMoveMode();
	void	setRegionInfo(bool isCombo = true);

	bool	useRatio() { return m_ratio; }

protected:
	bool eventFilter(QObject* watched, QEvent* e) override;

public slots:
	void	slot_OnSelect(bool val = false);
	void	slot_OnComboChanged(int index = 0);
	void	slot_OnApply();
	void	slot_OnSaveToNII();
	void	slot_OnSaveToHURAW();
	void	slot_OnMoveRegion(bool);
private:
	float		m_xRegion;
	float		m_yRegion;
	float		m_zRegion;

	bool		m_ratio;	//default (true)

	QPushButton* m_btnPoint;
	QLabel* m_labelPoint;

	QLineEdit* m_editRegion;
	QLabel* m_labelInfo;

	BoundingBoxI m_regionBox;
	QComboBox* m_comboRegion;

	QPushButton* m_btnMove;

private slots:
	void	slot_OnRatioChk(bool);

};
#endif