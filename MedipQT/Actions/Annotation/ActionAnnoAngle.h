#pragma once

#ifndef ACTION_ANNOANGLE_H
#define ACTION_ANNOANGLE_H

#include "define.h"
#include <QUndoCommand>

#include "Renderer/Slice.h"
#include "graphics/color.h"

class AnnoAngle;

class ActionAnnoAngleAdd : public QUndoCommand
{
public:
	ActionAnnoAngleAdd(AnnoAngle* pAnnoAngle, QUndoCommand *parent = NULL);

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	int						m_id;
	int						m_index;

	mip::VECTOR3			_v1;
	mip::VECTOR3			_v2;
	mip::VECTOR3			_v3;
	COLOR					_color;
};

class ActionAnnoAngleMove : public QUndoCommand
{
public:
	ActionAnnoAngleMove(int annoIndex, AnnoAngle* pOrigin, AnnoAngle* pNew, QUndoCommand *parent = NULL);
	virtual ~ActionAnnoAngleMove();
	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	int m_id;
	int m_index;

	std::unique_ptr<AnnoAngle> m_pOrigin;
	std::unique_ptr<AnnoAngle> m_pNew;
};

class ActionAnnoAngleEdit : public QUndoCommand
{
public:
	ActionAnnoAngleEdit(int annoIndex, QColor _col);
	virtual ~ActionAnnoAngleEdit();
	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	int m_id;

	int m_index;
	COLOR m_col;
	COLOR m_preCol;
};
#endif