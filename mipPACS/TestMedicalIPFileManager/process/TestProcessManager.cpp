#include "stdafx.h"
#include "CppUnitTest.h"
#include "TestStub\PACSServer\DicooglePACSServer.h"
#include <filemanager/net/ProcessManager.h>
#include <qthread>
#include <qdir>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestMedicalIPFileManager
{
	TEST_CLASS(TestProcessManager)
	{
	private:
		fm::ProcessManager m_pmanager;
	public:
		TEST_METHOD_INITIALIZE(SetUp)
		{
		}

		TEST_METHOD_CLEANUP(TearDown)
		{
		}

		TEST_METHOD(FindProcessByName_DontCareCase)
		{
			DicooglePACSServer server;
			server.RunAndWaitProcess(1000);
			Assert::IsTrue(fm::ProcessManager::IsProcessExist(L"JaVa.exe"));
		}

		TEST_METHOD(StdString_ToLower)
		{

		}

	private:
	};
}