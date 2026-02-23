#pragma once

#include "filemanager/export.h"
#include "FileManager/dicom/dicom_defines.h"
#include "FileManager/appcore/appcore_defines.h"
#include "FileManager/appcore/UI/ColumnHeaderInfoList.h"
#include "FileManager/dicom/Network/DicomNetworkSCUStatus.h"
#include <Qwidget>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QTableWidget>

namespace Ui
{
	class PACSExportWidget;
}

namespace fm
{
	class AppCoreContext;
	class PACSDicomConvertUploadWidget;
	class PACSListWidget;
	class FolderViewWidget;
	class PACSConfig;
	class DicomHostInfo;
	class DicomInfomationModelSeriesObject;
	class DicomExportManager;

	class FM_CORE_EXPORT PACSExportWidget : public QWidget
	{
		Q_OBJECT

	public:
		enum EColumnPatientInfo
		{
			COL_PATIENT_INFO_ITEMS,
			COL_PATIENT_INFO_CONTENT,
		};

		enum ERowPatientInfo
		{
			ROW_STUDY_INSTANCE_UID,
			ROW_PATIENT_ID,
			ROW_PATIENT_NAME,
			ROW_PATIENT_BIRTH_DATE,
			ROW_PATIENT_SEX,
			ROW_PATIENT_AGE,
			ROW_STURY_DESCRIPTION,
			ROW_STURY_DATETIME,
			ROW_MODALITY,
			ROW_ACCESSION_NUMBER,
			ROW_EXAM_ID,
			ROW_MAX_COUNT,
		};

		enum EColumnData
		{
			COL_DATA_SERIES_SELECT,
			COL_DATA_SERIES_DATE,
			COL_DATA_SERIES_NUMBER,
			COL_DATA_SERIES_DESCRIPTION,
			COL_DATA_SERIES_COUNT,
		};

		enum EColumnPACSList
		{
			COL_PACS_LIST_NAME,
		};

	public:
		PACSExportWidget(QWidget* parent = nullptr);
		virtual ~PACSExportWidget();

	public:
		bool Init(AppCoreContext* pContext);
		void Refresh();

		void SaveStatus();

	private:
		void InitLayout();
		void UpdateExportDicomInfoTable();
		void UpdateExportDataListTable();
		void UpdatePACSList();
		void UpdatePreviewPage(DicomInfomationModelSeriesObject* pDcmSeries);
		void UpdateControlEnableStatus();
		void UpdateByRuntimeMode();
		void InitExportDicomInfoList();

		bool GetCurrentSelectedSeriesDataList(std::vector<DicomInfomationModelSeriesObject*>& selectedSeriesDataList);
		bool GetCurrentSelectedStoreDicomHostInfo(DicomHostInfo* pOutDicomHostInfo);

	private:
		void Update();

	Q_SIGNALS:

	private slots:
		void onSettingChanged();

		void onPACSSelectionChanged();
		void onExportDataListCurrentItemChanged(QTableWidgetItem* current, QTableWidgetItem* previous);
		void onExportToPACS();

		void onLoadExportData_DICOM_Debug();
		void onLoadExportData_Image_Debug();
		void onLoadExportData_PDF_Debug();
		void onLoadDICOMExportInfo_Local_Debug();
		void onLoadDICOMExportInfo_PACS_Debug();

		void onDcmNetUploadStarted();
		void onDcmNetUploadInProgress(DicomNetworkSCUStatus status);
		void onDcmNetUploadFinished();
		void onDcmNetError(QString errorMessage);

		void onExportPACSServerChanged(int index);

	private:
		Ui::PACSExportWidget* m_ui;
		AppCoreContext* m_pContext;
		PACSConfig* m_pPACSConfig;
		DicomExportManager* m_pDcmExportManager;

		ColumnHeaderInfoList m_colHeaderList_PatientInfo;
		ColumnHeaderInfoList m_colHeaderList_ExportData;
	};
}

