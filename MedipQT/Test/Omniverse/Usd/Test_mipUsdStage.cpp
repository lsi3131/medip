#include "stdafx.h"
#include "Test/test_pch.h"
#include "Omniverse/Usd/mipUsdStage.h"
#include "Test/Omniverse/Usd/UsdFileTestResource.h"
#include "Test/TestContext.h"

class Test_mipUsdStage : public ::testing::Test
{
public:
	void SetUp() override
	{
		QFile::remove(m_usdFileResource.TempFilePath_Usda.c_str());
		QFile::remove(m_usdFileResource.TempFilePath_USD.c_str());

		m_pTest = TestContext::TestForMesh();
		m_pRenderer = &m_pTest->pRenderer->Data;
	}

	void TearDown() override
	{
	}

protected:
	UsdFileTestResource m_usdFileResource;
	std::shared_ptr<TestContext> m_pTest;
	mip::Renderer* m_pRenderer;
};

TEST_F(Test_mipUsdStage, Test_TestFileCheck)
{
	EXPECT_TRUE(QFile::exists(m_usdFileResource.TestDirPath_USD.c_str()));
	EXPECT_TRUE(QFile::exists(m_usdFileResource.MEDIP_Sample_USD.c_str()));

	EXPECT_TRUE(QFile::exists(m_usdFileResource.TestDirPath_Usda.c_str()));
	EXPECT_TRUE(QFile::exists(m_usdFileResource.MaterialSample_Usda.c_str()));

	EXPECT_TRUE(QFile::exists(m_usdFileResource.TestDirPath_MDL.c_str()));
	EXPECT_TRUE(QFile::exists(m_usdFileResource.Ceiling_MDL_FilePath.c_str()));
	EXPECT_TRUE(QFile::exists(m_usdFileResource.Roof_MDL_FilePath.c_str()));
	EXPECT_TRUE(QFile::exists(m_usdFileResource.Shingles_MDL_FilePath.c_str()));

	EXPECT_FALSE(QFile::exists(m_usdFileResource.TempFilePath_Usda.c_str()));
	EXPECT_FALSE(QFile::exists(m_usdFileResource.TempFilePath_USD.c_str()));
}

TEST_F(Test_mipUsdStage, TestDefaultStageStatus)
{
	mipUsdStage usdStage;
	EXPECT_FALSE(usdStage.IsOpen());
}

TEST_F(Test_mipUsdStage, TestCreateNew)
{
	mipUsdStage usdStage;
	EXPECT_TRUE(usdStage.CreateNew(m_usdFileResource.TempFilePath_Usda));
	EXPECT_TRUE(usdStage.IsOpen());
	EXPECT_TRUE(QFile::exists(m_usdFileResource.TempFilePath_Usda.c_str()));
}

TEST_F(Test_mipUsdStage, WhenNotOpen_CreateAnyList_ReturnEmpty)
{
	mipUsdStage usdStage;
	EXPECT_TRUE(usdStage.CreateMeshList().empty());
	EXPECT_TRUE(usdStage.CreateMaterialList().empty());
	EXPECT_TRUE(usdStage.CreateLightList().empty());
}

TEST_F(Test_mipUsdStage, TestCreateCube)
{
	mipUsdStage usdStage;
	usdStage.CreateNew(m_usdFileResource.TempFilePath_USD);

	EXPECT_TRUE(usdStage.AddMeshByStlFile(m_usdFileResource.CubeSTLFilePath, "cube", m_pRenderer));
	EXPECT_TRUE(usdStage.IsMeshExist("cube"));
}

TEST_F(Test_mipUsdStage, Test_CompatibleName)
{
	mipUsdStage usdStage;
	usdStage.CreateNew(m_usdFileResource.TempFilePath_USD);

	EXPECT_FALSE(usdStage.AddMeshByStlFile(m_usdFileResource.CubeSTLFilePath, "ÇÑ±Û", m_pRenderer));
	EXPECT_FALSE(usdStage.AddMeshByStlFile(m_usdFileResource.CubeSTLFilePath, "1abc", m_pRenderer));
	EXPECT_FALSE(usdStage.AddMeshByStlFile(m_usdFileResource.CubeSTLFilePath, "abc*", m_pRenderer));
	EXPECT_TRUE(usdStage.AddMeshByStlFile(m_usdFileResource.CubeSTLFilePath, "a_b_1", m_pRenderer));
}

TEST_F(Test_mipUsdStage, Test_AddMaterial_And_Receive)
{
	mipUsdStage usdStage;
	usdStage.CreateNew(m_usdFileResource.TempFilePath_Usda);
	usdStage.AddMaterialByMDLFile(m_usdFileResource.Roof_MDL_FilePath, "roof", "Roof_Tiles");

	mipUSDMaterialPtr exist = usdStage.CreateMeterialByName("roof");
	mipUSDMaterialPtr notExist = usdStage.CreateMeterialByName("not_exist_file_name");

	EXPECT_NE(nullptr, exist);
	EXPECT_EQ(nullptr, notExist);
}

TEST_F(Test_mipUsdStage, Test_ReadMedipSampleFile)
{
	mipUsdStage usdStage;
	EXPECT_TRUE(usdStage.Open(m_usdFileResource.MEDIP_Sample_USD));

	std::vector<mipUSDMeshPtr> meshLayerList = usdStage.CreateMeshList();

	mipUSDMeshPtr pBone_Mesh = usdStage.CreateMeshByName("Bone");
	EXPECT_NE(nullptr, pBone_Mesh);
	EXPECT_STREQ("Bone", pBone_Mesh->GetName().c_str());

	mipUSDMaterialPtr pBone_Material = usdStage.CreateMeterialByName("bone_OmniSurface_Plastic");
	EXPECT_NE(nullptr, pBone_Material);
	EXPECT_STREQ("bone_OmniSurface_Plastic", pBone_Material->GetName().c_str());
}

TEST_F(Test_mipUsdStage, Test_SampleData_PrimPath)
{
	mipUsdStage usdStage;

	usdStage.Open(m_usdFileResource.MEDIP_Sample_USD);

	EXPECT_NE(nullptr, usdStage.CreateMeshByPath("/Root/Bone"));
	EXPECT_NE(nullptr, usdStage.CreateMeshByPath("/Root/Skin"));
	EXPECT_NE(nullptr, usdStage.CreateMeshByPath("/Root/Lung"));
	EXPECT_NE(nullptr, usdStage.CreateMeshByPath("/Root/Artery_noresize_cop"));
	EXPECT_NE(nullptr, usdStage.CreateMeshByPath("/Root/RA_IVC"));
	EXPECT_NE(nullptr, usdStage.CreateMeshByPath("/Root/Airway"));
	EXPECT_NE(nullptr, usdStage.CreateMeshByPath("/Root/myocardium_L"));
	EXPECT_NE(nullptr, usdStage.CreateMeshByPath("/Root/PulmArtery_noresize"));
	EXPECT_NE(nullptr, usdStage.CreateMeshByPath("/Root/PulmVein_noresize_c"));
	EXPECT_NE(nullptr, usdStage.CreateMeshByPath("/Root/myocardium_R"));

	EXPECT_NE(nullptr, usdStage.CreateMeterialByPath("/Root/Looks/bone_OmniSurface_Plastic"));
	EXPECT_NE(nullptr, usdStage.CreateMeterialByPath("/Root/Looks/Aorta_OmniSurface_Blood"));
	EXPECT_NE(nullptr, usdStage.CreateMeterialByPath("/Root/Looks/Myocardium_L_OmniSurface_Blood"));
	EXPECT_NE(nullptr, usdStage.CreateMeterialByPath("/Root/Looks/PA_OmniSurface_Blood"));
	EXPECT_NE(nullptr, usdStage.CreateMeterialByPath("/Root/Looks/Myocardium_R_OmniSurface_Blood"));
	EXPECT_NE(nullptr, usdStage.CreateMeterialByPath("/Root/Looks/PV_OmniSurface_Blood"));
	EXPECT_NE(nullptr, usdStage.CreateMeterialByPath("/Root/Looks/lung_OmniSurface_Skin_1"));
	EXPECT_NE(nullptr, usdStage.CreateMeterialByPath("/Root/Looks/skin_OmniSurface_Skin_2"));
	EXPECT_NE(nullptr, usdStage.CreateMeterialByPath("/Root/Looks/trachea_OmniSurface_Skin_4"));
}

TEST_F(Test_mipUsdStage, Test_WhenOpenAndClose_CreateListReturnEmpty)
{
	mipUsdStage usdStage;

	usdStage.Open(m_usdFileResource.MEDIP_Sample_USD);
	usdStage.Close();

	EXPECT_TRUE(usdStage.CreateMeshList().empty());
	EXPECT_TRUE(usdStage.CreateMaterialList().empty());
	EXPECT_TRUE(usdStage.CreateLightList().empty());
}


TEST_F(Test_mipUsdStage, Test_AddNewMaterial_And_Save)
{
	mipUsdStage usdStage_src;
	mipUsdStage usdStage_dest;

	usdStage_src.Open(m_usdFileResource.MEDIP_Sample_USD);

	mipUSDMaterialPtr pBone_Material = usdStage_src.CreateMeterialByPath("/Root/Looks/bone_OmniSurface_Plastic");
	mipUSDMaterialPtr pAorta_Material = usdStage_src.CreateMeterialByPath("/Root/Looks/Aorta_OmniSurface_Blood");

	EXPECT_TRUE(usdStage_dest.CreateNew(m_usdFileResource.TempFilePath_Usda));

	usdStage_dest.AddMaterial(pBone_Material, "/Root/Looks/bone_OmniSurface_Plastic");
	usdStage_dest.AddMaterial(pAorta_Material, "/Root/Looks/Aorta_OmniSurface_Blood");

	usdStage_dest.Save();

	mipUsdStage usdStage_tester;
	EXPECT_TRUE(usdStage_tester.Open(m_usdFileResource.TempFilePath_Usda));

	pBone_Material = usdStage_tester.CreateMeterialByPath("/Root/Looks/bone_OmniSurface_Plastic");
	pAorta_Material = usdStage_tester.CreateMeterialByPath("/Root/Looks/Aorta_OmniSurface_Blood");

	EXPECT_NE(nullptr, pBone_Material);
	EXPECT_NE(nullptr, pAorta_Material);
}

TEST_F(Test_mipUsdStage, Test_MeterialBindingInMesh)
{
	mipUsdStage usdStage_src;

	usdStage_src.Open(m_usdFileResource.MEDIP_Sample_USD);

	mipUSDMeshPtr pMesh = usdStage_src.CreateMeshByPath("/Root/Bone");
	mipUSDMaterialPtr pMaterial = pMesh->GetMaterial();

	EXPECT_NE(nullptr, pMaterial);
	EXPECT_STREQ("/Root/Looks/bone_OmniSurface_Plastic", pMaterial->GetPath().c_str());
}

TEST_F(Test_mipUsdStage, Test_CopyUsdRenderSetting)
{
	mipUsdStage usdStage_target;
	usdStage_target.CreateNew(m_usdFileResource.TempFilePath_Usda);
	usdStage_target.CopyUsdFile_CustomLayerRenderSetting(m_usdFileResource.MaterialSample_Usda);

	mipUsdCustomLayerData customLayerData = usdStage_target.GetCustomLayerData();

	mipUsdRenderSetting renderSetting;
	EXPECT_TRUE(customLayerData.TryGetRenderSetting(&renderSetting));

	usdStage_target.Save();
}

TEST_F(Test_mipUsdStage, TestWCharType)
{
	std::wstring wstr_hangul = L"°¡";
	std::wstring wstr_alpha = L"a";
	std::wstring wstr_digit = L"8";
	std::wstring wstr_specific = L"*";

	EXPECT_TRUE(iswalpha(wstr_hangul[0]));
	EXPECT_FALSE(iswascii(wstr_hangul[0]));
	EXPECT_FALSE(iswdigit(wstr_hangul[0]));

	EXPECT_TRUE(iswalpha(wstr_alpha[0]));
	EXPECT_TRUE(iswascii(wstr_alpha[0]));
	EXPECT_FALSE(iswdigit(wstr_alpha[0]));

	EXPECT_FALSE(iswalpha(wstr_digit[0]));
	EXPECT_TRUE(iswascii(wstr_digit[0]));
	EXPECT_TRUE(iswdigit(wstr_digit[0]));

	EXPECT_FALSE(iswalpha(wstr_specific[0]));
	EXPECT_TRUE(iswascii(wstr_specific[0]));
	EXPECT_FALSE(iswdigit(wstr_specific[0]));
	EXPECT_FALSE(iswalnum(wstr_specific[0]));
}
