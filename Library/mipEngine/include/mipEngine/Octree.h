#pragma once
#include "Topology.h"

namespace mip
{
	class OcNodeBase : public TBase
	{
	public:
		OcNodeBase			*pChildren[8];
		
		// 경계 상자
		AABB				box;
		std::vector<muint32>	tris;
	public:
		OcNodeBase()
		{
			for (unsigned long i = 0; i<8; i++)
				pChildren[i] = NULL;
		}
		~OcNodeBase()
		{
			for (unsigned long i = 0; i<8; i++)
				SAFE_DELETE(pChildren[i]);
		}
		void set(float XPos, float YPos, float ZPos, float Size)
		{
			box.set(AARBB(XPos, YPos, ZPos, Size));
		}
		void set(AARBB & aarbb)
		{
			box.set(aarbb);
		}
		void set(AABB & aabb)
		{
			box = aabb;
		}
	};

	class OcTree
	{
	public:
		OcTree(TREETYPE type = OCTREE, int MaxCount = 100, float MaxSize = 1.0f);
		~OcTree();

	public:
		bool buildTree(std::vector<muint32> * tris, std::vector<VECTOR3> * verts, AABB & box);
		//		void drawTest();
		void setType(TREETYPE type, unsigned int MaxCount = 100, float MaxSize = 1.0f)
		{
			m_rpTris = NULL;  m_root = NULL;  m_type = type; m_maxSize = MaxSize, m_maxCount = MaxCount;
		}
		bool isLoaded() { return m_root != NULL; }
		void clear(OcNodeBase * node);
		OcNodeBase * getRoot() { return m_root; }
		TREETYPE getType() { return m_type; }

		std::vector<int>	 m_checklist;
		std::vector<TBase>   m_visit;
		std::vector<OcNodeBase*> m_checkTree;
	private:
		float				m_maxSize;
		unsigned int		 m_maxCount;

		std::vector<muint32>  * m_rpTris;
		std::vector<VECTOR3> * m_rpVerts;

		OcNodeBase		* m_root;
		TREETYPE		m_type;
		VECTOR3			m_size;
		MATRIX44		m_ToLocal;

	private:
		bool sortNode(OcNodeBase *Node, AABB & aabb, bool root = false, std::vector<muint32> * list = NULL);
		unsigned int countTris(AABB & aabb, bool root = false, std::vector<muint32> * list = NULL);
		bool isPolygonContained(muint32 t1, muint32 t2, muint32 t3, AABB & aabb);

		// intersect
	public:
		void setToLocalMatrix(MATRIX44 & mat) { m_ToLocal = mat; }
		MATRIX44 getToLocalMatrix()			{ return m_ToLocal; }

		bool intersectRay(const mip::VECTOR3 & org, const mip::VECTOR3 & dir, OcNodeBase * node, bool root = false, mip::VECTOR3 * r = NULL);
		bool intersectPlane(const PLANE & plane, OcNodeBase * node, bool root = false, std::vector<mip::VECTOR3> * r = NULL);
		bool intersectPlane(const PLANE & plane, OcNodeBase * node, bool root, std::vector<OcNodeBase*> & nodes, GET_NODE_TYPE type = GNT_INTERSECT);
		bool intersectPlane2(const PLANE & plane, OcNodeBase * node, bool root, std::vector<OcNodeBase*> & nodes1, GET_NODE_TYPE type1, std::vector<OcNodeBase*> & intersect_node);
		bool intersectBox(const AABB & box, OcNodeBase * node, bool root = false, std::vector<mip::OcNodeBase*> * r = NULL);
		bool intersectSphere(const SPHERE & s, OcNodeBase * node, bool root, std::vector<mip::OcNodeBase*> * r);
		bool intersectOcTree(mip::OcTree * tree, OcNodeBase * node, bool root = false, std::vector<mip::VECTOR3> * r = NULL);
		bool intersectOcTree(mip::OcTree * tree, OcNodeBase * node, bool root, std::vector<OcNodeBase*> & nodes, std::vector<OcNodeBase*> * not_intersed_tris = NULL);
	};
};