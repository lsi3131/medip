#include "stdafx.h"
#include "Test/test_pch.h"
#include "graphics/MeshLayerDataOmniverse.h"
#include "Renderer/Renderer.h"

class TestMeshLayerDataOmniverse : public ::testing::Test
{
public:
	void SetUp() override
	{
		m_testDirPath = GetUnitTestDataDirectory("Mesh");
		m_cubeSTLFilePath = m_testDirPath + "/cube.stl";
		m_cylinderSTLFilePath = m_testDirPath + "/cylinder.stl";
		m_sphereSTLFilePath = m_testDirPath + "/sphere.stl";
		m_pRenderer = new mip::Renderer();

	}
	void TearDown() override
	{
	}

protected:
	mip::Renderer* m_pRenderer;
	std::string m_testDirPath;
	std::string m_cubeSTLFilePath;
	std::string m_cylinderSTLFilePath;
	std::string m_sphereSTLFilePath;
};

TEST_F(TestMeshLayerDataOmniverse, TestFileExist)
{
	EXPECT_TRUE(QFile::exists(QString::fromStdString(m_testDirPath)));
	EXPECT_TRUE(QFile::exists(QString::fromStdString(m_cubeSTLFilePath)));
	EXPECT_TRUE(QFile::exists(QString::fromStdString(m_cylinderSTLFilePath)));
	EXPECT_TRUE(QFile::exists(QString::fromStdString(m_sphereSTLFilePath)));
}

TEST_F(TestMeshLayerDataOmniverse, TestLoadSTL_InvalidFilePath)
{
	MeshLayerDataOmniverse meshData(m_pRenderer);
	EXPECT_FALSE(meshData.LoadSTL("c:\\invalid\\file\\path\\notexist", "name"));
}

TEST_F(TestMeshLayerDataOmniverse, TestLoadSTL)
{
	MeshLayerDataOmniverse meshData(m_pRenderer);
	EXPECT_TRUE(meshData.LoadSTL(m_cubeSTLFilePath, "name"));
	EXPECT_STREQ(L"name", meshData.GetInfo()->MeshName);
	EXPECT_NE(nullptr, meshData.GetData());
}


