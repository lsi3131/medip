#include "stdafx.h"
#include "DirectoryTreeView.h"
#include <QKeyEvent>
#include <QDirModel>
#include <qfilesystemmodel.h>
#include <qelapsedtimer.h>

namespace fm
{
	DirectoryTreeView::DirectoryTreeView(QWidget* parent) :
		QTreeView(parent)
	{
		m_dirModel = new QFileSystemModel();
		m_dirModel->setFilter(QDir::Dirs | QDir::Drives | QDir::NoDotAndDotDot);
		m_dirModel->setRootPath(QDir::rootPath());
		setModel(m_dirModel);
	}

	DirectoryTreeView::~DirectoryTreeView()
	{
	}

	void DirectoryTreeView::Initialize()
	{
		setHeaderHidden(true);
		setColumnHidden(1, true);
		setColumnHidden(2, true);
		setColumnHidden(3, true);

		sortByColumn(0, Qt::SortOrder::AscendingOrder);
	}

	void DirectoryTreeView::SetCurrentPath(QString dirpath)
	{
		QElapsedTimer timer;
		timer.start();

		SetRootDrivePath(dirpath);
		setCurrentIndex(GetModel()->index(dirpath));

		//qInfo() << "set current path time : " << timer.elapsed() << "mm";
	}

	void DirectoryTreeView::SetRootDrivePath(QString dirpath)
	{
		QDir dir(dirpath);

		QString rootDirPath = GetRootDriveDirectory(dirpath);
		GetModel()->setRootPath(rootDirPath);
	}

	QFileSystemModel* DirectoryTreeView::GetModel()
	{
		return (QFileSystemModel*)model();
	}

	bool DirectoryTreeView::IsRootDriveDirectory(QString dirpath)
	{
		QDir dir(dirpath);
		return (dir.cdUp() == false);
	}

	QString DirectoryTreeView::GetRootDriveDirectory(QString dirpath)
	{
		QDir dir(dirpath);
		while (dir.cdUp())
		{
		}
		return dir.absolutePath();
	}

	void DirectoryTreeView::keyPressEvent(QKeyEvent* e)
	{
		if (e->key() == Qt::Key_Return)
		{
			emit returnPressed(currentIndex());
		}
		QTreeView::keyPressEvent(e);
	}

}


