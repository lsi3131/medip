#include "stdafx.h"
#include "dcmqrscpPACSServer.h"
#include <qthread>
#include <qdir>
#include "filemanager/Net/ProcessManager.h"

dcmqrscpPACSServer::dcmqrscpPACSServer()
{
	RootDirPath = QDir::currentPath() + "/dcmqrscp";
	ExeFileName = "dcmqrscp.exe";
	ExeFilePath = RootDirPath + "/" + ExeFileName;
	StorageDirPath = RootDirPath + "/storage";

	/* TODO : dcmqrscp.cfg에서 parsing하여 load할 것 */
	AETitle = "DCMQRSCP";
	IP = "localhost";
	QueryRetrievePort = 12000;
	StorePort = 12000;

	ProcessEventHandler = new QProcessEventHandlerTestStub();
	QObject::connect(&m_process, &QProcess::started, ProcessEventHandler, &QProcessEventHandlerTestStub::onStarted);
	QObject::connect(&m_process, SIGNAL(finished(int)), ProcessEventHandler, SLOT(onFinished(int)));

	StoreSCP_AETitle = "StoreSCP";
	StoreSCP_Port = 9000;
}

dcmqrscpPACSServer::~dcmqrscpPACSServer()
{
	delete ProcessEventHandler;
}

bool dcmqrscpPACSServer::RunAndWaitProcess(int wait_msec)
{
	fm::ProcessManager::KillByName(ExeFileName.toStdWString());

	QDir dir;
	dir.mkdir(StorageDirPath);

	if (fm::ProcessManager::IsProcessExist(ExeFileName.toStdWString()))
	{
		return false;
	}

	QString command = QString("%1 -d").arg(ExeFileName);
	command = RootDirPath + "/" + command;
	m_process.setWorkingDirectory(RootDirPath);
	m_process.start(command);
	m_process.waitForStarted();

	if (fm::ProcessManager::IsProcessExist(ExeFileName.toStdWString()) == false)
	{
		return false;
	}

	QThread::msleep(wait_msec);
	return true;
}

void dcmqrscpPACSServer::KillAndWaitProcess()
{
	m_process.kill();
	m_process.waitForFinished(-1);

	/* 확실하게 Kill 하기 위해 호출*/
	fm::ProcessManager::KillByName(ExeFileName.toStdWString());
}

void dcmqrscpPACSServer::DeleteStorageDirectory()
{
	QDir dir;

	dir.setPath(StorageDirPath);
	dir.removeRecursively();
}
