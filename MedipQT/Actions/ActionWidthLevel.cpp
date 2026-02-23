#include "stdafx.h"
#include "ActionWidthLevel.h"
#include "Windows/WindowBase.h"
#include "Windows/windowManager.h"
#include "Windows/Main/MainSegmentWidget.h"

ActionWidth::ActionWidth(int width)
{
	m_width = width;
}

void ActionWidth::redo()
{
	WIN_MANAGER->setWindowWidth(m_width);
	WIN_MANAGER->setAIRange(false);

	WIN_MANAGER->mainSegmentWidget->SetWindowWidthLine();
	if (WIN_MANAGER->mainTabType == MAINTAB_SEGMENTATION ||
		WIN_MANAGER->mainTabType == MAINTAB_TA)
	{
		WIN_MANAGER->forceUpdate2DViewData(true, !WIN_MANAGER->getEdgeMode());
		WIN_MANAGER->renderLater_GridView(WIN_MANAGER->getClip2DPlane());
		WIN_MANAGER->renderLater_SubView();
	}
	else
	{
		WIN_MANAGER->forceUpdate2DViewData();
		WIN_MANAGER->renderLater_All();
	}

}


ActionLevel::ActionLevel(int level)
{
	m_level = level;
}

void ActionLevel::redo()
{
	WIN_MANAGER->setWindowLevel(m_level);
	WIN_MANAGER->mainSegmentWidget->SetWindowWidthLine();
	WIN_MANAGER->setAIRange(false);

#ifdef LEVEL_LINE
	WIN_MANAGER->mainSegmentWidget->SetWindowLevelLine();
#endif
	if (WIN_MANAGER->mainTabType == MAINTAB_SEGMENTATION ||
		WIN_MANAGER->mainTabType == MAINTAB_TA)
	{
		WIN_MANAGER->forceUpdate2DViewData(true, !WIN_MANAGER->getEdgeMode());
		WIN_MANAGER->renderLater_GridView(WIN_MANAGER->getClip2DPlane());
		WIN_MANAGER->renderLater_SubView();
	}
	else
	{
		WIN_MANAGER->forceUpdate2DViewData();
		WIN_MANAGER->renderLater_All();
	}

}


ActionVolumeWidth::ActionVolumeWidth(int width)
{
	m_width = width;
}

void ActionVolumeWidth::redo()
{
	WIN_MANAGER->setVolumeWidth(m_width);
	WIN_MANAGER->mainSegmentWidget->SetVolumeWidthLine();
	WIN_MANAGER->renderLater_3DView();
//	WIN_MANAGER->renderLater_SubView();
}


ActionVolumeLevel::ActionVolumeLevel(int level)
{
	m_level = level;
}

void ActionVolumeLevel::redo()
{
	WIN_MANAGER->setVolumeLevel(m_level);
	WIN_MANAGER->mainSegmentWidget->SetVolumeWidthLine();
	WIN_MANAGER->renderLater_3DView();
//	WIN_MANAGER->renderLater_SubView();
}


ActionWidthLevel::ActionWidthLevel(VOLUME_DATA* pVolumeData, int prelevel, int level, int prewidth, int width, float prescalescope, float scalescope)
{
	if (pVolumeData->getHuMin() > level)
		level = pVolumeData->getHuMin();
	if (pVolumeData->getHuMax() < level)
		level = pVolumeData->getHuMax();

	m_level = level;
	
	
	if (0.0f >= width)
		width = 1.0f;
	if (abs(pVolumeData->getHuMax() - pVolumeData->getHuMin()) < width)
		width = pVolumeData->getHuMax() - pVolumeData->getHuMin();

	m_width = width;

	m_scalescope = scalescope;
}

void ActionWidthLevel::redo()
{
	WIN_MANAGER->setAutoScaleSlope(m_scalescope);

	WIN_MANAGER->setWindowLevel(m_level);
	WIN_MANAGER->setWindowWidth(m_width);
	WIN_MANAGER->setAIRange(false);

	WIN_MANAGER->mainSegmentWidget->SetWindowWidthLine();
#ifdef LEVEL_LINE
	WIN_MANAGER->mainSegmentWidget->SetWindowLevelLine();
#endif

	WIN_MANAGER->forceUpdate2DViewData(true, !WIN_MANAGER->getEdgeMode());
	WIN_MANAGER->renderLater_SubView();

	WIN_MANAGER->renderLater_All();
}


ActionPreset::ActionPreset(int prelevel, int prewidth, SLICE_PRESET prePreset, SLICE_PRESET preset, int cusPre)
{
	m_preset = preset;
	m_cusPre = cusPre;
}

void ActionPreset::redo()
{
	WIN_MANAGER->setPreset(m_preset, false, m_cusPre);

	m_level = WIN_MANAGER->getWindowLevel();
	m_width = WIN_MANAGER->getWindowWidth();

	WIN_MANAGER->setAIRange(false);
	WIN_MANAGER->mainSegmentWidget->SetWindowWidthLine(true);
#ifdef LEVEL_LINE
	WIN_MANAGER->mainSegmentWidget->SetWindowLevelLine();
#endif

	WIN_MANAGER->forceUpdate2DViewData(true, !WIN_MANAGER->getEdgeMode());
	WIN_MANAGER->renderLater_SubView();
	WIN_MANAGER->renderLater_All();
}


ActionVolumePreset::ActionVolumePreset(int prelevel, int prewidth, SLICE_PRESET prePreset, SLICE_PRESET preset, int cusPre)
{
	m_preset = preset;
	m_cusPre = cusPre;
}

void ActionVolumePreset::redo()
{
//	m_prePreset = WIN_MANAGER->mainSegmentWidget->getSelectedVolumePreset();
	WIN_MANAGER->setVolumePreset(m_preset, false, m_cusPre);

	m_level = WIN_MANAGER->getVolumeLevel();
	m_width = WIN_MANAGER->getVolumeWidth();
	WIN_MANAGER->mainSegmentWidget->SetVolumeWidthLine(true);
	WIN_MANAGER->renderLater_3DView();
//	WIN_MANAGER->renderLater_SubView();
}
