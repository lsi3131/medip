#include "stdafx.h"
#include "FolderViewWidget.h"
#include "ui_FolderViewWidget.h"
#include "filemanager/appcore/AppCoreContext.h"
#include "filemanager/appcore/UI/FileTreeView.h"
#include "filemanager/appcore/UI/DirectoryTreeView.h"
#include "filemanager/appcore/UI/FavoritesDirectoryListWidget.h"
#include "filemanager/appcore/Dialog/Util/ProgressBarDialog.h"
#include "filemanager/appcore/Util/FileFilterParser.h"
#include "filemanager/appcore/Util/FileSystemUtil.h"
#include "filemanager/appcore/Dialog/PACS/PACSDicomConvertUploadDialog.h"
#include "filemanager/appcore/Resource/IconManager.h"
#include "filemanager/appcore/Dialog/SingleInputDialog.h"
#include "filemanager/dicom/Convert/DicomConverter.h"
#include <qfilesystemmodel>
#include <qradiobutton>
#include <qdebug>
#include <qkeyevent>
#include <qlistwidget>
#include <qelapsedtimer>
#include <qtimer>
#include <qmessagebox>
#include <cmath>
#include <QComboBox>
#include <QStandardItemModel>
#include <utility>
#include <QStandardPaths>

#define MAX_RECENT_LIST_COUNT (20)

namespace fm
{
	DirectoryPathCommand::DirectoryPathCommand(FolderViewWidget* widget, QString prevDirPath, QString curDirPath, QUndoCommand* parent) :
		m_widget(widget),
		m_prevDirectoryPath(prevDirPath),
		m_curDirectoryPath(curDirPath),
		QUndoCommand(parent)
	{
	}

	void DirectoryPathCommand::undo()
	{
		m_widget->UpdateControl_DirectoryPath(m_prevDirectoryPath);
	}

	void DirectoryPathCommand::redo()
	{
		m_widget->UpdateControl_DirectoryPath(m_curDirectoryPath);
	}

	//================================================================
	//			FolderViewWidget
	//================================================================
	FolderViewWidget::FolderViewWidget(QWidget* parent) :
		m_isFirstSetCurrentDirectoryPath(true),
		QWidget(parent)
	{
		m_ui = new Ui::FolderViewWidget();
		m_ui->setupUi(this);

		connect(m_ui->m_btnPrev, &QPushButton::clicked, this, &FolderViewWidget::onPrev);
		connect(m_ui->m_btnNext, &QPushButton::clicked, this, &FolderViewWidget::onNext);
		connect(m_ui->m_btnParent, &QPushButton::clicked, this, &FolderViewWidget::onParent);
		connect(m_ui->m_btnRefresh, &QPushButton::clicked, this, &FolderViewWidget::onBtnRefresh);

		connect(m_ui->m_btnOpen, &QCheckBox::clicked, this, &FolderViewWidget::onBtnFileOpen);
		connect(m_ui->m_btnSelectFolder, &QCheckBox::clicked, this, &FolderViewWidget::onBtnSelectFolder);
		connect(m_ui->m_btnSave, &QCheckBox::clicked, this, &FolderViewWidget::onBtnFileSave);
		connect(m_ui->m_btnCancel, &QCheckBox::clicked, this, &FolderViewWidget::onClose);

		connect(m_ui->m_editDirectoryPath, &QLineEdit::editingFinished, this, &FolderViewWidget::onDirectoryPathEditingFinished);
		connect(m_ui->m_editFileName, &QLineEdit::returnPressed, this, &FolderViewWidget::onEditFileNameReturnPressed);

		connect(m_ui->m_chkFolderOpen, &QCheckBox::clicked, this, &FolderViewWidget::onFolderViewOpen);
		connect(m_ui->m_chkFavorite, &QCheckBox::clicked, this, &FolderViewWidget::onFavoriteViewOpen);
		connect(m_ui->m_chkRecentOpen, &QCheckBox::clicked, this, &FolderViewWidget::onRecentViewOpen);
		connect(m_ui->m_chkSystem, &QCheckBox::clicked, this, &FolderViewWidget::onSystemViewOpen);

		connect(m_ui->m_directoryTreeView, &DirectoryTreeView::clicked, this, &FolderViewWidget::onTreeFolderViewSelected);
		connect(m_ui->m_directoryTreeView, &DirectoryTreeView::returnPressed, this, &FolderViewWidget::onTreeFolderViewSelected);

		connect(m_ui->m_systemListView, &SystemDirectoryView::clicked, this, &FolderViewWidget::onSystemListFolderViewSelected);
		connect(m_ui->m_systemListView, &SystemDirectoryView::returnPressed, this, &FolderViewWidget::onSystemListFolderViewSelected);

		connect(m_ui->m_mainFileTreeView, &FileTreeView::doubleClicked, this, &FolderViewWidget::onFileViewDoubleClicked);
		connect(m_ui->m_mainFileTreeView, &FileTreeView::returnPressed, this, &FolderViewWidget::onFileViewReturnPressed);
		connect(m_ui->m_mainFileTreeView, &FileTreeView::currentSelectChanged, this, &FolderViewWidget::onFileViewSelectChanged);

		connect(m_ui->m_favoriteListWidget, &FavoritesDirectoryListWidget::directorySelected, this, &FolderViewWidget::onFavoritesDirectorySelected);
		connect(m_ui->m_btnAddNewFavorites, &QPushButton::clicked, this, &FolderViewWidget::onFavoritesAddNewDirectory);
		connect(m_ui->m_btnEditFavorites, &QPushButton::clicked, this, &FolderViewWidget::onFavoritesEditDirectory);
		connect(m_ui->m_btnDeleteFavorites, &QPushButton::clicked, this, &FolderViewWidget::onFavoritesDeleteDirectory);

		connect(m_ui->m_listRecentListWidget, &QListWidget::currentItemChanged, this, &FolderViewWidget::onRecentListChanged);
		connect(m_ui->m_listRecentListWidget, &QListWidget::itemDoubleClicked, this, &FolderViewWidget::onRecentListItemDblClicked);

		connect(m_ui->m_cboExtensionTypeFilter, SIGNAL(currentIndexChanged(int)), this, SLOT(onCboExtensionTypeChanged(int)));

		/* Data 클래스 초기화 */
		m_mainFileViewModel = m_ui->m_mainFileTreeView->GetModel();
		m_folderModel = m_ui->m_directoryTreeView->GetModel();
		m_systemModel = m_ui->m_systemListView->GetModel();

		m_ui->m_mainFileTreeView->Initialize();
		m_ui->m_directoryTreeView->Initialize();
		m_ui->m_systemListView->Initialize();

		//QTableWidget
		/* Display Vertical 모드 기본으로 설정 */
		//rdoDisplayVert->setChecked(true);
		m_ui->m_btnRefresh->setVisible(false);

		/* 초기화 */
		m_ui->m_listRecentListWidget->setIconSize(QSize(16, 16));

		m_ui->m_btnAddNewFavorites->setStyleSheet(g_ResourceManager.GetButtonPopupAdd());
		m_ui->m_btnEditFavorites->setStyleSheet(g_ResourceManager.GetButtonPopupEdit());
		m_ui->m_btnDeleteFavorites->setStyleSheet(g_ResourceManager.GetButtonPopupDelete());
	}

	FolderViewWidget::~FolderViewWidget()
	{

	}

	void FolderViewWidget::Init(AppCoreContext* pContext)
	{
		m_pContext = pContext;

		m_ui->m_favoriteListWidget->Init(m_pContext->GetConfigManager()->GetFolderViewConfig());
		InitLayout();
		UpdateControl();
		Refresh();
	}

	void FolderViewWidget::keyPressEvent(QKeyEvent* e)
	{
		if (e->key() == Qt::Key_Backspace)
		{
			onPrev();
		}
		else if (
			e->key() == Qt::Key_Return ||
			e->key() == Qt::Key_Enter
			)
		{
			//enter, return key 처리를 skip
			return;
		}
		else if (
			e->key() == Qt::Key_F2
			)
		{
			//m_ui->m_splitterLeftScrollContainer->refresh();
		}
		QWidget::keyPressEvent(e);
	}

	void FolderViewWidget::onPrev()
	{
		if (m_undoStack.canUndo())
		{
			m_undoStack.undo();
		}
		UpdateControl();
	}

	void FolderViewWidget::onNext()
	{
		if (m_undoStack.canRedo())
		{
			m_undoStack.redo();
		}
		UpdateControl();
	}

	void FolderViewWidget::onParent()
	{
		QDir dir = m_mainFileViewModel->rootDirectory();
		if (dir.cdUp())
		{
			SetCurrentDirectoryPath(dir.path());
		}
	}

	void FolderViewWidget::onBtnRefresh()
	{
		m_ui->m_mainFileTreeView->Refresh();
	}

	void FolderViewWidget::onBtnFileOpen()
	{
		if (m_mode.OpenSaveMode() == FileViewMode::EOpenSaveMode::open)
		{
			QStringList filePathList = GetSelectedFilePathList();

			if (m_mode.CanSelectMultiItem())
			{
				FileListOpen(filePathList);
			}
			else
			{
				QString filePath;
				if (filePathList.isEmpty())
				{
					QString dirpath = m_mainFileViewModel->rootDirectory().path();
					QString fileName = m_ui->m_editFileName->text();
					filePath = dirpath + "/" + fileName;
				}
				else
				{
					filePath = filePathList[0];
				}

				FileOpen(filePath);
			}
		}
	}

	void FolderViewWidget::onBtnSelectFolder()
	{
		QString rootDirpath = m_mainFileViewModel->rootDirectory().path();
		QString fileName = m_ui->m_editFileName->text();
		QString filePath = rootDirpath + "/" + fileName;

		if (fileName.isEmpty())
		{
			SelectFolder(rootDirpath);
		}
		else
		{
			QFileInfo fileInfo(filePath);
			if (fileInfo.isDir())
			{
				SelectFolder(filePath);
			}
			else if (fileInfo.isFile())
			{
				SelectFolder(rootDirpath);
			}
		}

	}

	void FolderViewWidget::onBtnFileSave()
	{
		QString dirpath = m_mainFileViewModel->rootDirectory().path();
		QString fileName = m_ui->m_editFileName->text();
		QString filePath = dirpath + "/" + fileName;

		FileSave(filePath);
	}

	void FolderViewWidget::onClose()
	{
		m_pContext->GetActionManager()->CloseApp();
	}

	void FolderViewWidget::onDirectoryPathEditingFinished()
	{
		QString curDirectoryPath = m_mainFileViewModel->rootDirectory().path();
		QString newDirectoryPath = m_ui->m_editDirectoryPath->text();
		QDir dir;
		if (dir.exists(newDirectoryPath))
		{
			SetCurrentDirectoryPath(newDirectoryPath);
		}
		else
		{
			m_ui->m_editDirectoryPath->setText(QDir::toNativeSeparators(curDirectoryPath));
		}
	}

	void FolderViewWidget::onEditFileNameReturnPressed()
	{
		FileViewMode::EOpenSaveMode mode = m_mode.OpenSaveMode();
		if (mode == FileViewMode::EOpenSaveMode::open)
		{
			onBtnFileOpen();
		}
		else if (mode == FileViewMode::EOpenSaveMode::save)
		{
			onBtnFileSave();
		}
	}

	void FolderViewWidget::onTreeFolderViewSelected(const QModelIndex& index)
	{
		QString filepath = m_folderModel->filePath(index);
		SetCurrentDirectoryPath(filepath);
	}

	void FolderViewWidget::onSystemListFolderViewSelected(const QModelIndex& index)
	{
		QString filepath = m_systemModel->filePath(index);
		SetCurrentDirectoryPath(filepath);
	}

	void FolderViewWidget::onFileViewSelected(const QModelIndex& index)
	{
		if (m_mode.OpenSaveMode() == FileViewMode::EOpenSaveMode::open)
		{
			if (m_mode.FileSelectMode() & FileViewMode::EFileSelectMode::file)
			{
				if (m_mode.CanSelectMultiItem())
				{
					QStringList filePathList = GetSelectedFilePathList();
					FileListOpen(filePathList);
				}
				else
				{
					FileOpen(m_mainFileViewModel->filePath(index));
				}
			}
			else if (m_mode.FileSelectMode() & FileViewMode::EFileSelectMode::directory)
			{
				QFileInfo info = QFileInfo(m_mainFileViewModel->filePath(index));
				if (info.isDir())
				{
					SetCurrentDirectoryPath(info.filePath());
				}
			}
		}
		else if (m_mode.OpenSaveMode() == FileViewMode::EOpenSaveMode::save)
		{
			QFileInfo info = QFileInfo(m_mainFileViewModel->filePath(index));
			if (info.isDir())
			{
				SetCurrentDirectoryPath(info.filePath());
			}
			else
			{
				FileSave(m_mainFileViewModel->filePath(index));
			}
		}
	}

	void FolderViewWidget::onFileViewSelectChanged(const QModelIndex& index)
	{
		QString filepath = m_mainFileViewModel->filePath(index);
		if (m_mainFileViewModel->rootDirectory().absolutePath() == filepath)
		{
			/* root directory일경우 setText 설정 pass*/
			m_ui->m_editFileName->setText("");
			return;
		}
		QFileInfo info = QFileInfo(filepath);
		if (m_mode.CanSelectDirectory())
		{
			m_ui->m_editFileName->setText(info.fileName());
		}
		else /* file mode */
		{
			if (info.isFile())
			{
				m_ui->m_editFileName->setText(info.fileName());
			}
		}
	}

	void FolderViewWidget::onFileViewDoubleClicked(const QModelIndex& index)
	{
		TriggerEventCurrentSingleSelectFile();
	}

	void FolderViewWidget::onFileViewReturnPressed(const QModelIndex& index)
	{
		QStringList filepathList = GetSelectedFilePathList();
		if (!filepathList.isEmpty())
		{
			if (filepathList.count() == 1)
			{
				TriggerEventCurrentSingleSelectFile();
			}
			else if (filepathList.count() > 1)
			{
				onFileViewSelected(index);
			}
		}
	}

	void FolderViewWidget::onRecentListChanged(QListWidgetItem* current, QListWidgetItem* previous)
	{
		if (current)
		{
			QString filepath = current->data(Qt::UserRole).toString();
			if (CheckFilePathExist(filepath))
			{
				QFileInfo fileInfo(filepath);
				QString dirpath;
				if (fileInfo.isDir())
				{
					dirpath = filepath;
				}
				else
				{
					dirpath = fileInfo.dir().absolutePath();
				}

				SetCurrentDirectoryPath(dirpath);
			}
			else
			{
				DeleteFilePathItemFromList(filepath);
			}
		}
	}

	void FolderViewWidget::onRecentListItemDblClicked(QListWidgetItem* item)
	{
		if (item)
		{
			QString path = item->data(Qt::UserRole).toString();
			if (CheckFilePathExist(path))
			{
				QFileInfo fileInfo(path);

				if (fileInfo.isFile())
				{
					QString selectedFileSuffix = fileInfo.suffix().toLower();
					QString currentSuffix = GetCurrentFirstExtensionSuffix().toLower();
					if ((selectedFileSuffix == currentSuffix) || currentSuffix.isEmpty())
					{
						m_pContext->GetActionManager()->FileOpen(path);
					}
				}
			}
			else
			{
				DeleteFilePathItemFromList(path);
			}
		}
	}

	void FolderViewWidget::onFavoritesDirectorySelected(QString dirpath)
	{
		SetCurrentDirectoryPath(dirpath);
	}

	void FolderViewWidget::onFavoritesAddNewDirectory()
	{
		SingleInputDialog dlg(this);
		int result = dlg.exec();
		if (result == QDialog::Accepted)
		{
			QString dirpath = GetCurrentDirectoryPath();
			QString name = dlg.GetData();
			m_ui->m_favoriteListWidget->Add(dirpath, name);
		}
	}

	void FolderViewWidget::onFavoritesEditDirectory()
	{

		SingleInputDialog dlg(this);
		int result = dlg.exec();
		if (result == QDialog::Accepted)
		{
			QString dirpath = GetCurrentDirectoryPath();
			QString name = dlg.GetData();
			m_ui->m_favoriteListWidget->EditName(dirpath, name);
		}
	}

	void FolderViewWidget::onFavoritesDeleteDirectory()
	{
		m_ui->m_favoriteListWidget->DeleteCurSel();
	}

	void FolderViewWidget::onFolderViewOpen(bool checked)
	{
		/* Folder */
		ResizeLeftContainerList();
	}

	void FolderViewWidget::onFavoriteViewOpen(bool checked)
	{
		/* Favorite */
		ResizeLeftContainerList();
	}

	void FolderViewWidget::onSystemViewOpen(bool checked)
	{
		/* System */
		ResizeLeftContainerList();
	}

	void FolderViewWidget::onRecentViewOpen(bool checked)
	{
		/* Recent */
		ResizeLeftContainerList();
	}

	void FolderViewWidget::onCboExtensionTypeChanged(int index)
	{
		QSplitter m;
		if (index >= 0)
		{
			if (m_mode.OpenSaveMode() == FileViewMode::save)
			{
				QString fileName = m_ui->m_editFileName->text();
				SetFileNameWithCurrentExtension(fileName);
			}

			UpdateFileViewByCurrentFilter();
			UpdateRecentList();
		}
	}

	void FolderViewWidget::onDcmFileLoadedStarted()
	{
		//ProgressBarDialog::Show();
		//ProgressBarDialog::SetText("dcm file load started");
		//setEnabled(false);
	}

	void FolderViewWidget::onDcmFileLoaded_InProgress(int progressCount, int maxCount, fm::DicomDataset dcmDataet)
	{
		//ProgressBarDialog::SetText("dcm file load in progress");
	}

	void FolderViewWidget::onDcmFileLoadedFinished()
	{
		//ProgressBarDialog::Hide();
		//ProgressBarDialog::SetText("dcm file load finished");
		//setEnabled(true);
	}

	void FolderViewWidget::InitLayout()
	{
		GUISettingConfig* pGuiSettingConfig = m_pContext->GetConfigManager()->GetGUISettingConfig();

		FolderViewWidgetInfo folderViewWidgetInfo = pGuiSettingConfig->GetFolderViewWidgetInfo();
		m_ui->m_mainFileTreeView->header()->restoreState(folderViewWidgetInfo.FileViewHeaderStatus);
		m_ui->m_centerHorzSplitter->restoreState(folderViewWidgetInfo.CenterHorizontalContainerSplitterStatus);
		m_ui->m_splitterLeftContainer->restoreState(folderViewWidgetInfo.LeftContainerSplitterStatus);
		m_ui->m_chkFolderOpen->setChecked(folderViewWidgetInfo.SubWindowInfo_Folder.Show);
		m_ui->m_chkFavorite->setChecked(folderViewWidgetInfo.SubWindowInfo_Favorite.Show);
		m_ui->m_chkSystem->setChecked(folderViewWidgetInfo.SubWindowInfo_System.Show);
		m_ui->m_chkRecentOpen->setChecked(folderViewWidgetInfo.SubWindowInfo_Recent.Show);

		ResizeLeftContainerList();
	}

	void FolderViewWidget::UpdateControl()
	{
		UpdateFileViewByCurrentFilter();
		ShowControlByMode();

		m_ui->m_btnPrev->setEnabled(m_undoStack.canUndo());
		m_ui->m_btnNext->setEnabled(m_undoStack.canRedo());
	}

	void FolderViewWidget::UpdateExtensionFilterComboBox(QString filters, QString defaultSelectFilter)
	{
		std::vector<FileFilterInfo> filterInfos = FileFilterParser::Parse(filters);

		m_ui->m_cboExtensionTypeFilter->clear();
		for (FileFilterInfo& filterInfo : filterInfos)
		{
			m_ui->m_cboExtensionTypeFilter->addItem(filterInfo.GetPresentationName(), filterInfo.Filters());
		}

		if (!defaultSelectFilter.isEmpty())
		{
			m_ui->m_cboExtensionTypeFilter->setCurrentText(defaultSelectFilter);
		}
	}

	void FolderViewWidget::ShowControlByMode()
	{
		if (m_mode.OpenSaveMode() == FileViewMode::EOpenSaveMode::open)
		{
			bool showFile = m_mode.CanSelectFile();
			bool showDirectory = m_mode.CanSelectDirectory();
			m_ui->m_btnOpen->setVisible(showFile);
			//m_ui->m_btnSelectFolder->setVisible(showDirectory);
			m_ui->m_btnSelectFolder->setVisible(false);		//현재 사용X
			m_ui->m_btnSave->setVisible(false);
		}
		else if (m_mode.OpenSaveMode() == FileViewMode::EOpenSaveMode::save)
		{
			m_ui->m_btnOpen->setVisible(false);
			m_ui->m_btnSelectFolder->setVisible(false);
			m_ui->m_btnSave->setVisible(true);
		}
		else
		{
			m_ui->m_btnOpen->setVisible(true);
			m_ui->m_btnSelectFolder->setVisible(false);
			m_ui->m_btnSave->setVisible(true);
		}

		if (m_mode.CanSelectFile())
		{
			m_ui->m_cboExtensionTypeFilter->setVisible(true);
		}
		else
		{
			m_ui->m_cboExtensionTypeFilter->setVisible(false);
		}
	}

	void FolderViewWidget::SetFileNameWithCurrentExtension(QString fileName)
	{
		/* 파일 모드일 경우에만 적용 */
		if (m_mode.IsOnlyDirectorySelectMode() == false)
		{
			if (!fileName.isEmpty())
			{
				QString extensionFilter = GetCurrentFirstExtensionFilter();

				fileName = FileSystemUtil::RemoveExtension(fileName);
				fileName = FileSystemUtil::AppendExtension(fileName, extensionFilter);
				m_ui->m_editFileName->setText(fileName);
			}
		}
	}

	void FolderViewWidget::UpdateControl_DirectoryPath(QString dirpath)
	{
		m_ui->m_mainFileTreeView->SetCurrentDirectoryPath(dirpath);

		m_ui->m_directoryTreeView->SetCurrentPath(dirpath);

		m_ui->m_editDirectoryPath->setText(QDir::toNativeSeparators(dirpath));
		if (m_mode.IsOnlyDirectorySelectMode())
		{
			m_ui->m_editFileName->setText("");
		}
	}

	bool FolderViewWidget::CheckFilePathExist(QString filepath)
	{
		QFileInfo fileInfo(filepath);
		QUrl url(filepath);
		bool isFileExist = false;
		/*
		현재 원격, 로컬일 경우에 동일하게 처리.
		추후 원격의 경우 Thread로 처리하여 반응성 확보할 것
		*/
		if (url.isLocalFile())
		{
			qInfo() << "open local directory file : " << filepath;
			isFileExist = fileInfo.exists();
		}
		else
		{
			qInfo() << "open remote directory file : " << filepath;
			isFileExist = fileInfo.exists();
		}
		return isFileExist;
	}

	void FolderViewWidget::DeleteFilePathItemFromList(QString filepath)
	{
		QMessageBox::warning(
			this,
			StringManager::GetString(STR_TITLE_WARNING),
			StringManager::GetString(STR_WARN_FILE_NOT_EXIST_AND_DELETE_FROM_LIST),
			QMessageBox::StandardButton::Ok);

		qWarning() << "file path is not exist. delete from list : " << filepath;

		m_pContext->GetDataManager()->GetLastEditedFileDAO()->Delete(filepath);
		Refresh();
	}

	void FolderViewWidget::FileOpen(QString filepath)
	{
		QFileInfo fileInfo = QFileInfo(filepath);
		if (fileInfo.isDir())
		{
			SetCurrentDirectoryPath(fileInfo.filePath());
		}
		else
		{
			QFileInfo fileInfo(filepath);
			if (fileInfo.fileName().isEmpty())
			{
				/* Skip */
				return;
			}

			if (!fileInfo.exists())
			{
				/* Skip */
				return;
			}

			if (IsExtensionTypeMatched(filepath) == false)
			{
				/* Skip */
				return;
			}

			m_pContext->GetActionManager()->FileOpen(filepath);
		}
	}

	void FolderViewWidget::FileListOpen(QStringList pathList)
	{
		m_pContext->GetActionManager()->FileListOpen(pathList);
	}

	void FolderViewWidget::FileSave(QString filepath)
	{
		if (m_mode.CanSelectFile())
		{
			QFileInfo fileInfo(filepath);
			QString dirpath = fileInfo.absoluteDir().absolutePath();
			QString filename = fileInfo.fileName();

			if (filename.isEmpty())
			{
				/* Skip */
				return;
			}

			if (fileInfo.exists())
			{
				QMessageBox::StandardButton result = QMessageBox::question(this,
					StringManager::GetString(STR_TITLE_QUESTION),
					StringManager::GetString(STR_QUESTION_FILE_EXIST_AND_ASK_FILE_CHANGE),
					QMessageBox::Ok |
					QMessageBox::Cancel);

				if (result == QMessageBox::Cancel)
				{
					return;
				}
			}

			QString extension = GetCurrentFirstExtensionFilter();
			filename = FileSystemUtil::AppendExtension(filename, extension);

			m_pContext->GetActionManager()->FileSave(dirpath, filename);
		}
		else if (m_mode.IsOnlyDirectorySelectMode())
		{
			QFileInfo info = QFileInfo(filepath);
			if (info.isDir())
			{
				SetCurrentDirectoryPath(info.filePath());
				m_pContext->GetActionManager()->DirectorySave(filepath);
			}
		}
	}

	void FolderViewWidget::SelectFolder(QString dirpath)
	{
		QFileInfo fileInfo(dirpath);

		if (fileInfo.exists())
		{
			m_pContext->GetActionManager()->DirectorySave(dirpath);
		}
	}

	QString FolderViewWidget::FileName()
	{
		return m_ui->m_editFileName->text();
	}

	QString FolderViewWidget::GetCurrentDirectoryPath()
	{
		return m_ui->m_editDirectoryPath->text();
	}

	void FolderViewWidget::SetFileOpenMode(const FileViewMode& mode)
	{
		m_mode = mode;

		m_ui->m_mainFileTreeView->SetFileViewMode(m_mode);

		m_undoStack.clear();
		m_ui->m_editFileName->setText("");

		UpdateExtensionFilterComboBox(m_mode.Filters(), m_mode.GetDefaultSelectFilter());

		UpdateControl();
	}

	void FolderViewWidget::SetFilePath(QString dirpath, QString fileName)
	{
		/* Directory 이동여부를 Flag로 설정한다. */
		if (!dirpath.isEmpty())
		{
			SetCurrentDirectoryPath(dirpath);
		}
		else
		{
			LastEditedFileDTO lastEditedFile;
			QString filepath = QDir::homePath();
			if (m_pContext->GetDataManager()->GetLastEditedFileDAO()->GetLastest(&lastEditedFile))
			{
				filepath = QString::fromStdWString(lastEditedFile.FilePath);
			}
			QFileInfo fileInfo(filepath);
			if (fileInfo.isDir())
			{
				SetCurrentDirectoryPath(filepath);
			}
			else
			{
				SetCurrentDirectoryPath(fileInfo.dir().absolutePath());
			}
		}
		SetFileNameWithCurrentExtension(fileName);
	}

	void FolderViewWidget::Refresh()
	{
		UpdateRecentList();

		UpdateControl();
	}

	void FolderViewWidget::SetCurrentDirectoryPath(QString dirpath)
	{
		if (m_isFirstSetCurrentDirectoryPath)
		{
			m_isFirstSetCurrentDirectoryPath = false;
			UpdateControl_DirectoryPath(dirpath);
		}
		else
		{
			QString prevDirectoryPath = m_mainFileViewModel->rootDirectory().absolutePath();
			QString curDirectoryPath = dirpath;

			prevDirectoryPath = FileSystemUtil::RemoveLastSlash(prevDirectoryPath);
			curDirectoryPath = FileSystemUtil::RemoveLastSlash(curDirectoryPath);

			if (prevDirectoryPath != curDirectoryPath)
			{
				m_undoStack.push(new DirectoryPathCommand(this, prevDirectoryPath, curDirectoryPath));
			}
		}
		UpdateControl();
	}

	QString FolderViewWidget::GetCurrentSelectedLastFilePath()
	{
		QModelIndex index = m_ui->m_mainFileTreeView->currentIndex();
		return m_mainFileViewModel->filePath(index);
	}

	void FolderViewWidget::SaveStatus()
	{
		/* === 현재 Folder 정보 상태 저장 === */
		GUISettingConfig* pGUISettingConfig = m_pContext->GetConfigManager()->GetGUISettingConfig();

		FolderViewWidgetInfo info = pGUISettingConfig->GetFolderViewWidgetInfo();

		info.FileViewHeaderStatus = m_ui->m_mainFileTreeView->header()->saveState();
		info.LeftContainerSplitterStatus = m_ui->m_splitterLeftContainer->saveState();
		info.CenterHorizontalContainerSplitterStatus = m_ui->m_centerHorzSplitter->saveState();
		info.SubWindowInfo_Folder.Show = m_ui->m_chkFolderOpen->isChecked();
		info.SubWindowInfo_Favorite.Show = m_ui->m_chkFavorite->isChecked();
		info.SubWindowInfo_System.Show = m_ui->m_chkSystem->isChecked();
		info.SubWindowInfo_Recent.Show = m_ui->m_chkRecentOpen->isChecked();

		pGUISettingConfig->SetFolderViewWidgetInfo(info);
		pGUISettingConfig->Save();
	}

	void FolderViewWidget::UpdateFileViewByCurrentFilter()
	{
		setEnabled(true);

		if (m_mode.IsOnlyDirectorySelectMode())
		{
			m_mainFileViewModel->setFilter(QDir::AllDirs | QDir::NoDot | QDir::NoDotDot);
		}
		else
		{
			QStringList extensionFilterList = GetCurrentExtensionFilterList();
			if (extensionFilterList.isEmpty())
			{
				/* Filter가 없으면 화면 비활성화 하도록 수정 */
				setEnabled(false);
			}
			else
			{
				m_mainFileViewModel->setFilter(QDir::AllEntries | QDir::AllDirs | QDir::NoDot | QDir::NoDotDot);
				m_mainFileViewModel->setNameFilters(extensionFilterList);
				m_mainFileViewModel->setNameFilterDisables(false);
			}
		}
	}

	QStringList FolderViewWidget::GetSelectedFilePathList()
	{
		QModelIndexList indexList = m_ui->m_mainFileTreeView->selectionModel()->selectedRows();
		QStringList filePathList;
		for (auto& modelIndex : indexList)
		{
			/* RootDirectory는 Skip한다. */
			QString filepath = m_mainFileViewModel->filePath(modelIndex);
			if (filepath != m_mainFileViewModel->rootDirectory().absolutePath())
			{
				filePathList << m_mainFileViewModel->filePath(modelIndex);
			}
		}
		return filePathList;
	}

	bool FolderViewWidget::IsDirectoryMode()
	{
		return m_mode.IsOnlyDirectorySelectMode();
	}

	bool FolderViewWidget::IsExtensionTypeMatched(QString filepath)
	{
		QFileInfo fileInfoTarget(filepath);
		QStringList extFilters = GetCurrentExtensionFilterList();
		for (QString filter : extFilters)
		{
			/* filter = "*" */
			if (filter == "*" || filter.isEmpty())
			{
				return true;
			}

			/* filter = "XXX.*" */
			QFileInfo fileInfoFilter(filter);
			if (fileInfoFilter.suffix() == "*")
			{
				return true;
			}

			if (fileInfoFilter.suffix().toLower() == fileInfoTarget.suffix().toLower())
			{
				return true;
			}
		}
		return false;
	}

	void FolderViewWidget::ResizeLeftContainerList()
	{
		bool show = false;
		/* Folder */
		show = m_ui->m_chkFolderOpen->isChecked();
		m_ui->m_directoryTreeView->setVisible(show);
		ResizeContainer(m_ui->m_directoryTreeContainer, show);

		/* Favorite */
		show = m_ui->m_chkFavorite->isChecked();
		m_ui->m_favoriteDirectoryContainer->setVisible(show);
		ResizeContainer(m_ui->m_favoriteContainer, show);

		/* System */
		show = m_ui->m_chkSystem->isChecked();
		m_ui->m_systemListView->setVisible(show);
		ResizeContainer(m_ui->m_systemContainer, show);

		/* Recent */
		show = m_ui->m_chkRecentOpen->isChecked();
		m_ui->m_listRecentListWidget->setVisible(show);
		ResizeContainer(m_ui->m_recentContainer, show);
	}

	void FolderViewWidget::ResizeContainer(QWidget* container, bool show)
	{
		if (show)
		{
			container->setMaximumHeight(INT16_MAX);
		}
		else
		{
			QSize hint = container->sizeHint();
			container->setMaximumHeight(hint.height());
		}
	}

	void FolderViewWidget::UpdateRecentList()
	{
		/* Recent Widget 초기화 */
		m_ui->m_listRecentListWidget->clear();

		std::vector<LastEditedFileDTO> lastEditedFiles;
		EntityDataManager* pDataManager = m_pContext->GetDataManager();
		if (pDataManager->GetLastEditedFileDAO()->GetAll(lastEditedFiles))
		{
			int addCount = 0;
			for (int i = 0; i < lastEditedFiles.size(); ++i)
			{
				QFileInfo fileinfo(QString::fromStdWString(lastEditedFiles[i].FilePath));
				QString dirName = fileinfo.dir().dirName();
				QString fileName = fileinfo.fileName();
				QString itemName = dirName + "/" + fileName;
				QString filepath = fileinfo.filePath();

				bool canAddItem = true;

				/* 현재 filter와 파일의 확장자가 일치하는지 확인 */
				if (fileinfo.isFile())
				{
					QString currentSuffix = GetCurrentFirstExtensionSuffix().toLower();
					QString itemSuffix = fileinfo.suffix().toLower();
					if (!currentSuffix.isEmpty())
					{
						canAddItem = (currentSuffix == itemSuffix);
					}
				}

				if (canAddItem)
				{
					QListWidgetItem* item = new QListWidgetItem(itemName);
					QIcon icon = IconManager::GetFileIcon(filepath);
					item->setIcon(icon);
					item->setData(Qt::UserRole, filepath);
					m_ui->m_listRecentListWidget->addItem(item);
					addCount++;
				}

				if (addCount >= MAX_RECENT_LIST_COUNT)
				{
					break;
				}
			}
			//qDebug() << "Refresh folder finished. elapsed time : " << timer.elapsed();
		}
	}

	void FolderViewWidget::TriggerEventCurrentSingleSelectFile()
	{
		QString filepath = GetCurrentSelectedLastFilePath();
		QFileInfo info = QFileInfo(filepath);
		if (info.isDir())
		{
			SetCurrentDirectoryPath(info.filePath());
		}
		else if (info.isFile())
		{
			if (m_mode.OpenSaveMode() == FileViewMode::EOpenSaveMode::open)
			{
				FileOpen(filepath);
			}
			else if (m_mode.OpenSaveMode() == FileViewMode::EOpenSaveMode::save)
			{
				FileSave(filepath);
			}
		}
	}

	QString FolderViewWidget::GetCurrentFirstExtensionFilter()
	{
		if (m_ui->m_cboExtensionTypeFilter->count() <= 0)
		{
			return "";
		}

		QStringList filterList = m_ui->m_cboExtensionTypeFilter->currentData().toStringList();
		if (filterList.isEmpty())
		{
			return "";
		}
		return filterList[0];
	}

	QString FolderViewWidget::GetCurrentFirstExtensionSuffix()
	{
		QString currentFilter = GetCurrentFirstExtensionFilter();
		if (currentFilter.isEmpty())
		{
			return "";
		}

		QFileInfo dir(currentFilter);
		return dir.suffix();
	}

	QStringList FolderViewWidget::GetCurrentExtensionFilterList()
	{
		if (m_ui->m_cboExtensionTypeFilter->count() <= 0)
		{
			return QStringList();
		}

		return m_ui->m_cboExtensionTypeFilter->currentData().toStringList();
	}
}

