#include "stdafx.h"
#include "Test/test_pch.h"
#include "Omniverse/OmniverseStage.h"
#include "Test/Omniverse/OmniverseFileTestResource.h"
#include "Test/Omniverse/Usd/UsdFileTestResource.h"
#include "Test/Omniverse/OmniverseFileTestResource.h"
#include "Omniverse/OmniverseContext.h"
#include "Omniverse/OmniverseFileUtil.h"
#include "graphics/MeshLayerDataOmniverse.h"
#include "Renderer/Mesh.h"
#include "Renderer/Renderer.h"

static bool IsEqual(const mip::QUATERNION& d1, const mip::QUATERNION& d2)
{
	return
		d1.x == d2.x &&
		d1.y == d2.y &&
		d1.z == d2.z &&
		d1.w == d2.w;
}

class TestOmniverseStage : public ::testing::Test
{
public:
	void SetUp() override
	{
		OmniverseConnector connector;
		connector.Connect(m_OmniFileResource.LocalIpAddress);
		OmniverseFileUtil::DeleteServerFile(&connector, m_OmniFileResource.Server_EmptyUsdFilePath);

		m_pRenderer = new mip::Renderer();
		m_pConnector = new OmniverseConnector();
	}

	void TearDown() override
	{
		delete m_pConnector;
		m_OmniFileResource.Clear();
	}

	std::shared_ptr<OmniverseStage> CreateStage(const std::string& ipAddr, const std::string& filepath)
	{
		auto pStage = std::make_shared<OmniverseStage>();
		m_pConnector->Connect(ipAddr);
		pStage->Create(m_pConnector, filepath);
		return pStage;
	}

protected:
	OmniverseFileTestResource m_OmniFileResource;
	UsdFileTestResource m_UsdFileResource;
	OmniverseConnector* m_pConnector;
	mip::Renderer* m_pRenderer;
};


TEST_F(TestOmniverseStage, TestFileCheck)
{
	EXPECT_TRUE(QFile::exists(QString::fromStdString(m_UsdFileResource.CubeSTLFilePath)));
	EXPECT_TRUE(QFile::exists(QString::fromStdString(m_UsdFileResource.CylinderSTLFilePath)));
	EXPECT_TRUE(QFile::exists(QString::fromStdString(m_UsdFileResource.SphereSTLFilePath)));

	EXPECT_TRUE(QFile::exists(QString::fromStdString(m_UsdFileResource.TestDirPath_MDL)));
	EXPECT_TRUE(QFile::exists(QString::fromStdString(m_UsdFileResource.Ceiling_MDL_FilePath)));
	EXPECT_TRUE(QFile::exists(QString::fromStdString(m_UsdFileResource.Roof_MDL_FilePath)));
	EXPECT_TRUE(QFile::exists(QString::fromStdString(m_UsdFileResource.Shingles_MDL_FilePath)));

	EXPECT_TRUE(QFile::exists(QString::fromStdString(m_UsdFileResource.TestDirPath_USD)));
	EXPECT_TRUE(QFile::exists(QString::fromStdString(m_UsdFileResource.USD_Empty_FilePath)));

	EXPECT_TRUE(QFile::exists(QString::fromStdString(m_UsdFileResource.PlaneFilePath)));
	EXPECT_TRUE(QFile::exists(QString::fromStdString(m_UsdFileResource.CubeFilePath)));
	EXPECT_TRUE(QFile::exists(QString::fromStdString(m_UsdFileResource.SphereFilePath)));
	EXPECT_TRUE(QFile::exists(QString::fromStdString(m_UsdFileResource.CylinderFilePath)));
	EXPECT_TRUE(QFile::exists(QString::fromStdString(m_UsdFileResource.ConeFilePath)));

	EXPECT_TRUE(QFile::exists(QString::fromStdString(m_UsdFileResource.PlaneFilePath_UsdObj)));
	EXPECT_TRUE(QFile::exists(QString::fromStdString(m_UsdFileResource.CubeFilePath_UsdObj)));
	EXPECT_TRUE(QFile::exists(QString::fromStdString(m_UsdFileResource.SphereFilePath_UsdObj)));
	EXPECT_TRUE(QFile::exists(QString::fromStdString(m_UsdFileResource.CylinderFilePath_UsdObj)));
	EXPECT_TRUE(QFile::exists(QString::fromStdString(m_UsdFileResource.ConeFilePath_UsdObj)));
}

TEST_F(TestOmniverseStage, WhenConnectorNotConnected_CreateReturnFalse)
{
	OmniverseConnector connector;
	OmniverseStage stage;
	EXPECT_FALSE(stage.Create(&connector, m_OmniFileResource.TempUsdPath));
	EXPECT_FALSE(stage.IsOpen());
}


TEST_F(TestOmniverseStage, TestCreateCube)
{
	OmniverseStagePtr pStage = CreateStage(m_OmniFileResource.LocalIpAddress, m_OmniFileResource.TempUsdPath);
	EXPECT_TRUE(pStage->AddMeshByStlFile(m_UsdFileResource.CubeSTLFilePath, "cube", m_pRenderer));
}

TEST_F(TestOmniverseStage, TestCreateCylinder)
{
	OmniverseStagePtr pStage = CreateStage(m_OmniFileResource.LocalIpAddress, m_OmniFileResource.TempUsdPath);

	EXPECT_TRUE(pStage->AddMeshByStlFile(m_UsdFileResource.CylinderSTLFilePath, "cylinder", m_pRenderer));
}

TEST_F(TestOmniverseStage, TestCreateSphere)
{
	OmniverseStagePtr pStage = CreateStage(m_OmniFileResource.LocalIpAddress, m_OmniFileResource.TempUsdPath);

	EXPECT_TRUE(pStage->AddMeshByStlFile(m_UsdFileResource.SphereSTLFilePath, "sphere", m_pRenderer));
}

TEST_F(TestOmniverseStage, TestCreateMultiObject)
{
	OmniverseStagePtr pStage = CreateStage(m_OmniFileResource.LocalIpAddress, m_OmniFileResource.TempUsdPath);

	EXPECT_TRUE(pStage->AddMeshByStlFile(m_UsdFileResource.SphereSTLFilePath, "sphere", m_pRenderer));
	EXPECT_TRUE(pStage->AddMeshByStlFile(m_UsdFileResource.CylinderSTLFilePath, "cylinder", m_pRenderer));
	EXPECT_TRUE(pStage->AddMeshByStlFile(m_UsdFileResource.CubeSTLFilePath, "cube", m_pRenderer));
}

TEST_F(TestOmniverseStage, TestOpenAndCreateMultiObject)
{
	OmniverseStagePtr pStage = CreateStage(m_OmniFileResource.LocalIpAddress, m_OmniFileResource.TempUsdPath);

	EXPECT_TRUE(pStage->Open(m_pConnector, m_OmniFileResource.TempUsdPath));
	EXPECT_TRUE(pStage->AddMeshByStlFile(m_UsdFileResource.SphereSTLFilePath, "sphere_2", m_pRenderer));
	EXPECT_TRUE(pStage->AddMeshByStlFile(m_UsdFileResource.CylinderSTLFilePath, "cylinder_2", m_pRenderer));
	EXPECT_TRUE(pStage->AddMeshByStlFile(m_UsdFileResource.CubeSTLFilePath, "cube_2", m_pRenderer));
}

TEST_F(TestOmniverseStage, TestMeshDataWithSRT_And_ReceiveMeshData)
{
	OmniverseStagePtr pStage = CreateStage(m_OmniFileResource.LocalIpAddress, m_OmniFileResource.TempUsdPath);

	MeshLayerDataOmniverse mipMesh(m_pRenderer);
	mipMesh.LoadSTL(m_UsdFileResource.CubeSTLFilePath, "cube");
	mipMesh.GetData()->setTranslate(mip::VECTOR3(1.0f, 2.0f, 3.0f));
	mipMesh.GetData()->setRotate(mip::QUATERNION(10.0f, 20.0f, 30.0f, 1.0f));
	mipMesh.GetData()->setScale(mip::VECTOR3(1.0f, 1.0f, 1.0f));

	EXPECT_TRUE(pStage->AddMeshByData(*mipMesh.GetData(), *mipMesh.GetInfo()));

	mipUSDMeshPtr usdMesh = pStage->CreateMeshByName("cube");

	EXPECT_EQ(mipMesh.GetData()->m_verts, usdMesh->GetVertice_cm());
	//EXPECT_EQ(mipMesh.GetData()->m_normals, usdMesh->GetNormals());
	EXPECT_EQ(mipMesh.GetData()->m_tris, usdMesh->GetIndices());

	EXPECT_EQ(mipMesh.GetData()->translation, usdMesh->GetTranslation_cm());
	EXPECT_TRUE(IsEqual(mipMesh.GetData()->rotation, usdMesh->GetRotation()));
	EXPECT_EQ(mipMesh.GetData()->scale, usdMesh->GetScale());
}

TEST_F(TestOmniverseStage, TestUSDMeshDataWithSRT_And_ConvertToMeshData)
{
	OmniverseStagePtr pStage = CreateStage(m_OmniFileResource.LocalIpAddress, m_OmniFileResource.TempUsdPath);

	EXPECT_TRUE(pStage->AddMeshByStlFile(m_UsdFileResource.CubeSTLFilePath, "cube", m_pRenderer));

	pStage->TrasformMesh("cube", mip::VECTOR3(2.0f, 3.0f, 4.0f), mip::VECTOR3(10.0f, 20.0f, 30.0f), mip::VECTOR3(1.0f, 2.0f, 3.0f));

	mipUSDMeshPtr usdMesh = pStage->CreateMeshByName("cube");

	MeshLayerDataOmniverse mipMesh(m_pRenderer);
	EXPECT_TRUE(usdMesh->CopyToMeshTopology(mipMesh.GetData()));

	EXPECT_EQ(mip::VECTOR3(2.0f, 3.0f, 4.0f), mipMesh.GetData()->translation);
	EXPECT_TRUE(IsEqual(mip::QUATERNION(10.0f, 20.0f, 30.0f, 1.0f), mipMesh.GetData()->rotation));
	EXPECT_EQ(mip::VECTOR3(1.0f, 2.0f, 3.0f), mipMesh.GetData()->scale);
}

TEST_F(TestOmniverseStage, TestVisibilityOnOff)
{
	OmniverseStagePtr pStage = CreateStage(m_OmniFileResource.LocalIpAddress, m_OmniFileResource.TempUsdPath);

	pStage->AddMeshByStlFile(m_UsdFileResource.CubeSTLFilePath, "cube", m_pRenderer);

	mipUSDMeshPtr usdMesh;

	usdMesh = pStage->CreateMeshByName("cube");
	EXPECT_EQ(true, usdMesh->IsVisible());

	pStage->SetVisible("cube", false);

	usdMesh = pStage->CreateMeshByName("cube");
	EXPECT_EQ(false, usdMesh->IsVisible());
}

TEST_F(TestOmniverseStage, TestAddMDL_To_Prim)
{
	OmniverseStagePtr pStage = CreateStage(m_OmniFileResource.LocalIpAddress, m_OmniFileResource.TempUsdPath);

	EXPECT_TRUE(pStage->AddMaterialByMDLFile(m_UsdFileResource.Roof_MDL_FilePath, "roof", "Roof_Tiles"));
	EXPECT_TRUE(pStage->AddMaterialByMDLFile(m_UsdFileResource.Ceiling_MDL_FilePath, "ceil", "Ceiling_Tiles"));

	std::vector<mipUSDMaterialPtr> usdMeterialList = pStage->CreateMaterialList();
	EXPECT_EQ(2, usdMeterialList.size());
	EXPECT_STREQ("roof", usdMeterialList[0]->GetName().c_str());
	EXPECT_STREQ("ceil", usdMeterialList[1]->GetName().c_str());
}

TEST_F(TestOmniverseStage, TestCreateMaterial_And_ConnectToMesh)
{
	OmniverseStagePtr pStage = CreateStage(m_OmniFileResource.LocalIpAddress, m_OmniFileResource.TempUsdPath);

	pStage->AddMeshByStlFile(m_UsdFileResource.CubeSTLFilePath, "cube", m_pRenderer);
	pStage->AddMaterialByMDLFile(m_UsdFileResource.Ceiling_MDL_FilePath, "ceil", "Ceiling_Tiles");

	EXPECT_TRUE(pStage->BindMeshMaterial("cube", "ceil"));
	EXPECT_FALSE(pStage->BindMeshMaterial("cube", "invalid"));
}

//TEST_F(TestOmniverseStage, TestAdd_Hangul_MeshData_ReturnFalse)
//{
//	OmniverseStagePtr pStage = CreateStage(m_OmniFileResource.LocalIpAddress, m_OmniFileResource.TempUsdPath);
//	EXPECT_FALSE(pStage->AddMeshByStlFile(m_UsdFileResource.CubeSTLFilePath, "ÇÑ±Û"));
//}

