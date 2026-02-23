#pragma once

#include <string>
#include <unordered_map>
#include "defineMEDIP.h"
#include "FunctionLevel.h"

class WorkDeepPredictResult
{
public:
	static std::string KeyAIResult();
	static std::string KeyAIName();
	static std::string KeyWeightType();
	static std::string KeyCopyMask();

public:
	WorkDeepPredictResult(std::unordered_map<std::string, std::shared_ptr<void>>* pResult);

	void SetResult(
		const AIClassNameList& classNames,
		const AIClassResultList& classResults,
		eMEDIP_FUNCTION_LEVEL weightType,
		eDeepPredictAICopyMask copyMask
	);

	AIClassResultList* GetClassResultList() const;
	AIClassNameList* GetClassNameList() const;
	eMEDIP_FUNCTION_LEVEL GetWeightType() const;
	eDeepPredictAICopyMask GetCopyMask() const;

private:
	std::unordered_map<std::string, std::shared_ptr<void>>* m_pResult;

};
