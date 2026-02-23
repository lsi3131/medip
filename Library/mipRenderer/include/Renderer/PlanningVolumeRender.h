/*
@company		메디컬아이피
@autor			허 건 과장
@date			2021-11-26
@brief			PlanningVolumeRenderer 헤더파일
*/

#pragma once
#include "math/math.h"
#include "VolumeRender.h"

namespace mip
{
	class Renderer;
	class MeshCore;

	struct SPlanningVolumeRenderInfo
	{
		Renderer*	p_renderer;

		muint32		renderpass;
		muint32		quality;

		TEXTURE_ID	volume_texture;
		TEXTURE_ID	mesh_volume_texture;
		TEXTURE_ID	mask_texture;
		TEXTURE_ID	color_preset;

		mip::VECTOR3 sz_volume_xyz;
		mip::VECTOR2 hu_min_max;

		int legend;
		int density_mode;

		float alpha;
		float totalSlice;
		float samplingRate;
		float gamma; 

		SPlanningVolumeRenderInfo()
		{
			p_renderer = nullptr;

			renderpass = 0;

			volume_texture = 0;
			mask_texture = 0;
			mesh_volume_texture = 0;
			color_preset = 0;

			legend = 0;
			alpha = 0.4f;
			totalSlice = 320.0f;
			samplingRate = 1.0f;
			gamma = 2.5f;
			quality = 0;

			density_mode = 0;
		}
	};

	class PlanningVolumeRenderer : public VolumeRenderer
	{
	public:
		PlanningVolumeRenderer();
		~PlanningVolumeRenderer();
		void release();

		bool drawVolume(SPlanningVolumeRenderInfo & _render_info);

		bool drawVolumePlanningPlaneBegin(SPlanningVolumeRenderInfo & _render_info);
		bool drawVolumePlanningPlaneRender(SPlanningVolumeRenderInfo & _render_info, std::vector<mip::VECTOR3> & point, std::vector<mip::VECTOR3> & texcoord, muint32 id = 0);
		bool drawVolumePlanningPlaneEnd(SPlanningVolumeRenderInfo & _render_info);
	};
};