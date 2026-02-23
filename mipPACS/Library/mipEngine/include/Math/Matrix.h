#pragma once
#include "mip/core.h"

namespace mip
{
	class PLANE;

	class MIP_LIB MATRIX33
	{
	public:
		union {
			struct {
				float        _11, _12, _13;
				float        _21, _22, _23;
				float        _31, _32, _33;
			};
			float m[3][3];
		};
	public:
		MATRIX33();
		MATRIX33(const MATRIX33& mat);
		MATRIX33(const VECTOR3& x, const VECTOR3& y, const VECTOR3& z);

		MATRIX33& identity();
		MATRIX33& inverse();
		MATRIX33& transpose();

		void	 set(QUATERNION& q);
		void	 set(float Yaw, float Pitch, float Roll);

		MATRIX33& rotationX(float fAngle);
		MATRIX33& rotationY(float fAngle);
		MATRIX33& rotationZ(float fAngle);

		MATRIX33& operator *= (const MATRIX33& mat);
		MATRIX33 operator * (const MATRIX33& mat) const;
#ifdef USE_DX9
		D3DXMATRIX operator * (const D3DXMATRIX& mat) const;
#endif
		void operator += (const MATRIX33 &mat);
		void operator -= (const MATRIX33 &mat);
		MATRIX33 operator + (const MATRIX33 &mat) const;
		MATRIX33 operator - (const MATRIX33 &mat) const;
		void operator *= (float f);
		MATRIX33&	operator = (const MATRIX33& mat);
		MATRIX33&	operator = (const MATRIX44& mat);
#ifdef USE_DX9
		MATRIX33&	operator = (const D3DXMATRIX& mat);
#endif
	public:
		static const MATRIX33 Identity;
	};


	class MIP_LIB MATRIX44
	{
	public:
		union {
			struct {
				float        _11, _12, _13, _14;
				float        _21, _22, _23, _24;
				float        _31, _32, _33, _34;
				float        _41, _42, _43, _44;
			};
			float m[4][4];
			float f[16];
		};
	public:
		MATRIX44();
		MATRIX44(const PLANE & x, const PLANE & y, const PLANE & z, const PLANE & w);
		MATRIX44(const VECTOR3 & inX, const VECTOR3 & inY, const VECTOR3 & inZ, const VECTOR3 & inW);
		MATRIX44(const MATRIX44& mat);
#ifdef USE_DX9
		MATRIX44(const D3DXMATRIX& mat);
#endif
		MATRIX44& identity();
		MATRIX44& inverse();
		MATRIX44& transpose();

		void	 set(QUATERNION& q);
		void	 set(float Yaw, float Pitch, float Roll, VECTOR3 & org = VECTOR3(0,0,0));
		void	 setAffine(QUATERNION& q, VECTOR3& trans, float scale = 1.0f);
		void	 setAffine(QUATERNION& q, VECTOR3& trans, VECTOR3& pivot, float scale = 1.0f);


		MATRIX44& rotationX(float fAngle);
		MATRIX44& rotationY(float fAngle);
		MATRIX44& rotationZ(float fAngle);
		MATRIX44& translation(float x, float y, float z);
		MATRIX44& translation(const VECTOR3& v);
		MATRIX44& scaling(VECTOR3& v);
		MATRIX44& scaling(float x, float y, float z);
		MATRIX44& scaling(float xyz);

		MATRIX44  getTranslationMatrixOnly() const;
		MATRIX44  getRotationMatrixOnly() const;

		MATRIX44& operator *= (const MATRIX44& mat);
		MATRIX44 operator * (const MATRIX44& mat) const;
#ifdef USE_DX9
		D3DXMATRIX operator * (const D3DXMATRIX& mat) const;
#endif
		void operator += (const MATRIX44 &mat);
		void operator -= (const MATRIX44 &mat);
		MATRIX44 operator + (const MATRIX44 &mat) const;
		MATRIX44 operator - (const MATRIX44 &mat) const;
		void operator *= (float f);
		VECTOR4 operator *(const VECTOR4& vec) const;
		VECTOR3 operator *(const VECTOR3& vec) const;
		void	operator = (const MATRIX44& mat);
		void	operator = (const MATRIX33& mat);
#ifdef USE_DX9
		void	operator = (const D3DXMATRIX& mat);
#endif

		float &	operator [] (int n);

		VECTOR3 getScaledXaxis() const;
		VECTOR3 getScaledYaxis() const;
		VECTOR3 getScaledZaxis() const;
		VECTOR3 getOrigin() const;
		void getYawPitchRoll(float & Yaw, float & Pitch, float & Roll) const;

		VECTOR3 getScaleVector() const;
		VECTOR3 extractScale();
		
		float	determinant() const;
		float   rotDeterminant() const;

		void    removeTranslation();
		void    removeRotation();
		void    removeScale();

		void setAxis(int i, const VECTOR3& Axis);
		void setOrigin(const VECTOR3& NewOrigin);

		QUATERNION getViewRotateXaxis(float radian) const;
		QUATERNION getViewRotateYaxis(float radian) const;
		QUATERNION getViewRotateZaxis(float radian) const;
		QUATERNION getQuaternion() const;

#ifdef USE_DX9
		D3DXMATRIX dx() const {
			D3DXMATRIX m; memcpy(&m, this, sizeof(D3DXMATRIX)); return m;
		}
#endif
	public:
		static const MATRIX44 Identity;
	};


	MIP_INLINE
		MATRIX44::MATRIX44()
	{
		memset(this, 0, sizeof(MATRIX44));
	}

	MIP_INLINE
		MATRIX44::MATRIX44(const MATRIX44& mat)
	{
		memcpy(this, &mat, sizeof(MATRIX44));
	}
#ifdef USE_DX9
	MIP_INLINE
		MATRIX44::MATRIX44(const D3DXMATRIX& mat)
	{
		memcpy(this, &mat, sizeof(MATRIX44));
	}
#endif

	MIP_INLINE
		void MATRIX44::operator += (const MATRIX44 &mat)
	{
		_11 += mat._11; _12 += mat._12; _13 += mat._13; _14 += mat._14;
		_21 += mat._21; _22 += mat._22; _23 += mat._23; _24 += mat._24;
		_31 += mat._31; _32 += mat._32; _33 += mat._33; _34 += mat._34;
		_41 += mat._41; _42 += mat._42; _43 += mat._43; _44 += mat._44;
	}

	MIP_INLINE
		void MATRIX44::operator -= (const MATRIX44 &mat)
	{
		_11 -= mat._11; _12 -= mat._12; _13 -= mat._13; _14 -= mat._14;
		_21 -= mat._21; _22 -= mat._22; _23 -= mat._23; _24 -= mat._24;
		_31 -= mat._31; _32 -= mat._32; _33 -= mat._33; _34 -= mat._34;
		_41 -= mat._41; _42 -= mat._42; _43 -= mat._43; _44 -= mat._44;
	}

	MIP_INLINE
		MATRIX44 MATRIX44::operator + (const MATRIX44 &mat) const
	{
		MATRIX44 m(*this);
		m._11 += mat._11; m._12 += mat._12; m._13 += mat._13; m._14 += mat._14;
		m._21 += mat._21; m._22 += mat._22; m._23 += mat._23; m._24 += mat._24;
		m._31 += mat._31; m._32 += mat._32; m._33 += mat._33; m._34 += mat._34;
		m._41 += mat._41; m._42 += mat._42; m._43 += mat._43; m._44 += mat._44;
		return m;
	}

	MIP_INLINE
		MATRIX44 MATRIX44::operator - (const MATRIX44 &mat) const
	{
		MATRIX44 m(*this);
		m._11 -= mat._11; m._12 -= mat._12; m._13 -= mat._13; m._14 -= mat._14;
		m._21 -= mat._21; m._22 -= mat._22; m._23 -= mat._23; m._24 -= mat._24;
		m._31 -= mat._31; m._32 -= mat._32; m._33 -= mat._33; m._34 -= mat._34;
		m._41 -= mat._41; m._42 -= mat._42; m._43 -= mat._43; m._44 -= mat._44;
		return m;
	}

	MIP_INLINE
		void MATRIX44::operator *= (float f)
	{
		_11 *= f; _12 *= f; _13 *= f; _14 *= f;
		_21 *= f; _22 *= f; _23 *= f; _24 *= f;
		_31 *= f; _32 *= f; _33 *= f; _34 *= f;
		_41 *= f; _42 *= f; _43 *= f; _44 *= f;
	}

	MIP_INLINE
		VECTOR4 MATRIX44::operator * (const VECTOR4& vec) const
	{
		VECTOR4 v;
		v.x = this->_11 * vec.x + this->_21 * vec.y + this->_31 * vec.z + this->_41 * vec.w;
		v.y = this->_12 * vec.x + this->_22 * vec.y + this->_32 * vec.z + this->_42 * vec.w;
		v.z = this->_13 * vec.x + this->_23 * vec.y + this->_33 * vec.z + this->_43 * vec.w;
		v.w = this->_14 * vec.x + this->_24 * vec.y + this->_34 * vec.z + this->_44 * vec.w;
		return v;
	}

	MIP_INLINE
		VECTOR3 MATRIX44::operator * (const VECTOR3& vec) const
	{
		VECTOR3 v;
		v.x = this->_11 * vec.x + this->_21 * vec.y + this->_31 * vec.z;
		v.y = this->_12 * vec.x + this->_22 * vec.y + this->_32 * vec.z;
		v.z = this->_13 * vec.x + this->_23 * vec.y + this->_33 * vec.z;
		return v;
	}

	MIP_INLINE
		void MATRIX44::operator = (const MATRIX44& mat)
	{
		memcpy(this, &mat, sizeof(MATRIX44));
	}

#ifdef USE_DX9
	MIP_INLINE
		void MATRIX44::operator = (const D3DXMATRIX& mat)
	{
		memcpy(this, &mat, sizeof(MATRIX44));
	}
#endif
	MIP_INLINE
		void MATRIX44::operator = (const MATRIX33& mat)
	{
		_11 = mat._11; _12 = mat._12; _13 = mat._13;
		_21 = mat._21; _22 = mat._22; _23 = mat._23;
		_31 = mat._31; _32 = mat._32; _33 = mat._33;
		_14 = _24 = _34 = _41 = _42 = _43 = 0.0f;
		_44 = 1.0f;
	}

	MIP_INLINE
		float & MATRIX44::operator [] (int n)
	{
		return f[n];
	}

	MIP_INLINE
		MATRIX44& MATRIX44::identity()
	{
		m[0][1] = m[0][2] = m[0][3] =
			m[1][0] = m[1][2] = m[1][3] =
			m[2][0] = m[2][1] = m[2][3] =
			m[3][0] = m[3][1] = m[3][2] = 0.0f;

		m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0f;
		return *this;
	}

	MIP_INLINE
		MATRIX44& MATRIX44::transpose()
	{
		MATRIX44 mat;
		mat.m[0][0] = m[0][0];
		mat.m[0][1] = m[1][0];
		mat.m[0][2] = m[2][0];
		mat.m[0][3] = m[3][0];

		mat.m[1][0] = m[0][1];
		mat.m[1][1] = m[1][1];
		mat.m[1][2] = m[2][1];
		mat.m[1][3] = m[3][1];

		mat.m[2][0] = m[0][2];
		mat.m[2][1] = m[1][2];
		mat.m[2][2] = m[2][2];
		mat.m[2][3] = m[3][2];

		mat.m[3][0] = m[0][3];
		mat.m[3][1] = m[1][3];
		mat.m[3][2] = m[2][3];
		mat.m[3][3] = m[3][3];
		*this = mat;
		return *this;
	}

	MIP_INLINE
		MATRIX44& MATRIX44::translation(float x, float y, float z)
	{
		*this = MATRIX44::Identity;
		this->_41 = x;
		this->_42 = y;
		this->_43 = z;
		return *this;
	}

	MIP_INLINE
		MATRIX44& MATRIX44::translation(const VECTOR3& v)
	{
		*this = MATRIX44::Identity;
		this->_41 = v.x;
		this->_42 = v.y;
		this->_43 = v.z;
		return *this;
	}

	MIP_INLINE
		MATRIX44& MATRIX44::scaling(VECTOR3& v)
	{
		*this = MATRIX44::Identity;
		this->_11 = v.x;
		this->_22 = v.y;
		this->_33 = v.z;
		return *this;
	}

	MIP_INLINE
		MATRIX44& MATRIX44::scaling(float x, float y, float z)
	{
		*this = MATRIX44::Identity;
		this->_11 = x;
		this->_22 = y;
		this->_33 = z;
		return *this;
	}

	MIP_INLINE
		MATRIX44& MATRIX44::scaling(float xyz)
	{
		*this = MATRIX44::Identity;
		this->_11 = xyz;
		this->_22 = xyz; 
		this->_33 = xyz;
		return *this;
	}


	MIP_INLINE VECTOR3 MATRIX44::getScaledXaxis() const
	{
		return VECTOR3(m[0][0], m[0][1], m[0][2]);
	}

	MIP_INLINE VECTOR3 MATRIX44::getScaledYaxis() const
	{
		return VECTOR3(m[1][0], m[1][1], m[1][2]);
	}

	MIP_INLINE VECTOR3 MATRIX44::getScaledZaxis() const
	{
		return VECTOR3(m[2][0], m[2][1], m[2][2]);
	}

	MIP_INLINE VECTOR3 MATRIX44::getOrigin() const
	{
		return VECTOR3(m[3][0], m[3][1], m[3][2]);
	}

	MIP_INLINE
		VECTOR3 MATRIX44::extractScale()
	{
		VECTOR3 Scale3D(0, 0, 0);

		// For each row, find magnitude, and if its non-zero re-scale so its unit length.
		const float SquareSum0 = (m[0][0] * m[0][0]) + (m[0][1] * m[0][1]) + (m[0][2] * m[0][2]);
		const float SquareSum1 = (m[1][0] * m[1][0]) + (m[1][1] * m[1][1]) + (m[1][2] * m[1][2]);
		const float SquareSum2 = (m[2][0] * m[2][0]) + (m[2][1] * m[2][1]) + (m[2][2] * m[2][2]);

		if (SquareSum0 > MIP_EPSILON)
		{
			float Scale0 = sqrt(SquareSum0);
			Scale3D[0] = Scale0;
			float InvScale0 = 1.f / Scale0;
			m[0][0] *= InvScale0;
			m[0][1] *= InvScale0;
			m[0][2] *= InvScale0;
		}
		else
		{
			Scale3D[0] = 0;
		}

		if (SquareSum1 > MIP_EPSILON)
		{
			float Scale1 = sqrt(SquareSum1);
			Scale3D[1] = Scale1;
			float InvScale1 = 1.f / Scale1;
			m[1][0] *= InvScale1;
			m[1][1] *= InvScale1;
			m[1][2] *= InvScale1;
		}
		else
		{
			Scale3D[1] = 0;
		}

		if (SquareSum2 > MIP_EPSILON)
		{
			float Scale2 = sqrt(SquareSum2);
			Scale3D[2] = Scale2;
			float InvScale2 = 1.f / Scale2;
			m[2][0] *= InvScale2;
			m[2][1] *= InvScale2;
			m[2][2] *= InvScale2;
		}
		else
		{
			Scale3D[2] = 0;
		}

		return Scale3D;
	}

	MIP_INLINE
		VECTOR3 MATRIX44::getScaleVector() const
	{
		VECTOR3 Scale3D(1, 1, 1);

		// For each row, find magnitude, and if its non-zero re-scale so its unit length.
		for (unsigned int i = 0; i<3; i++)
		{
			const float SquareSum = (m[i][0] * m[i][0]) + (m[i][1] * m[i][1]) + (m[i][2] * m[i][2]);
			if (SquareSum > MIP_EPSILON)
			{
				Scale3D[i] = sqrt(SquareSum);
			}
			else
			{
				Scale3D[i] = 0.f;
			}
		}

		return Scale3D;
	}

	MIP_INLINE float MATRIX44::determinant() const
	{
		return	m[0][0] * (
			m[1][1] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) -
			m[2][1] * (m[1][2] * m[3][3] - m[1][3] * m[3][2]) +
			m[3][1] * (m[1][2] * m[2][3] - m[1][3] * m[2][2])
			) -
			m[1][0] * (
			m[0][1] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) -
			m[2][1] * (m[0][2] * m[3][3] - m[0][3] * m[3][2]) +
			m[3][1] * (m[0][2] * m[2][3] - m[0][3] * m[2][2])
			) +
			m[2][0] * (
			m[0][1] * (m[1][2] * m[3][3] - m[1][3] * m[3][2]) -
			m[1][1] * (m[0][2] * m[3][3] - m[0][3] * m[3][2]) +
			m[3][1] * (m[0][2] * m[1][3] - m[0][3] * m[1][2])
			) -
			m[3][0] * (
			m[0][1] * (m[1][2] * m[2][3] - m[1][3] * m[2][2]) -
			m[1][1] * (m[0][2] * m[2][3] - m[0][3] * m[2][2]) +
			m[2][1] * (m[0][2] * m[1][3] - m[0][3] * m[1][2])
			);
	}

	MIP_INLINE float MATRIX44::rotDeterminant() const
	{
		return
			m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) -
			m[1][0] * (m[0][1] * m[2][2] - m[0][2] * m[2][1]) +
			m[2][0] * (m[0][1] * m[1][2] - m[0][2] * m[1][1]);
	}

	MIP_INLINE
		void MATRIX44::removeTranslation()
	{
		this->_41 = 0.0f;
		this->_42 = 0.0f;
		this->_43 = 0.0f;
	}

	MIP_INLINE
		void MATRIX44::removeRotation()
	{

	}

	MIP_INLINE
		void MATRIX44::removeScale()
	{
		// For each row, find magnitude, and if its non-zero re-scale so its unit length.
		const float SquareSum0 = (m[0][0] * m[0][0]) + (m[0][1] * m[0][1]) + (m[0][2] * m[0][2]);
		const float SquareSum1 = (m[1][0] * m[1][0]) + (m[1][1] * m[1][1]) + (m[1][2] * m[1][2]);
		const float SquareSum2 = (m[2][0] * m[2][0]) + (m[2][1] * m[2][1]) + (m[2][2] * m[2][2]);
		const float Scale0 = (SquareSum0 - MIP_EPSILON) >= 0.0f ? 1.0f / sqrt(SquareSum0) : 1.0f;
		const float Scale1 = (SquareSum1 - MIP_EPSILON) >= 0.0f ? 1.0f / sqrt(SquareSum1) : 1.0f;
		const float Scale2 = (SquareSum2 - MIP_EPSILON) >= 0.0f ? 1.0f / sqrt(SquareSum2) : 1.0f;
		m[0][0] *= Scale0;
		m[0][1] *= Scale0;
		m[0][2] *= Scale0;
		m[1][0] *= Scale1;
		m[1][1] *= Scale1;
		m[1][2] *= Scale1;
		m[2][0] *= Scale2;
		m[2][1] *= Scale2;
		m[2][2] *= Scale2;
	}
	
	MIP_INLINE void MATRIX44::setAxis(int i, const VECTOR3& Axis)
	{
//		checkSlow(i >= 0 && i <= 2);
		m[i][0] = Axis.x;
		m[i][1] = Axis.y;
		m[i][2] = Axis.z;
	}

	MIP_INLINE void MATRIX44::setOrigin(const VECTOR3& NewOrigin)
	{
		m[3][0] = NewOrigin.x;
		m[3][1] = NewOrigin.y;
		m[3][2] = NewOrigin.z;
	}
	
	MIP_INLINE
		MATRIX33::MATRIX33()
	{
	}

	MIP_INLINE
		MATRIX33::MATRIX33(const MATRIX33 & mat)
	{
		memcpy(this, &mat, sizeof(float) * 9);
	}

	MIP_INLINE
		MATRIX33::MATRIX33(const VECTOR3& x, const VECTOR3& y, const VECTOR3& z)
	{
		m[0][0] = x.x; m[0][1] = x.y;  m[0][2] = x.z;
		m[1][0] = y.x; m[1][1] = y.y;  m[1][2] = y.z;
		m[2][0] = z.x; m[2][1] = z.y;  m[2][2] = z.z;
	}

	MIP_INLINE
		MATRIX33& MATRIX33::operator *= (const MATRIX33& mat)
	{
		MATRIX33 m;
		m._11 = _11 * mat._11 + _21 * mat._12 + _31 * mat._13;
		m._12 = _21 * mat._11 + _22 * mat._12 + _23 * mat._13;
		m._13 = _31 * mat._11 + _32 * mat._12 + _33 * mat._13;
		m._21 = _11 * mat._21 + _21 * mat._22 + _31 * mat._23;
		m._22 = _21 * mat._21 + _22 * mat._22 + _23 * mat._23;
		m._23 = _31 * mat._21 + _32 * mat._22 + _23 * mat._23;
		m._31 = _11 * mat._31 + _21 * mat._32 + _31 * mat._33;
		m._32 = _21 * mat._31 + _22 * mat._32 + _23 * mat._33;
		m._33 = _31 * mat._31 + _32 * mat._32 + _23 * mat._33;
		return *this = m;
	}

	MIP_INLINE
		MATRIX33 MATRIX33::operator * (const MATRIX33& mat) const
	{
		MATRIX33 m;
		m._11 = _11 * mat._11 + _21 * mat._12 + _31 * mat._13;
		m._12 = _21 * mat._11 + _22 * mat._12 + _23 * mat._13;
		m._13 = _31 * mat._11 + _32 * mat._12 + _33 * mat._13;
		m._21 = _11 * mat._21 + _21 * mat._22 + _31 * mat._23;
		m._22 = _21 * mat._21 + _22 * mat._22 + _23 * mat._23;
		m._23 = _31 * mat._21 + _32 * mat._22 + _23 * mat._23;
		m._31 = _11 * mat._31 + _21 * mat._32 + _31 * mat._33;
		m._32 = _21 * mat._31 + _22 * mat._32 + _23 * mat._33;
		m._33 = _31 * mat._31 + _32 * mat._32 + _23 * mat._33;
		return m;
	}
#ifdef USE_DX9
	MIP_INLINE
		D3DXMATRIX MATRIX33::operator * (const D3DXMATRIX& mat) const
	{
		D3DXMATRIX m = mat;;
		m._11 = _11 * mat._11 + _21 * mat._12 + _31 * mat._13;
		m._12 = _21 * mat._11 + _22 * mat._12 + _23 * mat._13;
		m._13 = _31 * mat._11 + _32 * mat._12 + _33 * mat._13;
		m._21 = _11 * mat._21 + _21 * mat._22 + _31 * mat._23;
		m._22 = _21 * mat._21 + _22 * mat._22 + _23 * mat._23;
		m._23 = _31 * mat._21 + _32 * mat._22 + _23 * mat._23;
		m._31 = _11 * mat._31 + _21 * mat._32 + _31 * mat._33;
		m._32 = _21 * mat._31 + _22 * mat._32 + _23 * mat._33;
		m._33 = _31 * mat._31 + _32 * mat._32 + _23 * mat._33;
		return m;
	}
#endif
	MIP_INLINE
		void MATRIX33::operator += (const MATRIX33 &mat)
	{
		_11 += mat._11; _12 += mat._12; _13 += mat._13;
		_21 += mat._21; _22 += mat._22; _23 += mat._23;
		_31 += mat._31; _32 += mat._32; _33 += mat._33;
	}

	MIP_INLINE
		void MATRIX33::operator -= (const MATRIX33 &mat)
	{
		_11 -= mat._11; _12 -= mat._12; _13 -= mat._13;
		_21 -= mat._21; _22 -= mat._22; _23 -= mat._23;
		_31 -= mat._31; _32 -= mat._32; _33 -= mat._33;
	}

	MIP_INLINE
		MATRIX33 MATRIX33::operator + (const MATRIX33 &mat) const
	{
		MATRIX33 m(*this);
		m._11 += mat._11; m._12 += mat._12; m._13 += mat._13;
		m._21 += mat._21; m._22 += mat._22; m._23 += mat._23;
		m._31 += mat._31; m._32 += mat._32; m._33 += mat._33;
		return m;
	}

	MIP_INLINE
		MATRIX33 MATRIX33::operator - (const MATRIX33 &mat) const
	{
		MATRIX33 m(*this);
		m._11 -= mat._11; m._12 -= mat._12; m._13 -= mat._13;
		m._21 -= mat._21; m._22 -= mat._22; m._23 -= mat._23;
		m._31 -= mat._31; m._32 -= mat._32; m._33 -= mat._33;
		return m;
	}

	MIP_INLINE
		void MATRIX33::operator *= (float f)
	{
		_11 *= f; _12 *= f; _13 *= f;
		_21 *= f; _22 *= f; _23 *= f;
		_31 *= f; _32 *= f; _33 *= f;
	}

	MIP_INLINE
		MATRIX33& MATRIX33::operator = (const MATRIX33& mat)
	{
		memcpy(this, &mat, sizeof(MATRIX33));
		return *this;
	}

	MIP_INLINE
		MATRIX33& MATRIX33::operator = (const MATRIX44& mat)
	{
		_11 = mat._11; _12 = mat._12; _13 = mat._13;
		_21 = mat._21; _22 = mat._22; _23 = mat._23;
		_31 = mat._31; _32 = mat._32; _33 = mat._33;
		return *this;
	}
#ifdef USE_DX9
	MIP_INLINE
		MATRIX33& MATRIX33::operator = (const D3DXMATRIX& mat)
	{
		_11 = mat._11; _12 = mat._12; _13 = mat._13;
		_21 = mat._21; _22 = mat._22; _23 = mat._23;
		_31 = mat._31; _32 = mat._32; _33 = mat._33;
		return *this;
	}
#endif
	MIP_INLINE
		MATRIX33& MATRIX33::identity()
	{
		m[0][1] = m[0][2] =
			m[1][0] = m[1][2] =
			m[2][0] = m[2][1] = 0.0f;

		m[0][0] = m[1][1] = m[2][2] = 1.0f;
		return *this;
	}

	MIP_INLINE
		MATRIX33& MATRIX33::inverse()
	{
		MATRIX44 m;
#ifdef USE_DX9
		m = *this;
		D3DXMatrixInverse((D3DXMATRIX*)&m, NULL, (D3DXMATRIX*)this);
#endif
		return *this = m;
	}

	MIP_INLINE
		void MATRIX33::set(QUATERNION& q)
	{
		MATRIX44 m;
#ifdef USE_DX9
		D3DXMatrixRotationQuaternion((D3DXMATRIX*)&m, (D3DXQUATERNION*)&q);
#endif
		*this = m;
	}

	MIP_INLINE
		void MATRIX33::set(float Yaw, float Pitch, float Roll)
	{
		MATRIX44 m;
#ifdef USE_DX9
		D3DXMatrixRotationYawPitchRoll((D3DXMATRIX*)&m, Yaw, Pitch, Roll);
#endif
		*this = m;
	}

	MIP_INLINE
		MATRIX33& MATRIX33::transpose()
	{
		MATRIX33 mat(*this);
		_11 = mat._11; _12 = mat._21; _13 = mat._31;
		_21 = mat._12; _22 = mat._22; _23 = mat._32;
		_31 = mat._13; _32 = mat._23; _33 = mat._33;
		return *this;
	}

	MIP_INLINE
		MATRIX33& MATRIX33::rotationX(float fAngle)
	{
		MATRIX44 m;
#ifdef USE_DX9
		D3DXMatrixRotationX((D3DXMATRIX*)&m, fAngle);
#endif
		return *this = m;
	}

	MIP_INLINE
		MATRIX33& MATRIX33::rotationY(float fAngle)
	{
		MATRIX44 m;
#ifdef USE_DX9
		D3DXMatrixRotationY((D3DXMATRIX*)&m, fAngle);
#endif
		return *this = m;
	}

	MIP_INLINE
		MATRIX33& MATRIX33::rotationZ(float fAngle)
	{
		MATRIX44 m;
#ifdef USE_DX9
		D3DXMatrixRotationZ((D3DXMATRIX*)&m, fAngle);
#endif
		return *this = m;
	}

	MIP_INLINE
		MATRIX44 MATRIX44::getTranslationMatrixOnly() const
	{
		MATRIX44 m = MATRIX44::Identity;
		m.setOrigin(getOrigin());
		return m;
	}

	MIP_INLINE
		MATRIX44 MATRIX44::getRotationMatrixOnly() const
	{
		MATRIX44 m = *this;
		m.setOrigin(VECTOR3(0,0,0));
		return m;
	}
};