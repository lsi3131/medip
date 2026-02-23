#pragma once

#include <QtWidgets/QMainWindow>
#include <QRadioButton>
#include <QStackedWidget>
#include <qfilesystemmodel>
#include "filemanager/export.h"

class QTcpSocket;
class QTcpServer;

namespace fm
{
	class StatusMonitorDialog;
	class TestDicomNetworkDialog;
	class AppCoreContext;
	class MainWidget;

	class FM_CORE_EXPORT MainWindow : public QMainWindow
	{
		Q_OBJECT
	public:
		MainWindow(AppCoreContext* pContext, QWidget *parent = Q_NULLPTR);

	protected:
		virtual void closeEvent(QCloseEvent* e) override;
		virtual bool event(QEvent* e) override;
		virtual void showEvent(QShowEvent *e) override;
		virtual void hideEvent(QHideEvent *e) override;

		private slots:
		void onUpdateConnectToServer();
		void onUpdateDisconnectedFromServer();

		void onDebugServerClientCommunication();
		void onDebugStatusMonitor();
		void onDebugDICOMNetwork();

		void onClose();

	private:
		void SetWindowOnTop();
	private:
		AppCoreContext* m_pAppCoreContext;
		MainWidget* m_pMainWidget;

		StatusMonitorDialog* m_pStatusMonitorDialog;
		TestDicomNetworkDialog* m_pDicomNetworkDialog;

		QAction *m_actionDebug_ServerClientCommunication;
		QAction *m_actionDebug_StatusMonitor;
		QAction *m_actionDebug_DICOMNetwork;

		bool m_forceToClose;
		bool m_blockHideEvent;
	};
}

