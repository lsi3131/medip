#pragma once

#include <memory>
#include "System/LicenseManager.h"
#include "System/ProductManager.h"

class LicenseManagerTestUtil
{
public:
	static std::shared_ptr<LicenseManager> CreateMEDIP(ProductManager* pProductManager);

};
