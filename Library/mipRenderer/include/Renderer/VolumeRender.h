#pragma once
#include "math/math.h"

namespace mip
{
	class Renderer;
	class MeshCore;

	enum BGTYPE
	{
		BGT_MPRVIEW,
		BGT_MPRSIDEVIEW,
		BGT_PANOSIDEVIEW,
		BGT_IMPSIDEVIEW,
	};

	class VolumeRenderer
	{
	public:
		VolumeRenderer();
		~VolumeRenderer();

	public:
		void initVolumeSizeInfo(float fXSpace, float fYSpace, float fZSpace, int nXCnt, int nYCnt, int nZCnt);
		bool drawVolume(Renderer* r, muint32 renderpass, TEXTURE_ID volume_texture, TEXTURE_ID mask_texture, TEXTURE_ID color_preset,
			float xSize, float ySize, float zSize, float fMin, float fMax, int legend = 0, float alpha = 0.4f, float totalSlice = 320.0f,
			float samplingRate = 1.0f, float gamma = 2.5f, muint32 quality = 0, bool vr_mode = false, const mip::MATRIX44* vr_view = NULL, const mip::VECTOR3* pointer = NULL);

		bool drawVolumePlane(Renderer* r, std::vector<mip::VECTOR3>& point, std::vector<mip::VECTOR3>& texcoord,
			muint32 renderpass, TEXTURE_ID volume_texture, TEXTURE_ID mask_texture, TEXTURE_ID color_preset,
			float fMin, float fMax, int legend = 0, float alpha = 0.4f, float gamma = 2.5f);

		//   bool drawVolumeAdvPlane(Renderer * r, std::vector<mip::VECTOR3> & point, std::vector<mip::VECTOR3> & texcoord,
		//    muint32 renderpass, TEXTURE_ID volume_texture, TEXTURE_ID mask_texture, TEXTURE_ID color_preset,
		//    float fMin, float fMax, int legend = 0, float alpha = 0.4f, float gamma = 2.5f);

		bool drawVolumeAdvPlaneBegin(Renderer* r, muint32 renderpass, TEXTURE_ID volume_texture, TEXTURE_ID mask_texture, TEXTURE_ID color_preset, float fMin, float fMax, int legend = 0, float alpha = 1.0f, float gamma = 2.5f);
		bool drawVolumeAdvPlaneRender(Renderer* r, std::vector<mip::VECTOR3>& point, std::vector<mip::VECTOR3>& texcoord, muint32 id = 0);
		bool drawVolumeAdvPlaneEnd(Renderer* r);

		bool drawVolumeAdvPlaneBegin2D(Renderer* r, muint32 renderpass, TEXTURE_ID color_preset, float fMin, float fMax, float xLengh, float yLengh, float xSpace, float ySpace, int legend = 0);
		bool drawVolumeAdvPlaneRender2D(Renderer* r, TEXTURE_ID volume_texture2D, TEXTURE_ID mask_texture2D, std::vector<mip::VECTOR3>& point, std::vector<mip::VECTOR2>& texcoord, muint32 id = 0, float alpha = 1.0f, float gamma = 2.5f);
		bool drawVolumeAdvPlaneEnd2D(Renderer* r);

		bool ray_VolumeCheck(const VECTOR3& org, const VECTOR3& dir, float check_value, float* HuValue, VECTOR3* picking_point = NULL, int zpos = -100);
		bool ray_MaskCheck(const mip::RAY ray, unsigned char** pMaskDataPoint, const std::vector<unsigned char>& vecMaskShowFlag, std::vector<unsigned char>& vecHitMaskBitAll, VECTOR3* vHitPoint = nullptr);

		int getVoxelIndex(const VECTOR3 local, VECTOR3* pMaskCoord = nullptr);
	public:
		static bool DrawBG(Renderer* r, BGTYPE type);
		static void RenderGuideBox(Renderer* r, muint32 TextureID);
		static void RenderCoordLine(Renderer* r);

	protected:
		mip::MeshCore* m_pRayBox;
		float m_fWidth;
		float m_fHeight;
		float m_fDepth;

		float m_fXSpace;
		float m_fYSpace;
		float m_fZSpace;

		muint32 m_XCount;
		muint32 m_YCount;
		muint32 m_ZCount;
	};
};