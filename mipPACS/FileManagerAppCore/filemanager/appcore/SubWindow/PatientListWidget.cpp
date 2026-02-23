#include "stdafx.h"
#include "PatientListWidget.h"
#include <filemanager/appcore/AppCoreContext.h>
#include <filemanager/appcore/Event/EventManager.h>
#include <filemanager/appcore/Util/FileSystemUtil.h>
#include <filemanager/appcore/Dialog/Util/ProgressBarDialog.h>
#include <filemanager/appcore/Dialog/DicomEditDialog.h>
#include <filemanager/data/Entity/CommonDefines.h>
#include <filemanager/data/Entity/ImportedDicomInfoDefines.h>
#include <filemanager/dicom/Convert/DicomConverter.h>

#include <QFileDialog>
#include <QMouseEvent>
#include <QDesktopServices>
#include <QCheckBox>
#include <QMessageBox>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QClipboard>

namespace fm
{
	PatientListWidget::PatientListWidget(QWidget* parent) :
		QWidget(parent)
	{
		setupUi(this);

		m_mainDicomInfoModel = DicomInfomationModel::CreateDefault();

		m_actDicomEdit_Study = new QAction("Dicom Edit");
		m_actOpenContainingFolder_Study = new QAction("Open Containing Folder");
		m_actDelete_Study = new QAction("Delete");
		m_actCopyAllToClipboard_Study = new QAction("Copy All");

		m_actOpenFile = new QAction("Open File");
		m_actOpenContainingFolder_Series = new QAction("Open Containing Folder");
		m_actCopyFolder_Series = new QAction("Copy Folder");
		m_actDelete_Series = new QAction("Delete");
		m_actCopyAllToClipboard_Series = new QAction("Copy All");

		m_colHeaderList_Study.Add(COL_STUDY_UID, "Study UID", true);
		m_colHeaderList_Study.Add(COL_STUDY_DATETIME, "Study Date", true);
		m_colHeaderList_Study.Add(COL_STUDY_PATIENT_NAME, "Patient Name", true);
		m_colHeaderList_Study.Add(COL_STUDY_PATIENT_ID, "Patient ID", true);
		m_colHeaderList_Study.Add(COL_STUDY_SEX, "Sex", true);
		m_colHeaderList_Study.Add(COL_STUDY_AGE, "Age", true);
		m_colHeaderList_Study.Add(COL_STUDY_IMPORTED_DATE, "Imported Date ", true);
		m_colHeaderList_Study.Add(COL_STUDY_IMAGE_COUNT, "Count", true);
		m_colHeaderList_Study.Add(COL_STUDY_DESCRIPTION, "Study Description", true);
		m_colHeaderList_Study.Add(COL_STUDY_ACCESSION_NUMBER, "Accession Number", true);
		m_colHeaderList_Study.Add(COL_STUDY_EXAM_ID, "Exam ID", true);
		m_colHeaderList_Study.Add(COL_STUDY_DCM_FOLDER, "Dicom Folder", true);

		m_colHeaderList_Series.Add(COL_SERIES_UID, "Series UID", true);
		m_colHeaderList_Series.Add(COL_SERIES_DATETIME, "Series Date", true);
		m_colHeaderList_Series.Add(COL_SERIES_NUMBER, "Number", true);
		m_colHeaderList_Series.Add(COL_SERIES_MODALITY, "Modality", true);
		m_colHeaderList_Series.Add(COL_SERIES_IMAGE_COUNT, "Count", true);
		m_colHeaderList_Series.Add(COL_SERIES_DESCRIPTION, "Description", true);
		m_colHeaderList_Series.Add(COL_SERIES_DCM_FILEPATH, "Dicom Filepath", true);
		m_colHeaderList_Series.Add(COL_SERIES_DCM_DIRECTORY_PATH, "Dicom Directory Path", true);
		m_colHeaderList_Series.Add(COL_SERIES_DCM_FILE_LIST, "Dicom File List", true);

		m_findOptionList.Add(ImportedDicomInfoDefines::COL_NAME_STUDY_INSTANCE_UID, L"StudyInstanceUID", true);
		m_findOptionList.Add(ImportedDicomInfoDefines::COL_NAME_EXAM_ID, L"ExamID", true);
		m_findOptionList.Add(ImportedDicomInfoDefines::COL_NAME_PATIENT_NAME, L"PATIENT_NAME", true);
		m_findOptionList.Add(ImportedDicomInfoDefines::COL_NAME_PATIENT_ID, L"PATIENT_ID", true);
		m_findOptionList.Add(ImportedDicomInfoDefines::COL_NAME_PATIENT_SEX, L"PATIENT_SEX", true);
		m_findOptionList.Add(ImportedDicomInfoDefines::COL_NAME_PATIENT_AGE, L"PATIENT_AGE", true);
		m_findOptionList.Add(ImportedDicomInfoDefines::COL_NAME_ACCESSION_NUMBER, L"ACCESSION_NUMBER", true);
		m_findOptionList.Add(ImportedDicomInfoDefines::COL_NAME_STUDY_DESCRIPTION, L"STUDY_DESCRIPTION", true);
		m_findOptionList.Add(ImportedDicomInfoDefines::COL_NAME_REFERRING_PHYSICIAN, L"REFERRING_PHYSICIAN", true);
		m_findOptionList.Add(ImportedDicomInfoDefines::COL_NAME_PERFORMING_PHYSICIAN, L"PERFORMING_PHYSICIAN", true);

		QAction* actionSeperator = new QAction();
		actionSeperator->setSeparator(true);

		m_tableStudyList->setColumnCount(m_colHeaderList_Study.Count());
		m_tableStudyList->setHorizontalHeaderLabels(m_colHeaderList_Study.GetNameHeaderList());
		m_tableStudyList->setEditTriggers(QAbstractItemView::NoEditTriggers);
		m_tableStudyList->setAlternatingRowColors(true);
		//m_tableStudyList->setSelectionMode(QAbstractItemView::SingleSelection);
		m_tableStudyList->setSelectionMode(QAbstractItemView::ExtendedSelection);
		m_tableStudyList->setSelectionBehavior(QAbstractItemView::SelectRows);
		m_tableStudyList->horizontalHeader()->setHighlightSections(false);
		m_tableStudyList->setSortingEnabled(true);
		m_tableStudyList->setContextMenuPolicy(Qt::ActionsContextMenu);
		/* == Add Study Action ==*/
		m_tableStudyList->addAction(m_actDicomEdit_Study);
		m_tableStudyList->addAction(m_actOpenContainingFolder_Study);
		m_tableStudyList->addAction(m_actDelete_Study);
		m_tableStudyList->addAction(m_actCopyAllToClipboard_Study);

		m_tableSeriesList->setColumnCount(m_colHeaderList_Series.Count());
		m_tableSeriesList->setHorizontalHeaderLabels(m_colHeaderList_Series.GetNameHeaderList());
		m_tableSeriesList->setEditTriggers(QAbstractItemView::NoEditTriggers);
		m_tableSeriesList->setAlternatingRowColors(true);
		//m_tableSeriesList->setSelectionMode(QAbstractItemView::SingleSelection);
		m_tableSeriesList->setSelectionMode(QAbstractItemView::ExtendedSelection);
		m_tableSeriesList->setSelectionBehavior(QAbstractItemView::SelectRows);
		m_tableSeriesList->horizontalHeader()->setHighlightSections(false);
		m_tableSeriesList->setSortingEnabled(true);
		m_tableSeriesList->setContextMenuPolicy(Qt::ActionsContextMenu);
		/* == Add Study Action ==*/
		m_tableSeriesList->addAction(m_actOpenFile);
		m_tableSeriesList->addAction(m_actOpenContainingFolder_Series);
		m_tableSeriesList->addAction(actionSeperator);
		m_tableSeriesList->addAction(m_actCopyFolder_Series);
		m_tableSeriesList->addAction(m_actDelete_Series);
		m_tableSeriesList->addAction(m_actCopyAllToClipboard_Series);

		m_btnImportFromPACS->hide();	/* 현재 사용하지 않는다. */

		m_btnImportDICOM->setStyleSheet(g_ResourceManager.GetButtonImportDicom());

		connect(m_btnImportDICOM, &QPushButton::clicked, this, &PatientListWidget::onImportDICOM);
		connect(m_btnImportFromPACS, &QPushButton::clicked, this, &PatientListWidget::onImportFromPACS);
		connect(m_btnSearch, &QPushButton::clicked, this, &PatientListWidget::onSearch);
		connect(m_editSearch, &QLineEdit::returnPressed, this, &PatientListWidget::onSearch);
		connect(m_chkPreview, &QCheckBox::clicked, this, &PatientListWidget::onChkPreview);

		connect(m_tableStudyList, &QTableWidget::itemChanged, this, &PatientListWidget::onStudyListItemChanged);
		connect(m_tableStudyList, &QTableWidget::currentCellChanged, this, &PatientListWidget::onStudyListCurrentCellChanged);

		connect(m_tableSeriesList, &QTableWidget::doubleClicked, this, &PatientListWidget::onSeriesListDblClick);
		connect(m_tableSeriesList, &QTableWidget::currentCellChanged, this, &PatientListWidget::onSeriesListCurrentCellChanged);

		connect(m_actDicomEdit_Study, &QAction::triggered, this, &PatientListWidget::onActDicomEdit_Study);
		connect(m_actOpenContainingFolder_Study, &QAction::triggered, this, &PatientListWidget::onActOpenContainingFolder_Study);
		connect(m_actDelete_Study, &QAction::triggered, this, &PatientListWidget::onActDelete_Study);
		connect(m_actCopyAllToClipboard_Study, &QAction::triggered, this, &PatientListWidget::onActCopyAllToClipboard_Study);

		connect(m_actOpenFile, &QAction::triggered, this, &PatientListWidget::onActOpenFile);
		connect(m_actOpenContainingFolder_Series, &QAction::triggered, this, &PatientListWidget::onActOpenContainingFolder_Series);
		connect(m_actCopyFolder_Series, &QAction::triggered, this, &PatientListWidget::onActCopyFolder_Series);
		connect(m_actDelete_Series, &QAction::triggered, this, &PatientListWidget::onActDelete_Series);
		connect(m_actCopyAllToClipboard_Series, &QAction::triggered, this, &PatientListWidget::onActCopyAllToClipboard_Series);

		connect(&g_EventManager, &EventManager::patientList_dicomUpload_Started, this, &PatientListWidget::onPatientListDicomUploadStarted);
		connect(&g_EventManager, &EventManager::patientList_dicomUpload_InProgress, this, &PatientListWidget::onPatientListDicomUploadInProgress);
		connect(&g_EventManager, &EventManager::patientList_dicomUpload_Finished, this, &PatientListWidget::onPatientListDicomUploadFinished);

		connect(&g_EventManager, &EventManager::patientList_dicomEdit_Started, this, &PatientListWidget::onPatientListDicomEditStarted);
		connect(&g_EventManager, &EventManager::patientList_dicomEdit_InProgress, this, &PatientListWidget::onPatientListDicomEditInProgress);
		connect(&g_EventManager, &EventManager::patientList_dicomEdit_Finished, this, &PatientListWidget::onPatientListDicomEditFinished);

		connect(&g_EventManager, &EventManager::settingChanged, this, &PatientListWidget::onSettingChanged);
		connect(&g_EventManager, &EventManager::patientList_dicomListAdded, this, &PatientListWidget::onDicomListAdded);
		connect(&g_EventManager, &EventManager::patientList_dicomListModified, this, &PatientListWidget::onDicomListModified);

		m_chkPreview->setChecked(true);

		this->setAcceptDrops(true);
	}

	PatientListWidget::~PatientListWidget()
	{

	}

	void PatientListWidget::Init(AppCoreContext* pContext)
	{
		m_pContext = pContext;

		m_previewWidget->SetStudyListVisible(false);
		m_previewWidget->SetSeriesListVisibile(false);
		UpdateByRuntimeMode();

		InitLayout();
	}

	void PatientListWidget::Refresh(bool forceToUpdate)
	{
		if (forceToUpdate)
		{
			RefreshModel();
		}
		else
		{
			if (m_tableStudyList->rowCount() == 0)
			{
				RefreshModel();
			}
		}
	}

	void PatientListWidget::SaveStatus()
	{
		GUISettingConfig* pConfig = m_pContext->GetConfigManager()->GetGUISettingConfig();
		PatientListWidgetInfo widgetInfo = pConfig->GetPatientListWidgetInfo();

		widgetInfo.CenterHorizontalContainerSplitterStatus = m_splitterHorzCenter->saveState();
		widgetInfo.RightContainerSplitterStatus = m_splitterRightContainer->saveState();
		pConfig->SetPatientListWidgetInfo(widgetInfo);

		pConfig->Save();
	}

	void PatientListWidget::dragEnterEvent(QDragEnterEvent* e)
	{
		e->accept();
		//QStringList format = e->mimeData()->formats();
		//qDebug() << "format :" << format;
		//if (e->mimeData()->hasFormat("text/plain"))
		//	e->acceptProposedAction();
	}

	void PatientListWidget::dropEvent(QDropEvent* e)
	{
		const QMimeData* mimeData = e->mimeData();
		if (mimeData->text().isEmpty())
		{
			e->ignore();
			return;
		}

		QString filename;
		QList<QUrl> fileUrlList = mimeData->urls();
		QStringList filePathList;
		foreach(QUrl fileUrl, fileUrlList)
		{
			filePathList << fileUrl.toLocalFile();
		}

		m_pContext->GetActionManager()->PatientList_UploadDicomFileList(filePathList);
	}

	void PatientListWidget::InitLayout()
	{
		GUISettingConfig* pConfig = m_pContext->GetConfigManager()->GetGUISettingConfig();
		PatientListWidgetInfo info = pConfig->GetPatientListWidgetInfo();

		m_splitterHorzCenter->restoreState(info.CenterHorizontalContainerSplitterStatus);
		m_splitterRightContainer->restoreState(info.RightContainerSplitterStatus);
	}

	void PatientListWidget::RefreshModel(std::wstring selectedStudyInstanceUID, std::wstring selectedSeriesInstanceUID)
	{
		ImportedDicomInfoDAO* pImportDcmDAO = m_pContext->GetDataManager()->GetImportedDicomInfoDAO();
		std::vector<ImportedDicomInfoDTO> importedDcmInfoList;
		pImportDcmDAO->GetAll(importedDcmInfoList);
		ResetDicomDatasetModel_As_DTODataList(importedDcmInfoList, selectedStudyInstanceUID, selectedSeriesInstanceUID);
	}

	void PatientListWidget::ResetDicomDatasetModel_As_DTODataList(std::vector<ImportedDicomInfoDTO>& dtoList, std::wstring studyInstanceUID, std::wstring seriesInstanceUID)
	{
		m_mainDicomInfoModel->Clear();
		AddToDicomDatasetModel_As_DTODataList(dtoList, studyInstanceUID, seriesInstanceUID);
	}

	void PatientListWidget::AddToDicomDatasetModel_As_DTODataList(std::vector<fm::ImportedDicomInfoDTO>& dtoList, std::wstring studyInstanceUID, std::wstring seriesInstanceUID)
	{
		for (auto& dto : dtoList)
		{
			m_mainDicomInfoModel->Add(dto.ToDcmDataset());
		}

		UpdateStudyList(studyInstanceUID, seriesInstanceUID);
	}

	void PatientListWidget::UpdateStudyList(std::wstring studyInstanceUID, std::wstring seriesInstanceUID)
	{
		std::vector<DicomInfomationModelStudyObject*> studyList = m_mainDicomInfoModel->GetStudyList();

		/* Series, Study RowCount 초기화 */
		m_tableSeriesList->setRowCount(0);
		m_tableStudyList->setRowCount(0);

		int studyRowCount = studyList.size();
		m_tableStudyList->setRowCount(studyRowCount);
		m_tableStudyList->setSortingEnabled(false);

		int rowIndex = 0;
		for (DicomInfomationModelStudyObject* pStudy : studyList)
		{
			SetTableStudyRow(*pStudy, rowIndex);
			rowIndex++;
		}

		m_tableStudyList->resizeColumnsToContents();
		m_tableStudyList->setSortingEnabled(true);

		SelectStudyList_And_SereisList(studyInstanceUID, seriesInstanceUID);
	}

	void PatientListWidget::SetTableStudyRow(DicomInfomationModelStudyObject& study, int rowIndex)
	{
		DicomDataset* pStudyDataset;
		if (study.GetStudyDataset(&pStudyDataset))
		{
			DICOM_HEADER_INFO dcmHeaderInfo = pStudyDataset->ToDcmHeaderInfo();
			std::wstring importDataTime_Format = pStudyDataset->GetValue_Extension(DCM_EXT_IMPORT_DATETIME_FORMAT);

			m_tableStudyList->setItem(rowIndex, COL_STUDY_UID, new QTableWidgetItem(QString::fromStdWString(dcmHeaderInfo.StudyInstanceUID)));
			m_tableStudyList->setItem(rowIndex, COL_STUDY_DATETIME, new QTableWidgetItem(QString::fromStdString(dcmHeaderInfo.StudyDateTime.ToFormatText_DateTime())));
			m_tableStudyList->setItem(rowIndex, COL_STUDY_PATIENT_NAME, new QTableWidgetItem(QString::fromStdWString(dcmHeaderInfo.PatientName)));
			m_tableStudyList->setItem(rowIndex, COL_STUDY_PATIENT_ID, new QTableWidgetItem(QString::fromStdWString(dcmHeaderInfo.PatientID)));
			m_tableStudyList->setItem(rowIndex, COL_STUDY_SEX, new QTableWidgetItem(QString::fromStdWString(dcmHeaderInfo.PatientSex)));
			m_tableStudyList->setItem(rowIndex, COL_STUDY_AGE, new QTableWidgetItem(QString::fromStdWString(dcmHeaderInfo.PatientAge)));
			m_tableStudyList->setItem(rowIndex, COL_STUDY_IMPORTED_DATE, new QTableWidgetItem(QString::fromStdWString(importDataTime_Format)));

			//m_tableStudyList->setItem(rowIndex, COL_STUDY_IMAGE_COUNT, new QTableWidgetItem(QString::number(study.DicomSeriesCount())));
			QTableWidgetItem* itemImageCount = new QTableWidgetItem();
			itemImageCount->setData(Qt::EditRole, study.DicomSeriesCount());
			m_tableStudyList->setItem(rowIndex, COL_STUDY_IMAGE_COUNT, itemImageCount);
			m_tableStudyList->setItem(rowIndex, COL_STUDY_ACCESSION_NUMBER, new QTableWidgetItem(QString::fromStdWString(dcmHeaderInfo.AccessionNumber)));
			m_tableStudyList->setItem(rowIndex, COL_STUDY_EXAM_ID, new QTableWidgetItem(QString::fromStdWString(dcmHeaderInfo.ExamID)));
			m_tableStudyList->setItem(rowIndex, COL_STUDY_DESCRIPTION, new QTableWidgetItem(QString::fromStdWString(dcmHeaderInfo.StudyDescription)));

			/* Folder 경로 저장 */
			DicomDataset* pFirstSeriesDataset;
			if (study.GetFirstSeriesDataset(&pFirstSeriesDataset))
			{
				std::wstring dcmFilePath = pFirstSeriesDataset->GetValue_Extension(DCM_EXT_FILE_PATH);
				QFileInfo fileInfo(QString::fromStdWString(dcmFilePath));
				QDir dir = fileInfo.dir();
				dir.cdUp();
				QString studyFolderPath = dir.absolutePath();
				m_tableStudyList->setItem(rowIndex, COL_STUDY_DCM_FOLDER, new QTableWidgetItem(studyFolderPath));
			}
		}
	}

	bool PatientListWidget::SetTableSeriesRow(DicomInfomationModelSeriesObject& series, int rowIndex)
	{
		DicomDataset* pDataset;
		if (series.GetFirst(&pDataset) == false)
		{
			qWarning() << "fail to add table series row";
			return false;
		}

		DICOM_HEADER_INFO dcmHeaderInfo = pDataset->ToDcmHeaderInfo();
		QTableWidgetItem* item;
		item = new QTableWidgetItem();

		std::wstring dcmFilePath = pDataset->GetValue_Extension(DCM_EXT_FILE_PATH);
		std::wstring dcmDirectoryPath = pDataset->GetValue_Extension(DCM_EXT_DIRECTORY_PATH);
		std::wstring dcmFileNameList = pDataset->GetValue_Extension(DCM_EXT_FILE_LIST);

		m_tableSeriesList->setItem(rowIndex, COL_SERIES_UID, new QTableWidgetItem(QString::fromStdWString(dcmHeaderInfo.SeriesInstanceUID)));
		m_tableSeriesList->setItem(rowIndex, COL_SERIES_DATETIME, new QTableWidgetItem(QString::fromStdString(dcmHeaderInfo.SeriesDateTime.ToFormatText_DateTime())));

		item = new QTableWidgetItem();
		item->setData(Qt::EditRole, dcmHeaderInfo.SeriesNumber);
		m_tableSeriesList->setItem(rowIndex, COL_SERIES_NUMBER, item);

		m_tableSeriesList->setItem(rowIndex, COL_SERIES_MODALITY, new QTableWidgetItem(QString::fromStdWString(dcmHeaderInfo.Modality)));

		QTableWidgetItem* itemImageCount = new QTableWidgetItem();
		itemImageCount->setData(Qt::EditRole, series.DicomSeriesCount());
		m_tableSeriesList->setItem(rowIndex, COL_SERIES_IMAGE_COUNT, itemImageCount);

		m_tableSeriesList->setItem(rowIndex, COL_SERIES_DESCRIPTION, new QTableWidgetItem(QString::fromStdWString(dcmHeaderInfo.SeriesDescription)));
		m_tableSeriesList->setItem(rowIndex, COL_SERIES_DCM_FILEPATH, new QTableWidgetItem(QString::fromStdWString(dcmFilePath)));

		m_tableSeriesList->setItem(rowIndex, COL_SERIES_DCM_DIRECTORY_PATH, new QTableWidgetItem(QString::fromStdWString(dcmDirectoryPath)));
		m_tableSeriesList->setItem(rowIndex, COL_SERIES_DCM_FILE_LIST, new QTableWidgetItem(QString::fromStdWString(dcmFileNameList)));
		return true;
	}

	void PatientListWidget::UpdateByRuntimeMode()
	{
		bool isDebug = m_pContext->GetConfigManager()->GetAppConfig()->IsDebug();
		if (isDebug)
		{
			m_colHeaderList_Study.SetVisible(COL_STUDY_UID, true);
			m_colHeaderList_Study.SetVisible(COL_STUDY_DCM_FOLDER, true);
			m_colHeaderList_Series.SetVisible(COL_SERIES_UID, true);
			m_colHeaderList_Series.SetVisible(COL_SERIES_DCM_FILEPATH, true);
			m_colHeaderList_Series.SetVisible(COL_SERIES_DCM_DIRECTORY_PATH, true);
			m_colHeaderList_Series.SetVisible(COL_SERIES_DCM_FILE_LIST, true);

			m_findOptionList.SetVisible(ImportedDicomInfoDefines::COL_NAME_STUDY_INSTANCE_UID, true);
			m_findOptionList.SetVisible(ImportedDicomInfoDefines::COL_NAME_EXAM_ID, true);
		}
		else
		{
			m_colHeaderList_Study.SetVisible(COL_STUDY_UID, false);
			m_colHeaderList_Study.SetVisible(COL_STUDY_DCM_FOLDER, false);
			m_colHeaderList_Series.SetVisible(COL_SERIES_UID, false);
			m_colHeaderList_Series.SetVisible(COL_SERIES_DCM_FILEPATH, false);
			m_colHeaderList_Series.SetVisible(COL_SERIES_DCM_DIRECTORY_PATH, false);
			m_colHeaderList_Series.SetVisible(COL_SERIES_DCM_FILE_LIST, false);

			m_findOptionList.SetVisible(ImportedDicomInfoDefines::COL_NAME_STUDY_INSTANCE_UID, false);
			m_findOptionList.SetVisible(ImportedDicomInfoDefines::COL_NAME_EXAM_ID, false);
		}

		//=== Column Visible, Invisible
		//Study
		for (int idx : m_colHeaderList_Study.GetInvisibleHeaderIndexList())
		{
			m_tableStudyList->hideColumn(idx);
		}
		for (int idx : m_colHeaderList_Study.GetVisibleHeaderIndexList())
		{
			m_tableStudyList->showColumn(idx);
		}

		//Series
		for (int idx : m_colHeaderList_Series.GetInvisibleHeaderIndexList())
		{
			m_tableSeriesList->hideColumn(idx);
		}
		for (int idx : m_colHeaderList_Series.GetVisibleHeaderIndexList())
		{
			m_tableSeriesList->showColumn(idx);
		}

		m_tableStudyList->resizeColumnsToContents();
		m_tableSeriesList->resizeColumnsToContents();

		//=== ComboBox
		m_cboSearchMode->clear();
		for (auto& option : m_findOptionList.GetVisibleList())
		{
			QString name = QString::fromStdWString(option.Name);
			QVariant data = QString::fromStdWString(option.ID);
			m_cboSearchMode->addItem(name, data);
		}
	}

	void PatientListWidget::UpdateSeriesList(std::vector<DicomInfomationModelSeriesObject*>& seriesList)
	{
		m_tableSeriesList->setRowCount(0);
		m_tableSeriesList->setSortingEnabled(false);
		m_tableSeriesList->setRowCount(seriesList.size());

		/* Set Table Series Row가 성공했을 경우에만 Row Count 적용되도록 수정*/
		int rowIndex = 0;
		for (DicomInfomationModelSeriesObject* pSeries : seriesList)
		{
			if (SetTableSeriesRow(*pSeries, rowIndex))
			{
				rowIndex++;
			}
		}
		int rowCount = rowIndex;
		m_tableSeriesList->setRowCount(rowCount);

		m_tableSeriesList->resizeColumnsToContents();
		m_tableSeriesList->setSortingEnabled(true);

		if (m_tableSeriesList->rowCount() > 0)
		{
			m_tableSeriesList->setCurrentCell(0, COL_SERIES_DATETIME);
		}
	}

	void PatientListWidget::UpdateSeriesListByCurSelStudy()
	{
		int currentRow = m_tableStudyList->currentRow();
		if (currentRow >= 0)
		{
			std::wstring studyID = m_tableStudyList->item(currentRow, COL_STUDY_UID)->data(Qt::DisplayRole).toString().toStdWString();
			DicomInfomationModelStudyObject* pStudy = m_mainDicomInfoModel->GetStudy(studyID);
			if (pStudy)
			{
				UpdateSeriesList(pStudy->GetSeriesList());
			}
		}
	}

	void PatientListWidget::UpdatePreview()
	{
		int col = COL_SERIES_DCM_FILEPATH;
		int row = m_tableSeriesList->currentRow();
		if (row >= 0)
		{
			QString dcmFilePath = m_tableSeriesList->item(row, col)->text();
			QStringList filePathList;
			filePathList << dcmFilePath;
			//m_previewWidget->Load_Async(filePathList);

			/* 동기식 처리 */
			bool result = m_dcmLoader.Load(m_previewSeriesDicomInfoModel, filePathList);
			if (result)
			{
				m_previewWidget->SetDicomInfomationModel(m_previewSeriesDicomInfoModel);
			}
		}
		else
		{
			m_previewWidget->Clear();
		}
	}

	void PatientListWidget::SelectStudyList_And_SereisList(std::wstring studyInstanceUID, std::wstring seriesInstanceUID)
	{
		for (int row = 0; row < m_tableStudyList->rowCount(); ++row)
		{
			int col = COL_STUDY_UID;
			std::wstring curStudyInstanceUID = m_tableStudyList->item(row, col)->text().toStdWString();
			if (curStudyInstanceUID == studyInstanceUID)
			{
				m_tableStudyList->setCurrentCell(row, col);
				break;
			}
		}

		bool seriesSelected = false;
		for (int row = 0; row < m_tableSeriesList->rowCount(); ++row)
		{
			int col = COL_SERIES_UID;
			std::wstring curSeriesInstanceUID = m_tableSeriesList->item(row, col)->text().toStdWString();
			if (curSeriesInstanceUID == seriesInstanceUID)
			{
				m_tableSeriesList->setCurrentCell(row, col);
				seriesSelected = true;
				break;
			}
		}

		/*
			Series가 선택되지 않으면 UpdatePreview가 호출되지 않음
			화면 초기화를 위해 UpdatePreview 함수 호출 진행
		*/
		if (seriesSelected == false)
		{
			UpdatePreview();
		}
	}

	void PatientListWidget::OpenFile()
	{
		int col_dcmFilePath = COL_SERIES_DCM_FILEPATH;
		int col_dcmDirectoryPath = COL_SERIES_DCM_DIRECTORY_PATH;
		int col_dcmFileNameList = COL_SERIES_DCM_FILE_LIST;
		int row = m_tableSeriesList->currentRow();
		if (row >= 0)
		{
			/* File Open 명령 전송*/
			QString dcmFilePath = m_tableSeriesList->item(row, col_dcmFilePath)->text();
			QString dcmDirectoryPath = m_tableSeriesList->item(row, col_dcmDirectoryPath)->text();
			//QString openFilePath = dcmFilePath;
			QStringList openFilePathList;
			std::wstring openFileNameListJoinText = m_tableSeriesList->item(row, col_dcmFileNameList)->text().toStdWString();
			std::vector<std::wstring> openFileNameList = SplitDicomFileList(openFileNameListJoinText);
			for (std::wstring wname : openFileNameList)
			{
				openFilePathList << dcmDirectoryPath + "/" + QString::fromStdWString(wname);
			}

			/* DCM 파일을 임시 파일로 설정 */
			DicomDataset dcmDataset;
			if (dcmDataset.LoadFromFile(dcmFilePath) == false)
			{
				return;
			}

			DicomDataset::EType type = dcmDataset.GetDicomType();
			if (type == DicomDataset::EType::Image_16bit)
			{
				m_pContext->GetActionManager()->FileListOpen(openFilePathList);
			}
			else if (type == DicomDataset::EType::Image_8bit)
			{
				/* 이미지로 파일명 변환 */
				//QFileInfo fileInfo(dcmFilePath);
				//QString dirpath = fileInfo.absoluteDir().path();
				//QString openFilePath = dirpath + "/" + FileSystemUtil::RemoveExtension(fileInfo.fileName()) + ".png";
				//DicomConverter::ConvertDcmFile_To_ImgFile(
				//	dcmFilePath.toStdWString(),
				//	openFilePath.toStdWString()
				//);
				//m_pContext->GetActionManager()->FileOpen(openFilePath);
				m_pContext->GetActionManager()->FileListOpen(openFilePathList);

			}
			else if (type == DicomDataset::EType::EncapsulatedPDFStorage)
			{
				QFileInfo fileInfo(dcmFilePath);
				QString dirpath = fileInfo.absoluteDir().path();
				QString openFilePath = dirpath + "/" + FileSystemUtil::RemoveExtension(fileInfo.fileName()) + ".pdf";
				if (DicomConverter::ConvertDcmFile_To_PDFFile(
					dcmFilePath.toStdWString(),
					openFilePath.toStdWString()))
				{
					/* PDF 파일 실행 */
					QDesktopServices::openUrl(QUrl::fromLocalFile(openFilePath));
				}
			}
		}
	}

	void fm::PatientListWidget::CopyToClipboard_Table(QTableWidget* widget)
	{
		QClipboard* clipboard = QApplication::clipboard();

		int colCount = widget->columnCount();
		int rowCount = widget->rowCount();

		QString text;
		int row = 0;
		int col = 0;
		QString data;
		QTextStream stream(&text);
		for (row = 0; row < rowCount; ++row)
		{
			for (col = 0; col < colCount - 1; ++col)
			{
				data = widget->item(row, col)->text();
				stream << data << "\t";
			}

			data = widget->item(row, col)->text();
			stream << data << endl;
		}
		clipboard->setText(text);
	}

	std::vector<DicomInfomationModelStudyObject*> PatientListWidget::GetSelectedStudyList()
	{
		std::vector<DicomInfomationModelStudyObject*> studyList;
		QList<QTableWidgetSelectionRange> selectedRangeList = m_tableStudyList->selectedRanges();

		if (selectedRangeList.isEmpty())
		{
			return studyList;
		}

		int startRow = selectedRangeList[0].topRow();
		int endRow = selectedRangeList[0].bottomRow();
		//int row = selectedRangeList[0].rowCount();
		for (int row = startRow; row <= endRow; ++row)
		{
			int col = COL_STUDY_UID;

			std::wstring studyID = m_tableStudyList->item(row, col)->data(Qt::DisplayRole).toString().toStdWString();
			DicomInfomationModelStudyObject* pStudy = m_mainDicomInfoModel->GetStudy(studyID);
			studyList.push_back(pStudy);
		}
		return studyList;
	}

	void PatientListWidget::onImportDICOM()
	{
		QString dirPath = QFileDialog::getExistingDirectory(
			this,
			"DICOM Directory open");

		if (dirPath.isEmpty())
		{
			return;
		}

		QStringList filePathList;
		filePathList << dirPath;
		m_pContext->GetActionManager()->PatientList_UploadDicomFileList(filePathList);
	}

	void PatientListWidget::onImportFromPACS()
	{
		//PACSDownloadDialog dlg;
		//int result = dlg.exec();
		//if (result == QDialog::Accepted)
		//{
		//	QString filepath = dlg.LastDownloadFilePath();
		//	g_Root.GetAppManager()->ImportDicomFileList(filepath);
		//}
	}

	void PatientListWidget::onSearch()
	{
		/*
			Search 인터페이스에 대해서 검토한다.
		*/
		ImportedDicomInfoDAO* pImportDcmDAO = m_pContext->GetDataManager()->GetImportedDicomInfoDAO();
		std::vector<ImportedDicomInfoDTO> importedDcmInfos;

		FindCondition cond;
		std::wstring key = m_cboSearchMode->currentData().toString().toStdWString();
		std::wstring data = m_editSearch->text().toStdWString();

		cond.Add(key, data);

		if (pImportDcmDAO->Find(importedDcmInfos, cond) == false)
		{
			//None Searched
		}

		ResetDicomDatasetModel_As_DTODataList(importedDcmInfos);
	}

	void PatientListWidget::onChkPreview()
	{
		if (m_chkPreview->isChecked())
		{
			m_previewWidget->setVisible(true);
		}
		else
		{
			m_previewWidget->setVisible(false);
		}
	}

	void PatientListWidget::onSeriesListDblClick(const QModelIndex& index)
	{
		OpenFile();
	}

	void PatientListWidget::onSeriesListCurrentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
	{
		if (currentRow != previousRow)
		{
			UpdatePreview();
		}
	}

	void PatientListWidget::onStudyListItemChanged(QTableWidgetItem* item)
	{
	}

	void PatientListWidget::onStudyListCurrentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
	{
		if (currentRow != previousRow)
		{
			UpdateSeriesListByCurSelStudy();
		}
	}

	void PatientListWidget::onSettingChanged()
	{
		UpdateByRuntimeMode();
	}

	void PatientListWidget::onDicomListAdded(std::vector<ImportedDicomInfoDTO> dtoList)
	{
		if (!dtoList.empty())
		{
			RefreshModel(dtoList.back().StudyInstanceUID, dtoList.back().SeriesInstanceUID);
			//ResetStudyListByDTODataList(dtoList, dtoList.back().StudyInstanceUID, dtoList.back().SeriesInstanceUID);
		}
	}

	void PatientListWidget::onDicomListModified(std::vector<ImportedDicomInfoDTO> dtoList)
	{
		if (!dtoList.empty())
		{
			RefreshModel(dtoList.back().StudyInstanceUID, dtoList.back().SeriesInstanceUID);
			//ResetStudyListByDTODataList(dtoList, dtoList.back().StudyInstanceUID, dtoList.back().SeriesInstanceUID);
		}
	}

	void PatientListWidget::onActDicomEdit_Study(bool checked)
	{
		std::vector<DicomInfomationModelStudyObject*> studyList = GetSelectedStudyList();

		DicomEditDialog dlg(this, studyList);
		int result = dlg.exec();
		if (result == QDialog::Accepted)
		{
			DicomDataset editDicomDataset_Study = dlg.GetEdittedDicomDataset_Study();

			/* 각 Series별 Dicom이 저장된 Directory를 가져온다. */
			QStringList dirpathList;
			for (auto& pStudy : studyList)
			{
				std::vector<DicomInfomationModelSeriesObject*> seriesList = pStudy->GetSeriesList();
				for (auto& pSeries : seriesList)
				{
					DicomDataset* pDataset;
					if (pSeries->GetFirst(&pDataset))
					{
						QString filepath = QString::fromStdWString(pDataset->GetFilePath_Extension());
						QFileInfo fileInfo(filepath);
						if (fileInfo.isDir())
						{
							dirpathList << filepath;
						}
						else if (fileInfo.isFile())
						{
							QString dirpath = fileInfo.dir().absolutePath();
							//QDir dir(filepath);
							dirpathList << dirpath;
						}
					}
				}
			}

			m_pContext->GetActionManager()->PatientList_EditDicomFileList(dirpathList, editDicomDataset_Study);
		}
	}

	void PatientListWidget::onActOpenContainingFolder_Study(bool checked)
	{
		int col = COL_STUDY_DCM_FOLDER;
		int row = m_tableStudyList->currentRow();
		if (row >= 0)
		{
			QString dirpath = m_tableStudyList->item(row, col)->text();
			QDesktopServices::openUrl(QUrl::fromLocalFile(dirpath));
		}
	}

	void PatientListWidget::onActDelete_Study(bool checked)
	{
		std::vector<DicomInfomationModelStudyObject*> selectedStudyList = GetSelectedStudyList();
		if (!selectedStudyList.empty())
		{
			QMessageBox::StandardButton result = QMessageBox::warning(
				this,
				StringManager::GetString(STR_TITLE_QUESTION),
				StringManager::GetString(STR_QUESTION_DELETE_PATIENT_LIST),
				QMessageBox::StandardButton::Ok | QMessageBox::StandardButton::Cancel
			);

			if (result == QMessageBox::StandardButton::Ok)
			{
				/*
					하나씩 처리할 때 병목 증상 발생.
					한번에 Delete 하는 기능 추가하여 성능개선 할 것.
				*/
				for (auto& pStudy : selectedStudyList)
				{
					DicomDataset* pDataset;
					if (pStudy->GetStudyDataset(&pDataset))
					{
						std::wstring studyInstanceUID = pDataset->GetValueWString(DicomTagID::StudyInstanceUID);
						ImportedDicomInfoDAO* pImportDcmDAO = m_pContext->GetDataManager()->GetImportedDicomInfoDAO();
						FindCondition cond;
						cond.Add(ImportedDicomInfoDefines::COL_NAME_STUDY_INSTANCE_UID, studyInstanceUID);
						pImportDcmDAO->Delete(cond, nullptr);
					}
				}

				RefreshModel();
			}
		}
	}

	void PatientListWidget::onActExport_Study(bool checked)
	{

	}

	void fm::PatientListWidget::onActCopyAllToClipboard_Study(bool checked)
	{
		CopyToClipboard_Table(m_tableStudyList);
	}

	void PatientListWidget::onActOpenFile(bool checked)
	{
		OpenFile();
	}

	void PatientListWidget::onActOpenContainingFolder_Series(bool checked)
	{
		int col = COL_SERIES_DCM_FILEPATH;
		int row = m_tableSeriesList->currentRow();
		if (row >= 0)
		{
			QString filepath = m_tableSeriesList->item(row, col)->text();
			QFileInfo fileInfo(filepath);
			QString dirpath = fileInfo.dir().absolutePath();

			QDesktopServices::openUrl(QUrl::fromLocalFile(dirpath));
		}
	}

	void PatientListWidget::onActCopyFolder_Series(bool checked)
	{
		int col = COL_SERIES_DCM_FILEPATH;
		int row = m_tableSeriesList->currentRow();
		if (row >= 0)
		{
			/* Action으로 이관하여 비동기 처리 */
			QString filepath = m_tableSeriesList->item(row, col)->text();
			QFileInfo fileInfo(filepath);
			QString srcDirpath = fileInfo.dir().absolutePath();

			QString targetDirpath = QFileDialog::getExistingDirectory(
				this,
				"Copy"
			);

			if (!targetDirpath.isEmpty())
			{
				m_pContext->GetActionManager()->PatientList_FileCopy(srcDirpath, targetDirpath);
			}
		}
	}

	void PatientListWidget::onActDelete_Series(bool checked)
	{
		int studyRow = m_tableStudyList->currentRow();
		int seriesRow = m_tableSeriesList->currentRow();
		if (seriesRow >= 0 && studyRow >= 0)
		{
			/* Action으로 이관하여 비동기 처리 */
			QMessageBox::StandardButton result = QMessageBox::warning(this,
				StringManager::GetString(STR_TITLE_QUESTION),
				StringManager::GetString(STR_QUESTION_DELETE_PATIENT_LIST),
				QMessageBox::StandardButton::Ok | QMessageBox::StandardButton::Cancel
			);

			if (result == QMessageBox::StandardButton::Ok)
			{
				QString filepath = m_tableSeriesList->item(seriesRow, COL_SERIES_DCM_FILEPATH)->text();
				QString studyInstanceUID = m_tableStudyList->item(studyRow, COL_STUDY_UID)->text();
				QString seriesInstanceUID = m_tableSeriesList->item(seriesRow, COL_SERIES_UID)->text();
				ImportedDicomInfoDAO* pImportDcmDAO = m_pContext->GetDataManager()->GetImportedDicomInfoDAO();
				FindCondition cond;
				cond.Add(ImportedDicomInfoDefines::COL_NAME_DICOM_FILE_PATH, filepath.toStdWString());
				if (pImportDcmDAO->Delete(cond, nullptr))
				{
					RefreshModel(studyInstanceUID.toStdWString());
				}
			}
		}
	}

	void PatientListWidget::onActCopyAllToClipboard_Series(bool checked)
	{
		CopyToClipboard_Table(m_tableSeriesList);
	}

	void PatientListWidget::onPatientListDicomUploadStarted()
	{
		ProgressBarDialog::Global_SetMode(ProgressBarDialog::busy_indicator);
		ProgressBarDialog::Global_SetProgress(0);
		ProgressBarDialog::Global_SetText("Calculate Dicom File List...");
		ProgressBarDialog::Global_Show();
		setEnabled(false);
	}

	void PatientListWidget::onPatientListDicomUploadInProgress(int progressCount, int maxCount)
	{
		float rate = (float)progressCount / (float)maxCount;
		ProgressBarDialog::Global_SetMode(ProgressBarDialog::normal);
		ProgressBarDialog::Global_SetText(QString("Dicom file load is in progress(%1/%2)").arg(progressCount).arg(maxCount));
		ProgressBarDialog::Global_SetProgress(rate);
	}

	void PatientListWidget::onPatientListDicomUploadFinished()
	{
		ProgressBarDialog::Global_SetMode(ProgressBarDialog::busy_indicator);
		ProgressBarDialog::Global_SetText("Save To Database List...");
		ProgressBarDialog::Global_Hide();
		setEnabled(true);
	}

	void PatientListWidget::onPatientListDicomEditStarted()
	{
		ProgressBarDialog::Global_SetMode(ProgressBarDialog::busy_indicator);
		ProgressBarDialog::Global_SetProgress(0);
		ProgressBarDialog::Global_SetText("Calculate Dicom File List...");
		ProgressBarDialog::Global_Show();
		setEnabled(false);
	}

	void PatientListWidget::onPatientListDicomEditInProgress(int progressCount, int maxCount)
	{
		float rate = (float)progressCount / (float)maxCount;
		ProgressBarDialog::Global_SetMode(ProgressBarDialog::normal);
		ProgressBarDialog::Global_SetText(QString("Dicom file load is in progress(%1/%2)").arg(progressCount).arg(maxCount));
		ProgressBarDialog::Global_SetProgress(rate);
	}

	void PatientListWidget::onPatientListDicomEditFinished()
	{
		ProgressBarDialog::Global_SetMode(ProgressBarDialog::busy_indicator);
		ProgressBarDialog::Global_SetText("Save To Database List...");
		ProgressBarDialog::Global_Hide();
		setEnabled(true);
	}
}

