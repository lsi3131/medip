#pragma once
#include "mip/core.h"

namespace mip
{
	class Renderer;
	class MeshCore;
	class ShaderCompute;

	class VolumeComputer
	{
	public:
		VolumeComputer();
		~VolumeComputer();
		
		void runComputer(const mint16* pInput_data, int xlen, int ylen, int zlen, VECTOR3 angle, /*out*/ std::vector<mint16> &vecOut);

	private:
		ShaderCompute*	m_pShaderCompute = nullptr;
	};
}

