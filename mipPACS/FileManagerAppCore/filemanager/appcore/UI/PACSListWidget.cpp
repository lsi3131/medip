#include "stdafx.h"
#include "PACSListWidget.h"
#include "filemanager/appcore/AppCoreContext.h"
#include "filemanager/appcore/Dialog/SettingDialog.h"
#include "filemanager/appcore/Event/EventManager.h"
#include <qlayout>

namespace fm
{
	PACSListWidget::PACSListWidget(QWidget* parent) :
		QWidget(parent)
	{
		QHBoxLayout* layout = new QHBoxLayout();
		layout->setContentsMargins(0, 0, 0, 0);
		layout->setSpacing(0);
		setLayout(layout);

		QLabel* label = new QLabel(tr("PACS : "), this);

		m_cboPACSList = new QComboBox(this);
		m_cboPACSList->setObjectName(tr("m_cboPACSList"));

		connect(&g_EventManager, &EventManager::settingChanged, this, &PACSListWidget::onSettingChanged);

		layout->addWidget(label);
		layout->addWidget(m_cboPACSList);

	}

	void PACSListWidget::Init(AppCoreContext* pContext)
	{
		m_pContext = pContext;
		Reset();
	}

	void PACSListWidget::Reset()
	{
		disconnect(m_cboPACSList, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentPACSChanged(int)));

		m_cboPACSList->clear();
		PACSConfig* pConfig = m_pContext->GetConfigManager()->GetPACSConfig();
		std::vector<PACSConfig::Host> hosts = pConfig->GetQueryRetrieveHosts();
		for (int i = 0; i < hosts.size(); ++i)
		{
			m_cboPACSList->addItem(QString::fromStdWString(hosts[i].Name));
		}
		m_cboPACSList->setCurrentIndex(pConfig->GetSelectedQueryRetrieveHost());

		connect(m_cboPACSList, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentPACSChanged(int)));
	}

	void PACSListWidget::onCurrentPACSChanged(int index)
	{
		if (index >= 0)
		{
			auto pPACSConfig = m_pContext->GetConfigManager()->GetPACSConfig();

			int selectedHost = index;
			if (pPACSConfig->GetSelectedQueryRetrieveHost() != selectedHost)
			{
				pPACSConfig->ModifySelectedHost(selectedHost);
				m_pContext->GetDicomNetworkManager()->SetHostInfoQueryRetrieveByConfig(pPACSConfig);

				emit selectedPACSChanged();
			}
		}
	}

	void PACSListWidget::onSettingChanged()
	{
		PACSConfig* pConfig = m_pContext->GetConfigManager()->GetPACSConfig();
		m_pContext->GetDicomNetworkManager()->SetAllByConfig(pConfig);
		Reset();

		emit selectedPACSChanged();
	}
}
