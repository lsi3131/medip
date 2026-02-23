#include "stdafx.h"
#include "OmniverseStageListWidget.h"
#include "graphics/DataContext.h"
#include "Omniverse/Usd/mipUsdStage.h"
#include "Omniverse/Converter/UsdMeshPureDataConverter.h"
#include "styleManager.h"

OmniverseStageListWidget::OmniverseStageListWidget(QWidget* parent) :
	QTreeWidget(parent),
	m_pDataContext(nullptr),
	m_pUsdStage(nullptr)
{
}

bool OmniverseStageListWidget::Init(DataContext* pDataContext, mipUsdStage* pUsdStage)
{
	m_pDataContext = pDataContext;
	m_pUsdStage = pUsdStage;

	setStyleSheet(STYLE_MANAGER->treeOmniversePresetList);

	setHeaderLabels({ "Name", "Type", "Status"});

	Update();

	return true;
}

void OmniverseStageListWidget::Update()
{
	clear();

	if ((m_pDataContext == nullptr) || (m_pUsdStage == nullptr))
	{
		return;
	}

	if (m_pUsdStage->IsOpen() == false)
	{
		return;
	}

	std::vector<mipUSDMeshPtr> meshList = m_pUsdStage->CreateMeshList();
	for (auto& pMesh : meshList)
	{
		QTreeWidgetItem* item = new QTreeWidgetItem();
		item->setText(eColumn::Name, QString::fromStdString(pMesh->GetName()));
		item->setText(eColumn::Type, "Mesh");

		bool canConvertable = UsdMeshPureDataConverter::CanConvertable(*pMesh, eSupportMeshTypes::MipMesh);
		QString statusText;
		if (canConvertable)
		{
			statusText = "Supported";
		}
		else
		{
			statusText = "Not Supported";
		}

		item->setText(eColumn::Status, statusText);
		addTopLevelItem(item);
	}
}

void OmniverseStageListWidget::updateMaterialInfo()
{
	//clear();
	//std::vector<mipUSDMeshPtr> meshList = m_pUsdStage->CreateMeshList();
	//for (auto& pMesh : meshList)
	//{
	//	QTreeWidgetItem* item = new QTreeWidgetItem();
	//	item->setText(0, QString::fromStdString(pMesh->GetName()));
	//	item->setText(1, "Mesh");
	//	addTopLevelItem(item);
	//}

	//std::vector<mipUSDMaterialPtr> materialList = m_pUsdStage->CreateMaterialList();
	//for (auto& pMat : materialList)
	//{
	//	QTreeWidgetItem* item = new QTreeWidgetItem();
	//	item->setText(0, QString::fromStdString(pMat->GetName()));
	//	item->setText(1, "Material");
	//	addTopLevelItem(item);
	//}

	//std::vector<mipUsdLightPtr> lightList = m_pUsdStage->CreateLightList();
	//for (auto& pLight : lightList)
	//{
	//	QTreeWidgetItem* item = new QTreeWidgetItem();
	//	item->setText(0, QString::fromStdString(pLight->GetName()));
	//	item->setText(1, "Light");
	//	addTopLevelItem(item);
	//}
}

