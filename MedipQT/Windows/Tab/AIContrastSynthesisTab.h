#pragma once

#ifndef AI_CONTRAST_SYNTHESISTAB_H
#define AI_CONTRAST_SYNTHESISTAB_H

#include "CollapseWidget.h"

class AIContrastSynthesisTab : public CollapseWidget
{
	Q_OBJECT

public:
	explicit AIContrastSynthesisTab(QWidget* parent = NULL);

protected:

private slots:
	void slot_Predict();

private:
	QGroupBox* m_groupOption;
	QRadioButton* m_rdoAdult;
	QRadioButton* m_rdoPediatric;
	QRadioButton* m_rdoNonContrastEnhancement;
	QPushButton* m_btnPredict;
};
#endif