#pragma once

#include <QDialog>
#include "ui_OmniversePresetDialog.h"
#include "Omniverse/Usd/mipUsdPreset.h"

class DataContext;
class mipUsdPresetManager;

class OmniversePresetDialog : public QDialog, public Ui::OmniversePresetDialog
{
	Q_OBJECT

public:
	OmniversePresetDialog(QWidget* parent);

public:
	void Init(DataContext* pDataContext);

private:
	void updateControls(mipUsdPreset* pPresetSelect = nullptr);
	void updatePresetNameControls(mipUsdPreset* pPresetSelect = nullptr);
	void updateDataControlsByCurselPreset();

	void updateMeshMaterialBindWidget(const mipUsdPreset& preset);
	void updateUsdPrimWidget(const mipUsdPreset& preset);
	void updateRenderSettingWidget(const mipUsdRenderSetting& setting);

	QTreeWidgetItem* createItem_MeshMaterialBind(const mipPresetMeshMaterialPair& pair);
	QTreeWidgetItem* createItem_UsdPrim(const QString& name, const QString& type);

	mipUsdPresetPtr getCurselPreset() const;

private slots:
	void slot_Add();
	void slot_Modify();
	void slot_Delete();

	void slot_Apply();
	void slot_Cancel();

	void slot_PresetItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);

private:
	DataContext* m_pDataContext;
	mipUsdPresetManager* m_pPresetManager;
};
