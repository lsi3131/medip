#include "stdafx.h"
#include "OmniverseLoginInfoEditDialog.h"
#include "ui_OmniverseLoginInfoEditDialog.h"
#include "Omniverse/Config/OmniverseConfig.h"
#include "StyleManager.h"

OmniverseLoginInfoEditDialog::OmniverseLoginInfoEditDialog(OmniverseConfig* pConfig, QWidget* parent) :
	QDialog(parent),
	m_pConfig(pConfig)
{
	ui = new Ui::OmniverseLoginInfoEditDialog();
	ui->setupUi(this);
	setStyleSheet(STYLE_MANAGER->mainFrame);

	ui->m_btnAdd->setStyleSheet(STYLE_MANAGER->buttonBehind);
	ui->m_btnEdit->setStyleSheet(STYLE_MANAGER->buttonBehind);
	ui->m_btnDelete->setStyleSheet(STYLE_MANAGER->buttonBehind);
	ui->m_btnCancel->setStyleSheet(STYLE_MANAGER->buttonBehind);
	ui->m_editServerName->setStyleSheet(STYLE_MANAGER->editBoxNormal);

	connect(ui->m_btnAdd, &QPushButton::clicked, this, &OmniverseLoginInfoEditDialog::slot_Add);
	connect(ui->m_btnEdit, &QPushButton::clicked, this, &OmniverseLoginInfoEditDialog::slot_Edit);
	connect(ui->m_btnDelete, &QPushButton::clicked, this, &OmniverseLoginInfoEditDialog::slot_Delete);
	connect(ui->m_btnCancel, &QPushButton::clicked, this, &OmniverseLoginInfoEditDialog::slot_Cancel);

	ui->m_editServerName->setText(m_pConfig->GetCurrentServer().c_str());
}

void OmniverseLoginInfoEditDialog::slot_Add()
{
	QString serverName = ui->m_editServerName->text();
	if (serverName.isEmpty())
	{
		return;
	}

	m_pConfig->AddServer(serverName.toStdString());
	m_pConfig->SetCurrentServer(serverName.toStdString());

	accept();
}

void OmniverseLoginInfoEditDialog::slot_Edit()
{
	QString newServerName = ui->m_editServerName->text();
	if (newServerName.isEmpty())
	{
		return;
	}

	std::string prevServerName = m_pConfig->GetCurrentServer();

	m_pConfig->EditServer(prevServerName, newServerName.toStdString());
	m_pConfig->SetCurrentServer(newServerName.toStdString());
	accept();
}

void OmniverseLoginInfoEditDialog::slot_Delete()
{
	QString serverName = ui->m_editServerName->text();
	if (serverName.isEmpty())
	{
		return;
	}

	m_pConfig->DeleteServer(serverName.toStdString());
	m_pConfig->SetCurrentServer("");
	accept();
}

void OmniverseLoginInfoEditDialog::slot_Cancel()
{
	reject();
}

