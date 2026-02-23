#include "stdafx.h"
#include "OmniverseFileViewDialog.h"
#include "styleManager.h"
#include "ui_OmniverseFileViewDialog.h"
#include "Omniverse/OmniverseContext.h"
#include "Omniverse/OmniverseStage.h"
#include "Omniverse/OmniverseConnector.h"
#include "ActionManager.h"
#include "StringManager.h"
#include "volumedata.h"

OmniverseFileViewDialog::OmniverseFileViewDialog(OmniverseContext* pOmniverse, eMode mode, QWidget* parent) :
	QDialog(parent),
	m_pOmniverse(pOmniverse),
	m_mode(mode),
	m_rootItem(nullptr)
{
	m_pStage = m_pOmniverse->GetStage();
	m_pConnector = m_pOmniverse->GetConnector();
	ui = new Ui::OmniverseFileViewDialog();
	ui->setupUi(this);
	setStyleSheet(STYLE_MANAGER->mainFrame);

	ui->m_treeDirectoryView->setHeaderHidden(true);
	ui->m_treeDirectoryView->setStyleSheet(STYLE_MANAGER->treeOmniversePresetList);

	ui->m_treeFileView->setHeaderLabels({ "File Name", "Date Modified", "Author" });
	ui->m_treeFileView->setStyleSheet(STYLE_MANAGER->treeOmniversePresetList);

	ui->m_cboFileExtension->addItem(".usd");
	ui->m_cboFileExtension->addItem(".usda");
	ui->m_cboFileExtension->addItem(".live");
	ui->m_cboFileExtension->setCurrentText(".usd");

	ui->m_editFileName->setText("");

	updateDirectoryTreeView();

	if (m_mode == eMode::Open)
	{
		ui->m_btnExport->setText("Open");
	}
	else
	{
		ui->m_btnExport->setText("Create");
	}

	connect(ui->m_treeDirectoryView, &QTreeWidget::currentItemChanged, this, &OmniverseFileViewDialog::slot_directoryViewCurrentItemChanged);
	connect(ui->m_treeFileView, &QTreeWidget::currentItemChanged, this, &OmniverseFileViewDialog::slot_fileViewcurrentItemChanged);

	connect(ui->m_btnExport, &QPushButton::clicked, this, &OmniverseFileViewDialog::slot_export);
	connect(ui->m_btnCancel, &QPushButton::clicked, this, &OmniverseFileViewDialog::slot_cancel);

	connect(ui->m_cboFileExtension, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_FileExtensionChanged(int)));
}

std::string OmniverseFileViewDialog::GetFilePath() const
{
	return m_filePath;
}

void OmniverseFileViewDialog::updateDirectoryTreeView()
{
	OmniverseFileInfoPtr pRoot = m_pConnector->GetRootDirectory();

	m_rootItem = new QTreeWidgetItem();
	m_rootItem->setText(0, QString::fromStdString(pRoot->IPAddress()));
	m_rootItem->setData(0, Qt::UserRole, QString::fromStdString(pRoot->ResourcePath()));

	ui->m_treeDirectoryView->addTopLevelItem(m_rootItem);

	std::vector<OmniverseFileInfoPtr> infoList = pRoot->GetChildren();

	addDirectoryItemList(m_rootItem, infoList);

	ui->m_treeDirectoryView->expandAll();
}

void OmniverseFileViewDialog::updateFileTreeView()
{
	std::vector<OmniverseFileInfoPtr> infoList = getCurrentDirectoryChildrenInfo();
	
	ui->m_treeFileView->clear();
	for (auto& info : infoList)
	{
		if (info->CanHaveChildren() == false)
		{
			QString fileName = QString::fromStdString(info->RelativePath());
			QFileInfo fileInfo(fileName);
			QString extension = "." + fileInfo.suffix();
			QString currentExtension = ui->m_cboFileExtension->currentText();

			if (extension.toLower() == currentExtension.toLower())
			{
				QTreeWidgetItem* item = new QTreeWidgetItem();
				item->setText(0, fileName);
				ui->m_treeFileView->addTopLevelItem(item);
			}
			//item->setData(0, Qt::UserRole, QString::fromStdString(pRoot->ResourcePath()));
		}
	}

	for (int i = 0; i < ui->m_treeFileView->columnCount(); ++i)
	{
		ui->m_treeFileView->resizeColumnToContents(i);
	}
}

std::vector<OmniverseFileInfoPtr> OmniverseFileViewDialog::getCurrentDirectoryChildrenInfo() const
{
	std::vector<OmniverseFileInfoPtr> list;

	QTreeWidgetItem* current = ui->m_treeDirectoryView->currentItem();

	if (current == nullptr)
	{
		return list;
	}

	if (current)
	{
		QString resourcePath = current->data(0, Qt::UserRole).toString();
		OmniverseFileInfoPtr pFileInfo = m_pConnector->FindFileByUrl(resourcePath.toStdString());

		return pFileInfo->GetChildren();
	}

	return list;
}

void OmniverseFileViewDialog::addDirectoryItemList(QTreeWidgetItem* parent, const std::vector<OmniverseFileInfoPtr>& infoList)
{
	QList<QTreeWidgetItem*> itemList = parent->takeChildren();
	for (auto& item : itemList)
	{
		delete item;
	}

	for (auto& pInfo : infoList)
	{
		addDirectoryItem(parent, *pInfo);
	}
}

void OmniverseFileViewDialog::addDirectoryItem(QTreeWidgetItem* parent, const OmniverseFileInfo& info)
{
	if (info.CanHaveChildren())
	{
		QTreeWidgetItem* item = new QTreeWidgetItem();
		std::string relativePath = info.RelativePath();
		std::string resourcePath = info.ResourcePath();
		item->setText(0, QString::fromStdString(info.RelativePath()));
		item->setData(0, Qt::UserRole, QString::fromStdString(info.ResourcePath()));

		parent->addChild(item);
	}
}

std::string OmniverseFileViewDialog::getExportFileUrl() const
{
	QTreeWidgetItem* item = ui->m_treeDirectoryView->currentItem();
	if (item == nullptr)
	{
		return "";
	}
	QString resourcePath = item->data(0, Qt::UserRole).toString();
	OmniverseFileInfoPtr pDirectoryInfo = m_pConnector->FindFileByUrl(resourcePath.toStdString());
	if (pDirectoryInfo == nullptr)
	{
		return "";
	}

	std::string fileName = ui->m_editFileName->text().toStdString();
	if (fileName.empty())
	{
		return "";
	}

	std::string extension = ui->m_cboFileExtension->currentText().toStdString();
	if (extension.empty())
	{
		return "";
	}
	std::string url = pDirectoryInfo->GetUrl() + "/" + fileName + extension;

	return url;
}

std::string OmniverseFileViewDialog::getExportFilePath() const
{
	QTreeWidgetItem* item = ui->m_treeDirectoryView->currentItem();
	if (item == nullptr)
	{
		return "";
	}

	QString resourcePath = item->data(0, Qt::UserRole).toString();
	if (resourcePath.isEmpty())
	{
		return "";
	}

	std::string fileName = ui->m_editFileName->text().toStdString();
	if (fileName.empty())
	{
		return "";
	}

	std::string extension = ui->m_cboFileExtension->currentText().toStdString();
	if (extension.empty())
	{
		return "";
	}
	std::string url = resourcePath.toStdString() + "/" + fileName + extension;

	return url;
}


void OmniverseFileViewDialog::slot_directoryViewCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
	if (current)
	{
		std::vector<OmniverseFileInfoPtr> childen = getCurrentDirectoryChildrenInfo();
		addDirectoryItemList(current, childen);
		updateFileTreeView();
	}
}

void OmniverseFileViewDialog::slot_fileViewcurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
	if (current)
	{
		QString fileName = current->text(0);
		QFileInfo fileInfo(fileName);
		ui->m_editFileName->setText(fileInfo.baseName());
	}
}

void OmniverseFileViewDialog::slot_export()
{
	m_filePath = getExportFilePath();

	if (m_filePath.empty())
	{
		QString message = "file path is empty. please enter the file path";
		QMessageBox::warning(this, STRING_MANAGER->getString(STR_WARN), message);
		return;
	}

	if (m_mode == eMode::Create)
	{
		if (m_pConnector->IsStageExist(m_filePath))
		{
			QString message = "file already exists";
			QMessageBox::warning(this, STRING_MANAGER->getString(STR_WARN), message);
			return;
		}
	}
	else
	{
		if (m_pConnector->IsStageExist(m_filePath) == false)
		{
			QString message = "file is not exists";
			QMessageBox::warning(this, STRING_MANAGER->getString(STR_WARN), message);
			return;
		}
	}

	accept();
}

void OmniverseFileViewDialog::slot_cancel()
{
	reject();
}

void OmniverseFileViewDialog::slot_FileExtensionChanged(int index)
{
	updateFileTreeView();
}

