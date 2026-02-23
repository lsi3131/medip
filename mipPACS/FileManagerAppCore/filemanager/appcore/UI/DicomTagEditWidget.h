#pragma once

#include <qtreewidget>
#include "filemanager/appcore/UI/ColumnHeaderInfoList.h"
#include "ui_DicomTagEditWidget.h"

class DcmItem;
class DcmElement;

namespace fm
{
	class DicomDataset;
	class DicomInfomationModelSeriesObject;

	class DicomTagEditWidget : public QTreeWidget, public Ui::DicomTagEditWidget
	{
		Q_OBJECT
	public:
		enum EColumn
		{
			COL_TAG_ID = 0,
			COL_VR,
			COL_VM,
			COL_LENGTH,
			COL_DESCRIPTION,
			COL_VALUE,
		};
	public:
		DicomTagEditWidget(QWidget* parent);

	public:
		void SetDicomSeries(DicomInfomationModelSeriesObject* pSeries, bool forceToUpdate);
		bool IsEmpty();
		void Clear();

	private:
		void InitSliderBySeries();
		void UpdateIndexRangeTextBySeries();
		bool GetCurSelDicomDataset(fm::DicomDataset*& pOutDicomDataset);
		void UpdateControl();
		void UpdateDicomTagTree();
		void UpdateFilePath();

		void SetTreeWidgetByDcmItem_Recursive(DcmItem* dcmItem, QTreeWidgetItem* parentTreeItem = nullptr);
		void SetTreeWidgetByDcmElement(DcmElement* element, QTreeWidgetItem* parentTreeItem = nullptr);

		/* Dicom Tag Tree*/
		QTreeWidgetItem* CreateNewDicomTagTreeItem(DcmElement* element);

	private slots:
		void onSliderSeriesRangeChanged(int value);
		void onEditSearchFilterChanged(const QString& text);
		void onBtnCopyDicomPath();
		void onBtnCopyData();
		void onBtnExpandAll();
		void onBtnCollapsedAll();

	private:
		DicomInfomationModelSeriesObject* m_pDicomSeries;
		ColumnHeaderInfoList m_colHeaderInfoList_DicomTag;
		QTreeWidget* m_pCopy;
	};
}
