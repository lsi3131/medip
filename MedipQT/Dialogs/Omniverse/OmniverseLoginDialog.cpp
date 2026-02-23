#include "stdafx.h"
#include "OmniverseLoginDialog.h"
#include "ui_OmniverseLoginDialog.h"
#include "Omniverse/OmniverseContext.h"
#include "Omniverse/Config/OmniverseConfig.h"
#include "Dialogs/Omniverse/OmniverseLoginInfoEditDialog.h"
#include "StyleManager.h"

OmniverseLoginDialog::OmniverseLoginDialog(OmniverseContext* pOmniverse, QWidget* parent) :
	QDialog(parent),
	m_pOmniverse(pOmniverse)
{
	ui = new Ui::OmniverseLoginDialog();
	ui->setupUi(this);
	setStyleSheet(STYLE_MANAGER->mainFrame);

	m_pConfig = m_pOmniverse->GetConfig();

	ui->m_btnConnect->setStyleSheet(STYLE_MANAGER->buttonBehind);
	ui->m_btnCancel->setStyleSheet(STYLE_MANAGER->buttonBehind);
	ui->m_btnEdit->setStyleSheet(STYLE_MANAGER->buttonBehind);
	ui->m_cboServerName->setStyleSheet(STYLE_MANAGER->comboBoxNormal);

	connect(ui->m_cboServerName, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_ServerChanged(int)));
	connect(ui->m_btnConnect, &QPushButton::clicked, this, &OmniverseLoginDialog::slot_Connect);
	connect(ui->m_btnEdit, &QPushButton::clicked, this, &OmniverseLoginDialog::slot_Edit);
	connect(ui->m_btnCancel, &QPushButton::clicked, this, &OmniverseLoginDialog::slot_Cancel);

	updateServerList();
}

QString OmniverseLoginDialog::currentServerName() const
{
	return ui->m_cboServerName->currentText();
}

void OmniverseLoginDialog::updateServerList(const QString& selectServer)
{
	ui->m_cboServerName->blockSignals(true);
	ui->m_cboServerName->clear();
	for (auto& ipAddr : m_pConfig->GetServerList())
	{
		ui->m_cboServerName->addItem(ipAddr.c_str());
	}

	ui->m_cboServerName->setCurrentText(m_pConfig->GetCurrentServer().c_str());
	ui->m_cboServerName->blockSignals(false);
}

void OmniverseLoginDialog::slot_ServerChanged(int index)
{
}

void OmniverseLoginDialog::slot_Connect()
{
	QString serverName = currentServerName();
	if (m_pOmniverse->Connect(serverName.toStdString()) == false)
		//if (g_Omniverse.Connect("192.168.1.193") == false)
	{
		QString message = QString("fail to init omniverse : %1").arg(serverName);
		QMessageBox::warning(this, "warning", message);
		qInfo() << "fail to init omniverse : " << serverName;
		return;
	}

	accept();
}

void OmniverseLoginDialog::slot_Edit()
{
	QString currentServerName = ui->m_cboServerName->currentText();
	m_pConfig->SetCurrentServer(currentServerName.toStdString());

	OmniverseLoginInfoEditDialog dlg(m_pConfig, this);
	dlg.exec();

	updateServerList();
}

void OmniverseLoginDialog::slot_Cancel()
{
	reject();
}

