#pragma once

#ifndef REPORTTAB_DLG_H
#define REPORTTAB_DLG_H

#include <qdialog.h>

class QLineEdit;

class ReportDirNamingDialog : public QDialog
{
	Q_OBJECT
public:
	explicit ReportDirNamingDialog(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
	~ReportDirNamingDialog();

	QString getDirName() { return m_dirName; };

public slots:
	void OnFinished();
	void OnCancel();

private:
	void createUI();

private:
	QString m_dirName;
	QLineEdit* nameEdit;
};
#endif
