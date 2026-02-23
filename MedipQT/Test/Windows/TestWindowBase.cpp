#include "stdafx.h"
#include "Test/test_pch.h"
#include "Windows/WindowBase.h"
#include "Windows/windowManager.h"
#include "MedipQT.h"

class TestWindowBase : public ::testing::Test
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

TEST_F(TestWindowBase, TestConstructor)
{
	//WindowManager* winManager = new WindowManager();
	//WIN_MANAGER->mainWindow = ;
	//MedipQT* mainwindow = new MedipQT();
	//WindowBase* win = new WindowBase(nullptr);
}
