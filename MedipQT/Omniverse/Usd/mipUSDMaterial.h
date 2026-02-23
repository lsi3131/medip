#pragma once

#include <memory>
#include <string>
#include "Omniverse/OmniverseHeader.h"

class mipUSDMaterialPrivate;

class mipUSDMaterial
{
public:
	mipUSDMaterial(const pxr::UsdShadeMaterial& raw);

public:
	std::string GetName() const;
	std::string GetPath() const;

	std::string GetTypeName() const;

	pxr::UsdShadeMaterial* GetRaw() const;

private:
	std::shared_ptr<mipUSDMaterialPrivate> m_p;
};

using mipUSDMaterialPtr = std::shared_ptr<mipUSDMaterial>;
