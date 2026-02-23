#include "stdafx.h"
#include "Test/test_pch.h"
#include "Dialogs/BrushSculptDlg.h"

class TestBrushSculptDlg : public ::testing::Test
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

TEST_F(TestBrushSculptDlg, Test)
{
	DataContext* pDataContext = new DataContext();
	BrushSculptDlg dlg(pDataContext);
}
