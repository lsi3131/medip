#pragma once

#include <qtreeview>
#include "filemanager/export.h"
#include "filemanager/appcore/UI/FileViewMode.h"

class QFileSystemModel;

namespace fm
{
	class FM_CORE_EXPORT FileTreeView : public QTreeView
	{
		Q_OBJECT
	public:
		FileTreeView(QWidget* parent = nullptr);
		virtual ~FileTreeView();

	public:
		void Initialize();
		void SetFileViewMode(FileViewMode mode);
		QFileSystemModel* GetModel();

		QString GetCurrentDirectoryPath();
		void SetCurrentDirectoryPath(QString dirpath);
		void Refresh();

	private:
		void SetContextMenu();
		void RenameDirectory(QModelIndex idx);
	protected:
		virtual void paintEvent(QPaintEvent *event) override;

		virtual void keyPressEvent(QKeyEvent* e) override;
		virtual void keyReleaseEvent(QKeyEvent* e) override;
		virtual void mousePressEvent(QMouseEvent* e) override;
		virtual void mouseMoveEvent(QMouseEvent* e) override;
		virtual void mouseReleaseEvent(QMouseEvent* e) override;
		virtual void currentChanged(const QModelIndex &current, const QModelIndex &previous) override;

		virtual void rowsInserted(const QModelIndex& parent, int start, int end) override;
		virtual void rowsAboutToBeRemoved(const QModelIndex& parent, int start, int end) override;


	Q_SIGNALS:
		void returnPressed(const QModelIndex &index);
		void currentSelectChanged(const QModelIndex &index);

	private slots:
		void OnCreateFolder(bool check);
		void OnOpenContainingFolder(bool check);
		void OnRename(bool check);
		void OnDelete(bool check);

		void OnActivated(const QModelIndex& model);
		void OnDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles);
		void OnRowsMoved(const QModelIndex& parent, int start, int end, const QModelIndex& destination, int row);

		void OnRootPathChanged(const QString& newPath);
		void OnFileRenamed(const QString& path, const QString& oldName, const QString& newName);
		void OnDirectoryLoaded(const QString& path);
	private:
		QMenu* m_pContextMenu;
		QAction* m_pActFileOpen;
		QAction* m_pActShowDicomTag;

		QAction* m_pActCreateFolder;
		QAction* m_pActOpenContainingFolder;
		QAction* m_pActRename;
		QAction* m_pActDelete;

		QFileSystemModel* m_pFileSystemModel;
		bool m_isEditState = false;

		FileViewMode m_fileViewMode;
	};
}
