#pragma once

#ifndef VISUALIZE3DTAB_H
#define VISUALIZE3DTAB_H

#include "CollapseWidget.h"
#include "ActionDeepCatchReport.h"
#include "defineMEDIP.h"
#include <QTreeWidget>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>

class ProductManager;
class Factory;

class Visualize3DTab : public CollapseWidget
{
	Q_OBJECT

public:
	explicit Visualize3DTab(ProductManager* pProductManager, QWidget* parent = NULL);

	void		setPreset(SLICE_PRESET type = SP_DEFAULT, bool reset = false, int cusType = -1);
	void		resetSlide();

	void		setWidth(int);
	void		setLevel(int);

	void		setVolumeAlpha(int val);
	void		setLayerAlpha(int val);

	SLICE_PRESET getPresetType() { return m_preset; }
	bool		isClipMode();
	void		clipOnOff(bool val);

	void		setDisableShader(bool set);
	void		OnVRMode();

	void		addCustomPreset(int index, bool isImport = false, bool bAppDataPath = false);
	void		delCustomPreset(int index);

	int			getCusPresetType() { return m_cusPreset; }
	void		setClipOpt();

	void		deepcatch3DTabSetting(DEEPCATCH_REPORT* pReport, bool bRestore = false);

	void		setItemData_presetCombo(eConfigFileLocation eConfigFileLoc);
	void		Show_PresetEditDeleteBtn(bool bShow);

protected:
	bool		eventFilter(QObject* watched, QEvent* event) override;

private:
	void		createPresetUI();
	void		createWidthLevelUI();
	void		createAlphaUI();
	void		createShaderQualityUI();
	void		createClipOptionUI();
	void		createMixModeUI();
	void		createBottomCheckBoxUI();
	void		createVRUI();
	void		InAreaLevel(int& nLevel);
	void		InAreaWidth(int& nWidth);

private slots:
	void		slot_OnWidthLevelOKClick();
	void		slot_OnLevelChanged(int pos);
	void		slot_OnWidthChanged(int pos);
	void		slot_OnSliderPress();
	void		slot_OnSliderRelease();
	void		slot_OnWidthLevelResetClick();
	void		slot_OnAlphaResetClick();

	void		slot_OnComboChanged(int index);

	void		slot_OnCullingVolumeChecked(bool check);
	void		slot_OnCullingSufaceChecked(bool check);
	void		slot_OnClip2DPlane(bool);
	void		slot_OnVisiableSufaceChecked(bool check);
	void		slot_OnAnnoStateChanged(bool);
	void		slot_OnBackfaceCulling(bool check);
	void		slot_OnVolumeAlphaChanged(int pos);
	void		slot_OnVolumeAlphaOKClick();
	void		slot_OnLayerAlphaChanged(int pos);
	void		slot_OnLayerAlphaOKClick();
	void		slot_OnDPIChanged(); //slider
	void		slot_OnDPIMoved(int pos);
	void		slot_OnClip();

	void		slot_OnSave();
	void		slot_OnDel();

	void		slot_OnHighLight();
	void		slot_OnVR();
	void		slot_OnMeshTest();
public slots:
	void		slot_OnChangeMode(int type);
	void		slot_OnShaderComboChanged(int index);

private:
	ProductManager* m_pProductManager;
	Factory* m_pProductFactory;

	QComboBox* m_presetCombo;
	QPushButton* m_presetEdit;
	QPushButton* m_presetDel;

	QSlider* m_slideLevel;
	QSlider* m_slideWidth;

	QLineEdit* m_textWidth;
	QLineEdit* m_textLevel;

	SLICE_PRESET m_preset;
	SLICE_PRESET m_prePreset;
	int m_cusPreset;
#ifdef DEV_VER
	QPushButton* m_btnHighLight;
#endif
	QComboBox* m_shaderCombo = nullptr;

	QButtonGroup* m_maskGroup = nullptr;
	QPushButton* m_btnVR = nullptr;

	QSlider* m_sliderVolumeAlpha = nullptr;
	QSlider* m_sliderLayerAlpha = nullptr;
	QSlider* m_sliderDPI = nullptr;

	QLineEdit* m_editVolumeAlpha = nullptr;
	QLineEdit* m_editLayerAlpha = nullptr;
	QLineEdit* m_editDPI = nullptr;

	QPushButton* m_clipModeBtn = nullptr;
	int			m_nRow;
	bool		m_bPress;
	QWidget* m_boxClip = nullptr;
};
#endif

