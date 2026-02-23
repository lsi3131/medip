#pragma once

#include <qtreeview>
#include <QEventLoop>
#include "filemanager/export.h"

class QAbstractItemModel;
class QFileSystemModel;

namespace fm
{
	class FM_CORE_EXPORT DirectoryTreeView : public QTreeView
	{
		Q_OBJECT
	public:
		DirectoryTreeView(QWidget* parent = nullptr);
		virtual ~DirectoryTreeView();

	public:
		void Initialize();
		void SetCurrentPath(QString dirpath);
		void SetRootDrivePath(QString dirpath);

		QFileSystemModel* GetModel();

	private:
		bool IsRootDriveDirectory(QString dirpath);
		QString GetRootDriveDirectory(QString dirpath);

	protected:
		virtual void keyPressEvent(QKeyEvent* e) override;

	Q_SIGNALS:
		void returnPressed(const QModelIndex& index);

	private:
		QFileSystemModel* m_dirModel;
	};
}
