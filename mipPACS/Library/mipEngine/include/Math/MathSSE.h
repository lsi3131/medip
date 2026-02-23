#pragma once
#include "mip/core.h"
//require SSE2
#include <emmintrin.h>

typedef __m128	VectorRegister;
typedef __m128i VectorRegisterInt;

// for an __m128, we need a single set of braces (for clang)
#define DECLARE_VECTOR_REGISTER(X, Y, Z, W) { X, Y, Z, W }

/**
* @param A0	Selects which element (0-3) from 'A' into 1st slot in the result
* @param A1	Selects which element (0-3) from 'A' into 2nd slot in the result
* @param B2	Selects which element (0-3) from 'B' into 3rd slot in the result
* @param B3	Selects which element (0-3) from 'B' into 4th slot in the result
*/
#define SHUFFLEMASK(A0,A1,B2,B3) ( (A0) | ((A1)<<2) | ((B2)<<4) | ((B3)<<6) )

namespace mip
{
	namespace sse
	{
		/**
		* Returns a bitwise equivalent vector based on 4 DWORDs.
		*
		* @param X		1st muint32 component
		* @param Y		2nd muint32 component
		* @param Z		3rd muint32 component
		* @param W		4th muint32 component
		* @return		Bitwise equivalent vector with 4 floats
		*/
		MIP_INLINE VectorRegister MakeVectorRegister(muint32 X, muint32 Y, muint32 Z, muint32 W)
		{
			union { VectorRegister v; VectorRegisterInt i; } Tmp;
			Tmp.i = _mm_setr_epi32(X, Y, Z, W);
			return Tmp.v;
		}

		/**
		* Returns a vector based on 4 FLOATs.
		*
		* @param X		1st float component
		* @param Y		2nd float component
		* @param Z		3rd float component
		* @param W		4th float component
		* @return		Vector of the 4 FLOATs
		*/
		MIP_INLINE VectorRegister MakeVectorRegister(float X, float Y, float Z, float W)
		{
			return _mm_setr_ps(X, Y, Z, W);
		}

		static const VectorRegister FloatOne = MakeVectorRegister(1.0f, 1.0f, 1.0f, 1.0f);
		static const VectorRegister FloatZero = MakeVectorRegister(0.0f, 0.0f, 0.0f, 0.0f);
		static const VectorRegister FloatMinusOne = MakeVectorRegister(-1.0f, -1.0f, -1.0f, -1.0f);
		static const VectorRegister Float0001 = MakeVectorRegister(0.0f, 0.0f, 0.0f, 1.0f);
		static const VectorRegister SmallLengthThreshold = MakeVectorRegister(1.e-8f, 1.e-8f, 1.e-8f, 1.e-8f);
		static const VectorRegister FloatOneHundredth = MakeVectorRegister(0.01f, 0.01f, 0.01f, 0.01f);
		static const VectorRegister Float111_Minus1 = MakeVectorRegister(1.f, 1.f, 1.f, -1.f);
		static const VectorRegister FloatMinus1_111 = MakeVectorRegister(-1.f, 1.f, 1.f, 1.f);
		static const VectorRegister FloatOneHalf = MakeVectorRegister(0.5f, 0.5f, 0.5f, 0.5f);
		static const VectorRegister KindaSmallNumber = MakeVectorRegister(MIP_EPSILON, MIP_EPSILON, MIP_EPSILON, MIP_EPSILON);
		static const VectorRegister SmallNumber = MakeVectorRegister(MIP_EPSILON, MIP_EPSILON, MIP_EPSILON, MIP_EPSILON);

		/** This is to speed up Quaternion Inverse. Static variable to keep sign of inverse **/
		static const VectorRegister QINV_SIGN_MASK = MakeVectorRegister(-1.f, -1.f, -1.f, 1.f);

		static const VectorRegister QMULTI_SIGN_MASK0 = MakeVectorRegister(1.f, -1.f, 1.f, -1.f);
		static const VectorRegister QMULTI_SIGN_MASK1 = MakeVectorRegister(1.f, 1.f, -1.f, -1.f);
		static const VectorRegister QMULTI_SIGN_MASK2 = MakeVectorRegister(-1.f, 1.f, 1.f, -1.f);

		static const VectorRegister DEG_TO_RAD = MakeVectorRegister(MIP_PI / (180.f), MIP_PI / (180.f), MIP_PI / (180.f), 0.0f);
		static const VectorRegister DEG_TO_RAD_HALF = MakeVectorRegister((MIP_PI / 180.f)*0.5f, (MIP_PI / 180.f)*0.5f, (MIP_PI / 180.f)*0.5f, 0.0f);
		static const VectorRegister RAD_TO_DEG = MakeVectorRegister((180.f) / MIP_PI, (180.f) / MIP_PI, (180.f) / MIP_PI, 0.0f);

		/** Bitmask to AND out the XYZ components in a vector */
		static const VectorRegister XYZMask = MakeVectorRegister((muint32)0xffffffff, (muint32)0xffffffff, (muint32)0xffffffff, (muint32)0x00000000);

		/** Bitmask to AND out the sign bit of each components in a vector */
#define SIGN_BIT ((1 << 31))
		static const VectorRegister SignBit = MakeVectorRegister((muint32)SIGN_BIT, (muint32)SIGN_BIT, (muint32)SIGN_BIT, (muint32)SIGN_BIT);
		static const VectorRegister SignMask = MakeVectorRegister((muint32)(~SIGN_BIT), (muint32)(~SIGN_BIT), (muint32)(~SIGN_BIT), (muint32)(~SIGN_BIT));
#undef SIGN_BIT

		/** Vector full of positive infinity */
		static const VectorRegister FloatInfinity = MakeVectorRegister((muint32)0x7F800000, (muint32)0x7F800000, (muint32)0x7F800000, (muint32)0x7F800000);


		static const VectorRegister Pi = MakeVectorRegister(MIP_PI, MIP_PI, MIP_PI, MIP_PI);
		static const VectorRegister TwoPi = MakeVectorRegister(2.0f*MIP_PI, 2.0f*MIP_PI, 2.0f*MIP_PI, 2.0f*MIP_PI);
		static const VectorRegister PiByTwo = MakeVectorRegister(0.5f*MIP_PI, 0.5f*MIP_PI, 0.5f*MIP_PI, 0.5f*MIP_PI);
		static const VectorRegister PiByFour = MakeVectorRegister(0.25f*MIP_PI, 0.25f*MIP_PI, 0.25f*MIP_PI, 0.25f*MIP_PI);
		static const VectorRegister OneOverPi = MakeVectorRegister(1.0f / MIP_PI, 1.0f / MIP_PI, 1.0f / MIP_PI, 1.0f / MIP_PI);
		static const VectorRegister OneOverTwoPi = MakeVectorRegister(1.0f / (2.0f*MIP_PI), 1.0f / (2.0f*MIP_PI), 1.0f / (2.0f*MIP_PI), 1.0f / (2.0f*MIP_PI));

		static const VectorRegister Float255 = MakeVectorRegister(255.0f, 255.0f, 255.0f, 255.0f);



		/**
		* Returns a vector with all zeros.
		*
		* @return		VectorRegister(0.0f, 0.0f, 0.0f, 0.0f)
		*/
#define VectorZero()					_mm_setzero_ps()

		/**
		* Returns a vector with all ones.
		*
		* @return		VectorRegister(1.0f, 1.0f, 1.0f, 1.0f)
		*/
#define VectorOne()						(GlobalVectorConstants::FloatOne)

		/**
		* Returns an component from a vector.
		*
		* @param Vec				Vector register
		* @param ComponentIndex	Which component to get, X=0, Y=1, Z=2, W=3
		* @return					The component as a float
		*/
		MIP_INLINE float VectorGetComponent(VectorRegister Vec, muint32 ComponentIndex)
		{
			return (((float*)&(Vec))[ComponentIndex]);
		}

		/**
		* Loads 4 FLOATs from unaligned memory.
		*
		* @param Ptr	Unaligned memory pointer to the 4 FLOATs
		* @return		VectorRegister(Ptr[0], Ptr[1], Ptr[2], Ptr[3])
		*/
#define VectorLoad( Ptr )				_mm_loadu_ps( (float*)(Ptr) )

		/**
		* Loads 3 FLOATs from unaligned memory and leaves W undefined.
		*
		* @param Ptr	Unaligned memory pointer to the 3 FLOATs
		* @return		VectorRegister(Ptr[0], Ptr[1], Ptr[2], undefined)
		*/
#define VectorLoadFloat3( Ptr )			MakeVectorRegister( ((const float*)(Ptr))[0], ((const float*)(Ptr))[1], ((const float*)(Ptr))[2], 0.0f )

		/**
		* Loads 3 FLOATs from unaligned memory and sets W=0.
		*
		* @param Ptr	Unaligned memory pointer to the 3 FLOATs
		* @return		VectorRegister(Ptr[0], Ptr[1], Ptr[2], 0.0f)
		*/
#define VectorLoadFloat3_W0( Ptr )		MakeVectorRegister( ((const float*)(Ptr))[0], ((const float*)(Ptr))[1], ((const float*)(Ptr))[2], 0.0f )

		/**
		* Loads 3 FLOATs from unaligned memory and sets W=1.
		*
		* @param Ptr	Unaligned memory pointer to the 3 FLOATs
		* @return		VectorRegister(Ptr[0], Ptr[1], Ptr[2], 1.0f)
		*/
#define VectorLoadFloat3_W1( Ptr )		MakeVectorRegister( ((const float*)(Ptr))[0], ((const float*)(Ptr))[1], ((const float*)(Ptr))[2], 1.0f )

		/**
		* Loads 4 FLOATs from aligned memory.
		*
		* @param Ptr	Aligned memory pointer to the 4 FLOATs
		* @return		VectorRegister(Ptr[0], Ptr[1], Ptr[2], Ptr[3])
		*/
#define VectorLoadAligned( Ptr )		_mm_load_ps( (float*)(Ptr) )

		/**
		* Loads 1 float from unaligned memory and replicates it to all 4 elements.
		*
		* @param Ptr	Unaligned memory pointer to the float
		* @return		VectorRegister(Ptr[0], Ptr[0], Ptr[0], Ptr[0])
		*/
#define VectorLoadFloat1( Ptr )			_mm_load1_ps( (float*)(Ptr) )

		/**
		* Creates a vector out of three FLOATs and leaves W undefined.
		*
		* @param X		1st float component
		* @param Y		2nd float component
		* @param Z		3rd float component
		* @return		VectorRegister(X, Y, Z, undefined)
		*/
#define VectorSetFloat3( X, Y, Z )		MakeVectorRegister( X, Y, Z, 0.0f )

		/**
		* Propagates passed in float to all registers
		*
		* @param F		Float to set
		* @return		VectorRegister(F,F,F,F)
		*/
#define VectorSetFloat1( F )	_mm_set1_ps( F )


		/**
		* Creates a vector out of four FLOATs.
		*
		* @param X		1st float component
		* @param Y		2nd float component
		* @param Z		3rd float component
		* @param W		4th float component
		* @return		VectorRegister(X, Y, Z, W)
		*/
#define VectorSet( X, Y, Z, W )			MakeVectorRegister( X, Y, Z, W )

		/**
		* Stores a vector to aligned memory.
		*
		* @param Vec	Vector to store
		* @param Ptr	Aligned memory pointer
		*/
#define VectorStoreAligned( Vec, Ptr )	_mm_store_ps( (float*)(Ptr), Vec )


		/**
		* Performs non-temporal store of a vector to aligned memory without polluting the caches
		*
		* @param Vec	Vector to store
		* @param Ptr	Aligned memory pointer
		*/
#define VectorStoreAlignedStreamed( Vec, Ptr )	_mm_stream_ps( (float*)(Ptr), Vec )


		/**
		* Stores a vector to memory (aligned or unaligned).
		*
		* @param Vec	Vector to store
		* @param Ptr	Memory pointer
		*/
#define VectorStore( Vec, Ptr )			_mm_storeu_ps( (float*)(Ptr), Vec )

		/**
		* Returns the minimum values of two vectors (component-wise).
		*
		* @param Vec1	1st vector
		* @param Vec2	2nd vector
		* @return		VectorRegister( min(Vec1.x,Vec2.x), min(Vec1.y,Vec2.y), min(Vec1.z,Vec2.z), min(Vec1.w,Vec2.w) )
		*/
#define VectorMin( Vec1, Vec2 )			_mm_min_ps( Vec1, Vec2 )

		/**
		* Returns the maximum values of two vectors (component-wise).
		*
		* @param Vec1	1st vector
		* @param Vec2	2nd vector
		* @return		VectorRegister( max(Vec1.x,Vec2.x), max(Vec1.y,Vec2.y), max(Vec1.z,Vec2.z), max(Vec1.w,Vec2.w) )
		*/
#define VectorMax( Vec1, Vec2 )			_mm_max_ps( Vec1, Vec2 )

		/**
		* Swizzles the 4 components of a vector and returns the result.
		*
		* @param Vec		Source vector
		* @param X			Index for which component to use for X (literal 0-3)
		* @param Y			Index for which component to use for Y (literal 0-3)
		* @param Z			Index for which component to use for Z (literal 0-3)
		* @param W			Index for which component to use for W (literal 0-3)
		* @return			The swizzled vector
		*/
#define VectorSwizzle( Vec, X, Y, Z, W )	_mm_shuffle_ps( Vec, Vec, SHUFFLEMASK(X,Y,Z,W) )

		/**
		* Creates a vector through selecting two components from each vector via a shuffle mask.
		*
		* @param Vec1		Source vector1
		* @param Vec2		Source vector2
		* @param X			Index for which component of Vector1 to use for X (literal 0-3)
		* @param Y			Index for which component to Vector1 to use for Y (literal 0-3)
		* @param Z			Index for which component to Vector2 to use for Z (literal 0-3)
		* @param W			Index for which component to Vector2 to use for W (literal 0-3)
		* @return			The swizzled vector
		*/
#define VectorShuffle( Vec1, Vec2, X, Y, Z, W )	_mm_shuffle_ps( Vec1, Vec2, SHUFFLEMASK(X,Y,Z,W) )

		/**
		* Replicates one element into all four elements and returns the new vector.
		*
		* @param Vec			Source vector
		* @param ElementIndex	Index (0-3) of the element to replicate
		* @return				VectorRegister( Vec[ElementIndex], Vec[ElementIndex], Vec[ElementIndex], Vec[ElementIndex] )
		*/
#define VectorReplicate( Vec, ElementIndex )	_mm_shuffle_ps( Vec, Vec, SHUFFLEMASK(ElementIndex,ElementIndex,ElementIndex,ElementIndex) )
			/**
			* Returns the absolute value(component - wise).
			*
			* @param Vec			Source vector
			* @return				VectorRegister(abs(Vec.x), abs(Vec.y), abs(Vec.z), abs(Vec.w))
			*/
#define VectorAbs( Vec )				_mm_and_ps(Vec, GlobalVectorConstants::SignMask)

			/**
			* Returns the negated value (component-wise).
			*
			* @param Vec			Source vector
			* @return				VectorRegister( -Vec.x, -Vec.y, -Vec.z, -Vec.w )
			*/
#define VectorNegate( Vec )				_mm_sub_ps(_mm_setzero_ps(),Vec)

			/**
			* Adds two vectors (component-wise) and returns the result.
			*
			* @param Vec1	1st vector
			* @param Vec2	2nd vector
			* @return		VectorRegister( Vec1.x+Vec2.x, Vec1.y+Vec2.y, Vec1.z+Vec2.z, Vec1.w+Vec2.w )
			*/
#define VectorAdd( Vec1, Vec2 )			_mm_add_ps( Vec1, Vec2 )

			/**
			* Subtracts a vector from another (component-wise) and returns the result.
			*
			* @param Vec1	1st vector
			* @param Vec2	2nd vector
			* @return		VectorRegister( Vec1.x-Vec2.x, Vec1.y-Vec2.y, Vec1.z-Vec2.z, Vec1.w-Vec2.w )
			*/
#define VectorSubtract( Vec1, Vec2 )	_mm_sub_ps( Vec1, Vec2 )

		/**
		* Multiplies two vectors (component-wise) and returns the result.
		*
		* @param Vec1	1st vector
		* @param Vec2	2nd vector
		* @return		VectorRegister( Vec1.x*Vec2.x, Vec1.y*Vec2.y, Vec1.z*Vec2.z, Vec1.w*Vec2.w )
		*/
#define VectorMultiply( Vec1, Vec2 )	_mm_mul_ps( Vec1, Vec2 )

		/**
		* Multiplies two vectors (component-wise), adds in the third vector and returns the result.
		*
		* @param Vec1	1st vector
		* @param Vec2	2nd vector
		* @param Vec3	3rd vector
		* @return		VectorRegister( Vec1.x*Vec2.x + Vec3.x, Vec1.y*Vec2.y + Vec3.y, Vec1.z*Vec2.z + Vec3.z, Vec1.w*Vec2.w + Vec3.w )
		*/
#define VectorMultiplyAdd( Vec1, Vec2, Vec3 )	_mm_add_ps( _mm_mul_ps(Vec1, Vec2), Vec3 )

		/**
		* Calculates the dot3 product of two vectors and returns a vector with the result in all 4 components.
		* Only really efficient on Xbox 360.
		*
		* @param Vec1	1st vector
		* @param Vec2	2nd vector
		* @return		d = dot3(Vec1.xyz, Vec2.xyz), VectorRegister( d, d, d, d )
		*/
		MIP_INLINE VectorRegister VectorDot3(const VectorRegister& Vec1, const VectorRegister& Vec2)
		{
			VectorRegister Temp = VectorMultiply(Vec1, Vec2);
			return VectorAdd(VectorReplicate(Temp, 0), VectorAdd(VectorReplicate(Temp, 1), VectorReplicate(Temp, 2)));
		}

		/**
		* Calculates the dot4 product of two vectors and returns a vector with the result in all 4 components.
		* Only really efficient on Xbox 360.
		*
		* @param Vec1	1st vector
		* @param Vec2	2nd vector
		* @return		d = dot4(Vec1.xyzw, Vec2.xyzw), VectorRegister( d, d, d, d )
		*/
		VectorRegister VectorDot4(const VectorRegister& Vec1, const VectorRegister& Vec2);

		VectorRegister VectorQuaternionMultiply2(const VectorRegister& Quat1, const VectorRegister& Quat2);
		/**
		* Multiplies two quaternions; the order matters.
		*
		* When composing quaternions: VectorQuaternionMultiply(C, A, B) will yield a quaternion C = A * B
		* that logically first applies B then A to any subsequent transformation (right first, then left).
		*
		* @param Result	Pointer to where the result Quat1 * Quat2 should be stored
		* @param Quat1	Pointer to the first quaternion (must not be the destination)
		* @param Quat2	Pointer to the second quaternion (must not be the destination)
		*/
		void VectorQuaternionMultiply(void* __restrict Result, const void* __restrict Quat1, const void* __restrict Quat2);
	};



	/**
	* Calculates the dot4 product of two vectors and returns a vector with the result in all 4 components.
	* Only really efficient on Xbox 360.
	*
	* @param Vec1	1st vector
	* @param Vec2	2nd vector
	* @return		d = dot4(Vec1.xyzw, Vec2.xyzw), VectorRegister( d, d, d, d )
	*/
	MIP_INLINE VectorRegister sse::VectorDot4(const VectorRegister& Vec1, const VectorRegister& Vec2)
	{
		VectorRegister Temp1, Temp2;
		Temp1 = VectorMultiply(Vec1, Vec2);
		Temp2 = _mm_shuffle_ps(Temp1, Temp1, SHUFFLEMASK(2, 3, 0, 1));	// (Z,W,X,Y).
		Temp1 = VectorAdd(Temp1, Temp2);								// (X*X + Z*Z, Y*Y + W*W, Z*Z + X*X, W*W + Y*Y)
		Temp2 = _mm_shuffle_ps(Temp1, Temp1, SHUFFLEMASK(1, 2, 3, 0));	// Rotate left 4 bytes (Y,Z,W,X).
		return VectorAdd(Temp1, Temp2);								// (X*X + Z*Z + Y*Y + W*W, Y*Y + W*W + Z*Z + X*X, Z*Z + X*X + W*W + Y*Y, W*W + Y*Y + X*X + Z*Z)
	}

	MIP_INLINE VectorRegister sse::VectorQuaternionMultiply2(const VectorRegister& Quat1, const VectorRegister& Quat2)
	{
		VectorRegister Result = VectorMultiply(VectorReplicate(Quat1, 3), Quat2);
		Result = VectorMultiplyAdd(VectorMultiply(VectorReplicate(Quat1, 0), VectorSwizzle(Quat2, 3, 2, 1, 0)), QMULTI_SIGN_MASK0, Result);
		Result = VectorMultiplyAdd(VectorMultiply(VectorReplicate(Quat1, 1), VectorSwizzle(Quat2, 2, 3, 0, 1)), QMULTI_SIGN_MASK1, Result);
		Result = VectorMultiplyAdd(VectorMultiply(VectorReplicate(Quat1, 2), VectorSwizzle(Quat2, 1, 0, 3, 2)), QMULTI_SIGN_MASK2, Result);

		return Result;
	}

	MIP_INLINE void sse::VectorQuaternionMultiply(void* __restrict Result, const void* __restrict Quat1, const void* __restrict Quat2)
	{
		*((VectorRegister *)Result) = VectorQuaternionMultiply2(*((const VectorRegister *)Quat1), *((const VectorRegister *)Quat2));
	}

};