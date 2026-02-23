#pragma once

#include <qwidget>
#include <qcolor>
#include <QScrollBar>
#include <QListWidgetItem>
#include "filemanager/appcore/appcore_defines.h"
#include "filemanager/dicom/Image/DicomDataImage_sint16.h"
#include "filemanager/dicom/DicomInfomationModel.h"
#include "filemanager/dicom/DicomDatasetIO.h"
#include "filemanager/dicom/DicomInfomationModelLoader.h"
#include "filemanager/appcore/UI/ColumnHeaderInfoList.h"
#include "filemanager/data/Image/WindowingInfo.h"
#include "filemanager/export.h"

class QLabel;
class QVBoxLayout;
class QTreeWidgetItem;
class DcmElement;
class DcmItem;

namespace Ui
{
	class DicomPreviewWidget;
	class DicomImageViewer;
}

namespace fm
{
	class DicomDataImage_sint16;
	class DicomImageViewer;
	class DicomInfomationModel;

	class FM_CORE_EXPORT DicomPreviewWidget : public QWidget
	{
		Q_OBJECT

	public:
		enum eTabIndex
		{
			ImageTabIndex = 0,
			PdfTabIndex,
		};

		enum eTreeColumn
		{
			StudyDescription = 0,
			SeriesDescription,
			Index,
			Modality,
			ImageCount
		};

	public:
		DicomPreviewWidget(QWidget* parent);

	public:
		bool TryGetCurrentSelectedSeries(DicomInfomationModelSeriesObject** ppOutSeries);
		DicomInfomationModelSeriesObject* GetCurrentSelectedSeries();
		bool SelectCurrentSeries();

		void SetDicomInfomationModel(std::shared_ptr<DicomInfomationModel> pDcmInfomationModel);
		void SetDicomInfomationModel_Series(const DicomInfomationModelSeriesObject* pSeries);

		void AbortAndWaitLoader();
		void Clear();

		void SetStudyListVisible(bool value);
		bool IsStudyListVisible();

		void SetSeriesListVisibile(bool value);
		bool IsSeriesListVisibile();

		void RestoreCenterSplitter(const QByteArray& state);
		void RestoreBottomSplitter(const QByteArray& state);

		QByteArray SaveCenterSplitter();
		QByteArray SaveBottomSplitter();

		void Update(bool forceToUpdate);
		void ForceToUpdate();
	protected:
		void resizeEvent(QResizeEvent*) override;

	private:
		void UpdateStudyModelTree();
		void UpdateSeriesModelList();
		void UpdateImageAndDicomTag(bool forceToUpdate);

		/* Study Tree Model */
		void SelectTreeItemBySeries(DicomInfomationModelSeriesObject* pSeries);
		QTreeWidgetItem* CreateStudyTreeWidgetItem(DicomInfomationModelStudyObject* pStudy);
		QTreeWidgetItem* CreateSeriesTreeWidgetItem(DicomInfomationModelSeriesObject* pSeries);
		QTreeWidgetItem* CreateSeriesWithImageTypeTreeWidgetItem(DicomInfomationModelSeriesObject* pSeries, int index);

		/* Image List */
		void SelectListBySeries(DicomInfomationModelSeriesObject* pSeries);
		QListWidgetItem* AddOrModifySeriesToSeriesPreivewList(DicomInfomationModelSeriesObject* pSeries);
		QListWidgetItem* FindItemFromSeriesList(DicomInfomationModelSeriesObject* pSeries);

		void UpdatePreviewByCurSeries();
		QIcon GetSeriesItemIcon(DicomInfomationModelSeriesObject* pSeries);
		void Draw_ImagePixmap(QPainter& p, int y, DicomDataset* pDicomDataset, QRect& outDrawRect);
		void Draw_Text(QPainter& p, int y, QString text, QRect& outDrawRect);

		bool selectSeries(DicomInfomationModelSeriesObject* pSeries);
		bool isDcmDatasetSupportedForMEDIP(DicomDataset* pDcmDataset) const;

	Q_SIGNALS:
		void currentSeriesChanged(DicomInfomationModelSeriesObject* pSeries);
		void currentSeriesSelected(DicomInfomationModelSeriesObject* pSeries);

	private slots:
		void onStudyModelCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
		void onStudyModelItemDoubleClicked(QTreeWidgetItem* item);

		void onSupportedSeriesListItemChanged(int row);
		void onSupportedSeriesListItemDoubleClicked(QListWidgetItem* item);

		void onDcmFileLoadedStarted();
		void onDcmFileLoaded_InProgress(int progressCount, int maxCount);
		void onDcmFileLoadedFinished();

	private:
		Ui::DicomPreviewWidget* m_ui;

		std::shared_ptr<DicomInfomationModel> m_pDcmInfomationModel;
		DicomInfomationModelLoader* m_pDcmInfoLoader;

		QFuture<bool> m_dcmLoaderFuture;
		ColumnHeaderInfoList m_colHeaderInfoList_StudyModel;
	};
}
