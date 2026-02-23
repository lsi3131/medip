#include "stdafx.h"
#include "Test/test_pch.h"
#include "MeshEdit/MeshEditScreenSelector.h"
#include "MeshEdit/CMeshWorkManager.h"
#include "MeshEdit/CMeshModelViewManager.h"
#include "graphics/MeshData.h"
#include "graphics/volumedata.h"
#include "Test/Renderer/RendererWrapper.h"

class TestMeshEditSelector : public ::testing::Test
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

TEST_F(TestMeshEditSelector, WhenMeshNotExist_ScreenSelectorReturnFalse)
{
	auto pRenderer = RendererWrapper::DefaultForTest();
	MeshData* pMeshData = new MeshData();
	VOLUME_DATA* pVolumeData = new VOLUME_DATA();
	CMeshWorkManager* pMeshWorkManager = new CMeshWorkManager();
	CMeshModelViewManager* pMeshModelViewManager = new CMeshModelViewManager();
	MeshEditScreenSelector selector(&pRenderer->Data, pMeshData, pVolumeData, pMeshWorkManager, pMeshModelViewManager);

	MeshLayerData meshLayer;
	QPoint screenPoint(1, 1);
	QSize screenSize(10, 10);

	EXPECT_FALSE(selector.TryGetIntersectedMesh(&meshLayer, screenPoint, screenSize));
}
