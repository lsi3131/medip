#include "stdafx.h"
#include "SettingDialog.h"
#include "filemanager/appcore/AppCoreContext.h"
#include "PACS/PACSSetupQueryRetrieveWidget.h"
#include "PACS/PACSSetupStoreWidget.h"
#include "filemanager/appcore/Event/EventManager.h"
#include <qmessagebox>
#include <qtablewidget>
#include <qpushbutton>
#include <QLineEdit>
#include <qtabwidget>
#include <qlayout>

namespace fm
{
	SettingDialog::SettingDialog(AppCoreContext* pContext, QWidget* parent) :
		QDialog(parent)
	{
		setupUi(this);

		m_btnSave->setStyleSheet(g_ResourceManager.GetButtonSave());
		m_btnCancel->setStyleSheet(g_ResourceManager.GetButtonCancel());

		m_pQueryRetrievalWidget->Init(pContext);
		m_pStoreWidget->Init(pContext);
		m_pAdminWidget->Init(pContext);

		connect(m_btnSave, &QPushButton::clicked, this, &SettingDialog::onSave);
		connect(m_btnCancel, &QPushButton::clicked, this, &SettingDialog::onCancel);
	}

	SettingDialog::~SettingDialog()
	{
	}

	void SettingDialog::onSave()
	{
		m_pQueryRetrievalWidget->Save();
		m_pStoreWidget->Save();
		m_pAdminWidget->Save();

		g_EventManager.settingChanged();
		accept();
	}

	void SettingDialog::onCancel()
	{
		reject();
	}
}

