#include "stdafx.h"
#include "CppUnitTest.h"
#include "filemanager/appcore/Util/FileFilterParser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace fm;

namespace TestMedicalIPFileManager
{

	TEST_CLASS(TestFileFilterParser)
	{
	private:
		std::vector<FileFilterInfo> m_list;

	public:
		TEST_METHOD_INITIALIZE(SetUp)
		{
		}

		TEST_METHOD_CLEANUP(TearDown)
		{
		}

		TEST_METHOD(TestEmptyString_ReturnEmptyList)
		{
			m_list = FileFilterParser::Parse("");
			Assert::AreEqual(0, (int)m_list.size());
		}

		TEST_METHOD(TestOneFilterWithoutName)
		{
			m_list = FileFilterParser::Parse("*.stl");
			Assert::AreEqual(1, (int)m_list.size());
			AssertFileFilterInfoEquals({ "*.stl" }, m_list[0].Filters());
		}


		TEST_METHOD(TestThreeFilterWithoutName)
		{
			m_list = FileFilterParser::Parse("*.stl;*.STL;*.vtk");
			Assert::AreEqual(1, (int)m_list.size());
			AssertFileFilterInfoEquals({ "*.stl", "*.STL", "*.vtk" }, m_list[0].Filters());
		}

		TEST_METHOD(TestOneFilterWithName)
		{
			m_list = FileFilterParser::Parse("Name (*.stl;*.STL)");
			Assert::AreEqual(1, (int)m_list.size());
			Assert::AreEqual("Name ", m_list[0].Name().toStdString().data());
			AssertFileFilterInfoEquals({ "*.stl", "*.STL" }, m_list[0].Filters());
		}

		TEST_METHOD(Integration)
		{
			m_list = FileFilterParser::Parse("STL File(*.stl;*.STL);;VTK File(*.vtk;*.VTK);;OBJ File(*.obj;*.OBJ);;3MF File(*.3mf;*.3MF)");
			Assert::AreEqual(4, (int)m_list.size());
			Assert::AreEqual("STL File", m_list[0].Name().toStdString().data());
			Assert::AreEqual("VTK File", m_list[1].Name().toStdString().data());
			Assert::AreEqual("OBJ File", m_list[2].Name().toStdString().data());
			Assert::AreEqual("3MF File", m_list[3].Name().toStdString().data());
			AssertFileFilterInfoEquals({ "*.stl", "*.STL" }, m_list[0].Filters());
			AssertFileFilterInfoEquals({ "*.vtk", "*.VTK" }, m_list[1].Filters());
			AssertFileFilterInfoEquals({ "*.obj", "*.OBJ" }, m_list[2].Filters());
			AssertFileFilterInfoEquals({ "*.3mf", "*.3MF" }, m_list[3].Filters());
		}

	private:
		void AssertFileFilterInfoEquals(QStringList expected, QStringList actual)
		{
			Assert::AreEqual(expected.size(), actual.size());
			for (int i = 0; i < expected.size(); ++i)
			{
				Assert::AreEqual(expected[i].toStdString(), actual[i].toStdString());
			}
		}
	};
}