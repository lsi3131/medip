#pragma once

#include <QDialog>
#include "ui_SessionSelectDialog.h"

class SessionSelectDialog : public QDialog, public Ui::SessionSelectDialog
{
	Q_OBJECT

public:
	SessionSelectDialog(QWidget* parent = nullptr);
	virtual ~SessionSelectDialog();

public:
	QString GetSession() const;

public slots:
	void slot_OK();
	void slot_Cancel();

private:
	QString m_session;

};

