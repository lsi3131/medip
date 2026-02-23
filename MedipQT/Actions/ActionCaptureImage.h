#pragma once

#ifndef ACTION_CAPTUREIMAGE_H
#define ACTION_CAPTUREIMAGE_H

#include "define.h"
#include <QUndoCommand>
#include <QImage>

class ActionCaptureImageAdd : public QUndoCommand
{
public:
	ActionCaptureImageAdd(QImage addImage, QUndoCommand *parent = NULL);

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	QImage _addImg;
	
	int		m_id;
};

class ActionCaptureImageDel : public QUndoCommand
{
public:
	ActionCaptureImageDel(int index, QUndoCommand *parent = NULL);
	
	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	bool	m_first;
	int		m_id;
	int		_index;
	QImage	_img;
};
#endif




