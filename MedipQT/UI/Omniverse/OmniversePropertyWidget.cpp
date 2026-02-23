#include "stdafx.h"
#include "OmniversePropertyWidget.h"
#include "StringManager.h"
#include "styleManager.h"
#include "DataContext.h"
#include "Omniverse/OmniverseContext.h"
#include "Omniverse/Usd/mipUsdStage.h"

#define MATEIRAL_NONE_FILE "None"

OmniversePropertyWidget::OmniversePropertyWidget(QWidget* parent) :
	QWidget(parent),
	m_pDataContext(nullptr)
{
	setupUi(this);

	m_groupPhysics->setVisible(false);
}

bool OmniversePropertyWidget::Init(DataContext* pDataContext)
{
	m_pDataContext = pDataContext;
	m_pOmniverse = m_pDataContext->GetOmniverseContext();

	/* ComboBox Material */
	m_cboMaterial->setStyleSheet(STYLE_MANAGER->comboBoxNormal);
	m_cboMaterial->addItem(MATEIRAL_NONE_FILE, QString(""));

	connect(m_cboMaterial, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_materialChanged(int)));

	updateMaterialComoBox();

	return true;
}

void OmniversePropertyWidget::updateMaterialComoBox()
{
	QString directoryPath = STRING_MANAGER->OmniverseMaterialDirectoryPath_Preset;

	QDir dir(directoryPath);
	QStringList filters;
	filters << "*.mdl";

	QFileInfoList mdlFiieList = dir.entryInfoList(filters, QDir::Filter::Files);
	for (QFileInfo& fileInfo : mdlFiieList)
	{
		QString name = fileInfo.baseName();
		m_cboMaterial->addItem(name, fileInfo.absoluteFilePath());
	}
}

void OmniversePropertyWidget::slot_materialChanged(int index)
{
	if (m_pOmniverse->GetStage()->IsOpen() == false)
	{
		return;
	}

	MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetCurrentMeshInfo();
	if (pMeshInfo == nullptr)
	{
		qWarning() << "current mesh info is empty";
		return;
	}
	QString meshName = pMeshInfo->GetName();

	QString materialFilepath = m_cboMaterial->currentData().toString();
	QString materialName = m_cboMaterial->currentText();
	QString materialId = materialName;

	if (materialFilepath.isEmpty())
	{
		m_pOmniverse->GetStage()->UnbindMeshMaterial(
			meshName.toStdString(), 
			materialName.toStdString());
	}
	else
	{
		if (m_pOmniverse->GetStage()->AddMaterialByMDLFile(
			materialFilepath.toStdString(),
			materialName.toStdString(),
			materialId.toStdString()) == false)
		{
			return;
		}

		if (m_pOmniverse->GetStage()->BindMeshMaterial(
			meshName.toStdString(),
			materialName.toStdString()) == false)
		{
			return;
		}

		qInfo() << "set mesh : " << pMeshInfo->GetName() << " to material : " << materialFilepath;
	}

	emit m_pOmniverse->GetStage()->sig_StageDataChanged();
}


