#pragma once

#include "define.h"
#include "Renderer/Mesh.h"
#include "UI/MaskList.h"
#include "Math/Vector.h"
#include "mipEngine/Topology.h"
#include "mipEngine/OctreeTri.h"
#include <QtWidgets>
#include <Qthread>
#include <QUndoCommand>

namespace mip
{
	class MeshTopology;
}

class WindowManager;
class CMeshManipulator;
class CMeshHoleFillManager;
class CMeshWorkManager;
class CMeshDlgManager;
class CMeshViewRenderManager;
class CMeshCutManager;

class CUndoRedo
{
public:
	CUndoRedo();
	CUndoRedo(
		WindowManager* pWinManager,
		CMeshWorkManager* pMeshWorkManager
	);

	virtual ~CUndoRedo();

protected:
	void saveDataAllRedoFile(QString filePath, mip::MeshTopology* pMesh = NULL, MESH_WORK_MODE _work_mode = MESH_WORK_NONE);
	void loadDataAllRedoFile(QString filePath, mip::MeshTopology* pMesh = NULL, MESH_WORK_MODE _work_mode = MESH_WORK_NONE);

	void saveCoreVertexRedoFile(QString filePath, mip::MeshTopology* pMesh);
	void saveCoreTrisRedoFile(QString filePath, mip::MeshTopology* pMesh);
	void saveCoreNormalRedoFile(QString filePath, mip::MeshTopology* pMesh);
	void saveCoreColorRedoFile(QString filePath, mip::MeshTopology* pMesh, bool _b_single_mode = false);
	void saveCoreColorRedoFile(QString filePath, std::vector<mip::VECTOR4>& _vt_color, int _start, int _end);
	void saveCoreTextureRedoFile(QString filePath, mip::MeshTopology* pMesh);
	void saveTVertexRedoFile(QString filePath, mip::MeshTopology* pMesh, bool _b_single_mode = false);
	void saveTVertexRedoFile(QString filePath, std::vector<mip::TVert>& _vt_verts, int _start, int _end);
	void saveTTrisRedoFile(QString filePath, mip::MeshTopology* pMesh, bool _b_single_mode = false);
	void saveTTrisRedoFile(QString filePath, std::vector<mip::TTri>& _vt_tris, int _start, int _end);
	void saveTHedgeRedoFile(QString filePath, mip::MeshTopology* pMesh, bool _b_single_mode = false);
	void saveTHedgeRedoFile(QString filePath, std::vector<mip::THEdge>& _vt_edges, int _start, int _end);
	void saveTBrushDataFile(QString filePath, mip::MeshTopology* pMesh, bool _b_single_mode = false);
	void saveTBrushDataFile(QString filePath, std::vector<std::vector<muint32>>& _vt_verts, int _start, int _end);
	void saveOctreeRedoFile(QString filePath, mip::MeshTopology* pMesh);
	void saveOctreeRoot(QFile* _p_file, mip::OcNode* _p_root);
	void saveManipulatorInfo(DataContext* pDataContext, QString filePath, int _idx_mesh, bool _b_prev);
	void saveHoleInfo(QString filePath);

	void loadCoreVertexRedoFile(QString filePath, mip::MeshTopology* pMesh);
	void loadCoreTrisRedoFile(QString filePath, mip::MeshTopology* pMesh);
	void loadCoreNormalRedoFile(QString filePath, mip::MeshTopology* pMesh);
	void loadCoreColorRedoFile(QString filePath, mip::MeshTopology* pMesh, bool _b_single_mode = false);
	void loadCoreColorRedoFile(QString filePath, std::vector<mip::VECTOR4>& _vt_color, int _start, int _end);
	void loadCoreTextureRedoFile(QString filePath, mip::MeshTopology* pMesh);
	void loadTVertexRedoFile(QString filePath, mip::MeshTopology* pMesh, bool _b_single_mode = false);
	void loadTVertexRedoFile(QString filePath, std::vector<mip::TVert>& _vt_tverts, int _start, int _end);
	void loadTTrisRedoFile(QString filePath, mip::MeshTopology* pMesh, bool _b_single_mode = false);
	void loadTTrisRedoFile(QString filePath, std::vector<mip::TTri>& _vt_ttris, int _start, int _end);
	void loadTHedgeRedoFile(QString filePath, mip::MeshTopology* pMesh, bool _b_single_mode = false);
	void loadTHedgeRedoFile(QString filePath, std::vector<mip::THEdge>& _vt_thedge, int _start, int _end);
	void loadTBrushDataFile(QString filePath, mip::MeshTopology* pMesh, bool _b_single_mode = false);
	void loadTBrushDataFile(QString filePath, std::vector<std::vector<muint32>>& _vt_diplay, int _start, int _end);
	void loadOctreeRedoFile(QString filePath, mip::MeshTopology* pMesh);
	void loadOctreeRoot(QFile* _p_file, mip::OcNode* _p_root);
	bool loadManipulatorInfo(DataContext* pDataContext, QString filePath, int _idx_mesh);
	void loadHoleInfo(QString filePath);

	void updateBB(mip::MeshTopology* pMesh);

	void deleteUndoRedoFile(const QString& id, QDir* dir);
	bool compareFileNameUndoRedoId(const QString& id, const QString fileName);

protected:
	WindowManager* m_pWinManager;
	CMeshManipulator* m_pMeshManipulator;
	CMeshHoleFillManager* m_pMeshHoleFillManager;
	CMeshWorkManager* m_pMeshWorkManager;
	CMeshDlgManager* m_pMeshDlgManager;
	CMeshViewRenderManager* m_pMeshViewRenderManager;
	CMeshCutManager* m_pMeshCutManager;

private:
	std::vector<int> m_vt_LayerIndex;
};


