#include "stdafx.h"
#include "Test/test_pch.h"
#include "ThreadEnd/ActionManager_ThreadEnd.h"
#include "WindowManager.h"
#include "ActionManager.h"
#include "ProductManager.h"
#include "Tabwindow.h"
#include "Windows/Tab/AISegTabDeepCatch.h"
#include "Test/System/ProductManagerTestUtil.h"
#include "Test/TestAICommon.h"
#include "Test/graphics/TestVolumeDataCommon.h"
#include "Test/TestContext.h"
#include "Actions/AI/WorkDeepPredictResult.h"

class TestActionManager_ThreadEnd : public ::testing::Test
{
public:
	void SetUp() override
	{
		m_pTest = TestContext::TestForMesh();
		m_pVolumeData = &m_pTest->pDataContext->volume_data;
		m_pActionManager = m_pTest->pActionManager.get();
		m_pWinManager = m_pTest->pWinManager.get();
		m_pProductManager = ProductManagerTestUtil::CreateEmpty();

		QDir().mkpath(STRING_MANAGER->cacheFilePath);

		m_pProductManager->AddFunctionLevel(MFL_Product_MEDIP_Plugin_AIPack, eAVAILABLE_STATE::CREATE);
		m_pAISegTab = new AISegTab(
			m_pVolumeData,
			m_pWinManager,
			m_pActionManager,
			m_pProductManager.get());
		m_pAISegTab->SetAIWeightType(MFL_Common_AI_AIPredict_Predict);
		m_threshold = m_pAISegTab->getOutVal();
	}
	void TearDown() override
	{
		if (m_pAISegTab)
		{
			delete m_pAISegTab;
		}

		QDir(STRING_MANAGER->cacheFilePath).removeRecursively();
	}

	ActionManager_ThreadEnd* CreateThreadEnd()
	{
		return new ActionManager_ThreadEnd(m_pVolumeData, m_pActionManager, m_pProductManager.get(), m_pAISegTab);
	}

	void InitActionParameter(const AIDataInfoList& aiDataInfoList, eMEDIP_FUNCTION_LEVEL weightType, eDeepPredictAICopyMask copyMask)
	{
		WorkDeepPredictResult result(&m_pActionManager->m_hashThreadResult);

		result.SetResult(
			aiDataInfoList.AINameList,
			aiDataInfoList.AIResultList,
			weightType,
			copyMask
		);
		m_pVolumeData->threadResult = aiDataInfoList.AINameList.size();
	}

protected:
	std::shared_ptr<TestContext> m_pTest;
	AISegTab* m_pAISegTab;
	VOLUME_DATA* m_pVolumeData;
	ActionManager* m_pActionManager;
	WindowManager* m_pWinManager;
	std::shared_ptr<ProductManager> m_pProductManager;
	int m_threshold;
};

TEST_F(TestActionManager_ThreadEnd, AISetTab_ShouldNotNull)
{
	EXPECT_NE(nullptr, m_pAISegTab->m_tabSet);
	EXPECT_EQ(128, m_pAISegTab->getOutVal());
}


TEST_F(TestActionManager_ThreadEnd, WhenThreadResultZero_ReturnFalse)
{
	ActionManager_ThreadEnd* threadEnd = CreateThreadEnd();
	m_pVolumeData->createData(2, 2, 1, 0.1f, 0.1f, 0.1f);
	m_pVolumeData->threadResult = 0;

	ActionManagerThreadEndResult result;
	EXPECT_FALSE(threadEnd->Handle(&result));
	EXPECT_FALSE(result.IsFreezeProject);
}

TEST_F(TestActionManager_ThreadEnd, WhenThreadResult_MinusOneErrorMessageReturn)
{
	ActionManager_ThreadEnd* threadEnd = CreateThreadEnd();
	m_pVolumeData->createData(2, 2, 1, 0.1f, 0.1f, 0.1f);
	m_pVolumeData->threadResult = -1;

	ActionManagerThreadEndResult result;
	EXPECT_FALSE(threadEnd->Handle(&result));
	EXPECT_FALSE(result.ErrorMessage.isEmpty());
	EXPECT_FALSE(result.IsFreezeProject);
}

TEST_F(TestActionManager_ThreadEnd, WhenThreadResult_MinusTwoErrorMessageReturn)
{
	ActionManager_ThreadEnd* threadEnd = CreateThreadEnd();
	m_pVolumeData->createData(2, 2, 1, 0.1f, 0.1f, 0.1f);
	m_pVolumeData->threadResult = -2;

	ActionManagerThreadEndResult result;
	EXPECT_FALSE(threadEnd->Handle(&result));
	EXPECT_FALSE(result.ErrorMessage.isEmpty());
	EXPECT_FALSE(result.IsFreezeProject);
}


TEST_F(TestActionManager_ThreadEnd, TestAddSingleAIData)
{
	ActionManager_ThreadEnd* threadEnd = CreateThreadEnd();
	m_pVolumeData->createData(2, 2, 1, 0.1f, 0.1f, 0.1f);

	AIDataInfoList aiDataInfoList;
	AddAIDataInfo(&aiDataInfoList, "test1", { 128, 128, 128, 128 });

	InitActionParameter(aiDataInfoList, MFL_Common_AI_PredictUsableCount_Credit, eDeepPredictAICopyMask::none_copy);

	ActionManagerThreadEndResult result;
	EXPECT_TRUE(threadEnd->Handle(&result));

	std::vector<MaskBitData> bitDatas = GetMaskBitDataList(m_pVolumeData);
	EXPECT_EQ(1, bitDatas.size());
	EXPECT_STREQ(L"test1", bitDatas[0].Info->maskName);
	EXPECT_EQ(AIClassResult({ 128, 128, 128, 128 }), bitDatas[0].GetAIResult().GetRaw());
}

TEST_F(TestActionManager_ThreadEnd, TestAddMultiAIData)
{
	ActionManager_ThreadEnd* threadEnd = CreateThreadEnd();
	m_pVolumeData->createData(2, 2, 1, 0.1f, 0.1f, 0.1f);

	AIDataInfoList aiDataInfoList;
	AddAIDataInfo(&aiDataInfoList, "test1", { 128, 128, 128, 128 });
	AddAIDataInfo(&aiDataInfoList, "test2", { 127, 127, 129, 129 });
	AddAIDataInfo(&aiDataInfoList, "test3", { 0, 255, 0, 255 });

	InitActionParameter(aiDataInfoList, MFL_Common_AI_PredictUsableCount_Credit, eDeepPredictAICopyMask::none_copy);

	ActionManagerThreadEndResult result;
	EXPECT_TRUE(threadEnd->Handle(&result));

	std::vector<MaskBitData> bitDatas = GetMaskBitDataList(m_pVolumeData);
	EXPECT_EQ(3, bitDatas.size());
	EXPECT_STREQ(L"test1", bitDatas[0].Info->maskName);
	EXPECT_STREQ(L"test2", bitDatas[1].Info->maskName);
	EXPECT_STREQ(L"test3", bitDatas[2].Info->maskName);

	EXPECT_EQ(AIClassResult({ 128, 128, 128, 128 }), bitDatas[0].GetAIResult().GetRaw());
	EXPECT_EQ(AIClassResult({ 127, 127, 129, 129 }), bitDatas[1].GetAIResult().GetRaw());
	EXPECT_EQ(AIClassResult({ 0, 255, 0, 255 }), bitDatas[2].GetAIResult().GetRaw());
}


