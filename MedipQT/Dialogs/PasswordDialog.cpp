#include "stdafx.h"
#include "PasswordDialog.h"
#include "System/styleManager.h"
#include "System/stringManager.h"

PasswordDialog::PasswordDialog(QWidget *parent)
	:QDialog(parent)
{
	setWindowTitle(STRING_MANAGER->getString(STR_PASSWORD));
	resize(180, 50);
	setMaximumSize(QSize(180, 100));
	setMinimumSize(QSize(180, 100));

	QVBoxLayout *layout = new QVBoxLayout(this);

	QWidget *widgetArea = new QWidget(this);
	widgetArea->setFixedSize(200, 200);
	layout->addWidget(widgetArea);

	QLabel *labelIter = new QLabel(widgetArea);
	labelIter->setGeometry(QRect(5, 7, 70, 20));
	labelIter->setText("Password");

	lineIter = new QLineEdit(widgetArea);
	lineIter->setGeometry(QRect(80, 5, 70, 25));
	lineIter->setStyleSheet(STYLE_MANAGER->editBoxNormal);
//	lineIter->setValidator(new QIntValidator(this));
	lineIter->setEchoMode(QLineEdit::Password);

	isOK = false;

	QPushButton *OKbtn = new QPushButton(this);
	OKbtn->setGeometry(QRect(this->width() - 10, this->height() - 10, 30, 30));
	OKbtn->setStyleSheet(STYLE_MANAGER->buttonNormal);
	OKbtn->setText(STRING_MANAGER->getString(STR_OK));
	connect(OKbtn, &QPushButton::clicked, this, &PasswordDialog::OnOK);
	layout->addWidget(OKbtn);

}

void PasswordDialog::OnOK()
{
	
	isOK = true;

	m_password = lineIter->text();
		

	close();
}

PasswordDialog::~PasswordDialog()
{
}

QString PasswordDialog::getPass()
{
	if (isOK)
		return m_password;
	else
		return -1;
}