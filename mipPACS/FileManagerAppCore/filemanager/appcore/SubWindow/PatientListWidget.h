#pragma once

#include <qwidget>
#include <qcombobox.h>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QComboBox>
#include <QSplitter>
#include <qlayout>
#include <qlabel>
#include <qheaderview>
#include <QDir>
#include "filemanager/appcore/UI/ColumnHeaderInfoList.h"
#include "filemanager/appcore/UI/FindOptionInfoList.h"
#include "filemanager/dicom/DicomInfomationModel.h"
#include "filemanager/data/Entity/ImportedDicomInfoDTO.h"
#include "filemanager/data/Entity/ImportedDicomInfoDTOManager.h"
#include "ui_PatientListWidget.h"

namespace fm
{
	class AppCoreContext;


	class PatientListWidget : public QWidget, public Ui::PatientListWidget
	{
		Q_OBJECT

	public:
		enum EStudyTableColIndex
		{
			COL_STUDY_UID,
			COL_STUDY_DATETIME,
			COL_STUDY_PATIENT_NAME,
			COL_STUDY_PATIENT_ID,
			COL_STUDY_SEX,
			COL_STUDY_AGE,
			COL_STUDY_IMPORTED_DATE,
			COL_STUDY_IMAGE_COUNT,
			COL_STUDY_DESCRIPTION,
			COL_STUDY_ACCESSION_NUMBER,
			COL_STUDY_EXAM_ID,
			COL_STUDY_DCM_FOLDER,
		};

		enum ESeriesTableColIndex
		{
			COL_SERIES_UID,
			COL_SERIES_DATETIME,
			COL_SERIES_NUMBER,
			COL_SERIES_MODALITY,
			COL_SERIES_IMAGE_COUNT,
			COL_SERIES_DESCRIPTION,
			COL_SERIES_DCM_FILEPATH,
			COL_SERIES_DCM_DIRECTORY_PATH,
			COL_SERIES_DCM_FILE_LIST,
		};

		enum EPreviewIndex
		{
			PREVIEW_INDEX_EMPTY,
			PREVIEW_INDEX_IMAGE,
			PREVIEW_INDEX_PDF,
			PREVIEW_INDEX_DICOM_IMAGE,
			PREVIEW_INDEX_NOT_SUPPORTED,
		};

	public:
		PatientListWidget(QWidget* parent = nullptr);
		virtual ~PatientListWidget();

	public:
		void Init(AppCoreContext* pContext);
		void Refresh(bool forceToUpdate = false);

		void SaveStatus();

	protected:
		void dragEnterEvent(QDragEnterEvent* e) override;
		void dropEvent(QDropEvent* e) override;

	private slots:
		void onSettingChanged();

		void onImportDICOM();
		void onImportFromPACS();

		void onSearch();
		void onChkPreview();

		void onStudyListCurrentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);
		void onStudyListItemChanged(QTableWidgetItem *item);

		void onSeriesListDblClick(const QModelIndex &index);
		void onSeriesListCurrentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);

		void onDicomListAdded(std::vector<ImportedDicomInfoDTO> dcmList);
		void onDicomListModified(std::vector<ImportedDicomInfoDTO> dcmList);

		void onActDicomEdit_Study(bool checked);
		void onActOpenContainingFolder_Study(bool checked);
		void onActDelete_Study(bool checked);
		void onActExport_Study(bool checked);
		void onActCopyAllToClipboard_Study(bool checked);

		void onActOpenFile(bool checked);
		void onActOpenContainingFolder_Series(bool checked);
		void onActCopyFolder_Series(bool checked);
		void onActDelete_Series(bool checked);
		void onActCopyAllToClipboard_Series(bool checked);

		void onPatientListDicomUploadStarted();
		void onPatientListDicomUploadInProgress(int progressCount, int maxCount);
		void onPatientListDicomUploadFinished();

		void onPatientListDicomEditStarted();
		void onPatientListDicomEditInProgress(int progressCount, int maxCount);
		void onPatientListDicomEditFinished();
	private:
		void InitLayout();
		void RefreshModel(std::wstring selectedStudyInstanceUID = L"", std::wstring selectedSeriesInstanceUID = L"");
		void ResetDicomDatasetModel_As_DTODataList(std::vector<fm::ImportedDicomInfoDTO>& datas, std::wstring studyInstanceUID = L"", std::wstring seriesInstanceUID = L"");
		void AddToDicomDatasetModel_As_DTODataList(std::vector<fm::ImportedDicomInfoDTO>& datas, std::wstring studyInstanceUID = L"", std::wstring seriesInstanceUID = L"");

		void UpdateStudyList(std::wstring studyInstanceUID = L"", std::wstring seriesInstanceUID = L"");

		void UpdateSeriesList(std::vector<DicomInfomationModelSeriesObject*>& seriesList);
		void UpdateSeriesListByCurSelStudy();

		void UpdatePreview();
		void SelectStudyList_And_SereisList(std::wstring studyInstanceUID, std::wstring seriesInstanceUID);
		void SetTableStudyRow(DicomInfomationModelStudyObject& study, int rowIndex);

		bool SetTableSeriesRow(DicomInfomationModelSeriesObject& sereis, int rowIndex);
		void UpdateByRuntimeMode();

		void OpenFile();
		void CopyToClipboard_Table(QTableWidget* widget);

		std::vector<DicomInfomationModelStudyObject*> GetSelectedStudyList();
	private:
		AppCoreContext* m_pContext;

		QAction* m_actDelete_Study;
		QAction* m_actDicomEdit_Study;
		QAction* m_actOpenContainingFolder_Study;
		QAction* m_actCopyAllToClipboard_Study;

		QAction* m_actOpenFile;
		QAction* m_actOpenContainingFolder_Series;
		QAction* m_actCopyFolder_Series;
		QAction* m_actDelete_Series;
		QAction* m_actCopyAllToClipboard_Series;

		ColumnHeaderInfoList m_colHeaderList_Study;
		ColumnHeaderInfoList m_colHeaderList_Series;

		FindOptionInfoList m_findOptionList;

		std::shared_ptr<DicomInfomationModel> m_mainDicomInfoModel;
		std::shared_ptr<DicomInfomationModel> m_previewSeriesDicomInfoModel;
		DicomInfomationModelLoader m_dcmLoader;
	};
}

