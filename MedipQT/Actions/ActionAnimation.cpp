#include "stdafx.h"
#include "ActionAnimation.h"
#include "Windows/windowManager.h"
#include "Windows/Tabwindow.h"
#include "System/styleManager.h"

ActionPathAdd::ActionPathAdd(QVector3D vec, int _group, QUndoCommand * parent)
	:QUndoCommand(parent)
{
	static int s_id = ACT_ID_ANNO_PATH_ADD;
	m_id = s_id++;

	m_point = vec;
	m_group = _group;

	m_Rindex = m_index = WIN_MANAGER->aniCount.at(m_group);
	m_Rindex += WIN_MANAGER->getAniStartPoint(m_group);

}

void ActionPathAdd::undo()
{
	WIN_MANAGER->setSaveState(false);

	AnimationTab* tab = WIN_MANAGER->GetTab()->getAniTab();

	if (tab)
	{
		QTreeWidgetItem* item = tab->takeItem(m_index, m_group);

		if (item)
			SAFE_DELETE(item);
	}

	WIN_MANAGER->aniLine.remove(m_Rindex);
	WIN_MANAGER->aniCount.replace(m_group, m_index);

	if (tab)
		tab->UpdateLength(m_group);

	WIN_MANAGER->renderLater_GridView();
}

void ActionPathAdd::redo()
{
	WIN_MANAGER->setSaveState(false);

	WIN_MANAGER->aniLine.insert(m_Rindex, m_point);

	AnimationTab* tab = WIN_MANAGER->GetTab()->getAniTab();

	if (tab)
	{
		tab->StopAnimation();
		tab->AddAnipoint(m_index, m_group);
	}

	int val = m_index + 1;
	WIN_MANAGER->aniCount.replace(m_group, val);

	if (tab)
		tab->UpdateLength(m_group);

	WIN_MANAGER->renderLater_GridView();
}

bool ActionPathAdd::mergeWith(const QUndoCommand * command)
{
	return false;
}

ActionPathDel::ActionPathDel(int index, int _group, QUndoCommand * parent)
	: QUndoCommand(parent)
{
	static int s_id = ACT_ID_ANNO_PATH_DEL;

	m_id = s_id++;

	m_item = NULL;
	m_group = _group;
	m_count = WIN_MANAGER->aniCount.at(m_group);
	m_Rindex = m_index = index;
	for (int i = 0; i < m_group; i++)
		m_Rindex += WIN_MANAGER->aniCount.at(i);
}

ActionPathDel::~ActionPathDel()
{
	SAFE_DELETE(m_item);
}

void ActionPathDel::undo()
{
	WIN_MANAGER->setSaveState(false);

	WIN_MANAGER->aniLine.insert(m_Rindex, m_point);

	AnimationTab* tab = WIN_MANAGER->GetTab()->getAniTab();

	if (tab && NULL != m_item)
	{
		tab->StopAnimation();
		tab->AddAnipoint(m_index, m_group, m_item);
		m_item = NULL;
	}

	WIN_MANAGER->aniCount.replace(m_group, m_count);

	if (tab)
		tab->UpdateLength(m_group);

	WIN_MANAGER->renderLater_GridView();
}

void ActionPathDel::redo()
{
	WIN_MANAGER->setSaveState(false);

	AnimationTab* tab = WIN_MANAGER->GetTab()->getAniTab();

	if (tab)
	{
		QTreeWidgetItem* item = tab->takeItem(m_index, m_group);

		if (item)
			m_item = item;
	}

	m_point = WIN_MANAGER->aniLine.takeAt(m_Rindex);

	WIN_MANAGER->aniCount.replace(m_group, m_count - 1);

	if (tab)
		tab->UpdateLength(m_group);

	WIN_MANAGER->renderLater_GridView();
}

bool ActionPathDel::mergeWith(const QUndoCommand * command)
{
	return false;
}

ActionPathClear::ActionPathClear(int _group, QUndoCommand * parent)
	:QUndoCommand(parent)
{
	static int s_id = ACT_ID_ANNO_PATH_CLEAR;

	m_id = s_id++;

	m_group = _group;
	m_count = WIN_MANAGER->aniCount.at(m_group);

	m_items = NULL;
	m_points = NULL;
	m_list = NULL;
}

ActionPathClear::~ActionPathClear()
{
	if (m_items)
	{
		for (int i = 0; i < m_count; i++)
			SAFE_DELETE(m_items[i]);

		SAFE_DELETES(m_items);
		SAFE_DELETES(m_points);

		m_list->deleteLater();
	}
}

void ActionPathClear::undo()
{
	WIN_MANAGER->setSaveState(false);

	AnimationTab* tab = WIN_MANAGER->GetTab()->getAniTab();

	if (tab)
	{
		tab->StopAnimation();
		if (m_list)
			tab->InsertList(m_list, m_group);
	}

	if (m_count > 0)
	{
		m_index = WIN_MANAGER->getAniStartPoint(m_group);
		for (int i = 0; i < m_count; i++)
		{
			WIN_MANAGER->aniLine.insert(m_index + i, m_points[i]);
			if (NULL != tab && NULL != m_items[i])
			{
				tab->AddAnipoint(i, m_group, m_items[i]);
				m_items[i] = NULL;
			}
		}
	}

	SAFE_DELETES(m_points);
	SAFE_DELETES(m_items);
	m_points = NULL;
	m_items = NULL;
	m_list = NULL;


	if (0 != m_group)
		WIN_MANAGER->aniCount.insert(m_group, m_count);
	else
		WIN_MANAGER->aniCount.replace(m_group, m_count);


	if (tab)
		tab->UpdateLength(m_group);

	WIN_MANAGER->renderLater_GridView();
}

void ActionPathClear::redo()
{
	WIN_MANAGER->setSaveState(false);

	AnimationTab* tab = WIN_MANAGER->GetTab()->getAniTab();

	if (m_count > 0)
	{
		m_items = new QTreeWidgetItem*[m_count];
		m_points = new QVector3D[m_count];
		m_index = WIN_MANAGER->getAniStartPoint(m_group);

		for (int i = 0; i < m_count; i++)
		{
			if (tab)
				m_items[i] = tab->takeItem(0, m_group);

			m_points[i] = WIN_MANAGER->aniLine.takeAt(m_index);			
		}
	}

	if (m_group != 0)
	{
		m_list = tab->takeList(m_group);
		WIN_MANAGER->aniCount.removeAt(m_group);
	}
	else
		WIN_MANAGER->aniCount.replace(m_group, 0);

	if (tab)
		tab->UpdateLength(m_group);

	WIN_MANAGER->renderLater_GridView();
}

bool ActionPathClear::mergeWith(const QUndoCommand * command)
{
	return false;
}

ActionPathTabAdd::ActionPathTabAdd(QUndoCommand * parent)
	:QUndoCommand(parent)
{
	static int s_id = ACT_ID_ANNO_PATH_TAB_ADD;

	m_id = s_id++;

	m_group = WIN_MANAGER->aniCount.count();

	m_tab = WIN_MANAGER->GetTab()->getAniTab();
	if (m_tab)
	{
		m_list = new QTreeWidget(m_tab);
		m_list->setColumnCount(1);
		m_list->setContentsMargins(0, 0, 0, 5);
		m_list->setHeaderHidden(true);
		m_list->setStyleSheet(STYLE_MANAGER->treeAniList);
		m_list->setRootIsDecorated(false);
		m_list->setMinimumWidth(20);
		m_list->setContextMenuPolicy(Qt::CustomContextMenu);
		m_list->setSelectionMode(QAbstractItemView::ExtendedSelection);
		m_list->installEventFilter(m_tab);
	}
}

ActionPathTabAdd::~ActionPathTabAdd()
{
	if (m_list)
		m_list->deleteLater();
}

void ActionPathTabAdd::undo()
{
	if (m_tab)
	{
		m_list = m_tab->takeList(m_group);

		WIN_MANAGER->aniCount.removeAt(m_group);
	}
}

void ActionPathTabAdd::redo()
{
	if (m_tab)
	{
		WIN_MANAGER->aniCount.insert(m_group, 0);

		m_tab->InsertList(m_list, m_group);
	}
	m_list = NULL;

}

bool ActionPathTabAdd::mergeWith(const QUndoCommand * command)
{
	return false;
}
