#pragma once

#ifndef VOLPROCESSTAB_H
#define VOLPROCESSTAB_H

#include "CollapseWidget.h"
#include <QPushButton>

class VOLProcessTab : public CollapseWidget
{
	Q_OBJECT

public:
	explicit VOLProcessTab(QWidget* parent = NULL);

	void	setFlipmode();

private:
	QPushButton* m_btnZFlip;
	QPushButton* m_btnYFlip;
	QPushButton* m_btnXFlip;

private slots:
	void	slot_OnIsolation();
	void	slot_OnXFlip();
	void	slot_OnYFlip();
	void	slot_OnZFlip();
	void	slot_OnScale();
	void	slot_OnVolumeCrop();
};
#endif
