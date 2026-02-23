/*
@company 메디컬아이피
@autor  허 건 과장
@date  2021-01-11
@brief  CMeshModelViewManager 헤더파일
*/

#pragma once

#ifndef CMESH_MODELVIEW_MANAGER_H
#define CMESH_MODELVIEW_MANAGER_H

#include "mipEngine/Camera.h"
#include "math/math.h"
#include "Renderer/Mesh.h"
#include "Renderer/model.h"

#include "DataContext.h"
#include "Windows/MEVolumeView.h"

class CMeshViewBtn3DScene;
class CMeshWorkManager;
class CMeshManipulator;
class CPlaneManiplator;
class ActionManager;
class WindowManager;
class DataContext;

class CMeshModelViewManager : public QObject
{
	Q_OBJECT

public:
	static CMeshModelViewManager* getInstance();
public:
	CMeshModelViewManager();
	~CMeshModelViewManager();

	void Init(DataContext* pDataContext, MEVolumeView* pViewer);
	void Init(
		DataContext* pDataContext,
		MEVolumeView* pViewer,
		CMeshViewBtn3DScene* btn3DScene,
		CMeshWorkManager* pWorkManager,
		CMeshManipulator* pMeshManipulator,
		CPlaneManiplator* pPlaneManipulator,
		ActionManager* pActionManager,
		WindowManager* pWinManager
	);

	void Init(
		DataContext* pDataContext,
		CMeshViewBtn3DScene* btn3DScene,
		CMeshWorkManager* pWorkManager,
		CMeshManipulator* pMeshManipulator,
		CPlaneManiplator* pPlaneManipulator,
		ActionManager* pActionManager,
		WindowManager* pWinManager
	);

	void SetView(MEVolumeView* pViewer);

	void Reset();
	void HomePosition();

	void pckMeshModel(QMouseEvent* e, QPoint _mouse_pt, bool _b_shift);
	void updatePckMeshAll();
	void updatePckMesh(int _idx, bool _b_flag);

	void MoveScreenCenterMesh(bool _b_all = true);
	void MoveScreenCenterMesh(int _idx);
	void UpdatePivotPoint(int _idx);
	void updatePivotPointGroup();
	void SetPivotPoint(mip::VECTOR3 _point);

	bool translateMeshModel();
	bool rotateMeshModel(bool _b_shift);
	void UpdatePivotPoint();

	void MeshFrontView();
	void MeshBackView();
	void MeshTopView();
	void MeshBottomView();
	void MeshLeftView();
	void MeshRightView();
	void MeshInitView(mip::TRANSFORM* _p_transform);
	mip::VECTOR3 getCetnerMeshes();
	mip::VECTOR3 getVolumeOffset();

	void initMeshTransform(int _idx);
	void initMeshTransformAll();

	mip::VECTOR3 getSceneCenter3d();
	mip::VECTOR2 WorldToScreen(mip::VECTOR3 _pt, mip::MATRIX44* _world_mat);
	mip::VECTOR3 ScreenToWorld(mip::VECTOR2 _pt, mip::MATRIX44* _world_mat);

	void TransformRotate(mip::TRANSFORM* tr, const mip::SCAMERA* camera, const mip::VECTOR3* pPivot = NULL, mip::MATRIX44* pTransform = NULL);
	mip::MATRIX44 calcMatrixRotateFromPoint(mip::VECTOR3& _vec, mip::VECTOR3& _pt);

	void OnReCalcZero();
	void OnOpacityChange(int val);

	mip::SCAMERA* GetCameraPtr();

	mip::VECTOR3 GetPivotPoint() const;
	bool IsPivotPointFlagOn() const;

	void InitMainTransform();
	mip::MATRIX44 GetMainTransform();

	CMeshWorkManager* GetWorkManager() const;

private:
	mip::SCAMERA m_camera;

	mip::VECTOR3 m_PivotPoint;
	bool m_bPivotPoint = false;

	mip::TRANSFORM m_TransformMain;

	DataContext* m_pDataContext;

	MEVolumeView* m_pMeshViewer;
	CMeshViewBtn3DScene* m_btn3DScene;
	CMeshWorkManager* m_pWorkManager;
	CMeshManipulator* m_pMeshManipulator;
	CPlaneManiplator* m_pPlaneManipulator;
	ActionManager* m_pActionManager;
	WindowManager* m_pWinManager;
};


#define MESH_MODELVIEW_MANAGER (CMeshModelViewManager::getInstance())
#endif
