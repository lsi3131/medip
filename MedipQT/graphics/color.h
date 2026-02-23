#pragma once

#ifndef COLOR_H
#define COLOR_H

#include "define.h"

class COLOR
{
public:
	union {
		struct { muint8 r, g, b, a; };
		muint32 allcolor;
	};

public:

	COLOR() { r = g = b = a = 0; };
	COLOR(muint8 InR, muint8 InG, muint8 InB, muint8 InA = 255)
	{
		// put these into the body for proper ordering with INTEL vs non-INTEL_BYTE_ORDER
		r = InR;
		g = InG;
		b = InB;
		a = InA;
	}
	COLOR(muint32 c) { allcolor = c;  }
	muint32 getData() const { return allcolor; }
	void set(const muint32 InColor)
	{
		allcolor = InColor;
	}

	// Operators.
	bool operator==(const COLOR &C) const
	{
		return allcolor == C.getData();
	}

	bool operator!=(const COLOR& C) const
	{
		return allcolor != C.getData();
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
#endif
