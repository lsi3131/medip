#pragma once

#ifndef INFO_DLG_H
#define INFO_DLG_H

#include <qdialog.h>
#include <qapplication.h>
#include <qclipboard.h>

#define	LINK_CLICKED	120
#define	MAIL_LINK_CLICKED	130

class QLabel;
class QLineEdit;
class WindowManager;
class LicenseManager;
class StringManager;

class InfoDialog : public QDialog
{
	Q_OBJECT

public:
	InfoDialog(QWidget* parent = NULL);
	virtual ~InfoDialog();

private slots:
	void slot_LinkClick();
	void slot_MailLinkClick();
	void slot_OpenSourceLinkClick();
	void slot_ModuleInfoClick();
	void slot_finish();
	void slot_logOut();

private:
	WindowManager* m_pWinManager;
	LicenseManager* m_pLicense;
	StringManager* m_pStrManager;
	int	 m_rValue;
};
#endif
