#include "stdafx.h"
#include "Test/test_pch.h"
#include "graphics/Mesh/MeshPrimitiveBuilder.h"
#include "graphics/MeshData.h"
#include "Renderer/MeshTopology.h"
#include "Test/Renderer/RendererWrapper.h"

class TestMeshPrimitiveBuilder : public ::testing::Test
{
public:
	void SetUp() override
	{
	}
	void TearDown() override
	{
	}

protected:
};

TEST_F(TestMeshPrimitiveBuilder, TestBuildCube)
{
	auto pRenderer = RendererWrapper::DefaultForTest();
	mip::MeshTopology mesh(&pRenderer->Data);
	mip::VECTOR3 size_mm(50.0f, 50.0f, 50.0f);
	MeshPrimitiveBuilder builder;

	EXPECT_TRUE(builder.BuildCube(&pRenderer->Data, &mesh, size_mm));
	EXPECT_EQ(8, mesh.m_verts.size());
	EXPECT_EQ(36, mesh.m_tris.size());
	EXPECT_EQ(8, mesh.m_normals.size());
	EXPECT_EQ(0, mesh.m_ttris.size());
	EXPECT_EQ(0, mesh.m_tverts.size());
	EXPECT_EQ(0, mesh.m_tVHedges.size());
}

TEST_F(TestMeshPrimitiveBuilder, TestBuildCubeSubdivision)
{
	auto pRenderer = RendererWrapper::DefaultForTest();
	mip::MeshTopology mesh(&pRenderer->Data);
	mip::VECTOR3 size_mm(50.0f, 50.0f, 50.0f);
	COLOR color(255, 0, 0);
	MeshPrimitiveBuilder builder;

	EXPECT_TRUE(builder.BuildCube_Subdivision(&pRenderer->Data, &mesh, size_mm, color));
	EXPECT_EQ(72, mesh.m_verts.size());
	EXPECT_EQ(72, mesh.m_tris.size());
	EXPECT_EQ(72, mesh.m_normals.size());
	EXPECT_EQ(24, mesh.m_ttris.size());
	EXPECT_EQ(14, mesh.m_tverts.size());
	EXPECT_EQ(72, mesh.m_tVHedges.size());
}

TEST_F(TestMeshPrimitiveBuilder, TestBuildCylinder)
{
	auto pRenderer = RendererWrapper::DefaultForTest();
	mip::MeshTopology mesh(&pRenderer->Data);
	mip::VECTOR3 size_mm(50.0f, 50.0f, 50.0f);
	int triangleCount = 25;
	MeshPrimitiveBuilder builder;

	EXPECT_TRUE(builder.BuildCylinder(&pRenderer->Data, &mesh, size_mm, triangleCount));
	EXPECT_EQ(52, mesh.m_verts.size());
	EXPECT_EQ(300, mesh.m_tris.size());
	EXPECT_EQ(52, mesh.m_normals.size());
	EXPECT_EQ(0, mesh.m_ttris.size());
	EXPECT_EQ(0, mesh.m_tverts.size());
	EXPECT_EQ(0, mesh.m_tVHedges.size());
}


TEST_F(TestMeshPrimitiveBuilder, TestBuildCylinder_Minimum)
{
	auto pRenderer = RendererWrapper::DefaultForTest();
	mip::MeshTopology mesh(&pRenderer->Data);
	mip::VECTOR3 size_mm(50.0f, 50.0f, 50.0f);
	int triangleCount = 5;
	MeshPrimitiveBuilder builder;

	EXPECT_TRUE(builder.BuildCylinder(&pRenderer->Data, &mesh, size_mm, triangleCount));
	EXPECT_EQ(12, mesh.m_verts.size());
	EXPECT_EQ(60, mesh.m_tris.size());
	EXPECT_EQ(12, mesh.m_normals.size());
	EXPECT_EQ(0, mesh.m_ttris.size());
	EXPECT_EQ(0, mesh.m_tverts.size());
	EXPECT_EQ(0, mesh.m_tVHedges.size());
}

TEST_F(TestMeshPrimitiveBuilder, TestBuildCylinderSubdivision)
{
	auto pRenderer = RendererWrapper::DefaultForTest();
	mip::MeshTopology mesh(&pRenderer->Data);
	mip::VECTOR3 size_mm(50.0f, 50.0f, 50.0f);
	int triangleCount = 25;
	COLOR color(255, 0, 0);
	MeshPrimitiveBuilder builder;

	EXPECT_TRUE(builder.BuildCylinder_Subdivision(&pRenderer->Data, &mesh, size_mm, color, triangleCount));
	EXPECT_EQ(600, mesh.m_verts.size());
	EXPECT_EQ(600, mesh.m_tris.size());
	EXPECT_EQ(600, mesh.m_normals.size());
	EXPECT_EQ(200, mesh.m_ttris.size());
	EXPECT_EQ(102, mesh.m_tverts.size());
	EXPECT_EQ(600, mesh.m_tVHedges.size());
}

TEST_F(TestMeshPrimitiveBuilder, TestBuildSphere)
{
	auto pRenderer = RendererWrapper::DefaultForTest();
	mip::MeshTopology mesh(&pRenderer->Data);
	mip::VECTOR3 size_mm(50.0f, 50.0f, 50.0f);
	int resolutionTheta = 25;
	int resolutionPhi = 25;
	MeshPrimitiveBuilder builder;

	EXPECT_TRUE(builder.BuildSphere(&pRenderer->Data, &mesh, size_mm, resolutionTheta, resolutionPhi));
	EXPECT_EQ(577, mesh.m_verts.size());
	EXPECT_EQ(3450, mesh.m_tris.size());
	EXPECT_EQ(577, mesh.m_normals.size());
	EXPECT_EQ(0, mesh.m_ttris.size());
	EXPECT_EQ(0, mesh.m_tverts.size());
	EXPECT_EQ(0, mesh.m_tVHedges.size());
}

TEST_F(TestMeshPrimitiveBuilder, TestBuildSphereSubdivision)
{
	auto pRenderer = RendererWrapper::DefaultForTest();
	mip::MeshTopology mesh(&pRenderer->Data);
	mip::VECTOR3 size_mm(50.0f, 50.0f, 50.0f);
	int resolutionTheta = 25;
	int resolutionPhi = 25;
	COLOR color(255, 0, 0);
	MeshPrimitiveBuilder builder;

	EXPECT_TRUE(builder.BuildSphere_Subdivision(&pRenderer->Data, &mesh, size_mm, color, resolutionTheta, resolutionPhi));
	EXPECT_EQ(7800, mesh.m_verts.size());
	EXPECT_EQ(7800, mesh.m_tris.size());
	EXPECT_EQ(7800, mesh.m_normals.size());
	EXPECT_EQ(2600, mesh.m_ttris.size());
	EXPECT_EQ(1302, mesh.m_tverts.size());
	EXPECT_EQ(7800, mesh.m_tVHedges.size());
}

