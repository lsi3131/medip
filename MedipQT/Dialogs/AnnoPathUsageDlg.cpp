#include "stdafx.h"
#include "Dialogs/AnnoPathUsageDlg.h"
#include "styleManager.h"

AnnoPathUsageDlg::AnnoPathUsageDlg(QWidget *parent /*= NULL*/)
	: QDialog(parent)
{
	setWindowTitle(tr("Usage : Annotation Path Creation"));
	this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

	setContentsMargins(0, 0, 0, 0);
	resize(160, 180);

	QVBoxLayout * layout = new QVBoxLayout(this);

	QLabel *labelClick = new QLabel(this);
	labelClick->setText("<strong>Left click:</strong> <br/>Create an annotation path point.<br/><br/>"
		"<strong>Right click :</strong><br/>Delete all existing annotation path points.<br/><br/><br/>");
	layout->addWidget(labelClick);

	QLabel *labelShow = new QLabel(this);
	labelShow->setText("*You can set annotation path point show / hide <br/>"
		"through the popup menu without clicking the button. *<br/>");
	layout->addWidget(labelShow);

	QCheckBox *showCheck = new QCheckBox(this);
	showCheck->setText("DO NOT Show Again");
	showCheck->setStyleSheet("font: bold");
	showCheck->setCheckable(true);
	showCheck->setChecked(false);
	showCheck->setStyleSheet(STYLE_MANAGER->m_Checkbox);
	layout->addWidget(showCheck);

	connect(showCheck, &QCheckBox::clicked, this, &AnnoPathUsageDlg::OnChecked);
	this->isShow = false;
	setLayout(layout);
}

AnnoPathUsageDlg::~AnnoPathUsageDlg()
{
	
}

void AnnoPathUsageDlg::OnChecked(bool check)
{
	this->isShow = check;
}
