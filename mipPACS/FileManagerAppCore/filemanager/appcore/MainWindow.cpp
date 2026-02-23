#include "stdafx.h"
#include "MainWindow.h"
#include "AppCoreContext.h"
#include "Dialog/Debug/TestServerClientCommunicationDialog.h"
#include "Dialog/Debug/StatusMonitorDialog.h"
#include "Dialog/Debug/TestDicomNetworkDialog.h"
#include "Dialog/Util/ProgressBarDialog.h"
#include "FileManager/appcore/System/ShortcutManager.h"
#include "MainWidget.h"
#include <qmenubar>
#include <QCloseEvent>
#include <qmessagebox>

namespace fm
{

	MainWindow::MainWindow(AppCoreContext* pContext, QWidget* parent)
		: QMainWindow(parent),
		m_pAppCoreContext(pContext),
		m_forceToClose(false)
	{
		resize(1450, 726);
		setObjectName("MainWindow");
		setWindowTitle("FileManager");

		//=================Widget Init ================
		m_pMainWidget = new MainWidget(m_pAppCoreContext, this);
		setCentralWidget(m_pMainWidget);

		/* ============ Menu 초기화 ============= */
		QMenu * menuDebug = this->menuBar()->addMenu("Test");

		m_actionDebug_ServerClientCommunication = new QAction("Server Client Communication", this);
		m_actionDebug_StatusMonitor = new QAction("Status Monitor", this);
		m_actionDebug_DICOMNetwork = new QAction("DICOM Network", this);

		connect(m_actionDebug_ServerClientCommunication, &QAction::triggered, this, &MainWindow::onDebugServerClientCommunication);
		connect(m_actionDebug_StatusMonitor, &QAction::triggered, this, &MainWindow::onDebugStatusMonitor);
		connect(m_actionDebug_DICOMNetwork, &QAction::triggered, this, &MainWindow::onDebugDICOMNetwork);

		menuDebug->addAction(m_actionDebug_ServerClientCommunication);
		menuDebug->addAction(m_actionDebug_StatusMonitor);
		menuDebug->addAction(m_actionDebug_DICOMNetwork);

		//this->menuBar()->hide();

		//==============================END===============
		SHORTCUT_MANAGER->Initialize(this);

		connect(SHORTCUT_MANAGER->Action_Close(), &QAction::triggered, this, &MainWindow::onClose);

		setWindowIcon(QIcon(":/MedicalIPFileManager.ico"));

		m_pStatusMonitorDialog = new StatusMonitorDialog(m_pAppCoreContext);
		m_pStatusMonitorDialog->hide();

		m_pDicomNetworkDialog = new TestDicomNetworkDialog(m_pAppCoreContext->GetDicomNetworkManager());
		m_pDicomNetworkDialog->hide();

		m_pMainWidget->UpdateControlByNavRadioButton();
	}

	void MainWindow::closeEvent(QCloseEvent * e)
	{
		/*
		Hidden 상태에서 종료 -> App 종료
		Show 상태에서 종료 -> Hide()
		*/
		bool isDebugMode = m_pAppCoreContext->GetConfigManager()->GetAppConfig()->IsDebug();

		if (isHidden() || m_forceToClose || isDebugMode)
		{
			QMainWindow::closeEvent(e);
		}
		else
		{
			hide();
			e->ignore();
		}
	}

	bool MainWindow::event(QEvent * e)
	{
		return QMainWindow::event(e);
	}

	void MainWindow::showEvent(QShowEvent *e)
	{
		QMainWindow::showEvent(e);
	}

	void MainWindow::hideEvent(QHideEvent *e)
	{
		if (m_blockHideEvent)
		{
			//Flag에 따라 Hide 
		}
		else
		{
			QMainWindow::hideEvent(e);
		}
	}

	void MainWindow::onUpdateConnectToServer()
	{
		qInfo() << "now this app is connectted to server";
	}

	void MainWindow::onUpdateDisconnectedFromServer()
	{
		bool isDebug = m_pAppCoreContext->GetConfigManager()->GetAppConfig()->IsDebug();

		if (isDebug)
		{
			qInfo() << "Debug mode is skip to close server";
		}
		else 
		{
			qInfo() << "disconnected from server. this application will be closed";
			m_forceToClose = true;
			close();
		}
	}

	void MainWindow::onDebugServerClientCommunication()
	{
		TestServerClientCommunicationDialog dlg(m_pAppCoreContext, this);
		dlg.exec();
	}

	void MainWindow::onDebugStatusMonitor()
	{
		m_pStatusMonitorDialog->show();
	}

	void MainWindow::onDebugDICOMNetwork()
	{
		m_pDicomNetworkDialog->show();
	}

	void MainWindow::onClose()
	{
		close();
	}

	void MainWindow::SetWindowOnTop()
	{
		/*
		해당 함수 호출하면 hideEvent가 발생됨,
		hideEvent를 막기 위해서 flag 변수 사용
		*/
		m_blockHideEvent = true;
		Qt::WindowFlags type = windowFlags() | Qt::WindowStaysOnTopHint;
		setWindowFlags(type | Qt::WindowStaysOnTopHint);
		m_blockHideEvent = false;
	}


}

