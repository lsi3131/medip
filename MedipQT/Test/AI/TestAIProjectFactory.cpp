#include "stdafx.h"
#include "Test/test_pch.h"
#include "AI/AIProjectFactory.h"

class TestAIProjectFactory : public ::testing::Test
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

TEST_F(TestAIProjectFactory, TestDefaultMedipProjectList)
{
	std::shared_ptr<AIProjectFactory> pFactory = AIProjectFactory::NewMEDIP();
	EXPECT_EQ(50, pFactory->GetProjectList().size());
}
