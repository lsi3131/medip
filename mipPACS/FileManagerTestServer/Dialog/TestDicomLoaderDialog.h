#pragma once

#include <qdialog>
#include "ui_TestDicomLoaderDialog.h"
#include "filemanager/dicom/DicomDatasetIO.h"
#include "filemanager/dicom/DicomInfomationModel.h"

namespace fm
{
	class DicomDataset;
}

class TestDicomLoaderDialog : public QDialog, public Ui::TestDicomLoaderDialog
{
	Q_OBJECT

public:
	TestDicomLoaderDialog(QWidget* parent = nullptr);

private slots:
	void onLoad();

private:
};
