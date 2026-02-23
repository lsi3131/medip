#pragma once

#include <qdialog>
#include "filemanager/export.h"
#include "filemanager/dicom/DicomDataset.h"

namespace fm
{
	class AppCoreContext;
	class PACSSearchDownloadWidget;

	class FM_CORE_EXPORT PACSSearchDialog : public QDialog
	{
		Q_OBJECT

	public:
		PACSSearchDialog(AppCoreContext* pContext, QWidget* parent = nullptr);

		DicomDataset GetDicomStudyInfo();
		DicomDataset GetDicomSeriesInfo();


	private slots:
		void onStudySelected(DicomDataset dcmStudySelected);
		void onSeriesSelected(DicomDataset dcmSeriesSelected);

	private:
		PACSSearchDownloadWidget* m_widget;
		DicomDataset m_dcmStudyInfo;
		DicomDataset m_dcmSeriesInfo;
	};
}
