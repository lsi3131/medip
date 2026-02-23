#include "stdafx.h"
#include "Test/test_pch.h"
#include "Actions/AI/WorkAIKernelNeutralization.h"
#include "Test/TestContext.h"

class TestWorkAIKernelNeutralization : public ::testing::Test
{
public:
	void SetUp() override
	{
		m_pTest = TestContext::TestForVolume();
		m_pVolumeData = &m_pTest->pDataContext->volume_data;
		m_pActionManager = m_pTest->pActionManager.get();
	}
	void TearDown() override
	{
	}

protected:
	std::shared_ptr<TestContext> m_pTest;
	VOLUME_DATA* m_pVolumeData;
	ActionManager* m_pActionManager;
};

#ifndef _DEBUG
TEST_F(TestWorkAIKernelNeutralization, TestRunEmptyVolume)
{
	WorkAIKernelNeutralization work(m_pVolumeData, m_pActionManager);

	work.threadRun();

	//ACTP_AI_KERNEL_NEUTRALIZATION_CONVERSION_PREDICT
	std::shared_ptr<std::vector<mint16>> pResult = PopActionThreadResult<std::vector<mint16>>(m_pActionManager, ACTP_AI_KERNEL_NEUTRALIZATION_CONVERSION_PREDICT);
	EXPECT_EQ(nullptr, pResult);
}

TEST_F(TestWorkAIKernelNeutralization, TestRunSimple_BoundaryTest)
{
	//현재 52 x 52 이상인 경우에만 정상동작한다.
	int width = 52;
	int height = 52;
	m_pVolumeData->create3DHUData(width, height, 100, 0.1f, 0.1f, 0.1f);
	WorkAIKernelNeutralization work(m_pVolumeData, m_pActionManager);

	work.threadRun();

	std::shared_ptr<std::vector<mint16>> pResult = PopActionThreadResult<std::vector<mint16>>(m_pActionManager, ACTP_AI_KERNEL_NEUTRALIZATION_CONVERSION_PREDICT);
	EXPECT_NE(nullptr, pResult);

	EXPECT_EQ(512 * 512, pResult->size());
	//EXPECT_EQ(width * height, pResult->size());
}

#endif
