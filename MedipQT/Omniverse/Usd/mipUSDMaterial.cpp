#include "stdafx.h"
#include "mipUSDMaterial.h"
#include "mipUsdMaterial_p.h"

mipUSDMaterial::mipUSDMaterial(const pxr::UsdShadeMaterial& raw)
{
	m_p = std::make_shared<mipUSDMaterialPrivate>(raw);
}

pxr::UsdShadeMaterial* mipUSDMaterial::GetRaw() const
{
	return &m_p->Raw;
}

std::string mipUSDMaterial::GetName() const
{
	return m_p->Raw.GetPrim().GetName().GetString();
}

std::string mipUSDMaterial::GetPath() const
{
	return m_p->Raw.GetPath().GetString();
}

std::string mipUSDMaterial::GetTypeName() const
{
	return m_p->Raw.GetPrim().GetTypeName().data();
}

