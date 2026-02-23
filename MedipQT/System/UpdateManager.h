#pragma once

#ifndef UPDATE_MANAGER_H
#define UPDATE_MANAGER_H

#define UPDATE_DATA UpdateManager::GetUpdateInstance()

class UpdateManager : public QObject
{
public:
	static UpdateManager* GetUpdateInstance()
	{
		static UpdateManager instance;
		return &instance;
	}

private:
	UpdateManager(void);
	virtual ~UpdateManager(void);

	// 상수 정의.
public:
	static const QString MEDIP_NOTICE_SERVER_URL;
	static const QString LICENSE_SERVER_GETVERSION_PATH;

	static const QString UPDATER_NOTICE_SERVER_URL;

	static const QString MEDIP_UPDATER_FILENAME;
	static const int UPDATER_CALL_MAX_WAITTING_TIME;

public:
	// medip update.
	bool medipUpdate();
	bool medipUpdaterExecute();

	// updater update.
	void updaterUpdate();
	quint16 isUpdaterUpdateAvailable(QString &strVersion);

	void progressBegin();
	void progressUpdate(qint64 _received, qint64 _total);
	void progressEnd();

	bool existProcess(const QString &_processName);

	QString &getUpdaterFilePath()				{ return updaterFilePath; }
	void setUpdaterFilePath(QString val)		{ updaterFilePath = val; }

	QString updateReqResultMessage(qint16 resultCode);

private:
	//function
	void getUpdaterVersion(QString &strVersion);

	// variable
	QProgressDialog *pProgressDlg = NULL;

	QString updaterFilePath;
};
#endif