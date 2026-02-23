#include "stdafx.h"
#include "ProductManagerTestUtil.h"

std::shared_ptr<ProductManager> ProductManagerTestUtil::CreateEmpty()
{
	std::shared_ptr<ProductManager> pNew = std::make_shared<ProductManager>();

	AddProductFunctionLevelList(pNew.get(), {});

	return pNew;
}

std::shared_ptr<ProductManager> ProductManagerTestUtil::CreateForAdmin()
{
	std::shared_ptr<ProductManager> pNew = std::make_shared<ProductManager>();

	std::vector<eMEDIP_FUNCTION_LEVEL> funcLevelList;

	funcLevelList.insert(funcLevelList.end(), MODULE_FUNCTION_LEVEL_LIST, MODULE_FUNCTION_LEVEL_LIST + _countof(MODULE_FUNCTION_LEVEL_LIST));
	funcLevelList.insert(funcLevelList.end(), PRODUCT_FUNCTION_LEVEL_LIST, PRODUCT_FUNCTION_LEVEL_LIST + _countof(PRODUCT_FUNCTION_LEVEL_LIST));
	funcLevelList.insert(funcLevelList.end(), PLUGIN_FUNCTION_LEVEL_LIST, PLUGIN_FUNCTION_LEVEL_LIST + _countof(PLUGIN_FUNCTION_LEVEL_LIST));

	AddProductFunctionLevelList(pNew.get(), funcLevelList);

	return pNew;
}

void ProductManagerTestUtil::AddProductFunctionLevelList(ProductManager* pProductManager, std::vector<eMEDIP_FUNCTION_LEVEL> funcLevelList)
{
	bool online = true;
	std::shared_ptr<MedipType> pMedipType = std::make_shared<MedipType>(L"test", L"test", online);

	for (auto func : funcLevelList)
	{
		pMedipType->AddFunctionLevel(func, eAVAILABLE_STATE::CREATE);
	}

	pProductManager->SetProduct(pMedipType);
}

