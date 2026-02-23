#pragma once

#ifndef TEST_AICOMMON_H
#define TEST_AICOMMON_H

#include "defineMEDIP.h"

struct AIDataInfo
{
	std::string AIName;
	AIClassResult AIResult;

	AIDataInfo(std::string name, AIClassResult result)
	{
		AIName = name;
		AIResult = result;
	}
};

struct AIDataInfoList
{
	AIClassNameList AINameList;
	AIClassResultList AIResultList;
};

AIDataInfo AddAIDataInfo(AIDataInfoList* pAIDataInfoList, std::string name, AIClassResult aiResult);


#endif