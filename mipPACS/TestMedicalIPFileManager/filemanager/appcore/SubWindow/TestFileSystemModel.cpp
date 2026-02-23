#include "stdafx.h"
#include "CppUnitTest.h"
#include "filemanager/GUI_Initializer.h"
#include "FolderViewTestStub.h"
#include <qfilesystemmodel>
#include <utility>
#include <qthread>
#include <QEventLoop>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestMedicalIPFileManager
{
	class MockFileSystemModel : public QFileSystemModel
	{
	public:
		MockFileSystemModel()
		{
			connect(this, &QFileSystemModel::rootPathChanged, this, &MockFileSystemModel::onRootPathChanged);
			connect(this, &QFileSystemModel::fileRenamed, this, &MockFileSystemModel::onFileRenamed);
			connect(this, &QFileSystemModel::directoryLoaded, this, &MockFileSystemModel::onDirectoryLoaded);
			connect(this, &QFileSystemModel::dataChanged, this, &MockFileSystemModel::onDataChanged);
		}
	public:
		QModelIndex SetDirectoryUntilLoaded(QString dirpath)
		{
			QModelIndex index = setRootPath(dirpath);

			QEventLoop loop;
			connect(this, &QFileSystemModel::directoryLoaded, &loop, &QEventLoop::quit);
			loop.exec();

			return index;
		}

	public slots:
		void onRootPathChanged(const QString& newPath)
		{
			RootPathChanged.push_back(newPath);
		}

		void onFileRenamed(const QString& path, const QString& oldName, const QString& newName)
		{
			FileRenamed.push_back(std::make_tuple(path, oldName, newName));
		}

		void onDirectoryLoaded(const QString& path)
		{
			DirectoryLoaded.push_back(path);
		}

		void onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
		{
			DataChanged.push_back(std::make_tuple(topLeft, bottomRight));
		}


	public:
		QVector<QString> RootPathChanged;
		QVector<std::tuple<QString, QString, QString> > FileRenamed;
		QVector<QString> DirectoryLoaded;
		QVector<std::tuple<QModelIndex, QModelIndex>> DataChanged;
	};


	TEST_CLASS(TestFileSystemModel)
	{
	private:
		MockFileSystemModel* m_pModel;
		GUI_Initializer* m_pGUIInit;
		FolderViewTestStub m_testStub;
	public:
		TEST_METHOD_INITIALIZE(SetUp)
		{
			m_pGUIInit = new GUI_Initializer();
			m_pModel = new MockFileSystemModel();
			m_pModel->setFilter(QDir::AllEntries | QDir::AllDirs | QDir::NoDot | QDir::NoDotDot);
		}

		TEST_METHOD_CLEANUP(TearDown)
		{
			delete m_pGUIInit;
		}

		TEST_METHOD(TestFileExist)
		{
			QDir dir;
			QFile file;
			Assert::IsTrue(dir.exists(m_testStub.TestRootDir));
			Assert::IsTrue(dir.exists(m_testStub.SampleFolder_1));
			Assert::IsTrue(dir.exists(m_testStub.SampleFolder_2));
			Assert::IsTrue(dir.exists(m_testStub.SampleFolder_3));
			Assert::IsTrue(file.exists(m_testStub.Filepath_dcm));
			Assert::IsTrue(file.exists(m_testStub.Filepath_csv));
			Assert::IsTrue(file.exists(m_testStub.Filepath_mip));
			Assert::IsTrue(file.exists(m_testStub.Filepath_json));
			Assert::IsTrue(file.exists(m_testStub.Filepath_obj));
			Assert::IsTrue(file.exists(m_testStub.Filepath_png));
			Assert::IsTrue(file.exists(m_testStub.Filepath_stl));
		}

		TEST_METHOD(FileSystemModel_NotLoaded_UntilDirectoryLoadedEmitted)
		{
			QModelIndex index = m_pModel->setRootPath(m_testStub.TestRootDir);

			Assert::AreNotEqual(m_testStub.FilePathList.size(), m_pModel->rowCount(index));
		}

		TEST_METHOD(FileSystemModel_Loaded_WhenDirectoryLoadedEmitted)
		{
			QModelIndex index = m_pModel->setRootPath(m_testStub.TestRootDir);

			QEventLoop loop;
			QObject::connect(m_pModel, &QFileSystemModel::directoryLoaded, &loop, &QEventLoop::quit);
			loop.exec();

			Assert::AreEqual(m_testStub.FilePathList.size(), m_pModel->rowCount(index));
			
		}


	private:
	};
}