#pragma once
#include "mip/core.h"

namespace mip
{
	class COLOR;

	class FCOLOR
	{
	public:
		float	r;
		float	g;
		float	b;
		float	a;
	public:
		MIP_INLINE FCOLOR() : r(0), g(0), b(0), a(0) {}
		MIP_INLINE FCOLOR(float InR, float InG, float InB, float InA = 1.0f) : r(InR), g(InG), b(InB), a(InA) {}
		FCOLOR(COLOR & color);

		MIP_INLINE FCOLOR operator+(const FCOLOR& ColorB) const
		{
			return FCOLOR(
				this->r + ColorB.r,
				this->g + ColorB.g,
				this->b + ColorB.b,
				this->a + ColorB.a
				);
		}
		MIP_INLINE FCOLOR& operator+=(const FCOLOR& ColorB)
		{
			r += ColorB.r;
			g += ColorB.g;
			b += ColorB.b;
			a += ColorB.a;
			return *this;
		}

		MIP_INLINE FCOLOR operator-(const FCOLOR& ColorB) const
		{
			return FCOLOR(
				this->r - ColorB.r,
				this->g - ColorB.g,
				this->b - ColorB.b,
				this->a - ColorB.a
				);
		}
		MIP_INLINE FCOLOR& operator-=(const FCOLOR& ColorB)
		{
			r -= ColorB.r;
			g -= ColorB.g;
			b -= ColorB.b;
			a -= ColorB.a;
			return *this;
		}

		MIP_INLINE FCOLOR operator*(const FCOLOR& ColorB) const
		{
			return FCOLOR(
				this->r * ColorB.r,
				this->g * ColorB.g,
				this->b * ColorB.b,
				this->a * ColorB.a
				);
		}
		MIP_INLINE FCOLOR& operator*=(const FCOLOR& ColorB)
		{
			r *= ColorB.r;
			g *= ColorB.g;
			b *= ColorB.b;
			a *= ColorB.a;
			return *this;
		}

		MIP_INLINE FCOLOR operator*(float Scalar) const
		{
			return FCOLOR(
				this->r * Scalar,
				this->g * Scalar,
				this->b * Scalar,
				this->a * Scalar
				);
		}

		MIP_INLINE FCOLOR& operator*=(float Scalar)
		{
			r *= Scalar;
			g *= Scalar;
			b *= Scalar;
			a *= Scalar;
			return *this;
		}

		MIP_INLINE FCOLOR operator/(const FCOLOR& ColorB) const
		{
			return FCOLOR(
				this->r / ColorB.r,
				this->g / ColorB.g,
				this->b / ColorB.b,
				this->a / ColorB.a
				);
		}
		MIP_INLINE FCOLOR& operator/=(const FCOLOR& ColorB)
		{
			r /= ColorB.r;
			g /= ColorB.g;
			b /= ColorB.b;
			a /= ColorB.a;
			return *this;
		}

		MIP_INLINE FCOLOR operator/(float Scalar) const
		{
			const float	InvScalar = 1.0f / Scalar;
			return FCOLOR(
				this->r * InvScalar,
				this->g * InvScalar,
				this->b * InvScalar,
				this->a * InvScalar
				);
		}
		MIP_INLINE FCOLOR& operator/=(float Scalar)
		{
			const float	InvScalar = 1.0f / Scalar;
			r *= InvScalar;
			g *= InvScalar;
			b *= InvScalar;
			a *= InvScalar;
			return *this;
		}

		MIP_INLINE FCOLOR& adjustContrast(const FCOLOR & color, float s)
		{
			r = 0.5f + s * (color.r - 0.5f);
			g = 0.5f + s * (color.g - 0.5f);
			b = 0.5f + s * (color.b - 0.5f);
			a = color.a;
			return *this;
		}


		MIP_INLINE FCOLOR& adjustSaturation(const FCOLOR & color, float s)
		{
			float grey = color.r * 0.2125f + color.g * 0.7154f + color.b * 0.0721f;
			r = grey + s * (color.r - grey);
			g = grey + s * (color.g - grey);
			b = grey + s * (color.b - grey);
			a = color.a;
			return *this;
		}

		// clamped in 0..1 range
		FCOLOR GetClamped(float InMin = 0.0f, float InMax = 1.0f) const;
		
		/** Comparison operators */
		MIP_INLINE bool operator==(const FCOLOR& ColorB) const
		{
			return this->r == ColorB.r && this->g == ColorB.g && this->b == ColorB.b && this->a == ColorB.a;
		}
		MIP_INLINE bool operator!=(const FCOLOR& Other) const
		{
			return this->r != Other.r || this->g != Other.g || this->b != Other.b || this->a != Other.a;
		}

	public:
		static FCOLOR GetHSV(muint8 H, muint8 S, muint8 V);
		static FCOLOR GetRandomColor();
		static FCOLOR COLOR2HSV(float R, float G, float B, float A);
		static FCOLOR COLOR2HSV(FCOLOR & color);
		static FCOLOR HSV2COLOR(float H, float U, float V);

	public:
		static const FCOLOR White;
		static const FCOLOR Gray;
		static const FCOLOR Black;
		static const FCOLOR Transparent;
		static const FCOLOR Red;
		static const FCOLOR Green;
		static const FCOLOR Blue;
		static const FCOLOR Yellow;
	};
	
	class COLOR
	{
	public:
		union {
			struct{ muint8 b, g, r, a; };
			muint32 allcolor;
			muint8 c[4];
		};
	public:

		COLOR() { r = g = b = a = 0;  };
		COLOR(muint8 InR, muint8 InG, muint8 InB, muint8 InA = 255)
		{
			// put these into the body for proper ordering with INTEL vs non-INTEL_BYTE_ORDER
			r = InR;
			g = InG;
			b = InB;
			a = InA;
		}
		COLOR(const FCOLOR& C);
		COLOR(muint32 c);
		DWORD getDW() const { return allcolor; }
		FCOLOR getFColor() const { 	return FCOLOR(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f); 	}
		void set(const DWORD InColor)
		{
			allcolor = InColor;
		}

		// Operators.
		MIP_INLINE bool operator==(const COLOR &C) const
		{
			return allcolor == C.getDW();
		}

		MIP_INLINE bool operator!=(const COLOR& C) const
		{
			return allcolor != C.getDW();
		}

		void operator+=(const COLOR& C);
		
	public:
		static const COLOR White;
		static const COLOR Gray;
		static const COLOR Black;
		static const COLOR Transparent;
		static const COLOR Red;
		static const COLOR Green;
		static const COLOR Blue;
		static const COLOR Yellow;
		static const COLOR Cyan;
		static const COLOR Magenta;
	};
};