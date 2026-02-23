#pragma once
#include "meshlib.h"

namespace mip
{
	namespace mesh
	{
		int splitPlaneMeshL(const VECTOR2 & screen_pos1, const VECTOR2 & screen_pos2, const VECTOR2 & screen_size, MATRIX44 & mat_wvp, bool hole_close, bool point_merge, 
									std::vector<VECTOR3> & _verts, std::vector<muint32> & _tris, std::vector<VECTOR3> & _normals, 
									std::vector<VECTOR3> * _verts2 = NULL, std::vector<muint32> * _tris2 = NULL, std::vector<VECTOR3> * _normals2 = NULL,
									meshLibprogUpdatefunc update = NULL, void * data = NULL);
		int splitPlaneMeshL(const VECTOR3 & local_p0, const VECTOR3 & local_p1, const VECTOR3 & local_p2, bool hole_close, bool point_merge, 
									std::vector<VECTOR3> & _verts, std::vector<muint32> & _tris, std::vector<VECTOR3> & _normals, 
									std::vector<VECTOR3> * _verts2 = NULL, std::vector<muint32> * _tris2 = NULL, std::vector<VECTOR3> * _normals2 = NULL,
									meshLibprogUpdatefunc update = NULL, void * data = NULL);
		int splitMeshFromPlaneListL(const std::vector<VECTOR2> & screen_lines, const VECTOR2 & screen_size, const MATRIX44 & wvp,
									std::vector<VECTOR3> & verts, std::vector<muint32> & tris, std::vector<VECTOR3> & normals, 
									bool hole_close, bool merge_point = false,
									meshLibprogUpdatefunc update = NULL, void * data = NULL);

		int splitMeshFromPlaneListInnerL(const std::vector<VECTOR2> & screen_lines, const VECTOR2 & screen_size, const MATRIX44 & wvp,
			std::vector<VECTOR3> & verts, std::vector<muint32> & tris, std::vector<VECTOR3> & normals,
			bool merge_point = false, bool view_depend = true,
			std::vector<VECTOR3> * _verts2 = NULL, std::vector<muint32> * _tris2 = NULL, std::vector<VECTOR3> * _normals2 = NULL,
			meshLibprogUpdatefunc update = NULL, void * data = NULL);

		int translateMesh(const VECTOR3 & pos, std::vector<VECTOR3> & _verts);
		int rotateMesh(const VECTOR3 & axis, float radian, std::vector<VECTOR3> & _verts);
		int scaleMesh(const VECTOR3 & scale, std::vector<VECTOR3> & _verts);
		int transformMesh(const MATRIX44 & mat, std::vector<VECTOR3> & _verts);
		int replaceCentor(std::vector<VECTOR3> & _verts);

		int distanceMeshL(const VECTOR2 & screen, const VECTOR2 & screen_size, const MATRIX44 & wvp, const VECTOR3 & local_p0, const VECTOR3 & local_p1, const std::vector<VECTOR3> & verts, const std::vector<muint32> & tris, float * _length, std::vector<VECTOR3> * _lineList = NULL);
		int distanceMeshL(const VECTOR3 & local_p0, const VECTOR3 & local_p1, const VECTOR3 & local_p2, const std::vector<VECTOR3> & verts, const std::vector<muint32> & tris, float * _length, std::vector<VECTOR3> * _lineList = NULL);

		int getMinMax(const std::vector<VECTOR3> & verts, VECTOR3 & _min, VECTOR3 & _max);
		int pickMesh(const VECTOR2 & screen, const VECTOR2 & screen_size, MATRIX44 & mat, const std::vector<VECTOR3> & verts, const std::vector<muint32> & tris, float maxdistance, VECTOR3 & _result);
		int pickMesh(const VECTOR3 & org, const VECTOR3 & dir, const std::vector<VECTOR3> & verts, const std::vector<muint32> & tris, float maxdistance, VECTOR3 & _result);
	};
};