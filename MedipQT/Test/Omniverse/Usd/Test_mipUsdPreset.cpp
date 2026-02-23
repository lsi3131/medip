#include "stdafx.h"
#include "Test/test_pch.h"
#include "Omniverse/Usd/mipUsdPreset.h"

class Test_mipUsdPreset : public ::testing::Test
{
public:
	void SetUp() override
	{
		QString testDir = GetUnitTestDataDirectory("Omniverse").c_str();
		m_testPresetTestDirPath_Root = testDir + "/preset_test";
		m_tempUsdaFilePath = testDir + "/usda/temp.usda";
		m_medipSampleFilePath_USD = testDir + "/usd/medip_sample.usd";
		
		QDir(m_testPresetTestDirPath_Root).removeRecursively();
		QDir().mkpath(m_testPresetTestDirPath_Root);
	}
	void TearDown() override
	{
	}

	void EXPECT_EQ_MESH_MATERIAL(const std::string& meshPath, const std::string& materialPath, const std::vector<mipPresetMeshMaterialPair>& pairList)
	{
		auto it = std::find_if(pairList.begin(), pairList.end(), [meshPath](const mipPresetMeshMaterialPair& d) { return d.MeshPath == meshPath; });
		if (it == pairList.end())
		{
			//Not Exist
			std::cout << "mesh path not exist : " << meshPath << std::endl;
			EXPECT_FALSE(true);
			return;
		}

		//EXPECT_STREQ(materialPath.c_str(), (*it).MaterialPath.c_str());
	}

	void EXPECT_MEDIP_SAMPLE_INTEGRATION(const mipUsdPreset& usdPreset)
	{
		std::vector<mipPresetMeshMaterialPair> meshMaterialPairList = usdPreset.GetMeshToMaterialPairList();
		EXPECT_NE(0, meshMaterialPairList.size());

		EXPECT_EQ_MESH_MATERIAL("/Root/Bone", "/Root/Looks/bone_OmniSurface_Plastic", meshMaterialPairList);
		EXPECT_EQ_MESH_MATERIAL("/Root/Skin", "/Root/Looks/skin_OmniSurface_Skin_2", meshMaterialPairList);
		EXPECT_EQ_MESH_MATERIAL("/Root/Lung", "/Root/Looks/lung_OmniSurface_Skin_1", meshMaterialPairList);
		EXPECT_EQ_MESH_MATERIAL("/Root/Artery_noresize_cop", "/Root/Looks/Aorta_OmniSurface_Blood", meshMaterialPairList);
		EXPECT_EQ_MESH_MATERIAL("/Root/Airway", "/Root/Looks/trachea_OmniSurface_Skin_4", meshMaterialPairList);
		EXPECT_EQ_MESH_MATERIAL("/Root/myocardium_L", "/Root/Looks/Myocardium_L_OmniSurface_Blood", meshMaterialPairList);
		EXPECT_EQ_MESH_MATERIAL("/Root/PulmArtery_noresize", "/Root/Looks/PA_OmniSurface_Blood", meshMaterialPairList);
		EXPECT_EQ_MESH_MATERIAL("/Root/PulmVein_noresize_c", "/Root/Looks/PV_OmniSurface_Blood", meshMaterialPairList);
		EXPECT_EQ_MESH_MATERIAL("/Root/myocardium_R", "/Root/Looks/Myocardium_R_OmniSurface_Blood", meshMaterialPairList);

		std::vector<mipUSDMaterialPtr> materialList = usdPreset.CreateMaterialList();
		EXPECT_STREQ("/Root/Looks/trachea_OmniSurface_Skin_4", materialList[0]->GetPath().c_str());
		EXPECT_STREQ("/Root/Looks/skin_OmniSurface_Skin_2", materialList[1]->GetPath().c_str());
		EXPECT_STREQ("/Root/Looks/lung_OmniSurface_Skin_1", materialList[2]->GetPath().c_str());
		EXPECT_STREQ("/Root/Looks/bone_OmniSurface_Plastic", materialList[3]->GetPath().c_str());
		EXPECT_STREQ("/Root/Looks/Aorta_OmniSurface_Blood", materialList[4]->GetPath().c_str());
		EXPECT_STREQ("/Root/Looks/Myocardium_L_OmniSurface_Blood", materialList[5]->GetPath().c_str());
		EXPECT_STREQ("/Root/Looks/PA_OmniSurface_Blood", materialList[6]->GetPath().c_str());
		EXPECT_STREQ("/Root/Looks/Myocardium_R_OmniSurface_Blood", materialList[7]->GetPath().c_str());
		EXPECT_STREQ("/Root/Looks/PV_OmniSurface_Blood", materialList[8]->GetPath().c_str());

		std::vector<mipUsdLightPtr> lightList = usdPreset.CreateLightList();
		EXPECT_STREQ("/Root/Environment/sky", lightList[0]->GetPath().c_str());
	}

protected:
	QString m_testPresetTestDirPath_Root;
	QString m_tempDirName;
	QString m_medipSampleFilePath_USD;
	QString m_tempUsdaFilePath;
};

TEST_F(Test_mipUsdPreset, Test_FileShouldBeExist)
{
	EXPECT_TRUE(QFile::exists(m_testPresetTestDirPath_Root));
}

TEST_F(Test_mipUsdPreset, WhenUsdFileNotExist_ReturnFalse)
{
	mipUsdPreset usdPreset;
	EXPECT_FALSE(usdPreset.OpenUsd("c:/invalid/file/path/notexist"));
}

TEST_F(Test_mipUsdPreset, WhenPresetDirNotExist_ReturnFalse)
{
	mipUsdPreset usdPreset;
	EXPECT_TRUE(usdPreset.OpenUsd(m_medipSampleFilePath_USD.toStdString()));
	EXPECT_FALSE(usdPreset.SaveToPresetFile("c:/invalid/dir/path/notexist", "preset"));
}

TEST_F(Test_mipUsdPreset, Test_LoadSampleUsdFile_And_SaveToFile)
{
	mipUsdPreset usdPreset;
	usdPreset.OpenUsd(m_medipSampleFilePath_USD.toStdString());
	EXPECT_TRUE(usdPreset.SaveToPresetFile(m_testPresetTestDirPath_Root.toStdString(), "preset"));
	EXPECT_TRUE(usdPreset.IsPresetFileLoaded());
	EXPECT_STREQ("preset", usdPreset.GetPresetName().c_str());

	EXPECT_TRUE(QFile::exists(m_testPresetTestDirPath_Root + "/preset"));
	EXPECT_TRUE(QFile::exists(m_testPresetTestDirPath_Root + "/preset/resource.usda"));
	EXPECT_TRUE(QFile::exists(m_testPresetTestDirPath_Root + "/preset/mesh_material_table.json"));
}

TEST_F(Test_mipUsdPreset, WhenPresetDirectoryHasNoData_ReturnFalse)
{
	mipUsdPreset usdPreset;
	QString testPresetDirPath = m_testPresetTestDirPath_Root + "/preset";
	EXPECT_FALSE(QFile::exists(testPresetDirPath));
	EXPECT_FALSE(usdPreset.LoadPreset(testPresetDirPath.toStdString()));
	EXPECT_FALSE(usdPreset.IsOpen());
}

TEST_F(Test_mipUsdPreset, TestLoadPreset)
{
	mipUsdPreset usdPreset_Save;
	QString testPresetDirPath = m_testPresetTestDirPath_Root + "/preset";

	usdPreset_Save.OpenUsd(m_medipSampleFilePath_USD.toStdString());
	usdPreset_Save.SaveToPresetFile(m_testPresetTestDirPath_Root.toStdString(), "preset");

	mipUsdPreset usdPreset_Load;
	EXPECT_TRUE(usdPreset_Load.LoadPreset(testPresetDirPath.toStdString()));
	EXPECT_TRUE(usdPreset_Load.IsPresetFileLoaded());
	EXPECT_STREQ("preset", usdPreset_Load.GetPresetName().c_str());

	EXPECT_MEDIP_SAMPLE_INTEGRATION(usdPreset_Load);
}

TEST_F(Test_mipUsdPreset, TestIntegration)
{
	mipUsdPreset usdPreset;
	usdPreset.OpenUsd(m_medipSampleFilePath_USD.toStdString());

	EXPECT_MEDIP_SAMPLE_INTEGRATION(usdPreset);
}

TEST_F(Test_mipUsdPreset, Test_ReadRenderSetting)
{
	mipUsdPreset usdPreset;
	usdPreset.OpenUsd(m_medipSampleFilePath_USD.toStdString());

	mipUsdRenderSetting renderSetting = usdPreset.GetRenderSetting();
	double hydraPointsDefaultWith = 0.0;
	bool backgroundComposite = true;
	mip::VECTOR3 backgroundDefaultColor;
	bool backgroundEnable = false;
	double histogramWhiteScale = 0.0;
	double wireFrameThickness = 0.0;

	EXPECT_EQ(mipUsdRenderSetting::PathTracing, renderSetting.GetRenderMode());

	EXPECT_TRUE(renderSetting.TryGet_Post_BackgroundZeroAlpha_Composite(&backgroundComposite));
	EXPECT_EQ(false, backgroundComposite);

	EXPECT_TRUE(renderSetting.TryGet_Post_BackgroundZeroAlpha_DefaultColor(&backgroundDefaultColor));
	EXPECT_EQ(mip::VECTOR3(1, 0.99999, 0.99999), backgroundDefaultColor);

	EXPECT_TRUE(renderSetting.TryGet_Post_BackgroundZeroAlpha_Enable(&backgroundEnable));
	EXPECT_EQ(true, backgroundEnable);

	EXPECT_TRUE(renderSetting.TryGet_Post_Histogram_WhiteScale(&histogramWhiteScale));
	EXPECT_NEAR(0.77, histogramWhiteScale, 0.001);

	EXPECT_TRUE(renderSetting.TryGet_Writeframe_Thickness(&wireFrameThickness));
	EXPECT_NEAR(0.099, wireFrameThickness, 0.001);
}

TEST_F(Test_mipUsdPreset, Test_WriteRenderSetting)
{
	mipUsdPreset usdPreset_Write;
	usdPreset_Write.OpenNew();

	mipUsdRenderSetting renderSetting_Write;
	renderSetting_Write.SetRenderMode(mipUsdRenderSetting::PathTracing);
	renderSetting_Write.Set_Hydra_Points_DefaultWidth(0.7);
	renderSetting_Write.Set_Post_BackgroundZeroAlpha_Composite(true);
	renderSetting_Write.Set_Post_BackgroundZeroAlpha_DefaultColor(mip::VECTOR3(0.1, 0.2, 0.3));
	renderSetting_Write.Set_Post_BackgroundZeroAlpha_Enable(true);
	renderSetting_Write.Set_Post_Histogram_WhiteScale(0.55);
	renderSetting_Write.Set_Writeframe_Thickness(0.1);

	usdPreset_Write.SetRenderSetting(renderSetting_Write);
	usdPreset_Write.SaveToPresetFile(m_testPresetTestDirPath_Root.toStdString(), "preset");

	QString testPresetDirPath = m_testPresetTestDirPath_Root + "/preset";
	mipUsdPreset usdPreset_Read;
	usdPreset_Read.LoadPreset(testPresetDirPath.toStdString());

	mipUsdRenderSetting renderSetting_Read = usdPreset_Read.GetRenderSetting();

	double hydraPointsDefaultWith = 0.0;
	bool backgroundComposite = true;
	mip::VECTOR3 backgroundDefaultColor;
	bool backgroundEnable = false;
	double histogramWhiteScale = 0.0;
	double wireFrameThickness = 0.0;

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
}

