#pragma once

#ifndef LOGIN_DLG_H
#define LOGIN_DLG_H

#include <QDialog>

class QLineEdit;

class LoginDlg : public QDialog
{
	Q_OBJECT
private slots:
	void loginExecute();
	void idSave();
	void autologin();

public slots:
	void onloginFinished(const sLoginResultInfo &info);
	void onlicenseCheckFinished(const sLoginResultInfo &info);

public:
	LoginDlg(QWidget *parent = Q_NULLPTR);
	~LoginDlg();

	bool IsAutoLoginCheck()		{ return chkBox_autologin->isChecked(); }
	void saveAutoLoginInfo();

private:

	// log in
	QLineEdit *m_lineEdit_id = nullptr;
	QLineEdit *m_lineEdit_pwd = nullptr;
	QCheckBox *chkBox_idsave;
	QCheckBox *chkBox_autologin;

};
#endif