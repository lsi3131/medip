#pragma once
#include "mipEngine/Camera.h"
#include "math/transform.h"

#ifdef USE_OPENGL
#else
#include "VertexBuffer.h"
#endif

namespace mip
{
	class Renderer;
	class DcmtkVolumeReader;

	enum SLICE_TYPE
	{
		ST_NONE,
		ST_AXIAL,
		ST_CORONAL,
		ST_SAGITTAL,
	};

	class Slice : public SCAMERA
	{
	public:
		enum
		{
			CLIPPOINT_CNT = 4,
		};

	private:
		mip::TRANSFORM			m_clippingTransform;
		mip::MATRIX44			m_matOffset;

#ifdef USE_OPENGL
		std::vector<mip::VECTOR3> m_clippingBox;
		std::vector<mip::VECTOR3> m_clippingCoord;
#else
		mip::VECTOR3			m_clippingBox[CLIPPOINT_CNT];
		mip::SVertexMeshPT3		m_clippingRender[CLIPPOINT_CNT-1];
#endif
		mip::VECTOR3			m_offset;
		mip::VECTOR3			m_size;

		SLICE_TYPE				m_type;
		float					m_cameraDistance;
	public:
		Slice();
		~Slice();

#ifdef USE_OPENGL
		Slice(SLICE_TYPE type, const mip::VECTOR3 & size);
		bool					drawTexturePlan(Renderer * r, float fMin, float fMax, TEXTURE_ID volume_texture, TEXTURE_ID mask_texture, TEXTURE_ID color_preset);
		mip::VECTOR3			point2texcoord(mip::VECTOR3 & point);

		void setClippingPlane(SLICE_TYPE type, const mip::VECTOR3 & size);
#else
		Slice(SLICE_TYPE type, const mip::VECTOR2 & size);

		bool drawTexturePlan(Renderer * pRenderer, const DcmtkVolumeReader * pVolumeReader, bool test = false);
		mip::SVertexMeshPT3*	getClippingRender() { return &m_clippingRender[0]; }
		mip::VECTOR3*			getClippingBox() { return &m_clippingBox[0]; }

		void setClippingPlane(SLICE_TYPE type, const mip::VECTOR2 & size);
#endif
		void slide(float delta);
		void slide(const mip::VECTOR3 & dir);

		void addOffsetDelta();
		void updateSliceCamera();
		void updateSliceCamera(std::vector<VECTOR3> &points);
		bool checkPickingLine(const mip::SCAMERA * camera);
		
		mip::PLANE				getClippingPlane(const mip::MATRIX44 * toLocal = NULL, bool reverse = false);
		mip::TRANSFORM&			getClippingTransform() { return m_clippingTransform; }
		mip::TRANSFORM			getClippingTransform() const { return m_clippingTransform; }

		void addOffset(const mip::VECTOR3 & add) { m_offset += add; }
		void setOffset(const mip::VECTOR3 & set) { m_offset = set; }
		const mip::VECTOR3 getOffset() const	 { return m_offset; }
		SLICE_TYPE getType() const					 { return m_type; }
	};

};