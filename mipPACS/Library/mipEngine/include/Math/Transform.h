#pragma once
#include "math.h"

namespace mip
{
	class TRANSFORM
	{
	public:
		QUATERNION		rotation;
//		float			pitch, yaw, roll;
		VECTOR3			translation;
		VECTOR3			scale;

	private:
		bool			needUpdate;
		MATRIX44		mat;
	public:
		TRANSFORM();
		TRANSFORM(const VECTOR3 & t);
		TRANSFORM(float _pitch, float _yaw, float _roll, const VECTOR3 & trans, const VECTOR3 & scal);
		TRANSFORM(float pitch, float yaw, float roll);
		TRANSFORM(VECTOR3 & euler);
		TRANSFORM(const QUATERNION & q);
		TRANSFORM(const MATRIX44 & m);
		TRANSFORM(const QUATERNION & rot, const VECTOR3 & trans, const VECTOR3 & scal);
		
		TRANSFORM operator+(const TRANSFORM& V) const;
		TRANSFORM operator*(float Scale) const;
		TRANSFORM operator*(const TRANSFORM& V) const;

		TRANSFORM operator+=(const TRANSFORM& V);
		TRANSFORM operator*=(float Scale);
		TRANSFORM operator*=(const TRANSFORM& V);

		QUATERNION getQuaternion() const;
		MATRIX44 getMatrix();
		MATRIX44 getMatrix(mip::VECTOR3 scale) const;
		MATRIX44 getMatrixNoScale() const;
		MATRIX44 getTranslationMatrixOnly();
		MATRIX44 getRotationMatrixOnly();
		VECTOR3 getEuler() const;
		VECTOR3 getVector() const;

		void set(float _pitch, float _yaw, float _roll, const VECTOR3 & translation, const VECTOR3 & scale);
		void set(const QUATERNION & q);
		void set(float pitch, float yaw, float roll);
		void set(VECTOR3 & euler);
		VECTOR3 transform(VECTOR3 v, bool scale = false) const;
		VECTOR3 transform(VECTOR4 v, bool scale = false) const;

		void addRotate(const QUATERNION & q);
		void addRotateX(float x);
		void addRotateY(float y);
		void addRotateZ(float z);
		void addTranslate(const VECTOR3 & v);
		void addScale(const VECTOR3 & v);

		void setRotate(const QUATERNION & q);
		void setTranslate(const VECTOR3 & v);
		void setScale(const VECTOR3 & v);
		void zero();
		bool isNeedUpdate() { return needUpdate; }
	};

	MIP_INLINE
		TRANSFORM::TRANSFORM()
	{
		mat = MATRIX44::Identity;
		needUpdate = false;
		//yaw = 0.0f;
		//pitch = 0.0f;
		//roll = 0.0f;
		rotation = QUATERNION::Identity;
		translation = VECTOR3(0, 0, 0);
		scale = VECTOR3(1, 1, 1);
	}

	MIP_INLINE
		TRANSFORM::TRANSFORM(const QUATERNION & q)
	{
		mat = MATRIX44::Identity;
		needUpdate = true;

		rotation = q;
		translation = VECTOR3(0, 0, 0);
		scale = VECTOR3(1, 1, 1);
	}

	MIP_INLINE
		TRANSFORM::TRANSFORM(const VECTOR3 & t)
	{
		mat = MATRIX44::Identity;
		needUpdate = true;

		//yaw = 0.0f;
		//pitch = 0.0f;
		//roll = 0.0f;
		rotation = QUATERNION::Identity;
		translation = t;
		scale = VECTOR3(1, 1, 1);
	}
	
	MIP_INLINE
		TRANSFORM::TRANSFORM(float _pitch, float _yaw, float _roll, const VECTOR3 & trans, const VECTOR3 & scal)
	{
		set(_pitch, _yaw, _roll, trans, scal);
	}

	MIP_INLINE
		TRANSFORM::TRANSFORM(float _pitch, float _yaw, float _roll)
	{
		translation = VECTOR3(0, 0, 0);
		scale = VECTOR3(1, 1, 1);
		set(_pitch, _yaw, _roll);
	}

	MIP_INLINE
		TRANSFORM::TRANSFORM(VECTOR3 & euler)
	{
		translation = VECTOR3(0, 0, 0);
		scale = VECTOR3(1, 1, 1);
		set(euler);
	}

	MIP_INLINE
		void TRANSFORM::zero()
	{
		needUpdate = true;
		rotation = QUATERNION::Identity;
		translation = VECTOR3(0, 0, 0);
		scale = VECTOR3(1, 1, 1);
	}

	MIP_INLINE
		VECTOR3 TRANSFORM::getEuler() const
	{
		float y, p, r;
		rotation.getYawPitchRoll(y,p,r);
		return VECTOR3(r, p, y);
	}

	MIP_INLINE
		VECTOR3 TRANSFORM::getVector() const
	{
		float y, p, r;
		rotation.getYawPitchRoll(y, p, r);

		float CP, SP, CY, SY;
		math::SinCos(&SP, &CP, math::DegreesToRadians(p));
		math::SinCos(&SY, &CY, math::DegreesToRadians(y));
		return VECTOR3(CP*CY, CP*SY, SP);
	}

	MIP_INLINE
		QUATERNION TRANSFORM::getQuaternion() const
	{
		//return QUATERNION(yaw, pitch, roll);
		return rotation;
	}

	MIP_INLINE
		MATRIX44 TRANSFORM::getMatrix()
	{
		if (needUpdate)
		{
			needUpdate = false;

			MATRIX44 OutMatrix;
			//		QUATERNION rotation(pitch, yaw, roll);

			OutMatrix.m[3][0] = translation.x;
			OutMatrix.m[3][1] = translation.y;
			OutMatrix.m[3][2] = translation.z;

			const float x2 = rotation.x + rotation.x;
			const float y2 = rotation.y + rotation.y;
			const float z2 = rotation.z + rotation.z;
			{
				const float xx2 = rotation.x * x2;
				const float yy2 = rotation.y * y2;
				const float zz2 = rotation.z * z2;

				OutMatrix.m[0][0] = (1.0f - (yy2 + zz2)) * scale.x;
				OutMatrix.m[1][1] = (1.0f - (xx2 + zz2)) * scale.y;
				OutMatrix.m[2][2] = (1.0f - (xx2 + yy2)) * scale.z;
			}
			{
				const float yz2 = rotation.y * z2;
				const float wx2 = rotation.w * x2;

				OutMatrix.m[2][1] = (yz2 - wx2) * scale.z;
				OutMatrix.m[1][2] = (yz2 + wx2) * scale.y;
			}
			{
				const float xy2 = rotation.x * y2;
				const float wz2 = rotation.w * z2;

				OutMatrix.m[1][0] = (xy2 - wz2) * scale.y;
				OutMatrix.m[0][1] = (xy2 + wz2) * scale.x;
			}
			{
				const float xz2 = rotation.x * z2;
				const float wy2 = rotation.w * y2;

				OutMatrix.m[2][0] = (xz2 + wy2) * scale.z;
				OutMatrix.m[0][2] = (xz2 - wy2) * scale.x;
			}

			OutMatrix.m[0][3] = 0.0f;
			OutMatrix.m[1][3] = 0.0f;
			OutMatrix.m[2][3] = 0.0f;
			OutMatrix.m[3][3] = 1.0f;

			mat = OutMatrix;
		}
	
		return mat;
	}

	MIP_INLINE
		MATRIX44 TRANSFORM::getTranslationMatrixOnly()
	{
		return getMatrix().getTranslationMatrixOnly();
	}

	MIP_INLINE
		MATRIX44 TRANSFORM::getRotationMatrixOnly()
	{
		return getMatrix().getRotationMatrixOnly();
	}

	MIP_INLINE
		MATRIX44 TRANSFORM::getMatrixNoScale() const
	{
		MATRIX44 OutMatrix;
//		QUATERNION rotation(yaw, pitch, roll);

		OutMatrix.m[3][0] = translation.x;
		OutMatrix.m[3][1] = translation.y;
		OutMatrix.m[3][2] = translation.z;

		const float x2 = rotation.x + rotation.x;
		const float y2 = rotation.y + rotation.y;
		const float z2 = rotation.z + rotation.z;
		{
			const float xx2 = rotation.x * x2;
			const float yy2 = rotation.y * y2;
			const float zz2 = rotation.z * z2;

			OutMatrix.m[0][0] = (1.0f - (yy2 + zz2));
			OutMatrix.m[1][1] = (1.0f - (xx2 + zz2));
			OutMatrix.m[2][2] = (1.0f - (xx2 + yy2));
		}
		{
			const float yz2 = rotation.y * z2;
			const float wx2 = rotation.w * x2;

			OutMatrix.m[2][1] = (yz2 - wx2);
			OutMatrix.m[1][2] = (yz2 + wx2);
		}
		{
			const float xy2 = rotation.x * y2;
			const float wz2 = rotation.w * z2;

			OutMatrix.m[1][0] = (xy2 - wz2);
			OutMatrix.m[0][1] = (xy2 + wz2);
		}
		{
			const float xz2 = rotation.x * z2;
			const float wy2 = rotation.w * y2;

			OutMatrix.m[2][0] = (xz2 + wy2);
			OutMatrix.m[0][2] = (xz2 - wy2);
		}

		OutMatrix.m[0][3] = 0.0f;
		OutMatrix.m[1][3] = 0.0f;
		OutMatrix.m[2][3] = 0.0f;
		OutMatrix.m[3][3] = 1.0f;

		return OutMatrix;
	}


	MIP_INLINE TRANSFORM TRANSFORM::operator+(const TRANSFORM& V) const
	{
//		return TRANSFORM(pitch + V.pitch, yaw + V.yaw, roll + V.roll, translation + V.translation, scale + V.scale);
		return TRANSFORM(rotation + V.rotation, translation + V.translation, scale + V.scale);
	}

	MIP_INLINE TRANSFORM TRANSFORM::operator*(float Scale) const
	{
//		return TRANSFORM(pitch*Scale, yaw*Scale, roll*Scale, translation*Scale, scale*Scale);
		return TRANSFORM(rotation*Scale, translation*Scale, scale*Scale);
	}

	MIP_INLINE TRANSFORM TRANSFORM::operator+=(const TRANSFORM& Atom)
	{
		needUpdate = true;
		translation += Atom.translation;

		//yaw += Atom.yaw;
		//pitch += Atom.pitch;
		//roll += Atom.roll;
		rotation.x += Atom.rotation.x;
		rotation.y += Atom.rotation.y;
		rotation.z += Atom.rotation.z;
		rotation.w += Atom.rotation.w;

		scale += Atom.scale;
		return *this;
	}

	MIP_INLINE TRANSFORM TRANSFORM::operator*=(float Scale)
	{
		needUpdate = true;
		//pitch *= Scale;
		//yaw *= Scale;
		//roll *= Scale;
		rotation *= Scale;
		translation *= Scale;
		scale *= Scale;
	}

	MIP_INLINE
		void TRANSFORM::set(float _pitch, float _yaw, float _roll, const VECTOR3 & trans, const VECTOR3 & scal)
	{
		needUpdate = true;
		//pitch = _pitch;
		//yaw = _yaw;
		//roll = _roll;
		rotation.setYawPitchRoll(_yaw, _pitch, _roll);
		translation = trans;
		scale = scal;
	}

	MIP_INLINE
		void TRANSFORM::set(VECTOR3 & euler)
	{
		needUpdate = true;
		//pitch = euler.y;
		//yaw = euler.z;
		//roll = euler.x;
		rotation.setYawPitchRoll(euler.z, euler.y, euler.x);
	}

	MIP_INLINE
		void TRANSFORM::set(const QUATERNION & q)
	{
		needUpdate = true;
		//q.getYawPitchRoll(yaw, pitch, roll);
		rotation = q;
	}

	MIP_INLINE
		void TRANSFORM::set(float _pitch, float _yaw, float _roll)
	{
		needUpdate = true;
		//pitch = _pitch;
		//yaw = _yaw;
		//roll = _roll;
		rotation.setYawPitchRoll(_yaw, _pitch, _roll);
	}

	MIP_INLINE
		VECTOR3 TRANSFORM::transform(VECTOR3 v, bool scale) const
	{
		return transform(VECTOR4(v.x, v.y, v.z, 1.0f), scale);
	}

	MIP_INLINE
		VECTOR3 TRANSFORM::transform(VECTOR4 v, bool _scale) const
	{
		// if not, this won't work
		//checkSlow(V.W == 0.f || V.W == 1.f);

		//Transform using QST is following
		//QST(P) = Q*S*P*-Q + T where Q = quaternion, S = scale, T = translation
//		QUATERNION rotation(pitch, yaw, roll);

		VECTOR4 Transform = (rotation * (_scale?( v * scale ):v));
		if (v.w == 1.f)
		{
			Transform += VECTOR4(translation, 1.f);
		}

		return Transform;
	}

	MIP_INLINE
		void TRANSFORM::addRotate(const QUATERNION & q)
	{
		needUpdate = true;
		//float y, p, r;
		//q.getYawPitchRoll(y, p, r);
		//yaw += y;
		//pitch += p;
		//roll += r;
		rotation *= q;
	}

	MIP_INLINE
		void TRANSFORM::addRotateX(float x)
	{
		needUpdate = true;
		VECTOR3 axis(1, 0, 0);

		QUATERNION qRot;
		qRot.setRotationAxis(axis, x);
		rotation *= qRot;
	}
		
	MIP_INLINE
		void TRANSFORM::addRotateY(float y)
	{
		needUpdate = true;
		VECTOR3 axis(0, 1, 0);

		QUATERNION qRot;
		qRot.setRotationAxis(axis, y);
		rotation *= qRot;
	}

	MIP_INLINE
		void TRANSFORM::addRotateZ(float z)
	{
		needUpdate = true;
		VECTOR3 axis(0, 0, 1);

		QUATERNION qRot;
		qRot.setRotationAxis(axis, z);
		rotation *= qRot;
	}
		
	MIP_INLINE
		void TRANSFORM::addTranslate(const VECTOR3 & v)
	{
		needUpdate = true;
		translation += v;
	}
	
	MIP_INLINE
		void TRANSFORM::addScale(const VECTOR3 & v)
	{
		needUpdate = true;
		scale += v;
	}

	MIP_INLINE
		void TRANSFORM::setTranslate(const VECTOR3 & v)
	{
		needUpdate = true;
		translation = v;
	}

	MIP_INLINE
		void TRANSFORM::setRotate(const QUATERNION & q)
	{
		needUpdate = true;
		rotation = q;
	}

	MIP_INLINE
		void TRANSFORM::setScale(const VECTOR3 & v)
	{
		needUpdate = true;
		scale = v;
	}
};