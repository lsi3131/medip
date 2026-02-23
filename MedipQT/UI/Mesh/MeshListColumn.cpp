#include "stdafx.h"
#include "MeshListColumn.h"

MeshListColumn::MeshListColumn(MESH_COLUMN_AKA column, int index, const QString& name, int columnWidth) :
	m_column(column),
	m_index(index),
	m_name(name),
	m_columnWidth(columnWidth)
{
}

bool MeshListColumn::HasColumnWidth() const
{
	return (m_columnWidth != COLUMN_WIDTH_UNDEFINED);
}

MESH_COLUMN_AKA MeshListColumn::GetKey() const
{
	return m_column;
}

int MeshListColumn::GetIndex() const
{
	return m_index;
}

QString MeshListColumn::GetName() const
{
	return m_name;
}

int MeshListColumn::GetColumnWidth() const
{
	return m_columnWidth;
}

