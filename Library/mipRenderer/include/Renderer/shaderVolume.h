#pragma once
#include "effect.h"

namespace mip
{
#ifdef USE_DX9
#else
#ifdef USE_OPENGL
	class ShaderVolume : public ShaderEffect
	{
	public:
		static const char* s_ps_volume[];
		static const char* s_vs_volume[];
		static const char* s_ps_volume2[];
	public:
		ShaderVolume();
		virtual ~ShaderVolume();

	public:
		virtual bool init();
		virtual void render();
	};
#else

#endif
#endif
};