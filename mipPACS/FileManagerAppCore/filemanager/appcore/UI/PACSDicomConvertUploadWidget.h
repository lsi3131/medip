#pragma once

#include "FileManager/dicom/dicom_defines.h"
#include "FileManager/appcore/appcore_defines.h"
#include "FileManager/dicom/DicomDataset.h"
#include "ui_PACSDicomConvertUploadWidget.h"
#include <qwidget>

namespace fm
{
	class AppCoreContext;
	class DicomDataset;

	class PACSDicomConvertUploadWidget : public QWidget, public Ui::PACSDicomConvertUploadWidget
	{
		Q_OBJECT

	public:
		enum EMode
		{
			upload_local_PDF_file_mode,
			upload_preview_pdf_mode,
			upload_preview_image_mode,
		};
	public:
		PACSDicomConvertUploadWidget(AppCoreContext* pContext, QWidget* parent = nullptr);
		virtual ~PACSDicomConvertUploadWidget();

	public:
		void InitFileDiskMode(QString filepath);
		void InitPreviewPDFMode(QTextDocument* pDocument);
		void InitPreviewImageMode(QImage& image);

	private slots:
		void onRegister();
		void onCheckBirthDate();

	private:
		void Clear();
		void SavePreviewPDFDoc_To_TempPDFFile(QString filepath);
		void UpdatePreviewStatusByMode();

		bool CheckCanRegister();
		void SetupAppendedDatasetFromControl();
		bool ConvertDataToTempDcmFile();
		bool ConvertPDFFileToDcm(QString pdfFilePath);
		bool ConvertPreviewPDFDocToDcm();
		bool ConvertPreviewImageToDcm();

	private:
		AppCoreContext* m_pContext;
		EMode m_mode;

		QString m_localUploadPDFFilePath;

		const QTextDocument* m_pPreviewDocument;
		QImage m_previewImage;

		QString m_tempPDFFilePath;
		QString m_tempBmpFilePath;
		QString m_tempDcmFilePath;

		DicomDataset m_appendedDicomDataset;
	};
}
