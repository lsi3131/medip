#pragma once

#ifndef ACTION_IMAGE_LIST_H
#define ACTION_IMAGE_LIST_H

#include "define.h"
#include <QUndoCommand>
#include <qimage.h>

class QListWidgetItem;
class QImage;

class ActionImageListDel : public QUndoCommand
{
public:
	ActionImageListDel(int index, QUndoCommand *parent = NULL);

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	int m_id;
	int m_index;
	bool m_first;
	QImage m_img;
};

class ActionImageListImport : public QUndoCommand
{
public:
	ActionImageListImport(const QImage& addImage, QUndoCommand *parent = NULL);

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	int m_id;
	QImage m_img;
};
#endif

