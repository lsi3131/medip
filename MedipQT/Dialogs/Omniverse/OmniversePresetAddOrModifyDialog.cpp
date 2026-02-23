#include "stdafx.h"
#include "OmniversePresetAddOrModifyDialog.h"
#include "styleManager.h"
#include "StringManager.h"
#include "Omniverse/Usd/mipUsdPresetManager.h"
#include "OmniversePresetAddNewTypeDialog.h"
#include <QMessageBox>

OmniversePresetAddOrModifyDialog::OmniversePresetAddOrModifyDialog(QWidget* parent) :
	QDialog(parent),
	m_pPresetManager(nullptr),
	m_mode(eMode::AddNew),
	m_pUsdPreset(nullptr)
{
	setupUi(this);
}

bool OmniversePresetAddOrModifyDialog::OpenAddMode(mipUsdPresetManager* pPresetManager)
{
	m_pPresetManager = pPresetManager;
	m_pUsdPreset = std::make_shared<mipUsdPreset>();

	if (m_pUsdPreset->OpenNew() == false)
	{
		return false;
	}

	m_mode = eMode::AddNew;

	initUI();
	return true;
}

bool OmniversePresetAddOrModifyDialog::OpenModifyMode(mipUsdPresetManager* pPresetManager, mipUsdPresetPtr pPreset)
{
	m_pPresetManager = pPresetManager;
	m_pUsdPreset = std::make_shared<mipUsdPreset>();

	m_pModifiedUsdPreset = pPreset;
	std::string modifiedUsdPresetDirPath = m_pModifiedUsdPreset->GetPresetDirectoryPath();
	if (m_pUsdPreset->LoadPreset(modifiedUsdPresetDirPath) == false)
	{
		qCritical() << "fail to load modified usd preset dirpath : " << modifiedUsdPresetDirPath.c_str();
		return false;
	}

	m_mode = eMode::Modify;

	initUI();
	return true;
}

void OmniversePresetAddOrModifyDialog::initUI()
{
	m_btnLoadUSD->setStyleSheet(STYLE_MANAGER->buttonBehind);
	m_btnAdd->setStyleSheet(STYLE_MANAGER->buttonBehind);
	m_btnDelete->setStyleSheet(STYLE_MANAGER->buttonBehind);

	m_btnApply->setStyleSheet(STYLE_MANAGER->buttonBehind);
	m_btnCancel->setStyleSheet(STYLE_MANAGER->buttonBehind);

	/* Mesh, Material Tree Widget */
	m_treeMeshMaterialInfo->setSelectionMode(QTreeWidget::NoSelection);
	m_treeMeshMaterialInfo->setStyleSheet(STYLE_MANAGER->treeOmniversePresetList);
	m_treeMeshMaterialInfo->setContentsMargins(0, 0, 0, 0);
	m_treeMeshMaterialInfo->setHeaderLabels({ "Mesh Name", "Materials" });

	/* Prim Tree Widget */
	m_treePrimInfo->setSelectionMode(QTreeWidget::NoSelection);
	m_treePrimInfo->setStyleSheet(STYLE_MANAGER->treeOmniversePresetList);
	m_treePrimInfo->setContentsMargins(0, 0, 0, 0);
	m_treePrimInfo->setHeaderLabels({ "Name", "Type" });

	connect(m_btnLoadUSD, &QPushButton::clicked, this, &OmniversePresetAddOrModifyDialog::slot_LoadUsd);
	connect(m_btnAdd, &QPushButton::clicked, this, &OmniversePresetAddOrModifyDialog::slot_Add);
	connect(m_btnDelete, &QPushButton::clicked, this, &OmniversePresetAddOrModifyDialog::slot_Delete);
	connect(m_btnApply, &QPushButton::clicked, this, &OmniversePresetAddOrModifyDialog::slot_Apply);
	connect(m_btnCancel, &QPushButton::clicked, this, &OmniversePresetAddOrModifyDialog::slot_Cancel);

	updateMeshMaterialTreeWidget();
	updatePrimTreeWidget();

	if (m_mode == eMode::Modify)
	{
		m_editPresetName->setEnabled(false);
		m_editPresetName->setText(m_pUsdPreset->GetPresetName().c_str());
	}
}

QTreeWidgetItem* OmniversePresetAddOrModifyDialog::createMeshToMaterialPairItem(const mipPresetMeshMaterialPair& pair)
{
	QTreeWidgetItem* item = new QTreeWidgetItem();
	QString meshName = pair.GetMeshName().c_str();
	QString materialName = pair.GetMaterialName().c_str();

	item->setText(0, meshName);
	item->setText(1, materialName);

	return item;
}

QTreeWidgetItem* OmniversePresetAddOrModifyDialog::createPrimItem(const std::string& name, const std::string& type)
{
	QTreeWidgetItem* item = new QTreeWidgetItem();
	item->setText(0, QString::fromStdString(name));
	item->setText(1, QString::fromStdString(type));

	return item;
}

void OmniversePresetAddOrModifyDialog::updateMeshMaterialTreeWidget()
{
	m_treeMeshMaterialInfo->clear();
	std::vector<mipPresetMeshMaterialPair> meshMaterialList = m_pUsdPreset->GetMeshToMaterialPairList();
	for (auto& pair : meshMaterialList)
	{
		m_treeMeshMaterialInfo->addTopLevelItem(createMeshToMaterialPairItem(pair));
	}

	for (int i = 0; i < m_treeMeshMaterialInfo->topLevelItemCount(); ++i)
	{
		m_treeMeshMaterialInfo->resizeColumnToContents(i);
	}
}

void OmniversePresetAddOrModifyDialog::updatePrimTreeWidget()
{
	m_treePrimInfo->clear();
	std::vector<mipUSDMaterialPtr> materialList = m_pUsdPreset->CreateMaterialList();
	for (auto& pMaterial : materialList)
	{
		std::string path = pMaterial->GetName();
		std::string type = pMaterial->GetTypeName();

		m_treePrimInfo->addTopLevelItem(createPrimItem(path, type));
	}

	std::vector<mipUsdLightPtr> lightList = m_pUsdPreset->CreateLightList();
	for (auto& pLight : lightList)
	{
		std::string path = pLight->GetName();
		std::string type = pLight->GetTypeName();

		m_treePrimInfo->addTopLevelItem(createPrimItem(path, type));
	}

	for (int i = 0; i < m_treePrimInfo->topLevelItemCount(); ++i)
	{
		m_treePrimInfo->resizeColumnToContents(i);
	}
}

void OmniversePresetAddOrModifyDialog::slot_LoadUsd()
{
	QString filters = "USD File(*.usd; *.USD)";
	QString filepath = QFileDialog::getOpenFileName(this, "export from usd file", QString(), filters);
	if (filepath.isEmpty())
	{
		return;
	}

	std::shared_ptr<mipUsdPreset> pPreset = std::make_shared<mipUsdPreset>();
	if (pPreset->OpenUsd(filepath.toStdString()) == false)
	{
		return;
	}
	m_pUsdPreset = pPreset;

	updateMeshMaterialTreeWidget();
	updatePrimTreeWidget();
}

void OmniversePresetAddOrModifyDialog::slot_Add()
{
	OmniversePresetAddNewTypeDialog dlg(this);
	dlg.Init(m_pUsdPreset);
	int result = dlg.exec();
	if (result == QDialog::Accepted)
	{
		updateMeshMaterialTreeWidget();
		updatePrimTreeWidget();
	}
}

void OmniversePresetAddOrModifyDialog::slot_Delete()
{
	OmniversePresetAddNewTypeDialog dlg(this);
	dlg.Init(m_pUsdPreset);
	int result = dlg.exec();
	if (result == QDialog::Accepted)
	{
		updateMeshMaterialTreeWidget();
		updatePrimTreeWidget();
	}
}

void OmniversePresetAddOrModifyDialog::slot_Apply()
{
	std::string rootPresetDirPath = m_pPresetManager->GetRootPresetDirectoryPath();
	std::string presetName = m_editPresetName->text().toStdString();

	if (presetName.empty())
	{
		return;
	}

	if (m_mode == eMode::AddNew)
	{
		saveNew(rootPresetDirPath, presetName);
	}
	else
	{
		saveModified(rootPresetDirPath);
	}

	accept();
}

void OmniversePresetAddOrModifyDialog::saveNew(const std::string& rootPresetDirPath, const std::string& presetName)
{
	if (m_pUsdPreset->IsOpen() == false)
	{
		QString message = QString("usd preset is empty");
		QMessageBox::warning(this, "warn", message);
		qWarning() << message;
		return;
	}

	if (m_pPresetManager->IsPresetExist(presetName))
	{
		QString message = QString("preset '%1' is already exists").arg(presetName.c_str());
		QMessageBox::warning(this, "warn", message);
		qWarning() << message;
		return;
	}

	if (m_pUsdPreset->SaveToPresetFile(rootPresetDirPath, presetName) == false)
	{
		qWarning() << "fail to save preset file : " << presetName.c_str();
		Q_ASSERT(false);
		return;
	}

	if (m_pPresetManager->Add(m_pUsdPreset) == false)
	{
		Q_ASSERT(false);
		return;
	}
}

void OmniversePresetAddOrModifyDialog::saveModified(const std::string& rootPresetDirPath)
{
	if (m_pUsdPreset->IsPresetFileLoaded() == false)
	{
		QString message = QString("modified usd must preset loaded");
		qWarning() << message;
		Q_ASSERT(false);
		return;
	}

	std::string presetName = m_pUsdPreset->GetPresetName();
	if (m_pUsdPreset->SaveToPresetFile(rootPresetDirPath, presetName) == false)
	{
		qWarning() << "fail to save preset file : " << presetName.c_str();
		Q_ASSERT(false);
		return;
	}

	/* usd 정보 변경 */
	m_pModifiedUsdPreset = m_pUsdPreset;
}

void OmniversePresetAddOrModifyDialog::slot_Cancel()
{
	reject();
}

