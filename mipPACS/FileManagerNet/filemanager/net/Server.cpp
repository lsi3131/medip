#include "stdafx.h"
#include "Server.h"
#include "filemanager/net/ProcessManager.h"
#include "filemanager/net/ServerPacketParser.h"
#include "Util.h"
#include <qlocalserver>
#include <qlocalsocket>
#include <QtWidgets/QApplication>
#include <qdir>
#include <Windows.h>

using namespace fm;
using namespace std;

const QString Server::CLIENT_PROCESS_NAME = "FileManager.exe";

Server::Server() :
	m_pServer(new QLocalServer()),
	m_pProcessManager(new fm::ProcessManager()),
	m_status(EServerConnectStatus::client_process_not_running)
{
	connect(m_pServer, &QLocalServer::newConnection, this, &Server::onNewClientSocketConnected);
	m_threadDelayUpdate = std::make_unique<ThreadDelayedUpdate>(this);
}

Server::~Server()
{
	if (m_pServer != nullptr)
	{
		delete m_pServer;
		m_pServer = nullptr;
	}

	if (m_pProcessManager != nullptr)
	{
		delete m_pProcessManager;
		m_pProcessManager = nullptr;
	}
}

bool Server::StartServer()
{
	qInfo() << "[Server] start file manager local server(name=" << MedipcalIPLocalServerName << ")";
	if (m_pServer->listen(MedipcalIPLocalServerName) == false)
	{
		qWarning() << "fail to listen local server(=" << MedipcalIPLocalServerName << ")";
		return false;
	}

	emit serverStarted();

	return true;
}

bool Server::StopServer()
{
	qInfo() << "[Server] try to close file manager local server(name=" << MedipcalIPLocalServerName << ")";
	m_pServer->close();
	m_pServer->destroyed();

	/*
		socket->close 진행 시 m_connectedClientSockets의 리스트가 삭제됨.
		이에 복사본으로 루프를 진행한다.
	*/
	std::vector<QLocalSocket*> sockets = m_connectedClientSockets;
	for (auto& socket : sockets)
	{
		socket->close();
	}
	m_connectedClientSockets.clear();

	emit serverStop();

	return true;
}

bool Server::IsServerRunning()
{
	return m_pServer->isListening();
}

bool Server::StartClient()
{
	bool result = false;
	if (m_status == EServerConnectStatus::client_process_not_running)
	{
		UpdateConnectStatus(EServerConnectStatus::client_process_in_running);

		/* 연결된 client가 존재하면 update를 전송 */
		if (m_connectedClientSockets.empty())
		{
			/* Process 존재 시 항상 종료되도록 설정 */
			if (m_pProcessManager->IsProcessExist(CLIENT_PROCESS_NAME.toStdWString()))
			{
				qInfo() << "[Server] process(=" << CLIENT_PROCESS_NAME << ") is already exist. "
					<< "kill the process will be started";

				std::vector<ProcessInfo> killedProcesses;
				m_pProcessManager->KillByName(CLIENT_PROCESS_NAME.toStdWString(), &killedProcesses);

				qInfo() << "[Server] process(=" << CLIENT_PROCESS_NAME << ") is killed. count=" << killedProcesses.size();
			}

			wstring filepath = GetClientProcessFilePath().toStdWString();
			result = m_pProcessManager->StartProcess(filepath);
		}
	}
	else
	{
		qInfo() << "[Server] client is already running.";
	}
	return result;
}

bool Server::OpenClient(QStringList fileOpenFilterList)
{
	bool result = false;
	if (m_status == EServerConnectStatus::client_process_not_running)
	{
		qInfo() << "[Server] client is not running. StartClient() should call first.";
	}
	else if (m_status == EServerConnectStatus::client_process_in_running)
	{
		/* Process는 실행 중이지만 Client와 Connect가 되지 않은 경우 지연 초기화 진행 */
		if (m_threadDelayUpdate->isRunning() == false)
		{
			m_threadDelayUpdate->SetFileOpenFilter(FileExtFilterUtil::ToSerialize(fileOpenFilterList).data());
			m_threadDelayUpdate->start();
		}
	}
	else
	{
		/* 연결된 client가 없다면 process 실행 */
		result = Send_Update(FileExtFilterUtil::ToSerialize(fileOpenFilterList).data());
	}
	return result;
}

int Server::GetConnectedClientCount()
{
	return m_connectedClientSockets.size();
}

QString fm::Server::GetClientProcessFilePath()
{
	/* Client Process 경로는 항상 Server Process 디렉토리와 동일한 경로에 있어야한다. */
	return QApplication::applicationDirPath() + "/" + CLIENT_PROCESS_NAME;
}

bool fm::Server::IsClientProcessFileExist()
{
	return QFile::exists(GetClientProcessFilePath());
}

EServerConnectStatus fm::Server::ConnectStatus()
{
	return m_status;
}

std::vector<QLocalSocket*> fm::Server::GetConnectedClientSockets()
{
	return m_connectedClientSockets;
}

bool Server::Send_Update(const char * fileOpenFilter)
{
	SERVER_TO_CLIENT_INFO info;
	info.Command = EServerCommand::update;
	strcpy(info.FileExtensionFilter, QString(fileOpenFilter).toUtf8());

	return Send(&info);
}

bool Server::Send(SERVER_TO_CLIENT_INFO* pInfo)
{
	qInfo() << "[Server] send to client START with flush. command:" << GetText_EServerCommand(pInfo->Command);
	for (QLocalSocket* pSocket : m_connectedClientSockets)
	{
		if (pSocket == nullptr)
		{
			qWarning() << "socket is nullptr";
			return false;
		}
		if (pSocket->isOpen() == false)
		{
			qWarning() << "socket is not opened";
			return false;
		}

		quint64 len = pSocket->write((char*)pInfo, sizeof(SERVER_TO_CLIENT_INFO));
		pSocket->flush();

		qInfo() << "[Server] send pacekt. with flush command:" << GetText_EServerCommand(pInfo->Command);
	}
	qInfo() << "[Server] send to client END with flush. command:" << GetText_EServerCommand(pInfo->Command);


	return true;
}

void fm::Server::UpdateConnectStatus(EServerConnectStatus status)
{
	if (m_status != status)
	{
		QString text = QString("server status change. <%1> -> <%2>").
			arg(GetText_EServerConnectStep(m_status)).
			arg(GetText_EServerConnectStep(status));

		m_status = status;
	}
}

bool fm::Server::RemoveClientSocketFromList(QLocalSocket * disconnectedClient)
{
	auto it = std::find(m_connectedClientSockets.begin(), m_connectedClientSockets.end(), disconnectedClient);
	if (it == m_connectedClientSockets.end())
	{
		qWarning() << "disconnected socket is not exist in socket list.";
		return false;
	}
	m_connectedClientSockets.erase(it);

	qInfo() << "[Server] socket is disconnected. current client count : " << m_connectedClientSockets.size();

	if (m_connectedClientSockets.empty())
	{
		UpdateConnectStatus(EServerConnectStatus::client_process_not_running);
	}

	emit clientDisconnected();
	return true;
}

void Server::onNewClientSocketConnected()
{
	QLocalSocket* pConnectedSocket = m_pServer->nextPendingConnection();

	connect(pConnectedSocket, &QLocalSocket::readyRead, this, &Server::onPacketReadFromClient);
	connect(pConnectedSocket, &QLocalSocket::disconnected, this, &Server::onClientSocketDisConnected);

	m_connectedClientSockets.push_back(pConnectedSocket);

	qInfo() << "[Server] new socket is connected. current client count : " << m_connectedClientSockets.size();
	UpdateConnectStatus(EServerConnectStatus::client_process_connect_finished);

	emit clientConnected();
}

void Server::onPacketReadFromClient()
{
	qInfo() << "[Server] onPacketReadFromClient - 1. event occured";
	QLocalSocket* socket = (QLocalSocket*)sender();

	QByteArray readPacket = socket->readAll();
	qInfo() << "[Server] onPacketReadFromClient - 2. read packet size : " << readPacket.size();

	QVector<fm::CLIENT_TO_SERVER_INFO> infos;
	if (m_parser.Parse(readPacket, infos))
	{
		for (int i = 0; i < infos.size(); ++i)
		{
			qInfo() << "[Server] parse finished. update signal will be emitted. index : " << i;
			emit updateFromClient(infos[i]);
		}
	}
}

void Server::onClientSocketDisConnected()
{
	QLocalSocket* disconnectedClient = (QLocalSocket*)sender();

	RemoveClientSocketFromList(disconnectedClient);
}

//==========================================
//				ThreadDelayedInitialize
//==========================================
ThreadDelayedUpdate::ThreadDelayedUpdate(Server * pServer) :
	m_pServer(pServer)
{
}

void fm::ThreadDelayedUpdate::SetFileOpenFilter(const char * fileOpenFilter)
{
	m_fileOpenFilter = fileOpenFilter;
}

void ThreadDelayedUpdate::run()
{
	qInfo() << "[Server] start to thread delayed update";

	while (true)
	{
		EServerConnectStatus status = m_pServer->ConnectStatus();
		if (status == EServerConnectStatus::client_process_connect_finished)
		{
			/* connect finished 된 경우가 완료된 경우 Update 진행 */
			m_pServer->Send_Update(m_fileOpenFilter.data());
			break;
		}

		QThread::msleep(300);
	}

	qInfo() << "[Server] finish to thread delayed update";
}



