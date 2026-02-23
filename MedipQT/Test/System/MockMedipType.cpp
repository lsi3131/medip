#include "stdafx.h"
#include "MockMedipType.h"

eAVAILABLE_STATE MockMedipType::IsAvailableFunctionLevel(eMEDIP_FUNCTION_LEVEL serviceID, eAVAILABLE_STATE eInvisible)
{
	return eAVAILABLE_STATE::CREATE;
}

void MockMedipType::AddFunctionLevel(eMEDIP_FUNCTION_LEVEL serviceID, eAVAILABLE_STATE eInvisible)
{
}

void MockMedipType::SetOnOffline(bool bOnline)
{
}

std::wstring MockMedipType::GetProductInfo(std::string strKey)
{
	return L"MEDIP";
}
