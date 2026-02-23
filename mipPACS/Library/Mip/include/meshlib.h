#pragma once
#include "math/math.h"

typedef void(*meshLibprogUpdatefunc)(int value, void * data);

namespace mip
{
	typedef struct _mPLANE
	{
		_mPLANE(VECTOR3 p, VECTOR3 o)
		{
			a = p.x; b = p.y; c = p.z;
			org = o;
		}
		_mPLANE(VECTOR3 v0, VECTOR3 v1, VECTOR3 v2)
		{
			VECTOR3 N = ((v1 - v0) ^ (v2 - v0)).normalize();
			a = N.x;
			b = N.y;
			c = N.z;

			org = v0;
		}
	public:
		float a, b, c;
		VECTOR3 org;
	} mPLANE;
}