#pragma once

#include "FileManager/config/PACSConfig.h"
#include "filemanager/export.h"
#include <qwidget>

namespace fm
{
	class FM_CORE_EXPORT PACSSetupInputInterface
	{
	public:
		virtual bool GetNewHost(PACSConfig::Host* pOutHost) = 0;
		virtual bool GetEdittedHost(PACSConfig::Host* pOutHost, PACSConfig::Host initHost) = 0;
		virtual bool CheckDeleteHost() = 0;

		virtual bool GetNewExportHost(PACSConfig::ExportHost* pOutExportHost) = 0;
		virtual bool GetEdittedExportHost(PACSConfig::ExportHost* pOutExportHost, PACSConfig::ExportHost initExportHost) = 0;
		virtual bool CheckDeleteExportHost() = 0;
	};

	class FM_CORE_EXPORT DefaultPACSSetupInputInterface : public PACSSetupInputInterface
	{
	public:
		DefaultPACSSetupInputInterface(PACSConfig* pConfig, QWidget* pParent);
	public:
		virtual bool GetNewHost(PACSConfig::Host* pOutHost);
		virtual bool GetEdittedHost(PACSConfig::Host* pOutHost, PACSConfig::Host initHost);
		virtual bool CheckDeleteHost();

		virtual bool GetNewExportHost(PACSConfig::ExportHost* pOutExportHost);
		virtual bool GetEdittedExportHost(PACSConfig::ExportHost* pOutExportHost, PACSConfig::ExportHost initExportHost);
		virtual bool CheckDeleteExportHost();
	private:
		PACSConfig* m_pConfig;
		QWidget* m_pParent;
	};
}
