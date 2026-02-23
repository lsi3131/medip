#pragma once

#ifndef ACTION_MANAGER_THREADEND_H
#define ACTION_MANAGER_THREADEND_H

#include "stringManager.h"

class VOLUME_DATA;
class WindowManager;
class ActionManager;
class ProductManager;
class AISegTab;
class AISetTab;

struct ActionManagerThreadEndResult
{
	bool IsFreezeProject;
	bool IsFreezeDeepCatchProject;
	bool IsPredictComplete;
	QString ErrorMessage;

	ActionManagerThreadEndResult() :
		IsFreezeProject(true),
		IsFreezeDeepCatchProject(true),
		IsPredictComplete(false)
	{
	}
};

class ActionManager_ThreadEnd
{
public:
	ActionManager_ThreadEnd(VOLUME_DATA* pVolumeData, ActionManager* pActionManager, ProductManager* pProductManager, AISegTab* aiSegTab);

public:
	bool Handle(ActionManagerThreadEndResult* pOutResult);

	bool ShouldFreezeProject() const;

private:
	bool initParameterFromActionHashMap();
	bool isExceedMaxMaskCount(int newAIResultCount);
	void handle_ExceedMaskCount(ActionManagerThreadEndResult* pOutResult);
	bool handle_DeepCatch(ActionManagerThreadEndResult* pOutResult);
	bool handle_MEDIP(ActionManagerThreadEndResult* pOutResult);

	void clearResult(ActionManagerThreadEndResult* pOutResult, const QString& errorMessage);
	QString readAIErrorLogFile();

	bool isGMMAutoApply() const;

private:
	VOLUME_DATA* m_pVolumeData;
	ActionManager* m_pActionManager;
	ProductManager* m_pProductManager;
	AISegTab* m_aiSegTab;
	AISetTab* m_aiSubSetTab;

	int m_start;
	int m_end;
	int m_threshold;
	int m_result;

	AIClassNameList m_AINameList;
	AIClassResultList m_AIResultList;
	int m_weightType;
};
#endif