#pragma once

#include <qstringlist>
#include <memory>
#include <qicon>
#include "filemanager/export.h"
#include "filemanager/FileManagerDialog.h"
#include "filemanager/define.h"

namespace fm
{
	class AppCoreContext;

	class FM_CORE_EXPORT FileManagerAppCore
	{
	public:
		FileManagerAppCore();
		~FileManagerAppCore();
	public:
		void Initialize(QString appName, ProductFunctionType productType, QIcon icon, ELanguageType languageType);
		void Deinitialize();
		bool IsInitialized() const;
		QString GetAppName() const;
		ProductFunctionType GetProductFunctionType() const;
		QIcon GetIcon() const;

		bool GetDialog(FileManagerDialog*& pOutDialog);
		AppCoreContext* GetContext();

	private:
		AppCoreContext* m_pContext;
		FileManagerDialog* m_pFileManagerDialog;

		QString m_appName;
		ProductFunctionType m_productType;
		QIcon m_icon;
	};
}

