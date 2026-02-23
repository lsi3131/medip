#pragma once

#include "Math/Vector.h"
#include "Omniverse/OmniverseHeader.h"

class mipUsdRenderSetting
{
public:
	enum eRenderMode
	{
		RealTime,
		PathTracing,
	};

public:
	mipUsdRenderSetting();
	mipUsdRenderSetting(const pxr::VtDictionary& renderSettingDictionary);

public:
	eRenderMode GetRenderMode() const;
	void SetRenderMode(eRenderMode value);

	bool TryGet_Hydra_Points_DefaultWidth(double* pOut) const;
	void Set_Hydra_Points_DefaultWidth(double value);

	bool TryGet_Post_BackgroundZeroAlpha_Composite(bool* pOut) const;
	void Set_Post_BackgroundZeroAlpha_Composite(bool value);

	bool TryGet_Post_BackgroundZeroAlpha_DefaultColor(mip::VECTOR3* pOut) const;
	void Set_Post_BackgroundZeroAlpha_DefaultColor(mip::VECTOR3 value);

	bool TryGet_Post_BackgroundZeroAlpha_Enable(bool* pOut) const;
	void Set_Post_BackgroundZeroAlpha_Enable(bool value);

	bool TryGet_Post_Histogram_WhiteScale(double* pOut) const;
	void Set_Post_Histogram_WhiteScale(double value);

	bool TryGet_Writeframe_Thickness(double* pOut) const;
	void Set_Writeframe_Thickness(double value);

public:
	pxr::VtDictionary GetDictionary() const;

private:
	bool tryFindValue(VtValue* pValue, const std::string& key) const;

private:
	pxr::VtDictionary m_renderSettingDictionary;

};
