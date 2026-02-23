#pragma once
#include "math/math.h"
//#include "Transform.h"

namespace mip
{
	namespace geom
	{
		// Point
		VECTOR3 PointPlaneProject(const VECTOR3& Point, const PLANE & Plane);
		VECTOR3 PointPlaneProject(const VECTOR3& Point, const VECTOR3& A, const VECTOR3& B, const VECTOR3& C);
		VECTOR3 MirrorByPlane(const VECTOR3 Point, const PLANE& Plane);

		//void Multiply(TRANSFORM* OutTransform, const TRANSFORM* A, const TRANSFORM* B);

		VECTOR3 Screen2Proj(float mouseX, float mouseY, float cx, float cy);
		VECTOR3 Screen2Camera(float mouseX, float mouseY, float cx, float cy, const MATRIX44 & proj);
		VECTOR3 Screen2Camera2(float mouseX, float mouseY, float cx, float cy, const MATRIX44 & proj);
		VECTOR3 Screen2World(float mouseX, float mouseY, float cx, float cy, const MATRIX44 & view, const MATRIX44 & proj, const MATRIX44 * world = NULL);
		VECTOR3 Camera2World(const VECTOR3 & v, const MATRIX44 & view, const MATRIX44 * world = NULL);
		VECTOR3 Proj2World(const VECTOR3 & v, const MATRIX44 & view_proj, const MATRIX44 * world = NULL);

		VECTOR3 WorldToScreen(const VECTOR3 & v, float cx, float cy, const MATRIX44 & view, const MATRIX44 & proj, const MATRIX44 * world = NULL);
		VECTOR3 ProjToViewPort(const VECTOR4 & v, float cx, float cy);
		RAY     ScreenToRay(const VECTOR2 & mousePoint, const float cx, const float cy, const MATRIX44 &matView, const MATRIX44 &matProj, const MATRIX44 * world);

		MATRIX44 GetCamera(const VECTOR3 & eye, const VECTOR3 & look, const VECTOR3 & up = VECTOR3(0, 1, 0));
	};
};

//#include "mipGeometry.h"