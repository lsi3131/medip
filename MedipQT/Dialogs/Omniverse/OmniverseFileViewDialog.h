#pragma once

#include <QDialog>
#include <vector>
#include "Omniverse/OmniverseFileInfo.h"

namespace Ui
{
	class OmniverseFileViewDialog;
}

class OmniverseContext;
class OmniverseStage;
class OmniverseConnector;
class MeshData;

class OmniverseFileViewDialog : public QDialog
{
	Q_OBJECT

public:
	enum eMode
	{
		Open = 0,
		Create
	};
public:
	OmniverseFileViewDialog(OmniverseContext* pOmniverse, eMode mode, QWidget* parent);

public:
	std::string GetFilePath() const;

private:
	void updateDirectoryTreeView();
	void updateFileTreeView();

	std::vector<OmniverseFileInfoPtr> getCurrentDirectoryChildrenInfo() const;

private:
	void addDirectoryItemList(QTreeWidgetItem* parent, const std::vector<OmniverseFileInfoPtr>& infoList);
	void addDirectoryItem(QTreeWidgetItem* parent, const OmniverseFileInfo& info);
	std::string getExportFileUrl() const;
	std::string getExportFilePath() const;

private slots:
	void slot_directoryViewCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
	void slot_fileViewcurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
	void slot_export();
	void slot_cancel();
	void slot_FileExtensionChanged(int index);

private:
	OmniverseContext* m_pOmniverse;
	OmniverseStage* m_pStage;
	OmniverseConnector* m_pConnector;
	QTreeWidgetItem* m_rootItem;
	Ui::OmniverseFileViewDialog* ui;
	std::string m_filePath;
	eMode m_mode;
};
