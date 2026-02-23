#include "stdafx.h"
#include "mipUsdLight.h"

mipUsdLight::mipUsdLight(const pxr::UsdLuxLight& raw) :
	m_raw(raw)
{
}

std::string mipUsdLight::GetPath() const
{
	return m_raw.GetPrim().GetPath().GetString();
}

std::string mipUsdLight::GetName() const
{
	return m_raw.GetPrim().GetName().GetString();
}

std::string mipUsdLight::GetTypeName() const
{
	return m_raw.GetPrim().GetTypeName().data();
}

pxr::UsdLuxLight* mipUsdLight::GetRaw()
{
	return &m_raw;
}

