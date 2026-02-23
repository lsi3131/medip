#include "stdafx.h"
#include "AdminSettingWidget.h"
#include "filemanager/appcore/AppCoreContext.h"
#include <qmessagebox>
#include <qtablewidget>
#include <qpushbutton>
#include <QLineEdit>

namespace fm
{
	//=====================================================
	//			AdminSettingWidget
	//=====================================================
	AdminSettingWidget::AdminSettingWidget(QWidget* parent) :
		QWidget(parent),
		m_pAppConfig(nullptr)
	{
		setupUi(this);

	}

	AdminSettingWidget::~AdminSettingWidget()
	{
	}

	void AdminSettingWidget::Init(AppCoreContext* pContext)
	{
		m_pContext = pContext;
		m_pAppConfig = m_pContext->GetConfigManager()->GetAppConfig();

		m_chkDebugMode->setChecked(m_pAppConfig->IsDebug());
	}

	bool AdminSettingWidget::Save()
	{
		m_pAppConfig->SetDebug(m_chkDebugMode->isChecked());
		return m_pAppConfig->Save();
	}

	void AdminSettingWidget::onChkDebugMode()
	{
	}
}

