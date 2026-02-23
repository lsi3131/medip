#pragma once

#include <qwidget>
#include "FileManager/dicom/dicom_defines.h"
#include "FileManager/appcore/appcore_defines.h"
#include "filemanager/dicom/Network/DicomFindOption.h"
#include "filemanager/dicom/Network/DicomNetworkSCPStatus.h"
#include "filemanager/dicom/DicomInfomationModel.h"
#include "FileManager/appcore/UI/ColumnHeaderInfoList.h"
#include "FileManager/appcore/UI/DateRangeInfo.h"
#include "ui_PACSSearchDownloadWidget.h"
#include <QTableWidgetItem>
#include <QPushButton>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <qdatetimeedit>
#include <unordered_map>

namespace fm
{
	class AppCoreContext;
	class PACSListWidget;
	class DicomNetworkManager;

	class PACSSearchDownloadWidget : public QWidget, public Ui::PACSSearchDownloadWidget
	{
		Q_OBJECT
	public:
		enum EStudySearchListColIndex
		{
			COL_STUDY_UID = 0,
			COL_STUDY_DATETIME,
			COL_STUDY_PATIENT_NAME,
			COL_STUDY_BIRTH_DATE,
			COL_STUDY_AGE,
			COL_STUDY_SEX,
			COL_STUDY_PATIENT_ID,
			COL_STUDY_DESCRIPTION,
			COL_STUDY_ACCESSION_NUMBER,
			COL_STUDY_EXAM_ID,
			COL_STUDY_REFERRING_PHYSICIAN,
			COL_STUDY_PERFORMING_PHYSICIAN,
			COL_STUDY_READING_PHYSICIAN,
			COL_STUDY_INSTITUTION_NAME,
			COL_STUDY_MANUFACTURER,
			COL_STUDY_MANUFACTURER_MODEL_NAME,
			COL_STUDY_IMAGE_COUNT,
			COL_STUDY_INSTANCE_COUNT,
			COL_STUDY_PATIENT_RELATED_STUDY_COUNT,
			COL_STUDY_PATIENT_RELATED_SERIES_COUNT,
			COL_STUDY_PATIENT_RELATED_INSTANCE_COUNT,
			COL_STUDY_RELATED_SERIES_COUNT,
			COL_STUDY_SERIES_COUNT,
		};

		enum EStudySearchListColDataIndex
		{
			COL_STUDY_DATA = 0,
		};

		enum ESeriesSearchListColIndex
		{
			COL_SERIES_UID = 0,
			COL_SERIES_DATETIME,
			COL_SERIES_NUMBER,
			COL_SERIES_MODALITY,
			COL_SERIES_DESCRIPTION,
			COL_SERIES_IMAGE_COUNT,
			COL_SERIES_INSTANCE_COUNT,
		};

		enum ESeriesSearchListColDataIndex
		{
			COL_SERIES_DATA = 0,
		};

	public:
		PACSSearchDownloadWidget(QWidget* parent = nullptr);
		virtual ~PACSSearchDownloadWidget();

	public:
		void Init(AppCoreContext* pContext, EPACSOperationMode mode, EPACSSearchMode searchMode);
		void SetMode(EPACSOperationMode mode, EPACSSearchMode searchMode);

		void Refresh();
		void UpdateControl();
		void Reset();

		void SaveStatus();

	private:
		void InitLayout();
		void InitStudyListTable();
		void InitSeriesListTable();

		void UpdateComboBox_SearchMode();
		void UpdateComboBox_Modality();
		void UpdateComboBox_SearchDateMode();

		EDateRangeMode CurrentDateRangeMode();
		void UpdateDateRange();

		void UpdateStudyList(const std::vector<DicomInfomationModelStudyObject*>& studyList);
		void UpdateSeriesList(const std::vector<DicomInfomationModelSeriesObject*>& seriesInfoList);

		void UpdateByRuntimeMode();

		void Download_As_CurselStudy();
		void Download_As_CurselSeries();

		void Select_As_CurselStudy();
		void Select_As_CurselSeries();

		EQueryRetrieveLevel GetFindQueryRetrieveLevel();
		DicomFindOption GetMainFindOption();

		void UpdateSCPRunningStatusControl();

	Q_SIGNALS:
		void studySelected(DicomDataset dcmStudySelected);
		void seriesSelected(DicomDataset dcmSeriesSelected);

	private slots:
		void onSearch();
		void onDownload();

		void onDateRangeModeChanged(int index);

		void onStudySearchListDoubleClicked();
		void onStudySearchListItemChanged(QTableWidgetItem* current, QTableWidgetItem* previous);

		void onSeriesSearchListDoubleClicked();
		void onSeriesSearchListItemChanged(QTableWidgetItem* current, QTableWidgetItem* previous);

		void onRdoDebugQRMode();

		void onBtnDebugExportData();
		void onBtnDebugCompare();
		void onBtnDebugClear();

		void onActDownloadStudy();
		void onActSaveFileStudy();

		void onActDownloadSeries();
		void onActSaveFileSeries();

		void onPACSSelectionChanged();
		void onRefreshDownloadServer();
		void onSCPServerStartFailed();
		void onSCPServerStatusChanged(DicomNetworkSCPStatus status);

	private slots:
		void onDcmNetFindFinished(DcmNetFindData result);
		void onDcmNetSCPServerStartFailed();
		void onDcmNetSCPServerStatusChanged(DicomNetworkSCPStatus status);

		void onSettingChanged();

	private:
		AppCoreContext* m_pContext;
		DicomNetworkManager* m_pDcmNetworkManager;
		DicomInfomationModel m_dcmInfoModel;

		QAction* m_actDownloadStudy;
		QAction* m_actSaveFileStudy;

		QAction* m_actDownloadSeries;
		QAction* m_actSaveFileSeries;

		ColumnHeaderInfoList m_colHeaderList_Study;
		ColumnHeaderInfoList m_colHeaderList_Series;

		EPACSOperationMode m_mode;
		EPACSSearchMode m_searchMode;


	};
}

