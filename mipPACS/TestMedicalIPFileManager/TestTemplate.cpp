#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestMedicalIPFileManager
{
	TEST_CLASS(TestTemplate)
	{
	public:
		TEST_METHOD_INITIALIZE(SetUp)
		{
		}

		TEST_METHOD_CLEANUP(TearDown)
		{
		}

		TEST_METHOD(Test)
		{
			Assert::AreEqual(1, 1);
		}

	private:
	};
}