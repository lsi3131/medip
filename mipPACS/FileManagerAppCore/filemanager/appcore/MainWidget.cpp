#include "stdafx.h"
#include "MainWidget.h"
#include "ui_MainWidget.h"
#include "AppCoreContext.h"
#include "SubWindow/FolderViewWidget.h"
#include "SubWindow/PatientListWidget.h"
#include "SubWindow/PACSExportWidget.h"
#include "Dialog/Util/ProgressBarDialog.h"
#include "FileManager/appcore/System/ShortcutManager.h"
#include "FileManager/appcore/Event/EventManager.h"
#include "FileManager/appcore/Util/FileSystemUtil.h"
#include "FileManager/dicom/Convert/DicomConverter.h"
#include "filemanager/appcore/Dialog/SettingDialog.h"
#include <qmenubar>
#include <QCloseEvent>
#include <qmessagebox>
#include <qfiledialog>
#include <qdesktopservices>

namespace fm
{
	MainWidget::MainWidget(AppCoreContext* pContext, QWidget* parent)
		: QWidget(parent),
		m_tabMode(import_only_file_mode),
		m_pContext(pContext)
	{
		setObjectName("MainWidget");
		m_ui = new Ui::MainWidget();
		m_ui->setupUi(this);

		m_mipProjectFileViewMode = FileViewMode(false, true, FileViewMode::open, FileViewMode::file, "MIP(*.mip;*.MIP)");
		m_dicomFileViewMode = FileViewMode(true, true, FileViewMode::open, FileViewMode::file_and_directory, "DICOM(*.dcm;*.DCM);;All(*)");
		m_importFileListViewMode = FileViewMode(true, true, FileViewMode::open, FileViewMode::file, "All(*)");
		m_exportFileListViewMode = FileViewMode(false, true, FileViewMode::save, FileViewMode::file, "All(*)");

		m_ui->m_rdoTab_MIPProject->setStyleSheet(g_ResourceManager.GetRadioNavTabProject());
		m_ui->m_rdoTab_DICOM->setStyleSheet(g_ResourceManager.GetRadioNavTabDicom());
		m_ui->m_rdoTab_Import->setStyleSheet(g_ResourceManager.GetRadioNavTabImport());
		m_ui->m_rdoTab_Save->setStyleSheet(g_ResourceManager.GetRadioNavTabSave());
		m_ui->m_rdoTab_PatientList->setStyleSheet(g_ResourceManager.GetRadioNavTabPatientList());
		//m_ui->m_rdoTab_PACSUpload->setStyleSheet(StyleSheetManager::GetRadioNavTabUpload());
		m_ui->m_rdoTab_PACSDownload->setStyleSheet(g_ResourceManager.GetRadioNavTabDownload());
		m_ui->m_rdoTab_PACSExport->setStyleSheet(g_ResourceManager.GetRadioNavTabUpload());
		m_ui->m_rdoTab_PACSSearch->setStyleSheet(g_ResourceManager.GetRadioNavTabSearch());

		m_ui->m_rdoTab_MIPProject->setText(StringManager::GetString(STR_GUI_NAV_BTN_MIP_PROJECT));
		m_ui->m_rdoTab_DICOM->setText(StringManager::GetString(STR_GUI_NAV_BTN_DICOM));
		m_ui->m_rdoTab_Import->setText(StringManager::GetString(STR_GUI_NAV_BTN_IMPORT));
		m_ui->m_rdoTab_Save->setText(StringManager::GetString(STR_GUI_NAV_BTN_SAVE));
		m_ui->m_rdoTab_PatientList->setText(StringManager::GetString(STR_GUI_NAV_BTN_PATIENT_LIST));
		m_ui->m_rdoTab_PACSDownload->setText(StringManager::GetString(STR_GUI_NAV_BTN_PACS_DOWNLOAD));
		m_ui->m_rdoTab_PACSExport->setText(StringManager::GetString(STR_GUI_BTN_PACS_UPLOAD));
		m_ui->m_rdoTab_PACSSearch->setText(StringManager::GetString(STR_GUI_BTN_PACS_SEARCH));
		m_ui->m_rdoTab_Setting->setText(StringManager::GetString(STR_GUI_BTN_SETTING));

		m_ui->m_folderViewWidget->Init(m_pContext);
		m_ui->m_patientListWidget->Init(m_pContext);
		m_ui->m_pacsSearchDownloadWidget->Init(m_pContext, EPACSOperationMode::can_download_mode, EPACSSearchMode::search_series);
		m_ui->m_pacsExportWidget->Init(m_pContext);

		m_ui->m_rdoButtonContainerWidget->layout();

		connect(m_ui->m_rdoTab_MIPProject, &QRadioButton::clicked, this, &MainWidget::onRdoTabClicked);
		connect(m_ui->m_rdoTab_DICOM, &QRadioButton::clicked, this, &MainWidget::onRdoTabClicked);
		connect(m_ui->m_rdoTab_Import, &QRadioButton::clicked, this, &MainWidget::onRdoTabClicked);
		connect(m_ui->m_rdoTab_PatientList, &QRadioButton::clicked, this, &MainWidget::onRdoTabClicked);
		//connect(m_ui->m_rdoTab_PACSUpload, &QRadioButton::clicked, this, &MainWidget::onRdoTabClicked);
		connect(m_ui->m_rdoTab_PACSDownload, &QRadioButton::clicked, this, &MainWidget::onRdoTabClicked);
		connect(m_ui->m_rdoTab_PACSExport, &QRadioButton::clicked, this, &MainWidget::onRdoTabClicked);
		connect(m_ui->m_rdoTab_PACSSearch, &QRadioButton::clicked, this, &MainWidget::onRdoTabClicked);

		connect(m_ui->m_rdoTab_Setting, &QRadioButton::clicked, this, &MainWidget::onSettingBtnClicked);

		connect(&g_EventManager, &EventManager::patientList_fileCopy_Started, this, &MainWidget::onPatientListFileCopyStarted);
		connect(&g_EventManager, &EventManager::patientList_fileCopy_InProgressed, this, &MainWidget::onPatientListFileCopyInProgressed);
		connect(&g_EventManager, &EventManager::patientList_fileCopy_Finished, this, &MainWidget::onPatientListFileCopyFinished);

		connect(&g_EventManager, &EventManager::dcmNet_Find_Started, this, &MainWidget::onDcmNetFindStarted);
		connect(&g_EventManager, &EventManager::dcmNet_Find_InProgress, this, &MainWidget::onDcmNetFindInProgress);
		connect(&g_EventManager, &EventManager::dcmNet_Find_Finished, this, &MainWidget::onDcmNetFindFinished);

		connect(&g_EventManager, &EventManager::dcmNet_Donwload_Started, this, &MainWidget::onDcmNetDonwloadStarted);
		connect(&g_EventManager, &EventManager::dcmNet_Download_InProgress, this, &MainWidget::onDcmNetDownloadInProgress);
		connect(&g_EventManager, &EventManager::dcmNet_Donwload_Finished, this, &MainWidget::onDcmNetDonwloadFinished);

		connect(&g_EventManager, &EventManager::dcmNet_Upload_Started, this, &MainWidget::onDcmNetUploadStarted);
		connect(&g_EventManager, &EventManager::dcmNet_Upload_InProgress, this, &MainWidget::onDcmNetUploadInProgress);
		connect(&g_EventManager, &EventManager::dcmNet_Upload_Finished, this, &MainWidget::onDcmNetUploadFinished);

		connect(&g_EventManager, &EventManager::dcmNet_Error, this, &MainWidget::onDcmNetError);

		connect(&g_EventManager, &EventManager::patientList_dicomListAdded, this, &MainWidget::onDicomListAdded);
		connect(&g_EventManager, &EventManager::patientList_dicomListModified, this, &MainWidget::onDicomListModifed);
		connect(&g_EventManager, &EventManager::fileSelected, this, &MainWidget::onFileSelected);
		connect(&g_EventManager, &EventManager::fileListSelected, this, &MainWidget::onFileListSelected);
		connect(&g_EventManager, &EventManager::appClosed, this, &MainWidget::onCloseApp);

		UpdateControlByNavRadioButton();
	}

	MainWidget::~MainWidget()
	{
		delete m_ui;
	}

	void MainWidget::SetImportDefaultMode(QString mipFileFilters, QString importFileFilters, QString defaultSelectFilter)
	{
		m_tabMode = ETabMode::import_default_mode;
		m_mipProjectFileViewMode.SetFilters(mipFileFilters);
		m_mipProjectFileViewMode.SetDefaultSelectFilter(defaultSelectFilter);

		m_importFileListViewMode.SetSelectMultiItem(true);
		m_importFileListViewMode.SetFilters(importFileFilters);
		m_importFileListViewMode.SetDefaultSelectFilter(defaultSelectFilter);
		m_ui->m_folderViewWidget->SetFilePath("", "");
		SetNavButtonType(nav_button_mip_project);
	}

	void MainWidget::SetImportDicomMode()
	{
		m_tabMode = ETabMode::import_dicom_mode;
		m_ui->m_folderViewWidget->SetFilePath("", "");
		SetNavButtonType(nav_button_dicom);
	}

	void MainWidget::SetImportFileMode(QString filters, QString filename, QString dirpath)
	{
		m_tabMode = ETabMode::import_only_file_mode;
		m_importFileListViewMode.SetSelectMultiItem(false);
		m_importFileListViewMode.SetFilters(filters);
		m_ui->m_folderViewWidget->SetFilePath(dirpath, filename);
		SetNavButtonType(nav_button_import);
	}

	void MainWidget::SetImportFileListMode(QString filters, QString filename, QString dirpath)
	{
		m_tabMode = ETabMode::import_only_file_mode;
		m_importFileListViewMode.SetSelectMultiItem(true);
		m_importFileListViewMode.SetFilters(filters);
		m_ui->m_folderViewWidget->SetFilePath(dirpath, filename);
		SetNavButtonType(nav_button_import);
	}

	void MainWidget::SetExportFileMode(QString filters, QString filename, QString dirpath)
	{
		m_tabMode = ETabMode::save_file_mode;
		m_exportFileListViewMode.SetSelectMultiItem(false);
		m_exportFileListViewMode.SetFilters(filters);
		m_exportFileListViewMode.SetFileFilterMode(fm::FileViewMode::EFileSelectMode::file);
		m_ui->m_folderViewWidget->SetFilePath(dirpath, filename);
		SetNavButtonType(nav_button_save);
	}

	void MainWidget::SetExportDirectoryMode(QString dirpath)
	{
		m_tabMode = ETabMode::save_file_mode;
		m_exportFileListViewMode.SetSelectMultiItem(false);
		m_exportFileListViewMode.SetFileFilterMode(fm::FileViewMode::EFileSelectMode::directory);
		m_ui->m_folderViewWidget->SetFilePath(dirpath, "");
		SetNavButtonType(nav_button_save);
	}

	void MainWidget::SetExport_PDF_Mode(QTextDocument* pPDFDocument, const FileViewMode& mode)
	{
		m_tabMode = ETabMode::export_PDF_mode;
		m_exportFileListViewMode = mode;
		m_ui->m_folderViewWidget->SetFilePath("", "");
		SetNavButtonType(nav_button_save);
	}

	void MainWidget::SetExport_PACS_Mode(DicomDataset& exportInfo, DicomExportData& exportData)
	{
		m_tabMode = ETabMode::upload_PACS_mode;
		m_pContext->GetDicomExportManager()->SetExportData(exportInfo, exportData);
		m_ui->m_folderViewWidget->SetFilePath("", "");
		SetNavButtonType(nav_button_pacs_upload);
	}

	void MainWidget::SetDebugMode()
	{
		m_tabMode = ETabMode::debug;
		m_ui->m_folderViewWidget->SetFilePath("", "");
		SetNavButtonType(nav_button_mip_project);
	}

	void MainWidget::UpdateControlByNavRadioButton()
	{
		if (m_ui->m_rdoTab_MIPProject->isChecked())
		{
			m_ui->m_mainViewContainerWidget->setCurrentIndex(page_folder);
			m_ui->m_folderViewWidget->SetFileOpenMode(m_mipProjectFileViewMode);
		}
		else if (m_ui->m_rdoTab_DICOM->isChecked())
		{
			m_ui->m_mainViewContainerWidget->setCurrentIndex(page_folder);
			m_ui->m_folderViewWidget->SetFileOpenMode(m_dicomFileViewMode);
		}
		else if (m_ui->m_rdoTab_Import->isChecked())
		{
			m_ui->m_mainViewContainerWidget->setCurrentIndex(page_folder);
			m_ui->m_folderViewWidget->SetFileOpenMode(m_importFileListViewMode);
		}
		else if (m_ui->m_rdoTab_Save->isChecked())
		{
			m_ui->m_mainViewContainerWidget->setCurrentIndex(page_folder);
			m_ui->m_folderViewWidget->SetFileOpenMode(m_exportFileListViewMode);
		}
		else if (m_ui->m_rdoTab_PatientList->isChecked())
		{
			m_ui->m_mainViewContainerWidget->setCurrentIndex(page_patient_list);
		}
		else if (m_ui->m_rdoTab_PACSDownload->isChecked())
		{
			m_ui->m_mainViewContainerWidget->setCurrentIndex(page_pacs_search_download);
		}
		else if (m_ui->m_rdoTab_PACSExport->isChecked())
		{
			m_ui->m_mainViewContainerWidget->setCurrentIndex(page_pacs_export);
		}
		else if (m_ui->m_rdoTab_PACSSearch->isChecked())
		{
			m_ui->m_mainViewContainerWidget->setCurrentIndex(page_pacs_search_download);
		}

		UpdateControl();
	}

	void MainWidget::UpdateControl()
	{
		UpdateVisiblityOfControl();

		if (CurrentPageIndex() == page_folder)
		{
			m_ui->m_folderViewWidget->Refresh();
		}
		else if (CurrentPageIndex() == page_patient_list)
		{
			m_ui->m_patientListWidget->Refresh();
		}
		else if (CurrentPageIndex() == page_pacs_search_download)
		{
			if (m_ui->m_rdoTab_PACSDownload->isChecked())
			{
				m_ui->m_pacsSearchDownloadWidget->SetMode(fm::EPACSOperationMode::can_download_mode, fm::EPACSSearchMode::search_series);
			}
			else if (m_ui->m_rdoTab_PACSSearch->isChecked())
			{
				m_ui->m_pacsSearchDownloadWidget->SetMode(fm::EPACSOperationMode::only_search_mode, fm::EPACSSearchMode::search_series);
			}

			m_ui->m_pacsSearchDownloadWidget->Refresh();
		}
		else if (CurrentPageIndex() == page_pacs_export)
		{
			m_ui->m_pacsExportWidget->Refresh();
		}
	}

	void MainWidget::UpdateVisiblityOfControl()
	{
		switch (m_tabMode)
		{
		case ETabMode::import_default_mode:
			m_ui->m_rdoTab_MIPProject->setVisible(true);
			m_ui->m_rdoTab_DICOM->setVisible(true);
			m_ui->m_rdoTab_Import->setVisible(true);
			m_ui->m_rdoTab_PatientList->setVisible(true);
			m_ui->m_rdoTab_PACSDownload->setVisible(true);

			m_ui->m_rdoTab_Save->setVisible(false);

			//m_rdoTab_PACSUpload->setVisible(false);
			m_ui->m_rdoTab_PACSExport->setVisible(false);
			m_ui->m_rdoTab_PACSSearch->setVisible(false);
			break;
		case ETabMode::import_only_file_mode:
			m_ui->m_rdoTab_MIPProject->setVisible(false);
			m_ui->m_rdoTab_DICOM->setVisible(false);
			m_ui->m_rdoTab_Import->setVisible(true);
			m_ui->m_rdoTab_PatientList->setVisible(false);
			m_ui->m_rdoTab_PACSDownload->setVisible(false);

			m_ui->m_rdoTab_Save->setVisible(false);

			//m_rdoTab_PACSUpload->setVisible(false);
			m_ui->m_rdoTab_PACSExport->setVisible(false);
			m_ui->m_rdoTab_PACSSearch->setVisible(false);
			break;

		case ETabMode::import_dicom_mode:
			m_ui->m_rdoTab_MIPProject->setVisible(false);
			m_ui->m_rdoTab_DICOM->setVisible(true);
			m_ui->m_rdoTab_Import->setVisible(false);
			m_ui->m_rdoTab_PatientList->setVisible(true);
			m_ui->m_rdoTab_PACSDownload->setVisible(true);

			m_ui->m_rdoTab_Save->setVisible(false);

			//m_rdoTab_PACSUpload->setVisible(false);
			m_ui->m_rdoTab_PACSExport->setVisible(false);
			m_ui->m_rdoTab_PACSSearch->setVisible(false);
			break;

		case ETabMode::save_file_mode:
		case ETabMode::save_directory_mode:
			m_ui->m_rdoTab_MIPProject->setVisible(false);
			m_ui->m_rdoTab_DICOM->setVisible(false);
			m_ui->m_rdoTab_Import->setVisible(false);
			m_ui->m_rdoTab_PatientList->setVisible(false);
			m_ui->m_rdoTab_PACSDownload->setVisible(false);

			m_ui->m_rdoTab_Save->setVisible(true);

			//m_rdoTab_PACSUpload->setVisible(false);
			m_ui->m_rdoTab_PACSExport->setVisible(false);
			m_ui->m_rdoTab_PACSSearch->setVisible(false);
			break;

		case ETabMode::export_PDF_mode:
			m_ui->m_rdoTab_MIPProject->setVisible(false);
			m_ui->m_rdoTab_DICOM->setVisible(false);
			m_ui->m_rdoTab_Import->setVisible(false);

			m_ui->m_rdoTab_PatientList->setVisible(false);
			m_ui->m_rdoTab_PACSDownload->setVisible(false);

			m_ui->m_rdoTab_Save->setVisible(true);

			//m_rdoTab_PACSUpload->setVisible(true);
			m_ui->m_rdoTab_PACSExport->setVisible(false);
			m_ui->m_rdoTab_PACSSearch->setVisible(false);
			break;

		case ETabMode::upload_PACS_mode:
			m_ui->m_rdoTab_MIPProject->setVisible(false);
			m_ui->m_rdoTab_DICOM->setVisible(false);
			m_ui->m_rdoTab_Import->setVisible(false);

			m_ui->m_rdoTab_PatientList->setVisible(false);
			m_ui->m_rdoTab_PACSDownload->setVisible(false);

			m_ui->m_rdoTab_Save->setVisible(false);

			//m_rdoTab_PACSUpload->setVisible(true);
			m_ui->m_rdoTab_PACSExport->setVisible(true);
			m_ui->m_rdoTab_PACSSearch->setVisible(true);
			break;

		case ETabMode::debug:
			m_ui->m_rdoTab_MIPProject->setVisible(true);
			m_ui->m_rdoTab_DICOM->setVisible(true);
			m_ui->m_rdoTab_Import->setVisible(true);
			m_ui->m_rdoTab_PatientList->setVisible(true);
			m_ui->m_rdoTab_PACSDownload->setVisible(true);

			m_ui->m_rdoTab_Save->setVisible(true);
			//m_rdoTab_PACSUpload->setVisible(true);

			m_ui->m_rdoTab_PACSExport->setVisible(true);
			m_ui->m_rdoTab_PACSSearch->setVisible(true);
			break;

		default:
			break;
		}

		if (m_pContext->GetProductFunctionType().CanPACSUpload == false)
		{
			m_ui->m_rdoTab_PACSExport->setVisible(false);
		}

		if (m_pContext->GetProductFunctionType().CanPACSDownload == false)
		{
			m_ui->m_rdoTab_PACSDownload->setVisible(false);
		}
	}

	void MainWidget::SetNavButtonType(ENavButtonType navButtonType)
	{
		if (navButtonType == nav_button_mip_project)
		{
			m_ui->m_rdoTab_MIPProject->setChecked(true);
			m_ui->m_mainViewContainerWidget->setCurrentIndex(page_folder);
			m_ui->m_folderViewWidget->SetFileOpenMode(m_mipProjectFileViewMode);
		}
		if (navButtonType == nav_button_dicom)
		{
			m_ui->m_rdoTab_DICOM->setChecked(true);
			m_ui->m_mainViewContainerWidget->setCurrentIndex(page_folder);
			m_ui->m_folderViewWidget->SetFileOpenMode(m_dicomFileViewMode);
		}
		if (navButtonType == nav_button_import)
		{
			m_ui->m_rdoTab_Import->setChecked(true);
			m_ui->m_mainViewContainerWidget->setCurrentIndex(page_folder);
			m_ui->m_folderViewWidget->SetFileOpenMode(m_importFileListViewMode);
		}
		if (navButtonType == nav_button_save)
		{
			m_ui->m_rdoTab_Save->setChecked(true);
			m_ui->m_mainViewContainerWidget->setCurrentIndex(page_folder);
			m_ui->m_folderViewWidget->SetFileOpenMode(m_exportFileListViewMode);
		}
		else if (navButtonType == nav_button_patient_list)
		{
			m_ui->m_rdoTab_PatientList->setChecked(true);
			m_ui->m_mainViewContainerWidget->setCurrentIndex(page_patient_list);
		}
		else if (navButtonType == nav_button_pacs_download)
		{
			m_ui->m_rdoTab_PACSDownload->setChecked(true);
			m_ui->m_mainViewContainerWidget->setCurrentIndex(page_pacs_search_download);
		}
		else if (navButtonType == nav_button_pacs_upload)
		{
			m_ui->m_rdoTab_PACSExport->setChecked(true);
			m_ui->m_mainViewContainerWidget->setCurrentIndex(page_pacs_export);
		}
		else if (navButtonType == nav_button_pacs_search)
		{
			m_ui->m_rdoTab_PACSSearch->setChecked(true);
			m_ui->m_mainViewContainerWidget->setCurrentIndex(page_pacs_search_download);
		}

		UpdateControl();
	}

	MainWidget::ENavButtonType MainWidget::GetNavButtonType()
	{
		if (m_ui->m_rdoTab_MIPProject->isChecked())
		{
			return ENavButtonType::nav_button_mip_project;
		}
		else if (m_ui->m_rdoTab_DICOM->isChecked())
		{
			return ENavButtonType::nav_button_dicom;
		}
		else if (m_ui->m_rdoTab_Import->isChecked())
		{
			return ENavButtonType::nav_button_import;
		}
		else if (m_ui->m_rdoTab_Save->isChecked())
		{
			return ENavButtonType::nav_button_save;
		}
		else if (m_ui->m_rdoTab_PatientList->isChecked())
		{
			return ENavButtonType::nav_button_patient_list;
		}
		else if (m_ui->m_rdoTab_PACSDownload->isChecked())
		{
			return ENavButtonType::nav_button_pacs_download;
		}
		else if (m_ui->m_rdoTab_PACSExport->isChecked())
		{
			return ENavButtonType::nav_button_pacs_upload;
		}
		else if (m_ui->m_rdoTab_PACSSearch->isChecked())
		{
			return ENavButtonType::nav_button_pacs_search;
		}
		else
		{
			Q_ASSERT(false);
			return ENavButtonType::nav_button_mip_project;
		}
	}

	MainWidget::EPageIndex MainWidget::CurrentPageIndex()
	{
		return (EPageIndex)m_ui->m_mainViewContainerWidget->currentIndex();
	}

	void MainWidget::SaveStatus()
	{
		m_ui->m_folderViewWidget->SaveStatus();
		m_ui->m_patientListWidget->SaveStatus();
		m_ui->m_pacsSearchDownloadWidget->SaveStatus();
		m_ui->m_pacsExportWidget->SaveStatus();
	}

	bool MainWidget::IsMIPProjectVisible() const
	{
		return m_ui->m_rdoTab_MIPProject->isVisible();
	}

	bool MainWidget::IsDICOMVisible() const
	{
		return m_ui->m_rdoTab_DICOM->isVisible();
	}

	bool MainWidget::IsImportFileVisible() const
	{
		return m_ui->m_rdoTab_Import->isVisible();
	}

	bool MainWidget::IsPatientListVisible() const
	{
		return m_ui->m_rdoTab_PatientList->isVisible();
	}

	bool MainWidget::IsPACSDownloadVisible() const
	{
		return m_ui->m_rdoTab_PACSDownload->isVisible();
	}

	bool MainWidget::IsFileSaveVisible() const
	{
		return m_ui->m_rdoTab_Save->isVisible();
	}

	bool MainWidget::IsPACSUploadVisible() const
	{
		return m_ui->m_rdoTab_PACSExport->isVisible();
	}

	bool MainWidget::IsPACSSearchVisible() const
	{
		return m_ui->m_rdoTab_PACSSearch->isVisible();
	}

	void MainWidget::HandleDicomDownloadResult(DcmNetDownloadData& dcmDownloadData)
	{
		m_pContext->GetActionManager()->ImportDicomDatasetList(dcmDownloadData.DicomDatasetList);

		//if (dcmDataset.GetDicomType() == fm::DicomDataset::EType::Default)
		//{
		//	CopyDcm_And_ImpotToPatientList(downloadDcmFilePath);
		//}
		//else if (dcmDataset.GetDicomType() == fm::DicomDataset::EType::EncapsulatedPDFStorage)
		//{
		//	ConvertDcmToPDF_And_OpenDirectoryPath(downloadDcmFilePath);
		//}
		//else if (dcmDataset.GetDicomType() == fm::DicomDataset::EType::SecondaryCaptureImage)
		//{
		//	ConvertDcmToImage_And_OpenDirectoryPath(downloadDcmFilePath);
		//}
	}

	void MainWidget::CopyDcm_And_ImpotToPatientList(QString dcmFilePath)
	{
		m_pContext->GetActionManager()->ImportDicomFileList({ dcmFilePath });
	}

	void MainWidget::ConvertDcmToPDF_And_OpenDirectoryPath(QString dcmFilePath)
	{
		QString targetPDFFilePath = QFileDialog::getSaveFileName(
			this,
			QString(),
			QString(),
			tr("PDF (*.pdf;*.PDF)"));

		if (!targetPDFFilePath.isEmpty())
		{
			if (fm::DicomConverter::ConvertDcmFile_To_PDFFile(dcmFilePath.toStdWString(), targetPDFFilePath.toStdWString()))
			{
				QString dirpath = FileSystemUtil::GetDirPath(targetPDFFilePath);
				QDesktopServices::openUrl(QUrl::fromLocalFile(dirpath));
			}
		}
	}

	void MainWidget::ConvertDcmToImage_And_OpenDirectoryPath(QString dcmFilePath)
	{
		QString targetImageFilePath = QFileDialog::getSaveFileName(
			this,
			QString(),
			QString(),
			tr("PNG (*.png;*.PNG);;JPEG (*.jpg;*.JPG);;BMP (*.bmp;*.BMP)"));

		if (!targetImageFilePath.isEmpty())
		{
			if (fm::DicomConverter::ConvertDcmFile_To_ImgFile(dcmFilePath.toStdWString(), targetImageFilePath.toStdWString()))
			{
				QString dirpath = FileSystemUtil::GetDirPath(targetImageFilePath);
				QDesktopServices::openUrl(QUrl::fromLocalFile(dirpath));
			}
		}
	}

	bool MainWidget::event(QEvent* e)
	{
		return QWidget::event(e);
	}

	void MainWidget::closeEvent(QCloseEvent* e)
	{
		QWidget::closeEvent(e);
	}

	void MainWidget::onRdoTabClicked()
	{
		UpdateControlByNavRadioButton();
	}

	void MainWidget::onSettingBtnClicked()
	{
		SettingDialog dlg(m_pContext, this);
		dlg.exec();
	}

	void MainWidget::onPatientListFileCopyStarted()
	{
		ProgressBarDialog::Global_SetText(QString::fromLocal8Bit("Start Copy"));
		ProgressBarDialog::Global_SetMode(ProgressBarDialog::EMode::busy_indicator);
		ProgressBarDialog::Global_Show();
	}

	void MainWidget::onPatientListFileCopyInProgressed(int progressCount, int maxCount, QString copiedFilePath)
	{
		QString text = QString::fromLocal8Bit("Copy in progress - Count( %1 )").arg(progressCount);
		ProgressBarDialog::Global_SetText(text);
	}

	void MainWidget::onPatientListFileCopyFinished(QString copiedDirectoryPath)
	{
		ProgressBarDialog::Global_Hide();

		m_ui->m_folderViewWidget->SetCurrentDirectoryPath(copiedDirectoryPath);
		SetNavButtonType(nav_button_patient_list);
	}

	void MainWidget::onDcmNetFindStarted()
	{
		//setEnabled(false);
		m_ui->m_navButtonContainerWidget->setEnabled(false);
		m_ui->m_mainViewContainerWidget->setEnabled(false);
		ProgressBarDialog::Global_SetText(QString::fromLocal8Bit("Find Start"));
		ProgressBarDialog::Global_SetMode(ProgressBarDialog::EMode::busy_indicator);
		ProgressBarDialog::Global_Show();
	}

	void MainWidget::onDcmNetFindInProgress(DicomNetworkSCUStatus status)
	{
		QString text = QString::fromLocal8Bit("Find in progress - Count( %1 )").arg(status.InProgressCount);
		ProgressBarDialog::Global_SetText(text);
	}

	void MainWidget::onDcmNetFindFinished(DcmNetFindData findData)
	{
		//setEnabled(true);
		m_ui->m_navButtonContainerWidget->setEnabled(true);
		m_ui->m_mainViewContainerWidget->setEnabled(true);
		m_ui->m_mainViewContainerWidget->setFocus();
		ProgressBarDialog::Global_Hide();
		//m_ui->m_pacsSearchDownloadWidget->setFocus();
	}

	void MainWidget::onDcmNetDonwloadStarted()
	{
		setEnabled(false);
		ProgressBarDialog::Global_SetText(QString::fromLocal8Bit("Download Start"));
		ProgressBarDialog::Global_SetMode(ProgressBarDialog::EMode::busy_indicator);
		ProgressBarDialog::Global_Show();
	}

	void MainWidget::onDcmNetDownloadInProgress(DicomNetworkSCUStatus status)
	{
		QString text = QString::fromLocal8Bit("Download in progress - Count( %1 )").arg(status.InProgressCount);
		ProgressBarDialog::Global_SetText(text);

		if (status.MaxProgressCount == 0)
		{
			ProgressBarDialog::Global_SetMode(ProgressBarDialog::EMode::busy_indicator);
		}
		else
		{
			ProgressBarDialog::Global_SetMode(ProgressBarDialog::EMode::normal);
			ProgressBarDialog::Global_SetProgress(status.ProgressRate());
		}
	}

	void MainWidget::onDcmNetDonwloadFinished(DcmNetDownloadData result)
	{
		setEnabled(true);
		ProgressBarDialog::Global_Hide();

		//QMessageBox::information(
		//	this,
		//	QString::fromLocal8Bit("Download"),
		//	QString::fromLocal8Bit("Download From PACS Finished"),
		//	QMessageBox::StandardButton::Ok
		//);

		HandleDicomDownloadResult(result);
	}

	void MainWidget::onDcmNetUploadStarted()
	{
		setEnabled(false);
		ProgressBarDialog::Global_SetText(QString::fromLocal8Bit("Upload start"));
		ProgressBarDialog::Global_SetMode(ProgressBarDialog::EMode::busy_indicator);
		ProgressBarDialog::Global_Show();
	}

	void MainWidget::onDcmNetUploadInProgress(DicomNetworkSCUStatus status)
	{
		setEnabled(false);
		QString text = QString::fromLocal8Bit("Upload in progress - Count( %1 )").arg(status.InProgressCount);
		ProgressBarDialog::Global_SetText(text);

		if (status.MaxProgressCount == 0)
		{
			ProgressBarDialog::Global_SetMode(ProgressBarDialog::EMode::busy_indicator);
		}
		else
		{
			ProgressBarDialog::Global_SetMode(ProgressBarDialog::EMode::normal);
			ProgressBarDialog::Global_SetProgress(status.ProgressRate());
		}
	}

	void MainWidget::onDcmNetUploadFinished()
	{
		setEnabled(true);
		ProgressBarDialog::Global_Hide();

		QMessageBox::information(this,
			StringManager::GetString(STR_TITLE_INFO),
			StringManager::GetString(STR_INFO_PACS_UPLOAD_FINISH));
	}

	void MainWidget::onDcmNetError(QString errorMessage)
	{
		setEnabled(true);
		m_ui->m_navButtonContainerWidget->setEnabled(true);
		m_ui->m_mainViewContainerWidget->setEnabled(true);

		ProgressBarDialog::Global_Hide();

		QMessageBox::critical(this,
			StringManager::GetString(STR_TITLE_ERROR),
			StringManager::GetString(STR_ERR_PACS_NET_ERROR)
		);

		qCritical() << "Dicom net error : " << errorMessage;
	}

	void MainWidget::onDicomListAdded(std::vector<ImportedDicomInfoDTO> dcmList)
	{
		//m_ui->m_mainViewContainerWidget->setCurrentIndex(page_patient_list);
		m_ui->m_rdoTab_PatientList->setChecked(true);
		UpdateControlByNavRadioButton();
	}

	void MainWidget::onDicomListModifed(std::vector<ImportedDicomInfoDTO> dcmList)
	{
		m_ui->m_rdoTab_PatientList->setChecked(true);
		UpdateControlByNavRadioButton();
	}

	void MainWidget::onFileSelected(QString filepath)
	{
		onFileListSelected({ filepath });
	}

	void MainWidget::onFileListSelected(QStringList filepathList)
	{
		ENavButtonType type = GetNavButtonType();
		if (type == ENavButtonType::nav_button_mip_project)
		{
			if (!filepathList.isEmpty())
			{
				emit mipFileSelected(filepathList[0]);
			}
		}
		else if (type == ENavButtonType::nav_button_dicom)
		{
			emit dicomFileListSelected(filepathList);
		}
		else if (type == ENavButtonType::nav_button_import)
		{
			emit importFileListSelected(filepathList);
		}
		else if (type == ENavButtonType::nav_button_patient_list)
		{
			emit dicomPatientListFilePathSelected(filepathList);
		}
		else if (type == ENavButtonType::nav_button_save)
		{
			emit exportFileListSelected(filepathList);
		}
		else
		{
			//TODO : 환자리스트 nav 버튼으로 적용하도록 수정
			qCritical() << "can't select file list. type is invalid(=" << type << ")";
			//Q_ASSERT(false);
		}
	}

	void MainWidget::onCloseApp()
	{
		emit closed();
	}

}

