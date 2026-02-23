#include "stdafx.h"
#include "CppUnitTest.h"
#include <qdir>
#include <qprocess>
#include <Windows.h>
#include "filemanager/net/Server.h"
#include "filemanager/net/ProcessManager.h"
#include <qthread>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace fm;


namespace TestMedicalIPFileManager
{
	TEST_CLASS(TestFileManagerServer)
	{
	private:
	public:
		TEST_METHOD_INITIALIZE(SetUp)
		{
		}

		TEST_METHOD_CLEANUP(TearDown)
		{
		}

	private:
	};
}