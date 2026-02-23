#pragma once

#ifndef ACTION_ANNOLEN_H
#define ACTION_ANNOLEN_H

#include "define.h"
#include <QUndoCommand>
#include <memory>

#include "Renderer/Slice.h"
#include "graphics/color.h"


class AnnoLength;

class ActionAnnoLenAdd : public QUndoCommand
{
public:
	ActionAnnoLenAdd(AnnoLength* pAnnoLength, QUndoCommand *parent = NULL);

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

class ActionAnnoLenMove : public QUndoCommand
{
public:
	ActionAnnoLenMove(int annoIndex, AnnoLength* pOrigin, AnnoLength* pNew);
	virtual ~ActionAnnoLenMove();
	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	int m_id;
	int m_index;

	std::unique_ptr<AnnoLength> m_pOrigin;
	std::unique_ptr<AnnoLength> m_pNew;
};

class ActionAnnoLenEdit : public QUndoCommand
{
public:
	ActionAnnoLenEdit(int annoIndex, QColor _col);
	virtual ~ActionAnnoLenEdit();
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