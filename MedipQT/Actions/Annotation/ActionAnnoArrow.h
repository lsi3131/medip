#pragma once

#ifndef ACTION_ANNOARROW_H
#define ACTION_ANNOARROW_H

#include "define.h"
#include <QUndoCommand>

#include "Renderer/Slice.h"
#include "graphics/color.h"

class AnnoArrow;

class ActionAnnoArrowAdd : public QUndoCommand
{
public:
	ActionAnnoArrowAdd(AnnoArrow* pAnnoArrow, QUndoCommand *parent = NULL);

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	int						m_id;
	int						m_index;

	mip::VECTOR3			_v1;
	mip::VECTOR3			_v2;
	COLOR					_color;
};

class ActionAnnoArrowMove : public QUndoCommand
{
public:
	ActionAnnoArrowMove(int annoIndex, AnnoArrow* pOrigin, AnnoArrow* pNew, QUndoCommand *parent = NULL);
	virtual ~ActionAnnoArrowMove();
	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	int m_id;
	int m_index;

	std::unique_ptr<AnnoArrow> m_pOrigin;
	std::unique_ptr<AnnoArrow> m_pNew;
};

class ActionAnnoArrowEdit : public QUndoCommand
{
public:
	ActionAnnoArrowEdit(int annoIndex, QColor _col);
	virtual ~ActionAnnoArrowEdit();
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
