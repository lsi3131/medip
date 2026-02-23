#include "stdafx.h"
#include "Test/test_pch.h"

class TestTemplate : public ::testing::Test
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

TEST_F(TestTemplate, Test)
{
}
