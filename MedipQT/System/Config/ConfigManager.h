/* 
	21.04.22(목) 이상일 대리
	목적 : MEDIP Config 관련 데이터 관리.
	TODO : 추후 WindowManager의 Config 관련 정보를 해당 클래스로 이관 작업 고려
*/

#pragma once

#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include "stringManager.h"
#include "VisualPrintConfig.h"

#define CONFIG_MANAGER ConfigManager::GetInstance()

class ConfigManager
{
public:
	static ConfigManager* GetInstance()
	{
		static ConfigManager instance;
		return &instance;
	}

public:
	ConfigManager();

public:
	bool init();

	void setConfig_VisualPrint(QString url, int port);
	VisualPrintConfig* getConfig_VisualPrint();

private:
	void setConfigValue(QString element, QString node, QString value);
	bool getConfigValue(QString element, QString node, QString& value);

private:
	VisualPrintConfig m_visualPrintConfig;
};
#endif

