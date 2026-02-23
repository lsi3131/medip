#include "stdafx.h"
#include "Test/test_pch.h"
#include "Windows/Tab/AISegTab.h"
#include "Test/System/MockProductManager.h"
#include "Test/TestContext.h"
#include "Test/System/ProductManagerTestUtil.h"

class TestAISegTab : public ::testing::Test
{
public:
	void SetUp() override
	{
		m_pTest = TestContext::TestForMesh();

		m_pVolumeData = &m_pTest->pDataContext->volume_data;
	}
	void TearDown() override
	{
	}

protected:
	std::shared_ptr<TestContext> m_pTest;
	VOLUME_DATA* m_pVolumeData;
};

TEST_F(TestAISegTab, TestEmptyFunctionLevel)
{
	std::shared_ptr<ProductManager> pProductManager = ProductManagerTestUtil::CreateEmpty();

	AISegTab* aiSegTab = new AISegTab(
		m_pVolumeData, 
		m_pTest->pWinManager.get(),
		m_pTest->pActionManager.get(),
		pProductManager.get());

	
	EXPECT_EQ(127, aiSegTab->getOutVal());
}
