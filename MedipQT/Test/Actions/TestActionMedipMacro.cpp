#include "stdafx.h"
#include "Test/test_pch.h"
#include "Test/TryCodeWrapper.h"
#include "ActionManager.h"
#include "Macro/MacroDefines.h"

class TestActionMedipMacro : public ::testing::Test
{
public:
	TestActionMedipMacro() {}

	void SetUp() override
	{
		ACTION_MANAGER->m_IsMacroMode = false;
	}

	void TearDown() override
	{
	}

public:
};

// 현재 block
#if 0

/* MEDIP MACRO 지원일 경우에만 Test 가능 */
#ifdef USE_MEDIP_MACRO
TEST_F(TestActionMedipMacro, TestTryCode)
{
	QStringList args;
	args << "main.exe";
	//TryCodeWrapper::Do(args);
}

TEST_F(TestActionMedipMacro, TestInvalidMacro)
{
	QStringList args;
	args << "main.exe";
	args << "-macro";
	args << "invalid_macro";
	int result = TryCodeWrapper::Do(args, false);

	EXPECT_EQ(-1, result);
}

TEST_F(TestActionMedipMacro, TestValidMacro)
{
	QStringList args;
	args << "main.exe";
	args << "-macro";
	args << MACRO_START_AND_CLOSE;
	int result = TryCodeWrapper::Do(args, false);

	EXPECT_EQ(0, result);
}

TEST_F(TestActionMedipMacro, TestMacroExportRawNII_WithBadArgument_ReturnFail)
{
	QStringList args;
	args << "main.exe";
	args << "-macro";
	args << MACRO_EXPORT_MASK_RAW;
	int result = TryCodeWrapper::Do(args, false);

	EXPECT_EQ(-1, result);
}

TEST_F(TestActionMedipMacro, TestMacroExportRawNII_InvalidFilePath_ReturnFail)
{
	QString testDirPath = QDir::currentPath() + "/../TestData/unittest/macro/export_raw_nii";
	QString sampleMipPath = testDirPath + "/invalid_file_name.mip";
	QStringList args;
	args << "main.exe";
	args << "-macro";
	args << MACRO_EXPORT_MASK_RAW;
	args << sampleMipPath;
	args << testDirPath;
	int result = TryCodeWrapper::Do(args, false);

	EXPECT_EQ(-1, result);
}

TEST_F(TestActionMedipMacro, TestMacroExportRaw_Save_SingleLayerMask)
{
	QString testDirPath = QDir::currentPath() + "/../TestData/unittest/macro/export_raw_nii";
	QString sampleMipPath = testDirPath + "/sample_layer_1.mip";
	QStringList args;
	QFile::remove(testDirPath + "/Layer 1.raw");

	args << "main.exe";
	args << "-macro";
	args << MACRO_EXPORT_MASK_RAW;
	args << sampleMipPath;
	args << testDirPath;
	int result = TryCodeWrapper::Do(args, false);

	EXPECT_EQ(0, result);
	EXPECT_TRUE(QFile::exists(testDirPath + "/Layer 1.raw"));
}

TEST_F(TestActionMedipMacro, TestMacroExportRaw_Save_MultiLayerMask)
{
	QString testDirPath = QDir::currentPath() + "/../TestData/unittest/macro/export_raw_nii";
	QString sampleMipPath = testDirPath + "/sample_layer_8.mip";
	QStringList args;
	for (int i = 1; i <= 8; ++i)
	{
		QString filepath = QString(testDirPath + "/Layer %1.raw").arg(i);
		QFile::remove(filepath);
	}

	args << "main.exe";
	args << "-macro";
	args << MACRO_EXPORT_MASK_RAW;
	args << sampleMipPath;
	args << testDirPath;
	int result = TryCodeWrapper::Do(args, false);

	EXPECT_EQ(0, result);
	for (int i = 1; i <= 8; ++i)
	{
		QString filepath = QString(testDirPath + "/Layer %1.raw").arg(i);
		EXPECT_TRUE(QFile::exists(filepath));
	}
}

TEST_F(TestActionMedipMacro, TestMacroExportMaskLayer_NII_Save_SingleLayerMask)
{
	QString testDirPath = QDir::currentPath() + "/../TestData/unittest/macro/export_raw_nii";
	QString sampleMipPath = testDirPath + "/sample_layer_1.mip";
	QFile::remove(testDirPath + "/Layer 1.nii");

	QStringList args;
	args << "main.exe";
	args << "-macro";
	args << MACRO_EXPORT_MASK_NII;
	args << sampleMipPath;
	args << testDirPath;
	int result = TryCodeWrapper::Do(args, false);

	EXPECT_EQ(0, result);
	EXPECT_TRUE(QFile::exists(testDirPath + "/Layer 1.nii"));
}

TEST_F(TestActionMedipMacro, TestMacroExportMaskLayer_NII_Save_MultiLayerMask)
{
	QString testDirPath = QDir::currentPath() + "/../TestData/unittest/macro/export_raw_nii";
	QString sampleMipPath = testDirPath + "/sample_layer_8.mip";

	for (int i = 1; i <= 8; ++i)
	{
		QString filepath = QString(testDirPath + "/Layer %1.nii").arg(i);
		QFile::remove(filepath);
	}

	QStringList args;
	args << "main.exe";
	args << "-macro";
	args << MACRO_EXPORT_MASK_NII;
	args << sampleMipPath;
	args << testDirPath;
	int result = TryCodeWrapper::Do(args, false);

	EXPECT_EQ(0, result);
	for (int i = 1; i <= 8; ++i)
	{
		QString filepath = QString(testDirPath + "/Layer %1.nii").arg(i);
		EXPECT_TRUE(QFile::exists(filepath));
	}
}
#endif
#endif
