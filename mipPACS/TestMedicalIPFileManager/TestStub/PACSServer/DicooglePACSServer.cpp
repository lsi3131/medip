#include "stdafx.h"
#include "DicooglePACSServer.h"
#include <qthread>
#include <qdir>
#include "filemanager/Net/ProcessManager.h"

DicooglePACSServer::DicooglePACSServer()
{
	DicoogleDirPath = "Dicoogle";
	DicoogleFileName = "dicoogle.jar";
	DicoogleFilepath = DicoogleDirPath + "/" + DicoogleFileName;
	IndexDirPath = DicoogleDirPath + "/index";
	StorageDirPath = DicoogleDirPath + "/storage";

	/* TODO : server.xml에서 parsing하여 load할 것 */

	AETitle = "DICOOGLE-STORAGE";
	IP = "localhost";
	QueryRetrievePort = 1045;
	StorePort = 6666;

	ProcessEventHandler = new QProcessEventHandlerTestStub();
	QObject::connect(&m_process, &QProcess::started, ProcessEventHandler, &QProcessEventHandlerTestStub::onStarted);
	QObject::connect(&m_process, SIGNAL(finished(int)), ProcessEventHandler, SLOT(onFinished(int)));
}

DicooglePACSServer::~DicooglePACSServer()
{
	delete ProcessEventHandler;
}

bool DicooglePACSServer::RunAndWaitProcess(int wait_msec)
{
	if (fm::ProcessManager::IsProcessExist(L"JaVa.exe"))
	{
		return false;
	}

	QString command = QString("java -jar %1 -s").arg(DicoogleFileName);
	m_process.setWorkingDirectory(DicoogleDirPath);
	m_process.start(command);

	QThread::msleep(wait_msec);
	return true;
}

void DicooglePACSServer::KillAndWaitProcess()
{
	m_process.kill();
	m_process.waitForFinished(-1);
}

void DicooglePACSServer::DeleteStorageDirectory()
{
	QDir dir;
	dir.setPath(IndexDirPath);
	dir.removeRecursively();

	dir.setPath(StorageDirPath);
	dir.removeRecursively();
}
