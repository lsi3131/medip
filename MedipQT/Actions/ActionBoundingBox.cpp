#include "stdafx.h"
#include "ActionBoundingBox.h"
#include "Windows/WindowBase.h"
#include "Windows/windowManager.h"
#include "DataContext.h"

ActionBoundingBox::ActionBoundingBox(const BoundingBoxI & box, QUndoCommand *parent)
	: QUndoCommand(parent)
{
	static int s_id = ACT_ID_BOUNDINGBOX_M;

	m_pre_box = DATA_CONTEXT->volume_data.getPreBoundingBox();
	m_post_box = box;
	m_id = s_id++;
}

bool ActionBoundingBox::mergeWith(const QUndoCommand *command)
{
	return false;
}

void ActionBoundingBox::undo()
{
	DATA_CONTEXT->volume_data.setBoundingBox(m_pre_box);
	WIN_MANAGER->setAIDepth(m_pre_box);
	
	WIN_MANAGER->forceUpdate2DViewData(true, true);
	WIN_MANAGER->setSaveState(false);

	WIN_MANAGER->setPatchyPoint(true);
	WIN_MANAGER->renderLater_GridView(true);
}

void ActionBoundingBox::redo()
{
	DATA_CONTEXT->volume_data.setBoundingBox(m_post_box);
	WIN_MANAGER->setAIDepth(m_post_box);

	WIN_MANAGER->forceUpdate2DViewData(true, true);
	WIN_MANAGER->setSaveState(false);

	WIN_MANAGER->setPatchyPoint(true);
	WIN_MANAGER->renderLater_GridView(true);
}
