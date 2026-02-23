#ifndef PASSWORD_DLG_H
#define PASSWORD_DLG_H

#include "define.h"
#include <qdialog.h>

class QLineEdit;

class PasswordDialog : public QDialog
{
	Q_OBJECT

public:
	PasswordDialog(QWidget *parent = NULL);
	virtual ~PasswordDialog();

	QString getPass();
	bool isAccept() { return isOK; }
private:
	QLineEdit *lineIter;
	
	bool isOK;

	QString m_password;
	
	private slots:
	void OnOK();
	
};
#endif
















