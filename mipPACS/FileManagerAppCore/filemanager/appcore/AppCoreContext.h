#pragma once

#include "filemanager/export.h"
#include "filemanager/define.h"
#include "filemanager/appcore/appcore_defines.h"
#include "filemanager/appcore/Action/ActionManager.h"
#include "filemanager/appcore/System/RuntimeManager.h"
#include "filemanager/appcore/System/DicomNetworkManager.h"
#include "filemanager/appcore/System/FilePathManager.h"
#include "filemanager/dicom/DicomInfomationModelLoader.h"
#include "filemanager/appcore/MainWindow.h"
#include "filemanager/dicom/Export/DicomExportManager.h"
#include "filemanager/data/Entity/EntityDataManager.h"
#include "filemanager/config/ConfigManager.h"
#include <memory>

class MainWindow;

namespace fm
{
	class FM_CORE_EXPORT AppCoreContext
	{
	public:
		AppCoreContext();
		~AppCoreContext();

		void Initialize(ProductFunctionType type, QString appName = "");
		void Deinitialize();

		bool IsInitialized();

	public:
		ActionManager* GetActionManager();
		FilePathManager* GetFilePathManager();
		DicomNetworkManager* GetDicomNetworkManager();
		EntityDataManager* GetDataManager();
		ConfigManager* GetConfigManager();
		RuntimeManager* GetRuntimeManager();
		ProductFunctionType GetProductFunctionType();
		DicomExportManager* GetDicomExportManager();
	private:
		ActionManager* m_ActionManager;
		FilePathManager* m_FilePathManager;
		DicomNetworkManager* m_DicomNetworkManager;
		EntityDataManager* m_DataManager;
		ConfigManager* m_pConfigManager;

		RuntimeManager* m_RuntimeManager;
		ProductFunctionType m_productFunctionType;
		DicomExportManager m_DcmExportManager;
		bool m_isInitialized;
	};
}

