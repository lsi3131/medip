#include "pch.h"
#include "filemanager/config/FolderViewConfig.h"

using namespace fm;

class TestFolderViewConfig : public ::testing::Test
{
public:
	TestFolderViewConfig() {}

	void SetUp() override
	{
		m_unitTestDirectory = GetUnitTestDataDirectory("config/TestFolderViewConfig/");
		m_notExistFilePath = m_unitTestDirectory + "not_exist_file";
		m_invalidFormatFilePath = m_unitTestDirectory + "/invalid_format_file";
		m_validFormatFilePath = m_unitTestDirectory + "/valid_format_file";
		m_tempFilePath = m_unitTestDirectory + "/temp_file";
		m_redmine1308IssueFilePath = m_unitTestDirectory + "/redmine1308/folder_view_config.xml";

		m_defaultFolderViewConfigData =
			"<?xml version='1.0' encoding='utf-8'?>\n"
			"<FolderViewConfig>\n"
			" <FavoritesFolderList/>\n"
			"</FolderViewConfig>\n";
	}

	void TearDown() override
	{
		QFile::remove(m_notExistFilePath);
		QFile::remove(m_invalidFormatFilePath);
		QFile::remove(m_validFormatFilePath);
		QFile::remove(m_tempFilePath);
	}

	QByteArray ReadData(QString configFilePath)
	{
		QFile file(configFilePath);
		file.open(QIODevice::ReadOnly);
		QByteArray data = file.readAll();
		file.close();
		return data;
	}

	void WriteData(QString configFilePath, const QByteArray& data)
	{
		QFile file(configFilePath);
		file.open(QFile::WriteOnly);
		file.write(data);
		file.close();
	}

public:
	QString m_unitTestDirectory;
	QString m_notExistFilePath;
	QString m_invalidFormatFilePath;
	QString m_validFormatFilePath;
	QString m_tempFilePath;

	QString m_redmine1308IssueFilePath;
	std::string m_defaultFolderViewConfigData;
};

TEST_F(TestFolderViewConfig, TestUnitTestFileExist)
{
	//EXPECT_TRUE(QFile::exists(m_unitTestDirectory));
	//EXPECT_TRUE(QFile::exists(m_redmine1308IssueFilePath));
}

TEST_F(TestFolderViewConfig, TestFileShouldBeClear)
{
	EXPECT_FALSE(QFile::exists(m_notExistFilePath));
	EXPECT_FALSE(QFile::exists(m_invalidFormatFilePath));
	EXPECT_FALSE(QFile::exists(m_validFormatFilePath));
	EXPECT_FALSE(QFile::exists(m_tempFilePath));
}

TEST_F(TestFolderViewConfig, EmptyFilePath_ReturnFalse)
{
	FolderViewConfig config;
	bool result = config.LoadXml(L"");

	EXPECT_FALSE(result);
}

TEST_F(TestFolderViewConfig, WhenFileNotExist_CreateNewFile)
{
	FolderViewConfig config;
	bool result = config.LoadXml(m_notExistFilePath.toStdWString());

	EXPECT_TRUE(result);
	EXPECT_TRUE(QFile::exists(m_notExistFilePath));
	EXPECT_STREQ(m_defaultFolderViewConfigData.c_str(), ReadData(m_notExistFilePath).constData());
}

TEST_F(TestFolderViewConfig, WhenLoadValidConfigFile_ReturnTrueAndDataExist)
{
	QString record = "<FavoritesFolder Name=\"Test\" DirectoryPath=\"C:\\Sample\" Order=\"0\"/>";
	QString validData = QString(
		"<?xml version='1.0' encoding='utf-8'?>\n"
		"<FolderViewConfig>\n"
		" <FavoritesFolderList>\n"
		" %1\n"
		" </FavoritesFolderList>\n"
		"</FolderViewConfig>\n").arg(record);

	WriteData(m_validFormatFilePath, validData.toLocal8Bit());

	FolderViewConfig config;
	bool result = config.LoadXml(m_validFormatFilePath.toStdWString());
	EXPECT_TRUE(result);
	EXPECT_TRUE(QFile::exists(m_validFormatFilePath));
	std::vector<FavoritesFolder> favoritesFolderList = config.GetFavoritesFolderList();
	EXPECT_EQ(1, favoritesFolderList.size());
	EXPECT_STREQ(L"Test", favoritesFolderList[0].Name.toStdWString().c_str());
	EXPECT_STREQ(L"C:\\Sample", favoritesFolderList[0].DirectoryPath.toStdWString().c_str());
	EXPECT_EQ(0, favoritesFolderList[0].Order);
}

//http://www.medicalip.net:30002/issues/1308 ÀÌ½´ Å×½ºÆ®
TEST_F(TestFolderViewConfig, TestRedmine_1308)
{
	FolderViewConfig config;
	QFile::copy(m_redmine1308IssueFilePath, m_tempFilePath);
	bool result = config.LoadXml(m_tempFilePath.toStdWString());
	EXPECT_TRUE(result);
	EXPECT_TRUE(QFile::exists(m_tempFilePath));
	EXPECT_STREQ(m_defaultFolderViewConfigData.c_str(), ReadData(m_tempFilePath).constData());
}

