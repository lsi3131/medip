#include "stdafx.h"
#include "SystemDirectoryView.h"
#include <QKeyEvent>
#include <QDirModel>
#include <qfilesystemmodel.h>
#include <qelapsedtimer.h>
#include <QStandardPaths>

namespace fm
{
	QString ToFileName(QString filepath)
	{
		QFileInfo info(filepath);
		return info.fileName();
	}

	SystemDirectoryView::SystemDirectoryView(QWidget* parent) :
		QListView(parent)
	{
		QStringList filterList;
		QString homePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);

		filterList << ToFileName(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
		filterList << ToFileName(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
		filterList << ToFileName(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
		filterList << ToFileName(QStandardPaths::writableLocation(QStandardPaths::MusicLocation));
		filterList << ToFileName(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));
		filterList << ToFileName(QStandardPaths::writableLocation(QStandardPaths::MoviesLocation));
		filterList << ToFileName(QStandardPaths::writableLocation(QStandardPaths::FontsLocation));

		m_dirModel = new QFileSystemModel();
		m_dirModel->setFilter(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden);
		m_dirModel->setNameFilters(filterList);
		m_dirModel->setNameFilterDisables(false);
		m_dirModel->setRootPath(homePath);

		QModelIndex modelIndex = m_dirModel->index(homePath);
		setModel(m_dirModel);
		setRootIndex(modelIndex);	
	}

	SystemDirectoryView::~SystemDirectoryView()
	{
	}

	void SystemDirectoryView::Initialize()
	{
	}

	QFileSystemModel* SystemDirectoryView::GetModel()
	{
		return (QFileSystemModel*)model();
	}

	void SystemDirectoryView::keyPressEvent(QKeyEvent* e)
	{
		if (e->key() == Qt::Key_Return)
		{
			emit returnPressed(currentIndex());
		}
		QListView::keyPressEvent(e);
	}

}


