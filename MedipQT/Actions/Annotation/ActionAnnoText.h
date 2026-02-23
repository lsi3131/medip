#pragma once

#ifndef ACTION_ANNOOTEXT_H
#define ACTION_ANNOOTEXT_H

#include <QUndoCommand>
#include <QString.h>
#include <QPoint>
#include <QVector3d.h>
#include "define.h"
#include "graphics/color.h"
#include "Math/Vector.h"

class WindowBase;
class AnnoString;

class ActionAnnoTextAdd : public QUndoCommand
{
public:
	ActionAnnoTextAdd(AnnoString* pAnnoText, QUndoCommand *parent = 0);

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	int		m_id;

	int		m_index;
	mip::VECTOR3 m_v;
	int		m_x;
	int		m_y;
	int		m_z;
	QString m_string;
	muint16	m_size;
	COLOR   m_color;
};


class Annotation;
class WindowBase;
class ActionAnnoDel : public QUndoCommand
{
public:
	ActionAnnoDel(WindowBase* view, int annoIndex, QUndoCommand *parent = 0);
	virtual ~ActionAnnoDel();

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	int		m_id;

	int		m_index;
	Annotation * m_anno;
	ANNOTATION_TYPE m_type;
};

class WindowBase;
class ActionAnnoTextMove : public QUndoCommand
{
public:
	ActionAnnoTextMove(WindowBase* view, muint32 annoIndex, AnnoString* pOrigin, AnnoString* pNew, QUndoCommand *parent = 0);

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	int						m_id;
	muint32		m_index;

	std::unique_ptr<AnnoString> m_pOrigin;
	std::unique_ptr<AnnoString> m_pNew;
};





class WindowBase;
class ActionAnnoTextEdit : public QUndoCommand
{
public:
	ActionAnnoTextEdit(muint32 annoIndex, QString & str, muint16 size, QColor & color, QUndoCommand *parent = 0);

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	int						m_id;

	muint32  m_index;
	QString m_string;
	muint16	m_size;
	COLOR   m_color;

	QString m_preString;
	muint16	m_preSize;
	COLOR   m_preColor;
};
#endif
