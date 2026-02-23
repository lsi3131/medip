#pragma once

#include "mipEngine\Camera.h"
#include "CManipulator.h"
#include "DataContext.h"
#include <vector>

/*
@brief
*/
class CMeshWorkManager;
class CMeshModelViewManager;

namespace mip
{
	class Renderer;
}

class	CMeshManipulator
{
public:
	static CMeshManipulator* getInstance();

public:
	CMeshManipulator();
	CMeshManipulator(mip::Renderer* pRenderer);
	~CMeshManipulator();

public:
	void Init(DataContext* pDataContext);
	void Init(
		DataContext* pDataContext,
		CMeshWorkManager* pMeshWorkManager,
		CMeshModelViewManager* pMeshModelViewManager,
		CMeshDlgManager* pMeshDlgManager
	);

	void Process(int _idx_mesh, QPoint _prev_mouse_pt, QPoint _cur_mouse_pt);
	void render(
		mip::Renderer* _p_renderer,
		mip::MATRIX44& _mat_view_world,
		mip::MATRIX44& _mat_offset,
		mip::SCAMERA* _p_camera
	);
	void DrawSnapping(QPainter* _p_paint);

	bool CheckClickedArrow(QPoint _pt, mip::SCAMERA* _p_camera);
	void ReadyProcess(
		int _idx_mesh,
		QPoint _prev_mouse_pt,
		QPoint _cur_mouse_pt,
		mip::MATRIX44& _mat_view_world,
		mip::MATRIX44& _mat_offset,
		mip::SCAMERA* _p_camera
	);
	bool FinishProcess(int _idx_mesh);

	void OkProcess(int _idx_mesh);
	void CancelProcess(int _idx_mesh);

	void UpdatePivotPoint(mip::MeshTopology* _p_mesh, mip::MATRIX44& _matrix);

	bool isCheckedArrow();

	void SaveUndoRedo(int _idx);

	void deleteManipulator(int _idx);
	bool InsertManipulator(int idx);
	void clear();
	void indexChange(int _idx, int _dest_idx);

	void Reset();
	void HomePosition();

	void UpdatePrevTransform(int _idx_mesh);
	void UpdateInitTransform(int _idx_mesh);

	void UpdateTransformAxis(int _idx_mesh, int _idx_axis, mip::MATRIX44& _mat);
	void AddTransformAxis(int _idx_mesh, mip::MATRIX44& _mat);

	void UpdatePosition(mip::MeshTopology* _p_mesh, bool _init = true);

	//void setTransformMat(int _idx, mip::MATRIX44 & _mat);
	void setTransformMat(int _idx, mip::TRANSFORM& _mat);
	mip::TRANSFORM getTransformMat(int _idx);
	mip::MATRIX44 getTransformMatAxis(int _idx_mesh, int _idx_axis);

	void setPrevTransformMat(int _idx, mip::TRANSFORM& _mat);
	mip::TRANSFORM getPrevTransformMat(int _idx);
	mip::MATRIX44 getPrevTransformMatAxis(int _idx_mesh, int _idx_axis);

	std::vector<CManipulator*> GetManipulatorList();

private:
	bool IsValidate();

private:
	std::vector<CManipulator> m_vt_Manipulator;
	mip::Renderer* m_pRenderer;

	DataContext* m_pDataContext;
	CMeshWorkManager* m_pMeshWorkManager;
	CMeshModelViewManager* m_pMeshModelViewManager;
	CMeshDlgManager* m_pMeshDlgManager;
};

#define MESH_MANIPULATOR (CMeshManipulator::getInstance())
