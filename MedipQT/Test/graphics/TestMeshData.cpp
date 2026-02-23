#include "stdafx.h"
#include "Test/test_pch.h"
#include "graphics/MeshData.h"
#include "WindowManager.h"
#include "Renderer/Renderer.h"
#include "Test/Renderer/RendererWrapper.h"

class TestMeshData : public ::testing::Test
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

TEST_F(TestMeshData, TestConstructMeshTopology)
{
	auto renderer = RendererWrapper::DefaultForTest();
	mip::MeshTopology* pTopology = new mip::MeshTopology(&renderer->Data);
}

TEST_F(TestMeshData, WhenMeshInfoNotExist_InsertMeshReturnFalse)
{
	auto renderer = RendererWrapper::DefaultForTest();
	CMeshModelViewManager meshViewModel;
	CMeshManipulator manipulator(&renderer->Data);
	MeshData meshData(&renderer->Data, &manipulator);
	int meshUID = 0;
	mip::MeshTopology* pTopology = new mip::MeshTopology(&renderer->Data);

	EXPECT_FALSE(meshData.InsertMesh(meshUID, pTopology));
}

