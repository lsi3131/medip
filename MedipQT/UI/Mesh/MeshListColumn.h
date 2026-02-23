#pragma once

#define COLUMN_WIDTH_UNDEFINED (-1)

class MeshListColumn
{
public:
	MeshListColumn(MESH_COLUMN_AKA column, int index, const QString& name, int columnWidth);

public:
	bool HasColumnWidth() const;

	MESH_COLUMN_AKA GetKey() const;
	int GetIndex() const;
	QString GetName() const;
	int GetColumnWidth() const;

private:
	MESH_COLUMN_AKA m_column;
	int m_index;
	QString m_name;
	int m_columnWidth;
};


