#include "stdafx.h"
#include "PACSSetupInputInterface.h"
#include "PACSRegisterDialog.h"

namespace fm
{
	//=====================================================
	//			DefaultPACSSetupInputInterface
	//=====================================================
	DefaultPACSSetupInputInterface::DefaultPACSSetupInputInterface(fm::PACSConfig* pConfig, QWidget* pParent) :
		m_pConfig(pConfig),
		m_pParent(pParent)
	{
	}

	bool DefaultPACSSetupInputInterface::GetNewHost(PACSConfig::Host* pOutHost)
	{
		PACSRegisterDialog dlg(m_pConfig, PACSRegisterDialog::ADD, PACSRegisterDialog::QUERY_RETRIEVE_HOST, m_pParent);
		int result = dlg.exec();
		if (result == QDialog::Accepted)
		{
			PACSConfig::Host host;
			host.Name = dlg.Name().toStdWString();
			host.ApplicationEntity = dlg.AE().toStdWString();
			host.IP = dlg.IPAddress().toStdString();
			host.Port = dlg.Port();
			host.Protocol = dlg.Protocol();
			*pOutHost = host;
			return true;
		}
		else
		{
			return false;
		}
	}

	bool DefaultPACSSetupInputInterface::GetEdittedHost(PACSConfig::Host* pOutHost, PACSConfig::Host initHost)
	{
		PACSRegisterDialog dlg(m_pConfig, PACSRegisterDialog::EDIT, PACSRegisterDialog::QUERY_RETRIEVE_HOST, m_pParent);
		dlg.SetName(QString::fromStdWString(initHost.Name));
		dlg.SetIPAddress(initHost.IP.c_str());
		dlg.SetPort(initHost.Port);
		dlg.SetAE(QString::fromStdWString(initHost.ApplicationEntity));
		dlg.SetProtocol(initHost.Protocol);
		if (dlg.exec() == QDialog::Accepted)
		{
			PACSConfig::Host host;
			host.Name = dlg.Name().toStdWString();
			host.ApplicationEntity = dlg.AE().toStdWString();
			host.IP = dlg.IPAddress().toStdString();
			host.Port = dlg.Port();
			host.Protocol = dlg.Protocol();
			*pOutHost = host;
			return true;
		}
		else
		{
			return false;
		}

		return true;
	}

	bool DefaultPACSSetupInputInterface::CheckDeleteHost()
	{
		/* 현재 바로 삭제되도록 설정 */
		//QMessageBox::StandardButton result = QMessageBox::warning(
		//	m_pParent,
		//	QString::fromLocal8Bit("Delete"),
		//	QString::fromLocal8Bit("삭제 하시겠습니까?"),
		//	QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No
		//);
		//return result == QMessageBox::StandardButton::Yes;
		return true;
	}
	bool DefaultPACSSetupInputInterface::GetNewExportHost(PACSConfig::ExportHost* pOutExportHost)
	{
		PACSRegisterDialog dlg(m_pConfig, PACSRegisterDialog::ADD, PACSRegisterDialog::EXPORT_HOST, m_pParent);
		int result = dlg.exec();
		if (result == QDialog::Accepted)
		{
			PACSConfig::ExportHost exportHost;
			exportHost.Name = dlg.Name().toStdWString();
			exportHost.ApplicationEntity = dlg.AE().toStdWString();
			exportHost.IP = dlg.IPAddress().toStdString();
			exportHost.Port = dlg.Port();
			*pOutExportHost = exportHost;
			return true;
		}
		else
		{
			return false;
		}
	}

	bool DefaultPACSSetupInputInterface::GetEdittedExportHost(PACSConfig::ExportHost* pOutExportHost, PACSConfig::ExportHost initExportHost)
	{
		PACSRegisterDialog dlg(m_pConfig, PACSRegisterDialog::EDIT, PACSRegisterDialog::EXPORT_HOST, m_pParent);
		dlg.SetName(QString::fromStdWString(initExportHost.Name));
		dlg.SetIPAddress(initExportHost.IP.c_str());
		dlg.SetPort(initExportHost.Port);
		dlg.SetAE(QString::fromStdWString(initExportHost.ApplicationEntity));

		if (dlg.exec() == QDialog::Accepted)
		{
			PACSConfig::ExportHost host;
			host.Name = dlg.Name().toStdWString();
			host.ApplicationEntity = dlg.AE().toStdWString();
			host.IP = dlg.IPAddress().toStdString();
			host.Port = dlg.Port();

			*pOutExportHost = host;
			return true;
		}
		else
		{
			return false;
		}
	}
	bool DefaultPACSSetupInputInterface::CheckDeleteExportHost()
	{
		return true;
	}
}
