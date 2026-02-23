#pragma once

#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "define.h"
#include "FileManager/net/Server.h"
#include "FileManager/FileManagerAppCore.h"
#include "System/LicenseManager.h"
#include <qfiledialog>

class QTextDocument;

#ifdef DEV_FILE_MANAGER
#define FILE_MANAGER FileManager::GetInstance()

class FileManager
{
public:
	static FileManager* GetInstance()
	{
		static FileManager instance;
		return &instance;
	}
public:
	FileManager();
	virtual ~FileManager();

public:
	fm::Server& GetServer();
public:
	fm::FileManagerAppCore App;
private:
	fm::Server m_server;
};


fm::EProductType GetProductType(LicenseManager * pLicenseManager);

#endif

QString ImportFileDialog(
	QWidget *parent,
	const QString &caption,
	const QString &fileName,
	const QString &dirpath,
	const QString &filter,
	QFileDialog::Options options = QFileDialog::Options()
);

QStringList ImportFileListDialog(
	QWidget *parent,
	const QString &caption,
	const QString &dirpath,
	const QString &filter,
	QFileDialog::Options options = QFileDialog::Options()
);

QString ExportFileDialog(
	QWidget *parent,
	const QString &caption,
	const QString &fileName,
	const QString &dirpath,
	const QString &filter,
	QFileDialog::Options options = QFileDialog::Options()
);

QString ExportDirectoryDialog(
	QWidget *parent,
	const QString &caption,
	const QString &dirpath
);

QString ExportPDFDialog(
	QWidget *parent,
	QTextDocument* pDocument,
	const QString &caption,
	const QString &fileName,
	const QString &dirpath,
	const QString &filter,
	QFileDialog::Options options = QFileDialog::Options()
);
#endif