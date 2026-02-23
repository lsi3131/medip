#include "stdafx.h"
#include "LevelsetDialog.h"

#include "System/styleManager.h"
#include "System/stringManager.h"

LevelsetDialog::LevelsetDialog(QWidget *parent)
	:QDialog(parent)
{
	setWindowTitle(STRING_MANAGER->getString(STR_LEVEL_SET));
	resize(180, 150);
	setMaximumSize(QSize(180, 150));
	setMinimumSize(QSize(180, 150));

	QVBoxLayout *layout = new QVBoxLayout(this);
	
	QWidget *widgetArea = new QWidget(this);
	widgetArea->setFixedSize(200, 200);
	layout->addWidget(widgetArea);

	QLabel *labelIter = new QLabel(widgetArea);
	labelIter->setGeometry(QRect(5, 7, 20, 20));
	labelIter->setText("Iter");

	lineIter = new QLineEdit(widgetArea);
	lineIter->setGeometry(QRect(80, 5, 60, 25));
	lineIter->setStyleSheet(STYLE_MANAGER->editBoxNormal);
	lineIter->setValidator(new QIntValidator(this));

	QLabel *labellam = new QLabel(widgetArea);
	labellam->setGeometry(QRect(5, 42, 50, 20));
	labellam->setText("Lambda");

	lineLam = new QLineEdit(widgetArea);
	lineLam->setGeometry(QRect(80, 40, 60, 25));
	lineLam->setStyleSheet(STYLE_MANAGER->editBoxNormal);
	lineLam->setValidator(new QDoubleValidator(this));

	QLabel *labelradi = new QLabel(widgetArea);
	labelradi->setGeometry(QRect(5,77,60,20));
	labelradi->setText("Radius");

	lineRadious = new QLineEdit(widgetArea);
	lineRadious->setGeometry(QRect(80, 75, 60, 25));
	lineRadious->setStyleSheet(STYLE_MANAGER->editBoxNormal);
	lineRadious->setValidator(new QIntValidator(this));

	isOK = false;

	QPushButton *OKbtn = new QPushButton(this);
	OKbtn->setGeometry(QRect(this->width() - 10, this->height() - 10, 30, 30));
	OKbtn->setStyleSheet(STYLE_MANAGER->buttonNormal);
	OKbtn->setText(STRING_MANAGER->getString(STR_OK));
	connect(OKbtn, &QPushButton::clicked, this, &LevelsetDialog::OnOK);
	layout->addWidget(OKbtn);

}

void LevelsetDialog::OnOK()
{
	bool tmpOK = false;
	isOK = true;

	iterVal = lineIter->text().toInt(&tmpOK);

	if (!tmpOK)
		isOK = false;

	lamVal = lineLam->text().toDouble(&tmpOK);

	if (!tmpOK)
		isOK = false;

	radiVal = lineRadious->text().toInt(&tmpOK);

	if (!tmpOK)
		isOK = false;

	close();
}

LevelsetDialog::~LevelsetDialog()
{
}

int LevelsetDialog::getIter()
{
	if (isOK)
		return iterVal;
	else
		return -1;
}

double LevelsetDialog::getLambda()
{
	if (isOK)
		return lamVal;
	else
		return 0.0;
}

int LevelsetDialog::getRadious()
{
	if (isOK)
		return radiVal;
	else
		return 0;
}
