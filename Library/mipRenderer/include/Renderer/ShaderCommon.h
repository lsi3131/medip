#pragma once

#include <map>
#include "effect.h"

#ifdef USE_DX9
#else
#ifdef USE_OPENGL

#define MAX_TEXTURE_SAMPLE2D_CNT 6

namespace mip
{
	class ShaderCommon
	{
	public:
		ShaderCommon() :
			matWorld(mip::MATRIX44::Identity), matView(mip::MATRIX44::Identity), matProj(mip::MATRIX44::Identity),
			bDepthTestMode(true), bBackFace(false), bAlpha(false), fAlpha(255.0f), color(mip::VECTOR4(0, 0, 0, 1)) {};

		void ShaderCommon::init(mip::MATRIX44& _W, mip::MATRIX44& _V, mip::MATRIX44& _P, mip::VECTOR4& _color
			, bool _bDepthTestMode, bool _bBackFace, bool _bAlpha, float _fAlpha)
		{
			this->matWorld = _W;
			this->matView = _V;
			this->matProj = _P;
			this->bDepthTestMode = _bDepthTestMode;
			this->bBackFace = _bBackFace;
			this->bAlpha = _bAlpha;
			this->fAlpha = _fAlpha;
			this->color = _color;
		};

		void ShaderCommon::set(mip::MATRIX44& _W, mip::MATRIX44& _V, mip::MATRIX44& _P, mip::VECTOR4& _color)
		{
			this->matWorld = _W;
			this->matView = _V;
			this->matProj = _P;
			this->color = _color;
		};

		mip::MATRIX44 getWorld() { return matWorld; };
		mip::MATRIX44 getView() { return matView; };
		mip::MATRIX44 getProj() { return matProj; };
		bool getDepthTest() { return bDepthTestMode; };
		bool getBackFace() { return bBackFace; };
		bool getAlpha() { return bAlpha; };
		float getAlphaVal() { return fAlpha; };
		mip::VECTOR4 getColor() { return color; };
		mip::VECTOR3 getCameraPos() { return cameraPos; };
		mip::VECTOR3 getCameraDir() { return cameraDir; };

		void setWorld(mip::MATRIX44 _mat) { matWorld = _mat; };
		void setView(mip::MATRIX44 _mat) { matView = _mat; };
		void setProj(mip::MATRIX44 _mat) { matProj = _mat; };
		bool setDepthTest(bool _bEnable) { bDepthTestMode = _bEnable; };
		bool setBackFace(bool _bEnable) { bBackFace = _bEnable; };
		bool setAlpha(bool _bEnable) { bAlpha = _bEnable; };
		bool setAlphaVal(float _fVal) { fAlpha = _fVal; };
		bool setColor(mip::VECTOR4 _color) { color = _color; };
		void setCameraPos(mip::VECTOR3 _cameraPos) { cameraPos = _cameraPos; };
		void setCameraDir(mip::VECTOR3 _cameraDir) { cameraDir = _cameraDir; };

	public:
		mip::MATRIX44 matWorld;
		mip::MATRIX44 matView;
		mip::MATRIX44 matProj;
		bool bDepthTestMode;
		bool bBackFace;
		bool bAlpha;
		float fAlpha;  // 0 ~ 255;
		mip::VECTOR4 color;
		mip::VECTOR3 cameraPos;
		mip::VECTOR3 cameraDir;
	};
}

#endif
#endif
