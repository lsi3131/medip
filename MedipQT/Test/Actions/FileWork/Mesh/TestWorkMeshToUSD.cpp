#include "stdafx.h"
#include "Test/test_pch.h"
#include "Test/Renderer/RendererWrapper.h"
#include "Actions/FileWork/Mesh/WorkMeshToUSD.h"
#include "graphics/Mesh/MeshPrimitiveBuilder.h"
#include "Omniverse/Usd/mipUsdStage.h"

class TestWorkMeshToUSD : public ::testing::Test
{
public:
	void SetUp() override
	{
		m_testDirectoryPath = GetUnitTestDataDirectory("usd").c_str();
		m_tempUsdFilePath = m_testDirectoryPath + "/temp.usd";
		m_pRenderer = RendererWrapper::DefaultForTest();

		QDir().mkpath(m_testDirectoryPath);
		QFile::remove(m_tempUsdFilePath);
	}
	void TearDown() override
	{
	}

protected:
	QString m_testDirectoryPath;
	QString m_tempUsdFilePath;
	std::shared_ptr<RendererWrapper> m_pRenderer;
};

TEST_F(TestWorkMeshToUSD, WhenMeshDataEmpty_ExportFileFailed)
{
	MeshData meshData;
	std::vector<int> indexList;

	WorkMeshToUSD work(&meshData, indexList, m_tempUsdFilePath);
	work.threadRun();

	EXPECT_FALSE(QFile::exists(m_tempUsdFilePath));
}

TEST_F(TestWorkMeshToUSD, TestExportSingleMesh)
{
	MeshData meshData;
	MeshPrimitiveBuilder meshBuilder;
	meshData.AddNew("cube", COLOR(255, 255, 255), meshBuilder.CreateCube(&m_pRenderer->Data, mip::VECTOR3(10, 20, 30)), false);
	std::vector<int> indexList = {0};

	WorkMeshToUSD work(&meshData, indexList, m_tempUsdFilePath);
	work.threadRun();

	EXPECT_TRUE(QFile::exists(m_tempUsdFilePath));

	mipUsdStage usdStage;
	EXPECT_TRUE(usdStage.Open(m_tempUsdFilePath.toStdString()));
	EXPECT_TRUE(usdStage.IsMeshExist("cube"));
}

