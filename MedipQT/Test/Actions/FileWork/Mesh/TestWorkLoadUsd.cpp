#include "stdafx.h"
#include "Test/test_pch.h"
#include "Test/Omniverse/Usd/UsdFileTestResource.h"
#include "Test/TestContext.h"

class TestWorkLoadUsd : public ::testing::Test
{
public:
	void SetUp() override
	{
		m_testDirectoryPath = GetUnitTestDataDirectory("usd").c_str();
		m_tempUsdFilePath = m_testDirectoryPath + "/temp.usd";

		QDir().mkpath(m_testDirectoryPath);
		QFile::remove(m_tempUsdFilePath);
	}
	void TearDown() override
	{
	}

protected:
	UsdFileTestResource m_usdFileTestResource;
	QString m_testDirectoryPath;
	QString m_tempUsdFilePath;
};

TEST_F(TestWorkLoadUsd, TestFileExist)
{
	EXPECT_TRUE(QFile::exists(m_usdFileTestResource.CubeFilePath.c_str()));
}


TEST_F(TestWorkLoadUsd, TestLoadUsd_SingleWork)
{
	std::shared_ptr<TestContext> pTest = TestContext::TestForMesh();
	pTest->pActionManager->setUndoStack(new QUndoStack());

	WorkLoadUsd work(
		QString::fromStdString(m_usdFileTestResource.CubeFilePath_UsdObj),
		&pTest->pRenderer->Data,
		pTest->pWinManager.get(),
		pTest->pDataContext.get(),
		pTest->pMeshWorkManager.get(),
		pTest->pMeshManipulator.get(),
		pTest->pShortcutManager.get(),
		pTest->pActionManager.get());

	work.threadRun();

	EXPECT_EQ(1, pTest->pDataContext->m_MeshData.GetMeshCount());
	EXPECT_STREQ("Cube", pTest->pDataContext->m_MeshData.Get(0).Info->GetName().toStdString().c_str());
}

TEST_F(TestWorkLoadUsd, Test_UndoAndRedo)
{
	std::shared_ptr<TestContext> pTest = TestContext::TestForMesh();
	pTest->pActionManager->setUndoStack(new QUndoStack());

	WorkLoadUsd work(
		QString::fromStdString(m_usdFileTestResource.CubeFilePath_UsdObj),
		&pTest->pRenderer->Data,
		pTest->pWinManager.get(),
		pTest->pDataContext.get(),
		pTest->pMeshWorkManager.get(),
		pTest->pMeshManipulator.get(),
		pTest->pShortcutManager.get(),
		pTest->pActionManager.get());

	work.threadRun();

	pTest->pActionManager->getUndoStack()->undo();
	EXPECT_EQ(0, pTest->pDataContext->m_MeshData.GetMeshCount());

	pTest->pActionManager->getUndoStack()->redo();
	EXPECT_EQ(1, pTest->pDataContext->m_MeshData.GetMeshCount());
	EXPECT_STREQ("Cube", pTest->pDataContext->m_MeshData.Get(0).Info->GetName().toStdString().c_str());
}

