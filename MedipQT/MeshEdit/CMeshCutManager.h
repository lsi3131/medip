/*
@company		메디컬아이피
@autor 허 건 과정
@date 2021-01-08
@brief CMeshCutManager 헤더파일
*/

#pragma once

#ifndef CMESHCUT_MANAGER_H
#define CMESHCUT_MANAGER_H

#include "defineMEDIP.h"

#include <qwidget.h>
#include "mipEngine/Camera.h"
#include "DataContext.h"

class MEVolumeView;
class CMeshViewBtn3DScene;
class CMeshWorkManager;
class CMeshModelViewManager;
class CMeshDlgManager;
class CPlaneManiplator;

/*
@brief	메쉬 컷 관리 클래스
*/
class	CMeshCutManager : public QObject
{
	Q_OBJECT

public:
	CMeshCutManager();
	~CMeshCutManager();

	static CMeshCutManager* getInstance();

	void Init(DataContext* pDataContext, MEVolumeView* pViewer);
	void Init(
		DataContext* pDataContext,
		MEVolumeView* pViewer,
		CMeshViewBtn3DScene* pBtn3DScene,
		CMeshWorkManager* pWorkManager,
		CMeshModelViewManager* pModelViewManager,
		CMeshDlgManager* pDlgManager,
		CPlaneManiplator* pPlaneManipulator
	);

	void Update(MESH_WORK_MODE _mode);

	bool Process(MESH_WORK_MODE _mode);

	void setPlaneCutFlag(bool _b_flag);
	bool* getPlaneCutFlag();

	void setCutFlag(bool _b_flag);
	bool* getCutFlag();

	void setPolyProcessFlag(bool _b_flag) { m_polyProcessCheck = _b_flag; }
	bool getPolyProcessFlag() { return m_polyProcessCheck; }

	QAction* getActionPtr(MESH_WORK_MODE _work);

	QPolygon* getPolyLinePtr();

	std::vector<std::vector<std::vector<mip::VECTOR3>>>* getHolePtr();

	bool process_FinishCut();
	bool process_CancelCut();
	void clearPlaneCutParam();

private:
	bool process_PlaneCut();
	bool process_FreePolyCut();
	bool process_PolyOutCut();
	bool process_PolyInnerCut();

	void calcPolyLineFromPlane(std::vector<mip::VECTOR2>& _vt_polygon);
	void getPlane(mip::PLANE& _plane);

private:
	QAction* m_ActPlane;
	QAction* m_ActPolygon;
	QAction* m_ActPolyline;
	QAction* m_ActPolyFree;

	bool m_b_planeCut = false;
	bool m_b_Cut = false;

	bool m_polyProcessCheck = false;

	QPolygon m_polyLine;

	mip::VECTOR2	m_polygonsPlaneTest[2];

	// 200811 plane cut(허 건 대리)
	std::vector<std::vector<std::vector<mip::VECTOR3>>> m_vt_HolePts;

	DataContext* m_pDataContext;

	MEVolumeView* m_pViewer;
	CMeshViewBtn3DScene* m_pBtn3DScene;
	CMeshWorkManager* m_pWorkManager;
	CMeshModelViewManager* m_pModelViewManager;
	CMeshDlgManager* m_pDlgManager;
	CPlaneManiplator* m_pPlaneManipulator;
};

#define MESH_CUT_MANAGER (CMeshCutManager::getInstance())
#endif