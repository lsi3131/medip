#pragma once

#include "Renderer/Renderer.h"
#include "Renderer/Effect.h"

namespace mip
{
	class ShaderFactory
	{
	public:
		static ShaderEffect* Create(eShaderType type);
	};
}
