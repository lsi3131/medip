#pragma once

#include "System/MedipType.h"
#include "ActionManager.h"

class Factory;
class MedipTypeString;
class AIProject;

class ProductManager
{
public:
	const std::string m_strMEDIP = "MEDIP";
	const std::string m_strMedipPro = "Pro";
	const std::string m_strMedipResearch = "Research";
	const std::string m_strMedipAcademy = "Light";
	const std::string m_strMedipMDBox = "MDBox";

	const std::string m_strMedipDeepCatch = "DeepCatch";
	const std::string m_strMedipDeepCatchV2 = "DeepCatch V2";
	const std::string m_strMedipCOVID19 = "COVID19";

	const std::string m_strMedipAI = "MEDIP AI";
	const std::string m_strMedipDemo = "Demo";

	//const std::string m_strDeepCatchWholebodyDemo = "[Demo] Wholebody";
	//const std::string m_strDeepCatchWholebody = "Wholebody & Abdomen";
	const std::string m_strDeepCatchAbdomen = "Abdomen";
	const std::string m_strDeepCatchChest = "Chest";
	const std::string m_strDeepDrawPredict_MEDIP_CREDIT = "MEDIP_CREDIT";

public:
	static ProductManager* getSingleton();

public:
	ProductManager();
	~ProductManager();

public:
	virtual std::wstring GetProductInfo(std::string strKeyVal);
	virtual std::string GetProductInfo_StdString(std::string strKeyVal);
	virtual eAVAILABLE_STATE IsAvailableFunctionLevel(eMEDIP_FUNCTION_LEVEL eService);
	virtual Factory* getFactory();

public:
	void SetProduct(std::shared_ptr<MedipType> pType);
	MedipType* GetProduct() const;

	void AddFunctionLevel(eMEDIP_FUNCTION_LEVEL eService, eAVAILABLE_STATE eState);

	void initProductResource(const std::string& strSettingMedipType);

	QString GetInfomation_Text_MEDIP(const QString& strProductDI, const QString& strProductPI);
	QString GetInfomation_Text_MEDIP_Etc();
	QString GetInfomation_Text_MEDIP_COVID19();
	QString GetInfomation_Text_DeepCatch(const QString& strProductDI, const QString& strProductPI);
	QString GetInfomation_Text_DeepCatch_V2(const QString& strProductDI, const QString& strProductPI);

private:
	void initAIGoodMatchProcess();
	ActionThreadArgument toThreadArgument(const AIProject& aiProject);

public:
	std::map<unsigned int, std::vector<std::vector<int>>> m_mapAI_Salt;

	std::map<std::string, std::vector<ActionThreadArgument>> m_mapAIGoodsMatchProcess;

	std::map<std::string, std::vector<ActionThreadArgument>> m_mapAIResearchMatchProcess;

private:
	std::shared_ptr<MedipType> m_pMedipType = nullptr;
	std::shared_ptr<Factory> m_pFactoryInstance = nullptr;
};

#define PRODUCT_MANAGER (ProductManager::getSingleton())
#define PRODUCT_FACTORY (ProductManager::getSingleton()->getFactory())


class Factory
{
public:
	Factory(ProductManager* pProductManager, MedipType* pType);
	~Factory() {};

	template<class T>
	T* createObject(eMEDIP_FUNCTION_LEVEL eFuncName, QObject* pObject)
	{
		if (m_pMedipType == nullptr)
			return nullptr;

		eAVAILABLE_STATE eState = m_pMedipType->IsAvailableFunctionLevel(eFuncName);
		if (eState == eAVAILABLE_STATE::NON_CREATE)
		{
			return nullptr;
		}
		else if (eState == eAVAILABLE_STATE::CREATE)
		{
			if (pObject != nullptr)
				return new T(pObject);
		}

		return nullptr;
	}

	template<class T>
	T* createWidget(eMEDIP_FUNCTION_LEVEL eFuncName, QWidget* pWidget)
	{
#ifdef USE_UNIT_TEST
		eAVAILABLE_STATE eState = eAVAILABLE_STATE::CREATE;
#else
		if (m_pMedipType == nullptr)
			return nullptr;

		eAVAILABLE_STATE eState = m_pMedipType->IsAvailableFunctionLevel(eFuncName);
#endif
		if (eState == eAVAILABLE_STATE::NON_CREATE)
		{
			return nullptr;
		}
		else if (eState == eAVAILABLE_STATE::CREATE)
		{
			if (pWidget == nullptr)
			{
				return new T();
			}
			else
			{
				return new T(pWidget);
			}
		}
		else if (eState == eAVAILABLE_STATE::INVISIABLE)
		{
			if (pWidget != nullptr)
			{
				T* pTmp = new T(pWidget);
				pTmp->hide();
				return pTmp;
			}
		}
		else if (eState == eAVAILABLE_STATE::DISABLE)
		{
			if (pWidget != nullptr)
			{
				T* pTmp = new T(pWidget);
				pTmp->setEnabled(false);
				return pTmp;
			}
		}

		return nullptr;
	}

	void initMedipTypeString(void);

private:
	ProductManager* m_pProductManager;
	MedipType* m_pMedipType = nullptr;
	MedipTypeString* m_pTypeString = nullptr;

};
