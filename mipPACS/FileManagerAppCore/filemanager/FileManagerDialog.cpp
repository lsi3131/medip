#include "stdafx.h"
#include "FileManagerDialog.h"
#include "filemanager/appcore/AppCoreContext.h"
#include "filemanager/appcore/System/ShortcutManager.h"
#include "filemanager/appcore/MainWidget.h"
#include "filemanager/appcore/Action/ActionManager.h"
#include "filemanager/appcore/Dialog/Util/ProgressBarDialog.h"
#include "filemanager/appcore/Dialog/DicomImportDataDialog.h"
#include "filemanager/appcore/Resource/ResourceManager.h"
#include "filemanager/dicom/DicomInfomationModelLoader.h"
#include <qmenubar>
#include <QCloseEvent>
#include <qmessagebox>
#include <qlayout>

namespace fm
{
	void FileManagerDialog::OnDicomLoadCanceled(void* pContext)
	{
		FileManagerDialog* pDlg = (FileManagerDialog*)pContext;
		pDlg->m_pDcmModelLoader_Image->Abort_IOProgress();
	}

	FileManagerDialog::FileManagerDialog(AppCoreContext* pContext, QIcon icon, QWidget* parent)
		: QDialog(parent, Qt::WindowCloseButtonHint),
		m_pContext(pContext),
		m_pMainWidget(nullptr),
		m_icon(icon),
		m_pDcmInfomationModel(nullptr)
	{
		/* Progress Bar Dialog 초기화*/
		ProgressBarDialog::Global_Initialize(this);
		//ProgressBarDialog::SetMode(ProgressBarDialog::busy_indicator);
		ProgressBarDialog::Global_SetMode(ProgressBarDialog::normal);

		setObjectName("FileManagerDialog");
		setWindowTitle("FileManager");

		setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);

		m_pDcmModelLoader_Image = new DicomInfomationModelLoader();
		QString styleSheet = g_ResourceManager.GetMainStyleSheet(m_pContext->GetProductFunctionType().ProductType);
		setStyleSheet(styleSheet);

		m_mainLayout = new QGridLayout(this);
		m_mainLayout->setSpacing(0);
		m_mainLayout->setContentsMargins(0, 0, 0, 0);
		setLayout(m_mainLayout);

		setWindowIcon(m_icon);

		connect(m_pDcmModelLoader_Image, &DicomInfomationModelLoader::dcmFileLoadedStarted, this, &FileManagerDialog::onDcmFileLoadedStarted, Qt::ConnectionType::QueuedConnection);
		connect(m_pDcmModelLoader_Image, &DicomInfomationModelLoader::dcmFileLoadedInProgress, this, &FileManagerDialog::onDcmFileLoaded_InProgress, Qt::ConnectionType::QueuedConnection);
		connect(m_pDcmModelLoader_Image, &DicomInfomationModelLoader::dcmFileLoadedFinished, this, &FileManagerDialog::onDcmFileLoadedFinished, Qt::ConnectionType::QueuedConnection);
		connect(m_pDcmModelLoader_Image, &DicomInfomationModelLoader::dcmFileLoadedAborted, this, &FileManagerDialog::onDcmFileLoadedAborted);

		connect(this, &QDialog::finished, this, &FileManagerDialog::onFinished);

		connect(ProgressBarDialog::Instance(), &ProgressBarDialog::canceled, this, &FileManagerDialog::onCloseApp);
	}

	FileManagerDialog::~FileManagerDialog()
	{
	}

	QString FileManagerDialog::GetImportOpenFilePath(QString filters, QString filename, QString dirpath)
	{
		ResetMainWidget();
		m_pMainWidget->SetImportFileMode(filters, filename, dirpath);

		if (exec() == QDialog::Accepted)
		{
			QStringList filepathList = m_ImportData.GetImportFilePathList();
			return filepathList.isEmpty() ? "" : filepathList[0];
		}
		else
		{
			return "";
		}
	}

	QStringList FileManagerDialog::GetImportOpenFilePathList(QString filters, QString filename, QString dirpath)
	{
		ResetMainWidget();
		m_pMainWidget->SetImportFileListMode(filters, filename, dirpath);

		if (exec() == QDialog::Accepted)
		{
			return m_ImportData.GetImportFilePathList();
		}
		else
		{
			return QStringList();
		}
	}

	QString FileManagerDialog::GetExportSaveFilePath(QString filters, QString filename, QString dirpath)
	{
		ResetMainWidget();
		m_pMainWidget->SetExportFileMode(filters, filename, dirpath);

		if (exec() == QDialog::Accepted)
		{
			QStringList filepathList = m_ExportData.GetExportFilePathList();
			return filepathList.isEmpty() ? "" : filepathList[0];
		}
		else
		{
			return "";
		}
	}

	QString FileManagerDialog::GetExportDirectoryPath(QString dirpath)
	{
		ResetMainWidget();
		m_pMainWidget->SetExportDirectoryMode(dirpath);

		if (exec() == QDialog::Accepted)
		{
			QStringList filepathList = m_ExportData.GetExportFilePathList();
			return filepathList.isEmpty() ? "" : filepathList[0];
		}
		else
		{
			return "";
		}
	}

	QString FileManagerDialog::GetExportPDF(QTextDocument* pPDFDocument, QString filters, QString filename, QString dirpath)
	{
		ResetMainWidget();
		//m_pMainWidget->SetExport_PDF_Mode(pPDFDocument, FileViewMode(false, false, FileViewMode::save, FileViewMode::directory, filters));
		m_pMainWidget->SetExportFileMode(filters, filename, dirpath);

		if (exec() == QDialog::Accepted)
		{
			QStringList filepathList = m_ExportData.GetExportFilePathList();
			return filepathList.isEmpty() ? "" : filepathList[0];
		}
		else
		{
			return "";
		}
	}

	bool FileManagerDialog::GetImportDefaultData(FileManagerImportData* pOutImportData, QString mipFileFilters, QString importFileFilters, QString defaultSelectFilter)
	{
		ResetMainWidget();

		//m_pMainWidget->SetImport(FileViewMode(true, true, FileViewMode::open, FileViewMode::file_and_directory, "", "", "DICOM(*.dcm);;All(*)"));
		m_pMainWidget->SetImportDefaultMode(mipFileFilters, importFileFilters, defaultSelectFilter);

		if (exec() == QDialog::Accepted)
		{
			*pOutImportData = m_ImportData;
			return true;
		}
		else
		{
			return false;
		}
	}

	bool FileManagerDialog::GetImportDicomData(FileManagerImportData* pOutImportData)
	{
		ResetMainWidget();

		//m_pMainWidget->SetImport(FileViewMode(true, true, FileViewMode::open, FileViewMode::file_and_directory, "", "", "DICOM(*.dcm);;All(*)"));
		m_pMainWidget->SetImportDicomMode();

		if (exec() == QDialog::Accepted)
		{
			*pOutImportData = m_ImportData;
			return true;
		}
		else
		{
			return false;
		}
	}

	void FileManagerDialog::ExportToPACS(FileManagerExportData* pExportData)
	{
		if (m_pContext->GetProductFunctionType().CanPACSUpload == false)
		{
			qWarning() << "Can't load PACS mode";
		}
		else
		{
			ResetMainWidget();
			m_pMainWidget->SetExport_PACS_Mode(
				*pExportData->GetExportDicomDataset(),
				*pExportData->GetDicomExportData()
			);

			exec();
		}
	}

	void FileManagerDialog::Debug()
	{
		ResetMainWidget();
		m_pMainWidget->SetDebugMode();

		exec();
	}

	void FileManagerDialog::closeEvent(QCloseEvent* e)
	{
		m_pMainWidget->SaveStatus();
		QDialog::closeEvent(e);
	}

	void FileManagerDialog::InitLayout()
	{
		GUISettingConfig* pConfig = m_pContext->GetConfigManager()->GetGUISettingConfig();
		restoreGeometry(pConfig->GetMainWidgetInfo().Geometry);
	}

	void FileManagerDialog::ResetMainWidget()
	{
		if (m_pMainWidget)
		{
			delete m_pMainWidget;
			m_pMainWidget = nullptr;
		}
		m_pMainWidget = new MainWidget(m_pContext);
		m_mainLayout->addWidget(m_pMainWidget);

		InitLayout();

		connect(m_pMainWidget, &MainWidget::mipFileSelected, this, &FileManagerDialog::onMipFileSelected);
		connect(m_pMainWidget, &MainWidget::dicomFileListSelected, this, &FileManagerDialog::onDicomFileListSelected);
		connect(m_pMainWidget, &MainWidget::importFileListSelected, this, &FileManagerDialog::onImportFileListSelected);
		connect(m_pMainWidget, &MainWidget::exportFileListSelected, this, &FileManagerDialog::onExportFileListSelected);
		connect(m_pMainWidget, &MainWidget::dicomPatientListFilePathSelected, this, &FileManagerDialog::onDicomPatientListFilePathSelected);

		connect(m_pMainWidget, &MainWidget::closed, this, &FileManagerDialog::onCloseApp);
	}

	void FileManagerDialog::onMipFileSelected(QString filepath)
	{
		m_ImportData.SetImportDataType(EFileManagerImportDataType::mip_project);
		m_ImportData.SetMIPProjectFilePath(filepath);

		accept();
	}

	void FileManagerDialog::onDicomFileListSelected(QStringList filepathList)
	{
		/* 
			파일의 Directory Path의 리스트를 구한다. 
			중복 제거를 위해 Set을 사용한다. 
		*/
		QSet<QString> dirpathSet;
		for (QString& filepath : filepathList)
		{
			QFileInfo fileInfo(filepath);
			if (fileInfo.isFile())
			{
				QString dirpath = fileInfo.absoluteDir().absolutePath();
				dirpathSet << dirpath;
			}
			else if (fileInfo.isDir())
			{
				dirpathSet << fileInfo.absoluteFilePath();
			}
		}
		/* 파일 경로가 아닌 Directory경로로 읽어온 후 처리한다. */
		QStringList dirpathList = QStringList::fromSet(dirpathSet);
		m_ImportData.SetImportDataType(EFileManagerImportDataType::dicom_data);
		
		/* onDcmFileLoadedFinished() 함수에서 결과 처리 */
		m_pDcmModelLoader_Image->LoadOnlyImage_Async(m_pDcmInfomationModel, dirpathList);
	}

	void FileManagerDialog::onImportFileListSelected(QStringList filepathList)
	{
		m_ImportData.SetImportDataType(EFileManagerImportDataType::import_file_list);
		m_ImportData.SetImportFilePathList(filepathList);
		accept();
	}

	void FileManagerDialog::onExportFileListSelected(QStringList filepathList)
	{
		m_ExportData.SetExportFilePathList(filepathList);
		accept();
	}

	void FileManagerDialog::onDicomPatientListFilePathSelected(QStringList filepathList)
	{
		/* 지정된 경로에서만 파일 load */
		m_ImportData.SetImportDataType(EFileManagerImportDataType::dicom_data);

		m_pDcmModelLoader_Image->Load_Async(m_pDcmInfomationModel, filepathList);
	}

	void FileManagerDialog::onCloseApp()
	{
		reject();
	}

	void FileManagerDialog::onDcmFileLoadedStarted()
	{
		ProgressBarDialog::Global_SetProgress(0);
		ProgressBarDialog::Global_SetMode(ProgressBarDialog::EMode::busy_indicator);
		ProgressBarDialog::Global_SetText("DICOM file load started");
		ProgressBarDialog::Global_Show(FileManagerDialog::OnDicomLoadCanceled, this);
		m_pMainWidget->setEnabled(false);
	}

	void FileManagerDialog::onDcmFileLoaded_InProgress(int progressCount, int maxCount)
	{
		float rate = (float)progressCount / (float)maxCount;
		ProgressBarDialog::Global_SetText(QString("DICOM file load in progress(%1/%2)").arg(progressCount).arg(maxCount));
		ProgressBarDialog::Global_SetMode(ProgressBarDialog::EMode::normal);
		ProgressBarDialog::Global_SetProgress(rate);
	}

	void FileManagerDialog::onDcmFileLoadedFinished()
	{
		ProgressBarDialog::Global_SetText("DICOM file load finished");
		ProgressBarDialog::Global_Hide();
		m_pMainWidget->setEnabled(true);

		GUISettingConfig* pGUIConfig = m_pContext->GetConfigManager()->GetGUISettingConfig();
		DicomImportDataDialog dlg(&m_ImportData, pGUIConfig, m_pDcmInfomationModel, this);

		/* Import Data 데이터 업데이트. */
		int result = dlg.exec();

		/* DicomInfomationModel 데이터를 초기화하여 용량을 확보한다. */
		m_pDcmInfomationModel->Clear();

		if (result == QDialog::Accepted)
		{
			accept();
		}
	}

	void FileManagerDialog::onDcmFileLoadedAborted()
	{
		ProgressBarDialog::Global_SetText("DICOM file load aborted");
		ProgressBarDialog::Global_Hide();
		m_pMainWidget->setEnabled(true);
	}

	void FileManagerDialog::onFinished(int result)
	{
		/* 
		   FileManagerDialog 상태를 종료한다.
			-GUI 상태 저장 
		*/
		GUISettingConfig* pGUIConfig = m_pContext->GetConfigManager()->GetGUISettingConfig();
		MainWidgetInfo info = pGUIConfig->GetMainWidgetInfo();
		info.Geometry = this->saveGeometry();
		pGUIConfig->SetMainWidgetInfo(info);
		pGUIConfig->Save();

		if (m_pMainWidget)
		{
			m_pMainWidget->SaveStatus();
		}
	}
}
