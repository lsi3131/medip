#include "stdafx.h"
#include "ActionAnnoLen.h"
#include "Windows/windowManager.h"
#include "Windows/Tabwindow.h"

ActionAnnoLenAdd::ActionAnnoLenAdd(AnnoLength* pAnnoLength, QUndoCommand *parent /* = NULL */)
{
	static int s_id = ACT_ID_ANNO_LEN_ADD;

	_v1 = pAnnoLength->getV1();
	_v2 = pAnnoLength->getV2();
	_color = pAnnoLength->getColor();
	
	m_id = s_id++;
}

void ActionAnnoLenAdd::undo()
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

void ActionAnnoLenAdd::redo()
{
	m_index = WIN_MANAGER->anotationList.size();

	AnnoLength *ano = new AnnoLength(_v1, _v2, Annotation::DRAWING_FINISHED, _color);
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

bool ActionAnnoLenAdd::mergeWith(const QUndoCommand * command)
{
	return false;
}

ActionAnnoLenMove::ActionAnnoLenMove(int annoIndex, AnnoLength* pOrigin, AnnoLength* pNew)
{
	static int s_id = ACT_ID_ANNO_LEN_MOVE;

	m_index = annoIndex;
	m_id = s_id++;

	m_pOrigin = std::make_unique<AnnoLength>(*pOrigin);
	m_pNew = std::make_unique<AnnoLength>(*pNew);
}

ActionAnnoLenMove::~ActionAnnoLenMove()
{
}

void ActionAnnoLenMove::undo()
{
	if (m_index >= WIN_MANAGER->anotationList.size())
	{
		return;
	}

	AnnoLength * anno = static_cast<AnnoLength*>(WIN_MANAGER->anotationList[m_index]);
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

void ActionAnnoLenMove::redo()
{
	if (m_index >= WIN_MANAGER->anotationList.size())
	{
		return;
	}

	AnnoLength* anno = static_cast<AnnoLength*>(WIN_MANAGER->anotationList[m_index]);
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

bool ActionAnnoLenMove::mergeWith(const QUndoCommand * command)
{
	return false;
}

ActionAnnoLenEdit::ActionAnnoLenEdit(int annoIndex, QColor _col)
{
	static int s_id = ACT_ID_ANNO_LEN_EDIT;

	m_id = s_id++;

	m_index = annoIndex;
	
	m_col = COLOR(_col.red(), _col.green(), _col.blue());

	m_preCol = WIN_MANAGER->anotationList.at(m_index)->getColor();
}

ActionAnnoLenEdit::~ActionAnnoLenEdit()
{
}

void ActionAnnoLenEdit::undo()
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

void ActionAnnoLenEdit::redo()
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

bool ActionAnnoLenEdit::mergeWith(const QUndoCommand * command)
{
	return false;
}
