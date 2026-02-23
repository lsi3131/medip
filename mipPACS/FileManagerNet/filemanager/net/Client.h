#pragma once

#include <QString>
#include <QObject>
#include "filemanager/net/defines.h"
#include "filemanager/net/export.h"
#include "filemanager/net/ClientPacketParser.h"

class QLocalSocket;
class QProcess;

namespace fm
{
	class FM_NET_EXPORT Client : public QObject
	{
		Q_OBJECT

	public:
		Client();
		virtual ~Client();

	public:
		bool LaunchProcess();
		bool ConnectToServer();
		bool CloseToServer();
		bool IsConnectedToServer();

		void SetSendPacketSizeAtOneTime(int size);
		int GetSendPacketSizeAtOneTime();
		int GetMaxSendPacketSize();

	public:
		bool Send_Update();
		bool Send_FileOpen(const char* filepath);
		bool Send_Activate(bool isActivate);
	private:
		bool Send(CLIENT_TO_SERVER_INFO* pInfo);

	private:
		void SocketConnectCheck_Loop();


	Q_SIGNALS:
		void sendPacketToServer(CLIENT_TO_SERVER_INFO clientToServerInfo);
		void updateFromServer(SERVER_TO_CLIENT_INFO serverToClientInfo);
		void updateConnected();
		void updateDisconnected();

	private slots:
		void onPacketReadFromServer();
		void onConnectedToServer();
		void onDisConnectedToServer();

		void onFileManagerProcessFinished(int exitCode);

	private:
		QProcess* m_pProcess;
		QLocalSocket* m_pSocket;
		QString m_fileMngrFilepath;
		int m_sendPacketSizeAtOneTime;
		ClientPacketParser m_parser;
	};
};
