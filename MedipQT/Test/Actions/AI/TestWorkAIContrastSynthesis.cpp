#include "stdafx.h"
#include "Test/test_pch.h"
#include "Actions/AI/WorkAIContrastSynthesis.h"
#include "Test/TestContext.h"

class TestWorkAIContrastSynthesis : public ::testing::Test
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

TEST_F(TestWorkAIContrastSynthesis, TestRunEmptyVolume)
{
	WorkAIContrastSynthesis work(m_pVolumeData, m_pActionManager, eAIContrastSynthesisOption::NonContrast, true);

	work.threadRun();

	std::shared_ptr<std::vector<mint16>> pResult = PopActionThreadResult<std::vector<mint16>>(m_pActionManager, ACTP_AI_CONTRAST_SYNTHESIS_PREDICT);
	EXPECT_EQ(nullptr, pResult);
}

TEST_F(TestWorkAIContrastSynthesis, TestSimplestRun)
{
	//현재 512 x 512로 W x H 값이 fix되어 있음
	int width = 512;
	int height = 512;
	int depth = 10;
	m_pVolumeData->create3DHUData(width, height, depth, 0.1f, 0.1f, 0.1f);
	WorkAIContrastSynthesis work(m_pVolumeData, m_pActionManager, eAIContrastSynthesisOption::NonContrast, true);

	work.threadRun();

	std::shared_ptr<std::vector<mint16>> pResult = PopActionThreadResult<std::vector<mint16>>(m_pActionManager, ACTP_AI_CONTRAST_SYNTHESIS_PREDICT);
	EXPECT_NE(nullptr, pResult);

	EXPECT_EQ(width * height * depth, pResult->size());
}

TEST_F(TestWorkAIContrastSynthesis, Test_CheckBoundaryOfWidthAndHeight)
{
	//현재 512 x 512로 W x H 값이 fix되어 있음
	std::shared_ptr<std::vector<mint16>> pResult;

	m_pVolumeData->create3DHUData(511, 511, 1, 0.1f, 0.1f, 0.1f);
	WorkAIContrastSynthesis work1(m_pVolumeData, m_pActionManager, eAIContrastSynthesisOption::NonContrast, true);
	work1.threadRun();

	pResult = PopActionThreadResult<std::vector<mint16>>(m_pActionManager, ACTP_AI_CONTRAST_SYNTHESIS_PREDICT);
	EXPECT_EQ(nullptr, pResult);

	m_pVolumeData->create3DHUData(513, 513, 1, 0.1f, 0.1f, 0.1f);
	WorkAIContrastSynthesis work2(m_pVolumeData, m_pActionManager, eAIContrastSynthesisOption::NonContrast, true);
	work2.threadRun();

	pResult = PopActionThreadResult<std::vector<mint16>>(m_pActionManager, ACTP_AI_CONTRAST_SYNTHESIS_PREDICT);
	EXPECT_EQ(nullptr, pResult);
}

#endif
