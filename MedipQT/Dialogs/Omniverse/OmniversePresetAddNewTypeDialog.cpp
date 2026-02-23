#include "stdafx.h"
#include "OmniversePresetAddNewTypeDialog.h"
#include "Omniverse/OmniverseContext.h"
#include "StringManager.h"
#include "StyleManager.h"

OmniversePresetAddNewTypeDialog::OmniversePresetAddNewTypeDialog(QWidget* parent) :
	QDialog(parent),
	m_pUsdPreset(nullptr)
{
	setupUi(this);

}

bool OmniversePresetAddNewTypeDialog::Init(mipUsdPresetPtr pUsdPreset)
{
	m_pUsdPreset = pUsdPreset;

	/* ComboBox Material */
	m_cboMaterialType->setStyleSheet(STYLE_MANAGER->comboBoxNormal);

	initMaterialComboBox();

	connect(m_btnAdd, &QPushButton::clicked, this, &OmniversePresetAddNewTypeDialog::slot_Add);
	connect(m_btnCancel, &QPushButton::clicked, this, &OmniversePresetAddNewTypeDialog::slot_Cancel);

	return true;
}

bool OmniversePresetAddNewTypeDialog::initMaterialComboBox() 
{
	m_cboMaterialType->clear();

	QString presetDirPath = STRING_MANAGER->OmniverseMaterialDirectoryPath_Preset;
	QDir dir(presetDirPath);
	QStringList filters;
	filters << "*.mdl";

	m_cboMaterialType->addItem("None", QString(""));
	QFileInfoList mdlFiieList = dir.entryInfoList(filters, QDir::Filter::Files);
	for (QFileInfo& fileInfo : mdlFiieList)
	{
		QString name = fileInfo.baseName();
		m_cboMaterialType->addItem(name, fileInfo.absoluteFilePath());
	}

	return true;
}

void OmniversePresetAddNewTypeDialog::slot_Add()
{
	/* 
		1. USD를 생성한다.
		2. 
	*/
	QString meshName = m_editBindMeshName->text();
	if (meshName.isEmpty())
	{
		qWarning() << "mesh name empty : " << meshName;
		return;
	}

	QString mdlFilePath = m_cboMaterialType->currentData().toString();
	if (mdlFilePath.isEmpty())
	{
		qWarning() << "mdl file path empty : " << mdlFilePath;
		return;
	}

	QString materialName = m_cboMaterialType->currentText();

	if (m_pUsdPreset->AddMaterialByMdlFile(mdlFilePath.toStdString(), materialName.toStdString()) == false)
	{
		qWarning() << "fail to add material";
		return;
	}

	if (m_pUsdPreset->BindMeshToMaterial(meshName.toStdString(), materialName.toStdString()) == false)
	{
		qCritical() << "fail to bind";
		return;
	}

	accept();
}

void OmniversePresetAddNewTypeDialog::slot_Cancel()
{
	reject();
}

