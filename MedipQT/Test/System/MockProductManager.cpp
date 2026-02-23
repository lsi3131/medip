#include "stdafx.h"
#include "MockProductManager.h"

MockProductManager::MockProductManager(QString productName) :
	m_productName(productName)
{
	m_pFactory = new Factory(this, &m_mockMedipType);
}

eAVAILABLE_STATE MockProductManager::IsAvailableFunctionLevel(eMEDIP_FUNCTION_LEVEL eService)
{
	if (eService == MFL_Product_DeepCatch)
	{
		if (m_productName == PRODUCT_NAME_DEEPCATCH)
		{
			return eAVAILABLE_STATE::CREATE;
		}
	}
	else
	{
		return eAVAILABLE_STATE::CREATE;
	}
	return eAVAILABLE_STATE::NON_CREATE;
}

std::wstring MockProductManager::GetProductInfo(std::string strKeyVal)
{
	return m_productName.toStdWString();
}

Factory* MockProductManager::getFactory()
{
	return m_pFactory;
}


