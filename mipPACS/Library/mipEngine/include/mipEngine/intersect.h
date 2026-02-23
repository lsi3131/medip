#pragma once

#include "math/math.h"

namespace mip
{
	namespace geom
	{
		// ---------------------- Screen Coord Calc
		// Cross Point return ( line and line )
		bool GetCrossPoint(VECTOR2* out, const VECTOR2 & A1, const VECTOR2& A2, const VECTOR2& B1, const VECTOR2& B2, bool isOverRange = false);
		bool LineLengthToFitBox(VECTOR3* out, VECTOR3 EntryBox[4], VECTOR3 AxisLine[4], float cx, float cy, const MATRIX44 & view, const MATRIX44 & proj, const MATRIX44 * world);
		// distance return
		float GetClosestDist(const VECTOR2 & v1, const VECTOR2 & v2, const VECTOR2 & point);

		VECTOR2 GetClosestPoint(const VECTOR2 &v1, const VECTOR2 &v2, const VECTOR2 &point);
		VECTOR3 GetClosestPoint(const VECTOR3 & v1, const VECTOR3 & v2, const VECTOR3 & point);
		float GetClosestPointDist(const VECTOR2 &v1, const VECTOR2 &v2, const VECTOR2 &point);
		float GetClosestPointDist(const VECTOR3 & v1, const VECTOR3 & v2, const VECTOR3 & point);

		// Box
		bool CheckPointBox(const VECTOR3 & point, const AABB box);
		bool CheckPointBox(const VECTOR3 & point, const AARBB box);
		bool CheckTriBox(const AABB & box, const VECTOR3 & v0, const VECTOR3 & v1, const VECTOR3 & v2);
		bool Intersect_BoxBox(const AABB & b1, const AABB & b2);
		bool Intersect_BoxBox(const AARBB & b1, const AARBB & b2);
		bool Intersect_LineBox(const AABB & box, const VECTOR3 & L1, const VECTOR3 & L2);
		bool Intersect_LineBox_XY(const AABB & box, const VECTOR3 & L1, const VECTOR3 & L2);
		bool Intersect_RayOBB(VECTOR3& o, VECTOR3& dir, AABB& box);
		bool Intersect_RayOBB(VECTOR3& o, VECTOR3& dir, AABB& box, VECTOR3& intersectpos);
		bool Intersect_RayTriangle(const mip::VECTOR3 & rayOrigin, const mip::VECTOR3 & rayVector,
			const mip::VECTOR3 & vertex0, const mip::VECTOR3 & vertex1, const mip::VECTOR3 & vertex2,
			mip::VECTOR3 & outIntersectionPoint);

		// ----------------------- Ray ( Line, Segment ) Intersection
		bool Ray_Point(const VECTOR3 & org, const VECTOR3 & dir, VECTOR3 & point);
		bool Ray_Line(const VECTOR3 & org, const VECTOR3 & dir, VECTOR3 & p1, VECTOR3 & p2);
		bool Ray_BoxCheck(const VECTOR3 & org, const VECTOR3 & dir, const VECTOR3 & min, const VECTOR3 & max);
		bool Ray_BoxCheck(const VECTOR3 & org, const VECTOR3 & dir, const AABB & aabb);
		bool Ray_BoxCheck(const VECTOR3 & org, const VECTOR3 & dir, const AARBB & aarbb);
		bool Ray_TriCheck(const VECTOR3 & org, const VECTOR3 & dir, const VECTOR3 & v0, const VECTOR3 & v1, const VECTOR3 & v2, VECTOR3 * r = NULL);
		bool Ray_PlanCheck(const mip::VECTOR3 & org, const mip::VECTOR3 & dir, const mip::VECTOR3 & center, const mip::VECTOR3 & normal, mip::VECTOR3 * r);

		// r => -1 : false ,  1 ; true ( intersected, spllited ),  2 : line in plane
		int Intersect_LinePlane(const PLANE plane, const VECTOR3 & L1, const VECTOR3 & L2, VECTOR3 * r = NULL, bool * v1front = NULL); 		
		bool Intersect_LineTri(const VECTOR3 & L1, const VECTOR3 & L2, const VECTOR3 & v0, const VECTOR3 & v1, const VECTOR3 & v2, VECTOR3 * r = NULL);

		// From Unreal Math
		bool Intersect_UELinePlane(const VECTOR3 & startPoint, const VECTOR3 & endPoint, const PLANE & plane, VECTOR3 * intersect_point);
		bool Intersect_UELineTri(const VECTOR3& Start, const VECTOR3& End, const VECTOR3& A, const VECTOR3& B, const VECTOR3& C, VECTOR3 * IntersectPoint);
		VECTOR3 UEComputeBaryCentric2D(const VECTOR3& Point, const VECTOR3& A, const VECTOR3& B, const VECTOR3& C);
		float GetLinePlane(const VECTOR3 & startPoint, const VECTOR3 & endPoint, const PLANE & plane);

		// ------------------------- Mesh
		int Intersect_TriPlane(const PLANE & plan, const VECTOR3 & v0, const VECTOR3 & v1, const VECTOR3 & v2, std::vector<VECTOR3> * r = NULL);
		int Intersect_TriTri(const VECTOR3 & va, const VECTOR3 & vb, const VECTOR3 & vc, const VECTOR3 & v0, const VECTOR3 & v1, const VECTOR3 & v2, std::vector<VECTOR3> * r = NULL);

		int Split_TriPlane(const PLANE & plane, const VECTOR3 & v0, const VECTOR3 & v1, const VECTOR3 & v2, std::vector<VECTOR3> * front = NULL, std::vector<VECTOR3> * back = NULL);
		int Check_TriPlane(const PLANE & plane, const VECTOR3 & v0, const VECTOR3 & v1, const VECTOR3 & v2); // 1 : front,  0 : intersect,  -1 : back
		int Check_TriPlanes(const std::vector<PLANE> & planes, const VECTOR3 & v0, const VECTOR3 & v1, const VECTOR3 & v2, bool clipRear); // 1 : front,  0 : intersect,  -1 : back
		
		// ------------------------- Plane
		int Intersect_OBBPlane(const PLANE & plan, const AABB & box); // 1 : front,  0 : intersect,  -1 : back
		int Intersect_AABBPlane(const PLANE & plan, const AABB & box); // 1 : front,  0 : intersect,  -1 : back
		int Intersect_AARBBPlane(const PLANE & plan, const AARBB & box); // 1 : front,  0 : intersect,  -1 : back
		bool Intersect_PlanePlane(const PLANE & plan1, const PLANE & plan2, RAY * r);

		// -------------------------- Sphere
		bool Intersect_SphereSphere(const SPHERE & s1, const SPHERE & s2);
		bool Intersect_BoxSphere(const SPHERE & s, const AABB & box);
		bool CheckTriSphere(const SPHERE & s, const VECTOR3 & v0, const VECTOR3 & v1, const VECTOR3 & v2);
	};
};

//#include "mipGeometry.h"