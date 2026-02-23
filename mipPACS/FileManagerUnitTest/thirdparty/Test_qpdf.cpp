#include "pch.h"
#include "filemanager/appcore/System/TextSaveQtLogger.h"
#include <qpdfwidget.h>

class Test_qpdf : public ::testing::Test
{
public:
	Test_qpdf() {}

	void SetUp() override
	{
	}

	void TearDown() override
	{
	}
};

TEST_F(Test_qpdf, TestLogStatus)
{
	TextSaveQtLogger::Initialize();

	QPdfWidget* pdfWidget = new QPdfWidget();
	pdfWidget->showMinimized();

	EXPECT_STREQ("", TextSaveQtLogger::GetLogWithLineFeed().toStdString().c_str());
}
