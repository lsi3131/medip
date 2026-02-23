#pragma once

#include <qdialog>
#include "ui_DicomEditDialog.h"
#include "filemanager/appcore/UI/ColumnHeaderInfoList.h"

namespace fm
{
	class DicomInfomationModelStudyObject;
	class DicomDataset;

	struct TableRecord
	{
		int ID;
		QString Name;
		QString Value;
	};

	class DicomEditDialog : public QDialog, public Ui::DicomEditDialog
	{
		Q_OBJECT

	public:
		DicomEditDialog(QWidget* parent, std::vector<DicomInfomationModelStudyObject*> dicomStudyModelList);

	public:
		DicomDataset GetEdittedDicomDataset_Study();

	private:
		void UpdateDicomStudyList();
		void UpdateDicomSeriesList_By_CurSelStudy();
		void UpdateDicomTagList();

		DicomInfomationModelStudyObject* GetCurselStudy();

		void UpdateRecordList_By_DicomDataset(DicomDataset* pDataset);
		void UpdateTableWidget_By_RecordList();
		void UpdateRecordList_By_TableWidget();


	private slots:
		void onSave();
		void onCancel();

		void onListDicomStudyCurrentItemChanged(QListWidgetItem* current, QListWidgetItem* previous);
		void onListDicomSeriesCurrentItemChanged(QListWidgetItem* current, QListWidgetItem* previous);
		
	private:
		std::vector<DicomInfomationModelStudyObject*> m_originDicomStudyModelList;

		ColumnHeaderInfoList m_colHeaderInfoList_DicomTag;

		std::vector<TableRecord> m_tableRecordList;
	};
}
