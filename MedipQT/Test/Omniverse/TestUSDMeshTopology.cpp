#include "stdafx.h"
#include "Test/test_pch.h"
#include "Omniverse/Usd/mipUSDMesh.h"
#include "Renderer/Renderer.h"

class TestUSDMeshTopology : public ::testing::Test
{
public:
	void SetUp() override
	{
		m_testDirPath = GetUnitTestDataDirectory("Omniverse/Mesh");
		m_cubeSTLFilePath = m_testDirPath + "/cube.stl";
		m_tempSTLFilePath = m_testDirPath + "/temp.stl";
		m_pRenderer = new mip::Renderer();

		QFile::remove(m_tempSTLFilePath.c_str());
	}
	void TearDown() override
	{
		if (m_pRenderer)
		{
			//delete renderer;
		}
	}

protected:
	std::string m_tempSTLFilePath;
	std::string m_testDirPath;
	std::string m_cubeSTLFilePath;
	mip::Renderer* m_pRenderer;
};

TEST_F(TestUSDMeshTopology, TestFileExist)
{
	EXPECT_TRUE(QFile::exists(QString::fromStdString(m_cubeSTLFilePath)));
	EXPECT_FALSE(QFile::exists(QString::fromStdString(m_tempSTLFilePath)));
}

//TEST_F(TestUSDMeshTopology, TestLoadSTL)
//{
//	mipUSDMesh usdTopology;
//	EXPECT_TRUE(usdTopology.LoadSTL(m_cubeSTLFilePath));
//
//	EXPECT_EQ(72, usdTopology.GetVertice_mm().size());
//	EXPECT_EQ(24, usdTopology.GetNormals().size());
//	EXPECT_EQ(72, usdTopology.GetIndices().size());
//	EXPECT_EQ(24, usdTopology.GetFaceVertexCounts().size());
//}
//
//TEST_F(TestUSDMeshTopology, TestLoadSTL_mipMeshTopology)
//{
//	mip::MeshTopology mipTopology(m_pRenderer);
//
//	EXPECT_TRUE(mipTopology.LoadSTL(m_cubeSTLFilePath.c_str()));
//
//	EXPECT_EQ(72, mipTopology.m_verts.size());
//	EXPECT_EQ(72, mipTopology.m_normals.size());
//	EXPECT_EQ(72, mipTopology.m_tris.size());
//}
//
//TEST_F(TestUSDMeshTopology, Compare_USD_To_Mip_Topology)
//{
//	mip::MeshTopology mipTopology(m_pRenderer);
//	mipTopology.LoadSTL(m_cubeSTLFilePath.c_str());
//
//	mipTopology.setTranslate(mip::VECTOR3(1.0f, 1.5f, 2.0f));
//	mipTopology.setRotate(mip::QUATERNION(1.0f, 2.0f, 3.0f, 1.0f));
//	mipTopology.setScale(mip::VECTOR3(2.0f, 3.0f, 4.0f));
//
//	mipUSDMesh usdTopology;
//	usdTopology.SetDataByMeshDataAndInfo(mipTopology);
//
//	//EXPECT_EQ(mipTopology.translation, usdTopology.GetTranslation());
//	//EXPECT_EQ(usdTopology.points, usdTopology_Converted.points);
//	//EXPECT_EQ(usdTopology.vecIndices, usdTopology_Converted.vecIndices);
//	//EXPECT_EQ(usdTopology.faceVertexCounts, usdTopology_Converted.faceVertexCounts);
//}
//
//TEST_F(TestUSDMeshTopology, TestInitFromMipMeshTopology)
//{
//	mip::MeshTopology mipTopology(m_pRenderer);
//	mipUSDMesh usdTopology;
//	mipUSDMesh usdTopology_Converted;
//
//	mipTopology.LoadSTL(m_cubeSTLFilePath.c_str());
//	usdTopology.LoadSTL(m_cubeSTLFilePath);
//
//	usdTopology_Converted.SetDataByMeshDataAndInfo(mipTopology);
//
//	EXPECT_EQ(usdTopology.meshNormals, usdTopology_Converted.meshNormals);
//	EXPECT_EQ(usdTopology.points_mm, usdTopology_Converted.points_mm);
//	EXPECT_EQ(usdTopology.vecIndices, usdTopology_Converted.vecIndices);
//	EXPECT_EQ(usdTopology.faceVertexCounts, usdTopology_Converted.faceVertexCounts);
//}
//
//TEST_F(TestUSDMeshTopology, TestEquality)
//{
//	mipUSDMesh eq1;
//	mipUSDMesh eq2;
//	mipUSDMesh ne1;
//
//	eq1.points_mm = { {1,2,3}, {4,5,6} };
//	eq1.meshNormals = { {1,2,3}, {4,5,6} };
//	eq1.vecIndices = { 1,2,3 };
//	eq1.faceVertexCounts = { 1,2,3 };
//
//	eq2.points_mm = { {1,2,3}, {4,5,6} };
//	eq2.meshNormals = { {1,2,3}, {4,5,6} };
//	eq2.vecIndices = { 1,2,3 };
//	eq2.faceVertexCounts = { 1,2,3 };
//
//	ne1.points_mm = { {4,5,6} };
//	ne1.meshNormals = { {4,5,6} };
//	ne1.vecIndices = { 4,5,6 };
//	ne1.faceVertexCounts = { 4,5,6 };
//
//	EXPECT_EQ(eq1, eq2);
//	EXPECT_NE(eq1, ne1);
//}
//
//TEST_F(TestUSDMeshTopology, TestSaveSTL)
//{
//	mipUSDMesh d1;
//	d1.LoadSTL(m_cubeSTLFilePath);
//	d1.SaveSTL(m_tempSTLFilePath);
//
//	EXPECT_TRUE(QFile::exists(m_tempSTLFilePath.c_str()));
//
//	mipUSDMesh d2;
//	d2.LoadSTL(m_tempSTLFilePath);
//
//	EXPECT_EQ(d1.points_mm, d2.points_mm);
//	EXPECT_EQ(d1.meshNormals, d2.meshNormals);
//	EXPECT_EQ(d1.vecIndices, d2.vecIndices);
//	EXPECT_EQ(d1.faceVertexCounts, d2.faceVertexCounts);
//
//	EXPECT_EQ(d1, d2);
//}