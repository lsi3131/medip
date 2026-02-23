#pragma once

#include <string>
#include <memory>
#include "Omniverse/OmniverseHeader.h"

class mipUsdLight
{
public:
	mipUsdLight(const pxr::UsdLuxLight& raw);

public:
	std::string GetPath() const;
	std::string GetName() const;
	
	std::string GetTypeName() const;

	pxr::UsdLuxLight* GetRaw();

private:
	pxr::UsdLuxLight m_raw;
};

using mipUsdLightPtr = std::shared_ptr<mipUsdLight>;
