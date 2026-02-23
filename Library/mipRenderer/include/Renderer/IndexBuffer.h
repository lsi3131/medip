#pragma once
#include "mip/core.h"

#ifdef USE_DX9
struct IDirect3DIndexBuffer9;
#else
#endif

namespace mip
{
	class Renderer;

	class INDEXBUFFER
	{
	public:
#ifdef USE_DX9
		IDirect3DIndexBuffer9 * indexbuffer;
#else

#ifdef USE_OPENGL
		muint32					indexbuffer;
#else
		ID3D11Buffer		*   indexbuffer;
#endif

#endif
		muint32				m_size;
	private:
		void*				m_pBuffer;

	public:
#ifdef USE_DX9
		INDEXBUFFER(IDirect3DIndexBuffer9 * index, muint32 indexCount);
#else
#ifdef USE_OPENGL
		INDEXBUFFER(muint32 index, muint32 indexCount);
#else
		INDEXBUFFER(ID3D11Buffer * index, muint32 indexCount);
#endif
#endif
		~INDEXBUFFER();

		bool lock(void ** retPoint = NULL);
		bool unlock();
		void* getPoint();
	};
};