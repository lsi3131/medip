#include "stdafx.h"
#include "ModuleInfoDialog.h"
#include "System/Version/FileVersionUtil.h"
#include "StringManager.h"
#include "styleManager.h"

ModuleInfoDialog::ModuleInfoDialog(QWidget* parent) :
	QDialog(parent)
{
	setupUi(this);

	this->setStyleSheet(STYLE_MANAGER->mainFrame);

	m_tab->setStyleSheet(STYLE_MANAGER->listTabWidget);

	m_treeModule_Internal->setHeaderLabels({"Name", "Version"});
	m_treeModule_Internal->setStyleSheet(STYLE_MANAGER->treeWithHeaderList);

	m_treeAIWeight->setHeaderLabels({"Name", "Version", "Description"});
	m_treeAIWeight->setStyleSheet(STYLE_MANAGER->treeWithHeaderList);

	updateInternalModuleList();
	updateAIWeightList();
}

void ModuleInfoDialog::updateInternalModuleList()
{
	m_treeModule_Internal->clear();
	std::vector<std::tuple<QString, QString>> moduleVersionList = FileVersionUtil::GetModuleVersionList_Internal(STRING_MANAGER);

	for (const auto& t : moduleVersionList)
	{
		QString name = std::get<0>(t);
		QString version = std::get<1>(t);

		QTreeWidgetItem* item = new QTreeWidgetItem();
		item->setText(0, name);
		item->setText(1, version);

		m_treeModule_Internal->addTopLevelItem(item);
	}

	for (int i = 0; i < m_treeModule_Internal->topLevelItemCount(); ++i)
	{
		m_treeModule_Internal->resizeColumnToContents(i);
	}
}

void ModuleInfoDialog::updateAIWeightList()
{
	m_treeAIWeight->clear();
	std::vector<AIWeightFileInfo> aiWeightVersionList = FileVersionUtil::GetAIWeightVersionList(STRING_MANAGER);

	for (const auto& t : aiWeightVersionList)
	{
		QTreeWidgetItem* item = new QTreeWidgetItem();
		item->setText(0, t.Name);
		item->setText(1, t.Version);
		item->setText(2, t.Description);

		m_treeAIWeight->addTopLevelItem(item);
	}

	for (int i = 0; i < m_treeAIWeight->topLevelItemCount(); ++i)
	{
		m_treeAIWeight->resizeColumnToContents(i);
	}
}

