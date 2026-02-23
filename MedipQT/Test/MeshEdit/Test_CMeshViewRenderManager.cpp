#include "stdafx.h"
#include "Test/test_pch.h"
#include "MeshEdit/CMeshViewRenderManager.h"

class Test_CMeshViewRenderManager : public ::testing::Test
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

TEST_F(Test_CMeshViewRenderManager, TestConstructor)
{
	CMeshViewRenderManager renderer;
}
