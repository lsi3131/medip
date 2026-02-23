#pragma once
#include "effect.h"

namespace mip
{
#ifdef USE_DX9
#else
#ifdef USE_OPENGL
	class ShaderLine : public ShaderEffect
	{
	public:
		ShaderLine(bool bWithInit = true);
		virtual ~ShaderLine();

	public:
		virtual bool init();
		virtual void render();
	};
#else

#endif
#endif
};