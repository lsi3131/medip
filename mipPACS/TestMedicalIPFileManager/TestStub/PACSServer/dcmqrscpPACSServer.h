#pragma once

#include <qprocess>
#include "QProcessEventHandlerTestStub.h"

class dcmqrscpPACSServer
{
public:
	dcmqrscpPACSServer();
	~dcmqrscpPACSServer();
public:
	bool RunAndWaitProcess(int wait_msec);
	void KillAndWaitProcess();
	void DeleteStorageDirectory();

private:
	QProcess m_process;
public:
	QProcessEventHandlerTestStub* ProcessEventHandler;
	QString RootDirPath;
	QString ExeFileName;
	QString ExeFilePath;
	QString StorageDirPath;

	QString AETitle;
	QString IP;
	int QueryRetrievePort;
	int StorePort;

	QString StoreSCP_AETitle;
	int StoreSCP_Port;
};
