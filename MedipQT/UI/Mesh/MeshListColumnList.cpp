#include "stdafx.h"
#include "MeshListColumnList.h"
#include "System/ProductManager.h"

std::shared_ptr<MeshListColumnList> MeshListColumnList::New(ProductManager* pProductManager, eType type)
{
	return std::make_shared<MeshListColumnList>(pProductManager, type);
}

MeshListColumnList::MeshListColumnList(ProductManager* pProductManager, eType type) :
	m_pProductManager(pProductManager),
	m_type(type)
{

	int index = 0;
	m_columnTable.insert(std::make_pair(M_COL_SUB, MeshListColumn{ M_COL_SUB, index++, "Sub", 20 }));
	m_columnTable.insert(std::make_pair(M_COL_COLOR, MeshListColumn{ M_COL_COLOR, index++, "Color", 20 }));
	m_columnTable.insert(std::make_pair(M_COL_SHOW, MeshListColumn{ M_COL_SHOW, index++, "Show", 20 }));
	m_columnTable.insert(std::make_pair(M_COL_NAME, MeshListColumn{ M_COL_NAME, index++, "Name", COLUMN_WIDTH_UNDEFINED }));

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_DEV_NvidiaOmniverse))
	{
		m_columnTable.insert(std::make_pair(M_COL_MATERIAL, MeshListColumn{ M_COL_MATERIAL, index++, "Material", COLUMN_WIDTH_UNDEFINED }));
		m_columnTable.insert(std::make_pair(M_COL_LOCK, MeshListColumn{ M_COL_LOCK, index++, "Lock", 20 }));
	}

	if (m_type == eType::Header)
	{
		m_columnTable.insert(std::make_pair(M_COL_COUNT, MeshListColumn{ M_COL_COUNT, index++, "Count", 20 }));
	}
}

const MeshListColumn* MeshListColumnList::GetByKey(MESH_COLUMN_AKA key) const
{
	auto it = m_columnTable.find(key);
	if (it != m_columnTable.end())
	{
		return &(*it).second;
	}
	return nullptr;
}

const MeshListColumn* MeshListColumnList::GetByIndex(int index) const
{
	for (auto& keyValue : m_columnTable)
	{
		if (keyValue.second.GetIndex() == index)
		{
			return &keyValue.second;
		}
	}
	return nullptr;
}

std::vector<const MeshListColumn*> MeshListColumnList::GetList() const
{
	std::vector<const MeshListColumn*> columnList;
	for (auto& it : m_columnTable)
	{
		columnList.push_back(&it.second);
	}
	 
	return columnList;
}

int MeshListColumnList::GetColumnCount() const
{
	return m_columnTable.size();
}

