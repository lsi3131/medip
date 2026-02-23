#include "stdafx.h"
#include "Test/test_pch.h"
#include "Omniverse/OmniverseHeader.h"
#include "Omniverse/Usd/mipUsdUtil.h"
#include "Test/Omniverse/OmniverseFileTestResource.h"

class TestOmniverseAPI : public ::testing::Test
{
public:
	void SetUp() override
	{
		QString testDir = GetUnitTestDataDirectory("Omniverse").c_str();
		m_usdDirPath = testDir + "/usda";
		m_tempUsdaFilePath = testDir + "/usda/temp.usda";
		m_materialSampleFilePath_usda = testDir + "/usda/material_sample.usda";

	}

	void TearDown() override
	{
	}

protected:
	OmniverseFileTestResource Resource;
	QString m_usdDirPath;
	QString m_tempUsdaFilePath;
	QString m_materialSampleFilePath_usda;
};

TEST_F(TestOmniverseAPI, TestCreateStage)
{
	pxr::UsdStageRefPtr pStage = pxr::UsdStage::CreateNew(m_tempUsdaFilePath.toStdString());
	EXPECT_NE(nullptr, pStage);
	EXPECT_TRUE(QFile::exists(m_tempUsdaFilePath));
}

TEST_F(TestOmniverseAPI, TestCreateGeometry)
{
	pxr::UsdStageRefPtr pStage = pxr::UsdStage::CreateNew(m_tempUsdaFilePath.toStdString());

	UsdGeomXform xform = pxr::UsdGeomXform::Define(pStage, SdfPath("/Root"));
	UsdGeomSphere sphere_src = pxr::UsdGeomSphere::Define(pStage, SdfPath("/Root/sphere_src"));
	UsdGeomCube cube = pxr::UsdGeomCube::Define(pStage, SdfPath("/Root/Cube"));
	pStage->GetRootLayer()->Save();
}

TEST_F(TestOmniverseAPI, TestSetColor)
{
	pxr::UsdStageRefPtr pStage = pxr::UsdStage::CreateNew(m_tempUsdaFilePath.toStdString());

	UsdGeomXform xform = pxr::UsdGeomXform::Define(pStage, SdfPath("/Root"));
	UsdGeomSphere sphere1 = pxr::UsdGeomSphere::Define(pStage, SdfPath("/Root/Sphere1"));
	UsdGeomSphere sphere2 = pxr::UsdGeomSphere::Define(pStage, SdfPath("/Root/Sphere2"));

	//GfVec3f color(0, 1, 0);
	EXPECT_TRUE(sphere1.GetDisplayColorAttr().Set(VtArray<GfVec3f>({ GfVec3f(0, 1, 0) })));
	EXPECT_TRUE(sphere2.GetDisplayColorAttr().Set(VtArray<GfVec3f>({ GfVec3f(0, 0, 1) })));

	VtArray<GfVec3f> color1;
	VtArray<GfVec3f> color2;

	EXPECT_TRUE(sphere1.GetDisplayColorAttr().Get<VtArray<GfVec3f>>(&color1));
	EXPECT_TRUE(sphere2.GetDisplayColorAttr().Get<VtArray<GfVec3f>>(&color2));

	pStage->GetRootLayer()->Save();
}

TEST_F(TestOmniverseAPI, TestCopyAttr)
{
	pxr::UsdStageRefPtr pStage = pxr::UsdStage::CreateNew(m_tempUsdaFilePath.toStdString());

	UsdGeomXform xform = pxr::UsdGeomXform::Define(pStage, SdfPath("/Root"));
	UsdGeomSphere sphere_src = pxr::UsdGeomSphere::Define(pStage, SdfPath("/Root/Sphere1"));
	UsdGeomSphere sphere_dst = pxr::UsdGeomSphere::Define(pStage, SdfPath("/Root/Sphere2"));

	sphere_src.GetDisplayColorAttr().Set(VtArray<GfVec3f>({ GfVec3f(0, 1, 0) }));
	sphere_dst.GetDisplayColorAttr().Set(VtArray<GfVec3f>({ GfVec3f(0, 0, 1) }));

	VtArray<GfVec3f> color1;
	VtArray<GfVec3f> color2;

	mipUsdUtil::CopyProperties(&sphere_dst.GetPrim(), sphere_src.GetPrim());

	EXPECT_TRUE(sphere_src.GetDisplayColorAttr().Get<VtArray<GfVec3f>>(&color1));
	EXPECT_TRUE(sphere_dst.GetDisplayColorAttr().Get<VtArray<GfVec3f>>(&color2));

	EXPECT_EQ(GfVec3f(0, 1, 0), color1[0]);
	EXPECT_EQ(GfVec3f(0, 1, 0), color2[0]);

	EXPECT_STREQ("/Root/Sphere1", sphere_src.GetPath().GetString().c_str());
	EXPECT_STREQ("/Root/Sphere2", sphere_dst.GetPath().GetString().c_str());
}

TEST_F(TestOmniverseAPI, TestCopyRelationShip)
{
	pxr::UsdStageRefPtr pStage = pxr::UsdStage::CreateNew(m_tempUsdaFilePath.toStdString());

	UsdGeomXform xform = pxr::UsdGeomXform::Define(pStage, SdfPath("/Root"));
	UsdGeomSphere sphere_src = pxr::UsdGeomSphere::Define(pStage, SdfPath("/Root/Sphere1"));
	UsdGeomSphere sphere_dst = pxr::UsdGeomSphere::Define(pStage, SdfPath("/Root/Sphere2"));

	sphere_src.CreateProxyPrimRel().AddTarget(SdfPath("src1"));
	sphere_dst.CreateProxyPrimRel().AddTarget(SdfPath("dst1"));

	SdfPathVector path1;
	SdfPathVector path2;

	mipUsdUtil::CopyProperties(&sphere_dst.GetPrim(), sphere_src.GetPrim());

	EXPECT_TRUE(sphere_src.GetProxyPrimRel().GetTargets(&path1));
	EXPECT_TRUE(sphere_dst.GetProxyPrimRel().GetTargets(&path2));

	EXPECT_STREQ("/Root/Sphere1/src1", path1[0].GetString().c_str());
	EXPECT_STREQ("/Root/Sphere1/src1", path2[0].GetString().c_str());
}

TEST_F(TestOmniverseAPI, TestMaterialTutorial)
{
	pxr::UsdStageRefPtr pStage = pxr::UsdStage::Open(m_materialSampleFilePath_usda.toStdString());

	SdfPath path("/Root/Looks/bone_OmniSurface_Plastic");
	pxr::UsdShadeMaterial mat = pxr::UsdShadeMaterial::Define(pStage, path);

	EXPECT_STREQ("/Root/Looks/bone_OmniSurface_Plastic", mat.GetPath().GetString().c_str());
}

TEST_F(TestOmniverseAPI, TestReference)
{
	pxr::UsdStageRefPtr pStage_Temp = pxr::UsdStage::CreateNew(m_tempUsdaFilePath.toStdString());
	//pxr::UsdStageRefPtr pStage_Sample = pxr::UsdStage::Open(m_materialSampleFilePath_usda.toStdString());

	pxr::UsdPrim refSample = pStage_Temp->OverridePrim(SdfPath("/reference"));
	EXPECT_TRUE(refSample.GetReferences().AddReference(m_materialSampleFilePath_usda.toStdString()));

	pStage_Temp->Save();
}

TEST_F(TestOmniverseAPI, Test_CopyUtil)
{
	pxr::UsdStageRefPtr pStage = pxr::UsdStage::CreateNew(m_tempUsdaFilePath.toStdString());

	UsdGeomXform xform = pxr::UsdGeomXform::Define(pStage, SdfPath("/Root"));
	UsdGeomSphere sphere_src = pxr::UsdGeomSphere::Define(pStage, SdfPath("/Root/Sphere1"));
	UsdGeomSphere sphere_dst = pxr::UsdGeomSphere::Define(pStage, SdfPath("/Root/Sphere2"));

	sphere_src.GetDisplayColorAttr().Set(VtArray<GfVec3f>({ GfVec3f(0, 1, 0) }));
	sphere_dst.GetDisplayColorAttr().Set(VtArray<GfVec3f>({ GfVec3f(0, 0, 1) }));

	SdfLayerHandle layerSrc = pStage->GetRootLayer();
	SdfLayerHandle layerDst = pStage->GetRootLayer();

	SdfCopySpec(layerSrc, sphere_src.GetPrim().GetPath(), layerDst, sphere_dst.GetPrim().GetPath());

	VtArray<GfVec3f> color1;
	VtArray<GfVec3f> color2;

	EXPECT_TRUE(sphere_src.GetDisplayColorAttr().Get<VtArray<GfVec3f>>(&color1));
	EXPECT_TRUE(sphere_dst.GetDisplayColorAttr().Get<VtArray<GfVec3f>>(&color2));

	EXPECT_EQ(GfVec3f(0, 1, 0), color1[0]);
	EXPECT_EQ(GfVec3f(0, 1, 0), color2[0]);

	EXPECT_STREQ("/Root/Sphere1", sphere_src.GetPath().GetString().c_str());
	EXPECT_STREQ("/Root/Sphere2", sphere_dst.GetPath().GetString().c_str());

	pStage->Save();
}

TEST_F(TestOmniverseAPI, Test_CopyUtil_DiffFile)
{
	std::string tempUsdaFilePath_1 = m_usdDirPath.toStdString() + "/temp_1.usda";
	std::string tempUsdaFilePath_2 = m_usdDirPath.toStdString() + "/temp_2.usda";

	pxr::UsdStageRefPtr pStage_src = pxr::UsdStage::CreateNew(tempUsdaFilePath_1);
	pxr::UsdStageRefPtr pStage_dst = pxr::UsdStage::CreateNew(tempUsdaFilePath_2);

	UsdGeomXform xform_src = pxr::UsdGeomXform::Define(pStage_src, SdfPath("/Root"));
	UsdGeomSphere sphere_src = pxr::UsdGeomSphere::Define(pStage_src, SdfPath("/Root/Sphere1"));

	//UsdGeomXform xform_dst = pxr::UsdGeomXform::Define(pStage_dst, SdfPath("/Root"));
	//UsdGeomSphere sphere_dst = pxr::UsdGeomSphere::Define(pStage_dst, SdfPath("/Root/Sphere2"));
	UsdPrim sphere_dst = pStage_dst->DefinePrim(SdfPath("/Root/Sphere2"));

	sphere_src.GetDisplayColorAttr().Set(VtArray<GfVec3f>({ GfVec3f(0, 1, 0) }));

	SdfLayerHandle layerSrc = pStage_src->GetRootLayer();
	SdfLayerHandle layerDst = pStage_dst->GetRootLayer();

	//SdfCopySpec(layerSrc, sphere_src.GetPrim().GetPath(), layerDst, sphere_dst.GetPrim().GetPath());
	SdfCopySpec(layerSrc, SdfPath("/Root/Sphere1"), layerDst, SdfPath("/Root/Sphere2"));

	//UsdGeomXform xform_dst = pxr::UsdGeomXform::Define(pStage_dst, SdfPath("/Root"));
	//UsdGeomSphere sphere_dst = pxr::UsdGeomSphere::Define(pStage_dst, SdfPath("/Root/Sphere2"));

	VtArray<GfVec3f> color1;
	//EXPECT_TRUE(sphere_dst.GetDisplayColorAttr().Get<VtArray<GfVec3f>>(&color1));
	//EXPECT_EQ(GfVec3f(0, 1, 0), color1[0]);

	pStage_src->Save();
	pStage_dst->Save();
}

TEST_F(TestOmniverseAPI, Test_CustomLayerData)
{
	pxr::UsdStageRefPtr pStage = pxr::UsdStage::Open(m_materialSampleFilePath_usda.toStdString());

	SdfLayerHandle rootLayer = pStage->GetRootLayer();
	VtDictionary layerData = rootLayer->GetCustomLayerData();
	VtDictionary::const_iterator itFind;
	itFind = layerData.find("notExistDataName");
	EXPECT_EQ(layerData.end(), itFind);

	itFind = layerData.find("renderSettings");
	EXPECT_NE(layerData.end(), itFind);

	VtValue value;
	value = (*itFind).second;
	EXPECT_STREQ("VtDictionary", value.GetTypeName().c_str());

	VtDictionary renderSettingDictionary = value.Get<VtDictionary>();
	itFind = renderSettingDictionary.find("rtx:rendermode");
	EXPECT_NE(layerData.end(), itFind);

	value = (*itFind).second;
	EXPECT_STREQ("TfToken", value.GetTypeName().c_str());
	EXPECT_STREQ("PathTracing", value.Get<TfToken>().GetString().c_str());
}


