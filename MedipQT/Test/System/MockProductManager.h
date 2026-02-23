#pragma once

#include "System/ProductManager.h"
#include "Test/System/MockMedipType.h"

class MockProductManager : public ProductManager
{
public:
	MockProductManager(QString productName);
public:
	eAVAILABLE_STATE IsAvailableFunctionLevel(eMEDIP_FUNCTION_LEVEL eService) override;
	std::wstring GetProductInfo(std::string strKeyVal) override;
	Factory* getFactory() override;

private:
	QString m_productName;
	MockMedipType m_mockMedipType;
	Factory* m_pFactory;

};