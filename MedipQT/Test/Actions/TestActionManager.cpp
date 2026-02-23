#include "stdafx.h"
#include "Test/test_pch.h"
#include "Actions/ActionManager.h"
#include "Windows/windowManager.h"
#include "ProductManager.h"
#include "MedipQT.h"

class ActionWorkerAdder : public ActionWorkBase
{
public:
	ActionWorkerAdder(int nResult, int* pCallCount);
	virtual ~ActionWorkerAdder() {};

public:
	virtual void threadRun();

public:
	int* m_pCallCount = 0;
};

ActionWorkerAdder::ActionWorkerAdder(int nResult, int* pCallCount)
{
	m_pCallCount = pCallCount;;
}

void ActionWorkerAdder::threadRun()
{
	(*m_pCallCount)++;
}

class TestActionManager : public ::testing::Test
{
public:
	TestActionManager() {}

	void SetUp() override
	{
		//int argc = 1;
		//const char* argv[] = { "dumi" };
		//m_app = new QApplication(argc, (char**)argv);
	}

	void TearDown() override
	{
		//ACTION_MANAGER->ClearAll();
		//PRODUCT_MANAGER->Clear();
	}

	void InitProductManager()
	{
		std::string productName = PRODUCT_NAME_MEDIP;
		bool online = true;
		PRODUCT_MANAGER->initProductResource(productName);
		PRODUCT_MANAGER->SetProduct(MedipType::New(L"name", L"usd", online));
	}

	void InitMedipQt()
	{
	}

	void OnCallBackMedipClose(void* pContext)
	{
		//m_medip->close();
	}

public:
	void* m_pContext;
	int argc = 1;
	const char* argv[1] = { "dumi" };
	QString m_errorMessage;
	QThread* m_pOutThread;
};

TEST_F(TestActionManager, TestSingleWorker)
{
	std::deque<ThreadArgExtension> queueThreadProcess;

	int count = 0;
	queueThreadProcess.push_back(new ActionWorkerAdder(0, &count));

	ACTION_MANAGER->action_Start_List(queueThreadProcess, &m_errorMessage, &m_pOutThread);
	m_pOutThread->wait();

	EXPECT_EQ(1, count);
}

TEST_F(TestActionManager, TestMultiWorker)
{
	std::deque<ThreadArgExtension> queueThreadArguments;

	int count = 0;
	queueThreadArguments.push_back(new ActionWorkerAdder(0, &count));
	queueThreadArguments.push_back(new ActionWorkerAdder(0, &count));
	queueThreadArguments.push_back(new ActionWorkerAdder(0, &count));

	ACTION_MANAGER->action_Start_List(queueThreadArguments, &m_errorMessage, &m_pOutThread);
	m_pOutThread->wait();

	EXPECT_EQ(3, count);
}
