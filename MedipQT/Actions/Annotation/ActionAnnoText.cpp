#include "stdafx.h"
#include "ActionAnnoText.h"
#include "Windows/WindowBase.h"
#include "Windows/windowManager.h"
#include "Windows/Tabwindow.h"
#include "Windows/AnalMPRPlaneView.h"

ActionAnnoTextAdd::ActionAnnoTextAdd(AnnoString* pAnnoText, QUndoCommand *parent)
	: QUndoCommand(parent)
{
	static int s_id = ACT_ID_ANNO_TEXT_ADD;

	mip::VECTOR3 volumePos = pAnnoText->getVolumePos();

	m_v = pAnnoText->getVertex();
	m_string = pAnnoText->getText();
	m_size = pAnnoText->getFontSize();
	m_color = pAnnoText->getColor();

	m_id = s_id++;
}

bool ActionAnnoTextAdd::mergeWith(const QUndoCommand *command)
{
	return false;
}

void ActionAnnoTextAdd::undo()
{
	Annotation * ano = WIN_MANAGER->anotationList[m_index];
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

void ActionAnnoTextAdd::redo()
{
	m_index = WIN_MANAGER->anotationList.size();
	AnnoString * ano = new AnnoString(m_v, Annotation::EState::DRAWING_FINISHED, m_string, m_size, m_color);
	WIN_MANAGER->anotationList.push_back(ano);
	
	if(WIN_MANAGER->GetTab())
	{
		AnnotationTab2* tab = WIN_MANAGER->GetTab()->getAnnoTab();

		if (tab)
			tab->UpdateAnnoList(WIN_MANAGER->mainTabType);
	}
	WIN_MANAGER->setSaveState(false);

	WIN_MANAGER->renderLater_GridView();
}


ActionAnnoDel::ActionAnnoDel(WindowBase* view, int annoIndex, QUndoCommand *parent)
	: QUndoCommand(parent)
{
	static int s_id = ACT_ID_ANNO_DEL;

	m_index = annoIndex;

	m_id = s_id++;
	m_anno = NULL;
}

ActionAnnoDel::~ActionAnnoDel()
{
	SAFE_DELETE(m_anno);
}

bool ActionAnnoDel::mergeWith(const QUndoCommand *command)
{
	return false;
}

void ActionAnnoDel::undo()
{
	WIN_MANAGER->anotationList.insert(m_index, m_anno);
	m_anno = NULL;
	if (WIN_MANAGER->GetTab())
	{
		AnnotationTab2* tab = WIN_MANAGER->GetTab()->getAnnoTab();

		if (tab)
			tab->UpdateAnnoList(WIN_MANAGER->mainTabType);
	}
	WIN_MANAGER->setSaveState(false);

	WIN_MANAGER->renderLater_GridView();
}

void ActionAnnoDel::redo()
{
	if (m_index >= WIN_MANAGER->anotationList.size()) return;

	Annotation * ano = WIN_MANAGER->anotationList[m_index];
	m_anno = ano;
	m_type = m_anno->getType();


	WIN_MANAGER->anotationList.remove(m_index);
	if (WIN_MANAGER->GetTab())
	{
		AnnotationTab2* tab = WIN_MANAGER->GetTab()->getAnnoTab();

		if (tab)
			tab->UpdateAnnoList(WIN_MANAGER->mainTabType);
	}
	WIN_MANAGER->setSaveState(false);

	WIN_MANAGER->renderLater_GridView();
}


ActionAnnoTextMove::ActionAnnoTextMove(WindowBase* view, muint32 annoIndex, AnnoString* pOrigin, AnnoString* pNew, QUndoCommand *parent)
	: QUndoCommand(parent)
{
	static int s_id = ACT_ID_ANNO_TEXT_MOVE;

	m_index = annoIndex;
	m_id = s_id++;

	m_pOrigin = std::make_unique<AnnoString>(*pOrigin);
	m_pNew = std::make_unique<AnnoString>(*pNew);
}

bool ActionAnnoTextMove::mergeWith(const QUndoCommand *command)
{
	return false;
}

void ActionAnnoTextMove::undo()
{
	if (m_index >= WIN_MANAGER->anotationList.size()) return;

	AnnoString* anno = (AnnoString*)WIN_MANAGER->anotationList[m_index];
	
	if (anno)
	{
		anno->setVertex(m_pOrigin->getVertex());
		anno->setColor(m_pOrigin->getColor());
		anno->setFontSize(m_pOrigin->getFontSize());
		anno->setText(m_pOrigin->getText());
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

void ActionAnnoTextMove::redo()
{
	if (m_index >= WIN_MANAGER->anotationList.size()) return;

	AnnoString* anno = (AnnoString*)WIN_MANAGER->anotationList[m_index];

	if (anno)
	{
		anno->setVertex(m_pNew->getVertex());
		anno->setColor(m_pNew->getColor());
		anno->setFontSize(m_pNew->getFontSize());
		anno->setText(m_pNew->getText());
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


ActionAnnoTextEdit::ActionAnnoTextEdit(muint32 annoIndex, QString & str, muint16 size, QColor & color, QUndoCommand *parent)
	: QUndoCommand(parent)
{
	static int s_id = ACT_ID_ANNO_TEXT_EDIT;

	m_index = annoIndex;
	m_string = str;
	m_size = size;
	m_color = COLOR(color.red(), color.green(), color.blue());
	
	m_id = s_id++;
}

bool ActionAnnoTextEdit::mergeWith(const QUndoCommand *command)
{
	return false;
}

void ActionAnnoTextEdit::undo()
{
	AnnoString * anno = static_cast<AnnoString *>(WIN_MANAGER->anotationList[m_index]);
	if (anno)
	{
		//anno->m_str = m_preString;
		//anno->m_size = m_preSize;
		//anno->setColor(m_preColor);
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

void ActionAnnoTextEdit::redo()
{
	AnnoString * anno = static_cast<AnnoString *>(WIN_MANAGER->anotationList[m_index]);
	if (anno)
	{
		//m_preString = anno->m_str;
		//m_preSize = anno->m_size;
		//m_preColor = anno->getColor();

		//anno->m_str = m_string;
		//anno->m_size = m_size;
		//anno->setColor(m_color);
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