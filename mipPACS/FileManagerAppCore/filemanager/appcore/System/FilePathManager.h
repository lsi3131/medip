#pragma once

#include <string>
#include "filemanager/export.h"

namespace fm
{
	class FM_CORE_EXPORT FilePathManager
	{
	public:
		const static std::wstring DEFAULT_MEDIP_DIR_NAME;
		const static std::wstring FILE_MANANER_DIR_NAME;
		const static std::wstring CONFIG_DIR_NAME;
		const static std::wstring LOG_FILE_NAME;
	public:
		static FilePathManager* Instance()
		{
			static FilePathManager instance;
			return &instance;
		}

	public:
		FilePathManager();
		~FilePathManager();

	public:
		void Initialize(QString appName = "");
		void InitializeWithoutCreateDirectory(QString appName = "");

		void ClearTempDir();

		/* appName으로 생성되는 최상위 Directory 경로 */
		std::wstring AppRootDirPath();
		std::wstring FileManagerRootDirPath();
		std::wstring DataDirPath();
		std::wstring LogDirPath();
		std::wstring ConfigDirPath();
		std::wstring TempDirPath();
		std::wstring DefaultRepositoryDirPath();

		std::wstring LogFilePath();

	private:
		QString m_appName;

		std::wstring m_appRootDirPath;
		std::wstring m_fileManagerRootDirPath;
		std::wstring m_dataDirPath;
		std::wstring m_logDirPath;
		std::wstring m_configDirPath;
		std::wstring m_tempDirPath;
		std::wstring m_defaultRepositoryPath;
	};
};
