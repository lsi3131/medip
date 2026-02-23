#include "stdafx.h"
#include "Test/test_pch.h"
#include "Omniverse/Usd/mipUsdCustomLayerData.h"
#include "Omniverse/Usd/mipUsdStage.h"
#include "Omniverse/OmniverseContext.h"
#include "Test/Omniverse/OmniverseFileTestResource.h"
#include "Test/Omniverse/Usd/UsdFileTestResource.h"
#include "Test/TestContext.h"

class Test_mipCustomLayerData : public ::testing::Test
{
public:
	void SetUp() override
	{
		QString testDir = GetUnitTestDataDirectory("Omniverse").c_str();
		m_tempUsdaFilePath = testDir + "/usda/temp.usda";
	}
	void TearDown() override
	{
	}

	void EXPECT_WRITE_READ_INTEGRATION(mipUsdStage& usdStage)
	{
		mipUsdCustomLayerData customLayerData = usdStage.GetCustomLayerData();

		mipUsdRenderSetting renderSetting_Write;
		renderSetting_Write.SetRenderMode(mipUsdRenderSetting::PathTracing);
		renderSetting_Write.Set_Hydra_Points_DefaultWidth(0.7);
		renderSetting_Write.Set_Post_BackgroundZeroAlpha_Composite(true);
		renderSetting_Write.Set_Post_BackgroundZeroAlpha_DefaultColor(mip::VECTOR3(0.1, 0.2, 0.3));
		renderSetting_Write.Set_Post_BackgroundZeroAlpha_Enable(true);
		renderSetting_Write.Set_Post_Histogram_WhiteScale(0.55);
		renderSetting_Write.Set_Writeframe_Thickness(0.1);

		customLayerData.SetRenderSetting(renderSetting_Write);

		mipUsdRenderSetting renderSetting_Read;

		double hydraPointsDefaultWith = 0.0;
		bool backgroundComposite = true;
		mip::VECTOR3 backgroundDefaultColor;
		bool backgroundEnable = false;
		double histogramWhiteScale = 0.0;
		double wireFrameThickness = 0.0;

		EXPECT_TRUE(customLayerData.TryGetRenderSetting(&renderSetting_Read));

		EXPECT_EQ(mipUsdRenderSetting::PathTracing, renderSetting_Read.GetRenderMode());

		EXPECT_TRUE(renderSetting_Read.TryGet_Hydra_Points_DefaultWidth(&hydraPointsDefaultWith));
		EXPECT_NEAR(0.7, hydraPointsDefaultWith, 0.001);

		EXPECT_TRUE(renderSetting_Read.TryGet_Post_BackgroundZeroAlpha_Composite(&backgroundComposite));
		EXPECT_EQ(true, backgroundComposite);

		EXPECT_TRUE(renderSetting_Read.TryGet_Post_BackgroundZeroAlpha_DefaultColor(&backgroundDefaultColor));
		EXPECT_EQ(mip::VECTOR3(0.1, 0.2, 0.3), backgroundDefaultColor);

		EXPECT_TRUE(renderSetting_Read.TryGet_Post_BackgroundZeroAlpha_Enable(&backgroundEnable));
		EXPECT_EQ(true, backgroundEnable);

		EXPECT_TRUE(renderSetting_Read.TryGet_Post_Histogram_WhiteScale(&histogramWhiteScale));
		EXPECT_NEAR(0.55, histogramWhiteScale, 0.001);

		EXPECT_TRUE(renderSetting_Read.TryGet_Writeframe_Thickness(&wireFrameThickness));
		EXPECT_NEAR(0.1, wireFrameThickness, 0.001);

		usdStage.SetCustomLayerData(customLayerData);
	}

protected:
	QString m_sampleUsdaFilePath;
	QString m_tempUsdaFilePath;

	OmniverseFileTestResource m_OmniFileResource;
	UsdFileTestResource m_UsdFileResource;
};

TEST_F(Test_mipCustomLayerData, TestDefaultValue)
{
	mipUsdCustomLayerData customLayerData;

	mipUsdRenderSetting renderSetting;
	EXPECT_FALSE(customLayerData.TryGetRenderSetting(&renderSetting));
}

TEST_F(Test_mipCustomLayerData, TestWriteAndReadRenderSetting_Local)
{
	mipUsdStage usdStage;
	usdStage.CreateNew(m_tempUsdaFilePath.toStdString());

	EXPECT_WRITE_READ_INTEGRATION(usdStage);

	usdStage.Save();
}

TEST_F(Test_mipCustomLayerData, TestWriteAndReadRenderSetting_Server)
{
	OmniverseContext omniContext;
	
	EXPECT_TRUE(omniContext.Connect(m_OmniFileResource.LocalIpAddress));
	EXPECT_TRUE(omniContext.CreateOmniverseStage(m_OmniFileResource.TempUsdPath));
	mipUsdStage* pUsdStage = omniContext.GetUsdStage();

	EXPECT_WRITE_READ_INTEGRATION(*pUsdStage);

	pUsdStage->Save();
}

TEST_F(Test_mipCustomLayerData, TestWriteExistRenderSetting_And_ReadRenderSetting_Local)
{
	mipUsdStage usdStage_Write;
	usdStage_Write.CreateNew(m_tempUsdaFilePath.toStdString());

	mipUsdCustomLayerData customLayerData = usdStage_Write.GetCustomLayerData();
	mipUsdRenderSetting renderSetting_Write;
	customLayerData.SetRenderSetting(renderSetting_Write);

	usdStage_Write.Save();

	mipUsdStage usdStage;
	usdStage.Open(m_tempUsdaFilePath.toStdString());

	EXPECT_WRITE_READ_INTEGRATION(usdStage);

	usdStage.Save();
}

