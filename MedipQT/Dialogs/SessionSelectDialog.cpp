#include "stdafx.h"
#include "SessionSelectDialog.h"

SessionSelectDialog::SessionSelectDialog(QWidget* parent) : 
	QDialog(parent)
{
	setupUi(this);

	m_cboSession->addItem("Session-0");
	m_cboSession->addItem("Session-1");
	m_cboSession->addItem("Session-2");
	m_cboSession->addItem("Session-3");

	connect(m_btnOk, &QPushButton::clicked, this, &SessionSelectDialog::slot_OK);
	connect(m_btnCancel, &QPushButton::clicked, this, &SessionSelectDialog::slot_Cancel);
}

SessionSelectDialog::~SessionSelectDialog()
{
}

QString SessionSelectDialog::GetSession() const
{
	return m_session;
}

void SessionSelectDialog::slot_OK()
{
	m_session = m_cboSession->currentText();
	accept();
}

void SessionSelectDialog::slot_Cancel()
{
	reject();
}

