#pragma once
#include "mip/core.h"

#ifdef USE_DX9
struct IDirect3DBaseTexture9;
#else

#endif

namespace mip
{
	class Renderer;

	enum TEXFORMAT
	{
		TEXF_XRGB = 0,
		TEXF_A8R8G8B8,
		TEXF_R8I,
		TEXF_R8UI,
		TEXF_R16UI,
		TEXF_R16I,
		TEXF_R32F,
		TEXF_R32I,
		TEXF_R32UI,
	};

	enum TEXTYPE
	{
		TEXT_NONE = 0,
		TEXT_TF,
		TEXT_ALPHA,
		TEXT_SYLINDER,
		TEXT_VOLUME = 10000,
		TEXT_VOLUME_CUT = 10001,
		TEXT_MASK,
		TEXT_USER = 100000,
		TEXT_USER2 = 100001,
		TEXT_HU8 = 100003,
		TEXT_HU16 = 100004,
		TEXT_HU32 = 100005,
		TEXT_HUHISTO = 200000,
		TEXT_TFUSER = 300000,
	};

	enum TEXLOCK_TYPE
	{
		TL_DISCARD = 0,
		TL_READ,
	};

	class TEXTURE
	{
	public:
#ifdef USE_DX9
		IDirect3DBaseTexture9 * texture;
#else
#ifdef USE_OPENGL
		muint32					texture;
#else
		ID3D11Resource		*   texture;
		ID3D11ShaderResourceView * SRV;
#endif
#endif
		std::wstring		name;
		std::string			shaerTexName;
		TEXFORMAT			format;

	private:
#ifdef USE_DX9
		D3DLOCKED_RECT		lockRect;
		D3DLOCKED_BOX		lockBox;
#endif
		TEXTYPE				type;

		muint32				width;
		muint32				height;
		muint32				depth;
		muint32				nrChannels;
		muint32				tileStyle;

		unsigned char* data;

	public:
#ifdef USE_DX9
		TEXTURE(IDirect3DBaseTexture9 * _texture, LPCWSTR filename, TEXTYPE type);
		static D3DFORMAT GetFormat(TEXFORMAT fmt);
#else
#ifdef USE_OPENGL
		TEXTURE(muint32 _texture, LPCWSTR filename, TEXTYPE type, TEXFORMAT _format, muint32 xSize, muint32 ySize, muint32 zSize = 0, muint32 nrChannels = 0, muint32 tileStyle = 0);
		TEXTURE(std::string filename, muint32 xSize, muint32 ySize, muint32 zSize = 0, muint32 nrChannels = 0, muint32 tileStyle = 0);
#else
		TEXTURE(ID3D11Resource * _texture, LPCWSTR filename, TEXTYPE type, ID3D11ShaderResourceView * _SRV);
#endif
#endif
		~TEXTURE();

		bool lock(void ** retPoint = NULL, int * pitch = NULL, TEXLOCK_TYPE locktype = TL_DISCARD);
		bool unlock();
		bool lockVolume(void ** retPoint = NULL, int * pitch = NULL, int * SlicePitch = NULL);

		int  getPitch();
		void* getPoint();

		TEXTYPE getType() {	return type; }
		TEXFORMAT getFormat() { return format; }
		muint32 getWidth() { return	width; }
		muint32 getHeight() { return height; }
		muint32 getDepth() { return depth; }
		
		muint32 getChannels() { return nrChannels; }
		muint32 getTileStyle() { return tileStyle; }
		unsigned char* getTextureData() { return data; }
		void setTextureData(unsigned char* pData) { data = pData; }
		
	};
};