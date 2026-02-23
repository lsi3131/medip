#pragma once

#include <memory>
#include "System/ProductManager.h"
#include "FunctionLevel.h"

class ProductManagerTestUtil
{
public:
	static std::shared_ptr<ProductManager> CreateEmpty();
	static std::shared_ptr<ProductManager> CreateForAdmin();

	static void AddProductFunctionLevelList(ProductManager* pProductManager, std::vector<eMEDIP_FUNCTION_LEVEL> funcLevelList);
};
