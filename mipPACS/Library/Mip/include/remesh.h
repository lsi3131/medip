#pragma once
#include "meshlib.h"

namespace mip
{
	namespace mesh
	{
		// factor = check factor > 0.1f
		int smooth(std::vector<VECTOR3> & _verts, std::vector<muint32> & _tris, std::vector<VECTOR3> & _normals, 
			float factor = 2.0f, meshLibprogUpdatefunc update = NULL, void * data = NULL);

		// levelCount : 1 ~ 4
		int subdivideFace(std::vector<VECTOR3> & _verts, std::vector<muint32> & _tris, std::vector<VECTOR3> & _normals, int levelCount,
						meshLibprogUpdatefunc update = NULL, void * data = NULL);

		// reduction : 1 ~ 100
		int collapseFace(std::vector<VECTOR3> & _verts, std::vector<muint32> & _tris, std::vector<VECTOR3> & _normals, int reduction,
						meshLibprogUpdatefunc update = NULL, void * data = NULL);

		// offset : -1.0 ~ 1.0
		// resolution : 0 (auto) ~ 512
		int solid(std::vector<VECTOR3> & _verts, std::vector<muint32> & _tris, std::vector<VECTOR3> & _normals, float offset = 0.0f, 
						int resolution = 0, meshLibprogUpdatefunc update = NULL, void * data = NULL);
		
		// nffset : -1.0 ~ -0.01
		// resolution : 0 (auto) ~ 512
		int hollow(std::vector<VECTOR3> & _verts, std::vector<muint32> & _tris, std::vector<VECTOR3> & _normals, float noffset = -0.1f,
						int resolution = 0, meshLibprogUpdatefunc update = NULL, void * data = NULL);

		int mergePoint( std::vector<VECTOR3> & _verts, std::vector<muint32> & _tris, std::vector<VECTOR3> & _normals );

		// return  0 : boundary mesh,  -1 : multimesh,  -2 : non-manifold
		int validateMesh ( std::vector<VECTOR3> & _verts, std::vector<muint32> & _tris, std::vector<VECTOR3> & _normals );
	};
};