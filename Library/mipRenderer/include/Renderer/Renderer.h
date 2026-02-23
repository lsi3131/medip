#pragma once
#include "mip/core.h"
#include "math/math.h"

#include "color.h"
#include "texture.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "font.h"
#include "mipEngine/Topology.h"
#include <map>

#define RB_CLIPPLANE (1)
#define RB_CLIPPLANE0 (1 << 0)
#define RB_CLIPPLANE1 (1 << 1)
#define RB_CLIPPLANE2 (1 << 2)
#define RB_CLIPPLANE3 (1 << 3)
#define RB_CLIPPLANE4 (1 << 4)
#define RB_CLIPPLANE5 (1 << 5)

#define TEXTINDEX_2D_A 0
#define TEXTINDEX_2D_B 1
#define TEXTINDEX_3D_A 2
#define TEXTINDEX_3D_B 3
#define TEXTINDEX_3D_C 4

#define FBO_MIN_WIDTH 800
#define FBO_MIN_HEIGHT 600

#ifdef USE_OPENGL
namespace vr
{
	class IVRSystem;
};

struct colorset
{
	muint32 id;
	mip::FCOLOR fcolor;
};
#else
struct ID3DXEffect;
struct ID3DX11Effect;
#endif

namespace mip
{
	enum DRAWTYPE
	{
		DT_NONE = 0,
		DT_POINTLIST = 1,
		DT_LINELIST = 2,
		DT_LINESTRIP = 3,
		DT_TRIANGLELIST = 4,
		DT_TRIANGLESTRIP = 5,
		// DT_TRIANGLELIST = 6,   // not in DX11
		DT_FORCE_DWORD = 0x7fffffff, /* force 32-bit size enum */
	};

	enum RENDERSTATETYPE
	{
		RS_ZENABLE = 7,
		RS_FILLMODE = 8,
		RS_SHADEMODE = 9,
		RS_ZWRITEENABLE = 14,
		RS_ALPHATESTENABLE = 15,
		RS_LASTPIXEL = 16,
		RS_SRCBLEND = 19,
		RS_DESTBLEND = 20,
		RS_CULLMODE = 22,
		RS_ZFUNC = 23,
		RS_ALPHAREF = 24,
		RS_ALPHAFUNC = 25,
		RS_DITHERENABLE = 26,
		RS_ALPHABLENDENABLE = 27,
		RS_FOGENABLE = 28,
		RS_SPECULARENABLE = 29,
		RS_FOGCOLOR = 34,
		RS_FOGTABLEMODE = 35,
		RS_FOGSTART = 36,
		RS_FOGEND = 37,
		RS_FOGDENSITY = 38,
		RS_RANGEFOGENABLE = 48,
		RS_STENCILENABLE = 52,
		RS_STENCILFAIL = 53,
		RS_STENCILZFAIL = 54,
		RS_STENCILPASS = 55,
		RS_STENCILFUNC = 56,
		RS_STENCILREF = 57,
		RS_STENCILMASK = 58,
		RS_STENCILWRITEMASK = 59,
		RS_TEXTUREFACTOR = 60,
		RS_WRAP0 = 128,
		RS_WRAP1 = 129,
		RS_WRAP2 = 130,
		RS_WRAP3 = 131,
		RS_WRAP4 = 132,
		RS_WRAP5 = 133,
		RS_WRAP6 = 134,
		RS_WRAP7 = 135,
		RS_CLIPPING = 136,
		RS_LIGHTING = 137,
		RS_AMBIENT = 139,
		RS_FOGVERTEXMODE = 140,
		RS_COLORVERTEX = 141,
		RS_LOCALVIEWER = 142,
		RS_NORMALIZENORMALS = 143,
		RS_DIFFUSEMATERIALSOURCE = 145,
		RS_SPECULARMATERIALSOURCE = 146,
		RS_AMBIENTMATERIALSOURCE = 147,
		RS_EMISSIVEMATERIALSOURCE = 148,
		RS_VERTEXBLEND = 151,
		RS_CLIPPLANEENABLE = 152,
		RS_POINTSIZE = 154,
		RS_POINTSIZE_MIN = 155,
		RS_POINTSPRITEENABLE = 156,
		RS_POINTSCALEENABLE = 157,
		RS_POINTSCALE_A = 158,
		RS_POINTSCALE_B = 159,
		RS_POINTSCALE_C = 160,
		RS_MULTISAMPLEANTIALIAS = 161,
		RS_MULTISAMPLEMASK = 162,
		RS_PATCHEDGESTYLE = 163,
		RS_DEBUGMONITORTOKEN = 165,
		RS_POINTSIZE_MAX = 166,
		RS_INDEXEDVERTEXBLENDENABLE = 167,
		RS_COLORWRITEENABLE = 168,
		RS_TWEENFACTOR = 170,
		RS_BLENDOP = 171,
		RS_POSITIONDEGREE = 172,
		RS_NORMALDEGREE = 173,
		RS_SCISSORTESTENABLE = 174,
		RS_SLOPESCALEDEPTHBIAS = 175,
		RS_ANTIALIASEDLINEENABLE = 176,
		RS_MINTESSELLATIONLEVEL = 178,
		RS_MAXTESSELLATIONLEVEL = 179,
		RS_ADAPTIVETESS_X = 180,
		RS_ADAPTIVETESS_Y = 181,
		RS_ADAPTIVETESS_Z = 182,
		RS_ADAPTIVETESS_W = 183,
		RS_ENABLEADAPTIVETESSELLATION = 184,
		RS_TWOSIDEDSTENCILMODE = 185,
		RS_CCW_STENCILFAIL = 186,
		RS_CCW_STENCILZFAIL = 187,
		RS_CCW_STENCILPASS = 188,
		RS_CCW_STENCILFUNC = 189,
		RS_COLORWRITEENABLE1 = 190,
		RS_COLORWRITEENABLE2 = 191,
		RS_COLORWRITEENABLE3 = 192,
		RS_BLENDFACTOR = 193,
		RS_SRGBWRITEENABLE = 194,
		RS_DEPTHBIAS = 195,
		RS_WRAP8 = 198,
		RS_WRAP9 = 199,
		RS_WRAP10 = 200,
		RS_WRAP11 = 201,
		RS_WRAP12 = 202,
		RS_WRAP13 = 203,
		RS_WRAP14 = 204,
		RS_WRAP15 = 205,
		RS_SEPARATEALPHABLENDENABLE = 206,
		RS_SRCBLENDALPHA = 207,
		RS_DESTBLENDALPHA = 208,
		RS_BLENDOPALPHA = 209,
		RS_FORCE_DWORD = 0x7fffffff
	};

	enum FILLMODE
	{
		FILL_POINT = 1,
		FILL_WIREFRAME = 2,
		FILL_SOLID = 3,
		FILL_FORCE_DWORD = 0x7fffffff
	};

	enum CULLMODE
	{
		CULL_NONE = 1,
		CULL_CW = 2,
		CULL_CCW = 3,
		CULL_FORCE_DWORD = 0x7fffffff
	};

	enum class eShaderType
	{
		Volume,
		Line,
		Mesh,
		Surface,
		Surface3D,
		AdvSurface3D,
		AdvSurface2D,
		Compute,
		PlanningVolume,
		PlanningSurface3D,
	};

#define RSCLIPPLANE0 (1 << 0)
#define RSCLIPPLANE1 (1 << 1)
#define RSCLIPPLANE2 (1 << 2)
#define RSCLIPPLANE3 (1 << 3)
#define RSCLIPPLANE4 (1 << 4)
#define RSCLIPPLANE5 (1 << 5)

	class OcTreeTri;
	class OcTree;
	class OcNode;
	class OcNodeBase;
	class TEXTURE;
	class MeshCore;
	class ShaderEffect;
	class ShaderMesh;
	class CGLRenderModel;
	class ShaderStringManager;

	struct HMDcontrolState
	{
		bool PressMenu;
		bool PressButtonA;
		bool PressButtonGrip;

		bool PressDUp;
		bool PressDDown;
		bool PressDLeft;
		bool PressDRight;
		HMDcontrolState()
		{
			PressMenu = false;
			PressButtonA = false;
			PressButtonGrip = false;
			PressDUp = false;
			PressDDown = false;
			PressDLeft = false;
			PressDRight = false;
		}
	};

	class Renderer
	{
	public:
		Renderer();
		~Renderer();

#ifdef USE_DX9
	public:
		LPDIRECT3DDEVICE9 pD3DDevice;

	public:
		bool init3D(bool reset = false, LPCWSTR initfile = NULL);

		ID3DXEffect* getCurrentEffect() { return pEffect[m_currentShaderID]; }
		LPDIRECT3DDEVICE9 getDevice() { return pD3DDevice; }

		VERTEX_ID createVertexBuffer(muint32 size, STREAMTYPE streamType, mip::VERTEXBUFFER** r = NULL);
	private:
		LPDIRECT3D9 pD3D;
		D3DPRESENT_PARAMETERS D3DPP;
		IDirect3DSwapChain9* m_pSwapChain;
		IDirect3DSurface9* m_pRenderSurface;
		std::vector<ID3DXEffect*>  pEffect;
#else
#ifdef USE_OPENGL
	public:
		bool initVolumeVAO();
		bool updateVolumeVAO(float* pos, float* coord);
		bool beginVolumeVAO();
		bool endVolumeVAO();

		void makeCurrent();
		void doneCurrent();
		void bindFBO(muint32 defaultFBO = 0);
		void unbindFBO();

		muint32 getDefaultFBOID() { return m_osFBOID; }

#ifndef _M_IX86
		void releaseVR();
#endif
		bool init3D(muint32 FBO_width, muint32 FBO_height);
		bool initShader();
		bool initFont(const char* font_file);

		TEXTURE_ID createTextureGL(muint32 width, muint32 height, TEXTYPE type = TEXTYPE::TEXT_NONE, void* data = NULL);
		int createTextureGL(unsigned char* data, muint32 width, muint32 height, muint32 nrChannels, unsigned int type = 0);
		bool updateTextureGL(TEXTURE_ID texture, void* data);
		TEXTURE_ID createVolumeTextureGL(muint32 width, muint32 height, muint32 depth, TEXTYPE type = TEXTYPE::TEXT_VOLUME, TEXFORMAT format = TEXFORMAT::TEXF_R32F, void* data = NULL);
		bool updateVolumeTextureGL(TEXTURE_ID texture, void* data);

		int getOpenGLError(std::string str);

		bool setShaderWVP(const mip::MATRIX44* world = NULL, const mip::MATRIX44* view = NULL, const mip::MATRIX44* proj = NULL);
		bool setShaderBillBoardWVP(const mip::MATRIX44* world = NULL, const mip::MATRIX44* view = NULL, const mip::MATRIX44* proj = NULL);
		VERTEX_ID createVertexBuffer(muint32 size, STREAMTYPE streamType, mip::VERTEXBUFFER** r = NULL);

		void setNotUseContext(bool value) { m_donotUseContext = value; }
		void setCacheAccel(int width);
		bool setDepthState(DWORD state, DWORD value);
		bool setRasterState(DWORD state, DWORD value);
		bool setBlendState(DWORD state, DWORD value);
	private:
		void createFrameBuffer(muint32 width, muint32 height);
		bool initOpenGLExt();

	public:
		std::vector<colorset> ColorSetList;

	public:
		ShaderEffect* pShaderVolume;
		ShaderEffect* pShaderLine;
		ShaderMesh* pShaderMesh;
		ShaderEffect* pShaderSurface;
		ShaderEffect* pShaderSurface3D;
		ShaderEffect* pShaderAdvSurface3D;
		ShaderEffect* pShaderAdvSurface2D;
		//ShaderEffect* m_pShaderCompute;
		ShaderEffect* pShaderPlanningVolume;
		ShaderEffect* pShaderPlanningSurface3D;
	private:
		mip::FONT m_font;
		int m_contextFormat;
		HDC m_hDC;
		HGLRC m_openGLContext;
		muint32 m_osZBufferID;
		muint32 m_osColorBufferID;
		muint32 m_osFBOID;
		muint32 m_osPBOID;

		muint32 m_tempFBOID;

		bool m_donotUseContext;
		void* m_lockPoint;
		muint32 m_VolumeImageCX;
		muint32 m_VolumeImageCY;
		muint32 m_VolumeImageCZ;

		muint32 m_VolumeRenderBufferPoint;
		muint32 m_VolumeRenderBufferTexcoord;
		muint32 m_VolumeRenderVAO;

		muint32 m_currentVAOID;
		muint32 m_currentProgramID;

		muint32 m_sampler2D_A;
		muint32 m_sampler2D_B;
		muint32 m_samplerVolume3D_A;
		muint32 m_samplerVolume3D_B;
		muint32 m_samplerVolume3D_C;
#else // DX11
	public:
		ID3D11Device* pD3DDevice;
		ID3D11DeviceContext* pD3DDeviceContext;

	public:
		bool init3D(bool reset = false, LPCWSTR initfile = NULL);
		ID3DX11Effect* getCurrentEffect() { return pEffect[m_currentShaderID]; }
		ID3D11Device* getDevice() { return pD3DDevice; }
		ID3D11DeviceContext* getDeviceContext() { return pD3DDeviceContext; }

		VERTEX_ID createVertexBuffer(muint32 size, STREAMTYPE streamType, mip::VERTEXBUFFER** r = NULL);

		bool setDepthState(DWORD state, DWORD value);
		bool setRasterState(DWORD state, DWORD value);
		bool setBlendState(DWORD state, DWORD value);

		muint32 getclipPlaneState() { return m_enable_clipPlaneState; }

		ID3DX11EffectPass* getEffectPass(muint32 shaderID, LPCSTR str, muint32 pass);
		bool createInputLayer(ID3DX11EffectPass* refpass, mip::STREAMTYPE, ID3D11InputLayout** ret);
		VERTEX_ID createVertexBuffer(muint32 size, STREAMTYPE streamType, ID3DX11EffectPass* refpass, mip::VERTEXBUFFER** r = NULL);
	private:
		D3D11_MAPPED_SUBRESOURCE m_lockDesc;
		VERTEX_ID m_primitiveUpVertex;

		IDXGISwapChain* m_pSwapChain;
		ID3D11RenderTargetView* m_pRenderView;
		ID3D11DepthStencilView* m_pDepthStencilView;
		ID3D11Texture2D* m_pDepthStencilBuffer;
		muint32 m_4xMsaaQuality;
		std::map<HWND, IDXGISwapChain*> m_SwapChinList;

		std::vector<ID3DX11Effect*>  pEffect;
		ID3DX11EffectTechnique* m_currentTech;
		ID3DX11EffectPass* m_currentEffectPass;
		muint32 m_currentPass;
#endif

#endif
	public:
		void release();
		void setHMDDeviceBuildCheck(muint32 id) { m_HMDDeviceBuildCheckList.push_back(id); }

		// Shader
		TEXTURE_ID createTextureFormFile(LPCWSTR filename, muint32 width, muint32 height, TEXTYPE type = TEXTYPE::TEXT_NONE);
		TEXTURE_ID createTexture(muint32 width, muint32 height, TEXTYPE type = TEXTYPE::TEXT_NONE);
		TEXTURE_ID createVolumeTexture(muint32 width, muint32 height, muint32 depth, TEXTYPE type = TEXTYPE::TEXT_VOLUME, TEXFORMAT format = TEXFORMAT::TEXF_R32F);
		bool setTexture(TEXTURE_ID texture);
		bool deleteTexture(TEXTURE_ID texture);
		bool deleteTextureLater(TEXTURE_ID texture);
		bool prepareTexture();
		TEXTURE* getTexture(TEXTURE_ID texture);
		//bool lock(RESOURCE_ID resource, void ** retPoint, int * pitch = NULL, int * SlicePitch = NULL, TEXLOCK_TYPE locktype = TL_DISCARD);
		bool lock(RESOURCE_ID resource, void** retPoint, int* pitch = NULL, int* SlicePitch = NULL);
		bool unlock(RESOURCE_ID resource);

		// VertexBuffer
		VERTEXBUFFER* getVertexBuffer(VERTEX_ID index);
		bool deleteVertexBuffer(VERTEX_ID index);
		bool setVertex(VERTEX_ID vert, INDEX_ID index);

		// IndexBuffer
		INDEX_ID createIndexBuffer(muint32 size, mip::INDEXBUFFER** r = NULL);
		INDEXBUFFER* getIndexBuffer(INDEX_ID index);
		bool deleteIndexBuffer(INDEX_ID texture);

		bool draw(DRAWTYPE type, VERTEX_ID vert, INDEX_ID index, muint32 polyCnt);
		bool drawPlane(std::vector<mip::VECTOR3>& point, std::vector<mip::VECTOR2>& texcoord, TEXTURE_ID texture);
		bool drawFont(const std::string& str, TEXTURE_ID texture, const mip::VECTOR2& size = mip::VECTOR2(1.0f, 1.0f));
		void makeText(const char* str);
		void drawText(mip::COLOR* color = NULL);

		// DEFBUFFER_ID createDeferredBuffer(mip::DeferredBuffer ** r = NULL);
		// mip::DeferredBuffer * getDeferredBuffer(DEFBUFFER_ID index);
		// bool deleteDeferredBuffer(DEFBUFFER_ID index);
		// bool renderDefferdQuad(DEFBUFFER_ID index, int sharp = 0);

		bool preRenderProcess();

		bool setShader(LPCSTR str, unsigned int id = 0);
		bool clear(mip::COLOR& color);
		bool beginScene();
		bool endScene();
		bool beginRender(HWND hwnd, int WindowCX, int WindowCY);
		bool endRender();
		bool present();
		void* getBackBuffer(muint32 _left, muint32 _top, muint32 _right, muint32 _bottom);

		bool setViewPort(muint32 left, muint32 top, muint32 width, muint32 height);

		void setLineColor(const mip::COLOR color);
		void setLineColor(muint8 r, muint8 g, muint8 b, muint8 a = 255);
		void setLineColor(const mip::FCOLOR color);
		void setPreset(float min, float max, int index);
		void setBrightness(float value);
		void setContrast(float value);
		void setVolumeAlpha(float value);
		void setEye(const mip::VECTOR3& eye);
		void setCutRange(float min, float max);
		void setWidth(float value);
		void setHeight(float value);
		void setDepth(float value);
		void setLight(const mip::VECTOR3& light);

		void setMaskColor(int mask_id, const mip::COLOR& color); // mask_id : 1 ~ 6

		void setSylinderRange(float min, float max);
		void setZPlaneClip(float index);
		void setBlendAmount(float value);

		void setWorld(const mip::MATRIX44& mat);
		void setView(const mip::MATRIX44& mat);
		void setProj(const mip::MATRIX44& mat);
		void setInvWorld(const mip::MATRIX44& mat);
		mip::MATRIX44 getWorld() { return m_world; }
		mip::MATRIX44 getView() { return m_view; }
		mip::MATRIX44 getProj() { return m_proj; }

		bool beginShader(muint32 program_id = 0);
		bool endShader();
		bool beginShaderPass(muint32 pass);
		bool endShaderPass();

		// Render State
		bool setFVF(STREAMTYPE fvf);
		bool setClipPlane(DWORD index, const float* plane);
		bool applyClipPlane(muint32 shaderProgramID);
		bool setRenderState(DWORD state, DWORD value);

		bool draw(mip::DRAWTYPE type, int pass, int drawCount, const void* vert, int Streamsize, int VertexCount);
		bool drawPrimitiveUp(mip::DRAWTYPE type, int primitiveCount, const void* vert, int Streamsize, int VertexCount);
		bool drawIndexedPrimitiveUp(mip::DRAWTYPE type, int primitiveCount, const void* vert, const muint32* tris, int Streamsize, int VertexCount, int IndexCount);
		// preline not use
		bool drawLineList(int pass, std::vector<mip::VECTOR3>& line, float thick = 1.0f, bool bDepthTest = true);
		// preline use
		bool renderLineList(std::vector<mip::VECTOR3>& line, mip::COLOR color, float thick = 1.0f, bool bDepthTest = true);
		bool renderSurfaceImage(std::vector<mip::VECTOR3>& point, std::vector<mip::VECTOR3>& texcoord);
		bool renderBG(mip::COLOR& color1, mip::COLOR& color2);
		bool renderImage(mip::VECTOR2& ndcPos, mip::VECTOR2& ndcSize, mip::VECTOR2& windowSize, mip::VECTOR2& uvoffset);
		bool renderPoint(std::vector<mip::VECTOR3>& point);

		void renderTestOctree(mip::OcTreeTri* root, const mip::MATRIX44& mat, const mip::MATRIX44& matworld, bool bTest = false);
		void renderTestOctree(mip::MeshCore* mesh, mip::OcTree* root);

		// Device
		bool checkDevice();
		void restoreDeviceObjects();

		const char* getRendererName() { return m_renderer.c_str(); }
		const char* getVendorName() { return m_rendererVendor.c_str(); }
		const char* getRendererVersion() { return m_rendererVersion.c_str(); }
		bool isAvailableVolumeRender() { return m_availableVolumeRender; }
		void setAvailableVolumeRender(bool active) { m_availableVolumeRender = active; }

	private:
		void renderNode(mip::OcNode* node, mip::TREETYPE type, const mip::MATRIX44& mat,
			const mip::MATRIX44& matworld, std::vector<mip::VECTOR3>& vec_lineList);
		void renderNode(mip::MeshCore* mesh, mip::OcNodeBase* node, mip::TREETYPE type);

	private:
		bool m_availableVolumeRender;
		std::string m_rendererVendor;
		std::string m_renderer;
		std::string m_rendererVersion;
		mip::TEXT_PRINT m_textprint;
		COLOR* m_presentData;
		muint32 m_preDataCX;
		muint32 m_preDataCY;

		////////////////////////////////////// VR
#ifdef USE_OPENGL
	public:
		struct FramebufferDesc
		{
			muint32 m_nDepthBufferId;
			muint32 m_nRenderTextureId;
			muint32 m_nRenderFramebufferId;
			muint32 m_nResolveTextureId;
			muint32 m_nResolveFramebufferId;
		};


	public:
		vr::IVRSystem* getHMD() { return m_pHMD; }
		bool isValidateHMD() { return m_pHMD != 0 ? true : false; }
		bool isValidateHMDShader() { return m_unSceneProgramID != 0 ? true : false; }
		bool initHMD(muint32 defaultFBO, muint32 width, muint32 height);
		bool initHMDGL(muint32 defaultFBO, muint32 width, muint32 height);
		bool createHMDShaders();

		void renderHMDController(muint32 nVREye, muint32 fontTexture);
		void makeControllerAxes(mip::MATRIX44& toLocal, mip::VECTOR3* firstDeltaVector = NULL, mip::VECTOR3* secondDeltaVector = NULL,
			mip::QUATERNION* firstDeltaRot = NULL, mip::QUATERNION* secondDeltaRot = NULL);
		void renderStereoTargets(muint32 defaultFBO = 0, renderHMDfunc func = NULL, void* data = NULL);
		void renderCompanionWindow(muint32 width, muint32 height, bool simple = false);
		CGLRenderModel* findOrLoadRenderModel(const char* pchRenderModelName);

		bool handleInput(HMDcontrolState* fristContorl = NULL, HMDcontrolState* secondControl = NULL);
		muint32 compileGLShader(const char* pchShaderName, const char* pchVertexShader, const char* pchFragmentShader);
		void setupCameras();
#ifndef _M_IX86
		void setupRenderModelForTrackedDevice(muint32 unTrackedDeviceIndex);
#endif
		bool setupStereoRenderTargets(muint32 defaultFBO, muint32 width, muint32 height);
		void setupCompanionWindow();
		void setupRenderModels();

		mip::MATRIX44 getCurrentViewProjectionMatrix(muint32 nVREye);
		mip::MATRIX44 getCurrentViewMatrix(muint32 nVREye);
		mip::MATRIX44 getCurrentProjectionMatrix(muint32 nVREye);
		mip::MATRIX44 getHMDMatrixProjectionEye(muint32 nVREye, float fNear, float fFar);
		mip::MATRIX44 getHMDMatrixPoseEye(muint32 nVREye);
		void updateHMDMatrixPose();
		bool createHMDFrameBuffer(int nWidth, int nHeight, FramebufferDesc& framebufferDesc, muint32 defaultFBO);
		muint32 getHMDWidth() { return m_nRenderWidth; }
		muint32 getHMDHeight() { return m_nRenderHeight; }

	public:
		mip::MATRIX44 m_mat4HMDPose;
		mip::MATRIX44 m_mat4eyePosLeft;
		mip::MATRIX44 m_mat4eyePosRight;

		mip::MATRIX44 m_mat4ProjectionCenter;
		mip::MATRIX44 m_mat4ProjectionLeft;
		mip::MATRIX44 m_mat4ProjectionRight;

		mip::MATRIX44 m_matfirstControl;
		mip::MATRIX44 m_matsecondControl;
	private:
		vr::IVRSystem* m_pHMD;
		std::string m_strDriver;
		std::string m_strDisplay;

		muint32 m_unSceneProgramID;
		muint32 m_unCompanionWindowProgramID;
		muint32 m_unControllerTransformProgramID;
		muint32 m_unRenderModelProgramID;

		muint32 m_nSceneMatrixLocation;
		muint32 m_nControllerMatrixLocation;
		muint32 m_nRenderModelMatrixLocation;

		FramebufferDesc m_leftEyeDesc;
		FramebufferDesc m_rightEyeDesc;

		muint32 m_nRenderWidth;
		muint32 m_nRenderHeight;

		muint32 m_nCompanionWindowWidth;
		muint32 m_nCompanionWindowHeight;

		muint32 m_glControllerVertBuffer;
		muint32 m_unControllerVAO;
		muint32 m_uiControllerVertcount;

		muint32 m_unCompanionWindowVAO;
		muint32 m_glCompanionWindowIDVertBuffer;
		muint32 m_glCompanionWindowIDIndexBuffer;
		muint32 m_uiCompanionWindowIndexSize;

		std::vector< CGLRenderModel* > m_vecRenderModels;
		CGLRenderModel* m_rTrackedDeviceToRenderModel[16 /*vr::k_unMaxTrackedDeviceCount*/];

		ShaderStringManager* m_pShaderStringManager;
	private:
		HWND m_tempHWND;
		RECTi m_viewPortSize;

		HWND m_hWnd;

		std::map<TEXTURE_ID, mip::TEXTURE*> m_textureList;
		std::map<VERTEX_ID, mip::VERTEXBUFFER*> m_vertexBufferLIst;
		std::map<INDEX_ID, mip::INDEXBUFFER*> m_indexBufferList;

		std::vector<TEXTURE_ID> m_deltextureList;

		//std::map<DEFBUFFER_ID, mip::DeferredBuffer*> m_deferredBufferList;
		muint32 m_texCount;
		muint32 m_tex3DCount;
		muint32 m_vertexCount;
		muint32 m_indexCount;
		muint32 m_deferredBufferCount;

		HWND m_osHWND;

		mip::MATRIX44 m_world;
		mip::MATRIX44 m_view;
		mip::MATRIX44 m_proj;

		muint32 m_currentShaderID;

		mip::STREAMTYPE m_currentStreamType;
		std::wstring m_initShaderFile;
		muint32 m_enable_clipPlaneState;
		mip::MATRIX44 m_clipPlane[6];
		mip::PLANE m_shaderClipplane[6];

		std::vector<muint32> m_HMDDeviceBuildCheckList;
#endif

		void getMeshGLSLString(std::string& strVS_glsl, std::string& strPS_glsl);
		void setMeshGLSLString(const std::string& strVS_glsl, const std::string& strPS_glsl);
	};
};
