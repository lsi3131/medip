#pragma once

#ifndef PATHBROWSE_DLG_H
#define PATHBROWSE_DLG_H

#include <qdialog.h>

class QLineEdit;

class PathBrowseDialog : public QDialog
{
	Q_OBJECT

public:
	PathBrowseDialog(QString title, QString content, QWidget *parent = NULL);

	QString getPath() { return strPath; };

private slots:
	void OnPathChanged();

private:
	QString	strPath;
	QLineEdit *editPath;
};
#endif
