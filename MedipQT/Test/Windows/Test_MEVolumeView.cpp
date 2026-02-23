#include "stdafx.h"
#include "Test/test_pch.h"
#include "Windows/MEVolumeView.h"
#include "Test/TestContext.h"

class Test_MEVolumeView : public ::testing::Test
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

TEST_F(Test_MEVolumeView, TestConstructor)
{
	MEVolumeView volumeViewer(
		&m_pTest->pRenderer->Data,
		m_pTest->pWinManager.get(),
		m_pTest->pActionManager.get(),
		m_pTest->pDataContext.get());
}

