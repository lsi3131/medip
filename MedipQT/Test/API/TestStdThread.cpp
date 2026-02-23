#include "stdafx.h"
#include "Test/test_pch.h"
#include <thread>
#include <memory>

class TestInternalThreadWorker
{
public:
	TestInternalThreadWorker(QStringList* pMessageList, int waitTime_ms)
	{
		IsRun = true;
		pThread = std::make_unique<std::thread>(&TestInternalThreadWorker::Run, this);
		this->pMessageList = pMessageList;
		this->WaitTime_ms = waitTime_ms;
	}

	~TestInternalThreadWorker()
	{
		IsRun = false;
		if (pThread != nullptr)
		{
			pThread->join();
		}
	}

	void Run()
	{
		*pMessageList << "start";
		while (IsRun)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(WaitTime_ms));
			//OmniverseLiveChannelMessagePtr pMessage = nullptr;
			//if (TryPopQueueEvent(&pMessage))
			//{
			//	qInfo() << "pop queue event. message : " << pMessage->GetType_Text().c_str();
			//	emit sig_receiveMessage();
			//}
		}
		*pMessageList << "end";
	}

	bool IsRun = true;
	std::unique_ptr<std::thread> pThread;
	QStringList* pMessageList;
	int WaitTime_ms = 0;
};

class TestStdThread : public ::testing::Test
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

TEST_F(TestStdThread, TestRunAndWait)
{
	QStringList messageList;
	////TestInternalThreadWorker* pWorker = new TestInternalThreadWorker(&messageList, 3000);
	//std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	//
	//delete pWorker;

	//EXPECT_EQ(QStringList({ "start", "end" }), messageList);

	TestInternalThreadWorker Worker(&messageList, 3000);
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}
