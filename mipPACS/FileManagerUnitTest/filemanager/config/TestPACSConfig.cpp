#include "pch.h"

class TestPACSConfig : public ::testing::Test
{
public:
	TestPACSConfig() {}

	void SetUp() override
	{
	}

	void TearDown() override
	{
	}
};


TEST_F(TestPACSConfig, Test)
{
	EXPECT_EQ(1, 1);
}
