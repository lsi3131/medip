#include "stdafx.h"
#include "CppUnitTest.h"
#include "filemanager/appcore/SubWindow/FolderViewWidget.h"
#include "filemanager/appcore/AppCoreContext.h"
#include "filemanager/GUI_Initializer.h"
#include "MockFolderViewWidget.h"
#include "FolderViewTestStub.h"
#include <QtWidgets/QApplication>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestMedicalIPFileManager
{
	TEST_CLASS(TestFolderViewWidget)
	{
	private:
		fm::AppCoreContext* m_pContext;
		fm::FolderViewWidget* m_pWidget;
		GUI_Initializer* m_pGUIInit;
		FolderViewTestStub m_testStub;

	public:
		TEST_METHOD_INITIALIZE(SetUp)
		{
			m_pGUIInit = new GUI_Initializer();
			m_pContext = new fm::AppCoreContext();
			bool canPACSDownload = true;
			bool canPACSUpload = true;
			fm::EProductType type = fm::EProductType::DEEPCATCH;
			fm::ProductFunctionType productFunctionType(type, canPACSDownload, canPACSUpload);
			m_pContext->Initialize(productFunctionType);

			m_pWidget = new fm::FolderViewWidget();
		}

		TEST_METHOD_CLEANUP(TearDown)
		{
			delete m_pGUIInit;
			delete m_pContext;
			delete m_pWidget;
		}
		TEST_METHOD(TestFileOpenMode_InitializedData)
		{
			MockFolderViewWidget widget(m_pContext);
			fm::FileViewMode mode;
			widget.SetFileOpenMode(mode);
			widget.WaitUntilMainViewDirectoryLoaded();

			QStringList fileNameList = widget.GetMainViewFileNameList();
			Assert::AreEqual(m_testStub.FilePathList.size(), fileNameList.size());
			Assert::IsTrue(IsFileNameExist(m_testStub.Filepath_csv, fileNameList), L"file not exist");
			Assert::IsTrue(IsFileNameExist(m_testStub.Filepath_dcm, fileNameList), L"file not exist");
			Assert::IsTrue(IsFileNameExist(m_testStub.Filepath_json, fileNameList), L"file not exist");
			Assert::IsTrue(IsFileNameExist(m_testStub.Filepath_mip, fileNameList), L"file not exist");
			Assert::IsTrue(IsFileNameExist(m_testStub.Filepath_obj, fileNameList), L"file not exist");
			Assert::IsTrue(IsFileNameExist(m_testStub.Filepath_png, fileNameList), L"file not exist");
			Assert::IsTrue(IsFileNameExist(m_testStub.Filepath_stl, fileNameList), L"file not exist");
			Assert::IsTrue(IsFileNameExist(m_testStub.SampleFolder_1, fileNameList), L"file not exist");
			Assert::IsTrue(IsFileNameExist(m_testStub.SampleFolder_2, fileNameList), L"file not exist");
			Assert::IsTrue(IsFileNameExist(m_testStub.SampleFolder_3, fileNameList), L"file not exist");

			Assert::AreEqual("filename", widget.GetFileName().toStdString().data());
			Assert::AreEqual(
				QDir(m_testStub.TestRootDir).absolutePath().toStdString(),
				QDir(widget.GetDirectoryPath()).absolutePath().toStdString());
		}

		TEST_METHOD(WhenSelectFile_)
		{

		}

	private:
		bool IsFileNameExist(QString filePath, QStringList fileNameList)
		{
			for (QString fileName : fileNameList)
			{
				QFileInfo fileInfo(filePath);
				if (fileInfo.fileName() == fileName)
				{
					return true;
				}
			}
			return false;
		}

		

		//TEST_METHOD(TestSetup_And_GetFileNameDirectoryPath)
		//{
		//	m_pWidget->Setup("filename", m_testFolderViewDirpath, "*.*;;*.dcm;;*.txt", fm::EFileViewMode::file_save_mode);
		//	Assert::AreEqual("filename", m_pWidget->FileName().toLocal8Bit().data());
		//	Assert::AreEqual(
		//		m_testFolderViewDirpath.toStdString().data(),
		//		m_pWidget->DirectoryPath().toStdString().data());

		//	Assert::AreEqual(3, m_pWidget->GetFilterList().size());
		//	Assert::AreEqual("*.*", m_pWidget->GetCurrentFilter().toStdString().data());
		//}

		//TEST_METHOD(TestFileNameNoExtension_WithDefaultExtension_HasThatExtension)
		//{
		//	m_pWidget->Setup("filename", m_testFolderViewDirpath, "*.dcm", fm::EFileViewMode::file_save_mode);
		//	Assert::AreEqual("filename.dcm", m_pWidget->FileName().toLocal8Bit().data());
		//}

		//TEST_METHOD(TestFileNameSameExtension_WithDefaultExtension)
		//{
		//	m_pWidget->Setup("filename.dcm", m_testFolderViewDirpath, "*.dcm", fm::EFileViewMode::file_save_mode);
		//	Assert::AreEqual("filename.dcm", m_pWidget->FileName().toLocal8Bit().data());
		//}

		//TEST_METHOD(TestFilterListChanged)
		//{
		//	m_pWidget->Setup("filename", m_testFolderViewDirpath, "*.dcm;;*.txt", fm::EFileViewMode::file_save_mode);
		//	m_pWidget->SetCurrentFilter("*.txt");
		//	Assert::AreEqual("filename.txt", m_pWidget->FileName().toLocal8Bit().data());
		//}

		//TEST_METHOD(TestRemoveExtension)
		//{
		//	QString nameWithExtension = "a.b.c.ext";
		//	int indexOfExtension = nameWithExtension.lastIndexOf(".");
		//	Assert::AreEqual(5, indexOfExtension);
		//	QString nameWithoutExtension = nameWithExtension.mid(0, indexOfExtension);
		//	Assert::AreEqual("a.b.c", nameWithoutExtension.toStdString().data());
		//}


	private:
	};
}