#include "stdafx.h"
#include "Test/test_pch.h"
#include "Windows/Tab/MeshTab.h"
#include "Windows/WindowManager.h"
#include "Actions/ActionManager.h"
#include "Test/System/ProductManagerTestUtil.h"
#include "Test/TestContext.h"

class TestMeshTab : public ::testing::Test
{
public:
	void SetUp() override
	{
		m_pTest = TestContext::TestForMesh();
	}

	void TearDown() override
	{
	}

	MeshTab* Create(ProductManager* pProductManager)
	{
		return new MeshTab(
			m_pTest->pWinManager.get(),
			m_pTest->pActionManager.get(),
			pProductManager,
			m_pTest->pMeshModelView.get(),
			m_pTest->pMeshManipulator.get(),
			m_pTest->pShortcutManager.get()
		);
	}

protected:
	std::shared_ptr<TestContext> m_pTest;
};

TEST_F(TestMeshTab, TestConstructor)
{
	std::shared_ptr<ProductManager> pProductManager = ProductManagerTestUtil::CreateEmpty();
	MeshTab* meshTab = Create(pProductManager.get());

	meshTab->Update();
}
