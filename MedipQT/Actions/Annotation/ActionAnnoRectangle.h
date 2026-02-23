#pragma once

#ifndef ACTION_ANNOORETANGLE_H
#define ACTION_ANNOORETANGLE_H

#include "define.h"
#include <QUndoCommand>

#include "Renderer/Slice.h"
#include "graphics/color.h"

class AnnoRectangle;
class AnalMPRPlaneView;

class ActionAnnoRectangleAdd : public QUndoCommand
{
public:
	ActionAnnoRectangleAdd(AnnoRectangle* pAnnoRect, QUndoCommand *parent = NULL);

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	int m_id;
	int m_index;

	AnalMPRPlaneView* m_win;

	mip::VECTOR3 m_vLT;
	mip::VECTOR3 m_vRT;
	mip::VECTOR3 m_vLB;
	mip::VECTOR3 m_vRB;

	COLOR m_color;
};

class ActionAnnoRectangleMove : public QUndoCommand
{
public:
	ActionAnnoRectangleMove(int annoIndex, AnnoRectangle* pOrigin, AnnoRectangle* pNew, QUndoCommand *parent = NULL);
	virtual ~ActionAnnoRectangleMove();
	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	int m_id;
	int m_index;

	std::unique_ptr<AnnoRectangle> m_pOrigin;
	std::unique_ptr<AnnoRectangle> m_pNew;
};
#endif
