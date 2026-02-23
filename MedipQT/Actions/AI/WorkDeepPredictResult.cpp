#include "stdafx.h"
#include "WorkDeepPredictResult.h"
#include "ActionDefinitions.h"

using namespace std;

std::string WorkDeepPredictResult::KeyAIResult()
{
	return std::to_string((int)ACTP_DEEP_DRAW_PREDICT) + std::string("AI_result");
}

std::string WorkDeepPredictResult::KeyAIName()
{
	return std::to_string((int)ACTP_DEEP_DRAW_PREDICT) + std::string("AI_name");
}

std::string WorkDeepPredictResult::KeyWeightType()
{
	return std::to_string((int)ACTP_DEEP_DRAW_PREDICT) + std::string("WeightType");
}

std::string WorkDeepPredictResult::KeyCopyMask()
{
	return std::to_string((int)ACTP_DEEP_DRAW_PREDICT) + std::string("AI_copy_mask");
}

WorkDeepPredictResult::WorkDeepPredictResult(std::unordered_map<std::string, std::shared_ptr<void>>* pResult) :
	m_pResult(pResult)
{
}

void WorkDeepPredictResult::SetResult(
	const AIClassNameList& classNames,
	const AIClassResultList& classResults,
	eMEDIP_FUNCTION_LEVEL weightType,
	eDeepPredictAICopyMask copyMask)
{
	m_pResult->erase(KeyAIName());
	m_pResult->insert({ KeyAIName(), std::make_shared<AIClassNameList>(classNames) });

	m_pResult->erase(KeyAIResult());
	m_pResult->insert({ KeyAIResult(), std::make_shared<AIClassResultList>(classResults) });

	m_pResult->erase(KeyWeightType());
	m_pResult->insert({KeyWeightType(), std::make_shared<eMEDIP_FUNCTION_LEVEL>(weightType)});

	m_pResult->erase(KeyCopyMask());
	m_pResult->insert({ KeyCopyMask(), std::make_shared<eDeepPredictAICopyMask>(copyMask) });
}

AIClassResultList* WorkDeepPredictResult::GetClassResultList() const
{
	auto it = m_pResult->find(KeyAIResult());
	if (it != m_pResult->end())
	{
		return (AIClassResultList*)(*it).second.get();
	}
	return nullptr;
}

AIClassNameList* WorkDeepPredictResult::GetClassNameList() const
{
	auto it = m_pResult->find(KeyAIName());
	if (it != m_pResult->end())
	{
		return (AIClassNameList*)(*it).second.get();
	}
	return nullptr;
}

eMEDIP_FUNCTION_LEVEL WorkDeepPredictResult::GetWeightType() const
{
	auto it = m_pResult->find(KeyWeightType());
	if (it != m_pResult->end())
	{
		void* ptr = (*it).second.get();
		return *(eMEDIP_FUNCTION_LEVEL*)(*it).second.get();
	}
	return eMEDIP_FUNCTION_LEVEL::MFL_UpdateService_AI_Dll_AIDll;
}

eDeepPredictAICopyMask WorkDeepPredictResult::GetCopyMask() const
{
	auto it = m_pResult->find(KeyCopyMask());
	if (it != m_pResult->end())
	{
		void* ptr = (*it).second.get();
		return *(eDeepPredictAICopyMask*)(*it).second.get();
	}

	qWarning() << "fail to find copy mask type";
	return eDeepPredictAICopyMask::none_copy;
}
