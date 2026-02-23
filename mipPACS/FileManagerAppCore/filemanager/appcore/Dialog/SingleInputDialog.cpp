#include "stdafx.h"
#include "SingleInputDialog.h"
#include <qpushbutton>
#include "filemanager/appcore/Resource/ResourceManager.h"

namespace fm
{
	SingleInputDialog::SingleInputDialog(QWidget* parent) :
		QDialog(parent)
	{
		setupUi(this);

		
		connect(m_editInput, &QLineEdit::returnPressed, this, &SingleInputDialog::onReturnPressed);
		connect(m_btnOk, &QPushButton::clicked, this, &SingleInputDialog::onOk);
		connect(m_btnCancel, &QPushButton::clicked, this, &SingleInputDialog::onCancel);

		m_btnOk->setStyleSheet(g_ResourceManager.GetButtonSave());
		m_btnCancel->setStyleSheet(g_ResourceManager.GetButtonCancel());
	}

	SingleInputDialog::~SingleInputDialog()
	{
	}

	QString SingleInputDialog::GetData()
	{
		return m_data;
	}

	void SingleInputDialog::onReturnPressed()
	{
		onOk();
	}

	void SingleInputDialog::onOk()
	{
		m_data = m_editInput->text();
		if (m_data.isEmpty())
		{
			return;
		}
		accept();
	}

	void SingleInputDialog::onCancel()
	{
		reject();
	}
}

