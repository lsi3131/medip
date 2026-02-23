/*
@company		메디컬아이피
@autor			허 건 과장
@date			2021-01-14
@brief			CMeshDistancMananager 클래스 헤더파일
*/

#pragma once

#ifndef CMESHDISTANCE_MANAGER_H
#define CMESHDISTANCE_MANAGER_H

#include "Renderer/Renderer.h"

#include "DataContext.h"

class CMeshWorkManager;
class CMeshCutManager;
class CMeshViewBtn3DScene;

/*
@brief
*/
class  CMeshDistancMananager
{
public:
	CMeshDistancMananager();
	~CMeshDistancMananager();

	static CMeshDistancMananager* getInstance();

	void Init(DataContext* pDataContext);
	void Init(
		DataContext* pDataContext,
		CMeshWorkManager* pMeshWorkManager,
		CMeshCutManager* pMeshCutManager,
		CMeshViewBtn3DScene* pBtn3DScene
	);

	void Process();

	void ClearList();

	void Update(MESH_WORK_MODE _mode, bool iconRefresh);

private:
	mip::MeshTopology* m_pPickedMesh; //picking point 

	int m_pckIndex; //picking point index
	int m_movIndex;

	float m_pckLength;

	mip::COLOR m_pckColor;

	std::vector<mip::VECTOR3> m_lineList3D; //distance check
	std::vector<mip::VECTOR3> m_pointList3D; //mouse point list

	DataContext* m_pDataContext;
	CMeshWorkManager* m_pMeshWorkManager;
	CMeshCutManager* m_pMeshCutManager;
	CMeshViewBtn3DScene* m_pBtn3DScene;
};

#define MESH_DISTANCE_MANAGER (CMeshDistancMananager::getInstance())
#endif