#pragma once

#include <qdialog>
#include "ui_PACSRegisterDialog.h"
#include "FileManager/config/PACSConfig.h"

namespace fm
{

	class PACSRegisterDialog : public QDialog
	{
		Q_OBJECT

	public:
		enum EMode
		{
			ADD,
			EDIT
		};

		enum EHostType
		{
			QUERY_RETRIEVE_HOST,
			EXPORT_HOST
		};

	public:
		PACSRegisterDialog(fm::PACSConfig* pConfig, EMode mode, EHostType hostType, QWidget* parent = nullptr);

	public:
		void SetName(QString name);
		void SetIPAddress(QString ipAddress);
		void SetPort(int port);
		void SetAE(QString AE);
		void SetProtocol(fm::EDcmProtocol protocol);

		QString Name();
		QString IPAddress();
		int Port();
		QString AE();
		fm::EDcmProtocol Protocol();

	public:
		virtual int exec() override;

		private slots:
		void onSave();
		void onCancel();

	private:
		fm::PACSConfig* m_pConfig;

		EMode m_mode;
		EHostType m_hostType;
		QString m_name;
		QString m_ipAddress;
		int m_port;
		QString m_AETitle;
		fm::EDcmProtocol m_protocol;

		QLabel* m_labelName;
		QLabel* m_labelApplicationEntity;
		QLabel* m_labelIPAddress;
		QLabel* m_labelPort;
		QLabel* m_labelProtocol;

		QLineEdit *m_editName;
		QLineEdit *m_editApplicationEntity;
		QLineEdit *m_editIPAddress;
		QLineEdit *m_editPort;
		QComboBox *m_cboProtocol;
		QPushButton *m_btnSave;
		QPushButton *m_btnCancel;
	};
}
