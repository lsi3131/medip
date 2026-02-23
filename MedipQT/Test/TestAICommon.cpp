#include "stdafx.h"
#include "TestAICommon.h"

AIDataInfo AddAIDataInfo(AIDataInfoList* pAIDataInfoList, std::string name, AIClassResult aiResult)
{
	pAIDataInfoList->AINameList.push_back(name);
	pAIDataInfoList->AIResultList.push_back(aiResult);

	return AIDataInfo(name, aiResult);
}

