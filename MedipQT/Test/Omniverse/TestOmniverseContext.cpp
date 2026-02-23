#include "stdafx.h"
#include "Test/test_pch.h"
#include "Omniverse/OmniverseContext.h"
#include "Omniverse/Usd/mipUsdStage.h"
#include "Omniverse/OmniverseFileUtil.h"
#include "volumedata.h"
#include "WindowManager.h"
#include "Test/Omniverse/OmniverseFileTestResource.h"
#include "Test/Omniverse/Usd/UsdFileTestResource.h"
#include "Test/TestContext.h"

class TestOmniverseContext : public ::testing::Test
{
public:
	void SetUp() override
	{
		m_pTest = TestContext::TestForMesh();
		m_pRenderer = &m_pTest->pRenderer->Data;

		OmniverseConnector connector;
		connector.Connect(m_OmniFileResource.LocalIpAddress);
		OmniverseFileUtil::DeleteServerFile(&connector, m_OmniFileResource.TempUsdPath);
		OmniverseFileUtil::DeleteServerFile(&connector, m_OmniFileResource.TempUsdLiveDirPath);

		m_pOmniverse = new OmniverseContext();
		m_pStage = m_pOmniverse->GetStage();
		m_pConnector = m_pOmniverse->GetConnector();

	}

	void TearDown() override
	{
		delete m_pOmniverse;
	}

protected:
	std::shared_ptr<TestContext> m_pTest;
	OmniverseFileTestResource m_OmniFileResource;
	UsdFileTestResource m_UsdFileResource;
	OmniverseContext* m_pOmniverse;
	OmniverseStage* m_pStage;
	OmniverseConnector* m_pConnector;
	mip::Renderer* m_pRenderer;
};

TEST_F(TestOmniverseContext, TestFileShouldExist)
{
	EXPECT_TRUE(QFile::exists(QString::fromStdString(m_UsdFileResource.TestDirPath)));
	EXPECT_TRUE(QFile::exists(QString::fromStdString(m_UsdFileResource.CubeSTLFilePath)));
	EXPECT_TRUE(QFile::exists(QString::fromStdString(m_UsdFileResource.CylinderSTLFilePath)));
	EXPECT_TRUE(QFile::exists(QString::fromStdString(m_UsdFileResource.SphereSTLFilePath)));
}

/*
	Omniverse가 켜져있지 않을 때 Exception이 발생함.
*/
TEST_F(TestOmniverseContext, TestInit)
{
	EXPECT_TRUE(m_pOmniverse->Connect(m_OmniFileResource.LocalIpAddress));
}

TEST_F(TestOmniverseContext, TestCreateModel)
{
	m_pOmniverse->Connect(m_OmniFileResource.LocalIpAddress);
	EXPECT_TRUE(m_pOmniverse->CreateOmniverseStage(m_OmniFileResource.TempUsdPath));
}
TEST_F(TestOmniverseContext, TestModelExist)
{
	m_pOmniverse->Connect(m_OmniFileResource.LocalIpAddress);
	EXPECT_FALSE(m_pConnector->IsStageExist(m_OmniFileResource.TempUsdPath));
	m_pOmniverse->CreateOmniverseStage(m_OmniFileResource.TempUsdPath);
	EXPECT_TRUE(m_pConnector->IsStageExist(m_OmniFileResource.TempUsdPath));
}

TEST_F(TestOmniverseContext, TestRootFolderList)
{
	m_pOmniverse->Connect(m_OmniFileResource.LocalIpAddress);

	OmniverseFileInfoPtr pRoot = m_pConnector->GetRootDirectory();
	EXPECT_NE(nullptr, pRoot);
	EXPECT_TRUE(pRoot->CanHaveChildren());

	std::vector<OmniverseFileInfoPtr> rootChildren = pRoot->GetChildren();
	EXPECT_NE(nullptr, pRoot->FindChild("Library"));
	EXPECT_NE(nullptr, pRoot->FindChild("NVIDIA"));
	EXPECT_NE(nullptr, pRoot->FindChild("Projects"));
	EXPECT_NE(nullptr, pRoot->FindChild("Users"));
}

TEST_F(TestOmniverseContext, TestFindChild)
{
	m_pOmniverse->Connect(m_OmniFileResource.LocalIpAddress);

	OmniverseFileInfoPtr pRoot = m_pConnector->GetRootDirectory();

	OmniverseFileInfoPtr pUsers = pRoot->FindByUrl("Users");
	EXPECT_NE(nullptr, pUsers);
	EXPECT_STREQ("Users", pUsers->RelativePath().c_str());
	EXPECT_STREQ("Users", pUsers->ResourcePath().c_str());
}

TEST_F(TestOmniverseContext, TestGetMeshList)
{
	m_pOmniverse->Connect(m_OmniFileResource.LocalIpAddress);
	m_pOmniverse->CreateOmniverseStage(m_OmniFileResource.TempUsdPath);
	m_pOmniverse->GetStage()->AddMeshByStlFile(m_UsdFileResource.SphereSTLFilePath, "sphere", m_pRenderer);
	m_pOmniverse->GetStage()->AddMeshByStlFile(m_UsdFileResource.CylinderSTLFilePath, "cylinder", m_pRenderer);
	m_pOmniverse->GetStage()->AddMeshByStlFile(m_UsdFileResource.CubeSTLFilePath, "cube", m_pRenderer);

	std::vector<std::string> nameList = m_pOmniverse->GetStage()->GetMeshNameList();
	EXPECT_EQ(3, nameList.size());
	EXPECT_STREQ("sphere", nameList[0].c_str());
	EXPECT_STREQ("cylinder", nameList[1].c_str());
	EXPECT_STREQ("cube", nameList[2].c_str());
}

TEST_F(TestOmniverseContext, TestMeshExist)
{
	m_pOmniverse->Connect(m_OmniFileResource.LocalIpAddress);
	m_pOmniverse->CreateOmniverseStage(m_OmniFileResource.TempUsdPath);
	m_pOmniverse->GetStage()->AddMeshByStlFile(m_UsdFileResource.SphereSTLFilePath, "sphere", m_pRenderer);

	EXPECT_TRUE(m_pOmniverse->GetStage()->IsMeshExist("sphere"));
	EXPECT_FALSE(m_pOmniverse->GetStage()->IsMeshExist("invalid"));
}

TEST_F(TestOmniverseContext, TestDeleteMesh)
{
	m_pOmniverse->Connect(m_OmniFileResource.LocalIpAddress);
	m_pOmniverse->CreateOmniverseStage(m_OmniFileResource.TempUsdPath);
	m_pOmniverse->GetStage()->AddMeshByStlFile(m_UsdFileResource.SphereSTLFilePath, "sphere", m_pRenderer);

	m_pOmniverse->GetStage()->DeleteMeshByName("sphere");

	EXPECT_FALSE(m_pOmniverse->GetStage()->IsMeshExist("sphere"));
}

TEST_F(TestOmniverseContext, TestGetUsdMeshTopologyList)
{
	m_pOmniverse->Connect(m_OmniFileResource.LocalIpAddress);
	m_pOmniverse->CreateOmniverseStage(m_OmniFileResource.TempUsdPath);
	m_pOmniverse->GetStage()->AddMeshByStlFile(m_UsdFileResource.CubeSTLFilePath, "cube", m_pRenderer);

	std::vector<std::shared_ptr<mipUSDMesh>> list = m_pOmniverse->GetStage()->CreateMeshList();

	EXPECT_EQ(1, list.size());

	EXPECT_STREQ("cube", list[0]->GetName().c_str());
	EXPECT_EQ(72, list[0]->points_mm.size());
	EXPECT_EQ(24, list[0]->meshNormals.size());
	EXPECT_EQ(72, list[0]->vecIndices.size());
	EXPECT_EQ(24, list[0]->faceVertexCounts.size());
}

TEST_F(TestOmniverseContext, TestTranslateMesh)
{
	m_pOmniverse->Connect(m_OmniFileResource.LocalIpAddress);
	m_pOmniverse->CreateOmniverseStage(m_OmniFileResource.TempUsdPath);
	m_pOmniverse->GetStage()->AddMeshByStlFile(m_UsdFileResource.CubeSTLFilePath, "cube", m_pRenderer);
	m_pOmniverse->GetStage()->TranslateMesh_cm("cube", mip::VECTOR3(2.0f, 2.0f, 2.0f));

	mipUSDMeshPtr usdMesh = m_pOmniverse->GetStage()->CreateMeshByName("cube");
	EXPECT_NE(nullptr, usdMesh);

	EXPECT_EQ(mip::VECTOR3(2.0f, 2.0f, 2.0f), usdMesh->GetTranslation_cm());
}

//TEST_F(TestOmniverseContext, TestLeaveLiveSession)
//{
//	m_pOmniverse->Connect(m_OmniFileResource.LocalIpAddress);
//	m_pOmniverse->CreateOmniverseStage("Projects/temp_usd.usd");
//
//	EXPECT_FALSE(m_pOmniverse->LeaveLiveSession());
//	m_pOmniverse->CreateLiveSession("session_name");
//	EXPECT_TRUE(m_pOmniverse->LeaveLiveSession());
//
//	EXPECT_FALSE(m_pOmniverse->IsLiveSessionMode());
//}
//
//TEST_F(TestOmniverseContext, TestJoinLiveSession)
//{
//	m_pOmniverse->Connect(m_OmniFileResource.LocalIpAddress);
//	m_pOmniverse->CreateOmniverseStage("Projects/temp_usd.usd");
//
//	EXPECT_FALSE(m_pOmniverse->JoinLiveSession("session_name"));
//	m_pOmniverse->CreateLiveSession("session_name");
//	m_pOmniverse->LeaveLiveSession();
//	EXPECT_TRUE(m_pOmniverse->JoinLiveSession("session_name"));
//
//	EXPECT_TRUE(m_pOmniverse->IsLiveSessionMode());
//}

TEST_F(TestOmniverseContext, TestRemoveNotExistString)
{
	std::vector<std::string> newData = { "a", "b"};
	std::vector<std::string> oldData = { "d", "c", "b", "a"};

	std::vector<std::string> notExistData = FindNotExistStringList(oldData, newData);

	EXPECT_EQ(notExistData, std::vector<std::string>({ "d", "c"}));
}

