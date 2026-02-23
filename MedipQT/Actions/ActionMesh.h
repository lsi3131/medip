#pragma once

#ifndef ACTION_MESH_H
#define ACTION_MESH_H

#include "define.h"
#include <QtWidgets>
#include <Qthread>
#include "Renderer/Mesh.h"
//#include "Mip/boolean.h"
#include "UI/MaskList.h"
#include <QUndoCommand>

#include "DataContext.h"
#include "Actions/Mesh/CUndoRedo.h"
#include "Actions/Mesh/ActionCreateMesh.h"
#include "Actions/Mesh/ActionAddMeshes.h"

class MEVolumeView;

class ActionAddMesh : public QUndoCommand, CUndoRedo
{
public:
	ActionAddMesh(DataContext* pDataContext, mint32 layerUID, QString strName, mip::MeshTopology* m, bool upScale = false, QUndoCommand* parent = NULL);
	virtual ~ActionAddMesh();

	void undo() override;
	void redo() override;
	int id() const override { return m_id; }
private:
	int  m_id;
	bool 	m_first;
	int  m_Points[3];
	bool 	m_upScale;
	mint32 	m_layerUID;
	mint8 	m_UID;
	QString 	m_strName;
	mip::MeshTopology* m_mesh;
	MeshInfo m_info;

	DataContext* m_pDataContext;
};

class ActionAddMeshVisualPrint : public QUndoCommand, CUndoRedo
{
public:
	ActionAddMeshVisualPrint(DataContext* pDataContext, mint32 layerUID, QString strName, mip::MeshTopology* m, bool upScale = false, QUndoCommand* parent = NULL);
	virtual ~ActionAddMeshVisualPrint();

	void undo() override;
	void redo() override;
	int id() const override { return m_id; }
private:
	int  m_id;
	bool 	m_first;
	int  m_Points[3];
	bool 	m_upScale;
	mint32 	m_layerUID;
	mint8 	m_UID;
	QString 	m_strName;
	mip::MeshTopology* m_mesh;
	MeshInfo m_info;

	DataContext* m_pDataContext;
};

class ActionUpdateMesh : public QUndoCommand, CUndoRedo
{
public:
	ActionUpdateMesh(DataContext* pDataContext, mint8 mUID, mip::MeshTopology* m, QUndoCommand* parent = NULL);
	virtual ~ActionUpdateMesh();

	void undo() override;
	void redo() override;
	int id() const override { return m_id; }
private:
	int  m_id;
	double 	m_Points[2][3];//0 : before, 1: after
	mint8 	m_UID;
	mip::MeshTopology* m_mesh;	//only use first

	DataContext* m_pDataContext;
};

class ActionUpdateMeshVisualPrint : public QUndoCommand, CUndoRedo
{
public:
	ActionUpdateMeshVisualPrint(DataContext* pDataContext, mint8 mUID, mip::MeshTopology* m, QUndoCommand* parent = NULL);
	virtual ~ActionUpdateMeshVisualPrint();

	void undo() override;
	void redo() override;
	int id() const override { return m_id; }
private:
	int  m_id;
	mint8 	m_UID;
	mip::MeshTopology* m_mesh;	//only use first
	DataContext* m_pDataContext;
};

// 201030 허 건 대리
class ActionUndoRedoMesh : public QUndoCommand, CUndoRedo
{
public:
	ActionUndoRedoMesh(
		DataContext* pDataContext,
		MESH_WORK_MODE _work_mode = MESH_WORK_NONE,
		MESH_WORK_MODE _work_prev_mode = MESH_WORK_NONE,
		QUndoCommand* parent = NULL
	);
	virtual ~ActionUndoRedoMesh();

	void undo() override;
	void redo() override;
	int id() const override { return m_id; }
private:
	void SaveUndoFiles();
	void SaveRedoFiles();

	void LoadUndoFiles();
	void LoadRedoFiles();

	void UpdateMeshList();
	void CheckMeshEditingTab();

	bool isMeshSelected() const;

private:
	int m_id;

	MESH_WORK_MODE m_WorkMode;
	MESH_WORK_MODE m_PrevWorkMode;

	int m_SculptMode;

	bool m_b_PlaneFill;

	bool m_b_first;

	std::vector<bool> m_vt_pckID;

	muint8 m_CurrentMeshIndex;

	DataContext* m_pDataContext;
};

// 201030 허 건 대리
// Visual Print Undo Rede Action
class ActionUndoRedoMeshVisualPrint : public QUndoCommand, CUndoRedo
{
public:
	ActionUndoRedoMeshVisualPrint(DataContext* pDataContext, QUndoCommand* parent = NULL);
	virtual ~ActionUndoRedoMeshVisualPrint();

	void undo() override;
	void redo() override;
	int id() const override { return m_id; }
private:
	int  m_id;

	DataContext* m_pDataContext;
};

class ActionSplitMesh : public QUndoCommand, CUndoRedo
{
public:
	ActionSplitMesh(DataContext* pDataContext, mint8 mUID, mip::MeshTopology* m, QUndoCommand* parent = NULL);
	virtual ~ActionSplitMesh();

	void undo() override;
	void redo() override;
	int id() const override { return m_id; }
private:
	int  m_id;
	bool 	m_first;
	double 	m_Points[2][3];//0 : before, 1: after
	mint8 	m_UID;
	mip::MeshTopology* m_mesh;	//only use first
	mint8 	m_SUID; //split uid
	int  m_SPoints[3]; //split points
	mip::MeshTopology* m_Smesh;	//only use first
	MeshInfo m_info; //split info

	DataContext* m_pDataContext;
};


class ActionDelMesh : public QUndoCommand, CUndoRedo
{
public:
	ActionDelMesh(DataContext* pDataContext, muint8 mUID, mint32 lUID = -1, QUndoCommand* parent = NULL);
	virtual ~ActionDelMesh();

	void undo() override;
	void redo() override;
	int id() const override { return m_id; }
private:
	int  m_id;
	bool 	m_first;
	double 	m_Points;
	mint32 	m_layerUID;
	mint8 	m_UID;
	MeshInfo* m_info;
	MESH_WORK_MODE  m_WorkMode;

	DataContext* m_pDataContext;
};

class ActionDelMeshes : public QUndoCommand, CUndoRedo
{
public:
	ActionDelMeshes(DataContext* pDataContext, std::vector<mint8> delUIDs, QUndoCommand* parent = NULL);
	virtual ~ActionDelMeshes();

	void undo() override;
	void redo() override;
	int id() const override { return m_id; }
private:
	int  m_id;
	bool 	m_first;
	int  m_DelCnt;//del count
	int  m_BeforeCnt;
	double* m_Points;
	mint32* m_layerUID;
	mint8* m_UID;
	MeshInfo* m_info;

	MESH_WORK_MODE m_WorkMode;

	DataContext* m_pDataContext;
};

//change mesh color
class ActionColorMesh : public QUndoCommand, CUndoRedo
{
public:
	ActionColorMesh(DataContext* pDataContext, QColor col, muint8 mUID, bool _b_visual_print = false, QUndoCommand* parent = NULL);
	virtual ~ActionColorMesh();

	void undo() override;
	void redo() override;
	int id() const override { return m_id; }
private:
	int  m_id;
	COLOR 	m_color[2]; //0 : before, 1: after
	muint8 	m_UID;
	bool 	m_updateSub;
	MeshInfo* m_info;
	bool 	m_bVisualPrint;
	DataContext* m_pDataContext;
};

class ActionNameMesh : public QUndoCommand, CUndoRedo
{
public:
	ActionNameMesh(DataContext* pDataContext, QString newName, muint8 mUID, bool _b_visual_print = false, QUndoCommand* parent = NULL);
	virtual ~ActionNameMesh();

	void undo() override;
	void redo() override;
	int id() const override { return m_id; }
private:
	int  m_id;
	QString 	m_name[2]; //0 : before, 1: after
	muint8 	m_UID;
	MeshInfo* m_info;
	bool 	m_bVisualPrint;

	DataContext* m_pDataContext;
};

class ActionUIDMesh : public QUndoCommand, CUndoRedo
{
public:
	ActionUIDMesh(DataContext* pDataContext, mint8 oldUID, mint8 newUID, QUndoCommand* parent = NULL);
	virtual ~ActionUIDMesh();

	void undo() override;
	void redo() override;
	int id() const override { return m_id; }
private:
	int  m_id;
	bool 	m_first;
	int  m_start;
	int  m_end;
	muint8 	m_orgUID;
	muint8* m_UID;
	mint32* m_layerUID;
	int  m_count;
	DataContext* m_pDataContext;
};

class ActionConnectMesh : public QUndoCommand, CUndoRedo
{
public:
	ActionConnectMesh(DataContext* pDataContext, muint32 lUID, QUndoCommand* parent = NULL);
	virtual ~ActionConnectMesh();

	void undo() override;
	void redo() override;
	int id() const override { return m_id; }
private:
	int  m_id;
	muint32 	m_lUID;
	mint8 	m_mUID;

	DataContext* m_pDataContext;
};

class ActionConnectMeshes : public QUndoCommand, CUndoRedo
{
public:
	ActionConnectMeshes(DataContext* pDataContext, bool show, QUndoCommand* parent = NULL);
	ActionConnectMeshes(DataContext* pDataContext, std::vector<muint32> lUID, bool Conn, QUndoCommand* parent = NULL);
	virtual ~ActionConnectMeshes();

	void undo() override;
	void redo() override;
	int id() const override { return m_id; }
private:
	bool  m_show;
	int  	m_id;
	std::vector<mint32> m_lUID;
	std::vector<mint8>	m_mUID;

	DataContext* m_pDataContext;
};

class ActionScaleMesh : public QUndoCommand, CUndoRedo
{
public:
	ActionScaleMesh(DataContext* pDataContext, muint8 uid, float* scal, QUndoCommand* parent = NULL);
	virtual ~ActionScaleMesh();
	void undo() override;
	void redo() override;
	int id() const override { return m_id; }
private:
	int  	m_id;
	mint8  m_UID;
	float  m_scal[3];
	int  	m_count;
	qint64  m_center;//center index (m->m_verts.size() /2)
	mip::MeshTopology* m_mesh;

	DataContext* m_pDataContext;
};


class ActionTransMesh : public QUndoCommand, CUndoRedo
{
public:
	ActionTransMesh(DataContext* pDataContext, muint8 uid, mip::VECTOR3 trans, QUndoCommand* parent = NULL);
	virtual ~ActionTransMesh();
	void undo() override;
	void redo() override;
	int id() const override { return m_id; }
private:
	int  	m_id;
	mint8  m_UID;
	mip::VECTOR3 m_trans;
	int  	m_count;
	qint64  m_center;//center index (m->m_verts.size() /2)
	mip::MeshTopology* m_mesh;

	DataContext* m_pDataContext;
};

class ActionRotateMesh : public QUndoCommand, CUndoRedo
{
public:
	ActionRotateMesh(DataContext* pDataContext, muint8 uid, float* fRot, QUndoCommand* parent = NULL);
	virtual ~ActionRotateMesh();

	void undo() override;
	void redo() override;
	int id() const override { return m_id; }
private:
	int  	m_id;
	mint8  m_UID;
	mip::VECTOR3 m_rotate;
	int  	m_count;
	qint64  m_center;//center index (m->m_verts.size() /2)
	mip::MeshTopology* m_mesh;

	DataContext* m_pDataContext;
};


class ActionAlignMesh : public QUndoCommand, CUndoRedo
{
public:
	ActionAlignMesh(DataContext* pDataContext, muint8 uid, mip::MeshTopology* newMesh, QUndoCommand* parent = NULL);
	virtual ~ActionAlignMesh();
	void undo() override;
	void redo() override;
	int id() const override { return m_id; }
private:
	int  	m_id;
	mint8  m_UID;
	mip::MeshTopology* m_mesh;
	bool  m_first;
	int  	m_count;
	qint64  m_center;

	DataContext* m_pDataContext;
};

class WorkMeshAlign : public QObject, CUndoRedo
{
	Q_OBJECT

public:
	WorkMeshAlign(mip::MeshTopology* newMesh, mip::MeshTopology* oldMesh, int Stype = 0, int Dtype = 0);

private:
	void setProgressValue(int value, bool init = false);

public slots:
	void threadRun();

signals:
	void progress(int);
	void finished();

private:
	mip::MeshTopology* m_new;
	mip::MeshTopology* m_old;
	int	m_stype;
	int m_dtype;
};


class WorkMeshPlanecut : public QObject, CUndoRedo
{
	Q_OBJECT

public:
	WorkMeshPlanecut(
		DataContext* pDataContext,
		MESH_WORK_MODE _mode,
		MESH_WORK_MODE _prev_mode,
		mip::MeshTopology* _plane_mesh,
		mip::MATRIX44& _plane_mat,
		std::vector<mip::MeshTopology*>& _vt_mesh,
		std::vector<std::vector<mip::VECTOR3>>* _vt_pt_holes,
		bool _b_fill_hole,
		bool _b_remesh,
		bool _b_smooth,
		bool _b_zaxis_inverse
	);

private:
	std::vector<mip::MeshTopology*>  m_vt_mesh;
	mip::MeshTopology* m_PlaneMesh;
	mip::MATRIX44   	m_PlaneMat;
	std::vector<std::vector<mip::VECTOR3>>* m_p_vt_pt_holes;
	bool m_b_fill_hole;
	bool m_b_remesh;
	bool m_b_smooth;
	bool m_b_InverseZAxis;

	MESH_WORK_MODE  	m_mode;
	MESH_WORK_MODE  	m_prev_mode;

	DataContext* m_pDataContext;

public slots:
	void threadRun();

signals:
	void progress(int);
	void finished();
	void sig_buildRenderBufferTopology(mip::MeshTopology* _p_mesh);
	void sig_renderLater();
};


class WorkMeshPolycut : public QObject, CUndoRedo
{
	Q_OBJECT

public:
	WorkMeshPolycut(
		DataContext* pDataContext,
		MESH_WORK_MODE _mode,
		MESH_WORK_MODE _prev_mode,
		std::vector<mip::VECTOR2>& _vt_polygons
	);

	~WorkMeshPolycut() {}

private:
	MESH_WORK_MODE  	m_mode;
	MESH_WORK_MODE  	m_prev_mode;
	std::vector<mip::VECTOR2> m_vt_polygons;
	mip::MeshTopology* m_p_mesh;

	DataContext* m_pDataContext;

public slots:
	void threadRun();

signals:
	void progress(int);
	void finished();
	void sig_buildRenderBufferTopology(mip::MeshTopology* _p_mesh);
	void sig_renderLater();
};

class WorkMeshHoleDetect : public QObject, CUndoRedo
{
	Q_OBJECT

public:
	WorkMeshHoleDetect(
		mip::MeshTopology* _p_mesh,
		std::vector<std::vector<mip::VECTOR3>>& _vt_boundary_pts,
		std::vector<std::pair<std::vector<int>, bool>>& _vt_boundary_pts_idx
	);
	~WorkMeshHoleDetect() {}

private:
	mip::MeshTopology* m_p_mesh;
	std::vector<std::vector<mip::VECTOR3>>* m_vt_boundary_pts;
	std::vector<std::pair<std::vector<int>, bool>>* m_vt_boundary_pts_idx;

public slots:
	void threadRun();

signals:
	void progress(int);
	void finished();
	void sig_buildRenderBufferTopology(mip::MeshTopology* _p_mesh);
	void sig_renderLater();
};

class WorkMeshFillSeltedHole : public QObject, CUndoRedo
{
	Q_OBJECT

public:
	WorkMeshFillSeltedHole(
		DataContext* pDataContext,
		mip::MeshTopology* _p_mesh,
		int _hole_idx,
		bool _b_remesh,
		bool _b_smooth,
		std::vector<std::vector<mip::VECTOR3>>& _vt_boundary_pts,
		std::vector<std::pair<std::vector<int>, bool>>& _vt_boundary_pts_idx,
		bool _b_FillHoleAll = false
	);
	~WorkMeshFillSeltedHole() {}

private:
	void selectedHoleFill();
	void HoleFillAll();

	mip::MeshTopology* m_p_mesh;
	int  m_hole_idx;
	bool m_b_remesh;
	bool m_b_smooth;

	bool m_b_FillHoleAll;

	std::vector<std::vector<mip::VECTOR3>>* m_vt_boundary_pts;
	std::vector<std::pair<std::vector<int>, bool>>* m_vt_boundary_pts_idx;

	DataContext* m_pDataContext;

public slots:
	void threadRun();

signals:
	void progress(int);
	void finished();
	void sig_buildRenderBufferTopology(mip::MeshTopology* _p_mesh);
	void sig_renderLater();
};

class WorkMeshBoolean : public QObject, CUndoRedo
{
	Q_OBJECT

public:
	WorkMeshBoolean(DataContext* pDataContext, MESH_WORK_MODE type, mip::MeshTopology* res, muint8 fUID, muint8 sUID);
	~WorkMeshBoolean() {}

	static void updateProgress(float, void*);

private:
	muint8 fUID; //first mesh uid
	muint8 sUID; //second mesh uid
	mip::MeshTopology* _mesh;//result mesh
	 //	mip::MeshTopology *_meshD; //second m2
	MESH_WORK_MODE type;
	static void* _dt;

private:
	void setProgressValue(int value, bool init = false);

	DataContext* m_pDataContext;

public slots:
	void threadRun();

signals:
	void progress(int);
	void sig_updateUI();
	void finished();
};

class WorkMeshRemesh : public QObject, CUndoRedo
{
	Q_OBJECT

public:
	WorkMeshRemesh(MESH_REMESH_TYPE type, float offset, DataContext* pDataContext, MEVolumeView* pViewer);

	static void updateProgress(float, void*);

	//20201207_byPHS
	void setEdgeLength(double value) { _edgeLength = value; };
	void setReducePreserveBound(bool bChecked) { _ReducePBChecked = bChecked; };
	void setReduceMethod(int method) { _ReduceMethod = method; };
	void setSmoothMethod(int method) { _SmoothMethod = method; };
	void setOffset(float value) { _offset = value; };

	// 200917 허건대리
	QString m_FileName;

	float m_ProgressInterval = 0;
	float m_ProgressTotal = 0;
	float m_ProgressValue = 0;
	int 	m_countMesh = 0;
	int 	m_pckMesh = 0;

	// 허 건 과장
	// 211018
	struct  SMeshParamsRemesh
	{
		SMeshParamsRemesh()
		{
			b_select_mode = false;
			b_sculpt_mode = false;

			method_sd = 0;

			b_inverse_if = false;
			n_remain_if = 1;
			n_trinagles_if = 1;
		}

		// Mode 
		bool b_select_mode;
		bool b_sculpt_mode;

		// Remesh
		double edge_len_remesh;

		// SubDivision
		int method_sd; // 0:mid-point, 1: L3-loop

		// Island - Filter
		bool b_inverse_if;
		int n_remain_if;
		int n_trinagles_if;
	};

	SMeshParamsRemesh m_Params;

private:
	MESH_REMESH_TYPE _type;
	mip::MeshTopology* _mesh2;
	float 	_offset;
	int  _SmoothMethod;
	int  _ReduceMethod;
	bool 	_ReducePBChecked;
	double 	_edgeLength;

	bool 	_bAuto;

	DataContext* m_pDataContext;
	MEVolumeView* m_pViewer;

private:
	void setProgressValue(int value, bool init = false);

	MESH_WORK_MODE getMeshWorkMode();

public slots:
	void threadRun();

signals:
	void progress(int);
	void finished();

	void sig_buildRenderBufferTopology(mip::MeshTopology* _p_mesh);
	void sig_renderLater();

	// 200917 허건대리
	void sig_updateProgress(int, QString);
	// 20201012 byPHS
	void showQMessageDlg(int, QString);
};

// 210530 허 건 과장
// Duplicate Mesh
class WorkMeshDuplicate : public QObject, CUndoRedo
{
	Q_OBJECT

public:
	WorkMeshDuplicate(DataContext* pDataContext, QList<muint32>& _list);
	~WorkMeshDuplicate();

public slots:
	void threadRun();

signals:
	void sig_progress(int, QString);
	void finished();

	void sig_updateUI();
	void sig_buildRenderBufferTopology(mip::MeshTopology* _p_mesh);
	void sig_renderLater();

private:
	void single_process();
	void parallel_process();

	void updateResult();

	QList<muint32>   m_MeshList;

	std::vector<mint32>  	m_vt_layer_uid;
	std::vector<mip::MeshTopology*> m_vt_p_topology;
	std::vector<QString>  m_vt_file_name;

	DataContext* m_pDataContext;
};

// 210530 허 건 과장
// Duplicate Mesh
class WorkMeshAttach : public QObject, CUndoRedo
{
	Q_OBJECT

public:
	WorkMeshAttach(QList<muint32>& _list, DataContext* pDataContext);

public slots:
	void threadRun();

signals:
	void sig_progress(int, QString);
	void finished();

	void sig_buildRenderBufferTopology(mip::MeshTopology* _p_mesh);
	void sig_renderLater();

private:
	QList<muint32>   m_MeshList;
	DataContext* m_pDataContext;
};


// 210823 허 건 과장
// Mesh To Mask
class WorkMeshToMask : public QObject, CUndoRedo
{
	Q_OBJECT

public:
	WorkMeshToMask(QList<muint32>& _list, DataContext* pDataContext);
	~WorkMeshToMask();

public slots:
	void threadRun();

signals:
	void sig_progress(int, QString);
	void finished();

	void sig_applyMaskToUI(int);

private:
	void MeshToMask();

	QList<muint32>   m_MeshList;

	DataContext* m_pDataContext;
};


/*
@작성자 박헌수 대리 2021_11_24
*/
class WorkMeshStamp3D : public QObject, CUndoRedo
{
	Q_OBJECT

public:
	WorkMeshStamp3D(
		DataContext* pDataContext,
		mip::MeshTopology* pTargetMesh,
		mip::MeshTopology* pFontMesh,
		int  pickFaceIdx,
		float fOffset,	//
		bool bIntaglio	//음각
	);
	~WorkMeshStamp3D();

	static void updateProgress(float, void*);
private:
	mip::MeshTopology* targetMesh;
	mip::MeshTopology* fontMesh;
	int faceIdx;	//
	float offset;	//
	bool bIntaglio;	//음각	

	DataContext* m_pDataContext;

public slots:
	void setProgressValue(int value, bool init = false);
	void threadRun();

signals:
	void progress(int);
	void finished();
	void sig_buildRenderBufferTopology(mip::MeshTopology* _p_mesh);
	void sig_renderLater();
	void sig_updateUI();
};
#endif