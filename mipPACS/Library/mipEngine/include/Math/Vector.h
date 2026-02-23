#pragma once
#include "mip/core.h"

namespace mip
{
	class VECTOR4;
	class MATRIX33;
	class MATRIX44;
	class QUATERNION;

	class MIP_LIB VECTOR2
	{
	public:
		float x;
		float y;
	public:
		VECTOR2();
		VECTOR2(float fx, float fy);
#ifdef USE_DX9
		VECTOR2(const D3DVECTOR& vec);
#endif
		void set(float fx, float fy);
		void set(const VECTOR2& v);
#ifdef USE_DX9
		void set(const D3DVECTOR& v);
#endif

		VECTOR2 operator + () const;
		VECTOR2 operator - () const;

		VECTOR2 operator + (float fValue) const;
		VECTOR2 operator - (float fValue) const;
		VECTOR2 operator * (float fValue) const;
		VECTOR2 operator / (float fValue) const;

		VECTOR2& operator += (const VECTOR2& vValue);
		VECTOR2& operator -= (const VECTOR2& vValue);
		VECTOR2& operator *= (float fValue);
		VECTOR2& operator /= (float fValue);

		void operator = (const VECTOR2& vValue);
		VECTOR2 operator + (const VECTOR2& vValue) const;
		VECTOR2 operator - (const VECTOR2& vValue) const;
		float   operator ^ (const VECTOR2& vValue) const;
		float   operator | (const VECTOR2& vValue) const;

		bool operator != (const VECTOR2& vValue) const;
		bool operator == (const VECTOR2& vValue) const;

		float& operator [] (const unsigned long n);

		float length() const;
		float lengthSq() const;
		VECTOR2& normalize();
#ifdef USE_DX9
		D3DXVECTOR2 dx() const { return D3DXVECTOR2(x, y); };
#endif
	};

	class MIP_LIB VECTOR3
	{
	public:
		float x;
		float y;
		float z;

	public:
		VECTOR3();
		VECTOR3(float fx, float fy, float fz);
#ifdef USE_DX9
		VECTOR3(const D3DVECTOR& vec);
#endif
		VECTOR3(const VECTOR3& vec);
		VECTOR3(const VECTOR4& vec);

		void set(float fx, float fy, float fz);
		void set(const VECTOR3& v);
		void set(const VECTOR4& v);
#ifdef USE_DX9
		void set(const D3DVECTOR& v);
#endif
		VECTOR3 operator + () const;
		VECTOR3 operator - () const;

		VECTOR3 operator + (float fValue) const;
		VECTOR3 operator - (float fValue) const;
		VECTOR3 operator * (float fValue) const;
		VECTOR3 operator / (float fValue) const;

		VECTOR3& operator += (const VECTOR3& vValue);
		VECTOR3& operator -= (const VECTOR3& vValue);
		VECTOR3& operator *= (float fValue);
		VECTOR3& operator /= (float fValue);

		void operator = (const VECTOR3& vValue);
		VECTOR3 operator * (const VECTOR3& vValue) const;
		VECTOR3 operator / (const VECTOR3& vValue) const;
		VECTOR3 operator ^ (const VECTOR3& vValue) const;
		float   operator | (const VECTOR3& vValue) const;
		VECTOR3 operator + (const VECTOR3& vValue) const;
		VECTOR3 operator - (const VECTOR3& vValue) const;

		bool operator < (const VECTOR3& vValue) const;
		bool operator > (const VECTOR3& vValue) const;
		bool operator != (const VECTOR3& vValue) const;
		bool operator == (const VECTOR3& vValue) const;

		float& operator [] (const unsigned long n);

		float		length() const;
		float		lengthSq() const;
		float		dot(const VECTOR3& vValue) const;
		VECTOR3		cross(const VECTOR3& v2);
		void		getYawPitchRoll(float & Yaw, float & Pitch, float & Roll) const;
#ifdef USE_DX9
		VECTOR3		transform(const D3DXMATRIX& mat);
#endif
		VECTOR3&	transform(const MATRIX44& mat, bool rotate_only = false);
		VECTOR3&	transformAbsRot(const MATRIX44& mat);
#ifdef USE_DX9
		VECTOR3&	transform(const D3DXQUATERNION& q);
		VECTOR3&	transform(const D3DXQUATERNION& q, const D3DXVECTOR3& v);
#endif
		VECTOR3&	transform(const QUATERNION& q);
		VECTOR3&	transform(const QUATERNION& q, const VECTOR3& v);
		VECTOR3&	transform(const MATRIX33& mat, const VECTOR3& v);
		VECTOR3&	transform(const MATRIX33& mat);

		VECTOR3		toTransform(const MATRIX44& mat, bool rotate_only = false) const;
		VECTOR3		toTransform(const QUATERNION& q, const VECTOR3& v) const;

		VECTOR3&	normalize();
		VECTOR3		getNormalizeSafe(float tolerance = MIP_SMALL_NUMBER) const;
		VECTOR3		reciprocal() const;

		bool		isNearlyZero() const;
#ifdef USE_DX9
		D3DXVECTOR3 dx() const { return D3DXVECTOR3(x, y, z); };
#endif
	public:
		static const VECTOR3 One;
		static const VECTOR3 Zero;
		static const VECTOR3 Max;
		static const VECTOR3 Min;
	};

	class MIP_LIB VECTOR4
	{
	public:
		float x;
		float y;
		float z;
		float w;

	public:
		VECTOR4();
		VECTOR4(float fx, float fy, float fz, float fw);
#ifdef USE_DX9
		VECTOR4(const D3DXVECTOR4& vec);
#endif
		VECTOR4(const VECTOR4& vec);
		VECTOR4(const VECTOR3& vec, float fw);

		void set(float fx, float fy, float fz, float fw);
		void set(const VECTOR4& v);
#ifdef USE_DX9
		void set(const D3DXVECTOR4& v);
#endif

		VECTOR4 operator + () const;
		VECTOR4 operator - () const;

		VECTOR4 operator + (float fValue) const;
		VECTOR4 operator - (float fValue) const;
		VECTOR4 operator * (float fValue) const;
		VECTOR4 operator / (float fValue) const;

		VECTOR4 operator * (const VECTOR3& fValue) const;

		VECTOR4& operator += (const VECTOR4& vValue);
		VECTOR4& operator -= (const VECTOR4& vValue);
		VECTOR4& operator *= (float fValue);
		VECTOR4& operator /= (float fValue);

		void operator = (const VECTOR4& vValue);
		VECTOR4 operator + (const VECTOR4& vValue) const;
		VECTOR4 operator - (const VECTOR4& vValue) const;

		bool operator != (const VECTOR4& vValue) const;
		bool operator == (const VECTOR4& vValue) const;

		float& operator [] (const unsigned long n);

		float		length() const;
		float		lengthSq() const;	
		float		dot(const VECTOR4& vValue) const;
		VECTOR4&	cross(const VECTOR4& v1, const VECTOR4& v2);
#ifdef USE_DX9
		VECTOR4&	transform(const D3DXMATRIX& mat);
#endif
		VECTOR4&	transform(const MATRIX44& mat);
#ifdef USE_DX9
		VECTOR4&	transform(const D3DXQUATERNION& q);
		VECTOR4&	transform(const D3DXQUATERNION& q, const D3DXVECTOR4& v);
#endif
		VECTOR4&	transform(const QUATERNION& q);
		VECTOR4&	transform(const QUATERNION& q, const VECTOR4& v);

		VECTOR4&	normalize();
#ifdef USE_DX9
		D3DXVECTOR4 dx() const { return D3DXVECTOR4(x, y, z, w); };
#endif
	};






	MIP_INLINE
		VECTOR2::VECTOR2()
	{
		x = y = 0.0f;
	}

	MIP_INLINE
		VECTOR2::VECTOR2(float fx, float fy)
	{
		x = fx;
		y = fy;
	}
#ifdef USE_DX9
	MIP_INLINE
		VECTOR2::VECTOR2(const D3DVECTOR& vec)
	{
		set(vec);
	}
#endif
	MIP_INLINE
		void VECTOR2::set(float fx, float fy)
	{
		x = fx;
		y = fy;
	}

	MIP_INLINE
		void VECTOR2::set(const VECTOR2& v)
	{
		x = v.x;
		y = v.y;
	}
#ifdef USE_DX9
	MIP_INLINE
		void VECTOR2::set(const D3DVECTOR& v)
	{
		x = v.x;
		y = v.y;
	}
#endif
	MIP_INLINE
		VECTOR2 VECTOR2::operator + () const
	{
		return *this;
	}

	MIP_INLINE
		VECTOR2 VECTOR2::operator - () const
	{
		return VECTOR2(-x, -y);
	}

	MIP_INLINE
		VECTOR2 VECTOR2::operator + (float fValue) const
	{
		return VECTOR2(fValue + x, fValue + y);
	}

	MIP_INLINE
		VECTOR2 VECTOR2::operator - (float fValue) const
	{
		return VECTOR2(fValue - x, fValue - y);
	}

	MIP_INLINE
		VECTOR2 VECTOR2::operator * (float fValue) const
	{
		return VECTOR2(fValue * x, fValue * y);
	}

	MIP_INLINE
		VECTOR2 VECTOR2::operator / (float fValue) const
	{
		float f = 1.0f / fValue;
		return VECTOR2(x * f, y * f);
	}

	MIP_INLINE
		bool VECTOR2::operator == (const VECTOR2& vValue) const
	{
		return (fabs(vValue.x - x) < MIP_EPSILON &&
			fabs(vValue.y - y) < MIP_EPSILON);
	}

	MIP_INLINE
		bool VECTOR2::operator != (const VECTOR2& vValue) const
	{
		return (fabs(vValue.x - x) > MIP_EPSILON ||
			fabs(vValue.y - y) > MIP_EPSILON);
	}

	MIP_INLINE
		VECTOR2& VECTOR2::operator += (const VECTOR2& vValue)
	{
		x += vValue.x;
		y += vValue.y;
		return *this;
	}

	MIP_INLINE
		VECTOR2& VECTOR2::operator -= (const VECTOR2& vValue)
	{
		x -= vValue.x;
		y -= vValue.y;
		return *this;
	}

	MIP_INLINE
		VECTOR2& VECTOR2::operator *= (float fValue)
	{
		x *= fValue;
		y *= fValue;
		return *this;
	}

	MIP_INLINE
		VECTOR2& VECTOR2::operator /= (float fValue)
	{
		float f = 1.0f / fValue;
		x *= f;
		y *= f;
		return *this;
	}

	MIP_INLINE
		void VECTOR2::operator = (const VECTOR2& vValue)
	{
		x = vValue.x;
		y = vValue.y;
	}

	MIP_INLINE
		VECTOR2 VECTOR2::operator + (const VECTOR2& vValue) const
	{
		return VECTOR2(x + vValue.x, y + vValue.y);
	}

	MIP_INLINE
		VECTOR2 VECTOR2::operator - (const VECTOR2& vValue) const
	{
		return VECTOR2(x - vValue.x, y - vValue.y);
	}

	MIP_INLINE
		float VECTOR2::operator | (const VECTOR2& vValue) const
	{
		return x * vValue.x + y * vValue.y;
	}

	MIP_INLINE
		float VECTOR2::operator ^ (const VECTOR2& vValue) const
	{
		return x * vValue.y - y * vValue.x;
	}

	MIP_INLINE
		float& VECTOR2::operator [] (const unsigned long n)
	{
		return *((float*)&x + n);
	}

	MIP_INLINE
		float VECTOR2::length() const
	{
		return sqrtf(x*x + y*y);
	}

	MIP_INLINE
		float VECTOR2::lengthSq() const
	{
		return x*x + y*y;
	}

	MIP_INLINE
		VECTOR2& VECTOR2::normalize()
	{
		float fLength = length();
		if (fLength > 0.0f)
			*this /= fLength;
		return *this;
	}


	MIP_INLINE
		VECTOR3::VECTOR3()
	{
		x = y = z = 0.0f;
	}

	MIP_INLINE
		VECTOR3::VECTOR3(float fx, float fy, float fz)
	{
		x = fx;
		y = fy;
		z = fz;
	}
#ifdef USE_DX9
	MIP_INLINE
		VECTOR3::VECTOR3(const D3DVECTOR& vec)
	{
		set(vec);
	}
#endif
	MIP_INLINE
		VECTOR3::VECTOR3(const VECTOR3& vec)
	{
		set(vec);
	}

	MIP_INLINE
		void VECTOR3::set(float fx, float fy, float fz)
	{
		x = fx;
		y = fy;
		z = fz;
	}

	MIP_INLINE
		void VECTOR3::set(const VECTOR3& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
	}
#ifdef USE_DX9
	MIP_INLINE
		void VECTOR3::set(const D3DVECTOR& v)
	{
		x = v.x; y = v.y; z = v.z;
	}
#endif
	MIP_INLINE
		VECTOR3 VECTOR3::operator + () const
	{
		return *this;
	}

	MIP_INLINE
		VECTOR3 VECTOR3::operator - () const
	{
		return VECTOR3(-x, -y, -z);
	}

	MIP_INLINE
		VECTOR3 VECTOR3::operator + (float fValue) const
	{
		return VECTOR3(fValue + x, fValue + y, fValue + z);
	}

	MIP_INLINE
		VECTOR3 VECTOR3::operator - (float fValue) const
	{
		return VECTOR3(fValue - x, fValue - y, fValue - z);
	}

	MIP_INLINE
		VECTOR3 VECTOR3::operator * (float fValue) const
	{
		return VECTOR3(fValue * x, fValue * y, fValue * z);
	}

	MIP_INLINE
		VECTOR3 VECTOR3::operator / (float fValue) const
	{
		float f = 1.0f / fValue;
		return VECTOR3(x * f, y * f, z * f);
	}

	MIP_INLINE
		bool VECTOR3::operator == (const VECTOR3& vValue) const
	{
		return (fabs(vValue.x - x) < MIP_EPSILON2 &&
			fabs(vValue.y - y) < MIP_EPSILON2 &&
			fabs(vValue.z - z) < MIP_EPSILON2);
	}

	MIP_INLINE
		bool VECTOR3::operator != (const VECTOR3& vValue) const
	{
		return (fabs(vValue.x - x) > MIP_EPSILON2 ||
			fabs(vValue.y - y) > MIP_EPSILON2 ||
			fabs(vValue.z - z) > MIP_EPSILON2);
	}

	MIP_INLINE
		VECTOR3& VECTOR3::operator += (const VECTOR3& vValue)
	{
		x += vValue.x;
		y += vValue.y;
		z += vValue.z;
		return *this;
	}

	MIP_INLINE
		VECTOR3& VECTOR3::operator -= (const VECTOR3& vValue)
	{
		x -= vValue.x;
		y -= vValue.y;
		z -= vValue.z;
		return *this;
	}

	MIP_INLINE
		VECTOR3& VECTOR3::operator *= (float fValue)
	{
		x *= fValue;
		y *= fValue;
		z *= fValue;
		return *this;
	}

	MIP_INLINE
		VECTOR3& VECTOR3::operator /= (float fValue)
	{
		float f = 1.0f / fValue;
		x *= f;
		y *= f;
		z *= f;
		return *this;
	}

	MIP_INLINE
		void VECTOR3::operator = (const VECTOR3& vValue)
	{
		x = vValue.x;
		y = vValue.y;
		z = vValue.z;
	}

	MIP_INLINE
		VECTOR3 VECTOR3::operator * (const VECTOR3& vValue) const
	{
		return VECTOR3(x * vValue.x, y * vValue.y, z * vValue.z);
	}

	MIP_INLINE
		VECTOR3 VECTOR3::operator / (const VECTOR3& vValue) const
	{
		return VECTOR3(x / vValue.x, y / vValue.y, z / vValue.z);
	}

	MIP_INLINE
		VECTOR3 VECTOR3::operator ^ (const VECTOR3& vValue) const
	{
		return VECTOR3(y * vValue.z - z * vValue.y, z * vValue.x - x * vValue.z, x * vValue.y - y * vValue.x);
	}

	MIP_INLINE
		float VECTOR3::operator | (const VECTOR3& vValue) const
	{
		return x * vValue.x + y * vValue.y + z * vValue.z;
	}

	MIP_INLINE
		VECTOR3 VECTOR3::operator + (const VECTOR3& vValue) const
	{
		return VECTOR3(x + vValue.x, y + vValue.y, z + vValue.z);
	}

	MIP_INLINE
		VECTOR3 VECTOR3::operator - (const VECTOR3& vValue) const
	{
		return VECTOR3(x - vValue.x, y - vValue.y, z - vValue.z);
	}

	MIP_INLINE
		bool VECTOR3::operator < (const VECTOR3& vValue) const
	{
		if (x < vValue.x) return true;
		if (x > vValue.x) return false;
		if (y < vValue.y) return true;
		if (y > vValue.y) return false;
		if (z < vValue.z) return true;
		if (z > vValue.z) return false;
		return false;
	}

	MIP_INLINE
		bool VECTOR3::operator > (const VECTOR3& vValue) const
	{
		if (x > vValue.x) return true;
		if (x < vValue.x) return false;
		if (y > vValue.y) return true;
		if (y < vValue.y) return false;
		if (z > vValue.z) return true;
		if (z < vValue.z) return false;
		return false;
	}
	
	MIP_INLINE
		float VECTOR3::length() const
	{
		return sqrtf(x*x + y*y + z*z);
	}

	MIP_INLINE
		float VECTOR3::lengthSq() const
	{
		return x*x + y*y + z*z;
	}

	MIP_INLINE
		VECTOR3& VECTOR3::normalize()
	{
		float fLength = length();
		if (fLength > 0.0f)
			*this /= fLength;
		return *this;
	}

	MIP_INLINE
		VECTOR3 VECTOR3::getNormalizeSafe(float tolerance) const
	{
		float SquareSum = lengthSq();

		if (SquareSum == 1.f)
		{
			return *this;
		}
		else if (SquareSum < tolerance)
		{
			return VECTOR3(0,0,0);
		}
		float scale = 1.0f/sqrtf(SquareSum);
		return VECTOR3(x*scale, y*scale, z*scale);
	}

	MIP_INLINE
		float VECTOR3::dot(const VECTOR3& vValue) const
	{
		return x * vValue.x + y * vValue.y + z * vValue.z;
	}

	MIP_INLINE
		VECTOR3 VECTOR3::cross(const VECTOR3& v2)
	{
		return VECTOR3(y * v2.z - z * v2.y, z * v2.x - x * v2.z, x * v2.y - y * v2.x);
	}

	MIP_INLINE
		float& VECTOR3::operator [] (const unsigned long n)
	{
		return *((float*)&x + n);
	}

	MIP_INLINE VECTOR3 operator*(float Scale, const VECTOR3 & V)
	{
		return V.operator*(Scale);
	}

	MIP_INLINE bool	VECTOR3::isNearlyZero() const
	{
		return abs(x) < MIP_EPSILON && abs(y) < MIP_EPSILON && abs(z) < MIP_EPSILON;
	}
	
	MIP_INLINE VECTOR3 VECTOR3::reciprocal() const
	{
		VECTOR3 RecVector;
		if (x != 0.f)
		{
			RecVector.x = 1.f / x;
		}
		else
		{
			RecVector.x = MIP_BIG_NUMBER;
		}
		if (y != 0.f)
		{
			RecVector.y = 1.f / y;
		}
		else
		{
			RecVector.y = MIP_BIG_NUMBER;
		}
		if (z != 0.f)
		{
			RecVector.z = 1.f / z;
		}
		else
		{
			RecVector.z = MIP_BIG_NUMBER;
		}

		return RecVector;
	}



	MIP_INLINE
		VECTOR4::VECTOR4()
	{
		x = y = z = w = 0.0f;
	}

	MIP_INLINE
		VECTOR4::VECTOR4(float fx, float fy, float fz, float fw)
	{
		x = fx;
		y = fy;
		z = fz;
		w = fw;
	}

	MIP_INLINE
		VECTOR4::VECTOR4(const VECTOR4& vec)
	{
		x = vec.x;
		y = vec.y;
		z = vec.z;
		w = vec.w;
	}

	MIP_INLINE
		VECTOR4::VECTOR4(const VECTOR3& vec, float fw)
	{
		x = vec.x;
		y = vec.y;
		z = vec.z;
		w = fw;
	}
#ifdef USE_DX9	
	MIP_INLINE
		VECTOR4::VECTOR4(const D3DXVECTOR4& vec)
	{
		set(vec);
	}
#endif
	MIP_INLINE
		void VECTOR4::set(float fx, float fy, float fz, float fw)
	{
		x = fx;
		y = fy;
		z = fz;
		w = fw;
	}

	MIP_INLINE
		void VECTOR4::set(const VECTOR4& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;
	}
#ifdef USE_DX9
	MIP_INLINE
		void VECTOR4::set(const D3DXVECTOR4& v)
	{
		x = v.x; y = v.y; z = v.z; w = v.w;
	}
#endif
	MIP_INLINE
		VECTOR4 VECTOR4::operator + () const
	{
		return *this;
	}

	MIP_INLINE
		VECTOR4 VECTOR4::operator - () const
	{
		return VECTOR4(-x, -y, -z, -w);
	}

	MIP_INLINE
		VECTOR4 VECTOR4::operator + (float fValue) const
	{
		return VECTOR4(fValue + x, fValue + y, fValue + z, fValue + w);
	}

	MIP_INLINE
		VECTOR4 VECTOR4::operator - (float fValue) const
	{
		return VECTOR4(fValue - x, fValue - y, fValue - z, fValue - w);
	}

	MIP_INLINE
		VECTOR4 VECTOR4::operator * (float fValue) const
	{
		return VECTOR4(fValue * x, fValue * y, fValue * z, fValue * w);
	}

	MIP_INLINE
		VECTOR4 VECTOR4::operator / (float fValue) const
	{
		float f = 1.0f / fValue;
		return VECTOR4(x * f, y * f, z * f, w * f);
	}

	MIP_INLINE
		VECTOR4 VECTOR4::operator * (const VECTOR3& v) const
	{
		return VECTOR4(v.x * x, v.y * y, v.z * z, w);
	}


	MIP_INLINE
		bool VECTOR4::operator == (const VECTOR4& vValue) const
	{
		return (fabs(vValue.x - x) < MIP_EPSILON &&
			fabs(vValue.y - y) < MIP_EPSILON &&
			fabs(vValue.z - z) < MIP_EPSILON &&
			fabs(vValue.w - w) < MIP_EPSILON);
	}

	MIP_INLINE
		bool VECTOR4::operator != (const VECTOR4& vValue) const
	{
		return (fabs(vValue.x - x) > MIP_EPSILON ||
			fabs(vValue.y - y) > MIP_EPSILON ||
			fabs(vValue.z - z) > MIP_EPSILON ||
			fabs(vValue.w - w) < MIP_EPSILON);
	}

	MIP_INLINE
		VECTOR4& VECTOR4::operator += (const VECTOR4& vValue)
	{
		x += vValue.x;
		y += vValue.y;
		z += vValue.z;
		w += vValue.w;
		return *this;
	}

	MIP_INLINE
		VECTOR4& VECTOR4::operator -= (const VECTOR4& vValue)
	{
		x -= vValue.x;
		y -= vValue.y;
		z -= vValue.z;
		w -= vValue.w;
		return *this;
	}

	MIP_INLINE
		VECTOR4& VECTOR4::operator *= (float fValue)
	{
		x *= fValue;
		y *= fValue;
		z *= fValue;
		w *= fValue;
		return *this;
	}

	MIP_INLINE
		VECTOR4& VECTOR4::operator /= (float fValue)
	{
		float f = 1.0f / fValue;
		x *= f;
		y *= f;
		z *= f;
		w *= f;
		return *this;
	}

	MIP_INLINE
		void VECTOR4::operator = (const VECTOR4& vValue)
	{
		x = vValue.x;
		y = vValue.y;
		z = vValue.z;
		w = vValue.w;
	}

	MIP_INLINE
		VECTOR4 VECTOR4::operator + (const VECTOR4& vValue) const
	{
		return VECTOR4(x + vValue.x, y + vValue.y, z + vValue.z, w + vValue.w);
	}

	MIP_INLINE
		VECTOR4 VECTOR4::operator - (const VECTOR4& vValue) const
	{
		return VECTOR4(x - vValue.x, y - vValue.y, z - vValue.z, w - vValue.w);
	}

	MIP_INLINE
		float VECTOR4::length() const
	{
		return sqrtf(x*x + y*y + z*z + w*w);
	}

	MIP_INLINE
		float VECTOR4::lengthSq() const
	{
		return x*x + y*y + z*z + w*w;
	}

	MIP_INLINE
		VECTOR4& VECTOR4::normalize()
	{
		float fLength = length();
		if (fLength > 0.0f)
			*this /= fLength;
		return *this;
	}

	MIP_INLINE
		float VECTOR4::dot(const VECTOR4& vValue) const
	{
		return x * vValue.x + y * vValue.y + z * vValue.z + w * vValue.w;
	}

	MIP_INLINE
		VECTOR4& VECTOR4::cross(const VECTOR4& v1, const VECTOR4& v2)
	{
		VECTOR4 v;
		v.x = v1.y * v2.z - v1.z * v2.y;
		v.y = v1.z * v2.x - v1.x * v2.z;
		v.z = v1.x * v2.y - v1.y * v2.x;
		x = v.x; y = v.y; z = v.z;
		w = 1.0f; //?
		return *this;
	}

	MIP_INLINE
		float& VECTOR4::operator [] (const unsigned long n)
	{
		return *((float*)&x + n);
	}
};