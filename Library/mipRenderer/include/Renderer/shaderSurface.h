#pragma once
#include "effect.h"

namespace mip
{
#ifdef USE_DX9
#else
#ifdef USE_OPENGL
	class ShaderSurface : public ShaderEffect
	{
	public:
		ShaderSurface(bool withInit = true);
		virtual ~ShaderSurface();

	public:
		virtual bool init() override;
		virtual void render() override;
	};

	class ShaderSurface3D : public ShaderEffect
	{
	public:
		ShaderSurface3D(bool withInit = true);
		virtual ~ShaderSurface3D();

		virtual bool init() override;
		virtual void render() override;
	};

	class ShaderAdvSurface3D : public ShaderEffect
	{
	public:
		ShaderAdvSurface3D(bool withInit = true);
		virtual ~ShaderAdvSurface3D();

	public:
		virtual bool init() override;
		virtual void render() override;
	};


	class ShaderPlanningSurface3D : public ShaderEffect
	{
	public:
		ShaderPlanningSurface3D(bool withInit = true);
		virtual ~ShaderPlanningSurface3D();

	public:
		virtual bool init() override;
		virtual void render() override;
	};

	class ShaderAdvSurface2D : public ShaderEffect
	{
	public:
		ShaderAdvSurface2D(bool withInit = true);
		virtual ~ShaderAdvSurface2D();

	public:
		virtual bool init() override;
		virtual void render() override;
	public:
		static const char * s_ps_line[];
		static const char * s_vs_line[];
	};
#else

#endif
#endif
};