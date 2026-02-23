#include "stdafx.h"
#include "Test/test_pch.h"
#include "ThreadEnd/ActionThreadEndDeepDrawPredict.h"
#include "Test/TestContext.h"
#include "Test/System/ProductManagerTestUtil.h"
#include "Test/System/LicenseManagerTestUtil.h"

class TestActionThreadEndDeepDrawPredict : public ::testing::Test
{
public:
	void SetUp() override
	{
		m_pTest = TestContext::TestForMesh();
	}
	void TearDown() override
	{
	}

protected:
	std::shared_ptr<TestContext> m_pTest;
};

TEST_F(TestActionThreadEndDeepDrawPredict, TestConstructor)
{
	std::shared_ptr<ProductManager> pProduct = ProductManagerTestUtil::CreateForAdmin();
	std::shared_ptr<LicenseManager> pLicense = LicenseManagerTestUtil::CreateMEDIP(pProduct.get());

	ActionThreadEndDeepDrawPredict action(
		m_pTest->pWinManager.get(),
		m_pTest->pDataContext.get(),
		pProduct.get(),
		pLicense.get(),
		m_pTest->pActionManager.get()
	);

	//EXPECT_TRUE(action.Do());
}
