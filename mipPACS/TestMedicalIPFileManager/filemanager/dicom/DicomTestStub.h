#pragma once

#include <qstringlist>

class DicomTestStub
{
public:
	DicomTestStub();
	~DicomTestStub();
public:
	QString TestDataDirPath;
	QString DownloadDirPath;
	QString DcmDirPath;
	QString ImageDirPath;
	QString PDFDirPath;
	QString BmpDirPath;
	QStringList ImageFilePathList_Sample_CCC;
	QStringList ImageFilePathList_Sample_BBB;
	QStringList ImageFilePathList_Sample_AAA;

	QString Source_PDF_To_Dcm_PDFFilePath;
	QString Target_PDF_To_Dcm_DcmFilePath;
	QString Expected_PDF_To_Dcm_DcmFilePath;

	QString Source_Dcm_To_PDF_DcmFilePath;
	QString Target_Dcm_To_PDF_PDFFilePath;

	QString Source_BMP_To_DCM_BmpFilePath;
	QString Target_BMP_To_DCM_DcmFilePath;

	QString Source_DCM_To_Img_DcmFilePath;
	QString Target_DCM_To_Img_BmpFilePath;
	QString Target_DCM_To_Img_JpgFilePath;
	QString Target_DCM_To_Img_PngFilePath;



private:
	void SetupFilePathList(QStringList* pFilePathList, QString dirname, int count);

};
