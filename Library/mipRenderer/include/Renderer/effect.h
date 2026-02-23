#pragma once
#include "math/math.h"

namespace mip
{
	class Renderer;

#ifdef USE_DX9
#else
#ifdef USE_OPENGL

	enum SHADERTYPE
	{
		SHADER_PONG = 0,
		SHADER_BALL,
		SHADER_SHADOW,
		SHADER_3MF,
		SHADER_BASIC,
		SHADER_PONG_F,
		SHADER_SELECT,
		SHADER_X_RAY,
		SHADER_MATCAPS,
		CS_SHADER_MATCHING, // compute Shader
		SHADER_MAX,
	};

	class ShaderEffect
	{
	public:
		ShaderEffect();
		virtual ~ShaderEffect();

	public:
		virtual bool init() = 0;
		virtual void render() = 0;

	public:
		void updatePoints(std::vector<mip::VECTOR3>& pointList);
		void updatePoints2(std::vector<mip::VECTOR2>& pointList);
		void updatePoints3(std::vector<mip::VECTOR3>& pointList);

		bool beginShader(Renderer* renderer, muint32 progHandle = 0);
		void endShader(Renderer* renderer);

		bool beginRender();
		void endRender();

		muint32 getProgramHandle();
		muint32 getVAOHandle();

	protected:
		bool initShader(
			unsigned int* pOutShader_VS, const std::string& shaderCode_VS, 
			unsigned int* pOutShader_PS, const std::string& shaderCode_PS
		);
		bool initShaderPS(unsigned int* pOutShader, const std::string& shaderCode);
		bool initShaderVS(unsigned int* pOutShader, const std::string& shaderCode);
		bool initShaderCompute(unsigned int* pOutShader, const std::string& shaderCode);
		bool initShaderWithCode(unsigned int shader, const std::string& shaderCode);

	protected:
		muint32 m_shaderProgram;
		muint32 m_shaderProgArr[SHADER_MAX];

		muint32 m_RenderBufferPoint;
		muint32 m_RenderBufferPoint2;
		muint32 m_RenderBufferPoint3;

		muint32 m_RenderVAO;
		muint32 m_RenderMax;
		muint32 m_RenderCount;
	};
#else
	class ShaderEffect
	{
	public:
		ShaderEffect(ID3D11Device* device, const std::wstring& filename);
		virtual ~ShaderEffect();

	private:
		ShaderEffect(const ShaderEffect& rhs);
		ShaderEffect& operator=(const ShaderEffect& rhs);

	protected:
		ID3DX11Effect* m_pEffect;
	};



	class RAINBOW_EFFECT : public ShaderEffect
	{
	public:
		RAINBOW_EFFECT(ID3D11Device* device, const std::wstring& filename);
		~RAINBOW_EFFECT();

		void setWorldViewProj(mip::MATRIX44& M) { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
		void setWorld(mip::MATRIX44& M) { World->SetMatrix(reinterpret_cast<const float*>(&M)); }
		// void SetWorldInvTranspose(mip::MATRIX44 & M)    { WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
		// void SetTexTransform(mip::MATRIX44 & M)         { TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
		// void SetEyePosW(const mip::VECTOR3& v)          { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
		// void SetDirLights(const DirectionalLight* lights)   { DirLights->SetRawValue(lights, 0, 3 * sizeof(DirectionalLight)); }
		// void SetMaterial(const Material& mat)               { Mat->SetRawValue(&mat, 0, sizeof(Material)); }
		void SetDiffuseMap(ID3D11ShaderResourceView* tex) { DiffuseMap->SetResource(tex); }

		ID3DX11EffectTechnique* Light1Tech;
		ID3DX11EffectTechnique* Light2Tech;
		ID3DX11EffectTechnique* Light3Tech;

		ID3DX11EffectTechnique* Light0TexTech;
		ID3DX11EffectTechnique* Light1TexTech;
		ID3DX11EffectTechnique* Light2TexTech;
		ID3DX11EffectTechnique* Light3TexTech;

		ID3DX11EffectMatrixVariable* WorldViewProj;
		ID3DX11EffectMatrixVariable* World;
		ID3DX11EffectMatrixVariable* View;
		ID3DX11EffectMatrixVariable* Project;

		ID3DX11EffectVectorVariable* EyePosW;
		ID3DX11EffectVariable* DirLights;
		ID3DX11EffectVariable* Mat;

		ID3DX11EffectShaderResourceVariable* DiffuseMap;
	};


	class EFFECTLIST
	{
	public:
		static void InitAll(ID3D11Device* device);
		static void DestroyAll();

		static RAINBOW_EFFECT* BasicFX;
	};
#endif
#endif
};