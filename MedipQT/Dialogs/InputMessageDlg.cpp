#include "stdafx.h"
#include "InputMessageDlg.h"
#include "System/styleManager.h"

InputMessageDlg::InputMessageDlg(QString title, QString inputName, QWidget* parent)
	: QDialog(parent)
{

	//setWindowTitle(tr("Component Choice"));
	setWindowTitle(title);
	this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
	setStyleSheet(STYLE_MANAGER->mainFrame);

	//resize(200, 200);

	QVBoxLayout * layout = new QVBoxLayout(this);
	setLayout(layout);

	QHBoxLayout * layH1= new QHBoxLayout(this);		
	layout->addLayout(layH1, 3);

	//QLabel* pLabelName = new QLabel("Cluster Number :");
	QLabel* pLabelName = new QLabel(inputName);
	pLabelName->setStyleSheet(STYLE_MANAGER->labelNormal);
	layH1->addWidget(pLabelName);

	m_LineEditNumber = new QLineEdit(this);
	m_LineEditNumber->setStyleSheet(STYLE_MANAGER->editBoxNormal);
	layH1->addWidget(m_LineEditNumber);


	QHBoxLayout * layH2 = new QHBoxLayout(this);
	layout->addLayout(layH2, 1);

	QPushButton* pBtnOk = new QPushButton("OK");
	pBtnOk->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	pBtnOk->setStyleSheet(STYLE_MANAGER->buttonNormal);
	connect(pBtnOk, &QPushButton::clicked, this, &InputMessageDlg::OnOk);
	layH2->addWidget(pBtnOk);

	QPushButton* pBtnCancel = new QPushButton("Cancel");
	pBtnCancel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	pBtnCancel->setStyleSheet(STYLE_MANAGER->buttonNormal);
	connect(pBtnCancel, &QPushButton::clicked, this, &InputMessageDlg::OnCancel);
	layH2->addWidget(pBtnCancel);
	
}

InputMessageDlg::~InputMessageDlg()
{

}

int InputMessageDlg::getInputNum(void)
{
	return m_nInputNum;
}

void InputMessageDlg::OnOk(void)
{
	m_nInputNum = m_LineEditNumber->text().toInt();

	done(QDialog::Accepted);
}

void InputMessageDlg::OnCancel(void)
{
	done(QDialog::Rejected);
}
