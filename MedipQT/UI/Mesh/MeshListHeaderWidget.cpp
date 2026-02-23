#include "stdafx.h"
#include "MeshListHeaderWidget.h"
#include "styleManager.h"
#include "ResourceManager.h"
#include "WindowManager.h"
#include "Mesh/MeshListColumnList.h"
#include "StringManager.h"

MeshListHeaderWidget::MeshListHeaderWidget(std::shared_ptr<MeshListColumnList> pColumnList, WindowManager* pWinManager, DataContext* pDataContext, QWidget* parent) :
	QTreeWidget(parent),
	m_pWinManager(pWinManager),
	m_pDataContext(pDataContext),
	m_pColumnList(pColumnList),
	m_isExpand(false)
{
	m_brushes = { QColor(0, 0, 0, 0) , QColor(48, 48, 48) };
	m_textArrowes = { QString(QChar(0x25B2)), QString(QChar(0x25BC)) };
}

void MeshListHeaderWidget::Init()
{
	header()->setMinimumSectionSize(20);

	setColumnCount(m_pColumnList->GetColumnCount());
	for (auto& pCol : m_pColumnList->GetList())
	{
		if (pCol->HasColumnWidth())
		{
			setColumnWidth(pCol->GetIndex(), pCol->GetColumnWidth());
		}
	}

	setHeaderHidden(true);
	setSelectionMode(QTreeWidget::NoSelection);
	setStyleSheet(STYLE_MANAGER->treeHeader);
	setAutoFillBackground(true);
	setHidden(true);	//show after load
	setContentsMargins(0, 0, 0, 0);
	setFocusPolicy(Qt::NoFocus);
	setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
	setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);

	QString value;
	if (!m_pWinManager->getConfigValue(ELEMENT_FILE, STRING_MANAGER->config_expandMesh, value))
	{
		m_isExpand = false;
	}
	else if (0 >= value.toInt())
	{
		m_isExpand = false;
	}
	else
	{
		m_isExpand = true;
	}
}

void MeshListHeaderWidget::SetExpand(bool isExpand)
{
	if (m_isExpand != isExpand)
	{
		m_isExpand = isExpand;

		m_pWinManager->setConfigValue(ELEMENT_FILE, STRING_MANAGER->config_expandMesh, QString::number(m_isExpand));
		UpdateExpand();
		emit sig_expandChanged(m_isExpand);
	}
}

bool MeshListHeaderWidget::IsExpand() const
{
	return m_isExpand;
}

bool MeshListHeaderWidget::ResetColumnAll()
{
	clear();
	QTreeWidgetItem* item = new QTreeWidgetItem(this);
	insertTopLevelItem(0, item);

	UpdateShow();
	UpdateLock();
	UpdateExpand();
	UpdateMaterial();

	return true;
}

bool MeshListHeaderWidget::UpdateColumn(MESH_COLUMN_AKA column)
{
	const MeshListColumn* pColumn = m_pColumnList->GetByKey((MESH_COLUMN_AKA)column);
	if (pColumn == nullptr)
	{
		return false;
	}

	if (pColumn->GetKey() == M_COL_SHOW)
	{
		UpdateShow();
	}
	else if (pColumn->GetKey() == M_COL_LOCK)
	{
		UpdateLock();
	}
	else
	{
		if (isExpandColumn(column))
		{
			UpdateExpand();
		}
	}

	return true;
}

bool MeshListHeaderWidget::ToggleColumn(MESH_COLUMN_AKA column)
{
	if (column == M_COL_SHOW)
	{
		ToggleShow();
	}
	else if (column == M_COL_LOCK)
	{
		ToggleLock();
	}
	else
	{
		if (isExpandColumn(column))
		{
			ToggleExpand();
		}
	}

	return true;
}

bool MeshListHeaderWidget::ItemClicked(QTreeWidgetItem* item, int column)
{
	if (item == nullptr)
	{
		return false;
	}

	const MeshListColumn* pColumn = m_pColumnList->GetByIndex(column);
	if (pColumn == nullptr)
	{
		return false;
	}

	ToggleColumn(pColumn->GetKey());

	return true;
}

void MeshListHeaderWidget::UpdateShow()
{
	QTreeWidgetItem* item = topLevelItem(0);
	if (item == nullptr)
	{
		return;
	}

	const MeshListColumn* pColumn = m_pColumnList->GetByKey(M_COL_SHOW);
	if (pColumn == nullptr)
	{
		return;
	}

	bool isVisible = m_pDataContext->m_MeshData.IsVisibleLayerExist();
	item->setIcon(pColumn->GetIndex(), RESOURCE_MANAGER->GetIcon_ListVisible(isVisible));
	item->setBackground(pColumn->GetIndex(), m_brushes[isVisible]);
	item->setForeground(pColumn->GetIndex(), m_brushes[isVisible]);
}

void MeshListHeaderWidget::UpdateLock()
{
	QTreeWidgetItem* item = topLevelItem(0);
	if (item == nullptr)
	{
		return;
	}

	const MeshListColumn* pColumn = m_pColumnList->GetByKey(M_COL_LOCK);
	if (pColumn == nullptr)
	{
		return;
	}

	bool isLock = m_pDataContext->m_MeshData.IsLockLayerExist();
	eOmniverseStatus status = isLock ? eOmniverseStatus::Lock : eOmniverseStatus::Unlock;

	item->setIcon(pColumn->GetIndex(), RESOURCE_MANAGER->GetIcon_ListOmniverseLock(status));
	item->setBackground(pColumn->GetIndex(), m_brushes[isLock]);
	item->setForeground(pColumn->GetIndex(), m_brushes[isLock]);
}

void MeshListHeaderWidget::UpdateExpand()
{
	QTreeWidgetItem* item = topLevelItem(0);
	if (item == nullptr)
	{
		return;
	}

	int columnArrowIndex = getExpandArrowColumnIndex();
	item->setText(columnArrowIndex, m_textArrowes[m_isExpand]);
}

void MeshListHeaderWidget::UpdateMaterial()
{
	QTreeWidgetItem* item = topLevelItem(0);
	if (item == nullptr)
	{
		return;
	}

	const MeshListColumn* pColumn = m_pColumnList->GetByKey(M_COL_MATERIAL);
	if (pColumn == nullptr)
	{
		return;
	}

	item->setText(pColumn->GetIndex(), QString("Material"));
}

void MeshListHeaderWidget::ToggleShow()
{
	bool isVisible = m_pDataContext->m_MeshData.IsVisibleLayerExist();

	for (auto& meshLayer : m_pDataContext->m_MeshData.GetMeshDataList())
	{
		meshLayer.Info->show = !isVisible;
	}
	UpdateShow();

	emit sig_columnStateChanged(M_COL_SHOW);
}

void MeshListHeaderWidget::ToggleLock()
{
	bool isLock = m_pDataContext->m_MeshData.IsLockLayerExist();

	for (auto& meshLayer : m_pDataContext->m_MeshData.GetMeshDataList())
	{
		eOmniverseStatus status = !isLock ? eOmniverseStatus::Lock : eOmniverseStatus::Unlock;
		meshLayer.Info->SetOmniverseStatus(status);
	}
	UpdateLock();

	emit sig_columnStateChanged(M_COL_LOCK);
}

void MeshListHeaderWidget::ToggleExpand()
{
	SetExpand(!m_isExpand);
}

int MeshListHeaderWidget::getExpandArrowColumnIndex() const
{
	int columnArrowIndex = m_pColumnList->GetColumnCount() - 1;

	return columnArrowIndex;
}

bool MeshListHeaderWidget::isExpandColumn(MESH_COLUMN_AKA column) const
{
	const MeshListColumn* pColumn = m_pColumnList->GetByKey(column);
	if (pColumn == nullptr)
	{
		return false;
	}

	return pColumn->GetIndex() == getExpandArrowColumnIndex();
}


