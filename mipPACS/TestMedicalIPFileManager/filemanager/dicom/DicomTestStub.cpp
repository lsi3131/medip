#include "stdafx.h"
#include "DicomTestStub.h"
#include <qdir>

DicomTestStub::DicomTestStub()
{
	TestDataDirPath = QDir::currentPath() + "/TestData";
	DownloadDirPath = TestDataDirPath + "/Download";
	DcmDirPath = TestDataDirPath + "/dcm";
	ImageDirPath = DcmDirPath + "/image";
	PDFDirPath = DcmDirPath + "/pdf";
	BmpDirPath = DcmDirPath + "/bmp";

	Source_PDF_To_Dcm_PDFFilePath = PDFDirPath + QString::fromLocal8Bit("/테스트_source_pdf_to_dcm.pdf");
	Target_PDF_To_Dcm_DcmFilePath = PDFDirPath + "/target_pdf_to_dcm.dcm";
	Expected_PDF_To_Dcm_DcmFilePath = PDFDirPath + QString::fromLocal8Bit("/테스트_target_pdf_to_dcm_expected.dcm");

	Source_Dcm_To_PDF_DcmFilePath = PDFDirPath + QString::fromLocal8Bit("/테스트_source_dcm_to_pdf.dcm");
	Target_Dcm_To_PDF_PDFFilePath = PDFDirPath + "/target_pdf_to_dcm.pdf";

	Source_BMP_To_DCM_BmpFilePath = BmpDirPath + QString::fromLocal8Bit("/테스트_source_bmp_to_dcm.bmp");
	Target_BMP_To_DCM_DcmFilePath = BmpDirPath + "/target_bmp_to_dcm.dcm";

	Source_DCM_To_Img_DcmFilePath = BmpDirPath + QString::fromLocal8Bit("/테스트_source_dcm_to_img.dcm");
	Target_DCM_To_Img_BmpFilePath = BmpDirPath + "/target_dcm_to_img.bmp";
	Target_DCM_To_Img_JpgFilePath = BmpDirPath + "/target_dcm_to_img.jpg";
	Target_DCM_To_Img_PngFilePath = BmpDirPath + "/target_dcm_to_img.png";

	SetupFilePathList(&ImageFilePathList_Sample_CCC, "sample_1", 10);
	SetupFilePathList(&ImageFilePathList_Sample_BBB, "sample_2", 10);
	SetupFilePathList(&ImageFilePathList_Sample_AAA, "sample_3", 10);

	QDir dir;
	dir.mkpath(DownloadDirPath);
}

DicomTestStub::~DicomTestStub()
{
	QDir dir(DownloadDirPath);
	dir.removeRecursively();
}

void DicomTestStub::SetupFilePathList(QStringList* pFilePathList, QString dirname, int count)
{
	QString imageFilePath;
	for (int i = 0; i < count; ++i)
	{
		imageFilePath.sprintf("%s/%s/%d.dcm",
			ImageDirPath.toLocal8Bit().data(),
			dirname.toLocal8Bit().data(), i);
		*pFilePathList << imageFilePath;
	}
}
