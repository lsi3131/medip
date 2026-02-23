#pragma once

#ifndef MEDIPTYPE_H
#define MEDIPTYPE_H

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include "FunctionLevel.h"

enum eAVAILABLE_STATE
{
	NON_CREATE = 0,
	CREATE,
	INVISIABLE,
	DISABLE,
};

extern const std::string PRODUCT_NAME_KEY;
extern const std::string PRODUCT_USE_KEY;

class MedipType
{
public:
	static std::shared_ptr<MedipType> New(const std::wstring& name, const std::wstring& use, bool isOnline);

public:	
	MedipType(const std::wstring& name, const std::wstring& use, bool isOnline = true);
	virtual ~MedipType();

public:
	virtual eAVAILABLE_STATE IsAvailableFunctionLevel(eMEDIP_FUNCTION_LEVEL serviceID, eAVAILABLE_STATE eInvisible = eAVAILABLE_STATE::CREATE);
	virtual void AddFunctionLevel(eMEDIP_FUNCTION_LEVEL serviceID, eAVAILABLE_STATE eInvisible);
	virtual void SetOnOffline(bool bOnline);
	virtual std::wstring GetProductInfo(std::string strKey);

public:
	std::unordered_map<eMEDIP_FUNCTION_LEVEL, eAVAILABLE_STATE> GetFunctionLevelMap() const;
		
protected:	
	std::unordered_map<eMEDIP_FUNCTION_LEVEL, eAVAILABLE_STATE> m_hashService;
	std::unordered_map<std::string, std::wstring> m_hashProductInfo;

protected:
	bool m_bIsOnline = true;
};

#endif