#pragma once

#ifndef VISUALIZE2DTAB_H
#define VISUALIZE2DTAB_H

#include <QComboBox>
#include <QPushButton>
#include <QSlider>
#include <QLineEdit>
#include <QCheckBox>
#include "defineMEDIP.h"
#include "UI/CollapseWidget.h"

class ProductManager;
class Factory;

class Visualize2DTab : public CollapseWidget
{
	Q_OBJECT

public:
	explicit Visualize2DTab(ProductManager* pProductManager, QWidget* parent = NULL);

	void	ResetUI();
	void	resetSlide();

	void	setPreset(SLICE_PRESET type = SP_DEFAULT, bool reset = false, int cusPre = -1);
	void	setWidth(int);
	void	setLevel(int);

	void	addCustomPreset(int index, bool isImport = false, bool bAppDataPath = false);
	void	delCustomPreset(int index);

	bool	getPresetSetting(int& wl, int& ww, SLICE_PRESET type = SP_DEFAULT, int cusPre = -1);

	float	getAutoScaleSlope() { return m_fAutoScaleSlope; }
	void	setAutoScaleSlope(float val) { m_fAutoScaleSlope = val; }

	SLICE_PRESET getPresetType() { return m_preset; }
	int			getCusPresetType() { return m_cusPreset; }

	void setItemData_presetCombo(eConfigFileLocation eConfigFileLoc);
	void Show_PresetEditDeleteBtn(bool bShow);

	void showWidgetByTabType(MAINTAB_TYPE type);

	bool isVisibleMesh2DOutlineCheckBox();
	bool isOutlineMode();

private:
	ProductManager* m_pProductManager;
	Factory* m_pProductFactory;

	QComboBox* m_presetCombo;
	QPushButton* m_presetEdit;
	QPushButton* m_presetDel;
	QSlider* m_slideLevel;
	QSlider* m_slideWidth;
	QSlider* m_slideLayer2D;

	QLineEdit* m_textWidth;
	QLineEdit* m_textLevel;
	QLineEdit* m_textLAlpha;

	QWidget* m_rootPETWidget;
	QSlider* m_slideOpacity;
	QLineEdit* m_editPETLoadStatus;
	QCheckBox* m_chkPETOn;

	QCheckBox* m_chkMesh2DOutline;

	SLICE_PRESET m_preset;
	SLICE_PRESET m_prePreset;
	int			m_cusPreset;
	float		m_fAutoScaleSlope;

protected:
	bool		eventFilter(QObject* watched, QEvent* event) override;

private:
	void		InAreaWidth(int&);
	void		InAreaLevel(int&);
	void		createPETOverlayWidget(int& nRow);
	void		updatePETOverlayControl();

private slots:
	void		slot_OnVisualizationOKClick();
	void		slot_OnLevelChanged(int pos);
	void		slot_OnWidthChanged(int pos);

	void		slot_OnAutoClick();
	void		slot_OnResetClick();

	void		slot_OnDirectionCheck(bool);
	void		slot_OnRepositioningCheck(bool);
	void		slot_OnInvertingLUTCheck(bool);

	void		slot_OnMeshOutlineCheck(bool);

	void		slot_OnColorCheck(bool);

	void		slot_OnComboChanged(int index);

	void		slot_OnSave();
	void		slot_OnDel();
	void		slot_OnAlphaEditFinish();
	void		slot_OnAlphaChanged(int);
	void		slot_OnAlphaRelease();

	void		slot_onPETLoadBtnClicked();
	void		slot_onPETONChkClicked(bool checked);
	void		slot_onPETOpacityValueChanged(int value);

signals:
	void	sig_autoClickSgl();

};
#endif
