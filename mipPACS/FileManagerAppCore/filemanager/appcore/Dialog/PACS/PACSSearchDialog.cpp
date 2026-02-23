#include "stdafx.h"
#include "PACSSearchDialog.h"
#include "filemanager/appcore/SubWindow/PACSSearchDownloadWidget.h"
#include <qlayout>

namespace fm
{
	PACSSearchDialog::PACSSearchDialog(AppCoreContext* pContext, QWidget* parent) :
		QDialog(parent)
	{
		QHBoxLayout* layout = new QHBoxLayout();
		setLayout(layout);

		m_widget = new PACSSearchDownloadWidget(parent);
		
		m_widget->Init(pContext, EPACSOperationMode::only_search_mode, EPACSSearchMode::search_study);

		connect(m_widget, &PACSSearchDownloadWidget::studySelected, this, &PACSSearchDialog::onStudySelected);
		connect(m_widget, &PACSSearchDownloadWidget::seriesSelected, this, &PACSSearchDialog::onSeriesSelected);

		layout->addWidget(m_widget);

		resize(1000, 600);
	}

	void PACSSearchDialog::onStudySelected(DicomDataset dcmStudySelected)
	{
		m_dcmStudyInfo = dcmStudySelected;
		accept();
	}

	void PACSSearchDialog::onSeriesSelected(DicomDataset dcmSeriesSelected)
	{
		m_dcmSeriesInfo = dcmSeriesSelected;
		accept();
	}

	DicomDataset PACSSearchDialog::GetDicomStudyInfo()
	{
		return m_dcmStudyInfo;
	}

	DicomDataset PACSSearchDialog::GetDicomSeriesInfo()
	{
		return m_dcmSeriesInfo;
	}
}
