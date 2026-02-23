#include "stdafx.h"
#include "CppUnitTest.h"
#include "QProcessEventHandlerTestStub.h"
#include "DicooglePACSServer.h"
#include <qfile>
#include <qdir>
#include <qprocess>
#include <QtConcurrent/qtconcurrentrun.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace TestMedicalIPFileManager
{
	TEST_CLASS(TestDicooglePACSServer)
	{
	private:
		DicooglePACSServer* m_pDicoogleServer;
	public:
		TEST_METHOD_INITIALIZE(SetUp)
		{
			m_pDicoogleServer = new DicooglePACSServer();
			m_pDicoogleServer->DeleteStorageDirectory();
		}

		TEST_METHOD_CLEANUP(TearDown)
		{
			m_pDicoogleServer->KillAndWaitProcess();
			m_pDicoogleServer->DeleteStorageDirectory();
			delete m_pDicoogleServer;
		}

		TEST_METHOD(TestRunTwiceReturnFalse)
		{
			m_pDicoogleServer->RunAndWaitProcess(100);
			Assert::IsFalse(m_pDicoogleServer->RunAndWaitProcess(100));
		}

		TEST_METHOD(TestDicoogleJarFileExist)
		{
			QDir dir(m_pDicoogleServer->DicoogleDirPath);
			QFile file(m_pDicoogleServer->DicoogleFilepath);
			Assert::IsTrue(dir.exists());
			Assert::IsTrue(file.exists());
		}

		TEST_METHOD(TestRunDicoogle_StartFinish_EventCheck)
		{
			m_pDicoogleServer->RunAndWaitProcess(500);
			m_pDicoogleServer->KillAndWaitProcess();

			Assert::AreEqual(1, m_pDicoogleServer->ProcessEventHandler->StartedCount);
			Assert::AreEqual(1, m_pDicoogleServer->ProcessEventHandler->FinishedCount);
		}

		TEST_METHOD(TestStorageDirectoryShouldBeEmptry_BeforeTest)
		{
			QDir dir;
			Assert::IsFalse(dir.exists(m_pDicoogleServer->IndexDirPath));
			Assert::IsFalse(dir.exists(m_pDicoogleServer->StorageDirPath));
		}


	private:
	};
}