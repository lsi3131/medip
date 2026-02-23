#pragma once
#include "math/math.h"
#include "Renderer/color.h"
#include "mip/MeshTopology.h"

namespace mip
{
	class MeshCore;

	enum { STL_LABEL_SIZE = 80 };

	enum LOAD_MASK 
	{
		LM_NONE = 0x00000,

		LM_VERTCOORD = 0x00001,
		LM_VERTFLAGS = 0x00002,
		LM_VERTCOLOR = 0x00004,
		LM_VERTQUALITY = 0x00008,
		LM_VERTNORMAL = 0x00010,
		LM_VERTTEXCOORD = 0x00020,
		LM_VERTRADIUS = 0x10000,
		
		LM_EDGEINDEX = 0x80000,
		
		LM_FACEINDEX = 0x00040,
		LM_FACEFLAGS = 0x00080,
		LM_FACECOLOR = 0x00100,
		LM_FACEQUALITY = 0x00200,
		LM_FACENORMAL = 0x00400,
		
		LM_WEDGCOLOR = 0x00800,
		LM_WEDGTEXCOORD = 0x01000,
		LM_WEDGTEXMULTI = 0x02000, // when textrue index is explicit
		LM_WEDGNORMAL = 0x04000,
		
		LM_BITPOLYGONAL = 0x20000, // loads explicit polygonal mesh
		
		LM_CAMERA = 0x08000,
		
		LM_FLAGS = LM_VERTFLAGS + LM_FACEFLAGS,
		
		LM_ALL = 0xFFFFF
	};

	namespace model
	{
		struct ObjIndexedFace
		{
			void set(const int & num){ v.resize(num); n.resize(num); t.resize(num); }
			std::vector<int> v;
			std::vector<int> n;
			std::vector<int> t;
			int tInd;
			bool  edge[3];// useless if the face is a polygon, no need to have variable length array
			mip::COLOR c;
		};

		struct ObjEdge
		{
			int v0;
			int v1;
		};

		struct Material
		{
			unsigned int index;//index of material
			std::string materialName;

			mip::VECTOR3 Ka;//ambient
			mip::VECTOR3 Kd;//diffuse
			mip::VECTOR3 Ks;//specular

			float		d;//alpha
			float		Tr;//alpha

			int			illum;//specular illumination
			float		Ns;

			std::string map_Kd; //filename texture
		};

		/*
		*  A face polygon composed of more than three vertices is triangulated
		*  according to the following schema:
		*                      v5
		*                     /  \
		*                    /    \
		*                   /      \
		*                  v1------v4
		*                  |\      /
		*                  | \    /
		*                  |  \  /
		*                 v2---v3
		*
		*  As shown above, the 5 vertices polygon (v1,v2,v3,v4,v5)
		*  has been split into the triangles (v1,v2,v3), (v1,v3,v4) e (v1,v4,v5).
		*  This way vertex v1 becomes the common vertex of all newly generated
		*  triangles, and this may lead to the creation of very thin triangles.
		*
		*  This function is intended as a trivial fallback when glutessellator is not available.
		*  it assumes just ONE outline
		*/
		template < class PointType >
		void FanTessellator(const std::vector< std::vector<PointType> > & outlines, std::vector<int> & indices)
		{
			indices.clear();
			if (outlines.empty()) return;
			const std::vector<PointType> &points = outlines[0];

			for (size_t i = 0; i<points.size() - 2; ++i)
			{
				indices.push_back(0);
				indices.push_back(i + 1);
				indices.push_back(i + 2);
			}
		}
		
		// .STL
		bool LoadSTLFile(const std::string & filename, MeshTopology * mesh, float scale = 1.0f);

		bool IsSTLBinary(const char * filename);
		bool OpenBinary(const char * filename, int &loadMask, MeshTopology * mesh, float scale = 1.0f);
		bool OpenAscii(const char * filename, int &loadMask, MeshTopology * mesh, float scale = 1.0f);
		bool IsSTLColored(const char * filename, bool &magicsMode);

		bool SaveSTLFile(const char * filename, MeshTopology * mesh, float scale = 1.0f, bool binary = true, bool invert = true, const char *objectname = 0, bool magicsMode = 0);

		// .OBJ
		bool LoadObjFile(const char * filename, MeshTopology * mesh, float scale = 1.0f);
		void TokenizeNextLine(std::ifstream &stream, std::vector< std::string > &tokens, std::vector<FCOLOR> *colVec);
		void SplitToken(const std::string & token, int & vId, int & nId, int & tId, int mask);
		bool GoodObjIndex(int &index, const int maxVal);
		bool LoadMaterials(const char * filename, std::vector<Material> &materials, std::vector<std::string> &textures);
	};
};