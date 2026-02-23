#include "stdafx.h"
#include "ActionAnnoAngle.h"
#include "Windows/windowManager.h"
#include "Windows/Tabwindow.h"
#include "Annotation/AnnoAngle.h"

ActionAnnoAngleAdd::ActionAnnoAngleAdd(AnnoAngle* pAnnoAngle, QUndoCommand * parent)
{
	static int s_id = ACT_ID_ANNO_ANGLE_ADD;

	_v1 = pAnnoAngle->getV1();
	_v2 = pAnnoAngle->getV2();
	_v3 = pAnnoAngle->getV3();
	_color = pAnnoAngle->getColor();

	m_id = s_id++;
}

void ActionAnnoAngleAdd::undo()
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

void ActionAnnoAngleAdd::redo()
{
	m_index = WIN_MANAGER->anotationList.size();

	AnnoAngle *ano = new AnnoAngle(_v1, _v2, _v3, Annotation::DRAWING_FINISHED, _color);
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

bool ActionAnnoAngleAdd::mergeWith(const QUndoCommand * command)
{
	return false;
}

ActionAnnoAngleMove::ActionAnnoAngleMove(int annoIndex, AnnoAngle* pOrigin, AnnoAngle* pNew, QUndoCommand * parent)
{
	static int s_id = ACT_ID_ANNO_ANGLE_MOVE;

	m_index = annoIndex;
	m_id = s_id++;

	m_pOrigin = std::make_unique<AnnoAngle>(*pOrigin);
	m_pNew = std::make_unique<AnnoAngle>(*pNew);
}

ActionAnnoAngleMove::~ActionAnnoAngleMove()
{
}

void ActionAnnoAngleMove::undo()
{
	if (m_index >= WIN_MANAGER->anotationList.size()) return;

	AnnoAngle* anno = static_cast<AnnoAngle*>(WIN_MANAGER->anotationList[m_index]);
	if (anno)
	{
		anno->setV1(m_pOrigin->getV1());
		anno->setV2(m_pOrigin->getV2());
		anno->setV3(m_pOrigin->getV3());

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

void ActionAnnoAngleMove::redo()
{
	if (m_index >= WIN_MANAGER->anotationList.size()) return;

	AnnoAngle *anno = static_cast<AnnoAngle*>(WIN_MANAGER->anotationList[m_index]);

	if (anno)
	{
		anno->setV1(m_pNew->getV1());
		anno->setV2(m_pNew->getV2());
		anno->setV3(m_pNew->getV3());

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

bool ActionAnnoAngleMove::mergeWith(const QUndoCommand * command)
{
	return false;
}

ActionAnnoAngleEdit::ActionAnnoAngleEdit(int annoIndex, QColor _col)
{
	static int s_id = ACT_ID_ANNO_ANGLE_EDIT;

	m_id = s_id++;

	m_index = annoIndex;

	m_col = COLOR(_col.red(), _col.green(), _col.blue());

	m_preCol = WIN_MANAGER->anotationList.at(m_index)->getColor();
}

ActionAnnoAngleEdit::~ActionAnnoAngleEdit()
{
}

void ActionAnnoAngleEdit::undo()
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

void ActionAnnoAngleEdit::redo()
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

bool ActionAnnoAngleEdit::mergeWith(const QUndoCommand * command)
{
	return false;
}
