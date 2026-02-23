#pragma once

#ifndef ACTION_WIDTHLEVEL_H
#define ACTION_WIDTHLEVEL_H

#include "define.h"
#include <QtWidgets>
#include <graphics\BoundingBox.h>
#include "volumedata.h"

class ActionWidth
{
public:
	ActionWidth(int width);

	void redo();
private:
	int	m_width;
};

class ActionLevel 
{
public:
	ActionLevel(int level);

	void redo();
private:
	int	m_level;
};

class ActionVolumeWidth 
{
public:
	ActionVolumeWidth(int width);

	void redo();
private:
	int	m_width;
};

class ActionVolumeLevel 
{
public:
	ActionVolumeLevel(int level);

	void redo();
private:
	int	m_level;
};

class ActionWidthLevel 
{
public:
	ActionWidthLevel(VOLUME_DATA* pVolumeData, int prelevel, int level, int prewidth, int width, float prescalescope, float scalescope);

	void redo();
private:
	int		m_width;
	int		m_level;
	float	m_scalescope;
};

class ActionPreset
{
public:
	ActionPreset(int prelevel, int prewidth, SLICE_PRESET prePreset, SLICE_PRESET preset, int cusPre=-1);

	void redo();
private:
	int				m_width;
	int				m_level;
	SLICE_PRESET	m_preset;
	int				m_cusPre;
};

class ActionVolumePreset
{
public:
	ActionVolumePreset(int prelevel, int prewidth, SLICE_PRESET prePreset, SLICE_PRESET preset, int cusPre = -1);

	void redo();
private:
	int				m_width;
	int				m_level;
	SLICE_PRESET	m_preset;
	int				m_cusPre;
};
#endif