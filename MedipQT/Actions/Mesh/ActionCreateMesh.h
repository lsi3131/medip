#pragma once

#include "define.h"
#include "Renderer/Mesh.h"
#include "UI/MaskList.h"
#include "Actions/Mesh/CUndoRedo.h"
#include "graphics/Mesh/MeshPrimitiveBuilder.h"
#include <QtWidgets>
#include <Qthread>
#include <QUndoCommand>

class ActionCreateMesh : public QUndoCommand, CUndoRedo
{
public:
	ActionCreateMesh(DataContext* pDataContext, eMeshPrimitiveType type, mip::VECTOR3 vecSize, QString strName, int resolution_1, int resolution_2, QUndoCommand* parent = NULL);
	virtual ~ActionCreateMesh();

public:
	void undo() override;
	void redo() override;
	int id() const override { return m_id; }
private:
	int m_id;
	bool m_first;
	double m_Points[3];
	eMeshPrimitiveType m_type; //0-cube, 1-cylinder, 2-sphere
	mip::VECTOR3	m_size;
	int  m_resolution_1;
	int  m_resolution_2;
	mint8 m_UID;
	QString m_name;
	mip::MeshTopology* m_pMesh;
	MeshInfo m_meshInfo;

	DataContext* m_pDataContext;
	MeshPrimitiveBuilder m_meshPrimitiveBuilder;
};