#include "stdafx.h"
#include "ActionAnnoArrow.h"
#include "Windows/windowManager.h"
#include "Windows/Tabwindow.h"
#include "Annotation/AnnoArrow.h"

ActionAnnoArrowAdd::ActionAnnoArrowAdd(AnnoArrow* pAnnoArrow, QUndoCommand * parent)
{
	static int s_id = ACT_ID_ANNO_ARROW_ADD;

	_v1 = pAnnoArrow->getV1();
	_v2 = pAnnoArrow->getV2();
	_color = pAnnoArrow->getColor();

	m_id = s_id++;
}

void ActionAnnoArrowAdd::undo()
{
	Annotation *ano = WIN_MANAGER->anotationList[m_index];
	SAFE_DELETE(ano);

	WIN_MANAGER->anotationList.pop_back();

	if (WIN_MANAGER->GetTab())
	{
		AnnotationTab2* tab = WIN_MANAGER->GetTab()->getAnnoTab();

		if (tab)
			tab->UpdateAnnoList(WIN_MANAGER->mainTabType);
	}

	WIN_MANAGER->setSaveState(false);

	WIN_MANAGER->renderLater_GridView();
}

void ActionAnnoArrowAdd::redo()
{
	m_index = WIN_MANAGER->anotationList.size();

	AnnoArrow *ano = new AnnoArrow(_v1, _v2, Annotation::DRAWING_FINISHED, _color);
	WIN_MANAGER->anotationList.push_back(ano);

	if (WIN_MANAGER->GetTab())
	{
		AnnotationTab2* tab = WIN_MANAGER->GetTab()->getAnnoTab();

		if (tab)
			tab->UpdateAnnoList(WIN_MANAGER->mainTabType);
	}

	WIN_MANAGER->setSaveState(false);

	WIN_MANAGER->renderLater_GridView();
}

bool ActionAnnoArrowAdd::mergeWith(const QUndoCommand * command)
{
	return false;
}

ActionAnnoArrowMove::ActionAnnoArrowMove(int annoIndex, AnnoArrow* pOrigin, AnnoArrow* pNew, QUndoCommand * parent)
{
	static int s_id = ACT_ID_ANNO_ARROW_MOVE;

	m_index = annoIndex;
	m_id = s_id++;

	m_pOrigin = std::make_unique<AnnoArrow>(*pOrigin);
	m_pNew = std::make_unique<AnnoArrow>(*pNew);
}

ActionAnnoArrowMove::~ActionAnnoArrowMove()
{
}

void ActionAnnoArrowMove::undo()
{
	if (m_index >= WIN_MANAGER->anotationList.size())
	{
		return;
	}

	AnnoArrow * anno = static_cast<AnnoArrow*>(WIN_MANAGER->anotationList[m_index]);
	if (anno)
	{
		anno->setV1(m_pOrigin->getV1());
		anno->setV2(m_pOrigin->getV2());
		anno->setColor(m_pOrigin->getColor());
	}

	if (WIN_MANAGER->GetTab())
	{
		AnnotationTab2* tab = WIN_MANAGER->GetTab()->getAnnoTab();

		if (tab)
			tab->UpdateAnnoList(WIN_MANAGER->mainTabType);
	}

	WIN_MANAGER->setSaveState(false);

	WIN_MANAGER->renderLater_GridView();
}

void ActionAnnoArrowMove::redo()
{
	if (m_index >= WIN_MANAGER->anotationList.size())
	{
		return;
	}

	AnnoArrow* anno = static_cast<AnnoArrow*>(WIN_MANAGER->anotationList[m_index]);
	if (anno)
	{
		anno->setV1(m_pNew->getV1());
		anno->setV2(m_pNew->getV2());
		anno->setColor(m_pNew->getColor());
	}

	if (WIN_MANAGER->GetTab())
	{
		AnnotationTab2* tab = WIN_MANAGER->GetTab()->getAnnoTab();

		if (tab)
			tab->UpdateAnnoList(WIN_MANAGER->mainTabType);
	}

	WIN_MANAGER->setSaveState(false);

	WIN_MANAGER->renderLater_GridView();
}

bool ActionAnnoArrowMove::mergeWith(const QUndoCommand * command)
{
	return false;
}

ActionAnnoArrowEdit::ActionAnnoArrowEdit(int annoIndex, QColor _col)
{
	static int s_id = ACT_ID_ANNO_ARROW_EDIT;

	m_id = s_id++;

	m_index = annoIndex;

	m_col = COLOR(_col.red(), _col.green(), _col.blue());

	m_preCol = WIN_MANAGER->anotationList.at(m_index)->getColor();
}

ActionAnnoArrowEdit::~ActionAnnoArrowEdit()
{
}

void ActionAnnoArrowEdit::undo()
{
	WIN_MANAGER->anotationList[m_index]->setColor(m_preCol);

	if (WIN_MANAGER->GetTab())
	{
		AnnotationTab2* tab = WIN_MANAGER->GetTab()->getAnnoTab();

		if (tab)
			tab->UpdateAnnoList(WIN_MANAGER->mainTabType);
	}

	WIN_MANAGER->setSaveState(false);

	WIN_MANAGER->renderLater_GridView();
}

void ActionAnnoArrowEdit::redo()
{
	WIN_MANAGER->anotationList[m_index]->setColor(m_col);

	if (WIN_MANAGER->GetTab())
	{
		AnnotationTab2* tab = WIN_MANAGER->GetTab()->getAnnoTab();

		if (tab)
			tab->UpdateAnnoList(WIN_MANAGER->mainTabType);
	}

	WIN_MANAGER->setSaveState(false);

	WIN_MANAGER->renderLater_GridView();
}

bool ActionAnnoArrowEdit::mergeWith(const QUndoCommand * command)
{
	return false;
}
