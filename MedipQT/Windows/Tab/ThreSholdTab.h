#pragma once

#ifndef THRESHOLDTAB_H
#define THRESHOLDTAB_H

#include "defineMEDIP.h"
#include "CollapseWidget.h"
#include <QColor>
#include <QLineEdit>
#include <QColor>
#include <QCheckBox>
#include <QWidget>
#include <QTreeWidget>
#include <QComboBox>
#include <QPushButton>

enum ThreType
{
	THRE_DEFAULT = 0,
	THRE_SOFT_TISSUE,
	THRE_AIR,
	THRE_FAT,
	THRE_BONE,
	THRE_FLUIDS,
	THRE_ALL,
};

class ProductManager;
class Factory;

class ThreSholdTab : public CollapseWidget
{
	Q_OBJECT

public:
	explicit ThreSholdTab(ProductManager* pProductManager, QWidget* parent = NULL);
	//RegionGrowTab* m_rgTab;
	void	Init();

	void	Reset() { slot_OnResetThreshold(); }

	void	cancelSelectMode();

	QColor	getPreviewColor() { return m_ThreColor; }

	void	ApplyRegionGrowing();
	void	PreviewFinish();
	void	ApplyThreshold() { slot_OnApplyThreshold(); }

	void	setRegionGrowing();

	void	setThreType(ThreType index);

	int		getStartHU();
	int		getEndHU();
	bool	getBrush() { return m_bBrushin; }
	void	setStartHU(int);
	void	setEndHU(int);

	void	setRegionGrowingMode(REGION_GROWING_MODE mode);
	void	setRegionGrowingConnectivity(REGION_GROWING_CONNECTIVITY connectivity);

	//void Init(); // »ç¿ë x
	// 	void ApplyRegionGrowing();
	// 	void cancelSelectMode();
	void UpdateRGList();
	// 	void ResetRG() { OnResetRG(); }

	void	setPreviewThreshold(bool bCheck);

protected:
	bool	eventFilter(QObject* watched, QEvent* event) override;

signals:
	void sig_currentColorChanged(const QColor&);

private slots:
	void	slot_OnWithinCheck(bool checked);
	void	slot_OnBrushinCheck(bool checked);
	//void	OnRangeChanged(int, bool); //int - change hu / bool - true : start, false : end
	void	slot_OnStartTextChanged(); //EditFinish
	void	slot_OnEndTextChanged(); //EditFinish
	void	slot_OnCheckPreview(bool check = false);
	void	slot_OnColorChanged();
	void	slot_OnApplyThreshold();
	//void	OnRGApply();
	void	slot_OnResetThreshold();
	void	slot_OnSelectSeed();
	void	slot_OnComboChanged(int index);
	void	slot_OnChangeMode(int mode);
	void	slot_OnChangeConnect(int mode);
	void	slot_OnResetRG();
	void	slot_OnDelRG();
	void	slot_OnApplyRG();
	//void	slot_OnSelectSeed();
private:
	ProductManager* m_pProductManager;
	Factory* m_pProductFactory;

	QLineEdit* m_startHU;
	QLineEdit* m_endHU;
	QColor	m_ThreColor;
	QCheckBox* m_checkBrushin;
	QCheckBox* m_bThrePreview;
	QWidget* m_ThrePreImg;

	QTreeWidget* m_SeedList;
	QComboBox* m_ThreType;
	int		m_Start; //save only default type
	int		m_End; //save only default type

	bool	m_bWithin;
	bool	m_bBrushin;

	//region growing
	REGION_GROWING_MODE	m_mode;
	REGION_GROWING_CONNECTIVITY	m_connectivity;
	QButtonGroup* m_grMode;
	QButtonGroup* m_grConnect;
	///////////////////////////
	//region
	QTreeWidget* m_RGList;
	QPushButton* m_btnSelect;
};
#endif
