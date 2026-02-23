#include "stdafx.h"
#include "Client.h"
#include <Windows.h>
#include <QLocalSocket>
#include <QProcess>
#include <qdir>
#include <qcoreapplication>
#include <qthread>
#include <future>

using namespace fm;

Client::Client() :
	m_pSocket(new QLocalSocket),
	m_pProcess(nullptr)
{
	m_fileMngrFilepath = QCoreApplication::applicationDirPath() + "/MedicalIPFileManager.exe";

	connect(m_pSocket, &QLocalSocket::readyRead, this, &Client::onPacketReadFromServer);
	connect(m_pSocket, &QLocalSocket::connected, this, &Client::onConnectedToServer);
	connect(m_pSocket, &QLocalSocket::disconnected, this, &Client::onDisConnectedToServer);

	m_sendPacketSizeAtOneTime = GetMaxSendPacketSize();
}

Client::~Client()
{
	if (m_pSocket != nullptr)
	{
		delete m_pSocket;
	}

	if (m_pProcess != nullptr)
	{
		delete m_pProcess;
	}
}

bool Client::LaunchProcess()
{
	return true;
}

bool Client::ConnectToServer()
{
	//서버 연결 요청
	if (m_pSocket->isOpen())
	{
		qInfo() << "[Client] connect is already opened";
		return false;
	}

	qInfo() << "[Client] try to connect to file manager local server(name=" << MedipcalIPLocalServerName << ")";
	/*
		QLocalSocket에서 아래의 코드가 동작하지 않음.
		  - m_pSocket->waitForConnected();
		이에 connect요청을 비동기로 보낸 후 open상태를 체크한다.
	*/

	int tryCount = 0;
	do
	{
		QThread::msleep(500);
		m_pSocket->connectToServer(MedipcalIPLocalServerName);
		if (m_pSocket->isOpen())
		{
			qInfo() << "[Client] success to connect server";
			break;
		}

		qWarning() << "[Client] fail to connect local server(=" << MedipcalIPLocalServerName << "). try count = " << tryCount;
		tryCount++;
	} while (tryCount < 10);

	if (m_pSocket->isOpen() == false)
	{
		qWarning() << "[Client] fail to connect local server(=" << MedipcalIPLocalServerName << "). try count = " << tryCount;
		return false;
	}

	return true;
}

bool Client::CloseToServer()
{
	if (m_pSocket->isOpen() == false)
	{
		qWarning() << "client is not connected to server ";
		return false;
	}

	qInfo() << "[Client] client will be disconnected to server ";
	m_pSocket->close();
	return true;
}

bool Client::IsConnectedToServer()
{
	return m_pSocket->isOpen();
}

void fm::Client::SetSendPacketSizeAtOneTime(int size)
{
	int MaxSize = sizeof(CLIENT_TO_SERVER_INFO);
	m_sendPacketSizeAtOneTime = min(MaxSize, size);
	qInfo() << "[Client] Set send packet size : " << m_sendPacketSizeAtOneTime;
}

int fm::Client::GetSendPacketSizeAtOneTime()
{
	return m_sendPacketSizeAtOneTime;
}

int fm::Client::GetMaxSendPacketSize()
{
	return sizeof(CLIENT_TO_SERVER_INFO);
}

bool Client::Send_Update()
{
	CLIENT_TO_SERVER_INFO info;
	info.Command = EClientCommand::update;

	return Send(&info);
}

bool Client::Send_FileOpen(const char* filepath)
{
	CLIENT_TO_SERVER_INFO info;
	info.Command = EClientCommand::file_open;
	strcpy(info.FilePath, QString(filepath).toUtf8());

	return Send(&info);
}

bool Client::Send_Activate(bool isActivate)
{
	CLIENT_TO_SERVER_INFO info;
	info.Command = EClientCommand::activate;
	info.IsActivated = isActivate;

	return Send(&info);
}

bool Client::Send(CLIENT_TO_SERVER_INFO* pInfo)
{
	if (m_pSocket->isOpen() == false)
	{
		qWarning() << "[Client] socket is not opened";
		return false;
	}

	int remainSendPacketSize = GetMaxSendPacketSize();
	int startIndex = 0;
	do
	{
		int sendPacketSize = 0;
		if (remainSendPacketSize <= m_sendPacketSizeAtOneTime)
		{
			sendPacketSize = remainSendPacketSize;
			remainSendPacketSize = 0;
		}
		else
		{
			sendPacketSize = m_sendPacketSizeAtOneTime;
			remainSendPacketSize -= m_sendPacketSizeAtOneTime;
		}

		qInfo() << "[Client] send packet to server. remain packet size : " << remainSendPacketSize <<
			", send packet size: " << sendPacketSize;

		quint64 len = m_pSocket->write((char*)pInfo + startIndex, sendPacketSize);
		m_pSocket->flush();
		startIndex += sendPacketSize;

		/* For Debug Timining */
		//QThread::msleep(1000);
	} while (remainSendPacketSize > 0);

	qInfo() << "[Client] send command to server command(=" << GetText_EClientCommand(pInfo->Command) << ") ,"
		<< "file path :" << pInfo->FilePath
		<< ", activate :" << (int)pInfo->IsActivated;

	emit sendPacketToServer(*pInfo);

	return true;
}

void Client::SocketConnectCheck_Loop()
{
	while (m_pSocket->isOpen() == false)
	{
		QThread::msleep(300);
		//qInfo() << "wait until socket open...";
	}
}

void Client::onPacketReadFromServer()
{
	qInfo() << "[Client] onPacketReadFromServer - 1. event occurred";
	QByteArray readPacket = m_pSocket->readAll();
	qInfo() << "[Client] onPacketReadFromServer - 2. read packet size : " << readPacket.size();

	QVector<fm::SERVER_TO_CLIENT_INFO> infos;
	if (m_parser.Parse(readPacket, infos))
	{
		for (int i = 0; i < infos.size(); ++i)
		{
			qInfo() << "[Client] parse finished. update signal will be emitted. index : " << i;
			emit updateFromServer(infos[i]);
		}
	}
}

void Client::onConnectedToServer()
{
	qInfo() << "[Client] client socket is connected to server";
	emit updateConnected();
}

void Client::onDisConnectedToServer()
{
	qInfo() << "[Client] client socket is disconnected to server";
	m_pSocket->close();
	emit updateDisconnected();
}

void Client::onFileManagerProcessFinished(int exitCode)
{
	qInfo() << "[Client] file manager process finished";
}

