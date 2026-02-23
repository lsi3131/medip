#pragma once
#include "math/math.h"
#include "math/Transform.h"

#ifdef USE_OPENGL
#define VAO_ATTRIB_ID_VERTEX	0
#define VAO_ATTRIB_ID_NORMAL	1
#define VAO_ATTRIB_ID_STATE		2
#define VAO_ATTRIB_ID_COLOR		3
#define VAO_ATTRIB_ID_TEX_COORD	4
#endif

namespace mip
{
	class COLOR;
	class Renderer;
	class OcTree;

	enum MESH_BUFFER_TYPE
	{
		MBT_P3 = 0,
		MBT_P3C,
		MBT_P3N3,
		MBT_P3T2,
		MBT_P3T3,
	};

	enum DRAWTYPE;

	struct DrawCall
	{
		mip::DRAWTYPE		drawType;
		int						polyCnt;
	};

	class MeshCore : public TRANSFORM
	{
	public:
		MeshCore(mip::Renderer * renderer);
		virtual ~MeshCore();

	public:
		mip::AABB						m_boundingBox;
		mip::OcTree	*					m_tree;
		
		std::vector<muint32>			m_tris;
		std::vector<mip::VECTOR3>		m_normals;
		std::vector<mip::VECTOR3>		m_edgeNormals;
		std::vector<mip::VECTOR3>		m_verts;
		mip::VECTOR3					m_vTranslateOffset;

		std::vector<mip::VECTOR2>		m_uv;
		std::vector<mip::VECTOR3>		m_uv3;
		std::vector<mip::COLOR>			m_vertColors;

		std::vector<std::string>		m_textures;

		float							m_scale;
	protected:
		bool							m_bLoaded;
		int								m_polyCnt;

		std::vector<DrawCall>			m_drawcall;

#ifdef USE_OPENGL
		muint32							m_VAO;

		muint32							m_pointBuffer;
		muint32							m_normalBuffer;
		muint32							m_indexBuffer;
		muint32							m_uvBuffer;

		muint32							m_shaderProgram;
#else
		VERTEX_ID						m_vb;
		INDEX_ID						m_ib;
#endif
		mip::Renderer*					m_rpRenderer;

		MESH_BUFFER_TYPE				m_bufferType;
	public:
		void release();
		void reset();

		// Core Data
		bool LoadSTL(const char*  filename);
		bool LoadObj(const char*  filename);

		// CreateModel
		bool createPlaneGrid(mip::MeshCore * m, mip::VECTOR3 & L1, mip::VECTOR3 & L2, mip::VECTOR3 & select_point, mip::VECTOR3 & camera_dir, float cx, float cy, float cellsize);
		bool createPlaneGrid(const mip::PLANE plane, bool xy, float cx, float cy, float cellsize);
		bool createPlaneLine(const mip::PLANE plane, bool xy, float cx, float cy, float cellsize);
		bool createQuad(muint32 cx, muint32 cy, bool xz = false);

		// Render
		bool buildRenderBuffer(MESH_BUFFER_TYPE type = MBT_P3);
		bool recreateCentor();
		bool createMinMax();
		bool createNormal();

		bool renderBegin(muint32 shaderProgram = 0);
		bool renderBegin(mip::Renderer * renderer, muint32 shaderProg = 0, bool backface_culling = false);
		bool renderEnd(mip::Renderer * renderer, muint32 shaderProg = 0);
		bool renderPosition(const mip::MATRIX44 * world, const mip::MATRIX44 & view, const mip::MATRIX44 & proj, const mip::COLOR * color = NULL, bool vr_mode = false, const mip::MATRIX44 * vr_view = NULL);
		bool renderPosition(const mip::MATRIX44 * world, const mip::COLOR * color = NULL, bool reverse_right = false);
		bool setLight2(const mip::VECTOR3 & lightPos, const mip::VECTOR3 & player_pos, const mip::MATRIX44 * matWorld = NULL);
		bool render(bool wire = false, mint32 shaderpass = 0);
		bool renderEnd();

		bool rendering(const mip::MATRIX44 * world, const mip::COLOR * color = NULL);
		bool rendering_Zoff(const mip::MATRIX44 * world, const mip::COLOR * color = NULL);
		
		bool intersectRayBox(const mip::VECTOR3 & org, const mip::VECTOR3 & dir, mip::MATRIX44 * mat = NULL);
		bool intersectRayBoxPos(const mip::VECTOR3 & org, const mip::VECTOR3 & dir, mip::VECTOR3 & intersectpos, mip::MATRIX44 * mat = NULL);

		void setColor(mip::COLOR color);

		// Device for DX
		//bool checkDevice();
		//void restoreDeviceObjects();

		inline bool isLoaded()		{ return m_bLoaded; }

		virtual bool isOutBounder()		{ return false; }
		virtual bool isTopologyed()		{ return false; }
		virtual void buildTree();
		virtual bool intersectRay(const mip::VECTOR3 & org, const mip::VECTOR3 & dir, mip::VECTOR3 & result, int & test);

		virtual bool intersectPlane(const PLANE & world_plane, std::vector<mip::VECTOR3> * r = NULL, bool transformPlane = true);
		virtual bool splitFromDragLines(const std::vector<mip::VECTOR3> & lines, mip::VECTOR3 & select_point, mip::VECTOR3 & camera_dir);
		virtual bool splitFromPlane(mip::VECTOR3 & L1, mip::VECTOR3 & L2, mip::VECTOR3 & select_point, mip::VECTOR3 & camera_dir, std::vector<mip::VECTOR3> * edge_list = NULL);
		//virtual bool splitFromPlane(mip::MeshCore * mesh1, mip::MATRIX44 & mat1, mip::MeshCore * mesh2, mip::MATRIX44 & mat2);
		virtual bool splitFromPlanes(const std::vector<PLANE> & local_planes, bool clipRear = false);

		virtual bool intersectMesh(MeshCore * mesh, std::vector<mip::VECTOR3> * r = NULL);
		//virtual bool splitFromMesh(MeshCore * mesh, MESH_SPLIT_TYPE cuttype = MST_CUT_A_REMAINED, std::vector<mip::VECTOR3> * ret1 = NULL, std::vector<mip::VECTOR3> * ret2 = NULL);
		//virtual bool attechMeshPosition(mip::MATRIX44 & mat1, mip::MeshCore * mesh2, mip::MATRIX44 & mat2, bool isUpdate);
		//virtual void dettechMeshPosition();
	};
};
