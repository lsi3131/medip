#include "pch.h"


class TestDumi : public ::testing::Test
{
public:
	TestDumi() {}

	void SetUp() override
	{
	}

	void TearDown() override
	{
	}
};


TEST_F(TestDumi, TestDumi1)
{
	EXPECT_EQ(1, 1);
}
