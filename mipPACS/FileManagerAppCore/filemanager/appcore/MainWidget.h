#pragma once

#include <qwidget>
#include <QRadioButton>
#include <QStackedWidget>
#include <qfilesystemmodel>
#include "fileManager/data/Entity/ImportedDicomInfoDTO.h"
#include "fileManager/appcore/appcore_defines.h"
#include "filemanager/dicom/Network/DicomNetworkSCUStatus.h"
#include "filemanager/export.h"
#include "filemanager/appcore/UI/FileViewMode.h"

class QTextDocument;

namespace Ui
{
	class MainWidget;
}

namespace fm
{
	class AppCoreContext;
	class FolderViewWidget;
	class PatientListWidget;
	class PACSUploadWidget;
	class PACSDownloadWidget;
	class PACSExportWidget;
	class DicomExportData;

	class FM_CORE_EXPORT MainWidget : public QWidget
	{
		Q_OBJECT

	public:
		enum ETabMode
		{
			import_default_mode,
			import_only_file_mode,
			import_dicom_mode,
			save_file_mode,
			save_directory_mode,
			export_PDF_mode,
			upload_PACS_mode,
			search_PACS_mode,
			debug,
		};

		enum EPageIndex
		{
			page_folder = 0,
			page_patient_list,
			page_pacs_search_download,
			page_pacs_export,
		};

		enum ENavButtonType
		{
			nav_button_mip_project = 0,
			nav_button_dicom,
			nav_button_import,
			nav_button_save,
			nav_button_patient_list,
			nav_button_pacs_download,
			nav_button_pacs_upload,
			nav_button_pacs_search,
		};

	public:
		MainWidget(AppCoreContext* pContext, QWidget* parent = Q_NULLPTR);
		virtual ~MainWidget();

	public:
		void SetImportDefaultMode(QString mipFileFilters, QString importFileFilters, QString defaultSelectFilter);
		void SetImportDicomMode();
		void SetImportFileMode(QString importFileFilters, QString filename, QString dirpath);
		void SetImportFileListMode(QString importFileFilters, QString filename, QString dirpath);

		void SetExportFileMode(QString exportFileFilters, QString filename, QString dirpath);
		void SetExportDirectoryMode(QString dirpath);
		void SetExport_PDF_Mode(QTextDocument* pPDFDocument, const FileViewMode& mode);
		void SetExport_PACS_Mode(DicomDataset& exportInfo, DicomExportData& exportData);

		void SetDebugMode();

		void UpdateControlByNavRadioButton();
		void UpdateControl();
		void UpdateVisiblityOfControl();
		void SetNavButtonType(ENavButtonType navButtonType);
		ENavButtonType GetNavButtonType();
		EPageIndex CurrentPageIndex();

		void SaveStatus();

		bool IsMIPProjectVisible() const;
		bool IsDICOMVisible() const;
		bool IsImportFileVisible() const;
		bool IsPatientListVisible() const;
		bool IsPACSDownloadVisible() const;

		bool IsFileSaveVisible() const;
		bool IsPACSUploadVisible() const;
		bool IsPACSSearchVisible() const;

	private:
		void HandleDicomDownloadResult(DcmNetDownloadData& dcmDownloadData);
		void CopyDcm_And_ImpotToPatientList(QString dcmFilePath);
		void ConvertDcmToPDF_And_OpenDirectoryPath(QString dirpath);
		void ConvertDcmToImage_And_OpenDirectoryPath(QString dirpath);

	protected:
		virtual bool event(QEvent* e) override;
		virtual void closeEvent(QCloseEvent* e) override;

	Q_SIGNALS:
		void mipFileSelected(QString filepath);
		void dicomFileListSelected(QStringList filepath);
		void importFileListSelected(QStringList filepath);
		void exportFileListSelected(QStringList filepath);
		void dicomPatientListFilePathSelected(QStringList filepath);
		void closed();

	private slots:
		void onRdoTabClicked();
		void onSettingBtnClicked();

		void onPatientListFileCopyStarted();
		void onPatientListFileCopyInProgressed(int progressCount, int maxCount, QString copiedFilePath);
		void onPatientListFileCopyFinished(QString copiedDirectoryPath);

		void onDcmNetFindStarted();
		void onDcmNetFindInProgress(DicomNetworkSCUStatus status);
		void onDcmNetFindFinished(DcmNetFindData result);

		void onDcmNetDonwloadStarted();
		void onDcmNetDownloadInProgress(DicomNetworkSCUStatus status);
		void onDcmNetDonwloadFinished(DcmNetDownloadData result);

		void onDcmNetUploadStarted();
		void onDcmNetUploadInProgress(DicomNetworkSCUStatus status);
		void onDcmNetUploadFinished();

		void onDcmNetError(QString errorMessage);

		void onDicomListAdded(std::vector<ImportedDicomInfoDTO> dcmList);
		void onDicomListModifed(std::vector<ImportedDicomInfoDTO> dcmList);
		void onFileSelected(QString filepath);
		void onFileListSelected(QStringList filepathList);
		void onCloseApp();

	protected:
		Ui::MainWidget* m_ui;
		ETabMode m_tabMode;
		FileViewMode m_mipProjectFileViewMode;
		FileViewMode m_dicomFileViewMode;
		FileViewMode m_importFileListViewMode;
		FileViewMode m_exportFileListViewMode;
		AppCoreContext* m_pContext;
	};
}

