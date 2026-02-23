#include "stdafx.h"
#include "Test/test_pch.h"
#include "MeshEdit/CManipulator.h"
#include "MeshEdit/CMeshDlgManager.h"
#include "Test/Renderer/RendererWrapper.h"

class Test_CManipulator : public ::testing::Test
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

TEST_F(Test_CManipulator, TestConstructor)
{
	auto pRenderer = RendererWrapper::DefaultForTest();
	CMeshModelViewManager modeViewManager;
	CMeshDlgManager dlgManager;
	CManipulator manipulator(&pRenderer->Data);
	manipulator.Init(&modeViewManager, &dlgManager);
}
