#include "stdafx.h"
#include "PACSRegisterDialog.h"
#include "filemanager/dicom/dicom_defines.h"
#include <qmessagebox>
#include <vector>

namespace fm
{
	PACSRegisterDialog::PACSRegisterDialog(fm::PACSConfig* pConfig, EMode mode, EHostType hostType, QWidget* parent) :
		QDialog(parent),
		m_pConfig(pConfig),
		m_mode(mode),
		m_hostType(hostType),
		m_port(0),
		m_protocol(EDcmProtocol::C_MOVE)
	{
		//setupUi(this);

		this->setSizeGripEnabled(false);
		QVBoxLayout* mainVertLayout = new QVBoxLayout(this);

		/* Top Input */
		QWidget* topInputWidget = new QWidget(this);
		QGridLayout* topInputLayout = new QGridLayout(topInputWidget);

		m_labelName = new QLabel("Name : ", topInputWidget);
		m_labelName->setAlignment(Qt::AlignRight | Qt::AlignTrailing | Qt::AlignVCenter);
		m_editName = new QLineEdit(topInputWidget);

		topInputLayout->addWidget(m_labelName, 0, 0, 1, 1);
		topInputLayout->addWidget(m_editName, 0, 1, 1, 1);

		m_labelIPAddress = new QLabel("IP Address : ", topInputWidget);
		m_labelIPAddress->setAlignment(Qt::AlignRight | Qt::AlignTrailing | Qt::AlignVCenter);
		m_editIPAddress = new QLineEdit(topInputWidget);

		topInputLayout->addWidget(m_labelIPAddress, 1, 0, 1, 1);
		topInputLayout->addWidget(m_editIPAddress, 1, 1, 1, 1);

		m_labelPort = new QLabel("Port : ", topInputWidget);
		m_labelPort->setAlignment(Qt::AlignRight | Qt::AlignTrailing | Qt::AlignVCenter);
		m_editPort = new QLineEdit(topInputWidget);

		topInputLayout->addWidget(m_labelPort, 2, 0, 1, 1);
		topInputLayout->addWidget(m_editPort, 2, 1, 1, 1);

		m_labelApplicationEntity = new QLabel("Application Entity : ", topInputWidget);
		m_labelApplicationEntity->setAlignment(Qt::AlignRight | Qt::AlignTrailing | Qt::AlignVCenter);
		m_editApplicationEntity = new QLineEdit(topInputWidget);

		topInputLayout->addWidget(m_labelApplicationEntity, 3, 0, 1, 1);
		topInputLayout->addWidget(m_editApplicationEntity, 3, 1, 1, 1);

		m_labelProtocol = new QLabel("Protocol : ", topInputWidget);
		m_labelProtocol->setAlignment(Qt::AlignRight | Qt::AlignTrailing | Qt::AlignVCenter);
		m_cboProtocol = new QComboBox(topInputWidget);

		topInputLayout->addWidget(m_labelProtocol, 4, 0, 1, 1);
		topInputLayout->addWidget(m_cboProtocol, 4, 1, 1, 1);

		/* Bottom Button */
		QWidget* bottomButtonList = new QWidget(this);
		QHBoxLayout* bottomButtonListLayout = new QHBoxLayout(bottomButtonList);
		QSpacerItem* horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);


		m_btnSave = new QPushButton(bottomButtonList);
		m_btnSave->setObjectName(QStringLiteral("m_btnPopUpSave"));
		QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		sizePolicy.setHorizontalStretch(0);
		sizePolicy.setVerticalStretch(0);
		sizePolicy.setHeightForWidth(m_btnSave->sizePolicy().hasHeightForWidth());
		m_btnSave->setSizePolicy(sizePolicy);


		m_btnCancel = new QPushButton(bottomButtonList);
		m_btnCancel->setObjectName(QStringLiteral("m_btnPopUpCancel"));
		sizePolicy.setHeightForWidth(m_btnCancel->sizePolicy().hasHeightForWidth());
		m_btnCancel->setSizePolicy(sizePolicy);

		bottomButtonListLayout->addItem(horizontalSpacer);
		bottomButtonListLayout->addWidget(m_btnSave);
		bottomButtonListLayout->addWidget(m_btnCancel);

		/* Vert Layout */
		QSpacerItem* verticalSpacer = new QSpacerItem(20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

		mainVertLayout->addWidget(topInputWidget);
		mainVertLayout->addWidget(bottomButtonList);
		mainVertLayout->addItem(verticalSpacer);

		QWidget::setTabOrder(m_editName, m_editIPAddress);
		QWidget::setTabOrder(m_editIPAddress, m_editPort);
		QWidget::setTabOrder(m_editPort, m_editApplicationEntity);
		QWidget::setTabOrder(m_editApplicationEntity, m_btnSave);
		QWidget::setTabOrder(m_btnSave, m_btnCancel);
		//==============================================================================================

		m_editPort->setValidator(new QIntValidator());
		connect(m_btnSave, &QPushButton::clicked, this, &PACSRegisterDialog::onSave);
		connect(m_btnCancel, &QPushButton::clicked, this, &PACSRegisterDialog::onCancel);

		m_cboProtocol->addItem(EDcmProtocol_Text(C_MOVE).c_str(), (int)EDcmProtocol::C_MOVE);
		m_cboProtocol->addItem(EDcmProtocol_Text(C_GET).c_str(), (int)EDcmProtocol::C_GET);

		if (m_hostType == EHostType::EXPORT_HOST)
		{
			m_labelProtocol->setVisible(false);
			m_cboProtocol->setVisible(false);
		}
		m_btnSave->setStyleSheet(g_ResourceManager.GetButtonSave());
		m_btnCancel->setStyleSheet(g_ResourceManager.GetButtonCancel());
	}

	void PACSRegisterDialog::SetName(QString name)
	{
		m_name = name;
	}

	void PACSRegisterDialog::SetIPAddress(QString ipAddress)
	{
		m_ipAddress = ipAddress;
	}

	void PACSRegisterDialog::SetPort(int port)
	{
		m_port = port;
	}

	void PACSRegisterDialog::SetAE(QString AE)
	{
		m_AETitle = AE;
	}

	void PACSRegisterDialog::SetProtocol(EDcmProtocol protocol)
	{
		m_protocol = protocol;
	}

	QString PACSRegisterDialog::Name()
	{
		return m_name;
	}

	QString PACSRegisterDialog::IPAddress()
	{
		return m_ipAddress;
	}

	int PACSRegisterDialog::Port()
	{
		return m_port;
	}

	QString PACSRegisterDialog::AE()
	{
		return m_AETitle;
	}

	fm::EDcmProtocol PACSRegisterDialog::Protocol()
	{
		return m_protocol;
	}

	int PACSRegisterDialog::exec()
	{
		if (m_mode == EDIT)
		{
			setWindowTitle(QString::fromLocal8Bit("Edit PACS Server"));
			m_editName->setEnabled(false);
			m_editName->setText(m_name);
			m_editIPAddress->setText(m_ipAddress);
			m_editPort->setText(QString::number(m_port));
			m_editApplicationEntity->setText(m_AETitle);
			int index = m_cboProtocol->findData(m_protocol);
			m_cboProtocol->setCurrentIndex(index);
		}
		else
		{
			setWindowTitle(QString::fromLocal8Bit("Add PACS Server"));
		}

		return QDialog::exec();
	}

	void PACSRegisterDialog::onSave()
	{
		QString name = m_editName->text();
		if (name.isEmpty())
		{
			QMessageBox::warning(
				this,
				StringManager::GetString(STR_TITLE_WARNING),
				StringManager::GetString(STR_WARN_REQUEST_NAME_PACS_REGISTER),
				QMessageBox::StandardButton::Ok);
			return;
		}

		if (m_mode == ADD)
		{
			bool isNameExist = false;
			if (m_hostType == QUERY_RETRIEVE_HOST)
			{
				isNameExist = m_pConfig->IsHostNameExist(name.toStdWString());
			}
			else if (m_hostType == EXPORT_HOST)
			{
				isNameExist = m_pConfig->IsExportHostNameExist(name.toStdWString());
			}

			if (isNameExist)
			{
				QMessageBox::warning(
					this,
					StringManager::GetString(STR_TITLE_WARNING),
					StringManager::GetString(STR_WARN_NAME_ALREAY_EXIST),
					QMessageBox::StandardButton::Ok);
				return;
			}
		}

		m_name = m_editName->text();
		m_ipAddress = m_editIPAddress->text();
		m_port = m_editPort->text().toInt();
		m_AETitle = m_editApplicationEntity->text();
		m_protocol = (EDcmProtocol)m_cboProtocol->currentData().toInt();

		accept();
	}

	void PACSRegisterDialog::onCancel()
	{
		reject();
	}
}

