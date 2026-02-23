/*
@company		메디컬아이피
@autor			허 건 과장
@date			2021-02-05
@brief			CManipulator 헤더파일
*/

#pragma once

#ifndef CMANIPULATOR_H
#define CMANIPULATOR_H

#include "mipEngine\Camera.h"
#include "DataContext.h"

#define INTERVAL_SCALE		0.25f	//0.025f
#define MIN_RATIO_SCALE		0.01f
#define NUMBER_OF_ROWS		41  //21
#define NUMBER_OF_COLS		41  //21
#define LEN_PLANE_LINE		6.f
#define DEFAULT_ZOOM_FACTOR 54.f

enum MANIPULATOR_DIR_TYPE
{
	TRANS_X_AXIS,
	TRANS_Y_AXIS,
	TRANS_Z_AXIS,
	TRANS_XY_AXIS,
	TRANS_YZ_AXIS,
	TRANS_XZ_AXIS,
	ROTATE_X_AXIS,
	ROTATE_Y_AXIS,
	ROTATE_Z_AXIS,
	SCALE_X_AXIS,
	SCALE_Y_AXIS,
	SCALE_Z_AXIS,
	SCALE_ALL_AXIS,
	INVERSE_Z_AXIS,
	NONE_AXIS,
};

#define NUM_AXIS_MANIPULATOR	14
#define NUM_Plane_SNAPPING		3
#define NUM_OFFSET_POINT		3
#define INTERVAL_TRANSLATE		0.25f	//2.5mm

class CMeshModelViewManager;
class CMeshDlgManager;
class DataContext;

namespace mip
{
	class Renderer;
	class MeshTopology;
	class OcNode;
	class TRANSFORM;
	class MATRIX44;
	class SCAMERA;
}

class	CManipulator
{
public:
	CManipulator(mip::Renderer* pRenderer);
	~CManipulator();

public:
	virtual void render(
		DataContext* pDataContext,
		mip::Renderer* _p_renderer,
		mip::MATRIX44& _mat_view_world,
		mip::MATRIX44& _mat_offset,
		mip::SCAMERA* _p_camera);

	virtual void DrawSnapping(QPainter* _p_paint);

	virtual void Reset();

public:
	void Init();
	void Init(
		CMeshModelViewManager* pModelViewManager,
		CMeshDlgManager* pMeshDlgManager
	);
	void Process(DataContext* pDataContext, mip::MeshTopology* _p_mesh, QPoint _prev_mouse_pt, QPoint _cur_mouse_pt);

	void ReadyProcess(
		QPoint& _prev_mouse_pt,
		QPoint& _cur_mouse_pt,
		mip::MATRIX44& _mat_view_world,
		mip::MATRIX44& _mat_offset,
		mip::SCAMERA* _p_camera
	);
	bool FinishProcess(mip::MeshTopology* _p_mesh);

	void OkProcess(mip::MeshTopology* _p_mesh);
	void CancelProcess(mip::MeshTopology* _p_mesh);

	void HomePosition();

	bool CheckClickedArrow(DataContext* pDataContext, QPoint _pt, mip::SCAMERA* _p_camera);

	void UpdatePivotPoint(mip::QUATERNION& _q, mip::VECTOR3& _trans);
	void UpdatePivotPoint(mip::MeshTopology* _p_mesh, mip::MATRIX44& _matrix);

	bool isCheckedArrow();
	bool isInverseZAxis();
	void setInverseZAxis();

	void UpdatePrevTransform(mip::MeshTopology* _p_mesh);
	void UpdateInitTransform(mip::MeshTopology* _p_mesh);

	void setTransformMat(mip::TRANSFORM& _trans);
	mip::TRANSFORM getTransformMat();

	mip::MATRIX44 getTransformMatAxis(int _idx);
	void setPrevTransformMat(mip::TRANSFORM& _trans);

	mip::TRANSFORM getPrevTransformMat();
	mip::MATRIX44 getPrevTransformMatAxis(int _idx);

	void UpdateTransformAxis(int _idx_axis, mip::MATRIX44& _mat);
	void AddTransformAxis(mip::MATRIX44& _mat);

	mip::MATRIX44 getRotateTempMat();
	mip::VECTOR3 getTranslateTempVal();
	mip::VECTOR3 getRotateTempAngle();
	mip::VECTOR3 getCenter();
	mip::VECTOR3 getScaleVec();
	mip::VECTOR3 getScaleTranslation();

	void setAxisDirection(MANIPULATOR_DIR_TYPE _axis);
	MANIPULATOR_DIR_TYPE getAxisDirection();

	void enableUpdateCtrl(bool _enable);

	void processTranslate(mip::MeshTopology* _p_mesh, mip::VECTOR3& _trans);
	void processRotate(
		mip::MeshTopology* _p_mesh,
		mip::MATRIX44& _rot,
		mip::VECTOR3& _angle,
		mip::VECTOR3& _center,
		mip::RAY& _dir_vec,
		mip::RAY& _cross_dir_vec,
		mip::MeshTopology* _p_pck_mesh
	);

	void processScale(
		mip::MeshTopology* _p_mesh,
		mip::VECTOR3& _scale_vec,
		mip::VECTOR3& _t
	);

	void UpdatePosition(
		mip::MeshTopology* _p_mesh,
		bool _b_init = true
	);

	void SaveUndoRedo(mip::MeshTopology* _p_mesh);

public:
	mip::RAY m_axis_dir;
	mip::RAY m_axis_cross_dir;

protected:
	void processTranslate(mip::MeshTopology* _p_mesh);
	void processRotate(DataContext* pDataContext, mip::MeshTopology* _p_mesh);
	void processScale(mip::MeshTopology* _p_mesh);

	void updateRotateInfo(mip::VECTOR3& _angle, mip::MATRIX44& _mat);
	void updateTranslateInfo(mip::VECTOR3& _trans);

	void UpdateOctreeBB(mip::MeshTopology* _p_mesh, mip::OcNode* _node);

	mip::MATRIX44 RotateFromPoint(mip::MeshTopology* _mesh, mip::VECTOR3& _vec, mip::VECTOR3& _pt, bool _b_tverts = false);
	void RotateFromPoint(std::vector<mip::VECTOR3>& _vt_points, mip::TRANSFORM& _tr);
	void RotateFromPoint(std::vector<mip::VECTOR3>& _vt_points, mip::VECTOR3& _vec, mip::VECTOR3& _pt);
	void RotateFromPoint(mip::VECTOR3& _vt_point, mip::VECTOR3& _vec, mip::VECTOR3& _pt);
	mip::MATRIX44 calcMatrixRotateFromPoint(mip::VECTOR3& _vec, mip::VECTOR3& _pt, mip::VECTOR3 _translate = mip::VECTOR3());
	void Translate(mip::MeshTopology* _mesh, mip::VECTOR3& _vec, bool _b_tverts = false);
	void Translate(std::vector<mip::VECTOR3>& _vt_points, mip::VECTOR3& _vec);

	void TransformRotate(
		mip::MeshTopology* _mesh,
		mip::VECTOR3& _center,
		mip::SCAMERA* _p_camera,
		mip::MATRIX44* trasnform = NULL
	);

	void DrawCircle();
	void DrawArrow(DataContext* pDataContext, mip::Renderer* _p_renderer);
	void DrawSphere();
	void DrawPlane();
	void DrawSnappingInternal(QPainter* _p_paint);

	void CreateArrow(mip::MeshTopology* _mesh, MANIPULATOR_DIR_TYPE _axis);
	void CreateRotateArrow(mip::MeshTopology* _mesh);
	void CreateCommonArrow(mip::MeshTopology* _mesh);
	void CreateInverseZArrow(mip::MeshTopology* _mesh);
	void CreateQuadArrow(mip::MeshTopology* _mesh);
	void CreateTriArrow(mip::MeshTopology* _mesh);
	void CreateCube(mip::MeshTopology* _mesh);
	void CreateSphere(mip::MeshTopology* _mesh, mip::VECTOR4 _color);
	void CreatePlane(mip::MeshTopology* _mesh, MANIPULATOR_DIR_TYPE _type, float _length_line);
	void CreateRotateSnappingPoint(std::vector<mip::VECTOR3>& _vt_points);
	void showAxisArrow(MANIPULATOR_DIR_TYPE _type, bool _b_show);

	void MoveCoordinate(mip::VECTOR3& _trans);
	void MovePlane(mip::VECTOR3& _trans);
	void RotateCoordinate(
		DataContext* pDataContext,
		mip::MeshTopology* _p_mesh,
		mip::VECTOR3& _vec_angle,
		mip::VECTOR3* _center = nullptr
	);

	void RotateCoordinateAxis(
		mip::MATRIX44& _mat
	);

	void RotateMesh(mip::MeshTopology* _p_mesh, mip::VECTOR3& _vec_angle, mip::VECTOR3& _center);

	void calcOffsetPoint(std::vector<mip::VECTOR3>& _vt_pt, mip::VECTOR3& _center, int _n_row, int _n_cols, float _offset);
	void checkCloseOffset(int& _dx, int& _dy, mip::QUATERNION& _q);

	void convertNewMousePt(
		QPoint& _prev_mouse_pt,
		QPoint& _cur_mouse_pt,
		mip::VECTOR3& _new_prev_mouse_pt,
		mip::VECTOR3& _new_cur_mouse_pt,
		mip::VECTOR3& _direction = mip::VECTOR3()
	);

	float calcAngleFromMousePt();
	float calcAngleOffsetPointAxisTrans();
	float calcAngle(mip::VECTOR2& _pt1, mip::VECTOR2& _pt2, mip::VECTOR2& _pt3);
	float calculateCCW(mip::VECTOR2& _pt1, mip::VECTOR2& _pt2, mip::VECTOR2& _pt3);
	void  checkAngleLimit(mip::VECTOR3& _angle);

	void calcAxisDir(mip::RAY& _axis_dir, mip::RAY& _axis_cross_dir);
	mip::VECTOR3 calcIntersectPoint(mip::RAY _ray1, mip::RAY _ray2);

	void initParams();
	void initSnapping();
	void initSphere();
	void initPlane();
	void initCtrl();

	void updateCtrl();

	void resetPrevTransform();
	void UpdatePrevTransform(mip::MATRIX44& _matrix);

protected:
	mip::Renderer* m_pRenderer;
	mip::MeshTopology* m_AxisMesh[NUM_AXIS_MANIPULATOR];
	bool						m_bShowFlag[NUM_AXIS_MANIPULATOR];
	bool						m_bUseFlag[NUM_AXIS_MANIPULATOR];

	mip::MeshTopology* m_SphereCube;
	mip::MeshTopology* m_Sphere;
	mip::MeshTopology* m_SnappingPlane[NUM_Plane_SNAPPING];
	mip::MeshTopology* m_SnappingRotate;

	MANIPULATOR_DIR_TYPE		m_Axis_Seleted;
	MANIPULATOR_DIR_TYPE		m_Axis_Prev;
	bool						m_bBack_Selected;

	mip::TRANSFORM				m_InitTransform;
	mip::TRANSFORM				m_InitAxisTransform[NUM_AXIS_MANIPULATOR];
	mip::TRANSFORM				m_PrevTransform;
	mip::TRANSFORM				m_PrevAxisTransform[NUM_AXIS_MANIPULATOR];
	mip::TRANSFORM				m_Transform;
	mip::TRANSFORM				m_TransformTemp;

	mip::AABB					m_PrevBB;
	mip::AABB					m_InitBB;

	mip::VECTOR3				m_Trans;
	mip::VECTOR3				m_TransAccumulate;

	mip::VECTOR3				m_Angle;
	mip::VECTOR3				m_Center;
	mip::VECTOR3				m_RotAngleAccumulate;
	mip::VECTOR3				m_RotAngleAccumulateTemp;

	mip::MATRIX44				m_RotMatTemp;

	mip::VECTOR3				m_ScaleVec;
	mip::VECTOR3				m_ScaleTranslate;

	bool						m_bFinished;
	bool						m_bUpdatePivot;
	bool						m_bInverseZAxis;

	int							m_nCount;

	bool						m_bSnapping;
	std::vector<mip::VECTOR3>	m_vt_RotateSnappingPoint;
	std::vector<mip::VECTOR3>	m_vt_OffsetPoint;
	mip::VECTOR3				m_centerSnapping;
	mip::VECTOR3				m_MoveSnapInterval;

	QPoint						m_InitMousePt;
	QPoint						m_CurrentMousePt;
	QPoint						m_PrevMousePt;

	QLabel* m_Label;
	QLabel* m_Label1;
	QLabel* m_Label2;

	bool						m_bUpdateCtrl;

	mip::MATRIX44				m_mat_view_world;
	mip::MATRIX44				m_mat_offset;

	mip::SCAMERA* m_p_camera;

	CMeshModelViewManager* m_pModelViewManager;
	CMeshDlgManager* m_pMeshDlgManager;
};

#include "CPlaneManiplator.h"
#include "CMeshManipulator.h"

#endif