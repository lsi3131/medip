#pragma once

#include <qdialog>
#include "ui_DicomImportDataDialog.h"
#include "filemanager/FileManagerImportData.h"

namespace fm
{
	class ProgressBarDialog;
	class DicomInfomationModelLoader;
	class DicomInfomationModelSeriesObject;
	class DicomInfomationModel;
	class GUISettingConfig;

	class FM_CORE_EXPORT DicomImportDataDialog : public QDialog, public Ui::DicomImportDataDialog
	{
		Q_OBJECT
	public:
		static void ProgressBarCancelCallBackFunction(void* pCallBackContext);

	public:
		DicomImportDataDialog(
			FileManagerImportData* pOutImportData,
			GUISettingConfig* pGUISettingConfig,
			std::shared_ptr<DicomInfomationModel> pDicomInfomationModel,
			QWidget* parent = nullptr);
		virtual ~DicomImportDataDialog();

	public:
		DicomInfomationModelSeriesObject* SelectedSeries();
		FileManagerImportData* ImportData();

	protected:
		void showEvent(QShowEvent* e) override;

	private:
		void SetupImportDataByCurSelSeries_Async();

	private slots:
		void onSetDicomDataStarted();
		void onSetDicomDataInProgress(float progressRate);
		void onSetDicomDataFinished();

		void onCurrentSeriesChanged(DicomInfomationModelSeriesObject* pSeries);
		void onCurrentSeriesSelected(DicomInfomationModelSeriesObject* pSeries);

		void onSelect();
		void onCancel();

		void onFinished();

	private:
		QString m_filepath;
		DicomInfomationModelSeriesObject* m_pSeries;
		FileManagerImportData* m_pImportData;
		std::shared_ptr<DicomInfomationModel> m_pDicomInfomationModel;

		GUISettingConfig* m_pGUISettingConfig;
		ProgressBarDialog* m_pProgressBarDialog;
	};
}

