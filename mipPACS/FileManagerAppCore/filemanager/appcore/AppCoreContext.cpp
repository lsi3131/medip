#include "stdafx.h"
#include "AppCoreContext.h"
#include "filemanager/appcore/Action/ActionManagerIPCVersion.h"
#include <qmessagebox>
#include <qfile>

namespace fm
{
	AppCoreContext::AppCoreContext() :
		m_isInitialized(false)
	{
		m_FilePathManager = FilePathManager::Instance();

		m_DicomNetworkManager = new DicomNetworkManager();

		m_DataManager = new EntityDataManager();

		m_pConfigManager = new ConfigManager();

		m_RuntimeManager = new RuntimeManager();

		/* Network를 사용할 경우*/
		//m_AppManager = new ActionManagerIPCVersion(
		//	m_DataManager,
		//	m_DicomFileManager,
		//	m_Client
		//);

		m_ActionManager = new ActionManager();
	}

	AppCoreContext::~AppCoreContext()
	{
		if (m_DicomNetworkManager != nullptr)
		{
			delete m_DicomNetworkManager;
			m_DicomNetworkManager = nullptr;
		}

		if (m_DataManager != nullptr)
		{
			delete m_DataManager;
			m_DataManager = nullptr;
		}

		if (m_RuntimeManager != nullptr)
		{
			delete m_RuntimeManager;
			m_RuntimeManager = nullptr;
		}

		if (m_ActionManager != nullptr)
		{
			delete m_ActionManager;
			m_ActionManager = nullptr;
		}
	}

	void AppCoreContext::Initialize(ProductFunctionType type, QString appName)
	{
		Deinitialize();
		m_productFunctionType = type;

		qInfo() << "Welcome MEDIP FileManager";

		m_FilePathManager->Initialize(appName);

		/* TODO : 설정 파일에 따라 XML, SQL 데이터 분기할 수 있도록 할 것*/
		m_DataManager->Initialize_XML(m_FilePathManager->DataDirPath());

		if (m_pConfigManager->Initialize(m_FilePathManager->ConfigDirPath()) == false)
		{
			qCritical() << "fail to initialize Config Manager";
		}

		/* Default Repository 경로를 설정한다. */
		PACSConfig* pPACSConfig = m_pConfigManager->GetPACSConfig();
		std::wstring repositoryDirectoryPath = pPACSConfig->GetRepositoryDirectoryPath();
		if (repositoryDirectoryPath.empty())
		{
			pPACSConfig->ModifyRepositoryDirectoryPath(m_FilePathManager->DefaultRepositoryDirPath());
		}

		/* Config 정보로 DicomNetwork 정보를 초기화*/
		if (m_DicomNetworkManager->SetAllByConfig(m_pConfigManager->GetPACSConfig()) == false)
		{
			qCritical() << "fail to initialize Network Config From PACS Config";
		}

		if (m_ActionManager->Initialize(this) == false)
		{
			qCritical() << "fail to initialize action manager";
		}

		if (m_DcmExportManager.Initialize(m_DicomNetworkManager, m_FilePathManager) == false)
		{
			qCritical() << "fail to initialize dcm export manager";
		}

		m_isInitialized = true;
	}

	void AppCoreContext::Deinitialize()
	{
		m_isInitialized = false;

		m_DicomNetworkManager->Deinitialize();
	}

	bool AppCoreContext::IsInitialized()
	{
		return m_isInitialized;
	}

	fm::ActionManager * AppCoreContext::GetActionManager()
	{
		return m_ActionManager;
	}

	fm::FilePathManager * AppCoreContext::GetFilePathManager()
	{
		return m_FilePathManager;
	}

	fm::DicomNetworkManager * AppCoreContext::GetDicomNetworkManager()
	{
		return m_DicomNetworkManager;
	}

	fm::EntityDataManager* AppCoreContext::GetDataManager()
	{
		return m_DataManager;
	}

	fm::ConfigManager * AppCoreContext::GetConfigManager()
	{
		return m_pConfigManager;
	}

	RuntimeManager* AppCoreContext::GetRuntimeManager()
	{
		return m_RuntimeManager;
	}

	ProductFunctionType AppCoreContext::GetProductFunctionType()
	{
		return m_productFunctionType;
	}

	DicomExportManager* AppCoreContext::GetDicomExportManager()
	{
		return &m_DcmExportManager;
	}
}

