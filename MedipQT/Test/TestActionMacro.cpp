#include "stdafx.h"
#include "test_pch.h"
#include "Actions/ActionManager.h"
#include "Windows/windowManager.h"
#include "ProductManager.h"
#include "MedipQT.h"

class TestActionMacro : public ::testing::Test
{
public:
	TestActionMacro() {}

	void SetUp() override
	{
		int argc = 1;
		const char* argv[] = { "dumi" };
		m_app = new QApplication(argc, (char**)argv);

		m_sampleDcmDirectoryPath = "";
		m_testMacroFilePath = QDir::currentPath() + "/sample_macro_file";
		//sampleMacroData = QString(";Path,%1\\*.* ;Type,abdomen ;UNET,2D ;Contrast,Con ;Gender,F ;Height,165 ;Weight,60 ;Slice,L3,A").
		//	arg(sampleDcmDirectoryPath);

		testMacroData = QString(";Path,%1\\*.* ").
			arg(m_sampleDcmDirectoryPath);

		m_row = -1;
		m_col = -1;
	}

	void TearDown() override
	{
		delete m_app;
	}

	QString CreateMacroData(QString dcmFilePath)
	{
		return QString(";Path,%1\\*.* ").arg(dcmFilePath);
	}

	void CreateMacroFile(QString macroFilePath, QString dcmFilePath)
	{
		QString macroData = CreateMacroData(m_sampleDcmDirectoryPath);
		QFile file(macroFilePath);
		file.open(QIODevice::WriteOnly);
		file.write(testMacroData.toUtf8());
		file.close();
	}

public:
	QString m_sampleDcmDirectoryPath;
	QString m_testMacroFilePath;
	QString testMacroData;
	int m_row = -1;
	int m_col = -1;
	QApplication* m_app;
};
