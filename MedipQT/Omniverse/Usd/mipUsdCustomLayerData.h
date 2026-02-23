#pragma once

#include "Omniverse/Usd/mipUsdRenderSetting.h"

class mipUsdCustomLayerData
{
public:
	mipUsdCustomLayerData();
	mipUsdCustomLayerData(const pxr::VtDictionary& dic);

public:
	bool SetRenderSetting(const mipUsdRenderSetting& setting);
	bool TryGetRenderSetting(mipUsdRenderSetting* pOut);

public:
	pxr::VtDictionary GetDictionary() const;

private:
	pxr::VtDictionary m_dictionary;
};
