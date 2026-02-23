#pragma once

#include <qprocess>
#include "QProcessEventHandlerTestStub.h"

class DicooglePACSServer
{
public:
	DicooglePACSServer();
	~DicooglePACSServer();
public:
	bool RunAndWaitProcess(int wait_msec);
	void KillAndWaitProcess();
	void DeleteStorageDirectory();

private:
	QProcess m_process;
public:
	QProcessEventHandlerTestStub* ProcessEventHandler;
	QString DicoogleDirPath;
	QString DicoogleFilepath;
	QString DicoogleFileName;
	QString IndexDirPath;
	QString StorageDirPath;

	/* TODO : server.xml에서 parsing 할 것 */
	QString AETitle;
	QString IP;
	int QueryRetrievePort;
	int StorePort;
};
