#include "pch.h"
#include "TextSaveQtLogger.h"
#include <QDebug>

class TestQtLogger : public ::testing::Test
{
public:
	TestQtLogger() {}

	void SetUp() override
	{
	}

	void TearDown() override
	{
	}
};

TEST_F(TestQtLogger, WithoutInitialize_LogNotSaved)
{
	qDebug() << "hello";
	EXPECT_STREQ("", TextSaveQtLogger::GetLogWithLineFeed().toStdString().c_str());
}

TEST_F(TestQtLogger, WithInitialize_LogSaved)
{
	TextSaveQtLogger::Initialize();

	qDebug() << "abc";
	EXPECT_STREQ("abc\n", TextSaveQtLogger::GetLogWithLineFeed().toStdString().c_str());
}

TEST_F(TestQtLogger, WhenInitializeAgain_LogReset)
{
	TextSaveQtLogger::Initialize();

	qDebug() << "cba";
	EXPECT_STREQ("cba\n", TextSaveQtLogger::GetLogWithLineFeed().toStdString().c_str());
}

TEST_F(TestQtLogger, TestMultiLog)
{
	TextSaveQtLogger::Initialize();

	qDebug() << "123";
	qInfo() << "456";
	qWarning() << "789";
	qCritical() << "abc";
	EXPECT_STREQ("123\n456\n789\nabc\n", TextSaveQtLogger::GetLogWithLineFeed().toStdString().c_str());
}

