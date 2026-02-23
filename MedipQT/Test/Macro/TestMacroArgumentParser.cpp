#include "stdafx.h"
#include "Test/test_pch.h"
#include "Macro/MacroArgumentParser.h"

class TestMacroArgumentParser : public ::testing::Test
{
public:
	void SetUp() override
	{
	}
	void TearDown() override
	{
	}

protected:
};

TEST_F(TestMacroArgumentParser, TestArgumentNotHasMacroOptionReturnFalse)
{
	MacroParameter parameter;
	QStringList args = { "MEDIP.exe", "not-macro", "option" };
	EXPECT_FALSE(MacroArgumentParser::Parse(&parameter, args));
}

TEST_F(TestMacroArgumentParser, TestMacroWithoutNameReturnFalse)
{
	MacroParameter parameter;
	QStringList args = { "MEDIP.exe", "-macro" };
	EXPECT_FALSE(MacroArgumentParser::Parse(&parameter, args));
}

TEST_F(TestMacroArgumentParser, TestMacroDumi)
{
	MacroParameter parameter;
	QStringList args = { "MEDIP.exe", "-macro", MACRO_DUMI_TEST };
	EXPECT_TRUE(MacroArgumentParser::Parse(&parameter, args));
	EXPECT_STREQ(MACRO_DUMI_TEST, parameter.MacroName.toStdString().c_str());
}

TEST_F(TestMacroArgumentParser, TestOmniverseCreateUsd_WithoutParameter_ReturnFalse)
{
	MacroParameter parameter;
	QStringList args = { "MEDIP.exe", "-macro", MACRO_OMNIVERSE_UPLOAD_USD };
	EXPECT_FALSE(MacroArgumentParser::Parse(&parameter, args));
}

TEST_F(TestMacroArgumentParser, TestOmniverseUploadUsd)
{
	MacroParameter parameter;
	QStringList args = { "MEDIP.exe", "-macro", MACRO_OMNIVERSE_UPLOAD_USD, "localhost", "C:/Upload/File.usd", "destination" };
	EXPECT_TRUE(MacroArgumentParser::Parse(&parameter, args));
	EXPECT_STREQ("localhost", parameter.OmniverseUploadUsd.IPAddress.toStdString().c_str());
	EXPECT_STREQ("destination", parameter.OmniverseUploadUsd.ServerDirectoryPath.toStdString().c_str());
	EXPECT_STREQ("C:/Upload/File.usd", parameter.OmniverseUploadUsd.LocalFilePath.toStdString().c_str());
}

TEST_F(TestMacroArgumentParser, TestOmniverseCreateUsdModel)
{
	MacroParameter parameter;
	QStringList args = { "MEDIP.exe", "-macro", MACRO_OMNIVERSE_CREATE_USD_MODEL, "localhost", "Projects/destination/test.usd" };
	EXPECT_TRUE(MacroArgumentParser::Parse(&parameter, args));
	EXPECT_STREQ("localhost", parameter.OmniverseCreateUsdModelUsd.IPAddress.toStdString().c_str());
	EXPECT_STREQ("Projects/destination/test.usd", parameter.OmniverseCreateUsdModelUsd.UsdFilePath.toStdString().c_str());
}


