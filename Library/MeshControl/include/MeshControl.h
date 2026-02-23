#pragma once

#include <vector>

#include "math/math.h"
#include "Renderer\MeshTopology.h"

#define DLL_DEFINE __declspec(dllexport)

typedef void(*meshLibprogUpdatefunc)(float value, void * data);

class GMesh;

namespace mip
{
	class VECTOR3;

	namespace mesh_control
	{
		// -----------------------------------------------------------
		// Data
		// -----------------------------------------------------------

		struct Brush_Data
		{
			std::vector<std::vector<muint32>>			_2ArrforDisplayVerts;		// 중복안된 Display Vert에서 중복된 Display Vert(m_verts)를 가르키는 2중 배열
			std::vector<std::vector<muint32>>			_2ArrforDisplayNormals;	// 중복된 Display Vert idx(m_verts)에서 해당 인덱스의 정점이 속한 삼각형의 TTris가 가지고 있는 vi idx를 가리키는 2중 배열
			std::vector<mint32>							_arrTVertToDisVert;		// Topology Vert idx(m_tverts)에서 중복안된 Display Vert idx를 가리키는 배열
			std::vector<mint32>							_arrDisVertToTVert;		// 중복안된 Display Vert idx에서 Topology Vert idx(m_tverts)를 가리키는 배열	
		};

		struct IndexStruct
		{
			//new
			std::vector<muint32> Tris;
			std::vector<muint32> Verts;
			std::vector<muint32> Heis;

			//org or del
			std::vector<muint32> org_Tris;
			std::vector<muint32> org_Verts;
			std::vector<muint32> org_Heis;
		};

		// -----------------------------------------------------------
		// Main Fuction
		// -----------------------------------------------------------

		//--------------
		// remesh.h
		//--------------

		// factor = check factor > 0.1f
		int DLL_DEFINE smooth(std::vector<VECTOR3> & _verts, std::vector<muint32> & _tris, std::vector<VECTOR3> & _normals, mip::MeshTopology * in_mesh, 
			float factor, meshLibprogUpdatefunc update = nullptr, void * data = nullptr);

		// levelCount : 1 ~ 4
		//int DLL_DEFINE subdivideFace(std::vector<VECTOR3> & _verts, std::vector<muint32> & _tris, std::vector<VECTOR3> & _normals, int levelCount,
		//	meshLibprogUpdatefunc update = nullptr, void * data = nullptr);

		// reduction : 1 ~ 100
		int DLL_DEFINE collapseFace(std::vector<VECTOR3> & _verts, std::vector<muint32> & _tris, std::vector<VECTOR3> & _normals, int reduction,
			meshLibprogUpdatefunc update = nullptr, void * data = nullptr);

		// offset : -1.0 ~ 1.0
		// resolution : 0 (auto) ~ 512
		int DLL_DEFINE solid(mip::MeshTopology * in_mesh, meshLibprogUpdatefunc update = nullptr, void * data = nullptr, int _start_val = 0, int _finisn_val = 100 ,float scale = 100.f);

		// nffset : -1.0 ~ -0.01
		// resolution : 0 (auto) ~ 512
		//int DLL_DEFINE hollow(std::vector<VECTOR3> & _verts, std::vector<muint32> & _tris, std::vector<VECTOR3> & _normals, float noffset = 0.0f, int resolution = 0,
		//	meshLibprogUpdatefunc update = nullptr, void * data = nullptr);

		int DLL_DEFINE mergePoint(std::vector<VECTOR3> & _verts, std::vector<muint32> & _tris, std::vector<VECTOR3> & _normals);

		int DLL_DEFINE collapseTopology( mip::MeshTopology* _pMeshTopology, int nReduceOption = 0, float optionVal = 0.0f, bool bPreserveBound = true, meshLibprogUpdatefunc update = nullptr, void * data = nullptr);

		int DLL_DEFINE hollowTopology(mip::MeshTopology * in_mesh, mip::MeshTopology * out_mesh, float offset = 0.0f, float edgeLength = 1.0f, meshLibprogUpdatefunc update = nullptr, void * data = nullptr);

		int DLL_DEFINE smoothTopology(mip::MeshTopology * in_mesh, int method = 0, float factor = 0.1f, meshLibprogUpdatefunc update = nullptr, void * data = nullptr);

		int DLL_DEFINE SeparateShells(mip::MeshTopology * _pMT, std::vector<std::vector<muint32>>& out_triIdxList, meshLibprogUpdatefunc update = nullptr, void * data = nullptr, int* progressFinishVal = nullptr);

		int DLL_DEFINE IslandFilter(
										mip::MeshTopology * _p_mesh, 
										int _n_remain_parts,
										bool _b_inverse, 
										int _n_tris_removal, 
										meshLibprogUpdatefunc update = nullptr, 
										void * data = nullptr
									);

		int DLL_DEFINE UniformResampling(mip::MeshTopology * in_mesh, float _offset = FLT_MIN, meshLibprogUpdatefunc update = nullptr, void * data = nullptr, int _start_val = 0, int _finisn_val = 100);

		// 210104 허 건 대리
		int DLL_DEFINE SubDivision(
									mip::MeshTopology * in_mesh, 
									int _method = 0,  // 0:mid-point, 1:L3-loop
									int _iteration = 3, 
									meshLibprogUpdatefunc update = nullptr, 
									void * data = nullptr, 
									int _start_val = 0, 
									int _finisn_val = 100
									);

		// 201112 허 건 대리
		//--------------
		// Generative design
		//--------------		
		int DLL_DEFINE generativeDesign(mip::MeshTopology * in_mesh, mip::MeshTopology * out_mesh, float _offset = FLT_MIN, meshLibprogUpdatefunc update = nullptr, void * data = nullptr, int _start_val = 0, int _finisn_val = 100);

		//--------------
		// boolean.h
		//--------------		
		// 210510 허 건 과장
		int DLL_DEFINE boolean(
								mip::MeshTopology* _p_input1, 
								mip::MeshTopology* _p_input2,
								mip::MeshTopology* _p_output,
								muint32 type, 
								mip::Renderer* _p_renderer,
								meshLibprogUpdatefunc update = NULL, 
								void * data = NULL);

		int DLL_DEFINE createCube(const VECTOR3 & size, std::vector<VECTOR3> & _verts, std::vector<muint32> & _tris, std::vector<VECTOR3> & _normals);

		// resolution > 5
		int DLL_DEFINE createCylinder(const VECTOR3 & size, int resolution, std::vector<VECTOR3> & _verts, std::vector<muint32> & _tris, std::vector<VECTOR3> & _normals);

		// resolution > 5
		int  DLL_DEFINE createSphere(const VECTOR3 & size, int theta_resolution, int phi_resolution, std::vector<VECTOR3> & _verts, std::vector<muint32> & _tris, std::vector<VECTOR3> & _normals);


		//--------------
		// cut.h
		//--------------	

		//tranform
		int DLL_DEFINE transformMesh(const MATRIX44 & mat, std::vector<VECTOR3> & _verts);
		int DLL_DEFINE translateMesh(const VECTOR3 & pos, std::vector<VECTOR3> & _verts);
		int DLL_DEFINE replaceCentor(std::vector<VECTOR3> & _verts);

		int DLL_DEFINE getMinMax(const std::vector<VECTOR3> & verts, VECTOR3 & _min, VECTOR3 & _max);

		int DLL_DEFINE pickMesh(const VECTOR2 & screen, const VECTOR2 & screen_size, MATRIX44 & mat, const std::vector<VECTOR3> & verts, const std::vector<muint32> & tris, float maxdistance, VECTOR3 & _result);

		int DLL_DEFINE pickMesh(const VECTOR3 & org, const VECTOR3 & dir, const std::vector<VECTOR3> & verts, const std::vector<muint32> & tris, float maxdistance, VECTOR3 & _result);

		int DLL_DEFINE distanceMeshL(const VECTOR2 & screen, const VECTOR2 & screen_size, const MATRIX44 & wvp, const VECTOR3 & local_p0, const VECTOR3 & local_p1, const std::vector<VECTOR3> & verts, const std::vector<muint32> & tris, float * _length, std::vector<VECTOR3> * _lineList = NULL);
		int DLL_DEFINE distanceMeshL(const VECTOR3 & local_p0, const VECTOR3 & local_p1, const VECTOR3 & local_p2, const std::vector<VECTOR3> & verts, const std::vector<muint32> & tris, float * _length, std::vector<VECTOR3> * _lineList = NULL);
		
		//Plane Cut
		//int DLL_DEFINE splitPlaneMeshL(const VECTOR2 & screen_pos1, const VECTOR2 & screen_pos2, const VECTOR2 & screen_size, MATRIX44 & mat_wvp, bool hole_close, bool point_merge,
		//	std::vector<VECTOR3> & _verts, std::vector<muint32> & _tris, std::vector<VECTOR3> & _normals,
		//	std::vector<VECTOR3> * _verts2 = NULL, std::vector<muint32> * _tris2 = NULL, std::vector<VECTOR3> * _normals2 = NULL,
		//	meshLibprogUpdatefunc update = NULL, void * data = NULL);
		//int DLL_DEFINE splitPlaneMeshL(const VECTOR3 & local_p0, const VECTOR3 & local_p1, const VECTOR3 & local_p2, bool hole_close, bool point_merge,
		//	std::vector<VECTOR3> & _verts, std::vector<muint32> & _tris, std::vector<VECTOR3> & _normals,
		//	std::vector<VECTOR3> * _verts2 = NULL, std::vector<muint32> * _tris2 = NULL, std::vector<VECTOR3> * _normals2 = NULL,
		//	meshLibprogUpdatefunc update = NULL, void * data = NULL);
		//int DLL_DEFINE splitMeshFromPlaneListL(const std::vector<VECTOR2> & screen_lines, const VECTOR2 & screen_size, const MATRIX44 & wvp,
		//	std::vector<VECTOR3> & verts, std::vector<muint32> & tris, std::vector<VECTOR3> & normals,
		//	bool hole_close, bool merge_point = false,
		//	meshLibprogUpdatefunc update = NULL, void * data = NULL);

		//int DLL_DEFINE splitMeshFromPlaneListInnerL(const std::vector<VECTOR2> & screen_lines, const VECTOR2 & screen_size, const MATRIX44 & wvp,
		//	std::vector<VECTOR3> & verts, std::vector<muint32> & tris, std::vector<VECTOR3> & normals,
		//	bool merge_point = false, bool view_depend = true,
		//	std::vector<VECTOR3> * _verts2 = NULL, std::vector<muint32> * _tris2 = NULL, std::vector<VECTOR3> * _normals2 = NULL,
		//	meshLibprogUpdatefunc update = NULL, void * data = NULL);

		// Mesh Cut : Geon 200619
		int DLL_DEFINE MeshPolyCutOut(const std::vector<mip::VECTOR2>& _vt_polygons, mip::MATRIX44 _world_mat, mip::MATRIX44 _view_mat, mip::MATRIX44 _proj_mat, mip::VECTOR2 _sz_screen, mip::MeshTopology* _p_topology);
		int DLL_DEFINE MeshPolyCutInner(const std::vector<mip::VECTOR2>& _vt_polygons, mip::MATRIX44 _world_mat, mip::MATRIX44 _view_mat, mip::MATRIX44 _proj_mat, mip::VECTOR2 _sz_screen, mip::MeshTopology* _p_topology);

		std::vector<std::vector<mip::VECTOR3>> DLL_DEFINE MeshPlaneCut(
																			//const std::vector<mip::VECTOR2>& _vt_polygons,
																			mip::PLANE* _p_plane,
																			//mip::MATRIX44 _world_mat,
																			//mip::MATRIX44 _view_mat,
																			//mip::MATRIX44 _proj_mat,
																			//mip::VECTOR2 _sz_screen,
																			mip::MeshTopology* _p_topology,
																			bool _b_fill_hole = false,
																			bool _b_remesh = false,
																			bool _b_smooth = false
		);

		int DLL_DEFINE MeshFreePolyCut(const std::vector<mip::VECTOR2>& _vt_polygons, mip::MATRIX44 _world_mat, mip::MATRIX44 _view_mat, mip::MATRIX44 _proj_mat, mip::VECTOR2 _sz_screen, mip::MeshTopology* _p_topology);
		int DLL_DEFINE MeshCutCancel(mip::MeshTopology* _p_topology);
		int DLL_DEFINE MeshCutOK(mip::MeshTopology* _p_topology);

		// Find Boundary : Geon 200909
		int DLL_DEFINE MeshFindboundary(
											mip::MeshTopology* _p_topology, 
											std::vector<std::vector<mip::VECTOR3>> & _vt_points, 
											std::vector<std::pair<std::vector<int>, bool>> & _vt_idx
										);

		int DLL_DEFINE FillHoleSelected(
											mip::MeshTopology* _p_topology,
											int _hole_idx,
											bool _b_refine  = true,
											bool _b_fairing = true
										);

		// 201222 허 건 대리
		int DLL_DEFINE FillHoleAll(mip::MeshTopology * in_mesh, meshLibprogUpdatefunc update = nullptr, void * data = nullptr, int _start_val = 0, int _finisn_val = 100);


		// Mesh Remesh : Geon 200814
		int DLL_DEFINE MeshAutoRemesh(mip::MeshTopology* _p_topology, float _edge_length = FLT_MIN, meshLibprogUpdatefunc update = nullptr, void * data = nullptr, float _scale = 100.f);

		// -----------------------------------------------------------
		// Stamp
		// -----------------------------------------------------------
		bool DLL_DEFINE Stamp3D(mip::MeshTopology * pTarget, mip::MeshTopology * pFontMT, const int pickFaceId, const float fOffset, std::vector<mip::VECTOR3> & TEST_list);

		bool DLL_DEFINE MoveFontPolygon(mip::MeshTopology * pTarget, mip::MeshTopology * pFontMT, const std::vector<mip::TVert> & orgFontTverts,
			const mip::VECTOR3 pickPoint, const int pickFaceId, const float fOffset, std::vector<mip::VECTOR3> & TEST_list, mip::QUATERNION quater = mip::QUATERNION(0.0f, 0.0f, 0.0f, 0.0f));

		// -----------------------------------------------------------
		// select
		// -----------------------------------------------------------
		int DLL_DEFINE	select_smooth(mip::MeshTopology * _pMT, int flag, int method, float factor, int scale, meshLibprogUpdatefunc update = nullptr, void * data = nullptr);

		int DLL_DEFINE	select_reduce(mip::MeshTopology * _pMT, int flag = SELECTED, int option = 0, float opValue = 50.0f, bool bPreserveBound = true, meshLibprogUpdatefunc update = nullptr, void * data = nullptr);

		// 허건과장 211028
		int DLL_DEFINE	select_remesh(
										mip::MeshTopology * _p_mesh,
										mip::MeshTopology * _p_partial,
										float _edge_length = FLT_MIN, 
										meshLibprogUpdatefunc update = nullptr, 
										void * data = nullptr,
										float _scale = 100.f
									);

		// -----------------------------------------------------------
		// brush
		// -----------------------------------------------------------

		// factor = check factor > 0.1f
		int DLL_DEFINE brush_selection(mip::MeshTopology * _pMT, mip::VECTOR3 pick, int Method = 0, int selMethod = 0, int pickVi = -1, float radi = 1.0f, int flag = 0);

		// factor = check factor > 0.1f
		int DLL_DEFINE brush_smooth(mip::MeshTopology * _pMT, std::vector<std::vector<muint32>>& _2ArrforDisplayVerts, std::vector<muint32> & _vt_idx,
			int method, int selMethod, mip::VECTOR3 pick, int pickVi = -1, float factor = 0.75, float radi = 1.00f
		);

		int DLL_DEFINE brush_Inflate(mip::MeshTopology * _pMT, std::vector<std::vector<muint32>>& _2ArrforDisplayVerts, mip::mesh_control::IndexStruct & out_IndexList, std::vector<muint32> & _vt_idx,
			mip::VECTOR3 pick, int pickVi = -1, float factor = 0.75, float radi = 1.00f, int selMethod = 0, bool bDeflate = false
		);

		int DLL_DEFINE brush_reduce(mip::MeshTopology * _pMT, std::vector<std::vector<muint32>>& in_dimArrVertIdx, std::vector<muint32> & _vt_idx,
			int selMethod, mip::VECTOR3 pick, int pickVi = -1, int factor = 30, float radi = 1.00f
		);

		int DLL_DEFINE brush_refine(mip::MeshTopology * _pMT, std::vector<std::vector<muint32>>& _2ArrforDisplayVerts, mip::mesh_control::IndexStruct & out_IndexList,
			const int pickVi, const mip::VECTOR3 pick, int selMethod = 0, int factor = 50.0f, float radi = 1.00f, float radiPer = 50.0f
		);

		int DLL_DEFINE brush_Move(mip::MeshTopology * _pMT, std::vector<std::vector<muint32>>& _2ArrforDisplayVerts, mip::mesh_control::IndexStruct & out_IndexList,
			const int pickVi, const mip::VECTOR3 pick, mip::VECTOR3& out_oldPick, mip::RAY ray, mip::VECTOR3 fixedDir, float radi, float radiPer, int mode, int selMethod = 0, bool updateDisplay = true
		);
		
		int DLL_DEFINE brush_drag(mip::MeshTopology * _pMT, std::vector<std::vector<muint32>>& _2ArrforDisplayVerts, mip::mesh_control::IndexStruct & out_IndexList,
			const int pickVi, const mip::VECTOR3 pick, const mip::VECTOR3 fixedPos, mip::RAY ray, const mip::VECTOR3 fixedDir, int selMethod = 0, int factor = 30, float radi = 1.00f, float radiPer = 50.0f
		);

		int DLL_DEFINE createMatchingData(mip::MeshTopology* _pMT);

		// -----------------------------------------------------------
		// Sub Support Fuction
		// -----------------------------------------------------------

		// return  0 : boundary mesh,  -1 : multimesh,  -2 : non-manifold
		int DLL_DEFINE validateMesh(std::vector<VECTOR3> & _verts, std::vector<muint32> & _tris, std::vector<VECTOR3> & _normals);

		int DLL_DEFINE ConstructIndexByVertex(std::vector<VECTOR3> & _InOutVerts, std::vector<muint32> & _tris, bool _bMakeNoOverLapVertex = true);

		int DLL_DEFINE ConstructNormalByNoOverLapVertex(std::vector<mip::VECTOR3> & _verts, std::vector<muint32> & _idxs, std::vector<VECTOR3> & _OutNormals, bool _bChangeIndexToAscendingOrder = true, bool _bMakeOverLapVertex = true);

		int DLL_DEFINE getDisplayIndexByVertex(mip::MeshTopology* _pMeshTopology, std::vector<std::vector<muint32>>& out_dimArrVertIdx, int noOverlapVertSize );

		int DLL_DEFINE hollowOk(mip::MeshTopology * in_mesh, mip::MeshTopology * in_mesh2);
		int DLL_DEFINE deleteNonManifoldVert(mip::MeshTopology * in_mesh, bool onlyCheck = false, bool onlyEdge = false);
		int DLL_DEFINE getNonManiHEdgeCnt(mip::MeshTopology * in_mesh);
		int DLL_DEFINE RecontstructTopology(mip::MeshTopology * in_mesh, mip::MeshTopology * _newMesh, bool bBuildtri = true);

		int DLL_DEFINE saveTopology(mip::MeshTopology * IN_pMT, char* _path, char* _mode = "wb");
		int DLL_DEFINE loadTopology(mip::MeshTopology * OUT_pMT, char * _path, bool _buildTree = true);

		float DLL_DEFINE getMaxDeviation(mip::MeshTopology * _pMT);
		int DLL_DEFINE getTopologyTriCnt(mip::MeshTopology * _pMT);

		int DLL_DEFINE brush_clear(mip::MeshTopology * _pMT);
		int DLL_DEFINE getRealPickPoint(mip::MeshTopology * pMT, mip::RAY ray, int pickVi, mip::VECTOR3 & out_realPickPt);

		int DLL_DEFINE updateDisplay(mip::MeshTopology * _pMT);
		int DLL_DEFINE updateDisplayDrag(mip::MeshTopology * _pMT, mip::mesh_control::IndexStruct & IndexStruct);

		int DLL_DEFINE editOctree(mip::MeshTopology * pMT, mip::OcNode * node, mip::mesh_control::IndexStruct & idxStruct);

		int DLL_DEFINE buildTopologyHEdgeOpposite(std::vector<TVert> & _verts, std::vector<THEdge> & _Hedges);

		int DLL_DEFINE DeleteTTri(mip::MeshTopology * _pMT, int fi);

		bool DLL_DEFINE isTriFlag(mip::MeshTopology * _pMT, int flag);

		mip::QUATERNION DLL_DEFINE RotationBetween_Vectors(mip::VECTOR3 start, mip::VECTOR3 dest);

		bool DLL_DEFINE getPickPtOfPlane(const mip::VECTOR3 & _triPtA, const mip::VECTOR3 & _triPtB, const mip::VECTOR3 & _triPtC, const mip::VECTOR3 In_Pt, const mip::VECTOR3 In_Dir, mip::VECTOR3& pOut_Pt);		

		// -----------------------------------------------------------
		// Test
		// -----------------------------------------------------------
		int DLL_DEFINE collapseCancel(mip::MeshTopology* _pMeshTopology);
		int DLL_DEFINE collapseVert(mip::MeshTopology* in_mesh, int pickId);

		// Conversion between mesh topology and mask
		bool DLL_DEFINE maskToMeshTopology(
											MeshTopology* pMesh,
											const std::vector<VECTOR4> & vecVoxelPosition,
											const VECTOR3 & extent,
											const VECTOR3 & space,
											const VECTOR3 & origin,
											bool bBuildTree = false
											);

		// Mesh 2D Outline
		bool DLL_DEFINE calcMesh2Outline(
											std::vector<std::vector<VECTOR3>>& vecOutline,
											const PLANE& plane,
											const MeshTopology* pMeshTolpology,
											MATRIX44* pTransMat
										);

	};
};