#pragma once
#include "Mip/core.h"

#define LERP(a, b, t)		((a)*(1.0f-(t))+(b)*(t))

#define MIP_EPSILON   ((float)0.001f)
#define MIP_EPSILON2   ((float)0.00001f)
#define MIP_EPSILON3   ((float)0.1f)
#define MIP_SMALL_NUMBER	((float)1.e-8f)
#define MIP_MAX_FLT 3.402823466e+38F
#define MIP_KINDA_SMALL_NUMBER	(1.e-4f)
#define MIP_BIG_NUMBER			(3.4e+38f)
#define MIP_EULERS_NUMBER       (2.71828182845904523536f)

#define MIP_PI		  (3.141592654f)
#define MIP_INV_PI	  (0.31830988618f)
#define MIP_HALF_PI	  (1.57079632679f)

#define MIN_uint8		((muint8)	0x00)
#define	MIN_uint16		((muint16)	0x0000)
#define	MIN_muint32		((muint32)	0x00000000)
#define MIN_uint64		((muint64)	0x0000000000000000)
#define MIN_int8		((mint8)		-128)
#define MIN_int16		((mint16)	-32768)
#define MIN_int32		((mint32)	0x80000000)
#define MIN_int64		((mint64)	0x8000000000000000)

#define MAX_uint8		((muint8)	0xff)
#define MAX_uint16		((muint16)	0xffff)
#define MAX_muint32		((muint32)	0xffffffff)
#define MAX_uint64		((muint64)	0xffffffffffffffff)
#define MAX_int8		((mint8)		0x7f)
#define MAX_int16		((mint16)	0x7fff)
#define MAX_int32		((mint32)	0x7fffffff)
#define MAX_int64		((mint64)	0x7fffffffffffffff)

#define MIN_float			(1.175494351e-38F)			/* min positive value */
#define MAX_float			(3.402823466e+38F)
#define MIN_double			(2.2250738585072014e-308)	/* min positive value */
#define MAX_double			(1.7976931348623158e+308)	

inline float RADIAN(int x)  { return float(x)*0.017453292519943295769236907684885f; }
inline float RADIAN(float x)  { return (x)*0.017453292519943295769236907684885f; }
inline double RADIAN(double x)  { return (x)*0.017453292519943295769236907684885; }
inline float DEGREE(float x)  { return (x)*57.295779513082320876798154814114f; }
inline double DEGREE(double x)  { return (x)*57.295779513082320876798154814114; }

#include "MathSSE.h"

#include "Vector.h"
#include "Matrix.h"
#include "Plane.h"
#include "Quaternion.h"
#include "Range.h"
#include "Box.h"

namespace mip
{
	namespace math
	{
		static MIP_INLINE mint32 TruncToInt(float F)
		{
			return _mm_cvtt_ss2si(_mm_set_ss(F));
		}

		/** Returns higher value in a generic way */
		template< class T >
		static MIP_INLINE T Max(const T A, const T B)
		{
			return (A >= B) ? A : B;
		}

		/** Returns lower value in a generic way */
		template< class T >
		static MIP_INLINE T Min(const T A, const T B)
		{
			return (A <= B) ? A : B;
		}

		/** Returns highest of 3 values */
		template< class T >
		static MIP_INLINE T Max3(const T A, const T B, const T C)
		{
			return Max(Max(A, B), C);
		}

		/** Returns lowest of 3 values */
		template< class T >
		static MIP_INLINE T Min3(const T A, const T B, const T C)
		{
			return Min(Min(A, B), C);
		}

		static MIP_INLINE mint32 Rand() { return rand(); }
		static MIP_INLINE float FRand() { return Rand() / (float)RAND_MAX; }

		static MIP_INLINE float Abs(float Value) { return fabsf(Value); }

		template< class T >
		static MIP_INLINE T Square(const T A)
		{
			return A*A;
		};

		template<class T>
		static MIP_INLINE float DegreesToRadians(T const& DegVal)
		{
			return DegVal * (MIP_PI / 180.f);
		}

		template<class T>
		static MIP_INLINE float RadiansToDegrees(T const& RadVal)
		{
			return RadVal * (180.f / MIP_PI);
		}

		template< class U >
		static MIP_INLINE bool IsWithin(const U& TestValue, const U& MinValue, const U& MaxValue)
		{
			return ((TestValue >= MinValue) && (TestValue < MaxValue));
		}

		/** Checks if value is within a range, inclusive on MaxValue) */
		template< class U >
		static MIP_INLINE bool IsWithinInclusive(const U& TestValue, const U& MinValue, const U& MaxValue)
		{
			return ((TestValue >= MinValue) && (TestValue <= MaxValue));
		}

		/**
		*	Checks if two floating point numbers are nearly equal.
		*	@param A				First number to compare
		*	@param B				Second number to compare
		*	@param ErrorTolerance	Maximum allowed difference for considering them as 'nearly equal'
		*	@return					true if A and B are nearly equal
		*/
		static MIP_INLINE bool IsNearlyEqual(float A, float B, float ErrorTolerance = MIP_SMALL_NUMBER)
		{
			return Abs(A - B) < ErrorTolerance;
		}

		/**
		*	Checks if two floating point numbers are nearly equal.
		*	@param A				First number to compare
		*	@param B				Second number to compare
		*	@param ErrorTolerance	Maximum allowed difference for considering them as 'nearly equal'
		*	@return					true if A and B are nearly equal
		*/
		static MIP_INLINE bool IsNearlyEqual(double A, double B, double ErrorTolerance = MIP_SMALL_NUMBER)
		{
			return Abs(A - B) < ErrorTolerance;
		}

		/**
		*	Checks if a floating point number is nearly zero.
		*	@param Value			Number to compare
		*	@param ErrorTolerance	Maximum allowed difference for considering Value as 'nearly zero'
		*	@return					true if Value is nearly zero
		*/
		static MIP_INLINE bool IsNearlyZero(float Value, float ErrorTolerance = MIP_SMALL_NUMBER)
		{
			return Abs(Value) < ErrorTolerance;
		}

		/**
		*	Checks if a floating point number is nearly zero.
		*	@param Value			Number to compare
		*	@param ErrorTolerance	Maximum allowed difference for considering Value as 'nearly zero'
		*	@return					true if Value is nearly zero
		*/
		static MIP_INLINE bool IsNearlyZero(double Value, double ErrorTolerance = MIP_SMALL_NUMBER)
		{
			return Abs(Value) < ErrorTolerance;
		}

		/**
		*	Checks whether a number is a power of two.
		*	@param Value	Number to check
		*	@return			true if Value is a power of two
		*/
		static MIP_INLINE bool IsPowerOfTwo(muint32 Value)
		{
			return ((Value & (Value - 1)) == 0);
		}

		/** Clamps X to be between Min and Max, inclusive */
		template< class T >
		static MIP_INLINE T Clamp(const T X, const T Min, const T Max)
		{
			return X<Min ? Min : X<Max ? X : Max;
		}

		float ClampAxis(float Angle);
		float NormalizeAxis(float Angle);

		static MIP_INLINE float Exp(float Value) { return expf(Value); }
		static MIP_INLINE float Loge(float Value) { return logf(Value); }
		static MIP_INLINE float LogX(float Base, float Value) { return Loge(Value) / Loge(Base); }
		static MIP_INLINE float Log2(float Value) { return Loge(Value) * 1.4426950f; } 		// 1.0 / Loge(2) = 1.4426950f
		static MIP_INLINE float Fmod(float X, float Y) { return fmodf(X, Y); }
		static MIP_INLINE float Sin(float Value) { return sinf(Value); }
		static MIP_INLINE float Asin(float Value) { return asinf((Value<-1.f) ? -1.f : ((Value<1.f) ? Value : 1.f)); }
		static MIP_INLINE float Cos(float Value) { return cosf(Value); }
		static MIP_INLINE float Acos(float Value) { return acosf((Value<-1.f) ? -1.f : ((Value<1.f) ? Value : 1.f)); }
		static MIP_INLINE float Tan(float Value) { return tanf(Value); }
		static MIP_INLINE float Atan(float Value) { return atanf(Value); }
		static MIP_INLINE float Atan2(float Y, float X) { return atan2f(Y, X); }
		static MIP_INLINE float Sqrt(float Value) { return sqrtf(Value); }
		static MIP_INLINE float Pow(float A, float B) { return powf(A, B); }

		
		MIP_LIB void SinCos(float* ScalarSin, float* ScalarCos, float  Value);
		
		// VECTOR
		MIP_LIB VECTOR3 VectorNormalize(const VECTOR3& v);
		MIP_LIB float	VectorDot(const VECTOR3& v1, const VECTOR3& v2);
		VECTOR3 VectorCross(const VECTOR3& v1, const VECTOR3& v2);
#ifdef USE_DX9
		MIP_LIB VECTOR3 VectorTransform(const VECTOR3& v, const D3DXMATRIX& mat);
#endif
		MIP_LIB VECTOR3 VectorTransform(const VECTOR3& v, const MATRIX44& mat);
		MIP_LIB VECTOR3 VectorTransform(const VECTOR3& v, const QUATERNION& q);
		MIP_LIB VECTOR3 VectorProject(const VECTOR3& v, muint32 vport_x, muint32 vport_y, muint32 vport_cx, muint32 vport_cy, muint32 vport_minZ, muint32 vport_maxZ,
			const MATRIX44 & projection, const MATRIX44 & view, const MATRIX44 * world = NULL);
		MIP_LIB VECTOR3 VectorUnproject(const VECTOR3& v, muint32 vport_x, muint32 vport_y, muint32 vport_cx, muint32 vport_cy, muint32 vport_minZ, muint32 vport_maxZ,
			const MATRIX44 & projection, const MATRIX44 & view, const MATRIX44 * world = NULL);
		MIP_LIB VECTOR3 QuaternionToAxisAngle(const QUATERNION & q, float * radian);

		MIP_LIB VECTOR4 Vector4Transform(const VECTOR3& v, const MATRIX44& mat);
		// PLANE
#ifdef USE_DX9
		MIP_LIB PLANE PlaneTransform(PLANE& p, const D3DXMATRIX& mat);
#endif
		MIP_LIB PLANE PlaneTransform(const PLANE& p, const MATRIX44& mat);
		MIP_LIB PLANE PlaneNormalize(PLANE& p);

		// MATRIX
		MIP_LIB MATRIX44 MatrixTransposeAdjoint(const MATRIX44 & m);
		MIP_LIB MATRIX44 MatrixRotationAxis(const VECTOR3 & axis, float radian);
		MIP_LIB MATRIX44 MatrixTranspose(const MATRIX44 & m);
		MIP_LIB MATRIX44 MatrixTranslation(float x, float y, float z);
		MIP_LIB MATRIX44 MatrixScaling(float sx, float sy, float sz);
		MIP_LIB MATRIX44 MatrixRotationX(float radian);
		MIP_LIB MATRIX44 MatrixRotationY(float radian);
		MIP_LIB MATRIX44 MatrixRotationZ(float radian);
		MIP_LIB MATRIX44 MatrixLookAtLH(const VECTOR3 & eye, const VECTOR3 & at, const VECTOR3 & up);
		MIP_LIB MATRIX44 MatrixLookAtRH(const VECTOR3 & eye, const VECTOR3 & at, const VECTOR3 & up);
				
		MIP_LIB MATRIX44 MatrixTRS(const VECTOR3 & translate, float Yaw, float Pitch, float Roll, const VECTOR3 & scale);
		MIP_LIB MATRIX44 MatrixLookAt(const VECTOR3 & eye, const VECTOR3 & look, const VECTOR3 & tup);
		MIP_LIB MATRIX44 MatrixOrthoLH(float w, float h, float zn, float zf);
		MIP_LIB MATRIX44 MatrixOrthoRH(float w, float h, float zn, float zf);
		MIP_LIB MATRIX44 MatrixOrthoL(float Width, float Height, float ZScale, float ZOffset);
		MIP_LIB MATRIX44 MatrixOrthoR(float Width, float Height, float ZScale, float ZOffset);
		MIP_LIB MATRIX44 MatrixOrthoOffCenterLH(float l, float r, float b, float t, float zn, float zf);
		MIP_LIB MATRIX44 MatrixOrthoOffCenterRH(float l, float r, float b, float t, float zn, float zf);

		MIP_LIB MATRIX44 MatrixPerspectiveFovLH(float fovy, float aspect, float zn, float zf);
		MIP_LIB MATRIX44 MatrixPerspectiveFovRH(float fovy, float aspect, float zn, float zf);
		MIP_LIB MATRIX44 MatrixPerspectiveL(float HalfFOVX, float HalfFOVY, float MultFOVX, float MultFOVY, float MinZ, float MaxZ);
		MIP_LIB MATRIX44 MatrixPerspectiveL(float HalfFOV, float Width, float Height, float MinZ, float MaxZ);
		MIP_LIB MATRIX44 MatrixPerspectiveL(float HalfFOV, float Width, float Height, float MinZ);
		MIP_LIB MATRIX44 MatrixPerspectiveR(float HalfFOVX, float HalfFOVY, float MultFOVX, float MultFOVY, float MinZ, float MaxZ);
		MIP_LIB MATRIX44 MatrixPerspectiveR(float HalfFOV, float Width, float Height, float MinZ, float MaxZ);
		MIP_LIB MATRIX44 MatrixPerspectiveR(float HalfFOV, float Width, float Height, float MinZ);
		MIP_LIB MATRIX44 MatrixPerspectiveOffCenterLH(float l, float r, float b, float t, float zn, float zf);
		MIP_LIB MATRIX44 MatrixPerspectiveOffCenterRH(float l, float r, float b, float t, float zn, float zf);
		MIP_LIB MATRIX44 MatrixShadow(VECTOR4 & light, PLANE & plane);
		MIP_LIB MATRIX44 MatrixReflect(PLANE & plane);

		MIP_LIB void MatrixInverse(void* DstMatrix, const void* SrcMatrix);
		MIP_LIB void MatrixMultiply(void * Result, const void * Matrix1, const void * Matrix2);
		MIP_LIB MATRIX44 MatrixFromX(const VECTOR3 & xAxis);
		MIP_LIB MATRIX44 MatrixFromY(const VECTOR3 & yAxis);
		MIP_LIB MATRIX44 MatrixFromZ(const VECTOR3 & zAxis);
		MIP_LIB MATRIX44 MatrixFromXY(const VECTOR3 & xAxis, const VECTOR3 & yAxis);
		MIP_LIB MATRIX44 MatrixFromXZ(const VECTOR3 & xAxis, const VECTOR3 & zAxis);
		MIP_LIB MATRIX44 MatrixFromYX(const VECTOR3 & yAxis, const VECTOR3 & xAxis);
		MIP_LIB MATRIX44 MatrixFromYZ(const VECTOR3 & yAxis, const VECTOR3 & zAxis);
		MIP_LIB MATRIX44 MatrixFromZX(const VECTOR3 & zAxis, const VECTOR3 & xAxis);
		MIP_LIB MATRIX44 MatrixFromZY(const VECTOR3 & zAxis, const VECTOR3 & yAxis);
		MIP_LIB MATRIX44 MatrixAffineTransformation(float scaling, const VECTOR3 & pivot, const QUATERNION & rot, const VECTOR3 & translate);


		// QUATERNION
		MIP_LIB void QuaternionMultiply(void *Result, const void* Quat1, const void* Quat2);
		MIP_LIB QUATERNION QuaternionRotationAxis(const VECTOR3 & axis, float radian);

		// Calc A ~ B  delta Quaternion
		MIP_LIB QUATERNION CalcDelta(const QUATERNION & A, const QUATERNION & B);
		// Linear interpolation
		MIP_LIB QUATERNION Lerp(const QUATERNION & A, const QUATERNION & B, const float alpha);
		// Spherical Linear Interpolation
		MIP_LIB QUATERNION Slerp(const QUATERNION & A, const QUATERNION & B, const float alpha);
		// Bilinear interpolation
		MIP_LIB QUATERNION Bilerp(const QUATERNION & P00, const QUATERNION & P10, const QUATERNION & P01, const QUATERNION & P11, float FracX, float FracY);

		// ------------------------- ETC
		//Calc 2point Distance
		MIP_LIB float CalcDistance2D(VECTOR3 v1, VECTOR3 v2);

		//Calc 2point 3D Distance
		MIP_LIB float CalcDistance(VECTOR3 v1, VECTOR3 v2);

		MIP_LIB bool GetCrossPointPlane(mip::VECTOR3 * out, const mip::PLANE & plane, const mip::VECTOR3 & L1, const mip::VECTOR3 & L2);
	};

	MIP_INLINE float math::ClampAxis(float Angle)
	{
		// returns Angle in the range (-360,360)
		Angle = Fmod(Angle, 360.f);

		if (Angle < 0.f)
		{
			// shift to [0,360) range
			Angle += 360.f;
		}

		return Angle;
	}

	MIP_INLINE float math::NormalizeAxis(float Angle)
	{
		// returns Angle in the range [0,360)
		Angle = ClampAxis(Angle);

		if (Angle > 180.f)
		{
			// shift to (-180,180]
			Angle -= 360.f;
		}

		return Angle;
	}
};

#include "Math\MipMath.h"
