#pragma once
#include "mip/core.h"
#include "color.h"
#include "math/math.h"

#ifdef USE_DX9
struct IDirect3DVertexBuffer9;
#else

#endif

namespace mip
{
	enum STREAMTYPE
	{
		ST_P = 0,
		ST_PC,
		ST_PT,
		ST_PT3,
		ST_PN,
		ST_PNT,
		ST_PNT3,
		ST_PNC,
		ST_MESH,
		ST_RAYBOX,
		ST_BG,
		ST_MAX,
	};
	struct SVertexMeshP
	{
		float x, y, z;
#ifdef USE_DX9
		static const DWORD FVF;
		static const D3DVERTEXELEMENT9   decl[];
#else
#ifdef USE_OPENGL

#else
		static const D3D11_INPUT_ELEMENT_DESC  decl[];
#endif
#endif
	};

	struct SVertexMeshPC
	{
		float x, y, z;
		DWORD color;
#ifdef USE_DX9
		static const DWORD FVF;
		static const D3DVERTEXELEMENT9   decl[];
#else
#ifdef USE_OPENGL

#else
		static const D3D11_INPUT_ELEMENT_DESC  decl[];
#endif
#endif
	};

	struct SVertexMeshPT
	{
		mip::VECTOR3 Pos;
		mip::VECTOR2 tex;
#ifdef USE_DX9
		static const DWORD FVF;
		static const D3DVERTEXELEMENT9   decl[];
#else
#ifdef USE_OPENGL

#else
		static const D3D11_INPUT_ELEMENT_DESC  decl[];
#endif
#endif
	};

	struct SVertexMeshPT3
	{
		mip::VECTOR3 Pos;
		mip::VECTOR3 tex;
#ifdef USE_DX9
		static const DWORD FVF;
		static const D3DVERTEXELEMENT9   decl[];
#else
#ifdef USE_OPENGL

#else
		static const D3D11_INPUT_ELEMENT_DESC  decl[];
#endif
#endif
	};

	struct SVertexMeshPN
	{
		mip::VECTOR3 Pos;
		mip::VECTOR3 Norm;
#ifdef USE_DX9
		static const DWORD FVF;
		static const D3DVERTEXELEMENT9   decl[];
#else
#ifdef USE_OPENGL

#else
		static const D3D11_INPUT_ELEMENT_DESC  decl[];
#endif
#endif
	};

	struct SVertexMeshPNT
	{
		mip::VECTOR3 Pos;
		mip::VECTOR3 Norm;
		mip::VECTOR2 tex;
#ifdef USE_DX9
		static const DWORD FVF;
		static const D3DVERTEXELEMENT9   decl[];
#else
#ifdef USE_OPENGL

#else
		static const D3D11_INPUT_ELEMENT_DESC  decl[];
#endif
#endif
	};

	struct SVertexMeshPNT3
	{
		mip::VECTOR3 Pos;
		mip::VECTOR3 Norm;
		mip::VECTOR3 tex;
#ifdef USE_DX9
		static const DWORD FVF;
		static const D3DVERTEXELEMENT9   decl[];
#else
#ifdef USE_OPENGL

#else
		static const D3D11_INPUT_ELEMENT_DESC  decl[];
#endif
#endif
	};

	struct SVertexMeshPNC
	{
		mip::VECTOR3 Pos;
		mip::VECTOR3 Norm;
#ifdef USE_DX9
		DWORD			 Color;
		static const DWORD FVF;
		static const D3DVERTEXELEMENT9   decl[];
#else
		mip::FCOLOR  Color;
#ifdef USE_OPENGL

#else
		static const D3D11_INPUT_ELEMENT_DESC  decl[];
#endif
#endif
	};

	struct SVertexMesh
	{
		mip::VECTOR3 Pos;
		DWORD			 Color;
		mip::VECTOR3 Norm;
		mip::VECTOR3 tangent;
#ifdef USE_DX9
		static const DWORD FVF;
		static const D3DVERTEXELEMENT9   decl[];
#else
#ifdef USE_OPENGL

#else
		static const D3D11_INPUT_ELEMENT_DESC  decl[];
#endif
#endif
	};

	struct SVertexRayBox
	{
		mip::VECTOR3 Pos;
		mip::VECTOR3 tex;
#ifdef USE_DX9
		static const DWORD FVF;
		static const D3DVERTEXELEMENT9   decl[];
#else
#ifdef USE_OPENGL

#else
		static const D3D11_INPUT_ELEMENT_DESC  decl[];
#endif
#endif
	};

	struct SVertexBackGround
	{
		float x, y, z;
		float a, r, g, b;
#ifdef USE_DX9
		static const DWORD FVF;
		static const D3DVERTEXELEMENT9   decl[];
#else
#ifdef USE_OPENGL

#else
		static const D3D11_INPUT_ELEMENT_DESC  decl[];
#endif
#endif
	};

	class Renderer;

	class VERTEXBUFFER
	{
	public:
#ifdef USE_DX9
		IDirect3DVertexBuffer9 *		vertexbuffer;
		IDirect3DVertexDeclaration9*	pVertexinfo;
#else
#ifdef USE_OPENGL
		muint32					vertexbuffer;
#else
		ID3D11Buffer		*   vertexbuffer;
		ID3D11InputLayout   *   pVertexinfo;
#endif
#endif
		muint32				m_size;
		muint32				m_streamSize;
		void*				m_pBuffer;
		STREAMTYPE			m_type;
	
	public:
#ifdef USE_DX9
		VERTEXBUFFER(IDirect3DVertexBuffer9 * _vertex, IDirect3DVertexDeclaration9 * info, STREAMTYPE type, muint32 vertexCount);
#else
#ifdef USE_OPENGL
		VERTEXBUFFER(muint32 _vertex, STREAMTYPE type, muint32 vertexCount);
#else
		VERTEXBUFFER(ID3D11Buffer * _vertex, ID3D11InputLayout * info, STREAMTYPE type, muint32 vertexCount);
#endif
#endif
		~VERTEXBUFFER();

		bool lock(void ** retPoint = NULL);
		bool unlock();
		void* getPoint();
	public:
		static muint32 GetStreamSize(STREAMTYPE type);
		static DWORD GetFVF(STREAMTYPE type);
#ifdef USE_DX9
		static const D3DVERTEXELEMENT9 * GetVertexDecl(STREAMTYPE type);
#else
#ifdef USE_OPENGL

#else
		static const D3D11_INPUT_ELEMENT_DESC * GetVertexDecl(STREAMTYPE type);
		static muint32 GetVElementCount(STREAMTYPE type);
#endif
#endif
	};
};