#include "stdafx.h"
#include "mipUsdRenderSetting.h"

#define RENDER_MODE_REAL_TIME_TEXT ("RealTime")
#define RENDER_MODE_PATH_TRACING_TEXT ("PathTracing")

mipUsdRenderSetting::mipUsdRenderSetting()
{
}

mipUsdRenderSetting::mipUsdRenderSetting(const VtDictionary& renderSettingDictionary) :
	m_renderSettingDictionary(renderSettingDictionary)
{
}

mipUsdRenderSetting::eRenderMode mipUsdRenderSetting::GetRenderMode() const
{
	VtValue v;
	if (tryFindValue(&v, "rtx:rendermode") == false)
	{
		return eRenderMode::RealTime;
	}
	
	std::string renderMode = v.Get<TfToken>().GetString();
	if (renderMode == RENDER_MODE_PATH_TRACING_TEXT)
	{
		return eRenderMode::PathTracing;
	}
	else
	{
		return eRenderMode::RealTime;
	}
}

void mipUsdRenderSetting::SetRenderMode(eRenderMode value)
{
	if (value == eRenderMode::PathTracing)
	{
		VtValue vt(TfToken("PathTracing"));
		m_renderSettingDictionary.insert(std::make_pair("rtx:rendermode", vt));
	}
}

bool mipUsdRenderSetting::TryGet_Hydra_Points_DefaultWidth(double* pOut) const
{
	VtValue v;
	if (tryFindValue(&v, "rtx:hydra:points:defaultWidth") == false)
	{
		return false;
	}

	*pOut = v.Get<double>();

	return true;
}

void mipUsdRenderSetting::Set_Hydra_Points_DefaultWidth(double value)
{
	VtValue vt(value);
	m_renderSettingDictionary.insert(std::make_pair("rtx:hydra:points:defaultWidth", vt));
}

bool mipUsdRenderSetting::TryGet_Post_BackgroundZeroAlpha_Composite(bool* pOut) const
{
	VtValue v;
	if (tryFindValue(&v, "rtx:post:backgroundZeroAlpha:backgroundComposite") == false)
	{
		return false;
	}

	*pOut = v.Get<bool>();
	return true;
}

void mipUsdRenderSetting::Set_Post_BackgroundZeroAlpha_Composite(bool value)
{
	VtValue vt(value);
	m_renderSettingDictionary.insert(std::make_pair("rtx:post:backgroundZeroAlpha:backgroundComposite", vt));
}

bool mipUsdRenderSetting::TryGet_Post_BackgroundZeroAlpha_DefaultColor(mip::VECTOR3* pOut) const
{
	VtValue v;
	if (tryFindValue(&v, "rtx:post:backgroundZeroAlpha:backgroundDefaultColor") == false)
	{
		return false;
	}

	GfVec3f gfColor = v.Get<GfVec3f>();
	*pOut = mip::VECTOR3(gfColor[0], gfColor[1], gfColor[2]);

	return true;
}

void mipUsdRenderSetting::Set_Post_BackgroundZeroAlpha_DefaultColor(mip::VECTOR3 value)
{
	GfVec3f gfColor(value[0], value[1], value[2]);
	VtValue vt(gfColor);
	m_renderSettingDictionary.insert(std::make_pair("rtx:post:backgroundZeroAlpha:backgroundDefaultColor", vt));
}

bool mipUsdRenderSetting::TryGet_Post_BackgroundZeroAlpha_Enable(bool* pOut) const
{
	VtValue v;
	if (tryFindValue(&v, "rtx:post:backgroundZeroAlpha:enabled") == false)
	{
		return false;
	}

	*pOut = v.Get<bool>();
	return true;
}

void mipUsdRenderSetting::Set_Post_BackgroundZeroAlpha_Enable(bool value)
{
	VtValue vt(value);
	m_renderSettingDictionary.insert(std::make_pair("rtx:post:backgroundZeroAlpha:enabled", vt));
}

bool mipUsdRenderSetting::TryGet_Post_Histogram_WhiteScale(double* pOut) const
{
	VtValue v;
	if (tryFindValue(&v, "rtx:post:histogram:whiteScale") == false)
	{
		return false;
	}

	*pOut = v.Get<double>();
	return true;
}

void mipUsdRenderSetting::Set_Post_Histogram_WhiteScale(double value)
{
	VtValue vt(value);
	m_renderSettingDictionary.insert(std::make_pair("rtx:post:histogram:whiteScale", vt));
}

bool mipUsdRenderSetting::TryGet_Writeframe_Thickness(double* pOut) const
{
	VtValue v;
	if (tryFindValue(&v, "rtx:wireframe:wireframeThickness") == false)
	{
		return false;
	}

	*pOut = v.Get<double>();
	return true;
}

void mipUsdRenderSetting::Set_Writeframe_Thickness(double value)
{
	VtValue vt(value);
	m_renderSettingDictionary.insert(std::make_pair("rtx:wireframe:wireframeThickness", vt));
}

pxr::VtDictionary mipUsdRenderSetting::GetDictionary() const
{
	return m_renderSettingDictionary;
}

bool mipUsdRenderSetting::tryFindValue(VtValue* pValue, const std::string& key) const
{
	auto it = m_renderSettingDictionary.find(key);
	if (it == m_renderSettingDictionary.end())
	{
		return false;
	}

	*pValue = (*it).second;
	return true;
}


