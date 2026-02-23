#include "stdafx.h"
#include "CppUnitTest.h"
#include "QProcessEventHandlerTestStub.h"
#include "dcmqrscpPACSServer.h"
#include <qfile>
#include <qdir>
#include <qprocess>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace TestMedicalIPFileManager
{
	TEST_CLASS(TestdcmqrscpPACSServer)
	{
	private:
		dcmqrscpPACSServer* m_pServer;
	public:
		TEST_METHOD_INITIALIZE(SetUp)
		{
			m_pServer = new dcmqrscpPACSServer();
		}

		TEST_METHOD_CLEANUP(TearDown)
		{
			delete m_pServer;
		}

		TEST_METHOD(Test_dcmqrscp_FileIsExist)
		{
			QDir dir;
			//Assert::AreEqual("", dir.currentPath().toLocal8Bit().data());
			Assert::IsTrue(dir.exists(m_pServer->RootDirPath));
			Assert::IsTrue(dir.exists(m_pServer->ExeFilePath));
		}

		TEST_METHOD(Test_dcmqrscp_StartFinish_EventCheck)
		{
			m_pServer->RunAndWaitProcess(500);
			m_pServer->KillAndWaitProcess();

			Assert::AreEqual(1, m_pServer->ProcessEventHandler->StartedCount);
			Assert::AreEqual(1, m_pServer->ProcessEventHandler->FinishedCount);
		}
	};
}