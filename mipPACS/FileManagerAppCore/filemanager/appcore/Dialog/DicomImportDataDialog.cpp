#include "stdafx.h"
#include "DicomImportDataDialog.h"
#include "filemanager/dicom/DicomInfomationModelLoader.h"
#include "filemanager/config/GUISettingConfig.h"
#include "filemanager/appcore/Dialog/Util/ProgressBarDialog.h"
#include <qmessagebox>
#include <qtablewidget>
#include <qpushbutton>
#include <QLineEdit>
#include <qtabwidget>
#include <qlayout>

namespace fm
{
	void DicomImportDataDialog::ProgressBarCancelCallBackFunction(void* pCallBackContext)
	{
		/* Abort 처리 진행 */
		DicomImportDataDialog* pDlg = (DicomImportDataDialog*)pCallBackContext;
		pDlg->m_pImportData->Abort();
	}

	DicomImportDataDialog::DicomImportDataDialog(
		FileManagerImportData* pOutImportData,
		GUISettingConfig* pGUISettingConfig,
		std::shared_ptr<DicomInfomationModel> pDicomInfomationModel,
		QWidget* parent) :
		QDialog(parent),
		m_pImportData(pOutImportData),
		m_pGUISettingConfig(pGUISettingConfig),
		m_pDicomInfomationModel(pDicomInfomationModel),
		m_pSeries(nullptr)
	{
		setupUi(this);

		m_pProgressBarDialog = new ProgressBarDialog(this);
		m_pProgressBarDialog->Initialize(this);

		connect(m_dicomPreviewViewer, &DicomPreviewWidget::currentSeriesChanged, this, &DicomImportDataDialog::onCurrentSeriesChanged);
		connect(m_dicomPreviewViewer, &DicomPreviewWidget::currentSeriesSelected, this, &DicomImportDataDialog::onCurrentSeriesSelected);

		connect(m_btnSelect, &QPushButton::clicked, this, &DicomImportDataDialog::onSelect);
		connect(m_btnCancel, &QPushButton::clicked, this, &DicomImportDataDialog::onCancel);

		connect(m_pImportData, &FileManagerImportData::setDicomDataStarted, this, &DicomImportDataDialog::onSetDicomDataStarted);
		connect(m_pImportData, &FileManagerImportData::setDicomDataInProgress, this, &DicomImportDataDialog::onSetDicomDataInProgress);
		connect(m_pImportData, &FileManagerImportData::setDicomDataFinished, this, &DicomImportDataDialog::onSetDicomDataFinished);

		connect(this, &QDialog::finished, this, &DicomImportDataDialog::onFinished);

		m_btnSelect->setStyleSheet(g_ResourceManager.GetButtonSelectFolder());
		m_btnCancel->setStyleSheet(g_ResourceManager.GetButtonCancel());

		/* Restore geometry */
		DicomImportDataDialogInfo info = m_pGUISettingConfig->GetDicomImportDataDialogInfo();
		restoreGeometry(info.Geometry);
		m_dicomPreviewViewer->RestoreBottomSplitter(info.BottomHorizontalContainerSplitterStatus);
		m_dicomPreviewViewer->RestoreCenterSplitter(info.CenterVerticalContainerSplitterStatus);

		m_dicomPreviewViewer->SetDicomInfomationModel(m_pDicomInfomationModel);
	}

	DicomImportDataDialog::~DicomImportDataDialog()
	{
	}

	DicomInfomationModelSeriesObject* DicomImportDataDialog::SelectedSeries()
	{
		return m_pSeries;
	}

	FileManagerImportData* DicomImportDataDialog::ImportData()
	{
		return m_pImportData;
	}

	void DicomImportDataDialog::showEvent(QShowEvent* e)
	{
		//QSize size = m_dicomPreviewViewer->size();
		QDialog::showEvent(e);
		m_dicomPreviewViewer->Update(true);
	}

	void DicomImportDataDialog::SetupImportDataByCurSelSeries_Async()
	{
		if (m_pSeries == nullptr)
		{
			return;
		}

		m_pImportData->SetDicomData_Async(m_pSeries);
	}

	void DicomImportDataDialog::onSetDicomDataStarted()
	{
		verticalLayout->setEnabled(false);
		m_pProgressBarDialog->SetText(QString::fromLocal8Bit("Initialize start"));
		m_pProgressBarDialog->SetMode(ProgressBarDialog::EMode::normal);
		m_pProgressBarDialog->SetProgress(0.0f);
		m_pProgressBarDialog->Show(ProgressBarCancelCallBackFunction, this);
	}

	void DicomImportDataDialog::onSetDicomDataInProgress(float progressRate)
	{
		QString text = QString::fromLocal8Bit("Initialize in progress...");
		m_pProgressBarDialog->SetText(text);
		m_pProgressBarDialog->SetProgress(progressRate);
	}

	void DicomImportDataDialog::onSetDicomDataFinished()
	{
		verticalLayout->setEnabled(true);
		m_pProgressBarDialog->Hide();

		/* DICOM 정보 Setting 완료 후 accept */
		accept();
	}

	void DicomImportDataDialog::onCurrentSeriesChanged(DicomInfomationModelSeriesObject* pSeries)
	{
		m_pSeries = pSeries;
	}

	void DicomImportDataDialog::onCurrentSeriesSelected(DicomInfomationModelSeriesObject* pSeries)
	{
		m_pSeries = pSeries;

		SetupImportDataByCurSelSeries_Async();
	}

	void DicomImportDataDialog::onSelect()
	{
		m_dicomPreviewViewer->SelectCurrentSeries();
	}

	void DicomImportDataDialog::onCancel()
	{
		reject();
	}

	void DicomImportDataDialog::onFinished()
	{
		/* Save Geometry */
		DicomImportDataDialogInfo info = m_pGUISettingConfig->GetDicomImportDataDialogInfo();

		info.Geometry = saveGeometry();
		info.BottomHorizontalContainerSplitterStatus = m_dicomPreviewViewer->SaveBottomSplitter();
		info.CenterVerticalContainerSplitterStatus = m_dicomPreviewViewer->SaveCenterSplitter();

		m_pGUISettingConfig->SetDicomImportDataDialogInfo(info);
		m_pGUISettingConfig->Save();

		m_dicomPreviewViewer->AbortAndWaitLoader();

		m_pDicomInfomationModel->Clear();
	}
}

