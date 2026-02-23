#include "stdafx.h"
#include "filemanager/FileManagerAppCore.h"
#include "filemanager/appcore/AppCoreContext.h"
#include "filemanager/appcore/Dialog/Util/ProgressBarDialog.h"

namespace fm
{
	FileManagerAppCore::FileManagerAppCore() :
		m_pFileManagerDialog(nullptr)
	{
		m_pContext = new AppCoreContext();
	}

	FileManagerAppCore::~FileManagerAppCore()
	{
		if (m_pContext != m_pContext)
		{
			delete m_pContext;
			m_pContext = nullptr;
		}
	}

	void FileManagerAppCore::Initialize(QString appName, ProductFunctionType productType, QIcon icon, ELanguageType languageType)
	{
		StringManager::Init(languageType);

		m_pContext->Initialize(productType, appName);

		m_appName = appName;
		m_productType = productType;
		m_icon = icon;

		if (m_pFileManagerDialog)
		{
			delete m_pFileManagerDialog;
			m_pFileManagerDialog = nullptr;
		}
		m_pFileManagerDialog = new FileManagerDialog(m_pContext, m_icon);
	}

	void FileManagerAppCore::Deinitialize()
	{
		m_pContext->Deinitialize();

		if (m_pFileManagerDialog)
		{
			delete m_pFileManagerDialog;
			m_pFileManagerDialog = nullptr;
		}
	}

	bool FileManagerAppCore::IsInitialized() const
	{
		return m_pContext->IsInitialized();
	}

	QString FileManagerAppCore::GetAppName() const
	{
		return m_appName;
	}

	ProductFunctionType FileManagerAppCore::GetProductFunctionType() const
	{
		return m_productType;
	}

	QIcon FileManagerAppCore::GetIcon() const
	{
		return m_icon;
	}

	bool FileManagerAppCore::GetDialog(FileManagerDialog*& pOutDialog)
	{
		if (m_pContext->IsInitialized() == false)
		{
			qWarning() << "File Manager is not initialized";
			return false;
		}

		if (m_pFileManagerDialog == nullptr)
		{
			qWarning() << "FileManager Dialoig is empty";
			return false;
		}
		pOutDialog = m_pFileManagerDialog;

		return true;
	}
	AppCoreContext* FileManagerAppCore::GetContext()
	{
		return m_pContext;
	}
}

