#include "stdafx.h"
#include "ConfigManager.h"
#include "filemanager/connection/XmlConnection.h"

using namespace fm;

const std::wstring ConfigManager::PACS_CONFIG_FILE_NAME = L"pacs_config.xml";
const std::wstring ConfigManager::APP_CONFIG_FILE_NAME = L"app_config.xml";
const std::wstring ConfigManager::FOLDER_VIEW_CONFIG_FILE_NAME = L"folder_view_config.xml";
const std::wstring ConfigManager::GUI_SETTING_CONFIG_FILE_NAME = L"gui_setting_config";

ConfigManager::ConfigManager() :
	m_PACSConfig(nullptr),
	m_AppConfig(nullptr),
	m_FolderViewConfig(nullptr),
	m_GUISettingConfig(nullptr)
{
}

ConfigManager::~ConfigManager()
{
}

bool ConfigManager::Initialize(std::wstring configDataDir)
{
	std::wstring pacsConfigFilePath = configDataDir + L"/" + PACS_CONFIG_FILE_NAME;
	std::wstring appConfigFilePath = configDataDir + L"/" + APP_CONFIG_FILE_NAME;
	std::wstring folderViewConfigFilePath = configDataDir + L"/" + FOLDER_VIEW_CONFIG_FILE_NAME;
	std::wstring guiSettingConfigFilePath = configDataDir + L"/" + GUI_SETTING_CONFIG_FILE_NAME;

	m_PACSConfig = std::make_unique<PACSConfig>();
	if (m_PACSConfig->LoadXml(pacsConfigFilePath) == false)
	{
		return false;
	}

	m_AppConfig = std::make_unique<AppConfig>();
	if (m_AppConfig->LoadXml(appConfigFilePath) == false)
	{
		return false;
	}

	m_FolderViewConfig = std::make_unique<FolderViewConfig>();
	if (m_FolderViewConfig->LoadXml(folderViewConfigFilePath) == false)
	{
		return false;
	}

	m_GUISettingConfig = std::make_unique<GUISettingConfig>();
	m_GUISettingConfig->Load(guiSettingConfigFilePath);
	m_GUISettingConfig->Save();

	return true;
}

PACSConfig* ConfigManager::GetPACSConfig()
{
	return m_PACSConfig.get();
}

AppConfig * fm::ConfigManager::GetAppConfig()
{
	return m_AppConfig.get();
}

FolderViewConfig* fm::ConfigManager::GetFolderViewConfig()
{
	return m_FolderViewConfig.get();
}

GUISettingConfig* fm::ConfigManager::GetGUISettingConfig()
{
	return m_GUISettingConfig.get();
}

