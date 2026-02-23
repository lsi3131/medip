#include "stdafx.h"
#include "PACSDicomConvertUploadWidget.h"
#include "FileManager/dicom/dicom_defines.h"
#include "FileManager/dicom/DicomDataset.h"
#include "FileManager/dicom/Convert/DicomConverter.h"
#include "FileManager/appcore/appcore_defines.h"
#include "FileManager/appcore/AppCoreContext.h"
#include "FileManager/appcore/Dialog/PACS/PACSDicomConvertUploadDialog.h"
#include <QtPrintSupport/qprinter>
#include <qmessagebox>

namespace fm
{
	PACSDicomConvertUploadWidget::PACSDicomConvertUploadWidget(AppCoreContext* pContext, QWidget* parent) :
		m_pContext(pContext),
		m_pPreviewDocument(nullptr),
		m_mode(upload_local_PDF_file_mode),
		QWidget(parent)
	{
		setupUi(this);

		connect(m_btnRegister, &QPushButton::clicked, this, &PACSDicomConvertUploadWidget::onRegister);
		connect(m_chkBirthDate, &QPushButton::clicked, this, &PACSDicomConvertUploadWidget::onCheckBirthDate);

		m_tempPDFFilePath = QString::fromStdWString(
			m_pContext->GetFilePathManager()->TempDirPath() +
			L"/temp_pdf.pdf");

		m_tempBmpFilePath = QString::fromStdWString(
			m_pContext->GetFilePathManager()->TempDirPath() +
			L"/temp_bmp.bmp");

		m_tempDcmFilePath = QString::fromStdWString(
			m_pContext->GetFilePathManager()->TempDirPath() +
			L"/temp_dcm.dcm");

		onCheckBirthDate();
		UpdatePreviewStatusByMode();
	}

	PACSDicomConvertUploadWidget::~PACSDicomConvertUploadWidget()
	{
	}

	void PACSDicomConvertUploadWidget::InitFileDiskMode(QString filepath)
	{
		Clear();
		m_mode = upload_local_PDF_file_mode;

		m_localUploadPDFFilePath = filepath;
		UpdatePreviewStatusByMode();
	}

	void PACSDicomConvertUploadWidget::InitPreviewPDFMode(QTextDocument* pDocument)
	{
		Clear();
		m_mode = upload_preview_pdf_mode;

		m_pPreviewDocument = pDocument;
		m_editPDFPreview->setDocument(m_pPreviewDocument->clone());
		UpdatePreviewStatusByMode();
	}

	void PACSDicomConvertUploadWidget::InitPreviewImageMode(QImage& image)
	{
		Clear();
		m_mode = upload_preview_image_mode;
		m_previewImage = image;
		m_labelImagePreview->setScaledContents(true);
		m_labelImagePreview->setPixmap(QPixmap::fromImage(image));

		UpdatePreviewStatusByMode();
	}

	void PACSDicomConvertUploadWidget::onCheckBirthDate()
	{
		m_dateEditBirthDate->setEnabled(m_chkBirthDate->isChecked());
	}

	void PACSDicomConvertUploadWidget::Clear()
	{
		m_editPatientName->setText("Default");
		m_editPatientID->setText("");
		m_editStudyDescription->setPlainText("");
		m_dateEditBirthDate->setDateTime(QDateTime::currentDateTime());
		m_dateTimeStudy->setDateTime(QDateTime::currentDateTime());
	}

	void PACSDicomConvertUploadWidget::SavePreviewPDFDoc_To_TempPDFFile(QString filepath)
	{
		QPrinter printer(QPrinter::HighResolution);
		printer.setOutputFormat(QPrinter::PdfFormat);
		printer.setPaperSize(QPrinter::A4);
		printer.setOutputFileName(filepath);
		printer.setPageMargins(QMarginsF(15, 15, 15, 15));

		m_pPreviewDocument->print(&printer);
	}

	void PACSDicomConvertUploadWidget::UpdatePreviewStatusByMode()
	{
		switch (m_mode)
		{
		case EMode::upload_local_PDF_file_mode:
			m_groupPreview->setVisible(false);
			break;
		case EMode::upload_preview_pdf_mode:
			m_groupPreview->setVisible(true);
			m_previewStackWidget->setCurrentIndex(0);
			break;
		case EMode::upload_preview_image_mode:
			m_groupPreview->setVisible(true);
			m_previewStackWidget->setCurrentIndex(1);
			break;
		}
	}

	bool PACSDicomConvertUploadWidget::CheckCanRegister()
	{
		if (m_editPatientName->text().isEmpty())
		{
			QMessageBox::warning(
				this,
				StringManager::GetString(STR_TITLE_WARNING),
				StringManager::GetString(STR_WARN_REQUEST_PATIENT_NAME),
				QMessageBox::StandardButton::Ok);
			return false;
		}

		QMessageBox::StandardButton button = QMessageBox::question(
			this,
			StringManager::GetString(STR_TITLE_QUESTION),
			StringManager::GetString(STR_QUESTION_UPLOAD_PACS),
			QMessageBox::Ok | QMessageBox::Cancel
		);

		if (button == QMessageBox::Cancel)
		{
			return false;
		}

		return true;
	}

	void PACSDicomConvertUploadWidget::SetupAppendedDatasetFromControl()
	{
		QString patientName = m_editPatientName->text();
		QString patientID = m_editPatientName->text();
		QString studyDescription = m_editStudyDescription->toPlainText();
		QDateTime datetime = m_dateTimeStudy->dateTime();
		QString dateText = datetime.toString("yyyyMMdd");
		QString timeText = datetime.toString("hhmmss");

		m_appendedDicomDataset.SetTagValue(fm::DicomTagID::PatientName, patientName.toStdWString());
		m_appendedDicomDataset.SetTagValue(fm::DicomTagID::PatientID, patientID.toStdWString());
		m_appendedDicomDataset.SetTagValue(fm::DicomTagID::StudyDescription, studyDescription.toStdWString());
		m_appendedDicomDataset.SetTagValue(fm::DicomTagID::StudyDate, dateText.toStdWString());
		m_appendedDicomDataset.SetTagValue(fm::DicomTagID::StudyTime, timeText.toStdWString());
		m_appendedDicomDataset.SetTagValue(fm::DicomTagID::SeriesDate, dateText.toStdWString());
		m_appendedDicomDataset.SetTagValue(fm::DicomTagID::SeriesTime, timeText.toStdWString());

		if (m_chkBirthDate->isChecked())
		{
			QDate birthDate = m_dateEditBirthDate->date();
			QString birthDateText = birthDate.toString("yyyyMMdd");
			m_appendedDicomDataset.SetTagValue(fm::DicomTagID::PatientBirthDate, birthDateText.toStdWString());
		}
	}

	bool PACSDicomConvertUploadWidget::ConvertDataToTempDcmFile()
	{
		bool result = false;
		if (m_mode == upload_local_PDF_file_mode)
		{
			result = ConvertPDFFileToDcm(m_localUploadPDFFilePath);
		}
		else if (m_mode == upload_preview_pdf_mode)
		{
			result = ConvertPreviewPDFDocToDcm();
		}
		else if (m_mode == upload_preview_image_mode)
		{
			result = ConvertPreviewImageToDcm();
		}
		return result;
	}

	bool PACSDicomConvertUploadWidget::ConvertPDFFileToDcm(QString pdfFilePath)
	{
		if (DicomConverter::ConvertPDFFile_To_DcmFile(
			pdfFilePath.toStdWString(),
			m_tempDcmFilePath.toStdWString(),
			&m_appendedDicomDataset) == false)
		{
			QMessageBox::warning(
				this,
				StringManager::GetString(STR_TITLE_WARNING),
				StringManager::GetString(STR_ERR_CONVERT_PDF_TO_DCM),
				QMessageBox::StandardButton::Ok);
			return false;
		}

		return true;
	}

	bool PACSDicomConvertUploadWidget::ConvertPreviewPDFDocToDcm()
	{
		if (m_pPreviewDocument == nullptr)
		{
			QMessageBox::warning(
				this,
				StringManager::GetString(STR_TITLE_WARNING),
				StringManager::GetString(STR_ERR_PDF_DOCUMENT_EMPTY),
				QMessageBox::StandardButton::Ok);
			return false;
		}

		SavePreviewPDFDoc_To_TempPDFFile(m_tempPDFFilePath);
		return ConvertPDFFileToDcm(m_tempPDFFilePath);
	}

	bool PACSDicomConvertUploadWidget::ConvertPreviewImageToDcm()
	{
		//1. BMP 파일 형태로 저장한다.
		if (m_previewImage.save(m_tempBmpFilePath) == false)
		{
			QMessageBox::warning(
				this,
				StringManager::GetString(STR_TITLE_WARNING),
				StringManager::GetString(STR_ERR_CONVERT_IMAGE_TO_DCM),
				QMessageBox::StandardButton::Ok);
			return false;
		}

		//2. BMP->DCM 변환 
		if (DicomConverter::ConvertBmpFile_To_DcmFile(
			m_tempBmpFilePath.toStdWString(),
			m_tempDcmFilePath.toStdWString(),
			&m_appendedDicomDataset) == false)
		{
			QMessageBox::warning(
				this,
				StringManager::GetString(STR_TITLE_WARNING),
				StringManager::GetString(STR_ERR_CONVERT_IMAGE_TO_DCM),
				QMessageBox::StandardButton::Ok);	
			return false;
		}

		return true;
	}

	void PACSDicomConvertUploadWidget::onRegister()
	{
		if (CheckCanRegister() == false)
		{
			return;
		}
		SetupAppendedDatasetFromControl();
		if (ConvertDataToTempDcmFile())
		{
			std::vector<PACSConfig::ExportHost> hostList = m_pContext->GetConfigManager()->GetPACSConfig()->GetExportHosts();
			if (!hostList.empty())
			{
				DicomHostInfo hostInfo = DicomHostInfo::CreateStore(
					hostList[0].ApplicationEntity,
					hostList[0].IP,
					hostList[0].TransferSyntax,
					hostList[0].Port,
					hostList[0].MaxAssoc,
					30
				);

				m_pContext->GetDicomNetworkManager()->SetHostInfoStore(hostInfo);
				m_pContext->GetDicomNetworkManager()->Upload_Async({ m_tempDcmFilePath.toStdString() });
			}
		}
	}
}

