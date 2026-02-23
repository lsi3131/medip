#include "stdafx.h"
#include "Test/test_pch.h"
#include "Omniverse/Usd/mipUSDMaterial.h"
#include "Omniverse/Usd/mipUsdStage.h"

class Test_mipUsdMaterial : public ::testing::Test
{
public:
	void SetUp() override
	{
		QString testDir = GetUnitTestDataDirectory("Omniverse").c_str();
		m_sampleUsdaFilePath = testDir + "/usda/material_sample.usda";
		m_tempUsdaFilePath = testDir + "/usda/temp.usda";
	}
	void TearDown() override
	{
	}

protected:
	QString m_sampleUsdaFilePath;
	QString m_tempUsdaFilePath;
};

TEST_F(Test_mipUsdMaterial, TestFileExist)
{
	EXPECT_TRUE(QFile::exists(m_sampleUsdaFilePath));
}

TEST_F(Test_mipUsdMaterial, TestSampleDataMaterial)
{
	mipUsdStage stage;
	EXPECT_TRUE(stage.Open(m_sampleUsdaFilePath.toStdString()));

	mipUSDMaterialPtr pMaterial = stage.CreateMeterialByPath("/Root/Looks/bone_OmniSurface_Plastic");
	EXPECT_STREQ("/Root/Looks/bone_OmniSurface_Plastic", pMaterial->GetPath().c_str());
}

