#include "pch.h"
#include "filemanager/appcore/AppCoreContext.h"

using namespace fm;

class TestAppCoreContext : public ::testing::Test
{
public:
	TestAppCoreContext() {}

	void SetUp() override
	{
		m_appName = "appcore_test_name";
		m_pFilePathManager = m_context.GetFilePathManager();
		m_pConfigureManager = m_context.GetConfigManager();
		m_unitTestDirecotry = GetUnitTestDataDirectory("appcore/TestAppCoreContext");
	}

	void TearDown() override
	{
		FilePathManager filePathManager;
		filePathManager.InitializeWithoutCreateDirectory(m_appName);

		QDir(QString::fromStdWString(filePathManager.AppRootDirPath())).removeRecursively();
	}

	bool IsFileExist(std::wstring path)
	{
		return QFile::exists(QString::fromStdWString(path));
	}

	ProductFunctionType GetDefaultProductFunctionType()
	{
		ProductFunctionType type;
		type.ProductType = EProductType::MEDIP;
		type.CanPACSUpload = true;
		type.CanPACSDownload = false;
		return type;
	}

	void SetupWithPACSRepository(std::wstring repositoryPath)
	{
		AppCoreContext appContext;
		appContext.Initialize(GetDefaultProductFunctionType(), m_appName);

		PACSConfig* pPACSConfig = appContext.GetConfigManager()->GetPACSConfig();
		pPACSConfig->ModifyRepositoryDirectoryPath(repositoryPath);
	}

	AppCoreContext m_context;
	FilePathManager* m_pFilePathManager;
	ConfigManager* m_pConfigureManager;

	QString m_unitTestDirecotry;
	QString m_appName;
};

TEST_F(TestAppCoreContext, WhenIntialized_DefaultFileAndDirectoryExist)
{
	AppCoreContext context;
	context.Initialize(GetDefaultProductFunctionType(), m_appName);

	fm::FilePathManager* pFilePathManager = context.GetFilePathManager();

	EXPECT_TRUE(IsFileExist(pFilePathManager->AppRootDirPath()));
	EXPECT_TRUE(IsFileExist(pFilePathManager->FileManagerRootDirPath()));
	EXPECT_TRUE(IsFileExist(pFilePathManager->DataDirPath()));
	EXPECT_TRUE(IsFileExist(pFilePathManager->LogDirPath()));
	EXPECT_TRUE(IsFileExist(pFilePathManager->ConfigDirPath()));
	EXPECT_TRUE(IsFileExist(pFilePathManager->TempDirPath()));
	EXPECT_TRUE(IsFileExist(pFilePathManager->DefaultRepositoryDirPath()));

	QFile::exists(m_unitTestDirecotry);
}

TEST_F(TestAppCoreContext, TestFileAndDirectoryShouldBeClear)
{
	FilePathManager filePathManager;
	filePathManager.InitializeWithoutCreateDirectory(m_appName);

	EXPECT_FALSE(IsFileExist(filePathManager.AppRootDirPath()));
	EXPECT_FALSE(IsFileExist(filePathManager.FileManagerRootDirPath()));
	EXPECT_FALSE(IsFileExist(filePathManager.DataDirPath()));
	EXPECT_FALSE(IsFileExist(filePathManager.LogDirPath()));
	EXPECT_FALSE(IsFileExist(filePathManager.ConfigDirPath()));
	EXPECT_FALSE(IsFileExist(filePathManager.TempDirPath()));
	EXPECT_FALSE(IsFileExist(filePathManager.DefaultRepositoryDirPath()));
}

TEST_F(TestAppCoreContext, WhenFirstInitialized_PACSConfigRepositoryPath_HasDefaultPath)
{
	m_context.Initialize(GetDefaultProductFunctionType(), m_appName);

	PACSConfig* pPACSConfig = m_pConfigureManager->GetPACSConfig();
	std::wstring repositoryDirPath = pPACSConfig->GetRepositoryDirectoryPath();

	EXPECT_STREQ(m_pFilePathManager->DefaultRepositoryDirPath().c_str(), repositoryDirPath.c_str());
}

TEST_F(TestAppCoreContext, WhenPACSConfigRepositoryEmpty_DoNotChangeValue)
{
	std::wstring prevRepositoryDirPath = m_unitTestDirecotry.toStdWString();
	SetupWithPACSRepository(prevRepositoryDirPath);

	m_context.Initialize(GetDefaultProductFunctionType(), m_appName);
	PACSConfig* pPACSConfig = m_pConfigureManager->GetPACSConfig();
	std::wstring repositoryDirPath = pPACSConfig->GetRepositoryDirectoryPath();

	EXPECT_STREQ(prevRepositoryDirPath.c_str(), repositoryDirPath.c_str());
}
