#include "stdafx.h"
#include "Test/test_pch.h"
#include "Actions/Omniverse/ActionSendToOmniverse.h"
#include "graphics/DataContext.h"
#include "Omniverse/OmniverseContext.h"

class TestActionSendToOmniverse : public ::testing::Test
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

TEST_F(TestActionSendToOmniverse, TestConstructor)
{
	DataContext dataContext;
	QList<muint32> sendIndexList;
	ActionSendToOmniverse action(&dataContext, sendIndexList);
}
