#pragma once
#include <map>
#include "effect.h"

namespace mip
{
#ifdef USE_DX9
#else
#ifdef USE_OPENGL
	class COLOR;
	class ShaderStringManager;
	class ShaderMeshData;
	class ShaderCommon;

	class ShaderMesh : public ShaderEffect
	{
	public:
		ShaderMesh();
		ShaderMesh(ShaderStringManager* pShaderString);
		virtual ~ShaderMesh();

	public:
		virtual bool init() override;
		virtual void render() override;

	public:
		muint32 initShader(int shaderType = mip::SHADERTYPE::SHADER_PONG);
		muint32 initCompShader(int shaderType = mip::SHADERTYPE::CS_SHADER_MATCHING);
		int Compile_Shader(const muint32 nShaderType, const muint32 unType);
		int getShaderProg(int shaderType);
		int getTextureCnt();

		int Add_Uniform(muint32 _shaderProgram, const std::string& _strUniform);
		int Add_Attribute(muint32 _shaderProgram, const std::string& _strAttribute);
		int Add_TextureID(muint32 _textureId, const std::string& _strUniform);

		int Update_UniformMat4fv(muint32 _shaderProgram, const float* value, const std::string& name);
		int Update_Uniform4fv(muint32 _shaderProgram, const float* value, const std::string& name);
		int Update_Uniform3fv(muint32 _shaderProgram, const float* value, const std::string& name);
		int Update_Uniform1fv(muint32 _shaderProgram, const float* value, const std::string& name);
		int Update_Uniform1iv(muint32 _shaderProgram, const int* value, const std::string& name);
		int Update_Uniform1i(muint32 _shaderProgram, const std::string& name, const int value = 0);

		int updateComponent(muint32 _shaderProgram, ShaderCommon& _ShaderCommon, ShaderMeshData& _ShaderMeshData, int shaderMode = mip::SHADERTYPE::SHADER_PONG);
		int updateComp_Pong(muint32 _shaderProgram, mip::MATRIX44* matWorld, const mip::MATRIX44& view, const mip::MATRIX44& proj, const mip::VECTOR4* color, float wireFrame);
		int updateComp_Pong_f(muint32 _shaderProgram, ShaderCommon& _shaderCommon, ShaderMeshData& _shaderMeshData, const mip::VECTOR4* color);
		int updateComp_Ball(muint32 _shaderProgram, mip::MATRIX44* matWorld, const mip::MATRIX44& view, const mip::MATRIX44& proj, const mip::VECTOR4* color);
		int updateComp_Shadow(muint32 _shaderProgram, ShaderCommon& _shaderCommon, ShaderMeshData& _shaderMeshData, const mip::VECTOR4* color,
			const mip::VECTOR3& pick, const float radius, const int screenHeight, const int screenWidth);
		int updateComp_3mf(muint32 _shaderProgram, ShaderCommon& _shaderCommon, ShaderMeshData& _shaderMeshData, const mip::VECTOR4* color);
		int updateComp_basic(muint32 _shaderProgram, mip::MATRIX44* matWorld, const mip::MATRIX44& view, const mip::MATRIX44& proj, const mip::VECTOR4* color);
		int updateComp_Select(muint32 _shaderProgram, ShaderCommon& _shaderCommon, ShaderMeshData& _shaderMeshData, const mip::VECTOR4* color);
		int updateComp_MatCaps(muint32 _shaderProgram, ShaderCommon& _shaderCommon, ShaderMeshData& _shaderMeshData, const mip::VECTOR4* color);
		int updateComp_Xray(muint32 _shaderProgram, ShaderCommon& _shaderCommon, ShaderMeshData& _shaderMeshData, const mip::VECTOR4* color);
		int runCompute_Matching(std::vector<mip::VECTOR4>& Tverts, std::vector<mip::VECTOR3>& verts, float min, float max, std::vector<std::vector<muint32>>& out_dimArrVertIdx);

		int initComponent(muint32 _shaderProgram, int shaderMode = mip::SHADERTYPE::SHADER_PONG);
		int initComp_Pong(muint32 _shaderProgram);
		int initComp_Pong_f(muint32 _shaderProgram);
		int initComp_Ball(muint32 _shaderProgram);
		int initComp_Shadow(muint32 _shaderProgram);
		int initComp_3mf(muint32 _shaderProgram);
		int initComp_basic(muint32 _shaderProgram);
		int initComp_matCaps(muint32 _shaderProgram);
		int initComp_Select(muint32 _shaderProgram);
		int initComp_Xray(muint32 _shaderProgram);
		int initComp_CS_Matching(muint32 _shaderProgram);
	private:
		int Get_Attribute(muint32 _shaderProgram, const std::string& _strKey);
		int Get_Uniform(muint32 _shaderProgram, const std::string& _strKey);
		int Get_Texture(std::map<std::string, int>& map, const std::string& _strKey);

	public:
		std::string vs_mesh_glsl;
		std::string ps_mesh_glsl;

		ShaderStringManager* m_pShaderStrMng;

	private:
		std::map<std::string, int> m_mapAttributeList[SHADER_MAX];
		std::map<std::string, int> m_mapUniformList[SHADER_MAX];
		std::map<std::string, int> m_mapTextureList;
	};
#else

#endif
#endif
};