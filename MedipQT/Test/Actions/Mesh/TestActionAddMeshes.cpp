#include "stdafx.h"
#include "Test/test_pch.h"
#include "Actions/Mesh/ActionAddMeshes.h"
#include "Test/TestContext.h"
#include "graphics/Mesh/MeshPrimitiveBuilder.h"

class TestActionAddMeshes : public ::testing::Test
{
public:
	void SetUp() override
	{
		m_addMeshDataList.clear();
		m_cacheDirectory = STRING_MANAGER->cacheFilePath;

		QDir(m_cacheDirectory).removeRecursively();
	}
	void TearDown() override
	{
	}

public:
	void AddMeshData(MeshData* pMeshData, std::string meshName, mip::MeshTopology* pMesh)
	{
		MeshLayerData meshLayer;
		pMeshData->AddNew(meshName, COLOR(0, 0, 0), pMesh, true, &meshLayer);
		ActionAddMeshesData actionData;
		actionData.LayerUID = meshLayer.Info->uid;
		actionData.MeshName = meshLayer.Info->GetName();
		actionData.pMeshData = meshLayer.Data;

		m_addMeshDataList.push_back(actionData);
	}

protected:
	std::vector<ActionAddMeshesData> m_addMeshDataList;
	MeshPrimitiveBuilder m_meshPrimitiveBuilder;
	QString m_cacheDirectory;
};

TEST_F(TestActionAddMeshes, TestConstructor)
{
	std::shared_ptr<TestContext> pTest = TestContext::TestForMesh();
	std::vector<mint32> layerUidList;
	std::vector<QString> fileNameList;
	std::vector<mip::MeshTopology*> meshTopologyList;
	bool isUpScale = false;

	ActionAddMeshes action(
		pTest->pDataContext.get(),
		pTest->pWinManager.get(),
		pTest->pMeshWorkManager.get(),
		pTest->pMeshDlgManager.get(),
		pTest->pPlaneManipulator.get(),
		&pTest->pRenderer->Data,
		m_addMeshDataList,
		m_cacheDirectory,
		isUpScale
	);
}

TEST_F(TestActionAddMeshes, TestRedo_EmptyMeshData)
{
	std::shared_ptr<TestContext> pTest = TestContext::TestForMesh();
	std::vector<mint32> layerUidList;
	std::vector<QString> fileNameList;
	std::vector<mip::MeshTopology*> meshTopologyList;
	bool isUpScale = false;

	ActionAddMeshes action(
		pTest->pDataContext.get(),
		pTest->pWinManager.get(),
		pTest->pMeshWorkManager.get(),
		pTest->pMeshDlgManager.get(),
		pTest->pPlaneManipulator.get(),
		&pTest->pRenderer->Data,
		m_addMeshDataList,
		m_cacheDirectory,
		isUpScale
	);

	action.redo();

	EXPECT_EQ(0, pTest->pDataContext->m_MeshData.GetMeshCount());
}

TEST_F(TestActionAddMeshes, TestRedo_SingleData)
{
	std::shared_ptr<TestContext> pTest = TestContext::TestForMesh();

	AddMeshData(&pTest->pDataContext->m_MeshData, "mesh_1", m_meshPrimitiveBuilder.CreateCube(&pTest->pRenderer->Data, mip::VECTOR3(1, 1, 1)));
	bool isUpScale = false;

	ActionAddMeshes action(
		pTest->pDataContext.get(),
		pTest->pWinManager.get(),
		pTest->pMeshWorkManager.get(),
		pTest->pMeshDlgManager.get(),
		pTest->pPlaneManipulator.get(),
		&pTest->pRenderer->Data,
		m_addMeshDataList,
		m_cacheDirectory,
		isUpScale
	);

	action.redo();

	std::vector<MeshLayerData> meshLayerList = pTest->pDataContext->m_MeshData.GetMeshDataList();
	EXPECT_EQ(1, meshLayerList.size());
	EXPECT_STREQ("mesh_1", meshLayerList[0].Info->GetName().toStdString().c_str());
}

TEST_F(TestActionAddMeshes, TestRedo_MultiData)
{
	std::shared_ptr<TestContext> pTest = TestContext::TestForMesh();

	AddMeshData(&pTest->pDataContext->m_MeshData, "mesh_1", m_meshPrimitiveBuilder.CreateCube(&pTest->pRenderer->Data, mip::VECTOR3(1, 1, 1)));
	AddMeshData(&pTest->pDataContext->m_MeshData, "mesh_2", m_meshPrimitiveBuilder.CreateCube(&pTest->pRenderer->Data, mip::VECTOR3(5, 5, 5)));
	AddMeshData(&pTest->pDataContext->m_MeshData, "mesh_3", m_meshPrimitiveBuilder.CreateCube(&pTest->pRenderer->Data, mip::VECTOR3(10, 10, 10)));
	bool isUpScale = false;

	ActionAddMeshes action(
		pTest->pDataContext.get(),
		pTest->pWinManager.get(),
		pTest->pMeshWorkManager.get(),
		pTest->pMeshDlgManager.get(),
		pTest->pPlaneManipulator.get(),
		&pTest->pRenderer->Data,
		m_addMeshDataList,
		m_cacheDirectory,
		isUpScale
	);

	action.redo();

	std::vector<MeshLayerData> meshLayerList = pTest->pDataContext->m_MeshData.GetMeshDataList();
	EXPECT_STREQ("mesh_1", meshLayerList[0].Info->GetName().toStdString().c_str());
	EXPECT_STREQ("mesh_2", meshLayerList[1].Info->GetName().toStdString().c_str());
	EXPECT_STREQ("mesh_3", meshLayerList[2].Info->GetName().toStdString().c_str());
}

TEST_F(TestActionAddMeshes, TestRedoUndo_SingleData)
{
	std::shared_ptr<TestContext> pTest = TestContext::TestForMesh();

	AddMeshData(&pTest->pDataContext->m_MeshData, "mesh_1", m_meshPrimitiveBuilder.CreateCube(&pTest->pRenderer->Data, mip::VECTOR3(1, 1, 1)));
	bool isUpScale = false;

	ActionAddMeshes action(
		pTest->pDataContext.get(),
		pTest->pWinManager.get(),
		pTest->pMeshWorkManager.get(),
		pTest->pMeshDlgManager.get(),
		pTest->pPlaneManipulator.get(),
		&pTest->pRenderer->Data,
		m_addMeshDataList,
		m_cacheDirectory,
		isUpScale
	);

	action.redo();
	action.undo();

	std::vector<MeshLayerData> meshLayerList = pTest->pDataContext->m_MeshData.GetMeshDataList();
	EXPECT_EQ(0, meshLayerList.size());
}

TEST_F(TestActionAddMeshes, TestRedoUndoRedo_SingleData)
{
	std::shared_ptr<TestContext> pTest = TestContext::TestForMesh();

	AddMeshData(&pTest->pDataContext->m_MeshData, "mesh_1", m_meshPrimitiveBuilder.CreateCube(&pTest->pRenderer->Data, mip::VECTOR3(1, 1, 1)));
	bool isUpScale = false;

	ActionAddMeshes action(
		pTest->pDataContext.get(),
		pTest->pWinManager.get(),
		pTest->pMeshWorkManager.get(),
		pTest->pMeshDlgManager.get(),
		pTest->pPlaneManipulator.get(),
		&pTest->pRenderer->Data,
		m_addMeshDataList,
		m_cacheDirectory,
		isUpScale
	);

	action.redo();
	action.undo();
	action.redo();

	std::vector<MeshLayerData> meshLayerList = pTest->pDataContext->m_MeshData.GetMeshDataList();
	EXPECT_STREQ("mesh_1", meshLayerList[0].Info->GetName().toStdString().c_str());
}

TEST_F(TestActionAddMeshes, TestRedoUndoRedo_MultiData)
{
	std::shared_ptr<TestContext> pTest = TestContext::TestForMesh();

	AddMeshData(&pTest->pDataContext->m_MeshData, "mesh_1", m_meshPrimitiveBuilder.CreateCube(&pTest->pRenderer->Data, mip::VECTOR3(1, 1, 1)));
	AddMeshData(&pTest->pDataContext->m_MeshData, "mesh_2", m_meshPrimitiveBuilder.CreateCube(&pTest->pRenderer->Data, mip::VECTOR3(3, 3, 3)));
	AddMeshData(&pTest->pDataContext->m_MeshData, "mesh_3", m_meshPrimitiveBuilder.CreateCube(&pTest->pRenderer->Data, mip::VECTOR3(5, 5, 5)));
	bool isUpScale = false;

	ActionAddMeshes action(
		pTest->pDataContext.get(),
		pTest->pWinManager.get(),
		pTest->pMeshWorkManager.get(),
		pTest->pMeshDlgManager.get(),
		pTest->pPlaneManipulator.get(),
		&pTest->pRenderer->Data,
		m_addMeshDataList,
		m_cacheDirectory,
		isUpScale
	);

	action.redo();
	action.undo();
	action.redo();

	std::vector<MeshLayerData> meshLayerList = pTest->pDataContext->m_MeshData.GetMeshDataList();
	EXPECT_STREQ("mesh_1", meshLayerList[0].Info->GetName().toStdString().c_str());
	EXPECT_STREQ("mesh_2", meshLayerList[1].Info->GetName().toStdString().c_str());
	EXPECT_STREQ("mesh_3", meshLayerList[2].Info->GetName().toStdString().c_str());
}



