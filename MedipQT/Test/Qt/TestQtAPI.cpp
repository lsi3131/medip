#include "stdafx.h"
#include "Test/test_pch.h"
#include <QTemporaryFile>

class TestQtAPI : public ::testing::Test
{
public:
	void SetUp() override
	{
		QString testDir = GetUnitTestDataDirectory("Qt").c_str();
		m_tempTextFilePath = testDir + "/temp.txt";
		m_tempJsonFilePath = testDir + "/temp.json";
	}
	void TearDown() override
	{
	}

	void WriteToFile(const QString& filepath, const QByteArray& data)
	{
		QFile fileWrite(filepath);
		fileWrite.open(QIODevice::WriteOnly);
		fileWrite.write(data);
		fileWrite.close();
	}

	QByteArray ReadFromFile(const QString& filepath)
	{
		QFile file(filepath);
		file.open(QIODevice::ReadOnly);
		QByteArray readData = file.readAll();
		file.close();

		return readData;
	}

protected:
	QString m_tempTextFilePath;
	QString m_tempJsonFilePath;
};

TEST_F(TestQtAPI, Test_WriteToTempFile_And_ReadRealFile)
{
	//WriteToFile(m_tempTextFilePath, "abc");
	//QByteArray readData = ReadFromFile(m_tempTextFilePath);

	//QTemporaryFile tempFile;
	//QString newTempFileName;
	//newTempFileName = tempFile.fileName();
	//EXPECT_STREQ("", newTempFileName.toStdString().c_str());
	//EXPECT_TRUE(tempFile.open());
	//newTempFileName = tempFile.fileName();
	//newTempFileName += ".usd";

	//EXPECT_TRUE(tempFile.rename(newTempFileName));
	//tempFile.close();

	//EXPECT_TRUE(tempFile.open());

	//int tempWriteLength = tempFile.write(readData);
	//EXPECT_EQ(3, tempWriteLength);
	//tempFile.close();

	//QFile readFile(newTempFileName);
	//EXPECT_TRUE(readFile.open(QIODevice::ReadOnly));

	//QByteArray readTempData = readFile.readAll();
	//readFile.close();

	//EXPECT_STREQ("abc", readTempData.toStdString().c_str());
}

TEST_F(TestQtAPI, Test_SaveJson)
{
	QFile file(m_tempJsonFilePath);
	file.open(QIODevice::WriteOnly);

	QJsonObject rootJsonObject{
		{"root" ,""}
	};

	QJsonObject keyValue
	{
		{"key1", "value1"},
		{"key2", "value2"},
	};

	rootJsonObject.insert("root", keyValue);

	QJsonDocument document;
	document.setObject(rootJsonObject);

	file.write(document.toJson());
	file.close();
}
