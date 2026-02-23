#pragma once
#include "Omniverse/OmniverseHeader.h"

class mipUSDMaterialPrivate
{
public:
	mipUSDMaterialPrivate(const pxr::UsdShadeMaterial& raw);

public:
	pxr::UsdShadeMaterial Raw;
	std::string Name;
};
