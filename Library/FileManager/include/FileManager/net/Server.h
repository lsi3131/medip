#pragma once

#include <QString>
#include <qobject>
#include <vector>
#include <qthread>
#include <memory>

#include "filemanager/net/defines.h"
#include "filemanager/net/export.h"
#include "filemanager/net/ServerPacketParser.h"

class QLocalServer;
class QLocalSocket;
class QProcess;

namespace fm
{
	class ProcessManager;
	class ThreadDelayedUpdate;

	class FM_NET_EXPORT Server : public QObject
	{
		Q_OBJECT
	public:
		const static QString CLIENT_PROCESS_NAME;

	public:
		Server();
		virtual ~Server();

	public:
		bool StartServer();
		bool StopServer();
		bool IsServerRunning();
		bool StartClient();
		bool OpenClient(QStringList fileOpenFilters);
		int GetConnectedClientCount();
		QString GetClientProcessFilePath();
		bool IsClientProcessFileExist();
		EServerConnectStatus ConnectStatus();
		std::vector<QLocalSocket*> GetConnectedClientSockets();


	public:
		bool Send_Update(const char* fileOpenFilter);
	private:
		bool Send(SERVER_TO_CLIENT_INFO* pInfo);
		void UpdateConnectStatus(EServerConnectStatus status);
		bool RemoveClientSocketFromList(QLocalSocket* disconnectedClient);

	Q_SIGNALS:
		void updateFromClient(CLIENT_TO_SERVER_INFO info);
		void clientConnected();
		void clientDisconnected();
		void serverStarted();
		void serverStop();

	private slots:
		void onPacketReadFromClient();
		void onNewClientSocketConnected();
		void onClientSocketDisConnected();

	private:
		QLocalServer* m_pServer;
		std::vector<QLocalSocket*> m_connectedClientSockets;
		QString m_fileMngrFilepath;
		fm::ProcessManager* m_pProcessManager;
		EServerConnectStatus m_status;
		std::unique_ptr<ThreadDelayedUpdate> m_threadDelayUpdate;
		QByteArray m_currentReadPacket;
		ServerPacketParser m_parser;
	};

	class ThreadDelayedUpdate : public QThread
	{
	public:
		ThreadDelayedUpdate(Server* pServer);

	public:
		void SetFileOpenFilter(const char* fileOpenFilter);

	protected:
		virtual void run() override;

	private:
		Server* m_pServer;
		std::string m_fileOpenFilter;
	};

};
