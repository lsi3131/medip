#pragma once

#ifndef REGIONGROWTAB_H
#define REGIONGROWTAB_H

#include "defineMEDIP.h"
#include "CollapseWidget.h"
#include <QTreeWidget>
#include <QPushButton>
#include <QButtonGroup>

/*
	21.03.29 이상일 대리
	현재 사용되지 않음. 확인 후 삭제할 것
*/
class RegionGrowTab : public CollapseWidget
{
	Q_OBJECT

public:
	RegionGrowTab(QWidget* parent = NULL);

	QTreeWidget* m_RGList;
	QPushButton* m_btnSelect;
	void Init();
	void ApplyRegionGrowing();
	void cancelSelectMode();
	void UpdateList();
	void Reset() { slot_OnReset(); }
protected:
	bool	eventFilter(QObject* watched, QEvent* event) override;

private:
	REGION_GROWING_MODE	m_mode;
	REGION_GROWING_CONNECTIVITY	m_connectivity;

	QButtonGroup* m_grMode;
	QButtonGroup* m_grConnect;
private slots:
	void	slot_OnChangeMode(int);
	void	slot_OnChangeConnect(int);
	void	slot_OnReset();
	void	slot_OnDel();
	void	slot_OnApply();
	void	slot_OnSelectSeed();
};
#endif