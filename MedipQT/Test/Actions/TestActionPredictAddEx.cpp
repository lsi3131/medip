#include "stdafx.h"
#include "Test/test_pch.h"
#include "Actions/AI/ActionPredictAddEx.h"
#include "stringManager.h"
#include "MedipType.h"
#include "Test/graphics/TestVolumeDataCommon.h"
#include "Test/System/MockProductManager.h"
#include "Test/TestAICommon.h"

class TestActionPredictAddEx : public ::testing::Test
{
public:
	void SetUp() override
	{
		m_eMEDIPType = MFL_Common_AI_PredictUsableCount_Credit;
		m_isGMMAutoApply = false;
		m_pVolumeData = new VOLUME_DATA();
		m_pProductManager = new MockProductManager(PRODUCT_NAME_MEDIP);
		m_GPU = true;

		m_start = 0;
		m_end = 0;

		QDir().mkpath(STRING_MANAGER->cacheFilePath);
	}

	void TearDown() override
	{
		QDir(STRING_MANAGER->cacheFilePath).removeRecursively();
		delete m_pVolumeData;
		delete m_pProductManager;
	}

	ActionPredictAddEx* CreateActionMEDIP(const AIDataInfoList& aiDataInfoList, int threshold)
	{
		return new ActionPredictAddEx(
			m_pVolumeData,
			m_pProductManager,
			m_start,
			m_end,
			aiDataInfoList.AINameList.size(),
			threshold,
			aiDataInfoList.AINameList,
			aiDataInfoList.AIResultList,
			m_isGMMAutoApply,
			m_eMEDIPType
			);
	}

	bool IsEqualTo(MaskBitData& bitData, std::vector<mip::VECTOR3> posList, wchar_t* name, bool isAIDataExist)
	{
		return
			bitData.IsBitList(posList) &&
			(wcscmp(bitData.Info->maskName, name) == 0) &&
			(bitData.IsAIDataExist() == isAIDataExist);
	}


protected:
	int m_eMEDIPType;
	bool m_isGMMAutoApply;
	VOLUME_DATA* m_pVolumeData;
	MockProductManager* m_pProductManager;
	bool m_GPU;

	int m_start;
	int m_end;
};

TEST_F(TestActionPredictAddEx, TestFileShouldExist)
{
	EXPECT_TRUE(QFile::exists(STRING_MANAGER->cacheFilePath));
}

TEST_F(TestActionPredictAddEx, WhenAIDataEmpty_RedoDoesNotWork)
{
	m_pVolumeData->createData(2, 2, 1, 0.1f, 0.1f, 0.1f);

	AIDataInfoList aiInfoList;
	ActionPredictAddEx* pAction = CreateActionMEDIP(aiInfoList, 128);

	pAction->redo();

	EXPECT_EQ(0, m_pVolumeData->getMaskInfoListCnt());
}

TEST_F(TestActionPredictAddEx, TestSingleRedo)
{
	m_pVolumeData->createData(2, 2, 1, 0.1f, 0.1f, 0.1f);

	AIDataInfoList aiInfoList;
	AddAIDataInfo(&aiInfoList, "test1", { 128, 128, 127, 127 });

	ActionPredictAddEx* pAction = CreateActionMEDIP(aiInfoList, 128);

	pAction->redo();
	std::vector<mip::VECTOR3> posList = { {0,0,0}, {1,0,0}, };

	std::vector<MaskBitData> maskBitDataList = GetMaskBitDataList(m_pVolumeData);
	EXPECT_EQ(1, maskBitDataList.size());

	EXPECT_TRUE(IsEqualTo(maskBitDataList[0], posList, L"test1", true));
}

TEST_F(TestActionPredictAddEx, TestMultiRedo)
{
	m_pVolumeData->createData(2, 2, 1, 0.1f, 0.1f, 0.1f);

	AIDataInfoList aiInfoList;
	AddAIDataInfo(&aiInfoList, "test1", { 128, 128, 127, 127 });
	AddAIDataInfo(&aiInfoList, "test2", { 128, 128, 127, 127 });
	AddAIDataInfo(&aiInfoList, "test3", { 128, 128, 127, 127 });

	ActionPredictAddEx* pAction = CreateActionMEDIP(aiInfoList, 128);

	pAction->redo();

	std::vector<mip::VECTOR3> posList = { {0,0,0}, {1,0,0},};

	std::vector<MaskBitData> maskBitDataList = GetMaskBitDataList(m_pVolumeData);
	EXPECT_EQ(3, maskBitDataList.size());
	EXPECT_TRUE(IsEqualTo(maskBitDataList[0], posList, L"test1", true));
	EXPECT_TRUE(IsEqualTo(maskBitDataList[1], posList, L"test2", true));
	EXPECT_TRUE(IsEqualTo(maskBitDataList[2], posList, L"test3", true));
}

