#pragma once

#ifndef ACTION_BOUNDINGBOX_H
#define ACTION_BOUNDINGBOX_H

#include "define.h"
#include <QtWidgets>
#include <QUndoCommand>
#include <graphics\BoundingBox.h>

class WindowBase;
class ActionBoundingBox : public QUndoCommand
{
public:
	ActionBoundingBox(const BoundingBoxI & box, QUndoCommand *parent = 0);

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	int						m_id;
	BoundingBoxI			m_pre_box;
	BoundingBoxI			m_post_box;
};
#endif
