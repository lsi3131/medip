#pragma once
#include "mip/core.h"

namespace mip
{
	class MIP_LIB SPHERE
	{
	public:
		VECTOR3 center;
		float radius;
		SPHERE()
		{
			center = VECTOR3::Zero; radius = 0.0f;
		}
		SPHERE(VECTOR3 _center, float _radius)
		{
			center = _center; radius = _radius;
		}
		bool checkInside(const VECTOR3 & v) const
		{
			return (v - center).length() <= radius ? true : false;
		}
	};

	class MIP_LIB RAY
	{
	public:
		VECTOR3 org;
		VECTOR3 dir;

	public:

	};

	class MIP_LIB TRIALGLE
	{
	public:
		VECTOR3 v0;
		VECTOR3 v1;
		VECTOR3 v2;

	public:

	};

	class MIP_LIB EDGE
	{
	public:
		VECTOR3 v0;
		VECTOR3 v1;
	public:

	};
};