#pragma once

#include <qwidget>
#include <qundostack>
#include <qundocommand>
#include <QListWidgetItem>
#include <QPushButton>
#include <QCheckBox>
#include <QLayout>
#include <QSplitter>
#include <QLineEdit>
#include <QDirModel>
#include "filemanager/export.h"
#include "filemanager/appcore/appcore_defines.h"
#include "filemanager/appcore/UI/FileViewMode.h"
#include "filemanager/dicom/DicomDatasetIO.h"

class QFileSystemModel;

namespace Ui
{
	class FolderViewWidget;
}

namespace fm
{
	class FolderViewWidget;
	class FileTreeView;
	class DirectoryTreeView;
	class AppCoreContext;

	class DirectoryPathCommand : public QUndoCommand
	{
	public:
		DirectoryPathCommand(FolderViewWidget* widget, QString prevDirPath, QString curDirPath, QUndoCommand* parent = 0);

		void undo() override;
		void redo() override;
	private:
		FolderViewWidget* m_widget;
		QString m_prevDirectoryPath;
		QString m_curDirectoryPath;
	};

	class FM_CORE_EXPORT FolderViewWidget : public QWidget
	{
		Q_OBJECT
	public:
		friend DirectoryPathCommand;

	public:
		FolderViewWidget(QWidget* parent = nullptr);
		virtual ~FolderViewWidget();

	public:
		void Init(AppCoreContext* pContext);

		QString FileName();
		QString GetCurrentDirectoryPath();
		QString GetCurrentFirstExtensionFilter();
		QString GetCurrentFirstExtensionSuffix();
		QStringList GetCurrentExtensionFilterList();

		void SetFileOpenMode(const FileViewMode& mode);
		void SetFilePath(QString dirpath, QString fileName);

		void Refresh();

		void SetCurrentDirectoryPath(QString dirpath);
		QString GetCurrentSelectedLastFilePath();

		void SaveStatus();
	protected:
		virtual void keyPressEvent(QKeyEvent* e) override;

	private slots:
		void onPrev();
		void onNext();
		void onParent();
		void onBtnRefresh();
		void onBtnFileOpen();
		void onBtnSelectFolder();
		void onBtnFileSave();
		void onClose();

		void onDirectoryPathEditingFinished();
		void onEditFileNameReturnPressed();

		void onTreeFolderViewSelected(const QModelIndex& index);

		void onSystemListFolderViewSelected(const QModelIndex& index);

		void onFileViewSelected(const QModelIndex& index);
		void onFileViewSelectChanged(const QModelIndex& index);

		void onFileViewDoubleClicked(const QModelIndex& index);
		void onFileViewReturnPressed(const QModelIndex& index);

		void onRecentListChanged(QListWidgetItem* current, QListWidgetItem* previous);
		void onRecentListItemDblClicked(QListWidgetItem* item);

		void onFavoritesDirectorySelected(QString dirpath);
		void onFavoritesAddNewDirectory();
		void onFavoritesEditDirectory();
		void onFavoritesDeleteDirectory();

		void onFolderViewOpen(bool checked);
		void onFavoriteViewOpen(bool checked);
		void onRecentViewOpen(bool checked);
		void onSystemViewOpen(bool checked);

		void onCboExtensionTypeChanged(int index);

		void onDcmFileLoadedStarted();
		void onDcmFileLoaded_InProgress(int progressCount, int maxCount, fm::DicomDataset dcmDataet);
		void onDcmFileLoadedFinished();

	private:
		void InitLayout();
		void UpdateControl();
		void UpdateExtensionFilterComboBox(QString filters, QString defaultSelectFilter);
		void ShowControlByMode();
		void SetFileNameWithCurrentExtension(QString fileName);
		void UpdateControl_DirectoryPath(QString dirpath);

		bool CheckFilePathExist(QString filepath);
		void DeleteFilePathItemFromList(QString filepath);

		void FileOpen(QString filepath);
		void FileListOpen(QStringList filepath);
		void FileSave(QString filepath);
		void SelectFolder(QString dirpath);

		void UpdateFileViewByCurrentFilter();
		QStringList GetSelectedFilePathList();

		bool IsDirectoryMode();
		bool IsExtensionTypeMatched(QString filepath);

		void ResizeLeftContainerList();
		void ResizeContainer(QWidget* container, bool show);

		void UpdateRecentList();
		void TriggerEventCurrentSingleSelectFile();
	protected:
		Ui::FolderViewWidget* m_ui;
		AppCoreContext* m_pContext;

		QFileSystemModel* m_mainFileViewModel;
		QFileSystemModel* m_folderModel;
		QFileSystemModel* m_systemModel;
		QUndoStack m_undoStack;

		FileViewMode m_mode;
		bool m_isFirstSetCurrentDirectoryPath;
	};
}

