#pragma once

#include "mip/core.h"

#include <vector>

namespace mip
{
	enum {
		DELETED = 0x0001,		// This bit indicate that the edge is deleted from the mesh
		NOTREAD = 0x0002,		// This bit indicate that the edge of the mesh is not readable
		NOTWRITE = 0x0004,		// This bit indicate that the edge is not modifiable
		MODIFIED = 0x0008,		// This bit indicate that the edge is modified
		VISITED = 0x0010,		// This bit can be used to mark the visited edge
		SELECTED = 0x0020,		// This bit can be used to select 
		BORDER = 0x0100,		// Border Flag
		USER_DEL = 0x0200,		// DEL user bit
		USER_UNDER1 = 0x0400,	// UNDER user bit
		USER_UNDER2 = 0x0800,	// UNDER user bit ( more under )
		USER_SEL1 = 0x1000,		// SELECTED 1 (inner) user bit
		USER_SEL2 = 0x2000,		// SELECTED 2 (outer) user bit
		USER_SEL3 = 0x4000,		// SELECTED 3 (exten) user bit
		USER_CHANG = 0x8000		// CHANGED (pos normal) user bit
	};

	enum TREETYPE
	{
		QUADTREE = 4,
		OCTREE = 8
	};

	enum GET_NODE_TYPE
	{
		GNT_INTERSECT,
		GNT_FRONT,
		GNT_FRONT_AND,
		GNT_REAR,
		GNT_REAR_AND,
		GNT_ALL,
	};

	class TBase
	{
	public:
		TBase() {
			clearFlags();
		}
	public:
		int getFlag() { return m_flag; }
		bool isD() const { return (m_flag & DELETED) != 0; } ///  checks if the vertex is deleted
		bool isR() const { return (m_flag & NOTREAD) == 0; } ///  checks if the vertex is readable
		bool isW() const { return (m_flag & NOTWRITE) == 0; }///  checks if the vertex is modifiable
		bool isRW() const { return (m_flag & (NOTREAD | NOTWRITE)) == 0; }/// This funcion checks whether the vertex is both readable and modifiable
		bool isS() const { return (m_flag & SELECTED) != 0; }///  checks if the vertex is Selected
		bool isB() const { return (m_flag & BORDER) != 0; }///  checks if the vertex is a border one
		bool isV() const { return (m_flag & VISITED) != 0; }///  checks if the vertex Has been visited

		bool isUD() const { return (m_flag & USER_DEL) != 0; }///  checks if the vertex Has been user DEL
		bool isUU1() const { return (m_flag & USER_UNDER1) != 0; }///  checks if the vertex Has been user UNDERCUT
		bool isUU2() const { return (m_flag & USER_UNDER2) != 0; }///  checks if the vertex Has been user UNDERCUT more under
		bool isUS1() const { return (m_flag & USER_SEL1) != 0; }///  checks if the vertex Has been user SELECTED 1 (inner)
		bool isUS2() const { return (m_flag & USER_SEL2) != 0; }///  checks if the vertex Has been user SELECTED 2 (outer)
		bool isUS3() const { return (m_flag & USER_SEL3) != 0; }///  checks if the vertex Has been user SELECTED 3 (exten)
		bool isUC() const { return (m_flag & USER_CHANG) != 0; }///  checks if the vertex Has been user USER CHANGED (pos normal)

		void clearFlags() { m_flag = 0; }
		void setD() { m_flag |= DELETED; }///  deletes the edge from the mesh
		void clearD() { m_flag &= (~DELETED); }///  un-delete a edge
		void setR() { m_flag &= (~NOTREAD); }///  marks the edge as readable
		void clearR() { m_flag |= NOTREAD; }///  marks the edge as not readable
		void clearW() { m_flag |= NOTWRITE; }///  marks the edge as writable
		void setW() { m_flag &= (~NOTWRITE); }///  marks the edge as not writable
		void setS() { m_flag |= SELECTED; }///  select the edge
		void clearS() { m_flag &= ~SELECTED; }/// Un-select a edge
		void setB() { m_flag |= BORDER; }
		void clearB() { m_flag &= ~BORDER; }
		void setV() { m_flag |= VISITED; }
		void clearV() { m_flag &= ~VISITED; }

		void setUD() { m_flag |= USER_DEL; }
		void clearUD() { m_flag &= ~USER_DEL; }
		void setUU1() { m_flag |= USER_UNDER1; }
		void clearUU1() { m_flag &= ~USER_UNDER1; }
		void setUU2() { m_flag |= USER_UNDER2; }
		void clearUU2() { m_flag &= ~USER_UNDER2; }
		void setUS1() { m_flag |= USER_SEL1; }
		void clearUS1() { m_flag &= ~USER_SEL1; }
		void setUS2() { m_flag |= USER_SEL2; }
		void clearUS2() { m_flag &= ~USER_SEL2; }
		void setUS3() { m_flag |= USER_SEL3; }
		void clearUS3() { m_flag &= ~USER_SEL3; }
		void setUC() { m_flag |= USER_CHANG; }
		void clearUC() { m_flag &= ~USER_CHANG; }
	private:
		unsigned int m_flag;
	};

	class TEdge : public TBase
	{
	public:
		TEdge()
		{
			e[0] = -1; e[1] = -1;
		}
	public:
		int		e[2];
	};

	class THEdge : public TBase
	{
	public:
		THEdge()
		{
			hei = -1;
			vi = -1;
			fi = -1;
			//			prev_ei = -1;
			//			next_ei = -1;
			opp_ei = -1;
		}
	public:
		int		hei;	 // index
		int		vi;		 // Vertex

		int		fi;		 // right face
						 //		int		prev_ei;
						 //		int		next_ei;
		int		opp_ei;  // opposite
	};

	class TTri : public TBase
	{
	public:
		enum
		{
			TRI_SIZE = 3,
		};
	public:
		TTri()
		{

		}

	public:
		int			vi[3];	// vertexs
		int			hei;	// half-edge
	};

	class TVert : public TBase
	{
	public:
		TVert()
		{
			pos.x = 0.0f; pos.y = 0.0f; pos.z = 0.0f;
			//			hei = -1;
			vi = -1;
		}
		TVert(mip::VECTOR3 & v)
		{
			pos = v;
			//			hei = -1;
			vi = -1;
		}

	public:
		inline bool operator == (TVert const & p) const
		{
			return pos.x == p.pos.x && pos.y == p.pos.y && pos.z == p.pos.z;
		}
		inline bool operator != (TVert const & p) const
		{
			return pos.x != p.pos.x && pos.y != p.pos.y && pos.z != p.pos.z;
		}
		inline bool operator < (TVert const & p) const
		{
			return	(pos.z != p.pos.z) ? (pos.z < p.pos.z) :
				(pos.y != p.pos.y) ? (pos.y < p.pos.y) :
				(pos.x < p.pos.x);
		}
		inline bool operator > (TVert const & p) const
		{
			return	(pos.z != p.pos.z) ? (pos.z > p.pos.z) :
				(pos.y != p.pos.y) ? (pos.y > p.pos.y) :
				(pos.x > p.pos.x);
		}
		inline bool operator <= (TVert const & p) const
		{
			return	(pos.z != p.pos.z) ? (pos.z < p.pos.z) :
				(pos.y != p.pos.y) ? (pos.y < p.pos.y) :
				(pos.x <= p.pos.x);
		}
		inline bool operator >= (TVert const & p) const
		{
			return	(pos.z != p.pos.z) ? (pos.z > p.pos.z) :
				(pos.y != p.pos.y) ? (pos.y > p.pos.y) :
				(pos.x >= p.pos.x);
		}
		void linkTri(int t)
		{
			for (int n = 0; n < tis.size(); n++)
			{
				if (tis[n] == t) return;
			}

			tis.push_back(t);
		}
		void linkHEdge(int t)
		{
			for (int n = 0; n < heis.size(); n++)
			{
				if (heis[n] == t) return;
			}

			heis.push_back(t);
		}

	public:
		int					    vi;		// vertex index ( for mapping try index )
		mip::VECTOR3		    pos;
//		int						hei;	// half-edge
		std::vector<int>		tis;	// tris index
		std::vector<int>		heis;	// half-edges
		float					weight; // shortest path
		mip::VECTOR3		    ori_pos;// original-pos
		mip::VECTOR3		    ori_nor;// original-normal
	};
};