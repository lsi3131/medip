#pragma once

#ifndef PROJECT_DLG_H
#define PROJECT_DLG_H

#include <qdialog.h>

class QLineEdit;

class ProjectDialog : public QDialog
{
	Q_OBJECT

public:
	ProjectDialog(QWidget* parent = NULL);

	QString getProjName();

	
public slots:
	virtual void accept();

private:
	void	OnOK();

private:
	QLineEdit* m_Name;
};
#endif
