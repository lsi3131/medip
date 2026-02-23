#pragma once

#include <pxr/base/vt/array.h>
#include <pxr/base/gf/vec3f.h>
#include <pxr/base/gf/vec3d.h>
#include "defineMEDIP.h"
#include "color.h"

/*
	OmniverseCommandManagerPrivate
*/
struct TransformSRT
{
	PXR_NS::GfVec3d Translate_mm;
	PXR_NS::GfVec3d Rotate;
	PXR_NS::GfVec3d Scale;
};

inline mip::VECTOR3 To_mipVECTOR3(PXR_NS::GfVec3f v)
{
	return mip::VECTOR3(v[0], v[1], v[2]);
}

inline mip::VECTOR3 To_mipVECTOR3(PXR_NS::GfVec3d v)
{
	return mip::VECTOR3(v[0], v[1], v[2]);
}

inline mip::QUATERNION To_mipQUATERNION(PXR_NS::GfVec3d v)
{
	return mip::QUATERNION(v[0], v[1], v[2], 1.0f);
}

inline PXR_NS::GfVec3d To_GfVec3d(mip::VECTOR3 v)
{
	return PXR_NS::GfVec3d(v[0], v[1], v[2]);
}

inline PXR_NS::GfVec3f To_GfVec3f(mip::VECTOR3 v)
{
	return PXR_NS::GfVec3f(v[0], v[1], v[2]);
}

inline COLOR To_mipCOLOR(PXR_NS::GfVec3f v)
{
	return COLOR(v[0] * 255.0f, v[1] * 255.0f, v[2] * 255.0f);
}

inline PXR_NS::GfVec3f To_GfVec3f_Color(COLOR v)
{
	return PXR_NS::GfVec3f(v.r / 255.0f, v.g / 255.0f, v.b / 255.0f);
}

inline mip::VECTOR4 To_mipVECTOR4_Color(COLOR v)
{
	return mip::VECTOR4(v.r / 255.0f, v.g / 255.0f, v.b / 255.0f, 1.0f);
}
