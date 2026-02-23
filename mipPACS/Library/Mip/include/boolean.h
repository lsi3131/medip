#pragma once
#include "meshlib.h"

enum BOTYPE
{
	BOT_INTERSECT,
	BOT_DIFF,
	BOT_UNION,
};

namespace mip
{
	namespace mesh
	{
		int createCube(const VECTOR3 & size, std::vector<VECTOR3> & _verts, std::vector<muint32> & _tris, std::vector<VECTOR3> & _normals);
		
		// resolution > 5
		int createCylinder(const VECTOR3 & size, int resolution, std::vector<VECTOR3> & _verts, std::vector<muint32> & _tris, std::vector<VECTOR3> & _normals);

		// resolution > 5
		int createSphere(const VECTOR3 & size, int theta_resolution, int phi_resolution, std::vector<VECTOR3> & _verts, std::vector<muint32> & _tris, std::vector<VECTOR3> & _normals);

		int booleanMesh(BOTYPE type, std::vector<VECTOR3> & verts1, std::vector<muint32> & tris1, std::vector<VECTOR3> & normals1, MATRIX44 & mat1,
						std::vector<VECTOR3> & verts2, std::vector<muint32> & tris2, std::vector<VECTOR3> & normals2, MATRIX44 & mat2, 
						bool merge_point = false, float tolerance = 1e-6, meshLibprogUpdatefunc update = NULL, void * data = NULL);
	};
};