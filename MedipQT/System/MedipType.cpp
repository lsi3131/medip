#include "stdafx.h"
#include "MedipType.h"
#include "define.h"

const std::string PRODUCT_NAME_KEY = "ProductName";
const std::string PRODUCT_USE_KEY = "ProductUse";

std::shared_ptr<MedipType> MedipType::New(const std::wstring& name, const std::wstring& use, bool isOnline)
{
	return std::make_shared<MedipType>(name, use, isOnline);
}

MedipType::MedipType(const std::wstring& name, const std::wstring& use, bool isOnline)
{
	m_bIsOnline = isOnline;

	m_hashProductInfo[PRODUCT_NAME_KEY] = name;
	m_hashProductInfo[PRODUCT_USE_KEY] = use;
}

MedipType::~MedipType()
{
}

eAVAILABLE_STATE MedipType::IsAvailableFunctionLevel(eMEDIP_FUNCTION_LEVEL serviceID, eAVAILABLE_STATE eInvisible)
{
	eAVAILABLE_STATE result = eAVAILABLE_STATE::NON_CREATE;
	if (m_hashService.count(serviceID) > 0)
	{
		if (m_hashService[serviceID] == eAVAILABLE_STATE::INVISIABLE)
			result = eAVAILABLE_STATE::INVISIABLE;
		else if (m_hashService[serviceID] == eAVAILABLE_STATE::NON_CREATE)
			result = eAVAILABLE_STATE::NON_CREATE;
		else if (m_hashService[serviceID] == eAVAILABLE_STATE::DISABLE)
			result = eAVAILABLE_STATE::DISABLE;
		else
			result = eAVAILABLE_STATE::CREATE;
	}		

	return result;
}

void MedipType::AddFunctionLevel(eMEDIP_FUNCTION_LEVEL serviceID, eAVAILABLE_STATE eInvisible)
{
	m_hashService[serviceID] = eInvisible;
}

void MedipType::SetOnOffline(bool bOnline)
{
	m_bIsOnline = bOnline;
}

std::wstring MedipType::GetProductInfo(std::string strKey)
{
	return m_hashProductInfo[strKey];
}

std::unordered_map<eMEDIP_FUNCTION_LEVEL, eAVAILABLE_STATE> MedipType::GetFunctionLevelMap() const
{
	return m_hashService;
}
