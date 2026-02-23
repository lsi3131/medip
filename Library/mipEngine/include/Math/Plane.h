#pragma once
#include "mip/core.h"

namespace mip
{
	class AABB;

	// Stores the coeffecients as Ax+By+Cz=D.
	// Note that this is different than many other Plane classes that use Ax+By+Cz+D=0.
	class MIP_LIB PLANE
	{
	public:
		float a;
		float b;
		float c;
		float d;
	public:
		PLANE();
#ifdef USE_DX9
		PLANE(const D3DXPLANE & plane);
#endif
		PLANE(float A, float B, float C, float D);
		PLANE(const VECTOR3 & normal, const VECTOR3 & point );
		PLANE(const VECTOR3 & v0, const VECTOR3 & v1, const VECTOR3 & v2);
		void fromTri(const VECTOR3 & v0, const VECTOR3 & v1, const VECTOR3 & v2);
#ifdef USE_DX9
		void fromTri(const D3DXVECTOR3 & v0, const D3DXVECTOR3 & v1, const D3DXVECTOR3 & v2);
#endif
		float dot(const VECTOR3 & point) const; // return distance
		
		VECTOR3 dir() const;
		
		float operator|(const PLANE& V) const;
		PLANE operator+(const PLANE& V) const;
		PLANE operator-(const PLANE& V) const;
		PLANE operator/(float Scale) const;
		PLANE operator*(float Scale) const;
		PLANE operator*(const PLANE& V);
		PLANE operator+=(const PLANE& V);
		PLANE operator-=(const PLANE& V);
		PLANE operator*=(float Scale);
		PLANE operator*=(const PLANE& V);
		PLANE operator/=(float V);

		PLANE& normalize();
		PLANE&	transform(const MATRIX44& mat);
#ifdef USE_DX9
		D3DXPLANE dx() {
			return D3DXPLANE(a, b, c, d);
		}
#endif
	};

	MIP_INLINE
		PLANE::PLANE()
	{
		a = b = c = d = 0;
	}

	MIP_INLINE
		PLANE::PLANE(float A, float B, float C, float D)
	{
		a = A; b = B; c = C; d = D;
	}
#ifdef USE_DX9
	MIP_INLINE
		PLANE::PLANE(const D3DXPLANE & plane)
	{
		a = plane.a; b = plane.b; c = plane.c; d = plane.d;
	}
#endif
	MIP_INLINE
		PLANE::PLANE(const VECTOR3 & normal, const VECTOR3 & point)
	{
		a = normal.x;
		b = normal.y;
		c = normal.z;
#ifdef USE_OPENGL
		d = -(normal | point);
#else
		d = normal | point;
#endif
	}

	MIP_INLINE
		PLANE::PLANE(const VECTOR3 & v0, const VECTOR3 & v1, const VECTOR3 & v2)
	{
		fromTri(v0, v1, v2);
	}

	MIP_INLINE
		VECTOR3 PLANE::dir() const
	{
		return VECTOR3(a, b, c);
	}


	MIP_INLINE
		float PLANE::dot(const VECTOR3 & point) const
	{
#ifdef USE_RIGHT_HAND
		return dir().dot(point) + d;
#else
		return dir().dot(point) - d;
#endif
	}


	MIP_INLINE float PLANE::operator|(const PLANE& V) const
	{
		return a*V.a + b*V.b + c*V.c + d*V.d;
	}


	MIP_INLINE PLANE PLANE::operator+(const PLANE& V) const
	{
		return PLANE(a + V.a, b + V.b, c + V.c, d + V.d);
	}


	MIP_INLINE PLANE PLANE::operator-(const PLANE& V) const
	{
		return PLANE(a - V.a, b - V.b, c - V.c, d - V.d);
	}


	MIP_INLINE PLANE PLANE::operator/(float Scale) const
	{
		const float RScale = 1.f / Scale;
		return PLANE(a * RScale, b * RScale, c * RScale, d * RScale);
	}


	MIP_INLINE PLANE PLANE::operator*(float Scale) const
	{
		return PLANE(a * Scale, b * Scale, c * Scale, d * Scale);
	}


	MIP_INLINE PLANE PLANE::operator*(const PLANE& V)
	{
		return PLANE(a*V.a, b*V.b, c*V.c, d*V.d);
	}


	MIP_INLINE PLANE PLANE::operator+=(const PLANE& V)
	{
		a += V.a; b += V.b; c += V.c; d += V.d;
		return *this;
	}


	MIP_INLINE PLANE PLANE::operator-=(const PLANE& V)
	{
		a -= V.a; b -= V.b; c -= V.c; d -= V.d;
		return *this;
	}


	MIP_INLINE PLANE PLANE::operator*=(float Scale)
	{
		a *= Scale; b *= Scale; c *= Scale; d *= Scale;
		return *this;
	}


	MIP_INLINE PLANE PLANE::operator*=(const PLANE& V)
	{
		a *= V.a; b *= V.b; c *= V.c; d *= V.d;
		return *this;
	}


	MIP_INLINE PLANE PLANE::operator/=(float V)
	{
		const float RV = 1.f / V;
		a *= RV; b *= RV; c *= RV; d *= RV;
		return *this;
	}

	MIP_INLINE PLANE& PLANE::normalize()
	{
		float norm = sqrt(a*a+b*b+c+c);
		if (norm)
		{
			a = a / norm;
			b = b / norm;
			c = c / norm;
			d = d / norm;
		}
		else
		{
			a = 0.0f;
			b = 0.0f;
			c = 0.0f;
			d = 0.0f;
		}
		return *this;
	}
};