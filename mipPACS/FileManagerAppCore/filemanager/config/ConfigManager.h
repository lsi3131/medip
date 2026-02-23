/*
	작성자 : 이상일
	목적 :
	파일관리자의 설정 파일 관리를 위한 클래스
*/

#pragma once

#include "filemanager/export.h"
#include "filemanager/config/PACSConfig.h"
#include "filemanager/config/AppConfig.h"
#include "filemanager/config/FolderViewConfig.h"
#include "filemanager/config/GUISettingConfig.h"
#include <memory>

namespace fm
{
	class XmlConnection;

	class FM_CORE_EXPORT ConfigManager
	{
	public:
		const static std::wstring PACS_CONFIG_FILE_NAME;
		const static std::wstring APP_CONFIG_FILE_NAME;
		const static std::wstring FOLDER_VIEW_CONFIG_FILE_NAME;
		const static std::wstring GUI_SETTING_CONFIG_FILE_NAME;

	public:
		ConfigManager();
		~ConfigManager();

		bool Initialize(
			std::wstring configDataDir
		);

	public:
		PACSConfig* GetPACSConfig();
		AppConfig* GetAppConfig();
		FolderViewConfig* GetFolderViewConfig();
		GUISettingConfig* GetGUISettingConfig();

	private:
		std::unique_ptr<PACSConfig> m_PACSConfig;
		std::unique_ptr<AppConfig> m_AppConfig;
		std::unique_ptr<FolderViewConfig> m_FolderViewConfig;
		std::unique_ptr<GUISettingConfig> m_GUISettingConfig;
	};
}
