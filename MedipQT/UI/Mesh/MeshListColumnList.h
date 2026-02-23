#pragma once

#include <memory>
#include <unordered_map>
#include "defineMEDIP.h"
#include "UI/Mesh/MeshListColumn.h"

class ProductManager;

class MeshListColumnList
{
public:
	enum eType
	{
		Header,
		Widget
	};
public:
	static std::shared_ptr<MeshListColumnList> New(ProductManager* pProductManager, eType type);

public:
	MeshListColumnList(ProductManager* pProductManager, eType type);

public:
	const MeshListColumn* GetByKey(MESH_COLUMN_AKA key) const;
	const MeshListColumn* GetByIndex(int index) const;
	std::vector<const MeshListColumn*> GetList() const;
	int GetColumnCount() const;

private:
	ProductManager* m_pProductManager;
	std::unordered_map<MESH_COLUMN_AKA, MeshListColumn> m_columnTable;
	eType m_type;
};
