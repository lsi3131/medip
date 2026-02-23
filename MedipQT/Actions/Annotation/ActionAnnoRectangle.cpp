#include "stdafx.h"
#include "ActionAnnoRectangle.h"
#include "Windows/windowManager.h"
#include "Windows/Tabwindow.h"
#include "Windows/AnalMPRPlaneView.h"

//================================================================
//				Add
//================================================================
ActionAnnoRectangleAdd::ActionAnnoRectangleAdd(AnnoRectangle* pAnnoRect, QUndoCommand * parent)
{
	static int s_id = ACT_ID_ANNO_RECTANGLE_ADD;

	m_vLT = pAnnoRect->getVertexLeftTop();
	m_vRT = pAnnoRect->getVertexRightTop();
	m_vLB = pAnnoRect->getVertexLeftBottom();
	m_vRB = pAnnoRect->getVertexRightBottom();

	m_color = pAnnoRect->getColor();

	m_id = s_id++;
}

void ActionAnnoRectangleAdd::undo()
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

void ActionAnnoRectangleAdd::redo()
{
	m_index = WIN_MANAGER->anotationList.size();

	AnnoRectangle *ano = new AnnoRectangle(m_vLT, m_vRT, m_vLB, m_vRB, Annotation::DRAWING_FINISHED, m_color);
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

bool ActionAnnoRectangleAdd::mergeWith(const QUndoCommand * command)
{
	return false;
}

//================================================================
//				Move
//================================================================
ActionAnnoRectangleMove::ActionAnnoRectangleMove(int annoIndex, AnnoRectangle* pOrigin, AnnoRectangle* pNew, QUndoCommand *parent)
{
	static int s_id = ACT_ID_ANNO_RECTANGLE_MOVE;

	m_index = annoIndex;
	m_id = s_id++;

	m_pOrigin = std::make_unique<AnnoRectangle>(*pOrigin);
	m_pNew = std::make_unique<AnnoRectangle>(*pNew);
}

ActionAnnoRectangleMove::~ActionAnnoRectangleMove()
{

}

void ActionAnnoRectangleMove::undo()
{
	if (m_index >= WIN_MANAGER->anotationList.size())
	{
		return;
	}

	AnnoRectangle *anno = static_cast<AnnoRectangle*>(WIN_MANAGER->anotationList[m_index]);
	if (anno)
	{
		anno->setVertex(
			m_pOrigin->getVertexLeftTop(),
			m_pOrigin->getVertexRightTop(),
			m_pOrigin->getVertexLeftBottom(),
			m_pOrigin->getVertexRightBottom()
		);

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

void ActionAnnoRectangleMove::redo()
{
	if (m_index >= WIN_MANAGER->anotationList.size())
	{
		return;
	}

	AnnoRectangle *anno = static_cast<AnnoRectangle*>(WIN_MANAGER->anotationList[m_index]);

	if (anno)
	{
		anno->setVertex(
			m_pNew->getVertexLeftTop(),
			m_pNew->getVertexRightTop(),
			m_pNew->getVertexLeftBottom(),
			m_pNew->getVertexRightBottom()
		);

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

bool ActionAnnoRectangleMove::mergeWith(const QUndoCommand *command)
{
	return false;;
}

