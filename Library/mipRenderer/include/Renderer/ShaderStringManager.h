#pragma once

#include <string>
#include "mip/core.h"

namespace mip
{
	class ShaderStringManager
	{
	public:
		ShaderStringManager();
		~ShaderStringManager() {};

		std::string Get_Shader(const muint32 nShaderType, const muint32 unType);

		std::string pong(int nType);
		std::string pong2(int nType);
		std::string pong_f(int nType);
		std::string ball(int nType);
		std::string shadow(int nType);
		std::string _3mf(int nType);
		std::string basic(int nType);
		std::string materialCapture(int nType);
		std::string select(int nType);
		std::string xray(int nType);
		std::string cp_matching_brush(int nType);
	public:
		std::string strVsMesh;
		std::string strPsMesh;
	};
};
