#pragma once

#include "Actions/Mesh/CUndoRedo.h"
#include "Renderer/MeshTopology.h"
#include "graphics/DataContext.h"

class WindowManager;
class CMeshDlgManager;
class CPlaneManiplator;

namespace mip
{
	class Renderer;
}

struct ActionAddMeshesData
{
	mint32 LayerUID;
	QString MeshName;
	mip::MeshTopology* pMeshData;
};

class ActionAddMeshes : public QUndoCommand, public CUndoRedo
{
public:
	ActionAddMeshes(
		DataContext* pDataContext, 
		const std::vector<ActionAddMeshesData>& dataList,
		bool isUpScale = false,
		QUndoCommand* parent = NULL);

	ActionAddMeshes(
		DataContext* pDataContext,
		WindowManager* pWinManager,
		CMeshWorkManager* pMeshWorkManager,
		CMeshDlgManager* pMeshDlgManager,
		CPlaneManiplator* pPlaneManipulator,
		mip::Renderer* pRenderer,
		const std::vector<ActionAddMeshesData>& dataList,
		const QString& cacheDirectory,
		bool isUpScale = false,
		QUndoCommand* parent = NULL);

	//ActionAddMeshes(
	//	DataContext* pDataContext,
	//	WindowManager* pWinManager,
	//	CMeshWorkManager* pMeshWorkManager,
	//	CMeshDlgManager* pMeshDlgManager,
	//	CPlaneManiplator* pPlaneManipulator,
	//	mip::Renderer* pRenderer,
	//	const std::vector<MeshLayerData>& meshLayerList,
	//	const QString& cacheDirectory,
	//	bool isUpScale = false,
	//	QUndoCommand* parent = NULL);

	virtual ~ActionAddMeshes();

public:
	void Run();

public:
	void undo() override;
	void redo() override;
	int id() const override { return m_id; }

private:
	mip::PLANE getCTPlane(WINDOW_TYPE winType);

private:
	int m_id;

	bool m_isFirst;
	bool m_isUpScale;

	std::vector<mint32> m_newLayerUidList;
	std::vector<mip::MeshTopology*> m_newMeshTopologyList;

	std::vector<mint8> m_addedUidList;
	std::vector<MeshInfo> m_addedMeshInfoList;

	DataContext* m_pDataContext;

	WindowManager* m_pWinManager;
	CMeshWorkManager* m_pMeshWorkManager;
	CMeshDlgManager* m_pMeshDlgManager;
	CPlaneManiplator* m_pPlaneManipulator;

	mip::Renderer* m_pRenderer;
	QString m_cacheDirectory;
};

