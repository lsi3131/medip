#include "stdafx.h"
#include "Test/test_pch.h"
#include "OmniverseFileTestResource.h"
#include "Omniverse/OmniverseContext.h"
#include "Omniverse/OmniverseStageEventListenerWorker.h"
#include "Test/Omniverse/TestOmniverseStageEventListenerWorkerHandlerStub.h"
#include "Test/Omniverse/CommandLineTestProcessor.h"

class TestOmniverseStageEventListenerWorker : public ::testing::Test
{
public:
	void SetUp() override
	{
		m_pEventHandler = new TestOmniverseStageEventListenerWorkerHandlerStub();
	}
	void TearDown() override
	{
		delete m_pEventHandler;
	}

protected:
	OmniverseFileTestResource Resource;
	TestOmniverseStageEventListenerWorkerHandlerStub* m_pEventHandler;
};

TEST_F(TestOmniverseStageEventListenerWorker, TestCreateUsdModel_By_CommandLineProcess)
{
}


TEST_F(TestOmniverseStageEventListenerWorker, TestEventListener_And_WaitOtherProcessChange)
{
}

