/*
@company		메디컬아이피
@autor			허 건 과장
@date			2021-01-08
@brief			CMeshDlgManager 헤더파일
*/

#pragma once

#ifndef CMESH_DLG_MANAGER_H
#define CMESH_DLG_MANAGER_H

#include <qwidget.h>

#include "BrushSculptDlg.h"
#include "BrushSelectDlg.h"
#include "SmoothDlg.h"
#include "ReduceDlg.h"
#include "HollowDlg.h"
#include "Stamp3dDlg.h"
#include "CRemeshDlg.h"
#include "CSolidDlg.h"
#include "CHoleFillingDlg.h"
#include "CPlaneCutDlg.h"
#include "CManpulateDlg.h"
#include "CIslandFilterDlg.h"
#include "CSubDivisionDlg.h"
#include "CMeshCutDlg.h"
#include "MeshOffsetDlg.h"

#include "DataContext.h"

class MEVolumeView;
class CMeshWorkManager;
class CMeshDlgManager;
class CMeshViewBtn3DScene;

enum MESH_DIALOG_TYPE
{
	MESH_DIALOG_SCULPT,
	MESH_DIALOG_SELECT,
	MESH_DIALOG_SMOOTH,
	MESH_DIALOG_REDUCE,
	MESH_DIALOG_HOLLOW,
	MESH_DIALOG_REMESH,
	MESH_DIALOG_SOLID,
	MESH_DIALOG_FILLHOLE,
	//MESH_DIALOG_PLANECUT,
	MESH_DIALOG_MESHCUT,
	MESH_DIALOG_MANIPULATE,
	MESH_DIALOG_ISLANDFILTER,
	MESH_DIALOG_SUBDIVISION,
	MESH_DIALOG_STAMP3D,
	MESH_DIALOG_MESHOFFSET
};

/*
@brief
*/
class	CMeshDlgManager
{
public:
	CMeshDlgManager();
	~CMeshDlgManager();

	static CMeshDlgManager* getInstance();

	void Init(DataContext* pDataContext, MEVolumeView* pViewer);
	void Init(
		DataContext* pDataContext,
		MEVolumeView* pViewer,
		CMeshWorkManager* pWorkManager,
		CMeshViewBtn3DScene* pBtn3DScene
	);

	bool makeMeshDialog(int mode, bool _b_force = false, bool bClearFlag = true);
	bool isMeshDialog();
	void rejectDialog(int old_idx = -1, bool bForce = false);
	void rejectDialog(MESH_DIALOG_TYPE _type);
	void closeDialog();
	QDialog* getMeshDialog();
	void* getMeshDialog(MESH_DIALOG_TYPE _type);
	void* getMeshDialog(MESH_WORK_MODE _type);

	void MoveMeshDialog(int fixed = 0); //default - 0, fixed Right - 1 , fixed Left - 2

	int getSculptMode();

public:
	int	 m_oldMode;

private:
	BrushSculptDlg* m_pSculptDlg;
	BrushSelectDlg* m_pSelectDlg;
	SmoothDlg* m_pSmoothDlg;
	ReduceDlg* m_pReduceDlg;
	HollowDlg* m_pHollowDlg;
	Stamp3dDlg* m_pStamp3dDlg;
	MeshOffsetDlg* m_pMeshOffsetDlg;
	CRemeshDlg* m_pRemeshDlg;
	CSolidDlg* m_pSolidDlg;
	CHoleFillingDlg* m_pHoleFillDlg;
	//CPlaneCutDlg*		m_pPlaneCutDlg;
	CMeshCutDlg* m_pMeshCutDlg;
	CManpulateDlg* m_pManpulateDlg;
	CIslandFilterDlg* m_pIslandFilterDlg;
	CSubDivisionDlg* m_pSubDivisionDlg;

	DataContext* m_pDataContext;
	MEVolumeView* m_pViewer;

	CMeshWorkManager* m_pWorkManager;
	CMeshViewBtn3DScene* m_pBtn3DScene;
};

#define MESH_DIALOG_MANAGER (CMeshDlgManager::getInstance())
#endif