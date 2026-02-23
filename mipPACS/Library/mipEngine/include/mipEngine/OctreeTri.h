#pragma once
#include "Topology.h"
#include <concurrent_vector.h>

#define TEST_PART_OCTREE_RENDER 0

namespace mip
{
	class OcNode : public TBase
	{
	public:
		OcNode				*pChildren[8];

		// 경계 상자
		AABB				box;
		std::vector<int>	tris;
		int					index;
	public:
		OcNode()
		{
			for (unsigned long i = 0; i<8; i++)
				pChildren[i] = NULL;
		}
		~OcNode()
		{
			for (unsigned long i = 0; i<8; i++)
				SAFE_DELETE(pChildren[i]);
		}
		void set(float XPos, float YPos, float ZPos, float Size)
		{
			box.set(AARBB(XPos, YPos, ZPos, Size));
		}
		void set(AARBB & aabb)
		{
			box = aabb;
		}
		void set(AABB & aabb)
		{
			box = aabb;
		}
	};

	class OcTreeTri
	{
	public:
		OcTreeTri(TREETYPE type = OCTREE, unsigned int MaxCount = 100, float MaxSize = 1.0f);
		~OcTreeTri();

	public:
		bool buildTree(std::vector<TTri> * ttris, std::vector<TVert> * tverts, AABB & box);
		bool buildTreePPL(std::vector<TTri> * ttris, std::vector<TVert> * tverts, AABB & box, int n_core);
//		void drawTest();
		void setType(TREETYPE type , unsigned int MaxCount = 100, float MaxSize = 1.0f)
		{
			m_rpTtris = NULL;  m_root = NULL;  m_type = type; m_maxSize = MaxSize, m_maxCount = MaxCount;
		}
		bool isLoaded() { return m_root != NULL; }
		bool intersectRay(const mip::VECTOR3 & org, const mip::VECTOR3 & dir, OcNode * node, bool root = false, mip::VECTOR3 * r = NULL);
#if TEST_PART_OCTREE_RENDER
		bool intersectRayTEST(const mip::VECTOR3 & org, const mip::VECTOR3 & dir, OcNode * node, std::vector<int> & passBoxIdx, bool root = false, mip::VECTOR3 * r = NULL);
#endif
		bool intersectPlane(const PLANE & plane, OcNode * node, bool root = false, std::vector<mip::VECTOR3> * r = NULL);
		void clear(OcNode * node);
		OcNode * getRoot() { return m_root; }
		void	 setRoot(OcNode * node) { m_root = node; }
		TREETYPE getType() { return m_type; }

		std::vector<int>	 m_checklist;
		//std::vector<OcNode*> m_nodelist; //20210515_byPHS_부분빌드관련추가
		concurrency::concurrent_vector<OcNode*> m_nodelist; //210616 허건과장

	private:
		float				m_maxSize;
		unsigned int		 m_maxCount;

		std::vector<TTri>  * m_rpTtris;
		std::vector<TVert> * m_rpTVerts;

		OcNode		* m_root;
		TREETYPE	m_type;
		VECTOR3			m_size;


		bool sortNode(OcNode *Node, AABB & aabb, bool root = false, std::vector<int> * list = NULL);
		unsigned int countTris(AABB & aabb, bool root = false, std::vector<int> * list = NULL );
		bool isPolygonContained(int ttri_index, AABB & aabb);
	};
};