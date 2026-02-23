#include "stdafx.h"
#include "FilePathManager.h"
#include <qstandardpaths>
#include <qdir>

using namespace fm;

const std::wstring FilePathManager::DEFAULT_MEDIP_DIR_NAME = L"MEDIP";
const std::wstring FilePathManager::FILE_MANANER_DIR_NAME = L"FileManager";
const std::wstring FilePathManager::CONFIG_DIR_NAME = L"Config";
const std::wstring FilePathManager::LOG_FILE_NAME = L"log_medip_filemanager.log";

FilePathManager::FilePathManager()
{
	/* Default app name*/
	Initialize("");
}

FilePathManager::~FilePathManager()
{
}

void FilePathManager::Initialize(QString appName)
{
	InitializeWithoutCreateDirectory(appName);

	QDir dir;
	dir.mkpath(QString::fromStdWString(m_appRootDirPath));
	dir.mkpath(QString::fromStdWString(m_fileManagerRootDirPath));
	dir.mkpath(QString::fromStdWString(m_tempDirPath));
	dir.mkpath(QString::fromStdWString(m_dataDirPath));
	dir.mkpath(QString::fromStdWString(m_logDirPath));
	dir.mkpath(QString::fromStdWString(m_configDirPath));
	dir.mkpath(QString::fromStdWString(m_defaultRepositoryPath));

	/* 임시 Directory 초기화 */
	ClearTempDir();
}

void fm::FilePathManager::InitializeWithoutCreateDirectory(QString appName)
{
	/* %APPDATA%/LOCAL/%현재exe파일이름% 으로 반환함 */
	QString appRootPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
	int index = appRootPath.lastIndexOf("/");
	if (index != -1)
	{
		appRootPath = appRootPath.left(index);
	}

	if (appName.isEmpty())
	{
		appName = QString::fromStdWString(DEFAULT_MEDIP_DIR_NAME);
	}
	appRootPath += "/" + appName;
	m_appRootDirPath = appRootPath.toStdWString();

	m_fileManagerRootDirPath = m_appRootDirPath + L"/FileManager";
	m_dataDirPath = m_fileManagerRootDirPath + L"/Data";
	m_logDirPath = m_fileManagerRootDirPath + L"/Log";
	m_configDirPath = m_fileManagerRootDirPath + L"/Config";
	m_tempDirPath = m_fileManagerRootDirPath + L"/Temp";
	m_defaultRepositoryPath = m_fileManagerRootDirPath + L"/Repository";
}

void fm::FilePathManager::ClearTempDir()
{
	QDir dir(QString::fromStdWString(m_tempDirPath));
	dir.removeRecursively();
	dir.mkpath(QString::fromStdWString(m_tempDirPath));
}

std::wstring fm::FilePathManager::AppRootDirPath()
{
	return m_appRootDirPath;
}

std::wstring FilePathManager::FileManagerRootDirPath()
{
	return m_fileManagerRootDirPath;
}

std::wstring FilePathManager::DataDirPath()
{
	return m_dataDirPath;
}

std::wstring FilePathManager::LogDirPath()
{
	return m_logDirPath;
}

std::wstring fm::FilePathManager::ConfigDirPath()
{
	return m_configDirPath;
}

std::wstring fm::FilePathManager::TempDirPath()
{
	return m_tempDirPath;
}

std::wstring fm::FilePathManager::DefaultRepositoryDirPath()
{
	return m_defaultRepositoryPath;
}

std::wstring FilePathManager::LogFilePath()
{
	return m_logDirPath + L"/" + LOG_FILE_NAME;
}

