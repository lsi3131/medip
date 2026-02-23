#include "stdafx.h"
#include "TestDicomLoaderDialog.h"
#include <qfiledialog.h>
#include <QElapsedTimer>

using namespace fm;

TestDicomLoaderDialog::TestDicomLoaderDialog(QWidget* parent) :
	QDialog(parent)
{
	setupUi(this);

	connect(m_btnLoad, &QPushButton::clicked, this, &TestDicomLoaderDialog::onLoad);
}

void TestDicomLoaderDialog::onLoad()
{
	QString sampleDataDir = QApplication::applicationDirPath() + "/SampleData/dcm/image";

	QString dirpath = QFileDialog::getExistingDirectory(
		this,
		"Load",
		sampleDataDir
	);

	if (!dirpath.isEmpty())
	{
		//m_dcmImageViewer->LoadFromFile_Async(dirpath);
	}
}
