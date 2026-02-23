#pragma once

#include <QDialog>
#include <QIcon>
#include <DicomReader.h>
#include "filemanager/export.h"
#include "filemanager/FileManagerExportData.h"
#include "filemanager/FileManagerImportData.h"

class QTextDocument;
class QHBoxLayout;
class QGridLayout;

namespace fm
{
	class AppCoreContext;
	class MainWidget;
	class MainImportWidget;
	class DicomModelFileLoader;
	class DicomModelRoot;

	class FileManagerDialogPrivate;

	class FM_CORE_EXPORT FileManagerDialog : public QDialog
	{
		Q_OBJECT
	public:
		enum EMode
		{
			import_file_mode,
			export_file_mode,
			import_default_mode,
			import_dicom_mode,
		};

	public:
		friend FileManagerDialogPrivate;

	public:
		static void OnDicomLoadCanceled(void* pContext);

	public:
		FileManagerDialog() = delete;
		FileManagerDialog(AppCoreContext* pContext, QIcon icon = QIcon(), QWidget* parent = Q_NULLPTR);
		virtual ~FileManagerDialog();

	public:
		QString GetImportOpenFilePath(QString filters, QString filename = "", QString dirpath = "");
		QStringList GetImportOpenFilePathList(QString filters, QString filename = "", QString dirpath = "");

		QString GetExportSaveFilePath(QString filters, QString filename = "", QString dirpath = "");
		QString GetExportDirectoryPath(QString dirpath = "");
		QString GetExportPDF(QTextDocument* pPDFDocument, QString filters, QString filename = "", QString dirpath = "");

		bool GetImportDefaultData(FileManagerImportData* pOutImportData, QString mipFileFilters, QString importFileFilters, QString defaultSelectFilter);
		bool GetImportDicomData(FileManagerImportData* pOutImportData);

		void ExportToPACS(FileManagerExportData* pExportData);

		void Debug();

	protected:
		void closeEvent(QCloseEvent* e) override;

	private:
		void InitLayout();
		void ResetMainWidget();

	private slots:
		void onMipFileSelected(QString filepath);
		void onDicomFileListSelected(QStringList filepathList);
		void onImportFileListSelected(QStringList filepathList);
		void onExportFileListSelected(QStringList filepathList);
		void onDicomPatientListFilePathSelected(QStringList filepathList);
		void onCloseApp();

		void onDcmFileLoadedStarted();
		void onDcmFileLoaded_InProgress(int progressCount, int maxCount);
		void onDcmFileLoadedFinished();
		void onDcmFileLoadedAborted();

		void onFinished(int result);

	private:
		std::shared_ptr<FileManagerDialogPrivate> m_p;
	};
}