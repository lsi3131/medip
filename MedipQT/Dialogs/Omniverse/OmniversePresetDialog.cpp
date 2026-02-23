#include "stdafx.h"
#include "OmniversePresetDialog.h"
#include "styleManager.h"
#include "OmniversePresetAddOrModifyDialog.h"
#include "graphics/DataContext.h"
#include "Omniverse/Usd/mipUsdPresetManager.h"

OmniversePresetDialog::OmniversePresetDialog(QWidget* parent) :
	QDialog(parent)
{
	setupUi(this);
}

void OmniversePresetDialog::Init(DataContext* pDataContext)
{
	m_pDataContext = pDataContext;
	m_pPresetManager = m_pDataContext->GetOmniversePresetManager();

	m_btnAdd->setStyleSheet(STYLE_MANAGER->buttonBehind);
	m_btnModify->setStyleSheet(STYLE_MANAGER->buttonBehind);
	m_btnDelete->setStyleSheet(STYLE_MANAGER->buttonBehind);

	m_btnApply->setStyleSheet(STYLE_MANAGER->buttonBehind);
	m_btnCancel->setStyleSheet(STYLE_MANAGER->buttonBehind);

	m_treePresetWidget->setStyleSheet(STYLE_MANAGER->treeOmniversePresetList);
	m_treePresetWidget->setHeaderLabels({ "Preset Name" });

	m_treeMeshToMaterialWidget->setStyleSheet(STYLE_MANAGER->treeOmniversePresetList);
	m_treeMeshToMaterialWidget->setContentsMargins(0, 0, 0, 0);
	m_treeMeshToMaterialWidget->setHeaderLabels({ "Mesh", "Materials" });

	m_treeUsdPrimWidget->setStyleSheet(STYLE_MANAGER->treeOmniversePresetList);
	m_treeUsdPrimWidget->setContentsMargins(0, 0, 0, 0);
	m_treeUsdPrimWidget->setHeaderLabels({ "Name", "Type" });

	m_cboRenderSetting_RenderMode->setStyleSheet(STYLE_MANAGER->comboBoxNormal);

	m_Hydra_DefaltWidth_SpinBox->setStyleSheet(STYLE_MANAGER->doubleSpinbox);

	m_Post_BackgroundAlpha_DefaultColor_Enable_ChkBox->setStyleSheet(STYLE_MANAGER->doubleSpinbox);
	m_Post_BackgroundAlpha_DefaultColor_Composite_ChkBox->setStyleSheet(STYLE_MANAGER->doubleSpinbox);

	m_Post_BackgroundAlpha_DefaultColor_SpinBox_Red->setStyleSheet(STYLE_MANAGER->doubleSpinbox);
	m_Post_BackgroundAlpha_DefaultColor_SpinBox_Green->setStyleSheet(STYLE_MANAGER->doubleSpinbox);
	m_Post_BackgroundAlpha_DefaultColor_SpinBox_Blue->setStyleSheet(STYLE_MANAGER->doubleSpinbox);

	m_Histogram_WhiteScale_SpinBox->setStyleSheet(STYLE_MANAGER->doubleSpinbox);

	m_RenderSetting_Wireframe_Thickness_spinBox->setStyleSheet(STYLE_MANAGER->doubleSpinbox);

	connect(m_btnAdd, &QPushButton::clicked, this, &OmniversePresetDialog::slot_Add);
	connect(m_btnModify, &QPushButton::clicked, this, &OmniversePresetDialog::slot_Modify);
	connect(m_btnDelete, &QPushButton::clicked, this, &OmniversePresetDialog::slot_Delete);

	connect(m_btnApply, &QPushButton::clicked, this, &OmniversePresetDialog::slot_Apply);
	connect(m_btnCancel, &QPushButton::clicked, this, &OmniversePresetDialog::slot_Cancel);

	connect(m_treePresetWidget, &QTreeWidget::currentItemChanged, this, &OmniversePresetDialog::slot_PresetItemChanged);

	m_cboRenderSetting_RenderMode->clear();
	m_cboRenderSetting_RenderMode->addItem("PathTracing", mipUsdRenderSetting::PathTracing);
	m_cboRenderSetting_RenderMode->addItem("RealTime", mipUsdRenderSetting::RealTime);

	updateControls();
}

void OmniversePresetDialog::updateControls(mipUsdPreset* pPresetSelect)
{
	updatePresetNameControls(pPresetSelect);
	updateDataControlsByCurselPreset();
}

void OmniversePresetDialog::updatePresetNameControls(mipUsdPreset* pPresetSelect)
{
	m_treePresetWidget->clear();
	std::vector<mipUsdPresetPtr> presetList = m_pPresetManager->GetPresetList();
	for (auto& pPreset : presetList)
	{
		QTreeWidgetItem* item = new QTreeWidgetItem();
		QString presetName = QString::fromStdString(pPreset->GetPresetName());
		item->setText(0, presetName);
		m_treePresetWidget->addTopLevelItem(item);

		if (pPresetSelect)
		{
			if (pPresetSelect->GetPresetName() == pPreset->GetPresetName())
			{
				m_treePresetWidget->setCurrentItem(item);
			}
		}
	}
}

void OmniversePresetDialog::updateDataControlsByCurselPreset()
{
	mipUsdPresetPtr pPreset = getCurselPreset();
	if (pPreset == nullptr)
	{
		return;
	}

	updateMeshMaterialBindWidget(*pPreset);
	updateUsdPrimWidget(*pPreset);

	updateRenderSettingWidget(pPreset->GetRenderSetting());
}

void OmniversePresetDialog::updateMeshMaterialBindWidget(const mipUsdPreset& preset)
{
	m_treeMeshToMaterialWidget->clear();

	std::vector<mipPresetMeshMaterialPair> meshMaterialList = preset.GetMeshToMaterialPairList();
	for (auto& pair : meshMaterialList)
	{
		m_treeMeshToMaterialWidget->addTopLevelItem(createItem_MeshMaterialBind(pair));
	}
}

void OmniversePresetDialog::updateUsdPrimWidget(const mipUsdPreset& preset)
{
	m_treeUsdPrimWidget->clear();

	std::vector<mipUSDMaterialPtr> materialList = preset.CreateMaterialList();
	for (auto& pMaterial : materialList)
	{
		m_treeUsdPrimWidget->addTopLevelItem(createItem_UsdPrim(pMaterial->GetName().c_str(), pMaterial->GetTypeName().c_str()));
	}

	std::vector<mipUsdLightPtr> lightList = preset.CreateLightList();
	for (auto& pLight : lightList)
	{
		m_treeUsdPrimWidget->addTopLevelItem(createItem_UsdPrim(pLight->GetName().c_str(), pLight->GetTypeName().c_str()));
	}
}

void OmniversePresetDialog::updateRenderSettingWidget(const mipUsdRenderSetting& setting)
{
	for (int i = 0; i < m_cboRenderSetting_RenderMode->count(); ++i)
	{
		mipUsdRenderSetting::eRenderMode mode = (mipUsdRenderSetting::eRenderMode)m_cboRenderSetting_RenderMode->currentData().toInt();
		if (mode == setting.GetRenderMode())
		{
			m_cboRenderSetting_RenderMode->setCurrentIndex(i);
			break;
		}
	}

	double doubleValue = 0;
	bool bCheck = false;
	mip::VECTOR3 color;

	m_Hydra_DefaltWidth_SpinBox->setValue(0.0);

	m_Post_BackgroundAlpha_DefaultColor_Composite_ChkBox->setChecked(false);

	m_Post_BackgroundAlpha_DefaultColor_SpinBox_Red->setValue(0.0);
	m_Post_BackgroundAlpha_DefaultColor_SpinBox_Green->setValue(0.0);
	m_Post_BackgroundAlpha_DefaultColor_SpinBox_Blue->setValue(0.0);

	m_Post_BackgroundAlpha_DefaultColor_Enable_ChkBox->setChecked(false);

	m_Histogram_WhiteScale_SpinBox->setValue(0.0);
	m_RenderSetting_Wireframe_Thickness_spinBox->setValue(0.0);

	if (setting.TryGet_Hydra_Points_DefaultWidth(&doubleValue))
	{
		m_Hydra_DefaltWidth_SpinBox->setValue(doubleValue);
	}

	if (setting.TryGet_Post_BackgroundZeroAlpha_Composite(&bCheck))
	{
		m_Post_BackgroundAlpha_DefaultColor_Composite_ChkBox->setChecked(bCheck);
	}

	if (setting.TryGet_Post_BackgroundZeroAlpha_DefaultColor(&color))
	{
		m_Post_BackgroundAlpha_DefaultColor_SpinBox_Red->setValue(color[0]);
		m_Post_BackgroundAlpha_DefaultColor_SpinBox_Green->setValue(color[1]);
		m_Post_BackgroundAlpha_DefaultColor_SpinBox_Blue->setValue(color[2]);
	}

	if (setting.TryGet_Post_BackgroundZeroAlpha_Enable(&bCheck))
	{
		m_Post_BackgroundAlpha_DefaultColor_Enable_ChkBox->setChecked(bCheck);
	}

	if (setting.TryGet_Post_Histogram_WhiteScale(&doubleValue))
	{
		m_Histogram_WhiteScale_SpinBox->setValue(doubleValue);
	}

	if (setting.TryGet_Writeframe_Thickness(&doubleValue))
	{
		m_RenderSetting_Wireframe_Thickness_spinBox->setValue(doubleValue);
	}
}

QTreeWidgetItem* OmniversePresetDialog::createItem_MeshMaterialBind(const mipPresetMeshMaterialPair& pair)
{
	QTreeWidgetItem* item = new QTreeWidgetItem();
	QString meshName = pair.GetMeshName().c_str();
	QString materialName = pair.GetMaterialName().c_str();

	item->setText(0, meshName);
	item->setText(1, materialName);

	return item;
}

QTreeWidgetItem* OmniversePresetDialog::createItem_UsdPrim(const QString& name, const QString& type)
{
	QTreeWidgetItem* item = new QTreeWidgetItem();

	item->setText(0, name);
	item->setText(1, type);

	return item;
}

mipUsdPresetPtr OmniversePresetDialog::getCurselPreset() const
{
	QTreeWidgetItem* item = m_treePresetWidget->currentItem();
	if (item == nullptr)
	{
		return nullptr;
	}
	QString presetName = item->text(0);
	mipUsdPresetPtr pPreset = m_pPresetManager->GetPresetByName(presetName.toStdString());
	if (pPreset == nullptr)
	{
		return nullptr;
	}

	return pPreset;
}

void OmniversePresetDialog::slot_Add()
{
	OmniversePresetAddOrModifyDialog dlg(this);
	dlg.OpenAddMode(m_pPresetManager);
	int result = dlg.exec();
	if (result == QDialog::Accepted)
	{
		updateControls();
	}
}

void OmniversePresetDialog::slot_Modify()
{
	mipUsdPresetPtr pCurSelPreset = getCurselPreset();
	if (pCurSelPreset == nullptr)
	{
		return;
	}

	OmniversePresetAddOrModifyDialog dlg(this);
	dlg.OpenModifyMode(m_pPresetManager, pCurSelPreset);
	int result = dlg.exec();
	if (result == QDialog::Accepted)
	{
		updateControls();
	}
}

void OmniversePresetDialog::slot_Delete()
{
	QTreeWidgetItem* item = m_treePresetWidget->currentItem();
	if (item == nullptr)
	{
		return;
	}

	QString presetName = item->text(0);
	if (m_pPresetManager->DeleteByName(presetName.toStdString()))
	{
		delete item;
		item = nullptr;
	}
}

void OmniversePresetDialog::slot_Apply()
{
	accept();
}

void OmniversePresetDialog::slot_Cancel()
{
	reject();
}

void OmniversePresetDialog::slot_PresetItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
	if (current)
	{
		updateDataControlsByCurselPreset();
	}
}

