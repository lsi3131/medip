#pragma once

#ifndef MOCK_MEDIPTYPE_H
#define MOCK_MEDIPTYPE_H

#include "System/MedipType.h"

class MockMedipType : public MedipType
{
public:
	MockMedipType() :
		MedipType(L"", L"")
	{}
public:
	virtual eAVAILABLE_STATE IsAvailableFunctionLevel(eMEDIP_FUNCTION_LEVEL serviceID, eAVAILABLE_STATE eInvisible = eAVAILABLE_STATE::CREATE) override;
	virtual void AddFunctionLevel(eMEDIP_FUNCTION_LEVEL serviceID, eAVAILABLE_STATE eInvisible) override;
	virtual void SetOnOffline(bool bOnline) override;
	virtual std::wstring GetProductInfo(std::string strKey) override;

};
#endif
