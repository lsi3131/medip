#include "stdafx.h"
#include "FileTreeView.h"
#include <QKeyEvent>
#include <QMenu>
#include <QAction>
#include <QFileSystemModel>
#include <qdesktopservices>
#include <qurl>
#include <QElapsedTimer>
#include <qthread>
#include <qmessagebox>

namespace fm
{
	FileTreeView::FileTreeView(QWidget* parent) :
		QTreeView(parent)
	{
		m_pContextMenu = new QMenu(this);

		m_pActCreateFolder = new QAction(this);
		m_pActCreateFolder->setText(tr("Create Folder"));

		m_pActOpenContainingFolder = new QAction(this);
		m_pActOpenContainingFolder->setText(tr("Open Containing Folder"));

		m_pActRename = new QAction(this);
		m_pActRename->setText(tr("Rename"));

		m_pActDelete = new QAction(this);
		m_pActDelete->setText(tr("Delete"));

		m_pFileSystemModel = new QFileSystemModel();
		m_pFileSystemModel->setReadOnly(false);
		setModel(m_pFileSystemModel);

		setItemsExpandable(false);

		connect(m_pActCreateFolder, &QAction::triggered, this, &FileTreeView::OnCreateFolder);
		connect(m_pActOpenContainingFolder, &QAction::triggered, this, &FileTreeView::OnOpenContainingFolder);
		connect(m_pActRename, &QAction::triggered, this, &FileTreeView::OnRename);
		connect(m_pActDelete, &QAction::triggered, this, &FileTreeView::OnDelete);

		connect(this, &FileTreeView::activated, this, &FileTreeView::OnActivated);

		connect(m_pFileSystemModel, &QFileSystemModel::rootPathChanged, this, &FileTreeView::OnRootPathChanged);
		connect(m_pFileSystemModel, &QFileSystemModel::fileRenamed, this, &FileTreeView::OnFileRenamed);
		connect(m_pFileSystemModel, &QFileSystemModel::directoryLoaded, this, &FileTreeView::OnDirectoryLoaded);
		connect(m_pFileSystemModel, &QFileSystemModel::dataChanged, this, &FileTreeView::OnDataChanged);
		connect(m_pFileSystemModel, &QFileSystemModel::rowsMoved, this, &FileTreeView::OnRowsMoved);
	}

	FileTreeView::~FileTreeView()
	{

	}

	void FileTreeView::Initialize()
	{
		setColumnWidth(0, 140);
		setSortingEnabled(true);
		sortByColumn(0, Qt::SortOrder::AscendingOrder);

		SetFileViewMode(m_fileViewMode);
	}

	void FileTreeView::SetFileViewMode(FileViewMode mode)
	{
		m_fileViewMode = mode;

		//setItemsExpandable(m_fileViewMode.ItemExpandable());
		setItemsExpandable(false);
		if (m_fileViewMode.CanSelectMultiItem())
		{
			setSelectionMode(SelectionMode::ExtendedSelection);
		}
		else
		{
			setSelectionMode(SelectionMode::SingleSelection);
		}
	}

	QFileSystemModel* FileTreeView::GetModel()
	{
		return m_pFileSystemModel;
	}

	QString FileTreeView::GetCurrentDirectoryPath()
	{
		/* 선택된 내용이 없을 경우 */
		QString dirpath;
		dirpath = m_pFileSystemModel->filePath(rootIndex());
		return dirpath;
	}

	void FileTreeView::SetCurrentDirectoryPath(QString dirpath)
	{
		m_pFileSystemModel->setRootPath(dirpath);
		setRootIndex(m_pFileSystemModel->index(dirpath));
		setCurrentIndex(m_pFileSystemModel->index(dirpath));
	}

	/*
		화면을 초기화할 때 사용
	*/
	void FileTreeView::Refresh()
	{
		m_pFileSystemModel->setNameFilters(m_pFileSystemModel->nameFilters());
	}

	void FileTreeView::SetContextMenu()
	{
		m_pContextMenu->clear();
		m_pContextMenu->setContextMenuPolicy(Qt::DefaultContextMenu);
		m_pContextMenu->addSeparator();

		if (m_pActCreateFolder)
		{
			m_pContextMenu->addAction(m_pActCreateFolder);
		}

		if (m_pActOpenContainingFolder)
		{
			m_pContextMenu->addAction(m_pActOpenContainingFolder);
		}

		if (m_pActRename)
		{
			m_pContextMenu->addAction(m_pActRename);
		}

		if (m_pActDelete)
		{
			m_pContextMenu->addAction(m_pActDelete);
		}
	}

	void FileTreeView::RenameDirectory(QModelIndex idx)
	{
		QString fileName = m_pFileSystemModel->filePath(idx);

		if (idx.isValid())
		{
			this->edit(idx);
		}
	}

	void FileTreeView::OnCreateFolder(bool check)
	{
		QString newDirName;
		QString newDirPath;

		int index = 0;
		do
		{
			if (index == 0)
			{
				newDirName = QString("New Folder");
			}
			else
			{
				newDirName = QString("New Folder(%1)").arg(index);
			}
			newDirPath = GetCurrentDirectoryPath() + "/" + newDirName;
			index++;
		} while (QDir(newDirPath).exists());

		QModelIndex newDirIdx = m_pFileSystemModel->mkdir(rootIndex(), newDirName);
		RenameDirectory(newDirIdx);
	}

	void FileTreeView::OnOpenContainingFolder(bool check)
	{
		QString dirpath = GetCurrentDirectoryPath();
		QDesktopServices::openUrl(QUrl::fromLocalFile(dirpath));
	}

	void FileTreeView::OnRename(bool check)
	{
		QModelIndex newIdx = currentIndex();
		RenameDirectory(newIdx);
	}

	void FileTreeView::OnDelete(bool check)
	{
		QMessageBox::StandardButton button = QMessageBox::question(this,
			StringManager::GetString(STR_TITLE_QUESTION),
			StringManager::GetString(STR_QUESTION_DELETE_FILE),
			QMessageBox::StandardButton::Ok | QMessageBox::StandardButton::Cancel
		);

		if (button == QMessageBox::StandardButton::Cancel)
		{
			return;
		}

		QModelIndexList indexList = this->selectedIndexes();
		for (auto& idx : indexList)
		{
			m_pFileSystemModel->remove(idx);
		}
	}

	void FileTreeView::OnActivated(const QModelIndex& index)
	{
	}

	void FileTreeView::OnDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
	{
	}

	void FileTreeView::OnRowsMoved(const QModelIndex& parent, int start, int end, const QModelIndex& destination, int row)
	{
	}

	void FileTreeView::OnRootPathChanged(const QString& newPath)
	{
	}

	void FileTreeView::OnFileRenamed(const QString& path, const QString& oldName, const QString& newName)
	{
		QString renamedFilePath = path + "/" + newName;
		//qDebug() << "file renamed : " << path << ", new name : " << newName << ", renamed path : " << renamedFilePath;
		if (!newName.isEmpty())
		{
			scrollTo(m_pFileSystemModel->index(renamedFilePath));
		}
	}

	void FileTreeView::OnDirectoryLoaded(const QString& path)
	{
		qDebug() << "directory loaded : " << path;
	}

	void FileTreeView::mousePressEvent(QMouseEvent* e)
	{
		if (e->button() == Qt::MouseButton::RightButton)
		{
			QModelIndex index = this->indexAt(e->pos());
			if (index.isValid())
			{
				setCurrentIndex(index);
			}
			else
			{
			}
			/* Context Menu 처리 이후에는 Mouse Press Event Block*/
			SetContextMenu();
			m_pContextMenu->exec(e->globalPos());
			return;
		}
		QTreeView::mousePressEvent(e);
	}

	void FileTreeView::mouseMoveEvent(QMouseEvent* e)
	{
		QTreeView::mouseMoveEvent(e);
	}

	void FileTreeView::mouseReleaseEvent(QMouseEvent* e)
	{
		QTreeView::mouseReleaseEvent(e);
	}

	void FileTreeView::currentChanged(const QModelIndex& current, const QModelIndex& previous)
	{
		QTreeView::currentChanged(current, previous);
		emit currentSelectChanged(current);
	}

	void FileTreeView::rowsInserted(const QModelIndex& parent, int start, int end)
	{
		QTreeView::rowsInserted(parent, start, end);

		QModelIndex child = m_pFileSystemModel->index(start, 0, parent);

		scrollTo(child);
	}

	void FileTreeView::rowsAboutToBeRemoved(const QModelIndex& parent, int start, int end)
	{
		QTreeView::rowsAboutToBeRemoved(parent, start, end);
	}

	void FileTreeView::paintEvent(QPaintEvent* event)
	{
		/* TODO : paintEvent에서 병목 현상 발생. 개선 검토할 것 */
		//QElapsedTimer timer;
		//timer.start();

		QTreeView::paintEvent(event);

		//qInfo() << "paint event file tree view : " << timer.elapsed() << "ms";
	}

	void FileTreeView::keyPressEvent(QKeyEvent* e)
	{
		if (e->key() == Qt::Key_Return ||
			e->key() == Qt::Key_Enter)
		{
			FileTreeView::State s = state();
			if (s == State::EditingState)
			{
			}
			else
			{
				//Enter 기능 Skip. 
				emit returnPressed(currentIndex());
				e->ignore();
				return;
			}
		}
		else if (e->key() == Qt::Key_Space)
		{
			//this->scrollTo(currentIndex());
			return;
		}
		else if (e->key() == Qt::Key_Delete)
		{
			OnDelete(true);
		}
		QTreeView::keyPressEvent(e);
	}

	void FileTreeView::keyReleaseEvent(QKeyEvent* e)
	{
		if (e->key() == Qt::Key_Return ||
			e->key() == Qt::Key_Enter)
		{
		}
		QTreeView::keyReleaseEvent(e);
	}
}

