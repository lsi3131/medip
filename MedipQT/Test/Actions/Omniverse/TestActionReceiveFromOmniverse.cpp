#include "stdafx.h"
#include "Test/test_pch.h"
#include "Actions/Omniverse/ActionReceiveFromOmniverse.h"
#include "Omniverse/OmniverseContext.h"
#include "graphics/DataContext.h"
#include "Windows/WindowManager.h"
#include "ShortcutManager.h"
#include "MeshEdit/CMeshWorkManager.h"
#include "MeshEdit/CMeshManipulator.h"

class TestActionReceiveFromOmniverse : public ::testing::Test
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

TEST_F(TestActionReceiveFromOmniverse, TestConstructor)
{
	DataContext* dataContext = new DataContext();
	WindowManager* winManager = new WindowManager();
	CMeshWorkManager* meshWorkManager = new CMeshWorkManager();
	CMeshManipulator* meshManipulator = new CMeshManipulator();
	ShortcutManager* shorcutManager = new ShortcutManager();

	ActionReceiveFromOmniverse action(dataContext, winManager, meshWorkManager, meshManipulator, shorcutManager);
}
