#include "stdafx.h"
#include "Test/test_pch.h"
#include "System/ProductManager.h"
#include "Test/System/ProductManagerTestUtil.h"

class TestProductManager : public ::testing::Test
{
public:
	void SetUp() override
	{
	}
	void TearDown() override
	{
	}

protected:
};

TEST_F(TestProductManager, TestAIGoodMatchProcess)
{
	std::shared_ptr<ProductManager> pProduct = ProductManagerTestUtil::CreateEmpty();

	std::map<std::string, std::vector<ActionThreadArgument>> aiDataList = pProduct->m_mapAIGoodsMatchProcess;

	for (auto& keyValue : aiDataList) 
	{
		std::cout << "name : " << keyValue.first << std::endl;
		std::vector<ActionThreadArgument> argList = keyValue.second;
		for (auto& arg : argList)
		{
			std::string name = *(std::string*)arg.pTempData.get();
			std::cout << "\t name : " << name << ", action : " << arg.eCurrentThread <<", thread : " << arg.eNextThread <<  std::endl;
		}
	}

	EXPECT_EQ(50, aiDataList.size());
}
