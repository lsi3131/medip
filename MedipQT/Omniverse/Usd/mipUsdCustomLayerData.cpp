#include "stdafx.h"
#include "mipUsdCustomLayerData.h"

#define CUSTOM_LAYER_DATA_NAME ("customLayerData")

mipUsdCustomLayerData::mipUsdCustomLayerData()
{
	//pxr::VtDictionary dic;
	//pxr::VtValue v(dic);
	//m_dictionary.SetValueAtPath(CUSTOM_LAYER_DATA_NAME, v);
}

mipUsdCustomLayerData::mipUsdCustomLayerData(const pxr::VtDictionary& dic) :
	m_dictionary(dic)
{
}

bool mipUsdCustomLayerData::SetRenderSetting(const mipUsdRenderSetting& setting)
{
	VtValue v(setting.GetDictionary());
	m_dictionary.SetValueAtPath("renderSettings", v);

	return true;
}

bool mipUsdCustomLayerData::TryGetRenderSetting(mipUsdRenderSetting* pOut)
{
	const VtValue* pValue = m_dictionary.GetValueAtPath("renderSettings");
	if (pValue == nullptr)
	{
		return false;
	}

	VtDictionary dic = pValue->Get<VtDictionary>();
	*pOut = mipUsdRenderSetting(dic);

	return true;
}

pxr::VtDictionary mipUsdCustomLayerData::GetDictionary() const
{
	return m_dictionary;
}

