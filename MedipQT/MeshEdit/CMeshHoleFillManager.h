/*
@company		메디컬아이피
@autor			허 건 과장
@date			2021-01-20
@brief			CMeshHoleFillManager 헤더파일
*/

#pragma once

#ifndef CMESH_HOLEFILLING_MANAGER_H
#define CMESH_HOLEFILLING_MANAGER_H

#include "DataContext.h"

class MEVolumeView;
class CMeshViewBtn3DScene;
class CMeshModelViewManager;
class CMeshDlgManager;

/*
@brief
*/
class	CMeshHoleFillManager : public QObject
{
	Q_OBJECT

public:

	static CMeshHoleFillManager* getInstance();

	CMeshHoleFillManager();
	~CMeshHoleFillManager();

	void Init(DataContext* pDataContext, MEVolumeView* pViewer);
	void Init(
		DataContext* pDataContext,
		MEVolumeView* pViewer,
		CMeshViewBtn3DScene* pBtn3DScene,
		CMeshModelViewManager* pModelViewManager,
		CMeshDlgManager* pDlgManager
	);

	QAction* getAction()
	{
		return m_Act_HoleFill;
	}

	std::vector<std::pair<std::vector<int>, bool>>* getBoundaryIdxPtr();

	void Process(MESH_WORK_MODE _work);

	void CalcNearestBoundary(QPoint _pt);

	void Update(MESH_WORK_MODE _mode);

	void drawHoleLine(QPainter* _p_paint);

	void setMeshIdx(int _idx)
	{
		m_PickMeshIdx = _idx;
	}

	void FillHoleAll();

private:
	void FindHole();
	void SelectedHoleFill();


	bool ContainsPoint(QPoint _pt, std::vector<int>& _vt_idx);

private:
	QAction* m_Act_HoleFill;

	int m_Current_HoleIdx;

	int m_PickMeshIdx;

	std::vector<std::vector<mip::VECTOR3>> m_vt_boundary_pts;
	std::vector<std::pair<std::vector<int>, bool>> m_vt_boundary_pts_idx;
	std::vector<float> vt_sz_hole;

	DataContext* m_pDataContext;
	CMeshViewBtn3DScene* m_pBtn3DScene;
	CMeshModelViewManager* m_pModelViewManager;
	CMeshDlgManager* m_pDlgManager;
};

#define MESH_HOLE_MANAGER (CMeshHoleFillManager::getInstance())
#endif