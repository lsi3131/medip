#pragma once

#ifndef LOG_MANAGER_H
#define LOG_MANAGER_H

#define LOG_MNG LogManager::GetLogInstance()

class LogManager : public QObject
{
public:
	static LogManager* GetLogInstance()
	{
		static LogManager instance;
		return &instance;
	}

private:
	LogManager();
	virtual ~LogManager();

	// 상수 정의.
public:
	static const qint64 LOG_FILES_MAX_LIMIT_SIZE;

public:
	const QString makeDateFilePath(const QString &strAppName);
	void installMessageHandler();
	void checkLogFiles();

};
#endif