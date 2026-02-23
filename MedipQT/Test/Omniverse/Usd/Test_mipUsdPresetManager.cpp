#include "stdafx.h"
#include "Test/test_pch.h"
#include "Omniverse/Usd/mipUsdPresetManager.h"

class Test_mipUsdPresetManager : public ::testing::Test
{
public:
	void SetUp() override
	{
		QString testDir = GetUnitTestDataDirectory("Omniverse").c_str();
		m_testDirectoryRoot = testDir + "/preset_manager_test";
		m_medipSampleFilePath_USD = testDir + "/usd/medip_sample.usd";

		QDir(m_testDirectoryRoot).removeRecursively();
		QDir().mkpath(m_testDirectoryRoot);
	}

	void TearDown() override
	{
	}

protected:
	QString m_testDirectoryRoot;
	QString m_medipSampleFilePath_USD;
};

TEST_F(Test_mipUsdPresetManager, WhenDirectoryNotExist_InitReturnFalse)
{
	mipUsdPresetManager usdPresetManager;
	EXPECT_FALSE(usdPresetManager.Init("C:/not/exist/directory/path"));
}

TEST_F(Test_mipUsdPresetManager, WhenDirectoryExist_ReturnTrue)
{
	mipUsdPresetManager usdPresetManager;
	EXPECT_TRUE(usdPresetManager.Init(m_testDirectoryRoot.toStdString()));
}

TEST_F(Test_mipUsdPresetManager, WhenPresetNotOpen_AddReturnFalse)
{
	mipUsdPresetManager usdPresetManager;
	usdPresetManager.Init(m_testDirectoryRoot.toStdString());

	mipUsdPresetPtr pPreset = std::make_shared<mipUsdPreset>();
	EXPECT_FALSE(usdPresetManager.Add(pPreset));
}

TEST_F(Test_mipUsdPresetManager, WhenPresetOpenButNotSaved_AddReturnFalse)
{
	mipUsdPresetManager usdPresetManager;
	usdPresetManager.Init(m_testDirectoryRoot.toStdString());

	mipUsdPresetPtr pPreset = std::make_shared<mipUsdPreset>();
	pPreset->OpenUsd(m_medipSampleFilePath_USD.toStdString());

	EXPECT_FALSE(usdPresetManager.Add(pPreset));
}

TEST_F(Test_mipUsdPresetManager, AddSingle)
{
	mipUsdPresetManager usdPresetManager;
	usdPresetManager.Init(m_testDirectoryRoot.toStdString());

	mipUsdPresetPtr pPreset = std::make_shared<mipUsdPreset>();
	pPreset->OpenUsd(m_medipSampleFilePath_USD.toStdString());
	pPreset->SaveToPresetFile(m_testDirectoryRoot.toStdString(), "preset");

	EXPECT_TRUE(usdPresetManager.Add(pPreset));
	EXPECT_EQ(1, usdPresetManager.GetPresetList().size());
}


