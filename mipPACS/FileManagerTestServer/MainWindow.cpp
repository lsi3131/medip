#include "stdafx.h"
#include "MainWindow.h"
#include "TestServerContext.h"
#include "filemanager/net//Server.h"
#include "filemanager/appcore/Dialog/PACS/PACSSearchDialog.h"
#include "filemanager/appcore/Dialog/DicomVolumeViewerDialog.h"
#include "filemanager/appcore/Util/StringUtil.h"
#include "filemanager/dicom/DicomDataset.h"
#include "filemanager/dicom/Export/DicomExportData.h"
#include "filemanager/data/Image/VolumeImageData.h"
#include "Dialog/TestDicomLoaderDialog.h"
#include "DicomReader.h"
#include <qfiledialog>
#include <qmessagebox>

using namespace fm;

void MainWindow::LogCallBack(void* pContext, const char* message)
{
	MainWindow* pMain = (MainWindow*)pContext;
	pMain->listLogger->addItem(QString(message));
	pMain->listLogger->setCurrentRow(pMain->listLogger->count() - 1);
}

MainWindow::MainWindow(QWidget* parent) :
	QMainWindow(parent)
{
	setupUi(this);
	//m_editAppName->setText("MEDIP");
	updateControls();

	m_pFileManager = new FileManagerAppCore();
	InitializeFileManagerAppCore();

	initExportDicomTable();

	Logger::AddCallBack(LogCallBack, this);

	cboServerToClientCommand->addItem(GetText_EServerCommand(EServerCommand::update), (int)EServerCommand::update);

	connect(btnServerStart, &QPushButton::clicked, this, &MainWindow::onServerStart);
	connect(btnServerDisconnect, &QPushButton::clicked, this, &MainWindow::onServerDisconnect);

	connect(btnOpenClient, &QPushButton::clicked, this, &MainWindow::onOpenClient);
	connect(btnSendToClient, &QPushButton::clicked, this, &MainWindow::onSendToClient);

	connect(g_testServerContext.GetServer(), &Server::updateFromClient, this, &MainWindow::onUpdateFromClient);
	connect(g_testServerContext.GetServer(), &Server::clientConnected, this, &MainWindow::onClientSocketStatusUpdate);
	connect(g_testServerContext.GetServer(), &Server::clientDisconnected, this, &MainWindow::onClientSocketStatusUpdate);


	/* Dialog */
	connect(m_btnInitialize, &QPushButton::clicked, this, &MainWindow::onInitialize);

	connect(m_btnOpenFile, &QPushButton::clicked, this, &MainWindow::onOpenFileDialog);
	connect(m_btnOpenFileList, &QPushButton::clicked, this, &MainWindow::onOpenFileListDialog);
	connect(m_btnImportDefault, &QPushButton::clicked, this, &MainWindow::onImportDefaultDialog);
	connect(m_btnImportDicom, &QPushButton::clicked, this, &MainWindow::onImportDicomDialog);
	connect(m_btnOpenExportDialog, &QPushButton::clicked, this, &MainWindow::onSaveFileDialog);
	connect(m_btnOpenExportDirectoryDialog, &QPushButton::clicked, this, &MainWindow::onExportDirectoryDialog);
	connect(m_btnExportToPDF, &QPushButton::clicked, this, &MainWindow::onExportToPDF);
	connect(m_btnExportToPACS, &QPushButton::clicked, this, &MainWindow::onExportToPACS);
	connect(m_btnDebug, &QPushButton::clicked, this, &MainWindow::onDebug);
	connect(m_btnDicomLoader_Test, &QPushButton::clicked, this, &MainWindow::onDicomLoader_Test);

	connect(m_btnLoadExportPatientInfo, &QPushButton::clicked, this, &MainWindow::onLoadExportPatientInfo);
	connect(m_btnLoadExportImageDataList, &QPushButton::clicked, this, &MainWindow::onLoadExportToPACSImageData);


	connect(m_btnPACSSearch, &QPushButton::clicked, this, &MainWindow::onPACSSearch);

	/* default PDF 정보 준비 */
	QTextDocument* pDocument = m_PDFText->document();
	m_exportData.AddPdfDocument({ pDocument });
}

void MainWindow::onUpdateFromClient(fm::CLIENT_TO_SERVER_INFO info)
{
	activateWindow();
	raise();

	editRecvFilePath->setText(info.FilePath);
	editRecvCommand->setText(GetText_EClientCommand(info.Command));
}

void MainWindow::onClientSocketStatusUpdate()
{
	updateControls();
}

void MainWindow::onOpenFileDialog()
{
	FileManagerDialog* pDlg;
	if (m_pFileManager->GetDialog(pDlg) == false)
	{
		return;
	}

	QString filepath = pDlg->GetImportOpenFilePath(GetImportFilterList());
	m_textFilePathList->clear();
	m_textFilePathList->setText(filepath);
}

void MainWindow::onOpenFileListDialog()
{
	FileManagerDialog* pDlg;
	if (m_pFileManager->GetDialog(pDlg) == false)
	{
		return;
	}

	QStringList filepathList = pDlg->GetImportOpenFilePathList(GetImportFilterList());
	m_textFilePathList->clear();

	QString filePathText;
	for (QString s : filepathList)
	{
		filePathText += s + "\r\n";
	}
	m_textFilePathList->setText(filePathText);
}

void MainWindow::onImportDefaultDialog()
{
	FileManagerDialog* pDlg;
	if (m_pFileManager->GetDialog(pDlg) == false)
	{
		return;
	}

	QString mipFilters = GetMIPFilterList();
	QString importFilters = GetImportFilterList();
	//QString defaultSelectFilter = "MEDIP File(*.mip; *.MIP)";
	QString defaultSelectFilter = "DeepCatch File(*.mipd; *.MIPD)";

	FileManagerImportData importData;
	if (pDlg->GetImportDefaultData(&importData, mipFilters, importFilters, defaultSelectFilter))
	{
		m_textFilePathList->clear();
		QString text;
		EFileManagerImportDataType type = importData.GetImportDataType();
		if (type == EFileManagerImportDataType::mip_project)
		{
			text = "==================mip file path======================\r\n";
			text += importData.GetMIPProjectFilePath();
		}
		else if (type == EFileManagerImportDataType::dicom_data)
		{
			text = "==================dicom data loaded======================\r\n";
		}
		else if (type == EFileManagerImportDataType::import_file_list)
		{
			text = "==================import file path list======================\r\n";
			for (QString& path : importData.GetImportFilePathList())
			{
				text += (path + "\r\n");
			}
		}
		m_textFilePathList->setText(text);
	}
}

void MainWindow::onImportDicomDialog()
{
	FileManagerDialog* pDlg;
	if (m_pFileManager->GetDialog(pDlg) == false)
	{
		return;
	}

	FileManagerImportData importData;
	if (pDlg->GetImportDicomData(&importData))
	{

		m_textFilePathList->clear();
		QString text;
		EFileManagerImportDataType type = importData.GetImportDataType();
		if (type == EFileManagerImportDataType::mip_project)
		{
			qDebug() << "invalid type";
		}
		else if (type == EFileManagerImportDataType::dicom_data)
		{
			text = "==================dicom data loaded======================\r\n";

			fm::VolumeImageData volume;
			DicomVolumeInfo* pVolumeInfo = importData.GetDcmVolumeInfo();
			if (volume.Init(importData.Data(), pVolumeInfo->dx, pVolumeInfo->dy, pVolumeInfo->dz) == false)
			{
				qWarning() << "invalid volume data";
				return;
			}

			DicomVolumeViewerDialog dlg(&volume, this);
			dlg.exec();
		}
		else if (type == EFileManagerImportDataType::import_file_list)
		{
			qDebug() << "invalid type";
		}
		m_textFilePathList->setText(text);
	}
}

void MainWindow::onSaveFileDialog()
{
	FileManagerDialog* pDlg;
	if (m_pFileManager->GetDialog(pDlg) == false)
	{
		return;
	}

	QString filepath = pDlg->GetExportSaveFilePath(GetImportFilterList());

	m_textFilePathList->clear();
	m_textFilePathList->setText(filepath);
}

void MainWindow::onExportDirectoryDialog()
{
	FileManagerDialog* pDlg;
	if (m_pFileManager->GetDialog(pDlg) == false)
	{
		return;
	}

	QString exportDirpath = pDlg->GetExportDirectoryPath();

	m_textFilePathList->clear();
	m_textFilePathList->setText(exportDirpath);
}

void MainWindow::onExportToPDF()
{
	FileManagerDialog* pDlg;
	if (m_pFileManager->GetDialog(pDlg) == false)
	{
		return;
	}

	QString filepath = pDlg->GetExportPDF(m_PDFText->document(), GetPDFFilterList());
	m_textFilePathList->clear();
	m_textFilePathList->setText(filepath);
}

void MainWindow::onExportToPACS()
{
	FileManagerDialog* pDlg;
	if (m_pFileManager->GetDialog(pDlg) == false)
	{
		return;
	}

	DcmtkSeriesInfo seriesInfo;
	QString studyInstanceUID = m_tableExportDicom->item((int)eRow::StudyInstance, (int)eColumn::Value)->text();
	seriesInfo.strStudyUID = studyInstanceUID.toStdString();
	m_exportData.SetExportDicomInfo(&seriesInfo);

	pDlg->ExportToPACS(&m_exportData);
}

void MainWindow::onDebug()
{
	FileManagerDialog* pDlg;
	if (m_pFileManager->GetDialog(pDlg) == false)
	{
		return;
	}

	//FileManagerExportData 
	//exportData.AddImage();
	//m_exportData.GetData();

	pDlg->Debug();
	qDebug() << "debug";
}

void MainWindow::onLoadExportPatientInfo()
{
	QString filters = "dcm(*.dcm)";
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

	fm::DicomDataset dataset;
	bool result = dataset.LoadFromFile(filePath);

	DcmtkSeriesInfo seriesInfo;
	seriesInfo.strStudyUID = fm::StringUtil::WideToMulitiByte(dataset.GetValueWString(fm::DicomTagID::StudyInstanceUID));
	seriesInfo.patientsName_ = fm::StringUtil::WideToMulitiByte(dataset.GetValueWString(fm::DicomTagID::PatientName));
	seriesInfo.patientId_ = fm::StringUtil::WideToMulitiByte(dataset.GetValueWString(fm::DicomTagID::PatientID));
	seriesInfo.patientSize_ = fm::StringUtil::WideToMulitiByte(dataset.GetValueWString(fm::DicomTagID::PatientSize));
	seriesInfo.patientWeight_ = fm::StringUtil::WideToMulitiByte(dataset.GetValueWString(fm::DicomTagID::PatientWeight));
	seriesInfo.birthday_ = fm::StringUtil::WideToMulitiByte(dataset.GetValueWString(fm::DicomTagID::PatientBirthDate));
	seriesInfo.age_ = fm::StringUtil::WideToMulitiByte(dataset.GetValueWString(fm::DicomTagID::PatientAge));
	seriesInfo.sex_ = fm::StringUtil::WideToMulitiByte(dataset.GetValueWString(fm::DicomTagID::PatientSex));

	if (result)
	{
		m_exportData.SetExportDicomInfo(&seriesInfo);
		UpdateExportPatientInfoList();
	}
}

void MainWindow::onLoadExportToPACSImageData()
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

	static int index = 0;
	m_exportData.AddImage(image);

	UpdateExportImageList();
}

void MainWindow::onPACSSearch()
{
	PACSSearchDialog dlg(m_pFileManager->GetContext(), this);
	dlg.exec();
}

void MainWindow::onDicomLoader_Test()
{
	TestDicomLoaderDialog dlg(this);
	dlg.exec();
}

void MainWindow::onInitialize()
{
	if (InitializeFileManagerAppCore())
	{
		QMessageBox::information(
			this,
			"INFO",
			"initialize success"
		);
	}
}

void MainWindow::onServerStart()
{
	g_testServerContext.GetServer()->StartServer();
	updateControls();
}
void MainWindow::onServerDisconnect()
{
	g_testServerContext.GetServer()->StopServer();
	updateControls();
}

void MainWindow::onOpenClient()
{
	qInfo() << "try to open client";

	if (g_testServerContext.GetServer()->OpenClient(QStringList()) == false)
	{
		qInfo() << "fail to open client";
	}
}

void MainWindow::onSendToClient()
{
	EServerCommand cmd = (EServerCommand)cboServerToClientCommand->currentData(Qt::UserRole).toInt();

	switch (cmd)
	{
	case EServerCommand::update:
		if (g_testServerContext.GetServer()->Send_Update(""))
		{
		}
		break;
	default:
		qWarning() << "invalid command : " << (int)cmd;
		break;
	}
}

void MainWindow::initExportDicomTable()
{
	QStringList headers;
	headers << "Name";
	headers << "Value";
	m_tableExportDicom->setColumnCount(headers.size());
	m_tableExportDicom->setHorizontalHeaderLabels(headers);
	m_tableExportDicom->setRowCount(10);
	m_tableExportDicom->setItem((int)eRow::StudyInstance, (int)eColumn::Name, new QTableWidgetItem("StudyInstanceUID"));
	m_tableExportDicom->setItem((int)eRow::StudyInstance, (int)eColumn::Value, new QTableWidgetItem("123.456"));
}

bool MainWindow::InitializeFileManagerAppCore()
{
	bool canPACSDownload = CanDownloadPACS();
	bool canPACSUpload = CanUploadPACS();
	fm::EProductType productType = GetProductType();
	fm::ProductFunctionType productFunctionType(productType, canPACSDownload, canPACSUpload);
	fm::ELanguageType languageType = GetLanguageType();
	QString appName = GetAppName();
	QIcon icon(":/Resource/logo2.png");

	m_pFileManager->Initialize(appName, productFunctionType, icon, languageType);
	return true;
}

void MainWindow::updateControls()
{
	m_cboLanguage->addItem("KOR", qVariantFromValue<int>((int)fm::ELanguageType::KOR));
	m_cboLanguage->addItem("ENG", qVariantFromValue<int>((int)fm::ELanguageType::ENG));

	if (g_testServerContext.GetServer()->IsServerRunning())
	{
		editServerStatus->setText("Test Server is running");
		btnServerStart->setEnabled(false);
		btnServerDisconnect->setEnabled(true);
	}
	else
	{
		editServerStatus->setText("Test Server is stop");
		btnServerStart->setEnabled(true);
		btnServerDisconnect->setEnabled(false);
	}
	int clientCount = g_testServerContext.GetServer()->GetConnectedClientCount();
	editConnectedClient->setText(QString::number(clientCount));
}

void MainWindow::UpdateExportImageList()
{
	m_listExportImage->clear();

	DicomExportData* pExportDataList = m_exportData.GetDicomExportData();
	pExportDataList->GetImageListContainer();
	//for (int i = 0; i < pExportDataList->DataList.size(); ++i)
	//{
	//	if (pExportDataList->DataList[i].GetType() == fm::EExportDataType::image)
	//	{
	//		m_listExportImage->addItem(pExportDataList->DataList[i].GetName());
	//	}
	//}
}

void MainWindow::UpdateExportPatientInfoList()
{
	m_listExportPatientInfo->clear();

	//m_listExportPatientInfo->addItem(QString("Study UID : %1").arg(m_exportData.Data()->GetDicomTagValue(DicomTagID::StudyInstanceUID)));
	//m_listExportPatientInfo->addItem(QString("Patient Name : %1").arg(m_exportData.Data()->GetDicomTagValue(DicomTagID::PatientName)));
	//m_listExportPatientInfo->addItem(QString("Patient ID : %1").arg(m_exportData.Data()->GetDicomTagValue(DicomTagID::PatientID)));
	//m_listExportPatientInfo->addItem(QString("Patient Size : %1").arg(m_exportData.Data()->GetDicomTagValue(DicomTagID::PatientSize)));
	//m_listExportPatientInfo->addItem(QString("Patient Weight : %1").arg(m_exportData.Data()->GetDicomTagValue(DicomTagID::PatientWeight)));
	//m_listExportPatientInfo->addItem(QString("Patient BirthDate : %1").arg(m_exportData.Data()->GetDicomTagValue(DicomTagID::PatientBirthDate)));
	//m_listExportPatientInfo->addItem(QString("Patient Age : %1").arg(m_exportData.Data()->GetDicomTagValue(DicomTagID::PatientAge)));
	//m_listExportPatientInfo->addItem(QString("Patient Sex : %1").arg(m_exportData.Data()->GetDicomTagValue(DicomTagID::PatientSex)));
}

QString MainWindow::GetMIPFilterList()
{
	QString filters;

	filters += "MEDIP File(*.mip; *.MIP);;";
	filters += "DeepCatch File(*.mipd; *.MIPD);;";
	filters += "MEDIP AI File(*.mipa; *.MIPA)";

	return filters;
}

QString MainWindow::GetImportFilterList()
{
	//return tr("All(*.*);;DCM(*.dcm);;MIP(*.mip);;STL(*.stl)");
	return tr("DCM(*.dcm);;MIP(*.mip);;STL(*.stl);;All(*)");
}

QString MainWindow::GetPDFFilterList()
{
	return tr("HTML(*.html);;pdf(*.pdf)");
}

QString MainWindow::GetAppName()
{
	return m_editAppName->text();
}

bool MainWindow::CanDownloadPACS()
{
	return m_chkPACSDownload->isChecked();
}

bool MainWindow::CanUploadPACS()
{
	return m_chkPACSUpload->isChecked();
}

fm::EProductType MainWindow::GetProductType()
{
	if (m_rdoProductionType_Medip->isChecked())
	{
		return fm::EProductType::MEDIP;
	}
	else if (m_rdoProductionType_DeepCatch->isChecked())
	{
		return fm::EProductType::DEEPCATCH;
	}
	else
	{
		Q_ASSERT(false);
		return fm::EProductType::MEDIP;
	}
}

fm::ELanguageType MainWindow::GetLanguageType()
{
	QVariant var = m_cboLanguage->currentData(Qt::UserRole);
	return (fm::ELanguageType)var.toInt();
}

