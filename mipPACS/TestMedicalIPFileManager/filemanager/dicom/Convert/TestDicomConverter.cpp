#include "stdafx.h"
#include "CppUnitTest.h"
#include "../DicomTestStub.h"
#include "filemanager/dicom/Convert/DicomConverter.h"
#include "filemanager/dicom/DicomDataset.h"
#include <qdir>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <qimage>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestMedicalIPFileManager
{
	TEST_CLASS(TestDicomConverter)
	{
	private:
		DicomTestStub m_testStub;
		QDir m_dir;
		QFile m_file;

	public:
		TEST_METHOD_INITIALIZE(SetUp)
		{
		}

		TEST_METHOD_CLEANUP(TearDown)
		{
		}

		TEST_METHOD(TestSampleSourceFileExist)
		{
			Assert::IsTrue(m_dir.exists(m_testStub.Source_PDF_To_Dcm_PDFFilePath));
			Assert::IsTrue(m_dir.exists(m_testStub.Source_Dcm_To_PDF_DcmFilePath));
			Assert::IsTrue(m_dir.exists(m_testStub.Source_BMP_To_DCM_BmpFilePath));
			Assert::IsTrue(m_dir.exists(m_testStub.Source_DCM_To_Img_DcmFilePath));
			Assert::IsTrue(m_dir.exists(m_testStub.Expected_PDF_To_Dcm_DcmFilePath));
		}

		TEST_METHOD(ConvertInvalidPDFFileToDcm)
		{
			QString PDFSampleFilepath = m_testStub.PDFDirPath + "/invalid.pdf";
			QString DicomResultFilepath = m_testStub.PDFDirPath + "/invalid.dcm";
			Assert::IsFalse(fm::DicomConverter::ConvertPDFFile_To_DcmFile(
				PDFSampleFilepath.toStdWString(), 
				DicomResultFilepath.toStdWString()));
		}

		TEST_METHOD(ConvertPDF_To_DcmDataset)
		{
			fm::DicomDataset dataset;
			Assert::IsTrue(fm::DicomConverter::ConvertPDFFile_To_DcmDataset(
				m_testStub.Source_PDF_To_Dcm_PDFFilePath.toStdWString(),
				&dataset));

			std::wstring getValueModality;
			std::wstring getValueEncapsulatedDocument;
			std::wstring getValueEncapsulatedDocumentType;
			std::wstring getValueConversionType;
			Assert::IsTrue(dataset.TryGetValueText(fm::DicomTagID::Modality, &getValueModality));
			Assert::IsTrue(dataset.TryGetValueText(fm::DicomTagID::EncapsulatedDocument, &getValueEncapsulatedDocument));
			Assert::IsTrue(dataset.TryGetValueText(fm::DicomTagID::MIMETypeOfEncapsulatedDocument, &getValueEncapsulatedDocumentType));
			Assert::IsTrue(dataset.TryGetValueText(fm::DicomTagID::ConversionType, &getValueConversionType));

			Assert::AreEqual(L"DOC", getValueModality.data());
			Assert::AreEqual(L"application/pdf", getValueEncapsulatedDocumentType.data());
			Assert::AreEqual(L"WSD", getValueConversionType.data());
		}

		TEST_METHOD(ConvertPDF_To_DcmDatasetWithAppendDataset)
		{
			fm::DicomDataset dataset;
			fm::DicomDataset appendedDataset;
			appendedDataset.SetTagValue(fm::DicomTagID::PatientName, L"Name");
			appendedDataset.SetTagValue(fm::DicomTagID::PatientID, L"ID");
			appendedDataset.SetTagValue(fm::DicomTagID::PatientBirthDate, L"20080810");
			appendedDataset.SetTagValue(fm::DicomTagID::StudyDescription, L"Study");
			appendedDataset.SetTagValue(fm::DicomTagID::SeriesDescription, L"Series");
			Assert::IsTrue(fm::DicomConverter::ConvertPDFFile_To_DcmDataset(
				m_testStub.Source_PDF_To_Dcm_PDFFilePath.toStdWString(),
				&dataset,
				&appendedDataset));

			std::wstring getValueModality;
			std::wstring getValueEncapsulatedDocument;
			std::wstring getValueEncapsulatedDocumentType;
			std::wstring getValueConversionType;
			Assert::IsTrue(dataset.TryGetValueText(fm::DicomTagID::Modality, &getValueModality));
			Assert::IsTrue(dataset.TryGetValueText(fm::DicomTagID::EncapsulatedDocument, &getValueEncapsulatedDocument));
			Assert::IsTrue(dataset.TryGetValueText(fm::DicomTagID::MIMETypeOfEncapsulatedDocument, &getValueEncapsulatedDocumentType));
			Assert::IsTrue(dataset.TryGetValueText(fm::DicomTagID::ConversionType, &getValueConversionType));

			Assert::AreEqual(L"DOC", getValueModality.data());
			Assert::AreEqual(L"application/pdf", getValueEncapsulatedDocumentType.data());
			Assert::AreEqual(L"WSD", getValueConversionType.data());

			std::wstring getPatientName;
			std::wstring getPatientID;
			std::wstring getPatientBirthDate;
			std::wstring getStudyDescription;
			std::wstring getSeriesescription;
			Assert::IsTrue(dataset.TryGetValueText(fm::DicomTagID::PatientName, &getPatientName));
			Assert::IsTrue(dataset.TryGetValueText(fm::DicomTagID::PatientID, &getPatientID));
			Assert::IsTrue(dataset.TryGetValueText(fm::DicomTagID::PatientBirthDate, &getPatientBirthDate));
			Assert::IsTrue(dataset.TryGetValueText(fm::DicomTagID::StudyDescription, &getStudyDescription));
			Assert::IsTrue(dataset.TryGetValueText(fm::DicomTagID::SeriesDescription, &getSeriesescription));

			Assert::AreEqual(L"Name", getPatientName.data());
			Assert::AreEqual(L"ID", getPatientID.data());
			Assert::AreEqual(L"20080810", getPatientBirthDate.data());
			Assert::AreEqual(L"Study", getStudyDescription.data());
			Assert::AreEqual(L"Series", getSeriesescription.data());
		}

		TEST_METHOD(ConvertPDF_To_Dcm)
		{
			Assert::IsTrue(fm::DicomConverter::ConvertPDFFile_To_DcmFile(
				m_testStub.Source_PDF_To_Dcm_PDFFilePath.toStdWString(),
				m_testStub.Target_PDF_To_Dcm_DcmFilePath.toStdWString()));

			Assert::IsTrue(m_file.exists(m_testStub.Target_PDF_To_Dcm_DcmFilePath));

			fm::DicomDataset dataset;
			Assert::IsTrue(dataset.LoadFromFile(m_testStub.Target_PDF_To_Dcm_DcmFilePath));

			std::wstring getValueModality;
			std::wstring getValueEncapsulatedDocument;
			std::wstring getValueEncapsulatedDocumentType;
			std::wstring getValueConversionType;
			Assert::IsTrue(dataset.TryGetValueText(fm::DicomTagID::Modality, &getValueModality));
			Assert::IsTrue(dataset.TryGetValueText(fm::DicomTagID::EncapsulatedDocument, &getValueEncapsulatedDocument));
			Assert::IsTrue(dataset.TryGetValueText(fm::DicomTagID::MIMETypeOfEncapsulatedDocument, &getValueEncapsulatedDocumentType));
			Assert::IsTrue(dataset.TryGetValueText(fm::DicomTagID::ConversionType, &getValueConversionType));

			Assert::AreEqual(L"DOC", getValueModality.data());
			Assert::AreEqual(L"application/pdf", getValueEncapsulatedDocumentType.data());
			Assert::AreEqual(L"WSD", getValueConversionType.data());
		}

		TEST_METHOD(ConvertDcm_To_PDF)
		{
			Assert::IsTrue(fm::DicomConverter::ConvertDcmFile_To_PDFFile(
				m_testStub.Source_Dcm_To_PDF_DcmFilePath.toStdWString(),
				m_testStub.Target_Dcm_To_PDF_PDFFilePath.toStdWString()));

			Assert::IsTrue(m_file.exists(m_testStub.Target_Dcm_To_PDF_PDFFilePath));
		}

		TEST_METHOD(ConvertBmp_To_Dcm)
		{
			Assert::IsTrue(fm::DicomConverter::ConvertBmpFile_To_DcmFile(
				m_testStub.Source_BMP_To_DCM_BmpFilePath.toStdWString(),
				m_testStub.Target_BMP_To_DCM_DcmFilePath.toStdWString()
			));

			fm::DicomDataset dataset;
			Assert::IsTrue(dataset.LoadFromFile(m_testStub.Target_BMP_To_DCM_DcmFilePath));
		}

		TEST_METHOD(ConvertBmp_To_Dcm_WithAppendDataset)
		{
			fm::DicomDataset appendedDataset;
			appendedDataset.SetTagValue(fm::DicomTagID::PatientName, L"name");
			appendedDataset.SetTagValue(fm::DicomTagID::PatientID, L"id");
			appendedDataset.SetTagValue(fm::DicomTagID::PatientBirthDate, L"19911231");
			appendedDataset.SetTagValue(fm::DicomTagID::StudyDescription, L"study");
			appendedDataset.SetTagValue(fm::DicomTagID::SeriesDescription, L"series");

			Assert::IsTrue(fm::DicomConverter::ConvertBmpFile_To_DcmFile(
				m_testStub.Source_BMP_To_DCM_BmpFilePath.toStdWString(),
				m_testStub.Target_BMP_To_DCM_DcmFilePath.toStdWString(),
				&appendedDataset
			));

			fm::DicomDataset dataset;
			Assert::IsTrue(dataset.LoadFromFile(m_testStub.Target_BMP_To_DCM_DcmFilePath));

			std::wstring getPatientName;
			std::wstring getPatientID;
			std::wstring getPatientBirthDate;
			std::wstring getStudyDescription;
			std::wstring getSeriesescription;
			Assert::IsTrue(dataset.TryGetValueText(fm::DicomTagID::PatientName, &getPatientName));
			Assert::IsTrue(dataset.TryGetValueText(fm::DicomTagID::PatientID, &getPatientID));
			Assert::IsTrue(dataset.TryGetValueText(fm::DicomTagID::PatientBirthDate, &getPatientBirthDate));
			Assert::IsTrue(dataset.TryGetValueText(fm::DicomTagID::StudyDescription, &getStudyDescription));
			Assert::IsTrue(dataset.TryGetValueText(fm::DicomTagID::SeriesDescription, &getSeriesescription));

			Assert::AreEqual(L"name", getPatientName.data());
			Assert::AreEqual(L"id", getPatientID.data());
			Assert::AreEqual(L"19911231", getPatientBirthDate.data());
			Assert::AreEqual(L"study", getStudyDescription.data());
			Assert::AreEqual(L"series", getSeriesescription.data());
		}

		TEST_METHOD(ConvertDcm_To_Image)
		{
			Assert::IsTrue(fm::DicomConverter::ConvertDcmFile_To_ImgFile(
				m_testStub.Source_DCM_To_Img_DcmFilePath.toStdWString(),
				m_testStub.Target_DCM_To_Img_BmpFilePath.toStdWString()
			));

			Assert::IsTrue(fm::DicomConverter::ConvertDcmFile_To_ImgFile(
				m_testStub.Source_DCM_To_Img_DcmFilePath.toStdWString(),
				m_testStub.Target_DCM_To_Img_JpgFilePath.toStdWString()
			));

			Assert::IsTrue(fm::DicomConverter::ConvertDcmFile_To_ImgFile(
				m_testStub.Source_DCM_To_Img_DcmFilePath.toStdWString(),
				m_testStub.Target_DCM_To_Img_PngFilePath.toStdWString()
			));

			QImage imgBmp;
			QImage imgJpg;
			QImage imgPng;
			Assert::IsTrue(imgBmp.load(m_testStub.Target_DCM_To_Img_BmpFilePath));
			Assert::IsTrue(imgJpg.load(m_testStub.Target_DCM_To_Img_JpgFilePath));
			Assert::IsTrue(imgPng.load(m_testStub.Target_DCM_To_Img_PngFilePath));
		}

	private:
	};
}