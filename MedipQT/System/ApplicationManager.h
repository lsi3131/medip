#pragma once

#ifndef APPLICATION_MANAGER_H
#define APPLICATION_MANAGER_H

#define APP_MNG ApplicationManager::GetInstance()

class ApplicationManager : public QObject
{
	Q_OBJECT
signals:
	void fileLoadingFinishedSignal();

public slots:
	void fileLoadingFinishedSlot();
	void AutoSave();

public:
	static ApplicationManager* GetInstance()
	{
		static ApplicationManager instance;
		return &instance;
	}

private:
	ApplicationManager();
	virtual ~ApplicationManager();

public:
	void AppInit();
	void AppExit();
	void AppLogOut();

	void AppDataDirectoryInit();
	void copyFromProgramFileDirToAppDataDir(QString AppDataLocalProductPath, QString strInstallDir, QString processedDirName);

	void SaveOpendFile();

	bool noticeDlgPopup();

public:
	QString &getAppName()			{ return m_strAppName; }
	void setAppName(QString val)	{ m_strAppName = val; }

	bool getAutoSaveFlag() const	{ return m_bAutoSaveFlag; }
	void updateAutoSaveFlag(bool val);

	int getAutoSaveInterval() const { return m_nAutoSaveInterval; }
	void updateAutoSaveInteval(int val);


private:
	QString m_strAppName;
	QTimer *m_pDocAutoSaveTimer;
	bool m_bAutoSaveFlag;
	int m_nAutoSaveInterval;
	bool m_bFileOpened;

	// 상수 정의.
public:
	static const int AUTO_SAVE_DEFAULT_INTERVAL;
	static const int AUTO_SAVE_INTERVAL_RATIO;

};
#endif