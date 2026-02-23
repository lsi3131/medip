#pragma once

#include <QDialog>
#include <memory>
#include "ui_OmniversePresetAddOrModifyDialog.h"
#include "Omniverse/Usd/mipUsdPreset.h"

class mipUsdPresetManager;

class OmniversePresetAddOrModifyDialog : public QDialog, public Ui::OmniversePresetAddOrModifyDialog
{
	Q_OBJECT
public:
	enum class eMode
	{
		AddNew,
		Modify
	};

public:
	OmniversePresetAddOrModifyDialog(QWidget* parent = nullptr);

public:
	bool OpenAddMode(mipUsdPresetManager* pPresetManager);
	bool OpenModifyMode(mipUsdPresetManager* pPresetManager, mipUsdPresetPtr pPreset);

private:
	void initUI();

	QTreeWidgetItem* createMeshToMaterialPairItem(const mipPresetMeshMaterialPair& pair);
	QTreeWidgetItem* createPrimItem(const std::string& name, const std::string& type);

	void updateMeshMaterialTreeWidget();
	void updatePrimTreeWidget();

	void saveNew(const std::string& rootPresetDirPath, const std::string& presetName);
	void saveModified(const std::string& rootPresetDirPath);

private slots:
	void slot_LoadUsd();
	void slot_Add();
	void slot_Delete();

	void slot_Apply();
	void slot_Cancel();

private:
	mipUsdPresetManager* m_pPresetManager;
	mipUsdPresetPtr m_pUsdPreset;
	mipUsdPresetPtr m_pModifiedUsdPreset;

	eMode m_mode;
};
