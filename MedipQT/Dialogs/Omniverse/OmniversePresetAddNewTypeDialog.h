#pragma once

#include <QDialog>
#include "ui_OmniversePresetAddNewTypeDialog.h"
#include "Omniverse/Usd/mipUsdStage.h"
#include "Omniverse/Usd/mipUsdPreset.h"

class OmniverseContext;

class OmniversePresetAddNewTypeDialog : public QDialog, public Ui::OmniversePresetAddNewTypeDialog
{
	Q_OBJECT

public:
	OmniversePresetAddNewTypeDialog(QWidget* parent = nullptr);

public:
	bool Init(mipUsdPresetPtr pUsdPreset);

private:
	bool initMaterialComboBox();

private slots:
	void slot_Add();
	void slot_Cancel();

private:
	mipUsdPresetPtr m_pUsdPreset;
};
