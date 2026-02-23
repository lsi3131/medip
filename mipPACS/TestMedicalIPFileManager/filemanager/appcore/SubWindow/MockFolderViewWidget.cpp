#include "stdafx.h"
#include "MockFolderViewWidget.h"
#include "ui_FolderViewWidget.h"
#include <qfilesystemmodel>

MockFolderViewWidget::MockFolderViewWidget(AppCoreContext* pContext, QWidget* parent) :
	FolderViewWidget(parent)
{
}

MockFolderViewWidget::~MockFolderViewWidget()
{
}

void MockFolderViewWidget::WaitUntilMainViewDirectoryLoaded()
{
	QFileSystemModel* model = m_ui->m_mainFileTreeView->GetModel();
	QEventLoop loop;
	connect(model, &QFileSystemModel::directoryLoaded, &loop, &QEventLoop::quit);
	loop.exec();
}

QString MockFolderViewWidget::GetDirectoryPath()
{
	return m_ui->m_editDirectoryPath->text();
}

QString MockFolderViewWidget::GetFileName()
{
	return m_ui->m_editFileName->text();
}

QStringList MockFolderViewWidget::GetMainViewFileNameList()
{
	QFileSystemModel* model = m_ui->m_mainFileTreeView->GetModel();
	QModelIndex rootIndex =  m_ui->m_mainFileTreeView->rootIndex();

	int rowCount =  model->rowCount(rootIndex);
	QStringList fileNameList;

	for (int row = 0; row < rowCount; ++row)
	{
		fileNameList << model->fileName(model->index(row, 0, rootIndex));
	}

	return fileNameList;
}
