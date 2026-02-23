#pragma once
#include "mip/Mesh.h"
#include "mip/octreeTri.h"
#include "math/BezierCurve.h"

//#define CHECK_BOOST

#ifdef CHECK_BOOST
#include <boost/graph/adjacency_list.hpp>

typedef int	vertex_t;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, vertex_t> graph_type;
typedef boost::graph_traits<graph_type>::vertex_descriptor vdesc_t;
#endif

class QProgressDialog;

namespace mip
{
	class MeshTopology : public MeshCore
	{
	public:
		MeshTopology(mip::Renderer * renderer);
		virtual ~MeshTopology();

	public:
		// Topology
		OcTreeTri *							m_treeTris;
		std::vector<TTri>					m_ttris;
		std::vector<TVert>					m_tverts;
		std::vector<THEdge>					m_tVHedges;
		std::vector<muint32>				m_nIndex;
#ifdef CHECK_BOOST
		std::map<int, vdesc_t>				m_gverts;
		graph_type							m_graph;
#endif
		std::vector<mip::VECTOR3>		m_intersect_check_list;     // 3D Model List fast check.
		std::vector<mip::VECTOR3>		m_undercut_check_list;      // Undercut Area extension check.
		std::vector<mip::VECTOR3>		m_undercut_gap_check_list;  // Undercut gab check.

	private:
		bool								m_bTopologyed;
		bool								m_bOutBounder;

		bool buildVertexEdge(TVert & v);
		bool buildVertexTri(int t);

	public:
		// buildTopology
		void updateVertex();
		bool mergingVertex();
		bool buildTopologyHEdge();
		
		bool getShortestPath(int startV, int endV, std::vector<int> & list, std::vector<int> & allist);
		bool getSelectedTris(const mip::VECTOR3 & eye, std::vector<int> & loop, std::vector<TTri*> & list);
		void addSelectedTris(std::vector<TTri*> & list, TTri * tri);

		//bool createSurgicalGuide(mip::MeshCore * mesh, std::vector<int> & loop, std::vector<TTri*> & list);
		//bool createUndercutArea(mip::MeshCore * mesh, std::vector<TTri*> & list, mip::VECTOR3 & dir);
		//bool createUndercutMesh(mip::MeshCore * mesh, std::vector<int> & loop, std::vector<TTri*> & list, mip::VECTOR3 & dir, bool isThinckness);
		//bool deleteUndercutDownPath(mip::TVert & tvert, mip::VECTOR3 & guide_vec);
		int  checkConnectNodeCnt(mip::TVert & tvert);
		//int  checkCylinderThickness(mip::VECTOR3 pos, float in_thickness = 0.f, float out_thickness = 0.f, float ext_thickness = 0.f);
		bool checkMeshIntersectRay(const mip::VECTOR3 & org, const mip::VECTOR3 & dir, std::vector<mip::VECTOR3> & intersect_check_list, float scale, mip::VECTOR3 * result = NULL);
		bool checkMeshIntersectNear(const mip::VECTOR3 & vec, std::vector<mip::VECTOR3> & intersect_check_list, float dis);
		bool insertIntersectList(std::vector<mip::VECTOR3> & intersect_check_list, const mip::VECTOR3 & vec, TTri* ptri, float dis_limit);
		void checkUndercutArea(TVert & pstvert, mip::VECTOR3 & normal_dir, mip::VECTOR3 & dir, mip::MeshCore * mesh);
		void insertkUndercutAreaList(TVert & pstvert, std::vector<TTri*> & undercut_check_list);
		bool checkUndercutListAndMove(TTri* ptri);
		void createGuideAreaLoopBezier(Curves::BezierCurve & bezierCurve, std::vector<int> & re_loop);

		bool buildRenderBufferTopology(bool bRebuild = false);

		void setTopologyed(bool _bTopology) { m_bTopologyed = _bTopology; }
		virtual bool isOutBounder()		{ return m_bOutBounder; }
		virtual bool isTopologyed()		{ return m_bTopologyed; }
		virtual void buildTree();

		virtual bool intersectRay(const mip::VECTOR3 & org, const mip::VECTOR3 & dir, mip::VECTOR3 & result, int & test, bool clearflag = true);
		virtual bool intersectPlane(const PLANE & world_plane, std::vector<mip::VECTOR3> * r = NULL, bool transformPlane = true);
		virtual bool splitFromDragLines(const std::vector<mip::VECTOR3> & lines, mip::VECTOR3 & select_point, mip::VECTOR3 & camera_dir);
		virtual bool splitFromPlane(mip::VECTOR3 & L1, mip::VECTOR3 & L2, mip::VECTOR3 & select_point, mip::VECTOR3 & camera_dir, std::vector<mip::VECTOR3> * edge_list = NULL);
		//virtual bool splitFromPlane(mip::MeshCore * mesh1, mip::MATRIX44 & mat1, mip::MeshCore * mesh2, mip::MATRIX44 & mat2);
		virtual bool splitFromPlanes(const std::vector<PLANE> & local_planes, bool clipRear = false);

		virtual bool intersectMesh(MeshCore * mesh, std::vector<mip::VECTOR3> * r = NULL);
	};
};
