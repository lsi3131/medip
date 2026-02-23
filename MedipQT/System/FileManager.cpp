#include "stdafx.h"
#include "FileManager.h"

FileManager::FileManager()
{
}

FileManager::~FileManager()
{
}

fm::Server& FileManager::GetServer()
{
	return m_server;
}

fm::EProductType GetProductType(LicenseManager * pLicenseManager)
{
	if (pLicenseManager->isSubscribeFunctionLevel(MFL_Product_DeepCatch) || pLicenseManager->isSubscribeFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2))
	{
		return fm::EProductType::DEEPCATCH;
	}
	else
	{
		return fm::EProductType::MEDIP;
	}
}

QString ImportFileDialog(
	QWidget * parent,
	const QString & caption,
	const QString & fileName,
	const QString & dirpath,
	const QString & filter,
	QFileDialog::Options options)
{
	QString importFilePath;
	QString filepath = dirpath + "/" + fileName;
	QString filterRemoveLastSemicolon = filter;
	if (filter.endsWith(";;"))
	{
		int lastIndex = filter.lastIndexOf(";;");
		filterRemoveLastSemicolon = filter.left(lastIndex);
	}
#ifdef DEV_FILE_MANAGER
	fm::FileManagerDialog* pDlg;
	if (FILE_MANAGER->App.GetDialog(pDlg))
	{
		importFilePath = pDlg->GetImportOpenFilePath(filterRemoveLastSemicolon, fileName);
	}
#else
	QString selectedFilter;
	importFilePath = QFileDialog::getOpenFileName(
		parent,
		caption,
		filepath,
		filterRemoveLastSemicolon,
		&selectedFilter,
		options);
#endif

	return importFilePath;
}

QStringList ImportFileListDialog(
	QWidget *parent,
	const QString &caption,
	const QString &dirpath,
	const QString &filter,
	QFileDialog::Options options)
{
	QStringList importFilePathList;
	QString filepath = dirpath;
	QString filterRemoveLastSemicolon = filter;
	if (filter.endsWith(";;"))
	{
		int lastIndex = filter.lastIndexOf(";;");
		filterRemoveLastSemicolon = filter.left(lastIndex);
	}
#ifdef DEV_FILE_MANAGER
	fm::FileManagerDialog* pDlg;
	if (FILE_MANAGER->App.GetDialog(pDlg))
	{
		importFilePathList = pDlg->GetImportOpenFilePathList(filterRemoveLastSemicolon);
	}
#else
	QFileDialog dialog(parent, caption, dirpath, filterRemoveLastSemicolon);

	dialog.setDirectory(dirpath);
	dialog.setFileMode(QFileDialog::ExistingFiles);

	if (dialog.exec() == QDialog::Accepted)
	{
		importFilePathList = dialog.selectedFiles();
	}
#endif

	return importFilePathList;
}


QString ExportFileDialog(
	QWidget *parent,
	const QString &caption,
	const QString &fileName,
	const QString &dirpath,
	const QString &filter,
	QFileDialog::Options options)
{
	QString exportFilePath;
	QString filepath = dirpath + "/" + fileName;
	QString filterRemoveLastSemicolon = filter;
	if (filter.endsWith(";;"))
	{
		int lastIndex = filter.lastIndexOf(";;");
		filterRemoveLastSemicolon = filter.left(lastIndex);
	}

#ifdef DEV_FILE_MANAGER
	fm::FileManagerDialog* pDlg;
	if (FILE_MANAGER->App.GetDialog(pDlg))
	{
		exportFilePath = pDlg->GetExportSaveFilePath(filterRemoveLastSemicolon, fileName);
	}
#else
	QString selectedFilter;
	exportFilePath = QFileDialog::getSaveFileName(
		parent,
		caption,
		filepath,
		filterRemoveLastSemicolon,
		&selectedFilter,
		options);
#endif

	return exportFilePath;
}

QString ExportDirectoryDialog(QWidget * parent, const QString & caption, const QString & dirpath)
{
	QString exportDirectoryPath;
	QString defaultDirPath = dirpath.isEmpty() ? QDir::homePath() : dirpath;

#ifdef DEV_FILE_MANAGER
	fm::FileManagerDialog* pDlg;
	if (FILE_MANAGER->App.GetDialog(pDlg))
	{
		exportDirectoryPath = pDlg->GetExportDirectoryPath();
	}
#else
	QFileDialog dlg(parent);
	dlg.setFileMode(QFileDialog::DirectoryOnly);
	exportDirectoryPath = dlg.getExistingDirectory(
		parent,
		caption,
		defaultDirPath); // Options options = 0)
#endif

	return exportDirectoryPath;
}

QString ExportPDFDialog(
	QWidget *parent,
	QTextDocument* pDocument,
	const QString &caption,
	const QString &fileName,
	const QString &dirpath,
	const QString &filter,
	QFileDialog::Options options)
{
	QString exportFilePath;
	QString filepath = dirpath + "/" + fileName;

	QString filterRemoveLastSemicolon = filter;
	if (filter.endsWith(";;"))
	{
		int lastIndex = filter.lastIndexOf(";;");
		filterRemoveLastSemicolon = filter.left(lastIndex);
	}

#ifdef DEV_FILE_MANAGER
	fm::FileManagerDialog* pDlg;
	if (FILE_MANAGER->App.GetDialog(pDlg))
	{
		exportFilePath = pDlg->GetExportPDF(pDocument, filterRemoveLastSemicolon, fileName);
	}
#else
	exportFilePath = QFileDialog::getSaveFileName(
		parent,
		caption,
		filepath,
		filterRemoveLastSemicolon,
		nullptr,
		options);
#endif
	return exportFilePath;
}

