#pragma once

#ifndef ACTION_ANIMATION_H
#define ACTION_ANIMATION_H

#include "define.h"
#include <QUndoCommand>
#include <qvector3d.h>

class AnimationTab;
class QTreeWidgetItem;
class QTreeWidget;

class ActionPathAdd : public QUndoCommand
{
public:
	ActionPathAdd(QVector3D,int group, QUndoCommand*parent = NULL);

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	int			m_id;
	int			m_group;
	int			m_index;
	int			m_Rindex;
	QVector3D	m_point;
};

class ActionPathDel : public QUndoCommand
{
public:
	ActionPathDel(int,int, QUndoCommand*parent = NULL);
	virtual ~ActionPathDel();

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	int					m_id;
	int					m_count;
	int					m_index;
	int					m_Rindex;
	int					m_group;
	QTreeWidgetItem*	m_item;
	QVector3D			m_point;
};

class ActionPathClear : public QUndoCommand
{
public:
	ActionPathClear(int,QUndoCommand *parent = NULL);
	virtual ~ActionPathClear();

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	int					m_id;
	int					m_group;
	int					m_count;
	int					m_index;
	QTreeWidget*		m_list;
	QTreeWidgetItem**	m_items;
	QVector3D*			m_points;
};

class ActionPathTabAdd : public QUndoCommand
{
public:
	ActionPathTabAdd(QUndoCommand *parent = NULL);
	virtual ~ActionPathTabAdd();
	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	int					m_id;
	int					m_group;
	QTreeWidget*		m_list;
	AnimationTab*		m_tab;
};
#endif






