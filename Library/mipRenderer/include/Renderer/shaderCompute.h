#pragma once
#include "effect.h"

namespace mip
{
	class ShaderCompute : public ShaderEffect
	{
	public:
		ShaderCompute(bool withInit = true);
		virtual ~ShaderCompute();

	public:
		virtual bool init();
		virtual void render();
	};
}

