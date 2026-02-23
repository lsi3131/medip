#include "stdafx.h"
#include "PACSExportWidget.h"
#include "ui_PACSExportWidget.h"
#include "filemanager/appcore/UI/PACSListWidget.h"
#include "filemanager/appcore/SubWindow/FolderViewWidget.h"
#include "filemanager/appcore/UI/PACSDicomConvertUploadWidget.h"
#include "filemanager/appcore/AppCoreContext.h"
#include "filemanager/appcore/Event/EventManager.h"
#include "filemanager/appcore/Dialog/PACS/PACSSearchDialog.h"
#include "filemanager/dicom/DicomInfomationModel.h"
#include "filemanager/dicom/Convert/DicomConverter.h"
#include "filemanager/config/AppConfig.h"
#include "DicomReader.h"
#include <qfiledialog>
#include <qmessagebox>
#include <qpixmap>
#include <qdebug>

namespace fm
{
	//======================================
	//		PACSExportWidget
	//======================================
	PACSExportWidget::PACSExportWidget(QWidget* parent) :
		QWidget(parent)
	{
		m_ui = new Ui::PACSExportWidget();
		m_ui->setupUi(this);

		m_ui->m_previewWidget->SetSeriesListVisibile(false);
		m_ui->m_previewWidget->SetStudyListVisible(false);

		m_colHeaderList_PatientInfo.Add(COL_PATIENT_INFO_ITEMS, "Items", true);
		m_colHeaderList_PatientInfo.Add(COL_PATIENT_INFO_CONTENT, "Content", true);

		m_colHeaderList_ExportData.Add(COL_DATA_SERIES_SELECT, "", true);
		m_colHeaderList_ExportData.Add(COL_DATA_SERIES_DATE, "Date", true);
		m_colHeaderList_ExportData.Add(COL_DATA_SERIES_NUMBER, "Number", true);
		m_colHeaderList_ExportData.Add(COL_DATA_SERIES_DESCRIPTION, "Description", true);
		m_colHeaderList_ExportData.Add(COL_DATA_SERIES_COUNT, "Count", true);

		m_ui->m_tableExportPatientInfo->setColumnCount(m_colHeaderList_PatientInfo.Count());
		m_ui->m_tableExportPatientInfo->setHorizontalHeaderLabels(m_colHeaderList_PatientInfo.GetNameHeaderList());
		m_ui->m_tableExportPatientInfo->setEditTriggers(QAbstractItemView::NoEditTriggers);
		m_ui->m_tableExportPatientInfo->setAlternatingRowColors(true);
		m_ui->m_tableExportPatientInfo->setSelectionMode(QAbstractItemView::SingleSelection);
		m_ui->m_tableExportPatientInfo->setSelectionBehavior(QAbstractItemView::SelectRows);
		m_ui->m_tableExportPatientInfo->horizontalHeader()->setHighlightSections(false);

		m_ui->m_tableExportDataList->setColumnCount(m_colHeaderList_ExportData.Count());
		m_ui->m_tableExportDataList->setHorizontalHeaderLabels(m_colHeaderList_ExportData.GetNameHeaderList());
		m_ui->m_tableExportDataList->setEditTriggers(QAbstractItemView::NoEditTriggers);
		m_ui->m_tableExportDataList->setAlternatingRowColors(true);
		m_ui->m_tableExportDataList->setSelectionMode(QAbstractItemView::SingleSelection);
		m_ui->m_tableExportDataList->setSelectionBehavior(QAbstractItemView::SelectRows);
		m_ui->m_tableExportDataList->horizontalHeader()->setHighlightSections(false);
		connect(m_ui->m_tableExportDataList, &QTableWidget::currentItemChanged, this, &PACSExportWidget::onExportDataListCurrentItemChanged);

		connect(m_ui->m_btnExportToPACS, &QPushButton::clicked, this, &PACSExportWidget::onExportToPACS);

		connect(m_ui->m_btnLoadExportData_DICOM_Debug, &QPushButton::clicked, this, &PACSExportWidget::onLoadExportData_DICOM_Debug);
		connect(m_ui->m_btnLoadExportData_Image_Debug, &QPushButton::clicked, this, &PACSExportWidget::onLoadExportData_Image_Debug);
		connect(m_ui->m_btnLoadExportData_PDF_Debug, &QPushButton::clicked, this, &PACSExportWidget::onLoadExportData_PDF_Debug);
		connect(m_ui->m_btnLoadDICOMExportInfo_Local_Debug, &QPushButton::clicked, this, &PACSExportWidget::onLoadDICOMExportInfo_Local_Debug);
		connect(m_ui->m_btnLoadDICOMExportInfo_PACS_Debug, &QPushButton::clicked, this, &PACSExportWidget::onLoadDICOMExportInfo_PACS_Debug);

		connect(m_ui->m_cboExportPACSServer, SIGNAL(currentIndexChanged(int)), this, SLOT(onExportPACSServerChanged(int)));

		connect(&g_EventManager, &EventManager::dcmNet_Upload_Started, this, &PACSExportWidget::onDcmNetUploadStarted);
		connect(&g_EventManager, &EventManager::dcmNet_Upload_InProgress, this, &PACSExportWidget::onDcmNetUploadInProgress);
		connect(&g_EventManager, &EventManager::dcmNet_Upload_Finished, this, &PACSExportWidget::onDcmNetUploadFinished);
		connect(&g_EventManager, &EventManager::dcmNet_Error, this, &PACSExportWidget::onDcmNetError);

		connect(&g_EventManager, &EventManager::settingChanged, this, &PACSExportWidget::onSettingChanged);

		InitExportDicomInfoList();
	}

	PACSExportWidget::~PACSExportWidget()
	{
		delete m_ui;
	}

	bool PACSExportWidget::Init(AppCoreContext* pContext)
	{
		m_pContext = pContext;
		m_pPACSConfig = m_pContext->GetConfigManager()->GetPACSConfig();
		m_pDcmExportManager = m_pContext->GetDicomExportManager();

		InitLayout();
		Update();

		return true;
	}

	void PACSExportWidget::InitLayout()
	{
		GUISettingConfig* pConfig = m_pContext->GetConfigManager()->GetGUISettingConfig();
		PACSExportWidgetInfo info = pConfig->GetPACSExportWidget();

		m_ui->m_splitterCenter->restoreState(info.CenterVerticalContainerSplitterStatus);
		m_ui->m_splitterBottom->restoreState(info.BottomHorizontalContainerSplitterStatus);
	}

	void PACSExportWidget::UpdateExportDicomInfoTable()
	{
		DicomDataset dcmDataset = m_pDcmExportManager->GetDcmExportInfo();
		DICOM_HEADER_INFO dcmHeaderInfo = dcmDataset.ToDcmHeaderInfo();

		m_ui->m_tableExportPatientInfo->setItem(ROW_STUDY_INSTANCE_UID, COL_PATIENT_INFO_CONTENT,
			new QTableWidgetItem(QString::fromStdWString(dcmHeaderInfo.StudyInstanceUID)));
		m_ui->m_tableExportPatientInfo->setItem(ROW_PATIENT_ID, COL_PATIENT_INFO_CONTENT,
			new QTableWidgetItem(QString::fromStdWString(dcmHeaderInfo.PatientID)));
		m_ui->m_tableExportPatientInfo->setItem(ROW_PATIENT_NAME, COL_PATIENT_INFO_CONTENT,
			new QTableWidgetItem(QString::fromStdWString(dcmHeaderInfo.PatientName)));
		m_ui->m_tableExportPatientInfo->setItem(ROW_PATIENT_BIRTH_DATE, COL_PATIENT_INFO_CONTENT,
			new QTableWidgetItem(QString::fromStdString(dcmHeaderInfo.DateOfBirth.ToFormatText_DateTime())));
		m_ui->m_tableExportPatientInfo->setItem(ROW_PATIENT_SEX, COL_PATIENT_INFO_CONTENT,
			new QTableWidgetItem(QString::fromStdWString(dcmHeaderInfo.PatientSex)));
		m_ui->m_tableExportPatientInfo->setItem(ROW_PATIENT_AGE, COL_PATIENT_INFO_CONTENT,
			new QTableWidgetItem(QString::fromStdWString(dcmHeaderInfo.PatientAge)));
		m_ui->m_tableExportPatientInfo->setItem(ROW_STURY_DESCRIPTION, COL_PATIENT_INFO_CONTENT,
			new QTableWidgetItem(QString::fromStdWString(dcmHeaderInfo.StudyDescription)));
		m_ui->m_tableExportPatientInfo->setItem(ROW_STURY_DATETIME, COL_PATIENT_INFO_CONTENT,
			new QTableWidgetItem(QString::fromStdString(dcmHeaderInfo.StudyDateTime.ToFormatText_DateTime())));
		m_ui->m_tableExportPatientInfo->setItem(ROW_MODALITY, COL_PATIENT_INFO_CONTENT,
			new QTableWidgetItem(QString::fromStdWString(dcmHeaderInfo.Modality)));
		m_ui->m_tableExportPatientInfo->setItem(ROW_ACCESSION_NUMBER, COL_PATIENT_INFO_CONTENT,
			new QTableWidgetItem(QString::fromStdWString(dcmHeaderInfo.AccessionNumber)));
		m_ui->m_tableExportPatientInfo->setItem(ROW_EXAM_ID, COL_PATIENT_INFO_CONTENT,
			new QTableWidgetItem(QString::fromStdWString(dcmHeaderInfo.ExamID)));

		m_ui->m_tableExportPatientInfo->resizeColumnsToContents();
	}

	void PACSExportWidget::UpdateExportDataListTable()
	{
		std::vector<DicomInfomationModelSeriesObject*> seriesList = m_pDcmExportManager->GetSeriesList();

		int rowCount = seriesList.size();
		m_ui->m_tableExportDataList->setRowCount(rowCount);

		for (int rowIndex = 0; rowIndex < rowCount; ++rowIndex)
		{
			DicomDataset* pDcmDataset;
			QTableWidgetItem* item;
			if (seriesList[rowIndex]->GetFirst(&pDcmDataset))
			{
				QTableWidgetItem* tableWidgetItem = new QTableWidgetItem("");
				tableWidgetItem->setCheckState(Qt::CheckState::Checked);

				DICOM_HEADER_INFO dcmHeaderInfo = pDcmDataset->ToDcmHeaderInfo();

				m_ui->m_tableExportDataList->setItem(rowIndex, COL_DATA_SERIES_SELECT, tableWidgetItem);

				m_ui->m_tableExportDataList->setItem(rowIndex, COL_DATA_SERIES_DATE, new QTableWidgetItem(
					QString::fromStdString(dcmHeaderInfo.SeriesDateTime.ToFormatText_DateTime())
				));

				item = new QTableWidgetItem();
				item->setData(Qt::EditRole, dcmHeaderInfo.SeriesNumber);
				m_ui->m_tableExportDataList->setItem(rowIndex, COL_DATA_SERIES_NUMBER, item);

				m_ui->m_tableExportDataList->setItem(rowIndex, COL_DATA_SERIES_DESCRIPTION, new QTableWidgetItem(
					QString::fromStdWString(dcmHeaderInfo.SeriesDescription)
				));

				int count = seriesList[rowIndex]->ImageInstanceCount();
				m_ui->m_tableExportDataList->setItem(rowIndex, COL_DATA_SERIES_COUNT, new QTableWidgetItem(
					QString::number(count)
				));
			}
			else
			{
				qWarning() << "invalid export data";
			}
		}

		m_ui->m_tableExportDataList->resizeColumnsToContents();
	}

	void PACSExportWidget::UpdatePACSList()
	{
		std::vector<PACSConfig::ExportHost> exportHostList = m_pPACSConfig->GetExportHosts();
		m_ui->m_cboExportPACSServer->clear();
		for (int i = 0; i < exportHostList.size(); ++i)
		{
			m_ui->m_cboExportPACSServer->addItem(QString::fromStdWString(exportHostList[i].Name));
		}

		m_ui->m_cboExportPACSServer->setCurrentIndex(m_pPACSConfig->GetSelectedExportHost());
	}

	void PACSExportWidget::UpdatePreviewPage(DicomInfomationModelSeriesObject* pDcmSeries)
	{
		m_ui->m_previewWidget->SetDicomInfomationModel_Series(pDcmSeries);
	}

	void PACSExportWidget::UpdateControlEnableStatus()
	{
		std::vector<DicomInfomationModelSeriesObject*> selectedSeriesDataList;
		DicomHostInfo dcmHostInfo;

		if (m_pDcmExportManager->IsValidDicomExportInfo() == false)
		{
			m_ui->m_btnExportToPACS->setEnabled(false);
		}
		else if (GetCurrentSelectedSeriesDataList(selectedSeriesDataList) == false)
		{
			m_ui->m_btnExportToPACS->setEnabled(false);
		}
		else if (GetCurrentSelectedStoreDicomHostInfo(&dcmHostInfo) == false)
		{
			m_ui->m_btnExportToPACS->setEnabled(false);
		}
		else
		{
			m_ui->m_btnExportToPACS->setEnabled(true);
		}
	}

	void PACSExportWidget::UpdateByRuntimeMode()
	{
		bool isDebug = m_pContext->GetConfigManager()->GetAppConfig()->IsDebug();
		if (isDebug)
		{
			m_ui->m_groupDebug->setVisible(true);
			m_ui->m_btnLoadDICOMExportInfo_Local_Debug->setVisible(true);
		}
		else
		{
			m_ui->m_groupDebug->setVisible(false);
			m_ui->m_btnLoadDICOMExportInfo_Local_Debug->setVisible(false);
		}
	}

	void PACSExportWidget::InitExportDicomInfoList()
	{
		m_ui->m_tableExportPatientInfo->setRowCount(ROW_MAX_COUNT);
		m_ui->m_tableExportPatientInfo->setItem(ROW_STUDY_INSTANCE_UID, COL_PATIENT_INFO_ITEMS, new QTableWidgetItem("Study UID"));
		m_ui->m_tableExportPatientInfo->setItem(ROW_PATIENT_ID, COL_PATIENT_INFO_ITEMS, new QTableWidgetItem("PatientID"));
		m_ui->m_tableExportPatientInfo->setItem(ROW_PATIENT_NAME, COL_PATIENT_INFO_ITEMS, new QTableWidgetItem("PatientName"));
		m_ui->m_tableExportPatientInfo->setItem(ROW_PATIENT_BIRTH_DATE, COL_PATIENT_INFO_ITEMS, new QTableWidgetItem("BirthDate"));
		m_ui->m_tableExportPatientInfo->setItem(ROW_PATIENT_SEX, COL_PATIENT_INFO_ITEMS, new QTableWidgetItem("Sex"));
		m_ui->m_tableExportPatientInfo->setItem(ROW_PATIENT_AGE, COL_PATIENT_INFO_ITEMS, new QTableWidgetItem("Age"));
		m_ui->m_tableExportPatientInfo->setItem(ROW_STURY_DESCRIPTION, COL_PATIENT_INFO_ITEMS, new QTableWidgetItem("Study Description"));
		m_ui->m_tableExportPatientInfo->setItem(ROW_STURY_DATETIME, COL_PATIENT_INFO_ITEMS, new QTableWidgetItem("Study Date"));
		m_ui->m_tableExportPatientInfo->setItem(ROW_MODALITY, COL_PATIENT_INFO_ITEMS, new QTableWidgetItem("Modality"));
		m_ui->m_tableExportPatientInfo->setItem(ROW_ACCESSION_NUMBER, COL_PATIENT_INFO_ITEMS, new QTableWidgetItem("Accession Number"));
		m_ui->m_tableExportPatientInfo->setItem(ROW_EXAM_ID, COL_PATIENT_INFO_ITEMS, new QTableWidgetItem("Exam ID"));
	}

	bool PACSExportWidget::GetCurrentSelectedSeriesDataList(std::vector<DicomInfomationModelSeriesObject*>& selectedSeriesDataList)
	{
		selectedSeriesDataList.clear();

		std::vector<DicomInfomationModelSeriesObject*> seriesList = m_pDcmExportManager->GetSeriesList();

		int col = COL_DATA_SERIES_SELECT;
		for (int row = 0; row < seriesList.size(); ++row)
		{
			QTableWidgetItem* pItem = m_ui->m_tableExportDataList->item(row, col);
			Qt::CheckState state = pItem->checkState();
			if (state == Qt::CheckState::Checked)
			{
				selectedSeriesDataList.push_back(seriesList[row]);
			}
		}

		return selectedSeriesDataList.empty() == false;
	}

	bool PACSExportWidget::GetCurrentSelectedStoreDicomHostInfo(DicomHostInfo* pOutDicomHostInfo)
	{
		int index = m_ui->m_cboExportPACSServer->currentIndex();
		if (index < 0)
		{
			return false;
		}

		std::vector<PACSConfig::ExportHost> exportHostList = m_pPACSConfig->GetExportHosts();

		*pOutDicomHostInfo = DicomHostInfo::CreateStore(
			exportHostList[index].ApplicationEntity,
			exportHostList[index].IP,
			exportHostList[index].TransferSyntax,
			exportHostList[index].Port,
			exportHostList[index].MaxAssoc,
			m_pPACSConfig->GetTimeout()
		);

		return true;
	}

	void PACSExportWidget::Update()
	{
		UpdateExportDicomInfoTable();
		UpdateExportDataListTable();
		UpdatePACSList();
		UpdateControlEnableStatus();
		UpdateByRuntimeMode();
	}

	void PACSExportWidget::Refresh()
	{
		UpdateExportDicomInfoTable();
		UpdateExportDataListTable();
		UpdateControlEnableStatus();
	}

	void PACSExportWidget::SaveStatus()
	{
		GUISettingConfig* pConfig = m_pContext->GetConfigManager()->GetGUISettingConfig();
		PACSExportWidgetInfo info = pConfig->GetPACSExportWidget();

		info.CenterVerticalContainerSplitterStatus = m_ui->m_splitterCenter->saveState();
		info.BottomHorizontalContainerSplitterStatus = m_ui->m_splitterBottom->saveState();

		pConfig->SetPACSExportWidget(info);
		pConfig->Save();
	}

	void PACSExportWidget::onSettingChanged()
	{
		UpdatePACSList();
		UpdateByRuntimeMode();
	}

	void PACSExportWidget::onPACSSelectionChanged()
	{
	}

	void PACSExportWidget::onExportDataListCurrentItemChanged(QTableWidgetItem* current, QTableWidgetItem* previous)
	{
		int rowIdx = m_ui->m_tableExportDataList->currentRow();
		if (rowIdx >= 0)
		{
			std::vector<DicomInfomationModelSeriesObject*> seriesList = m_pDcmExportManager->GetSeriesList();
			UpdatePreviewPage(seriesList[rowIdx]);
			UpdateControlEnableStatus();
		}
	}

	void PACSExportWidget::onExportToPACS()
	{
		QMessageBox::StandardButton button = QMessageBox::warning(this,
			QString::fromLocal8Bit("Upload"),
			QString::fromLocal8Bit("Upload를 진행하시겠습니까?"),
			QMessageBox::StandardButton::Ok | QMessageBox::StandardButton::Cancel
		);

		if (button == QMessageBox::StandardButton::Cancel)
		{
			return;
		}

		/* 1. Export 데이터 및 Config 정보를 초기화 한다. */
		std::vector<DicomInfomationModelSeriesObject*> seriesInfoList;
		if (GetCurrentSelectedSeriesDataList(seriesInfoList) == false)
		{
			qWarning() << "fail to export";
			QMessageBox::critical(this, "FAIL", "fail to export image data");
			return;
		}

		DicomHostInfo dcmHostInfo;
		if (GetCurrentSelectedStoreDicomHostInfo(&dcmHostInfo) == false)
		{
			QMessageBox::critical(this, "FAIL", "fail to export image data");
			return;
		}

		m_pDcmExportManager->Export_Async(dcmHostInfo, seriesInfoList);
	}

	void PACSExportWidget::onLoadExportData_DICOM_Debug()
	{
		QString sampleDataDir = QApplication::applicationDirPath() + "/SampleData/dcm";
		QString dirpath = QFileDialog::getExistingDirectory(
			this,
			"",
			sampleDataDir);

		std::vector<DicomDataset> dcmDatasetList;
		if (DicomDataset::LoadList(dcmDatasetList, dirpath))
		{
			DicomExportData exportData;
			for (auto& dcmDataset : dcmDatasetList)
			{
				exportData.AddDicomDataset(dcmDataset);
			}
			m_pDcmExportManager->AddExportData(exportData);
		}

		UpdateExportDataListTable();
	}

	void PACSExportWidget::onLoadExportData_Image_Debug()
	{
		QString filters = "All(*.png;*.bmp;*.jpg)";
		QString sampleDataDir = QApplication::applicationDirPath() + "/SampleData/image/default";
		QString filePath = QFileDialog::getOpenFileName(
			this,
			QString(),
			sampleDataDir,
			filters
		);

		if (filePath.isEmpty())
		{
			return;
		}

		QImage image;
		if (image.load(filePath) == false)
		{
			return;
		}

		DicomExportData dcmExportData;
		std::vector<QImage> imageData = { image };
		dcmExportData.AddImageList(imageData);
		m_pDcmExportManager->AddExportData(dcmExportData);

		UpdateExportDataListTable();
		UpdateControlEnableStatus();
	}

	void PACSExportWidget::onLoadExportData_PDF_Debug()
	{
		QString filters = "PDF(*.pdf)";
		QString sampleDataDir = QApplication::applicationDirPath() + "/SampleData/pdf";
		QString filePath = QFileDialog::getOpenFileName(
			this,
			QString(),
			sampleDataDir,
			filters
		);

		if (filePath.isEmpty())
		{
			return;
		}

		DicomExportData dcmExportData;
		QStringList filePathList = { filePath };
		dcmExportData.AddPdfFilePathList(filePathList);
		m_pDcmExportManager->AddExportData(dcmExportData);

		UpdateExportDataListTable();
		UpdateControlEnableStatus();
	}

	void PACSExportWidget::onLoadDICOMExportInfo_Local_Debug()
	{
		QString filters = "DICOM(*.dcm)";
		QString sampleDataDir = QApplication::applicationDirPath() + "/SampleData/dcm/default";
		QString filePath = QFileDialog::getOpenFileName(
			this,
			QString(),
			sampleDataDir,
			filters
		);

		if (filePath.isEmpty())
		{
			return;
		}

		DicomDataset dataset;
		if (dataset.LoadFromFile(filePath) == false)
		{
			return;
		}

		m_pDcmExportManager->SetExportInfo(dataset);

		UpdateExportDicomInfoTable();
		UpdateControlEnableStatus();
	}

	void PACSExportWidget::onLoadDICOMExportInfo_PACS_Debug()
	{
		PACSSearchDialog dlg(m_pContext, this);
		int result = dlg.exec();

		if (result == QDialog::Accepted)
		{
			DicomDataset dcmDataset = dlg.GetDicomStudyInfo();
			m_pDcmExportManager->SetExportInfo(dcmDataset);

			UpdateExportDicomInfoTable();
			UpdateControlEnableStatus();
		}
	}


	void PACSExportWidget::onDcmNetUploadStarted()
	{
		setEnabled(false);
	}

	void PACSExportWidget::onDcmNetUploadInProgress(DicomNetworkSCUStatus status)
	{

	}

	void PACSExportWidget::onDcmNetUploadFinished()
	{
		setEnabled(true);
	}

	void PACSExportWidget::onDcmNetError(QString errorMessage)
	{
		setEnabled(true);
	}

	void PACSExportWidget::onExportPACSServerChanged(int index)
	{
		if (index >= 0)
		{
			m_pPACSConfig->ModifySelectedExportHost(index);
		}
	}
}

