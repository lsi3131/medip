#include "stdafx.h"
#include "PACSSetupStoreWidget.h"
#include "PACSRegisterDialog.h"
#include "filemanager/appcore/AppCoreContext.h"
#include <qmessagebox>
#include <qtablewidget>
#include <qpushbutton>
#include <QLineEdit>

namespace fm
{
	//=====================================================
	//			PACSSetupStoreWidget
	//=====================================================
	PACSSetupStoreWidget::PACSSetupStoreWidget(QWidget* parent) :
		QWidget(parent),
		m_defaultInterface(nullptr)
	{
		setupUi(this);
		InitLayout();

		connect(m_btnEcho, &QPushButton::clicked, this, &PACSSetupStoreWidget::onEcho);
		connect(m_btnAdd, &QPushButton::clicked, this, &PACSSetupStoreWidget::onAdd);
		connect(m_btnEdit, &QPushButton::clicked, this, &PACSSetupStoreWidget::onEdit);
		connect(m_btnDelete, &QPushButton::clicked, this, &PACSSetupStoreWidget::onDelete);

		connect(m_tablePACSList, &QTableWidget::doubleClicked,
			this, &PACSSetupStoreWidget::onPACSListDoubleClicked);

		connect(m_tablePACSList, &QTableWidget::currentItemChanged,
			this, &PACSSetupStoreWidget::onPACSListItemChanged);

		/* TODO : Echo 진행 시 상태 확인을 GUI로 보여주도록 구현할 것 */
		m_btnEcho->hide();

	}

	PACSSetupStoreWidget::~PACSSetupStoreWidget()
	{
		if (m_defaultInterface != nullptr)
		{
			delete m_defaultInterface;
			m_defaultInterface = nullptr;
		}
	}

	void PACSSetupStoreWidget::Init(AppCoreContext* pContext)
	{
		m_pContext = pContext;
		m_pConfig = m_pContext->GetConfigManager()->GetPACSConfig();

		m_defaultInterface = new DefaultPACSSetupInputInterface(m_pConfig, this);
		m_pInputInterface = m_defaultInterface;

		PACSConfig::Listener listener = m_pConfig->GetListener();
		SetupDataModelByConfig();
	}

	bool PACSSetupStoreWidget::Save()
	{
		for (auto& exportHost : m_dataModel.GetAddedExportHostList())
		{
			m_pConfig->AddExportHost(exportHost);
		}

		for (auto& exportHost : m_dataModel.GetModifiedExportHostList())
		{
			m_pConfig->ModifyExportHost(exportHost);
		}

		for (auto& exportHost : m_dataModel.GetDeletedExportHostList())
		{
			m_pConfig->DeleteExportHost(exportHost.Name);
		}

		/* Data Model 초기화 */
		SetupDataModelByConfig();

		return true;
	}

	void PACSSetupStoreWidget::InitLayout()
	{
		QStringList tableColumnHeader = {
			QString::fromLocal8Bit("Name"),
			QString::fromLocal8Bit("IP Address"),
			QString::fromLocal8Bit("Port"),
			QString::fromLocal8Bit("Application Entity"),
		};

		m_tablePACSList->setColumnCount(tableColumnHeader.size());
		m_tablePACSList->setHorizontalHeaderLabels(tableColumnHeader);
		m_tablePACSList->setEditTriggers(QAbstractItemView::NoEditTriggers);
		m_tablePACSList->setAlternatingRowColors(true);
		m_tablePACSList->setSelectionMode(QAbstractItemView::SingleSelection);
		m_tablePACSList->setSelectionBehavior(QAbstractItemView::SelectRows);
		m_tablePACSList->horizontalHeader()->setHighlightSections(false);
	}

	void PACSSetupStoreWidget::SetupDataModelByConfig()
	{
		m_dataModel.Init(m_pConfig->GetExportHosts());
		UpdateByModel();
	}

	void PACSSetupStoreWidget::UpdateByModel()
	{
		std::vector<PACSConfig::ExportHost> exportHosts = m_dataModel.GetExportHostList();

		int newRowCount = exportHosts.size();
		m_tablePACSList->setRowCount(newRowCount);
		for (int i = 0; i < exportHosts.size(); ++i)
		{
			m_tablePACSList->setItem(i, COL_HOST_NAME, new QTableWidgetItem(QString::fromStdWString(exportHosts[i].Name)));
			m_tablePACSList->setItem(i, COL_IP, new QTableWidgetItem(exportHosts[i].IP.c_str()));
			m_tablePACSList->setItem(i, COL_PORT, new QTableWidgetItem(QString::number(exportHosts[i].Port)));
			m_tablePACSList->setItem(i, COL_AETITLE, new QTableWidgetItem(QString::fromStdWString(exportHosts[i].ApplicationEntity)));
		}

		m_tablePACSList->resizeColumnsToContents();
		UpdateControl_As_CurStatus();
	}

	void PACSSetupStoreWidget::UpdateControl_As_CurStatus()
	{
		if (m_tablePACSList->currentRow() < 0)
		{
			m_btnEdit->setEnabled(false);
			m_btnDelete->setEnabled(false);
		}
		else
		{
			m_btnEdit->setEnabled(true);
			m_btnDelete->setEnabled(true);
		}
	}

	void PACSSetupStoreWidget::onEcho()
	{
		/* TODO : echo 기능을 통해 접속된 PACS가 동작되는지 확인 */
		//m_pContext->GetDicomNetworkManager()->Echo_Async();
	}

	void PACSSetupStoreWidget::onAdd()
	{
		PACSConfig::ExportHost exportHost;
		if (m_pInputInterface->GetNewExportHost(&exportHost))
		{
			m_dataModel.AddExportHost(exportHost);
			UpdateByModel();
		}
	}

	void PACSSetupStoreWidget::onEdit()
	{
		int row = m_tablePACSList->currentRow();
		if (row >= 0)
		{
			std::wstring hostName = m_tablePACSList->item(row, COL_HOST_NAME)->text().toStdWString();
			PACSConfig::ExportHost originHost;
			if (m_dataModel.FindExportHostByName(&originHost, hostName))
			{
				PACSConfig::ExportHost targetHost;
				if (m_pInputInterface->GetEdittedExportHost(&targetHost, originHost))
				{
					m_dataModel.EditExportHostByName(targetHost);
					UpdateByModel();
				}
			}
		}
	}

	void PACSSetupStoreWidget::onDelete()
	{
		int row = m_tablePACSList->currentRow();
		if (row >= 0)
		{
			if (m_pInputInterface->CheckDeleteHost())
			{
				std::wstring hostName = m_tablePACSList->item(row, COL_HOST_NAME)->text().toStdWString();
				m_dataModel.Delete(hostName);
				UpdateByModel();
			}
		}
	}


	void PACSSetupStoreWidget::onPACSListDoubleClicked()
	{
		onEdit();
	}

	void PACSSetupStoreWidget::onPACSListItemChanged(QTableWidgetItem* current, QTableWidgetItem* previous)
	{
		UpdateControl_As_CurStatus();
	}
}

