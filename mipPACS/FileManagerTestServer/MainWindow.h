#pragma once

#include <qdialog>
#include "ui_MainWindow.h"
#include "filemanager/net/defines.h"
#include "filemanager/FileManagerAppCore.h"

class MainWindow : public QMainWindow, public Ui::MainWindow
{
	Q_OBJECT
public:
	enum class eColumn
	{
		Name = 0,
		Value,
	};

	enum class eRow
	{
		StudyInstance = 0,
	};

public:
	MainWindow(QWidget* parent = nullptr);

public:
	static void LogCallBack(void* pContext, const char* message);

private slots:
	void onInitialize();

	void onServerStart();
	void onServerDisconnect();

	void onOpenClient();
	void onSendToClient();

	void onUpdateFromClient(fm::CLIENT_TO_SERVER_INFO info);
	void onClientSocketStatusUpdate();

	void onOpenFileDialog();
	void onOpenFileListDialog();
	void onImportDefaultDialog();
	void onImportDicomDialog();
	void onSaveFileDialog();
	void onExportDirectoryDialog();
	void onExportToPDF();
	void onExportToPACS();
	void onDebug();

	void onLoadExportPatientInfo();
	void onLoadExportToPACSImageData();

	void onPACSSearch();
	void onDicomLoader_Test();

private:
	void initExportDicomTable();

	bool InitializeFileManagerAppCore();
	void updateControls();
	void UpdateExportImageList();
	void UpdateExportPatientInfoList();

	QString GetMIPFilterList();
	QString GetImportFilterList();
	QString GetPDFFilterList();

	QString GetAppName();
	bool CanDownloadPACS();
	bool CanUploadPACS();
	fm::EProductType GetProductType();
	fm::ELanguageType GetLanguageType();

private:
	fm::FileManagerAppCore* m_pFileManager;
	fm::FileManagerExportData m_exportData;
};

