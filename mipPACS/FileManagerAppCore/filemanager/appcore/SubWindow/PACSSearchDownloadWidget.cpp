#include "stdafx.h"
#include "PACSSearchDownloadWidget.h"
#include <FileManager/appcore/Dialog/Util/ProgressBarDialog.h>
#include <FileManager/dicom/Network/DicomListenerInfo.h>
#include <FileManager/dicom/Network/DicomHostInfo.h>
#include <FileManager/dicom/Convert/DicomConverter.h>
#include <FileManager/dicom/dicom_defines.h>
#include <filemanager/appcore/AppCoreContext.h>
#include <filemanager/appcore/Event/EventManager.h>
#include <filemanager/appcore/UI/PACSListWidget.h>
#include <filemanager/dicom/Network/DicomFindOption.h>
#include <qfiledialog>
#include <qmessagebox>
#include <qstandarditemmodel>
#include <qmouseevent>
#include <qaction>

namespace fm
{
	static DateRangeInfo DATE_RANGE_INFO_LIST[] =
	{
		{ EDateRangeMode::All_Date, "All Date"},
		{ EDateRangeMode::Today, "Today"},
		{ EDateRangeMode::YesterDay, "YesterDay"},
		{ EDateRangeMode::LastWeek, "Last Week"},
		{ EDateRangeMode::LastMonth, "Last Month"},
		{ EDateRangeMode::LastYear, "Lasy Year"},
		{ EDateRangeMode::CustomDate, "Custom date"},
	};

	//======================================
	//		PACSSearchDownloadWidget
	//======================================
	PACSSearchDownloadWidget::PACSSearchDownloadWidget(QWidget* parent) :
		QWidget(parent),
		m_mode(EPACSOperationMode::only_search_mode),
		m_searchMode(EPACSSearchMode::search_series),
		m_dcmInfoModel(true)
	{
		setupUi(this);

		m_colHeaderList_Study.Add(COL_STUDY_UID, "Study UID", true);
		m_colHeaderList_Study.Add(COL_STUDY_DATETIME, "Date", true);
		m_colHeaderList_Study.Add(COL_STUDY_PATIENT_NAME, "Patient Name", true);
		m_colHeaderList_Study.Add(COL_STUDY_BIRTH_DATE, "Birth Date", true);
		m_colHeaderList_Study.Add(COL_STUDY_AGE, "Age", true);
		m_colHeaderList_Study.Add(COL_STUDY_SEX, "Sex", true);
		m_colHeaderList_Study.Add(COL_STUDY_PATIENT_ID, "Patient ID", true);
		m_colHeaderList_Study.Add(COL_STUDY_DESCRIPTION, "Description", true);
		m_colHeaderList_Study.Add(COL_STUDY_ACCESSION_NUMBER, "Accesstion Number", true);
		m_colHeaderList_Study.Add(COL_STUDY_EXAM_ID, "Exam ID", true);
		m_colHeaderList_Study.Add(COL_STUDY_REFERRING_PHYSICIAN, "Referring Physician", true);
		m_colHeaderList_Study.Add(COL_STUDY_PERFORMING_PHYSICIAN, "Performing physician", true);
		m_colHeaderList_Study.Add(COL_STUDY_READING_PHYSICIAN, "Reading physician", true);
		m_colHeaderList_Study.Add(COL_STUDY_INSTITUTION_NAME, "Institution name", true);
		m_colHeaderList_Study.Add(COL_STUDY_MANUFACTURER, "Manufacturer", true);
		m_colHeaderList_Study.Add(COL_STUDY_MANUFACTURER_MODEL_NAME, "Manufacturer Model", true);
		m_colHeaderList_Study.Add(COL_STUDY_IMAGE_COUNT, "Count", true);
		m_colHeaderList_Study.Add(COL_STUDY_INSTANCE_COUNT, "Image(s)", true);
		m_colHeaderList_Study.Add(COL_STUDY_PATIENT_RELATED_STUDY_COUNT, "Patient Study Image(s)", true);
		m_colHeaderList_Study.Add(COL_STUDY_PATIENT_RELATED_SERIES_COUNT, "Patient Series Image(s)", true);
		m_colHeaderList_Study.Add(COL_STUDY_PATIENT_RELATED_INSTANCE_COUNT, "Patient Instance Image(s)", true);
		m_colHeaderList_Study.Add(COL_STUDY_RELATED_SERIES_COUNT, "Study Series Image(s)", true);
		m_colHeaderList_Study.Add(COL_STUDY_SERIES_COUNT, "Series(s)", true);

		m_colHeaderList_Series.Add(COL_SERIES_UID, "Series UID", true);
		m_colHeaderList_Series.Add(COL_SERIES_DATETIME, "Series Date", true);
		m_colHeaderList_Series.Add(COL_SERIES_NUMBER, "Number", true);
		m_colHeaderList_Series.Add(COL_SERIES_MODALITY, "Modality", true);
		m_colHeaderList_Series.Add(COL_SERIES_DESCRIPTION, "Description", true);
		m_colHeaderList_Series.Add(COL_SERIES_IMAGE_COUNT, "Count", true);
		m_colHeaderList_Series.Add(COL_SERIES_INSTANCE_COUNT, "Image(s)", true);

		m_actDownloadStudy = new QAction("Download");
		m_actSaveFileStudy = new QAction("Save");

		m_actDownloadSeries = new QAction("Download");
		m_actSaveFileSeries = new QAction("Save");

		connect(m_btnSearch, &QPushButton::clicked, this, &PACSSearchDownloadWidget::onSearch);
		connect(m_editSearch, &QLineEdit::returnPressed, this, &PACSSearchDownloadWidget::onSearch);

		connect(m_btnDownload, &QPushButton::clicked, this, &PACSSearchDownloadWidget::onDownload);

		connect(m_tableSearchStudyList, &QTableWidget::doubleClicked,
			this, &PACSSearchDownloadWidget::onStudySearchListDoubleClicked);

		connect(m_tableSearchStudyList, &QTableWidget::currentItemChanged,
			this, &PACSSearchDownloadWidget::onStudySearchListItemChanged);

		connect(m_tableSearchSeriesList, &QTableWidget::doubleClicked,
			this, &PACSSearchDownloadWidget::onSeriesSearchListDoubleClicked);

		connect(m_tableSearchSeriesList, &QTableWidget::currentItemChanged,
			this, &PACSSearchDownloadWidget::onSeriesSearchListItemChanged);

		connect(m_cboDateRangeMode, SIGNAL(currentIndexChanged(int)), this, SLOT(onDateRangeModeChanged(int)));

		connect(m_btnDebugExportData, &QPushButton::clicked, this, &PACSSearchDownloadWidget::onBtnDebugExportData);
		connect(m_btnDebugCompare, &QPushButton::clicked, this, &PACSSearchDownloadWidget::onBtnDebugCompare);
		connect(m_btnDebugClear, &QPushButton::clicked, this, &PACSSearchDownloadWidget::onBtnDebugClear);

		connect(m_actDownloadStudy, &QAction::triggered, this, &PACSSearchDownloadWidget::onActDownloadStudy);
		connect(m_actSaveFileStudy, &QAction::triggered, this, &PACSSearchDownloadWidget::onActSaveFileStudy);

		connect(m_actDownloadSeries, &QAction::triggered, this, &PACSSearchDownloadWidget::onActDownloadSeries);
		connect(m_actSaveFileSeries, &QAction::triggered, this, &PACSSearchDownloadWidget::onActSaveFileSeries);

		connect(m_btnRefreshDownloadServer, &QPushButton::clicked, this, &PACSSearchDownloadWidget::onRefreshDownloadServer);
		connect(m_PACSListWidget, &PACSListWidget::selectedPACSChanged, this, &PACSSearchDownloadWidget::onPACSSelectionChanged);

		connect(&g_EventManager, &EventManager::dcmNet_Find_Finished, this, &PACSSearchDownloadWidget::onDcmNetFindFinished);
		connect(&g_EventManager, &EventManager::dcmNet_SCPServerStartFailed, this, &PACSSearchDownloadWidget::onDcmNetSCPServerStartFailed);
		connect(&g_EventManager, &EventManager::dcmNet_SCPServerStatusChanged, this, &PACSSearchDownloadWidget::onDcmNetSCPServerStatusChanged);

		connect(&g_EventManager, &EventManager::settingChanged, this, &PACSSearchDownloadWidget::onSettingChanged);

		/* Study Mode를 기본으로 준비 */
		m_rdoDebugQRStudyMode->setChecked(true);

	}

	PACSSearchDownloadWidget::~PACSSearchDownloadWidget()
	{
	}

	void PACSSearchDownloadWidget::Init(AppCoreContext* pContext, EPACSOperationMode mode, EPACSSearchMode searchMode)
	{
		m_pContext = pContext;
		m_mode = mode;
		m_searchMode = searchMode;

		m_pDcmNetworkManager = m_pContext->GetDicomNetworkManager();

		m_PACSListWidget->Init(m_pContext);

		InitStudyListTable();
		InitSeriesListTable();

		UpdateComboBox_SearchMode();
		UpdateComboBox_Modality();
		UpdateComboBox_SearchDateMode();

		UpdateControl();
		UpdateByRuntimeMode();

		SetMode(mode, searchMode);

		InitLayout();
	}

	void PACSSearchDownloadWidget::SetMode(EPACSOperationMode mode, EPACSSearchMode searchMode)
	{
		m_mode = mode;
		m_searchMode = searchMode;
		if (m_mode == EPACSOperationMode::only_search_mode)
		{
			m_btnDownload->setVisible(false);
		}
		else if (m_mode == EPACSOperationMode::can_download_mode)
		{
			m_btnDownload->setVisible(true);
		}

		if (m_searchMode == EPACSSearchMode::search_series)
		{
			m_tableSearchSeriesList->setVisible(true);
		}
		else if (m_searchMode == EPACSSearchMode::search_study)
		{
			m_tableSearchSeriesList->setVisible(false);
		}
	}

	void PACSSearchDownloadWidget::Refresh()
	{
		m_pContext->GetActionManager()->DicomSCPServerStart();
	}

	void PACSSearchDownloadWidget::UpdateControl()
	{
		if (m_pDcmNetworkManager->IsHostInfoEnabled() == false)
		{
			m_btnSearch->setEnabled(false);
			m_btnDownload->setEnabled(false);
			m_editSearch->setEnabled(false);
			m_cboDateRangeMode->setEnabled(false);
			m_cboModality->setEnabled(false);
			m_cboSearchMode->setEnabled(false);
		}
		else
		{
			m_btnSearch->setEnabled(true);
			m_btnDownload->setEnabled(true);
			m_editSearch->setEnabled(true);
			m_cboDateRangeMode->setEnabled(true);
			m_cboModality->setEnabled(true);
			m_cboSearchMode->setEnabled(true);

			if (m_pDcmNetworkManager->IsStoreSCPInitialized() == false)
			{
				m_btnDownload->setEnabled(false);
			}

			if (
				(m_tableSearchStudyList->currentRow() < 0)
				)
			{
				m_btnDownload->setEnabled(false);
			}
		}

		UpdateByRuntimeMode();
	}

	void PACSSearchDownloadWidget::Reset()
	{
		m_tableSearchStudyList->setRowCount(0);
		m_tableSearchSeriesList->setRowCount(0);

		UpdateControl();
	}

	void PACSSearchDownloadWidget::SaveStatus()
	{
		GUISettingConfig* pConfig = m_pContext->GetConfigManager()->GetGUISettingConfig();
		PACSSearchDownloadWidgetInfo info = pConfig->GetPACSSearchDownloadWidget();
		info.CenterHorizontalContainerSplitterStatus = m_splitter->saveState();

		pConfig->SetPACSSearchDownloadWidget(info);
		pConfig->Save();
	}

	void PACSSearchDownloadWidget::InitLayout()
	{
		GUISettingConfig* pConfig = m_pContext->GetConfigManager()->GetGUISettingConfig();
		PACSSearchDownloadWidgetInfo info = pConfig->GetPACSSearchDownloadWidget();
		
		m_splitter->restoreState(info.CenterHorizontalContainerSplitterStatus);
	}

	void PACSSearchDownloadWidget::InitStudyListTable()
	{
		m_tableSearchStudyList->setColumnCount(m_colHeaderList_Study.Count());
		m_tableSearchStudyList->setHorizontalHeaderLabels(m_colHeaderList_Study.GetNameHeaderList());
		m_tableSearchStudyList->setEditTriggers(QAbstractItemView::NoEditTriggers);
		m_tableSearchStudyList->setSelectionMode(QAbstractItemView::SingleSelection);
		m_tableSearchStudyList->setSelectionBehavior(QAbstractItemView::SelectRows);
		m_tableSearchStudyList->horizontalHeader()->setHighlightSections(false);
		m_tableSearchStudyList->setAlternatingRowColors(true);

		m_tableSearchStudyList->installEventFilter(this);

		m_tableSearchStudyList->setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);
		m_tableSearchStudyList->addAction(m_actDownloadStudy);
		m_tableSearchStudyList->addAction(m_actSaveFileStudy);
	}

	void PACSSearchDownloadWidget::InitSeriesListTable()
	{
		m_tableSearchSeriesList->setColumnCount(m_colHeaderList_Series.Count());
		m_tableSearchSeriesList->setHorizontalHeaderLabels(m_colHeaderList_Series.GetNameHeaderList());
		m_tableSearchSeriesList->setEditTriggers(QAbstractItemView::NoEditTriggers);
		m_tableSearchSeriesList->setSelectionMode(QAbstractItemView::SingleSelection);
		m_tableSearchSeriesList->setSelectionBehavior(QAbstractItemView::SelectRows);
		m_tableSearchSeriesList->horizontalHeader()->setHighlightSections(false);
		m_tableSearchSeriesList->setAlternatingRowColors(true);

		m_tableSearchSeriesList->installEventFilter(this);

		m_tableSearchSeriesList->setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);
		m_tableSearchSeriesList->addAction(m_actDownloadSeries);
		m_tableSearchSeriesList->addAction(m_actSaveFileSeries);
	}

	void PACSSearchDownloadWidget::UpdateComboBox_SearchMode()
	{
		for (int i = 0; i < (int)EDcmFindMode::MAXCOUNT; ++i)
		{
			m_cboSearchMode->addItem(EDcmFindMode_Text(i).c_str(), i);
		}
	}

	void PACSSearchDownloadWidget::UpdateComboBox_Modality()
	{
		for (int i = 0; i < (int)EDcmModality::MAXCOUNT; ++i)
		{
			m_cboModality->addItem(EDcmModality_Text(i).c_str(), i);
		}
	}

	void PACSSearchDownloadWidget::UpdateComboBox_SearchDateMode()
	{
		for (int i = 0; i < _countof(DATE_RANGE_INFO_LIST); ++i)
		{
			m_cboDateRangeMode->addItem(DATE_RANGE_INFO_LIST[i].Text, DATE_RANGE_INFO_LIST[i].Mode);
		}
	}

	EDateRangeMode PACSSearchDownloadWidget::CurrentDateRangeMode()
	{
		return (EDateRangeMode)m_cboDateRangeMode->currentData().toInt();
	}

	void PACSSearchDownloadWidget::UpdateDateRange()
	{
		EDateRangeMode mode = (EDateRangeMode)m_cboDateRangeMode->currentData().toInt();
		if (mode == EDateRangeMode::CustomDate)
		{
			m_dateEditStart->setEnabled(true);
			m_dateEditEnd->setEnabled(true);
		}
		else
		{
			m_dateEditStart->setEnabled(false);
			m_dateEditEnd->setEnabled(false);
		}

		QDateTime startDate = QDateTime::currentDateTime();
		QDateTime endDate = QDateTime::currentDateTime();

		switch (mode)
		{
		case EDateRangeMode::All_Date:
			startDate = QDateTime::fromString("19000101", "yyyyMMdd");
			endDate = QDateTime::fromString("21001231", "yyyyMMdd");
			break;
		case EDateRangeMode::Today:
		case EDateRangeMode::CustomDate:
			break;
		case EDateRangeMode::YesterDay:
			startDate = startDate.addDays(-1);
			break;
		case EDateRangeMode::LastWeek:
			startDate = startDate.addDays(-7);
			break;
		case EDateRangeMode::LastMonth:
			startDate = startDate.addMonths(-1);
			break;
		case EDateRangeMode::LastYear:
			startDate = startDate.addYears(-1);
			break;
		default:
			break;
		}

		m_dateEditStart->setDateTime(startDate);
		m_dateEditEnd->setDateTime(endDate);
	}

	void PACSSearchDownloadWidget::UpdateStudyList(const std::vector<DicomInfomationModelStudyObject*>& studyList)
	{
		DicomDataset* pDcmDataset;

		/* Event 처리를 위해 Row Count - 0으로 초기화 필요 */
		m_tableSearchStudyList->setRowCount(0);

		int newRowCount = studyList.size();
		m_tableSearchStudyList->setRowCount(newRowCount);
		m_tableSearchStudyList->setSortingEnabled(false);

		int rowIdx = 0;
		for (auto it = studyList.begin(); it != studyList.end(); ++it)
		{
			DicomDataset* pDcmDataset;
			if ((*it)->GetStudyDataset(&pDcmDataset))
			{
				DICOM_HEADER_INFO dcmHeaderInfo = pDcmDataset->ToDcmHeaderInfo();

				m_tableSearchStudyList->setItem(rowIdx, COL_STUDY_UID, new QTableWidgetItem(QString::fromStdWString(dcmHeaderInfo.StudyInstanceUID)));
				m_tableSearchStudyList->setItem(rowIdx, COL_STUDY_DATETIME, new QTableWidgetItem(QString::fromStdString(dcmHeaderInfo.StudyDateTime.ToFormatText_DateTime())));
				m_tableSearchStudyList->setItem(rowIdx, COL_STUDY_PATIENT_NAME, new QTableWidgetItem(QString::fromStdWString(dcmHeaderInfo.PatientName)));
				m_tableSearchStudyList->setItem(rowIdx, COL_STUDY_BIRTH_DATE, new QTableWidgetItem(QString::fromStdString(dcmHeaderInfo.DateOfBirth.ToFormatText_DateTime())));
				m_tableSearchStudyList->setItem(rowIdx, COL_STUDY_AGE, new QTableWidgetItem(QString::fromStdWString(dcmHeaderInfo.PatientAge)));
				m_tableSearchStudyList->setItem(rowIdx, COL_STUDY_SEX, new QTableWidgetItem(QString::fromStdWString(dcmHeaderInfo.PatientSex)));
				m_tableSearchStudyList->setItem(rowIdx, COL_STUDY_BIRTH_DATE, new QTableWidgetItem(QString::fromStdString(dcmHeaderInfo.DateOfBirth.ToFormatText_DateTime())));
				m_tableSearchStudyList->setItem(rowIdx, COL_STUDY_PATIENT_ID, new QTableWidgetItem(QString::fromStdWString(dcmHeaderInfo.PatientID)));
				m_tableSearchStudyList->setItem(rowIdx, COL_STUDY_DESCRIPTION, new QTableWidgetItem(QString::fromStdWString(dcmHeaderInfo.StudyDescription)));
				m_tableSearchStudyList->setItem(rowIdx, COL_STUDY_ACCESSION_NUMBER, new QTableWidgetItem(QString::fromStdWString(dcmHeaderInfo.AccessionNumber)));
				m_tableSearchStudyList->setItem(rowIdx, COL_STUDY_EXAM_ID, new QTableWidgetItem(QString::fromStdWString(dcmHeaderInfo.ExamID)));
				m_tableSearchStudyList->setItem(rowIdx, COL_STUDY_REFERRING_PHYSICIAN, new QTableWidgetItem(QString::fromStdWString(dcmHeaderInfo.ReferringPhysician)));
				m_tableSearchStudyList->setItem(rowIdx, COL_STUDY_PERFORMING_PHYSICIAN, new QTableWidgetItem(QString::fromStdWString(dcmHeaderInfo.PerformingPhysician)));
				m_tableSearchStudyList->setItem(rowIdx, COL_STUDY_READING_PHYSICIAN, new QTableWidgetItem(QString::fromStdWString(dcmHeaderInfo.RequestingPhysician)));
				m_tableSearchStudyList->setItem(rowIdx, COL_STUDY_INSTITUTION_NAME, new QTableWidgetItem(QString::fromStdWString(dcmHeaderInfo.InstitutionName)));
				m_tableSearchStudyList->setItem(rowIdx, COL_STUDY_MANUFACTURER, new QTableWidgetItem(QString::fromStdWString(dcmHeaderInfo.Manufacturer)));
				m_tableSearchStudyList->setItem(rowIdx, COL_STUDY_MANUFACTURER_MODEL_NAME, new QTableWidgetItem(QString::fromStdWString(dcmHeaderInfo.ManufacturerModelName)));

				/* PACS 검색 결과는 DefaultImageCount로 설정한다. */
				QTableWidgetItem* item;
				item = new QTableWidgetItem();
				item->setData(Qt::EditRole, dcmHeaderInfo.NumberOfStudyRelatedInstances);
				m_tableSearchStudyList->setItem(rowIdx, COL_STUDY_IMAGE_COUNT, item);

				//item->setData(Qt::EditRole, dcmHeaderInfo.ImagesInAcqusition);
				//m_tableSearchStudyList->setItem(rowIdx, COL_STUDY_IMAGE_COUNT, item);

				//qInfo() << "retired result : " <<
				//	dcmHeaderInfo.RETIRED_AcquisitionsInSeries <<
				//	dcmHeaderInfo.RETIRED_ImagesInSeries <<
				//	dcmHeaderInfo.RETIRED_AcquisitionsInStudy <<
				//	dcmHeaderInfo.RETIRED_ImagesInStudy;

				item = new QTableWidgetItem();
				item->setData(Qt::EditRole, dcmHeaderInfo.InstanceNumber);
				m_tableSearchStudyList->setItem(rowIdx, COL_STUDY_INSTANCE_COUNT, item);

				item = new QTableWidgetItem();
				item->setData(Qt::EditRole, dcmHeaderInfo.NumberOfPatientRelatedStudies);
				m_tableSearchStudyList->setItem(rowIdx, COL_STUDY_PATIENT_RELATED_STUDY_COUNT, item);

				item = new QTableWidgetItem();
				item->setData(Qt::EditRole, dcmHeaderInfo.NumberOfPatientRelatedSeries);
				m_tableSearchStudyList->setItem(rowIdx, COL_STUDY_PATIENT_RELATED_SERIES_COUNT, item);

				item = new QTableWidgetItem();
				item->setData(Qt::EditRole, dcmHeaderInfo.NumberOfPatientRelatedInstances);
				m_tableSearchStudyList->setItem(rowIdx, COL_STUDY_PATIENT_RELATED_INSTANCE_COUNT, item);

				item = new QTableWidgetItem();
				item->setData(Qt::EditRole, dcmHeaderInfo.NumberOfStudyRelatedSeries);
				m_tableSearchStudyList->setItem(rowIdx, COL_STUDY_RELATED_SERIES_COUNT, item);

				item = new QTableWidgetItem();
				item->setData(Qt::EditRole, dcmHeaderInfo.NumberOfSeriesRelatedInstances);
				m_tableSearchStudyList->setItem(rowIdx, COL_STUDY_SERIES_COUNT, item);

				QVariant data;
				data.setValue<void*>(pDcmDataset);
				m_tableSearchStudyList->item(rowIdx, COL_STUDY_DATA)->setData(Qt::UserRole, data);
			}

			rowIdx++;
		}
		m_tableSearchStudyList->resizeColumnsToContents();

		m_tableSearchStudyList->setSortingEnabled(true);
		m_tableSearchStudyList->sortByColumn(COL_STUDY_DATETIME, Qt::DescendingOrder);

		/* Study 정보 초기화시 Series 정보 Clear*/
		m_tableSearchSeriesList->setRowCount(0);

		UpdateControl();
	}

	void PACSSearchDownloadWidget::UpdateSeriesList(const std::vector<DicomInfomationModelSeriesObject*>& seriesInfoList)
	{
		/* Event 처리를 위해 Row Count - 0으로 초기화 필요 */
		m_tableSearchSeriesList->setRowCount(0);
		int newRowCount = seriesInfoList.size();
		m_tableSearchSeriesList->setRowCount(newRowCount);
		m_tableSearchSeriesList->setSortingEnabled(false);

		int rowIdx = 0;
		for (auto it = seriesInfoList.begin(); it != seriesInfoList.end(); ++it)
		{
			DicomDataset* pDcmDataset;
			if ((*it)->GetFirst(&pDcmDataset))
			{
				QTableWidgetItem* item;
				DICOM_HEADER_INFO dcmHeaderInfo = pDcmDataset->ToDcmHeaderInfo();
				m_tableSearchSeriesList->setItem(rowIdx, COL_SERIES_UID, new QTableWidgetItem(QString::fromStdWString(dcmHeaderInfo.SeriesInstanceUID)));
				m_tableSearchSeriesList->setItem(rowIdx, COL_SERIES_DATETIME, new QTableWidgetItem(QString::fromStdString(dcmHeaderInfo.SeriesDateTime.ToFormatText_DateTime())));

				item = new QTableWidgetItem();
				item->setData(Qt::EditRole, dcmHeaderInfo.SeriesNumber);
				m_tableSearchSeriesList->setItem(rowIdx, COL_SERIES_NUMBER, item);

				m_tableSearchSeriesList->setItem(rowIdx, COL_SERIES_MODALITY, new QTableWidgetItem(QString::fromStdWString(dcmHeaderInfo.Modality)));
				m_tableSearchSeriesList->setItem(rowIdx, COL_SERIES_DESCRIPTION, new QTableWidgetItem(QString::fromStdWString(dcmHeaderInfo.SeriesDescription)));

				item = new QTableWidgetItem();
				item->setData(Qt::EditRole, dcmHeaderInfo.NumberOfSeriesRelatedInstances);
				m_tableSearchSeriesList->setItem(rowIdx, COL_SERIES_IMAGE_COUNT, item);

				//item->setData(Qt::EditRole, dcmHeaderInfo.ImagesInAcqusition);
				//m_tableSearchSeriesList->setItem(rowIdx, COL_SERIES_IMAGE_COUNT, item);

				item = new QTableWidgetItem();
				item->setData(Qt::EditRole, dcmHeaderInfo.InstanceNumber);
				m_tableSearchSeriesList->setItem(rowIdx, COL_SERIES_INSTANCE_COUNT, item);

				QVariant data;
				data.setValue<void*>(pDcmDataset);
				m_tableSearchSeriesList->item(rowIdx, COL_SERIES_DATA)->setData(Qt::UserRole, data);

				rowIdx++;
			}
		}
		m_tableSearchSeriesList->resizeColumnsToContents();

		m_tableSearchSeriesList->setSortingEnabled(true);
		m_tableSearchSeriesList->sortByColumn(COL_SERIES_DATETIME, Qt::DescendingOrder);

		UpdateControl();
	}

	void PACSSearchDownloadWidget::UpdateByRuntimeMode()
	{
		bool isDebug = m_pContext->GetConfigManager()->GetAppConfig()->IsDebug();
		if (isDebug)
		{
			m_groupDebug->setVisible(true);

			m_colHeaderList_Study.SetVisible(COL_STUDY_UID, true);
			m_colHeaderList_Study.SetVisible(COL_STUDY_INSTANCE_COUNT, true);
			m_colHeaderList_Study.SetVisible(COL_STUDY_PATIENT_RELATED_STUDY_COUNT, true);
			m_colHeaderList_Study.SetVisible(COL_STUDY_PATIENT_RELATED_SERIES_COUNT, true);
			m_colHeaderList_Study.SetVisible(COL_STUDY_PATIENT_RELATED_INSTANCE_COUNT, true);
			m_colHeaderList_Study.SetVisible(COL_STUDY_RELATED_SERIES_COUNT, true);
			m_colHeaderList_Study.SetVisible(COL_STUDY_SERIES_COUNT, true);

			m_colHeaderList_Series.SetVisible(COL_SERIES_UID, true);
			m_colHeaderList_Series.SetVisible(COL_SERIES_INSTANCE_COUNT, true);
		}
		else
		{
			m_groupDebug->setVisible(false);

			m_colHeaderList_Study.SetVisible(COL_STUDY_UID, false);
			m_colHeaderList_Study.SetVisible(COL_STUDY_INSTANCE_COUNT, false);
			m_colHeaderList_Study.SetVisible(COL_STUDY_PATIENT_RELATED_STUDY_COUNT, false);
			m_colHeaderList_Study.SetVisible(COL_STUDY_PATIENT_RELATED_SERIES_COUNT, false);
			m_colHeaderList_Study.SetVisible(COL_STUDY_PATIENT_RELATED_INSTANCE_COUNT, false);
			m_colHeaderList_Study.SetVisible(COL_STUDY_RELATED_SERIES_COUNT, false);
			m_colHeaderList_Study.SetVisible(COL_STUDY_SERIES_COUNT, false);

			m_colHeaderList_Series.SetVisible(COL_SERIES_UID, false);
			m_colHeaderList_Series.SetVisible(COL_SERIES_INSTANCE_COUNT, false);
		}

		//=== Column Visible, Invisible
		//Study
		for (int idx : m_colHeaderList_Study.GetInvisibleHeaderIndexList())
		{
			m_tableSearchStudyList->hideColumn(idx);
		}
		for (int idx : m_colHeaderList_Study.GetVisibleHeaderIndexList())
		{
			m_tableSearchStudyList->showColumn(idx);
		}

		//Series
		for (int idx : m_colHeaderList_Series.GetInvisibleHeaderIndexList())
		{
			m_tableSearchSeriesList->hideColumn(idx);
		}
		for (int idx : m_colHeaderList_Series.GetVisibleHeaderIndexList())
		{
			m_tableSearchSeriesList->showColumn(idx);
		}

		m_tableSearchStudyList->resizeColumnsToContents();
		m_tableSearchSeriesList->resizeColumnsToContents();
	}

	void PACSSearchDownloadWidget::Download_As_CurselStudy()
	{
		if (m_mode == EPACSOperationMode::only_search_mode)
		{
			qWarning() << "can't download in search mode";
			return;
		}

		if (m_pDcmNetworkManager->IsStoreSCPInitialized() == false)
		{
			qWarning() << "Listener is not initialized";
			return;
		}

		if (m_tableSearchStudyList->currentRow() < 0)
		{
			qWarning() << "series item is not selected";
			return;
		}

		QMessageBox::StandardButton button = QMessageBox::warning(this,
			QString::fromLocal8Bit("Download"),
			QString::fromLocal8Bit("Download를 진행하시겠습니까?"),
			QMessageBox::StandardButton::Ok | QMessageBox::StandardButton::Cancel
		);

		if (button == QMessageBox::StandardButton::Cancel)
		{
			return;
		}

		QTableWidgetItem* item;

		/* study instance 정보 초기화 */
		int rowStudy = m_tableSearchStudyList->currentRow();
		int colStudyUID = COL_STUDY_UID;
		int colStudyImageCount = COL_STUDY_IMAGE_COUNT;

		item = m_tableSearchStudyList->item(rowStudy, colStudyUID);
		std::wstring studyInstanceUID = item->data(Qt::DisplayRole).toString().toStdWString();

		item = m_tableSearchStudyList->item(rowStudy, colStudyImageCount);
		int imageCount = item->data(Qt::DisplayRole).toInt();

		/* 경로 초기화 및 파일 확장자 초기화*/
		PACSConfig* pPACSConfig = m_pContext->GetConfigManager()->GetPACSConfig();
		std::wstring fileext = pPACSConfig->GetFileExtension();
		std::wstring repositoryDirpath = pPACSConfig->GetRepositoryDirectoryPath();

		m_pDcmNetworkManager->Download_Study_Async(
			studyInstanceUID,
			repositoryDirpath,
			fileext,
			imageCount);
	}

	void PACSSearchDownloadWidget::Download_As_CurselSeries()
	{
		if (m_mode == EPACSOperationMode::only_search_mode)
		{
			qWarning() << "can't download in search mode";
			return;
		}

		if (m_pDcmNetworkManager->IsStoreSCPInitialized() == false)
		{
			qWarning() << "Listener is not initialized";
			return;
		}

		if (m_tableSearchSeriesList->currentRow() < 0)
		{
			qWarning() << "series item is not selected";
			return;
		}

		QMessageBox::StandardButton button = QMessageBox::warning(this,
			QString::fromLocal8Bit("Download"),
			QString::fromLocal8Bit("Download를 진행하시겠습니까?"),
			QMessageBox::StandardButton::Ok | QMessageBox::StandardButton::Cancel
		);

		if (button == QMessageBox::StandardButton::Cancel)
		{
			return;
		}

		QTableWidgetItem* item;

		/* study instance 정보 초기화 */
		int rowStudy = m_tableSearchStudyList->currentRow();
		int colStudyUID = COL_STUDY_UID;

		item = m_tableSearchStudyList->item(rowStudy, colStudyUID);
		std::wstring studyInstanceUID = item->data(Qt::DisplayRole).toString().toStdWString();

		/* series instance 정보 초기화 */
		int rowSeries = m_tableSearchSeriesList->currentRow();
		int colSeriesUID = COL_SERIES_UID;
		int colSeriesImageCount = COL_SERIES_IMAGE_COUNT;

		item = m_tableSearchSeriesList->item(rowSeries, colSeriesUID);
		std::wstring seriesInstanceUID = item->data(Qt::DisplayRole).toString().toStdWString();

		item = m_tableSearchSeriesList->item(rowSeries, colSeriesImageCount);
		int imageCount = item->data(Qt::DisplayRole).toInt();

		/* 경로 초기화 및 파일 확장자 초기화*/
		PACSConfig* pPACSConfig = m_pContext->GetConfigManager()->GetPACSConfig();
		std::wstring fileext = pPACSConfig->GetFileExtension();
		std::wstring repositoryDirpath = pPACSConfig->GetRepositoryDirectoryPath();

		m_pDcmNetworkManager->Download_Series_Async(
			studyInstanceUID,
			seriesInstanceUID,
			repositoryDirpath,
			fileext,
			imageCount);
	}

	void PACSSearchDownloadWidget::Select_As_CurselStudy()
	{
		int rowIdx = m_tableSearchStudyList->currentRow();
		if (rowIdx >= 0)
		{
			QVariant var = m_tableSearchStudyList->item(rowIdx, COL_STUDY_DATA)->data(Qt::UserRole);
			DicomDataset* pData = (DicomDataset*)var.value<void*>();
			emit studySelected(*pData);
		}
	}

	void PACSSearchDownloadWidget::Select_As_CurselSeries()
	{
		int rowIdx = m_tableSearchSeriesList->currentRow();
		if (rowIdx >= 0)
		{
			QVariant var = m_tableSearchSeriesList->item(rowIdx, COL_SERIES_DATA)->data(Qt::UserRole);
			DicomDataset* pData = (DicomDataset*)var.value<void*>();
			emit seriesSelected(*pData);
		}
	}

	EQueryRetrieveLevel PACSSearchDownloadWidget::GetFindQueryRetrieveLevel()
	{
		EQueryRetrieveLevel queryRetrieveLevel = EQueryRetrieveLevel::QR_LEVEL_SERIES;
		if (m_rdoDebugQRPatientMode->isChecked())
		{
			queryRetrieveLevel = EQueryRetrieveLevel::QR_LEVEL_PATIENT;
		}
		else if (m_rdoDebugQRStudyMode->isChecked())
		{
			queryRetrieveLevel = EQueryRetrieveLevel::QR_LEVEL_STUDY;
		}
		else if (m_rdoDebugQRSeriesMode->isChecked())
		{
			queryRetrieveLevel = EQueryRetrieveLevel::QR_LEVEL_SERIES;
		}
		else if (m_rdoDebugQRImageMode->isChecked())
		{
			queryRetrieveLevel = EQueryRetrieveLevel::QR_LEVEL_IMAGE;
		}
		else
		{
			queryRetrieveLevel = EQueryRetrieveLevel::QR_LEVEL_STUDY;
		}
		return queryRetrieveLevel;
	}

	DicomFindOption PACSSearchDownloadWidget::GetMainFindOption()
	{
		EDcmFindMode findMode = (EDcmFindMode)m_cboSearchMode->currentData(Qt::UserRole).toInt();
		QString findText = m_editSearch->text();
		EDcmModality modality = (EDcmModality)m_cboModality->currentData(Qt::UserRole).toInt();
		EQueryRetrieveLevel queryRetrieveLevel = GetFindQueryRetrieveLevel();

		DicomFindOption findOption(findMode, findText.toStdWString(), modality, queryRetrieveLevel);

		if (CurrentDateRangeMode() != EDateRangeMode::All_Date)
		{
			DateTime startDateTime(m_dateEditStart->dateTime());
			DateTime endDateTime(m_dateEditEnd->dateTime());
			findOption.SetDateRange(startDateTime, endDateTime);
		}

		return findOption;
	}

	void PACSSearchDownloadWidget::UpdateSCPRunningStatusControl()
	{
		if (m_pContext->GetDicomNetworkManager()->IsStoreSCPRunning())
		{
			m_labelDownloadServerStatus->setText("Store SCP Running");
		}
		else
		{
			m_labelDownloadServerStatus->setText("Store SCP Not Running");
		}
	}

	void PACSSearchDownloadWidget::onSearch()
	{
		DicomFindOption findOption = GetMainFindOption();
		m_pDcmNetworkManager->Find_Async(findOption);
	}

	void PACSSearchDownloadWidget::onDownload()
	{
		/* Series를 우선적으로 체크하여 다운로드 */
		if (m_tableSearchSeriesList->currentRow() >= 0)
		{
			Download_As_CurselSeries();
		}
	}

	void PACSSearchDownloadWidget::onDateRangeModeChanged(int index)
	{
		if (index >= 0)
		{
			UpdateDateRange();
		}
	}

	void PACSSearchDownloadWidget::onStudySearchListDoubleClicked()
	{
		if (m_mode == EPACSOperationMode::can_download_mode)
		{
			Download_As_CurselStudy();
		}
		else if (m_mode == EPACSOperationMode::only_search_mode)
		{
			Select_As_CurselStudy();
		}
	}

	void PACSSearchDownloadWidget::onStudySearchListItemChanged(QTableWidgetItem* current, QTableWidgetItem* previous)
	{
		if (m_searchMode == EPACSSearchMode::search_study)
		{
			/* Skip */
			return;
		}

		bool shouldUpdate = false;
		if (current == nullptr)
		{
			shouldUpdate = false;
		}
		else if (previous == nullptr)
		{
			shouldUpdate = true;
		}
		else
		{
			shouldUpdate = (current->row() != previous->row());
		}

		if (shouldUpdate)
		{
			int row = m_tableSearchStudyList->currentRow();
			int col = COL_STUDY_UID;
			if (row >= 0)
			{
				QTableWidgetItem* item = m_tableSearchStudyList->item(row, col);
				std::wstring studyInstanceUID = item->data(Qt::DisplayRole).toString().toStdWString();

				EQueryRetrieveLevel queryRetrieveLevel = GetFindQueryRetrieveLevel();
				if (queryRetrieveLevel == EQueryRetrieveLevel::QR_LEVEL_STUDY)
				{
					/* Study Mode일 Study Instance UID로 검색 후 find */
					DicomFindOption studyUIDfindOption(studyInstanceUID);
					m_pDcmNetworkManager->Find_Async(studyUIDfindOption);
				}
				else if (queryRetrieveLevel == EQueryRetrieveLevel::QR_LEVEL_SERIES)
				{
					//std::vector<DicomSeriesInfo> seriesInfoList = m_dcmStudyInfoManager.GetDcmSeriesInfoListByStudyInstanceUID(studyInstanceUID);
					//UpdateSeriesList(&seriesInfoList);
					UpdateControl();
				}
			}
		}
	}

	void PACSSearchDownloadWidget::onSeriesSearchListDoubleClicked()
	{
		if (m_mode == EPACSOperationMode::can_download_mode)
		{
			Download_As_CurselSeries();
		}
		else if (m_mode == EPACSOperationMode::only_search_mode)
		{
			Select_As_CurselSeries();
		}
	}

	void PACSSearchDownloadWidget::onSeriesSearchListItemChanged(QTableWidgetItem* current, QTableWidgetItem* previous)
	{
		int row = m_tableSearchStudyList->currentRow();
		if (row >= 0)
		{
			UpdateControl();
		}
	}

	void PACSSearchDownloadWidget::onRdoDebugQRMode()
	{
	}

	void PACSSearchDownloadWidget::onBtnDebugExportData()
	{
	}

	void PACSSearchDownloadWidget::onBtnDebugCompare()
	{
	}

	void PACSSearchDownloadWidget::onBtnDebugClear()
	{
	}

	void PACSSearchDownloadWidget::onActDownloadStudy()
	{
		Download_As_CurselStudy();
	}

	void PACSSearchDownloadWidget::onActSaveFileStudy()
	{
		int rowIdx = m_tableSearchStudyList->currentRow();
		if (rowIdx < 0)
		{
			return;
		}

		QString targetFilePath = QFileDialog::getSaveFileName(
			this,
			QString(),
			QString(),
			tr("DCM (*.dcm;*.DCM)"));

		if (!targetFilePath.isEmpty())
		{
			QVariant var = m_tableSearchStudyList->item(rowIdx, COL_STUDY_DATA)->data(Qt::UserRole);
			DicomDataset* pData = (DicomDataset*)var.value<void*>();
			pData->SaveToFile(targetFilePath);
		}
	}

	void PACSSearchDownloadWidget::onActDownloadSeries()
	{
		Download_As_CurselSeries();
	}

	void PACSSearchDownloadWidget::onActSaveFileSeries()
	{
		int rowIdx = m_tableSearchSeriesList->currentRow();
		if (rowIdx < 0)
		{
			return;
		}

		QString targetFilePath = QFileDialog::getSaveFileName(
			this,
			QString(),
			QString(),
			tr("DCM (*.dcm;*.DCM)"));

		if (!targetFilePath.isEmpty())
		{
			QVariant var = m_tableSearchSeriesList->item(rowIdx, COL_SERIES_DATA)->data(Qt::UserRole);
			DicomDataset* pData = (DicomDataset*)var.value<void*>();
			pData->SaveToFile(targetFilePath);
		}
	}

	void PACSSearchDownloadWidget::onPACSSelectionChanged()
	{
		Reset();
	}

	void PACSSearchDownloadWidget::onRefreshDownloadServer()
	{
		m_pContext->GetActionManager()->DicomSCPServerStart();
	}

	void PACSSearchDownloadWidget::onSCPServerStartFailed()
	{
		UpdateSCPRunningStatusControl();
	}

	void PACSSearchDownloadWidget::onSCPServerStatusChanged(DicomNetworkSCPStatus status)
	{
		UpdateSCPRunningStatusControl();
	}


	void PACSSearchDownloadWidget::onDcmNetFindFinished(DcmNetFindData findData)
	{
		if (m_searchMode == EPACSSearchMode::search_study)
		{
			/* Study 정보 업데이트 */
			m_dcmInfoModel.Clear();
			for (auto& data : findData.DicomDatasetList)
			{
				m_dcmInfoModel.Add(data);
			}

			UpdateStudyList(m_dcmInfoModel.GetStudyList());
		}
		else
		{
			if (findData.FindOption.IsSearchByStudyInstanceUIDMode())
			{
				/* Series 정보 업데이트 */
				for (auto& data : findData.DicomDatasetList)
				{
					m_dcmInfoModel.Add(data);
				}

				DicomInfomationModelStudyObject* pStudyModel = m_dcmInfoModel.GetStudy(findData.FindOption.StudyInstanceUID);
				if (pStudyModel)
				{
					UpdateSeriesList(pStudyModel->GetSeriesList());
				}
			}
			else
			{
				/* Study 정보 업데이트 */
				m_dcmInfoModel.Clear();
				for (auto& data : findData.DicomDatasetList)
				{
					m_dcmInfoModel.Add(data);
				}

				UpdateStudyList(m_dcmInfoModel.GetStudyList());
			}
		}
	}

	void PACSSearchDownloadWidget::onDcmNetSCPServerStartFailed()
	{
		QString errorMessage = "fail to initialize STORE-SCP";
		QMessageBox::warning(this, "ERROR", errorMessage, QMessageBox::StandardButton::Ok);
		UpdateControl();
	}

	void PACSSearchDownloadWidget::onDcmNetSCPServerStatusChanged(DicomNetworkSCPStatus status)
	{
		UpdateControl();
	}

	void PACSSearchDownloadWidget::onSettingChanged()
	{
		UpdateByRuntimeMode();
	}
}

