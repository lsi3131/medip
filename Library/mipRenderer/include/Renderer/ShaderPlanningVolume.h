#pragma once
#include "effect.h"

namespace mip
{
#ifdef USE_DX9
#else
#ifdef USE_OPENGL
	class ShaderPlanningVolume : public ShaderEffect
	{
	public:
		ShaderPlanningVolume(bool withInit = true);
		virtual ~ShaderPlanningVolume();

	public:
		virtual bool init() override;
		virtual void render() override;
	};
#else

#endif
#endif
};
