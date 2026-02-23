#pragma once
#include "mip/core.h"
#include "Math.h"

namespace mip
{
	class VECTOR3;

	class MIP_LIB QUATERNION
	{
	public:
		float x;
		float y;
		float z;
		float w;
	public:
		QUATERNION();
#ifdef USE_DX9
		QUATERNION(const D3DXQUATERNION& q);
#endif
		QUATERNION(const QUATERNION& q);
		QUATERNION(const MATRIX44 & m);
		QUATERNION(float fx, float fy, float fz, float fw);
		QUATERNION(const VECTOR3& pos, float fw);
		QUATERNION(float pitch, float yaw, float roll);

		QUATERNION& operator *=  (const QUATERNION& q);
		QUATERNION  operator *  (const QUATERNION& q) const;
		QUATERNION  operator +  (const QUATERNION& q) const;
		QUATERNION&  operator +=  (const QUATERNION& q);
		QUATERNION&  operator -=  (const QUATERNION& q);
		QUATERNION  operator -  (const QUATERNION& q) const;


		QUATERNION  operator *  (float f) const;
		QUATERNION  operator /  (float f) const;
		float       operator |  (const QUATERNION& Q) const;

		QUATERNION& operator *=  (float scale);

		VECTOR3  operator *  (const VECTOR3& v) const;
		VECTOR4  operator *  (const VECTOR4& v) const;


		QUATERNION& normalize();

		void set(float fx, float fy, float fz, float fw);
		void set(const QUATERNION& qut);
		void set(const MATRIX44& mat);
		void setRotationAxis(const VECTOR3& axis, float angleRad);

		void setYawPitchRoll(float Yaw, float Pitch, float Roll);
		void getYawPitchRoll(float & Yaw, float & Pitch, float & Roll) const;

		VECTOR3 getRotationAxis() const;
		void    getAxisAngle(VECTOR3 & Axis, float & angle);
		VECTOR3 rotateVector(VECTOR3 v) const;

		VECTOR3 unRotateVector(VECTOR3 v) const;
		VECTOR3 getAxisX() const;
		VECTOR3 getAxisY() const;
		VECTOR3 getAxisZ() const;
		MATRIX44 QUATERNION::getMatrix(const VECTOR3 & trans = VECTOR3(0,0,0)) const;
		bool    equals(const QUATERNION& qut, float tolerance) const;

		void slerp(const QUATERNION& q2, float r);	// r - 0.0f : current, r - 1.0f : q2
		void lerp(const QUATERNION& q2, float r);	// r - 0.0f : current, r - 1.0f : q2
		QUATERNION & inverse();

		float length() const;
		float lengthSquared() const;
		bool isNomalized() const;
#ifdef USE_DX9
		D3DXQUATERNION dx() const {
			return D3DXQUATERNION(x,y,z,w);
		}
#endif
	public:
		static const QUATERNION Identity;
	};

	MIP_INLINE
		QUATERNION::QUATERNION()
	{
		x = y = z = w = 0.0f;
	}

	MIP_INLINE
		QUATERNION::QUATERNION(const QUATERNION& q)
	{
		x = q.x; y = q.y; z = q.z; w = q.w;
	}
#ifdef USE_DX9
	MIP_INLINE
		QUATERNION::QUATERNION(const D3DXQUATERNION& q)
	{
		x = q.x; y = q.y; z = q.z; w = q.w;
	}
#endif
	MIP_INLINE
		QUATERNION::QUATERNION(float fx, float fy, float fz, float fw)
	{
		x = fx; y = fy; z = fz; w = fw;
	}

	MIP_INLINE
		QUATERNION::QUATERNION(const MATRIX44 & m)
	{
		set(m);
	}

	MIP_INLINE
		QUATERNION::QUATERNION(const VECTOR3& pos, float fw)
	{
		x = pos.x; y = pos.y; z = pos.z; w = fw;
	}

	MIP_INLINE
		QUATERNION::QUATERNION(float pitch, float yaw, float roll)
	{
		setYawPitchRoll(yaw, pitch, roll);
	}

	MIP_INLINE
		QUATERNION& QUATERNION::operator *= (const QUATERNION& q)
	{
		/**
		* Now this uses VectorQuaternionMultiply that is optimized per platform.
		*/
		//VectorRegister A = VectorLoadAligned(this);
		//VectorRegister B = VectorLoadAligned(&q);
		//VectorRegister Result;
		//sse::VectorQuaternionMultiply(&Result, &A, &B);
		//VectorStoreAligned(Result, this);
		*this = *this  * q;
		return *this;
	}

	MIP_INLINE
		QUATERNION QUATERNION::operator + (const QUATERNION& q) const
	{
		return QUATERNION(x + q.x, y + q.y, z + q.z, w + q.w);
	}

	MIP_INLINE
		QUATERNION QUATERNION::operator *(float f) const
	{
		return QUATERNION(x * f, y * f, z * f, w * f);
	}

	MIP_INLINE
		QUATERNION& QUATERNION::operator *= (float f)
	{
		this->x *= f; this->y *= f; this->z *= f; this->w *= f;
		return *this;
	}

	MIP_INLINE QUATERNION&  QUATERNION::operator +=  (const QUATERNION& q)
	{
		this->x += q.x;
		this->y += q.y;
		this->z += q.z;
		this->w += q.w;

		return *this;
	}

	MIP_INLINE QUATERNION&  QUATERNION::operator-=  (const QUATERNION& q)
	{
		this->x -= q.x;
		this->y -= q.y;
		this->z -= q.z;
		this->w -= q.w;

		return *this;
	}

	MIP_INLINE QUATERNION  QUATERNION::operator-  (const QUATERNION& q) const
	{
		return QUATERNION(x - q.x, y - q.y, z - q.z, w - q.w);
	}

	MIP_INLINE
		QUATERNION QUATERNION::operator /(float f) const
	{
		float fInv = 1.0f / f;
		return QUATERNION(x * fInv, y * fInv, z * fInv, w * fInv);
	}

	MIP_INLINE 
		float QUATERNION::operator | (const QUATERNION& Q) const
	{
		return x * Q.x + y * Q.y + z * Q.z + w * Q.w;
	}

	MIP_INLINE
		VECTOR3  QUATERNION::operator *  (const VECTOR3& v) const
	{
		return rotateVector(v);
	}

	MIP_INLINE
		VECTOR4  QUATERNION::operator *  (const VECTOR4& v) const
	{
		return VECTOR4(rotateVector(v), 1.0f);
	}

	MIP_INLINE
		QUATERNION& QUATERNION::normalize()
	{
		const float SquareSum = x * x + y * y + z * z + w * w;

		if (SquareSum > MIP_EPSILON)
		{
			const float Scale = 1.0f/sqrt(SquareSum);

			x *= Scale;
			y *= Scale;
			z *= Scale;
			w *= Scale;
		}
		else
		{
			*this = QUATERNION::Identity;
		}

		return *this;
	}

	MIP_INLINE
		void QUATERNION::set(float fx, float fy, float fz, float fw)
	{
		x = fx; y = fy; z = fz; w = fw;
	}

	MIP_INLINE
		void QUATERNION::set(const QUATERNION& qut)
	{
		x = qut.x; y = qut.y; z = qut.z; w = qut.w;
	}

	MIP_INLINE
		void    QUATERNION::getAxisAngle(VECTOR3 & outAxis, float & outAngleRad)
	{
		outAngleRad = 2.f * acos(w);
		outAxis = getRotationAxis();
	}

	MIP_INLINE
		VECTOR3 QUATERNION::rotateVector(VECTOR3 v) const
	{
		// (q.W*q.W-qv.qv)v + 2(qv.v)qv + 2 q.W (qv x v)

		const VECTOR3 qv(x, y, z);
		VECTOR3 vOut = (2.f * w) * (qv ^ v);
		vOut += ((w * w) - (qv | qv)) * v;
		vOut += (2.f * (qv | v)) * qv;

		return vOut;
	}

	MIP_INLINE
		VECTOR3 QUATERNION::unRotateVector(VECTOR3 v) const
	{
		QUATERNION q = *this;
		return q.inverse().rotateVector(v);
	}

	MIP_INLINE VECTOR3 QUATERNION::getAxisX() const
	{
		return rotateVector(VECTOR3(1.f, 0.f, 0.f));
	}


	MIP_INLINE VECTOR3 QUATERNION::getAxisY() const
	{
		return rotateVector(VECTOR3(0.f, 1.f, 0.f));
	}


	MIP_INLINE VECTOR3 QUATERNION::getAxisZ() const
	{
		return rotateVector(VECTOR3(0.f, 0.f, 1.f));
	}

	MIP_INLINE MATRIX44 QUATERNION::getMatrix(const VECTOR3 & trans) const
	{
		const float x2 = x + x;  const float y2 = y + y;  const float z2 = z + z;
		const float xx = x * x2;   const float xy = x * y2;   const float xz = x * z2;
		const float yy = y * y2;   const float yz = y * z2;   const float zz = z * z2;
		const float wx = w * x2;   const float wy = w * y2;   const float wz = w * z2;

		MATRIX44 Mat;
		Mat.m[0][0] = 1.0f - (yy + zz);	Mat.m[1][0] = xy - wz;				Mat.m[2][0] = xz + wy;			Mat.m[3][0] = trans.x;
		Mat.m[0][1] = xy + wz;			Mat.m[1][1] = 1.0f - (xx + zz);		Mat.m[2][1] = yz - wx;			Mat.m[3][1] = trans.y;
		Mat.m[0][2] = xz - wy;			Mat.m[1][2] = yz + wx;				Mat.m[2][2] = 1.0f - (xx + yy);	Mat.m[3][2] = trans.z;
		Mat.m[0][3] = 0.0f;				Mat.m[1][3] = 0.0f;					Mat.m[2][3] = 0.0f;				Mat.m[3][3] = 1.0f;
		return Mat;
	}

	MIP_INLINE
		QUATERNION& QUATERNION::inverse()
	{
		*this = QUATERNION(-x, -y, -z, w);
		return *this;
	}

	MIP_INLINE
	bool QUATERNION::isNomalized() const
	{
		return (abs(1.f - lengthSquared()) < MIP_EPSILON);
	}

	MIP_INLINE
		float QUATERNION::length() const
	{
		return sqrt(x * x + y * y + z * z + w * w);
	}

	MIP_INLINE
		float QUATERNION::lengthSquared() const
	{
		return (x * x + y * y + z * z + w * w);
	}
};