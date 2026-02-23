#include "stdafx.h"
#include "FavoritesDirectoryListWidget.h"
#include "filemanager/config/FolderViewConfig.h"
#include "filemanager/appcore/Resource/IconManager.h"
#include <QDir>
#include <QStorageInfo>
#include <QtWin>
#include <QMessageBox>

namespace fm
{
	FavoritesDirectoryListWidget::FavoritesDirectoryListWidget(QWidget* parent) :
		m_pFolderViewConfig(nullptr)
	{
		this->setIconSize(QSize(16, 16));
		connect(this, &QListWidget::itemClicked, this, &FavoritesDirectoryListWidget::onItemClicked);
	}

	void FavoritesDirectoryListWidget::Init(FolderViewConfig* pFolderViewConfig)
	{
		m_pFolderViewConfig = pFolderViewConfig;
		UpdateList();
	}

	bool FavoritesDirectoryListWidget::Add(QString filepath, QString name)
	{
		FavoritesFolder folder;
		folder.Name = name;
		folder.DirectoryPath = filepath;

		std::vector<FavoritesFolder> favoritesFolderList = { folder };
		m_pFolderViewConfig->AddFavoritesFolderList(favoritesFolderList);
		UpdateList();
		return true;
	}

	bool FavoritesDirectoryListWidget::EditName(QString filepath, QString name)
	{
		FavoritesFolder folder;
		folder.Name = name;
		folder.DirectoryPath = filepath;

		m_pFolderViewConfig->ModifyFavoritesFolder(folder);
		UpdateList();
		return true;
	}

	bool FavoritesDirectoryListWidget::DeleteCurSel()
	{
		QListWidgetItem* item = this->currentItem();
		if (item == nullptr)
		{
			return false;
		}
		QString dirpath = item->data(Qt::UserRole).toString();
		m_pFolderViewConfig->DeleteFavoritesFolderByDirectoryPath(dirpath);

		UpdateList();
		return true;
	}

	void FavoritesDirectoryListWidget::UpdateList()
	{
		this->clear();

		m_pFolderViewConfig->Update();
		std::vector<FavoritesFolder> folderList = m_pFolderViewConfig->GetFavoritesFolderList();

		for (FavoritesFolder& folder : folderList)
		{
			QDir dir(folder.DirectoryPath);
			QStorageInfo storage(dir);
			QIcon icon = IconManager::GetFileIcon(dir.absolutePath());

			bool isNull = icon.isNull();
			QList<QSize> list = icon.availableSizes();

			QString itemName;
			if (dir.dirName().isEmpty())
			{
				itemName = QString("%1 - (%2)").arg(folder.Name).arg(storage.rootPath());
			}
			else
			{
				itemName = QString("%1 - %2 (%3)").arg(folder.Name).arg(dir.dirName()).arg(storage.rootPath());
			}
			QListWidgetItem* item = new QListWidgetItem();
			item->setText(itemName);
			item->setIcon(icon);
			item->setData(Qt::UserRole, qVariantFromValue<QString>(dir.absolutePath()));
			//item->setFlags(item->flags() | Qt::ItemIsEditable);
			this->addItem(item);
		}
	}

	void FavoritesDirectoryListWidget::onItemClicked(QListWidgetItem* item)
	{
		if (item)
		{
			QString dirpath = item->data(Qt::UserRole).toString();
			QDir dir(dirpath);
			if (dir.exists() == false)
			{
				QMessageBox::warning(
					this, 
					StringManager::GetString(STR_TITLE_WARNING),
					StringManager::GetString(STR_WARN_FILE_NOT_EXIST_AND_DELETE_FROM_LIST),
					QMessageBox::StandardButton::Ok);

				qWarning() << QString("filepath(=%1) is not exist.\nit will be deleted from list").
					arg(dirpath);
				m_pFolderViewConfig->DeleteFavoritesFolderByDirectoryPath(dirpath);
				UpdateList();
			}
			else
			{
				emit directorySelected(dirpath);
			}
		}
	}
}