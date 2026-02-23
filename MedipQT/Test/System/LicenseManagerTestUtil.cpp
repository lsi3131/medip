#include "stdafx.h"
#include "LicenseManagerTestUtil.h"

std::shared_ptr<LicenseManager> LicenseManagerTestUtil::CreateMEDIP(ProductManager* pProductManager)
{
	std::shared_ptr<LicenseManager> pNew = std::make_shared<LicenseManager>();
	pNew->SetProductManager(pProductManager);
	pNew->SetProductName(PRODUCT_NAME_MEDIP);

	return pNew;
}

