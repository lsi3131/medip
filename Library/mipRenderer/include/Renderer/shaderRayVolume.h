#pragma once
#include "effect.h"

namespace mip
{
#ifdef USE_DX9
#else
#ifdef USE_OPENGL
	class ShaderRayVolume : public ShaderEffect
	{
	public:
		ShaderRayVolume();
		virtual ~ShaderRayVolume();

		virtual bool init();
		virtual void render();
	public:
		static const char * s_ps_rayvolume[];
		static const char * s_vs_rayvolume[];
	};
#else

#endif
#endif
};