#include "stdafx.h"
#include "Test/test_pch.h"
#include "graphics/volumedata.h"
#include "WindowManager.h"
#include "DataContext.h"
#include "Renderer/MeshTopology.h"

class TestMeshLayerContainer : public ::testing::Test
{
public:
	void SetUp() override
	{
		m_testDirPath = GetUnitTestDataDirectory("Mesh");
		m_cubeSTLFilePath = m_testDirPath + "/cube.stl";
		m_cylinderSTLFilePath = m_testDirPath + "/cylinder.stl";
		m_sphereSTLFilePath = m_testDirPath + "/sphere.stl";
	}
	void TearDown() override
	{
	}

protected:
	std::string m_testDirPath;
	std::string m_cubeSTLFilePath;
	std::string m_cylinderSTLFilePath;
	std::string m_sphereSTLFilePath;
};

TEST_F(TestMeshLayerContainer, TestLoadSTL)
{
	MeshData meshData;
	EXPECT_TRUE(meshData.LoadSTL(m_cubeSTLFilePath, false));

	std::vector<MeshLayerData> list = meshData.GetList();
	EXPECT_EQ(1, list.size());
	EXPECT_STREQ(L"Mesh 0", list[0].Info->MeshName);
	EXPECT_EQ(eOmniverseStatus::Unlock, list[0].Info->GetOmniverseStatus());
}

TEST_F(TestMeshLayerContainer, TestGetListByStatus)
{
	MeshData meshData;

	MeshInfo* pMeshInfo_Lock;
	MeshInfo* pMeshInfo_Unlock;
	meshData.LoadSTL(m_cubeSTLFilePath, false, &pMeshInfo_Lock);
	meshData.LoadSTL(m_cubeSTLFilePath, false, &pMeshInfo_Unlock);

	pMeshInfo_Lock->SetName("lock");
	pMeshInfo_Lock->SetOmniverseStatus(eOmniverseStatus::Lock);

	pMeshInfo_Unlock->SetName("unlock");
	pMeshInfo_Unlock->SetOmniverseStatus(eOmniverseStatus::Unlock);

	std::vector<MeshLayerData> listLock = meshData.GetListByStatus(eOmniverseStatus::Lock);
	std::vector<MeshLayerData> listUnlock = meshData.GetListByStatus(eOmniverseStatus::Unlock);
	EXPECT_EQ(1, listLock.size());
	EXPECT_STREQ(L"lock", listLock[0].Info->MeshName);

	EXPECT_EQ(1, listUnlock.size());
	EXPECT_STREQ(L"unlock", listUnlock[0].Info->MeshName);
}

TEST_F(TestMeshLayerContainer, TestMeshSetGetPicked)
{
	MeshData meshData;

	meshData.LoadSTL(m_cubeSTLFilePath, false);
	meshData.LoadSTL(m_cubeSTLFilePath, false);
	meshData.LoadSTL(m_cubeSTLFilePath, false);

	meshData.SetPick(0, true);
	meshData.SetPick(2, true);

	EXPECT_TRUE(meshData.IsPicked(0));
	EXPECT_FALSE(meshData.IsPicked(1));
	EXPECT_TRUE(meshData.IsPicked(2));
}

TEST_F(TestMeshLayerContainer, TestMeshDataPicked)
{
	MeshData meshData;

	MeshLayerData data1;
	MeshLayerData data2;
	MeshLayerData data3;

	meshData.LoadSTL(m_cubeSTLFilePath, false);
	meshData.LoadSTL(m_cubeSTLFilePath, false);
	meshData.LoadSTL(m_cubeSTLFilePath, false);

	meshData.TryGet(&data1, 0);
	meshData.TryGet(&data2, 1);
	meshData.TryGet(&data3, 2);

	meshData.SetPick(data1, true);
	meshData.SetPick(data2, true);

	EXPECT_TRUE(meshData.IsPicked(data1));
	EXPECT_TRUE(meshData.IsPicked(data2));
	EXPECT_FALSE(meshData.IsPicked(data3));
}

TEST_F(TestMeshLayerContainer, TestMeshDataTopology)
{
	MeshData meshData;

	meshData.LoadSTL(m_cubeSTLFilePath, false);

	meshData.Get(0).Data->m_ttris;
	//meshLayerList.Get(0).Data->m_treeTris->m_checklist;
}
