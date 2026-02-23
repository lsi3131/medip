#include "stdafx.h"
#include "AIContrastSynthesisTab.h"
#include "System/styleManager.h"
#include "System/resourceManager.h"
#include "System/stringManager.h"
#include "System/ShortcutManager.h"
#include "ActionManager.h"

AIContrastSynthesisTab::AIContrastSynthesisTab(QWidget* parent)
	: CollapseWidget(QString(), parent)
{
	int row = 0;
	m_groupOption = new QGroupBox("Option", this);
	QLayout* layout = new QHBoxLayout();
	m_groupOption->setLayout(layout);

	m_rdoAdult = new QRadioButton("Adult");
	m_rdoPediatric = new QRadioButton("Pediatric");
	m_rdoNonContrastEnhancement = new QRadioButton("Non-Contrast Enhancement");
	m_btnPredict = new QPushButton("Predict", this);
	m_btnPredict->setStyleSheet(STYLE_MANAGER->buttonBehind);

	layout->addWidget(m_rdoAdult);
	layout->addWidget(m_rdoPediatric);
	layout->addWidget(m_rdoNonContrastEnhancement);

	m_rdoAdult->setChecked(true);

	connect(m_btnPredict, &QPushButton::clicked, this, &AIContrastSynthesisTab::slot_Predict);

	addWidget(m_groupOption, row++, 1);
	addWidget(m_btnPredict, row++, 1);

	QWidget* emptyBox0 = new QWidget(this);
	emptyBox0->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
	addWidget(emptyBox0, row++, 1);

	QString title = QString("AI Contrast Synthesis");
	setTitle(title);

	setOpenWidget(false);
}

void AIContrastSynthesisTab::slot_Predict()
{
	eAIContrastSynthesisOption option = eAIContrastSynthesisOption::Adult;

	if (m_rdoAdult->isChecked())
	{
		option = eAIContrastSynthesisOption::Adult;
	}
	else if (m_rdoPediatric->isChecked())
	{
		option = eAIContrastSynthesisOption::Pediatric;
	}
	else if (m_rdoNonContrastEnhancement->isChecked())
	{
		option = eAIContrastSynthesisOption::NonContrast;
	}

	ACTION_MANAGER->action_AI_ContrastSynthesis(option);
}

