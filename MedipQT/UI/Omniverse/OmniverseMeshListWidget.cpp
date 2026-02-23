#include "stdafx.h"
#include "OmniverseMeshListWidget.h"
#include "StyleManager.h"
#include "ResourceManager.h"
#include "volumedata.h"
#include "Omniverse/OmniverseContext.h"

OmniverseMeshListWidget::OmniverseMeshListWidget(QWidget* parent) :
	QTreeWidget(parent),
	m_pOmniverse(nullptr)
{

}

void OmniverseMeshListWidget::Init(OmniverseContext* pOmniverse)
{
	this->setHeaderHidden(false);
	this->setHeaderLabels({ "Sub", "Color", "Show", "Name", "Online", "Edit Mode"});
	this->setColumnCount(eColumn::COL_COUNT);
	this->header()->setMinimumSectionSize(20);
	this->setColumnWidth(eColumn::COL_SUB, 20);
	this->setColumnWidth(eColumn::COL_COLOR, 20);
	this->setColumnWidth(eColumn::COL_SHOW, 20);
	this->setColumnWidth(eColumn::COL_NAME, 40);
	this->setColumnWidth(eColumn::COL_ONLINE, 20);
	this->setColumnWidth(eColumn::COL_EDIT_MODE, 20);
	//this->setHeaderHidden(true);
	this->setStyleSheet(STYLE_MANAGER->treeBasicList);
	this->setAutoFillBackground(true);
	this->setContentsMargins(0, 0, 0, 0);
	this->setSelectionMode(QAbstractItemView::SelectionMode::MultiSelection);

	m_pOmniverse = pOmniverse;

	updateList();
}

std::vector<MeshLayerData> OmniverseMeshListWidget::GetSelectedMeshList() const
{
	return m_meshDataList;
}

void OmniverseMeshListWidget::Update()
{
	updateList();
}

void OmniverseMeshListWidget::updateList()
{
	/* MeshDataList 초기화 */
	//m_meshDataList = m_pOmniverse->GetMeshDataList();

	///* TreeItem 초기화 */
	//for (int i = 0; i < topLevelItemCount(); ++i)
	//{
	//	delete this->topLevelItem(i);
	//}

	//this->clear();

	//static QIcon icons[] = { RESOURCE_MANAGER->getIcon(ICON_LIST_INVISIBLE), RESOURCE_MANAGER->getIcon(ICON_LIST_VISIBLE) };

	//for (MeshLayerData& m : m_meshDataList)
	//{
	//	COLOR col = m.Info->color;

	//	QTreeWidgetItem* item = new QTreeWidgetItem();
	//	item->setText(M_COL_SUB, "+");
	//	item->setBackground(M_COL_COLOR, QBrush(QColor(col.r, col.g, col.b, 255)));
	//	item->setIcon(M_COL_SHOW, icons[m.Info->show]);
	//	item->setText(M_COL_NAME, QString::fromWCharArray(m.Info->MeshName));

	//	item->setData(0, Qt::UserRole, qVariantFromValue<void*>(&m));

	//	insertTopLevelItem(0, item);
	//}
}

