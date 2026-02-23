#pragma once
#include "mip/core.h"
#include "Math/Math.h"

namespace mip
{
	///////////////////////////////////////////////// VECTOR
	MIP_INLINE
		void PLANE::fromTri(const VECTOR3 & v0, const VECTOR3 & v1, const VECTOR3 & v2)
	{
		VECTOR3 N = ((v1 - v0) ^ (v2 - v0)).normalize();
		a = N.x;
		b = N.y;
		c = N.z;
#ifdef USE_RIGHT_HAND
		d = -math::VectorDot(N, v0);
#else
		d = math::VectorDot(N, v0);
#endif
	}
#ifdef USE_DX9
	MIP_INLINE
		void PLANE::fromTri(const D3DXVECTOR3 & v0, const D3DXVECTOR3 & v1, const D3DXVECTOR3 & v2)
	{
		D3DXVECTOR3 N;
		D3DXVec3Cross(&N, &(v1 - v0), &(v2 - v0));
		D3DXVec3Normalize(&N, &N);
		a = N.x;
		b = N.y;
		c = N.z;
		d = D3DXVec3Dot(&N, &v0);
	}
#endif

	MIP_INLINE
		VECTOR3::VECTOR3(const VECTOR4& vec)
	{
		set(vec);
	}

	MIP_INLINE
		void VECTOR3::set(const VECTOR4& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
	}
#ifdef USE_DX9
	MIP_INLINE
		VECTOR3 VECTOR3::transform(const D3DXMATRIX& mat)
	{
		VECTOR3		v;
		v.x = (mat._11 * x + mat._21 * y + mat._31 * z + mat._41);
		v.y = (mat._12 * x + mat._22 * y + mat._32 * z + mat._42);
		v.z = (mat._13 * x + mat._23 * y + mat._33 * z + mat._43);
		*this = v;
		return *this;
	}
#endif
	///////////////////////////// PLANE
	



	////////////////////////// MATRIX
	MIP_INLINE
		MATRIX44::MATRIX44(const PLANE & x, const PLANE & y, const PLANE & z, const PLANE & w)
	{
		m[0][0] = x.a; m[0][1] = x.b;  m[0][2] = x.c;  m[0][3] = x.d;
		m[1][0] = y.a; m[1][1] = y.b;  m[1][2] = y.c;  m[1][3] = y.d;
		m[2][0] = z.a; m[2][1] = z.b;  m[2][2] = z.c;  m[2][3] = z.d;
		m[3][0] = w.a; m[3][1] = w.b;  m[3][2] = w.c;  m[3][3] = w.d;
	}

	MIP_INLINE
		MATRIX44::MATRIX44(const VECTOR3 & inX, const VECTOR3 & inY, const VECTOR3 & inZ, const VECTOR3 & inW)
	{
		m[0][0] = inX.x; m[0][1] = inX.y;  m[0][2] = inX.z;  m[0][3] = 0.0f;
		m[1][0] = inY.x; m[1][1] = inY.y;  m[1][2] = inY.z;  m[1][3] = 0.0f;
		m[2][0] = inZ.x; m[2][1] = inZ.y;  m[2][2] = inZ.z;  m[2][3] = 0.0f;
		m[3][0] = inW.x; m[3][1] = inW.y;  m[3][2] = inW.z;  m[3][3] = 1.0f;
	}

	MIP_INLINE
		void MATRIX44::set(QUATERNION& q)
	{
		//setAffine(q, VECTOR3(0,0,0), 1.0f);
	}

	MIP_INLINE
		void MATRIX44::setAffine(QUATERNION& q, VECTOR3& trans, VECTOR3& pivot, float scale)
	{
#ifdef USE_DX9
		D3DXMatrixAffineTransformation((D3DXMATRIX*)this, scale, (D3DXVECTOR3*)&pivot,
			(D3DXQUATERNION*)&q, (D3DXVECTOR3*)&trans);
#endif
	}

	MIP_INLINE
		MATRIX44& MATRIX44::inverse()
	{
		MATRIX44 mat;
		if (getScaledXaxis().isNearlyZero() && getScaledYaxis().isNearlyZero() && getScaledZaxis().isNearlyZero())
		{
			mat = MATRIX44::Identity;
		}
		else
		{
			math::MatrixInverse(&mat, this);
		}

		*this = mat;
		return *this;
	}

	MIP_INLINE
		MATRIX44 MATRIX44::operator * (const MATRIX44& mat) const
	{
		MATRIX44		m;
		math::MatrixMultiply(&m, this, &mat);
		return m;
	}
#ifdef USE_DX9
	MIP_INLINE
		D3DXMATRIX MATRIX44::operator * (const D3DXMATRIX& mat) const
	{
		D3DXMATRIX		m;
		math::MatrixMultiply(&m, this, &mat);
		return m;
	}
#endif
	MIP_INLINE
		MATRIX44& MATRIX44::operator *= (const MATRIX44& mat)
	{
		math::MatrixMultiply(this, this, &mat);
		return *this;
	}

	MIP_INLINE
		MATRIX44& MATRIX44::rotationX(float fAngle)
	{
		*this = math::MatrixTRS(VECTOR3::Zero, fAngle, 0, 0, VECTOR3::One);
		return *this;
	}

	MIP_INLINE
		MATRIX44& MATRIX44::rotationY(float fAngle)
	{
		//D3DXMatrixRotationY((D3DXMATRIX*)this, fAngle);
		*this = math::MatrixTRS(VECTOR3::Zero, 0, fAngle, 0, VECTOR3::One);
		return *this;
	}

	MIP_INLINE
		MATRIX44& MATRIX44::rotationZ(float fAngle)
	{
		//D3DXMatrixRotationZ((D3DXMATRIX*)this, fAngle);
		*this = math::MatrixTRS(VECTOR3::Zero, 0, 0, fAngle, VECTOR3::One);
		return *this;
	}

	////////////////////////// QUATERNION
	MIP_INLINE
		QUATERNION QUATERNION::operator * (const QUATERNION& q) const
	{
		QUATERNION	rq;
		//sse::VectorQuaternionMultiply(&rq, this, &q);
		math::QuaternionMultiply(&rq, this, &q);
		return rq;
	}

	MIP_INLINE
		void QUATERNION::setRotationAxis(const VECTOR3& axis, float angleRad)
	{
		const float half_a = 0.5f * angleRad;
		float s, c;
		math::SinCos(&s, &c, half_a);

		x = s * axis.x;
		y = s * axis.y;
		z = s * axis.z;
		w = c;
	}

	MIP_INLINE bool QUATERNION::equals(const QUATERNION& qut, float tolerance) const
	{
		return (math::Abs(x - qut.x) < tolerance && math::Abs(y - qut.y) < tolerance && math::Abs(z - qut.z) < tolerance && math::Abs(w - qut.w) < tolerance)
			|| (math::Abs(x + qut.x) < tolerance && math::Abs(y + qut.y) < tolerance && math::Abs(z + qut.z) < tolerance && math::Abs(w + qut.w) < tolerance);
	}

	MIP_INLINE
		void QUATERNION::lerp(const QUATERNION& q2, float r)
	{
		*this = math::Lerp(*this, q2, r);
	}

	MIP_INLINE
		void QUATERNION::slerp(const QUATERNION& q2, float r)
	{
		*this = math::Slerp(*this, q2, r);
	}

	MIP_INLINE
		VECTOR3 QUATERNION::getRotationAxis() const
	{
		// Ensure we never try to sqrt a neg number
		const float S = sqrt(mip::math::Max(1.f - (w * w), 0.f));

		if (S >= MIP_EPSILON)
		{
			return VECTOR3(x / S, y / S, z / S);
		}

		return VECTOR3(1.f, 0.f, 0.f);
	}
};