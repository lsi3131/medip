#include "stdafx.h"
#include "Test/test_pch.h"
#include "UI/MaskList.h"
#include "Test/System/MockProductManager.h"
#include "graphics/volumedata.h"

class TestMaskListWidget : public ::testing::Test
{
public:
	void SetUp() override
	{
		m_pProductManager = new MockProductManager(PRODUCT_NAME_MEDIP);
		m_pVolumeData = new VOLUME_DATA();
	}
	void TearDown() override
	{
		delete m_pProductManager;
		delete m_pVolumeData;
	}

protected:
	MockProductManager* m_pProductManager;
	VOLUME_DATA* m_pVolumeData;
};

TEST_F(TestMaskListWidget, TestConstructor)
{
	MaskListWidget maskListWidet(m_pVolumeData, m_pProductManager);
}

TEST_F(TestMaskListWidget, WhenVolumeDataInvalid_UpdateByVolumeData_NotWork)
{
	MaskListWidget maskListWidet(m_pVolumeData, m_pProductManager);

	maskListWidet.UpdateByVolumeData();

	EXPECT_EQ(0, maskListWidet.topLevelItemCount());
}

TEST_F(TestMaskListWidget, WhenVolumeDataEmpty_MaskLayerItemEmpty)
{
	MaskListWidget maskListWidet(m_pVolumeData, m_pProductManager);

	m_pVolumeData->createData(2, 2, 2, 0.1f, 0.1f, 0.1f);

	maskListWidet.UpdateByVolumeData();

	EXPECT_EQ(0, maskListWidet.topLevelItemCount());
}

TEST_F(TestMaskListWidget, TestUpdateSingleMaskInfo)
{
	MaskListWidget maskListWidet(m_pVolumeData, m_pProductManager);

	m_pVolumeData->createData(2, 2, 2, 0.1f, 0.1f, 0.1f);
	m_pVolumeData->createMaskInfo();

	maskListWidet.UpdateByVolumeData();

	EXPECT_EQ(1, maskListWidet.topLevelItemCount());
}

TEST_F(TestMaskListWidget, TestUpdateMultiMaskInfo)
{
	MaskListWidget maskListWidet(m_pVolumeData, m_pProductManager);

	m_pVolumeData->createData(2, 2, 2, 0.1f, 0.1f, 0.1f);

	int count = 10;
	for (int i = 0; i < count; ++i)
	{
		m_pVolumeData->createMaskInfo();
	}

	maskListWidet.UpdateByVolumeData();

	EXPECT_EQ(10, maskListWidet.topLevelItemCount());
}

TEST_F(TestMaskListWidget, TestToggleShowColumn)
{
	MaskListWidget maskListWidet(m_pVolumeData, m_pProductManager);

	m_pVolumeData->createData(2, 2, 2, 0.1f, 0.1f, 0.1f);

	int count = 10;
	for (int i = 0; i < count; ++i)
	{
		m_pVolumeData->createMaskInfo();
	}

	maskListWidet.UpdateByVolumeData();

	EXPECT_EQ(10, maskListWidet.topLevelItemCount());
}

