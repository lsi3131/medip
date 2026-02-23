#pragma once

#include <memory>
#include <vector>
#include <string>
#include "defineMEDIP.h"

class WindowManager;
class DataContext;
class ProductManager;
class ActionManager;
class LicenseManager;

class AISegTab;
class AISegTabDeepCatch;

class ActionThreadEndDeepDrawPredict
{
public:
	ActionThreadEndDeepDrawPredict(
		WindowManager* pWinManager,
		DataContext* pDataContext,
		ProductManager* pProductManager,
		LicenseManager* pLicenseManager,
		ActionManager* pActionManager);

public:
	bool Do();

private:
	bool canAddNewMask() const;
	bool addNewMask();

	bool isNewMaskExceedCount() const;
	void handleExceedMaxMask();
	void handleAddNewMask();

	bool isDeepCatchProduct() const;
	bool isMEDIPProduct() const;

	void handleAddNewMask_DeepCatch();
	void handleAddNewMask_MEDIP();

	bool isTrunkPredict() const;
	void handleDeepCatch_AddPredict_Trunk();
	void handleDeepCatch_AddPredict();
	void handleDeepCatch_CompleteWork();

	void handleDeepCatch_CompleteWork_Report_V2();
	void handleDeepCatch_CompleteWork_Report();

	void enableAIReletedUI();

private:
	WindowManager* m_pWinManager;
	DataContext* m_pDataContext;
	ProductManager* m_pProductManager;
	LicenseManager* m_pLicenseManager;
	ActionManager* m_pActionManager;

	std::vector<std::string> m_vecAIName;
	std::vector<std::vector<unsigned char>> m_vecAIResult;
	int m_weightType;

	int m_addedNewMaskCount;

	AISegTab* m_pAISegTab;
	AISegTabDeepCatch* m_pDeepCatchTab;

	int m_start = 0;
	int m_end = 0;
	int m_threshold = 0;
};
