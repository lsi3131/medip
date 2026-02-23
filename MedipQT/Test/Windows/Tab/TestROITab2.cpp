#include "stdafx.h"
#include "Test/test_pch.h"
#include "Tabwindow.h"
#include "Test/System/MockProductManager.h"

class TestROITab2 : public ::testing::Test
{
public:
	void SetUp() override
	{
		m_pProductManager = new MockProductManager(PRODUCT_NAME_MEDIP);
	}
	void TearDown() override
	{
		delete m_pProductManager;
	}

protected:
	MockProductManager* m_pProductManager;
};

TEST_F(TestROITab2, Test)
{
	ROITab2* roiTab = new ROITab2(m_pProductManager);

}
