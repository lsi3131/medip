#pragma once

#ifndef IMGPROCESS_H
#define IMGPROCESS_H

#include "defineMEDIP.h"
#include "UI/CollapseWidget.h"
#include <QPushButton>

class IMGProcessTab : public CollapseWidget
{
	Q_OBJECT

public:
	explicit IMGProcessTab(QWidget* parent = NULL);

private:
	QPushButton* m_btnEnhance[5];
	QPushButton* m_btnGaussian[2];
	QPushButton* m_btnLaplacian[2];

private slots:
	void	slot_OnEnhance();
	void	slot_OnReset();
	void	slot_OnGaussian();
	void	slot_OnLaplacian();

};
#endif