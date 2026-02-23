#include "stdafx.h"
#include "Interpolation3dDlg.h"
#include "Actions/ActionManager.h"
#include "System/stringManager.h"
#include "System/styleManager.h"

Interpolation3dDlg::Interpolation3dDlg(QWidget* parent)
	: QDialog(parent)
{
	setWindowTitle(tr("3D Interpolation"));
	this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
	setStyleSheet(STYLE_MANAGER->mainFrame);

	//resize(200, 200);

	QVBoxLayout * layout = new QVBoxLayout(this);
	setLayout(layout);

	QHBoxLayout * layH1= new QHBoxLayout(this);		
	layout->addLayout(layH1, 3);

	QLabel* pLabelName = new QLabel("Direction :");
	pLabelName->setStyleSheet(STYLE_MANAGER->labelNormal);
	layH1->addWidget(pLabelName);

	m_ComboBox = new QComboBox(this);
	m_ComboBox->setObjectName(QString("InterpolDirection"));
	m_ComboBox->addItem(QString("AXIAL"));
	m_ComboBox->addItem(QString("SAGITTAL"));
	m_ComboBox->addItem(QString("CORONAL"));
	m_ComboBox->setStyleSheet(STYLE_MANAGER->comboBoxNormal);
	m_ComboBox->setFixedHeight(25);
	layH1->addWidget(m_ComboBox);

	QHBoxLayout * layH2 = new QHBoxLayout(this);
	layout->addLayout(layH2, 1);

	QPushButton* pBtnOk = new QPushButton("OK");
	pBtnOk->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	pBtnOk->setStyleSheet(STYLE_MANAGER->buttonNormal);
	connect(pBtnOk, &QPushButton::clicked, this, &Interpolation3dDlg::OnOk);
	layH2->addWidget(pBtnOk);

	QPushButton* pBtnCancel = new QPushButton("Cancel");
	pBtnCancel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	pBtnCancel->setStyleSheet(STYLE_MANAGER->buttonNormal);
	connect(pBtnCancel, &QPushButton::clicked, this, &Interpolation3dDlg::OnCancel);
	layH2->addWidget(pBtnCancel);
	
}

Interpolation3dDlg::~Interpolation3dDlg()
{

}

//1 = Axial, 2 = Coronal, 3 = Sagittal
void Interpolation3dDlg::setInterPolDirection(int direction)
{
	m_n3dInterpolDir = direction;
}

//1 = Axial, 2 = Coronal, 3 = Sagittal
int Interpolation3dDlg::getInterPolDirection(void)
{
	return m_n3dInterpolDir;
}

void Interpolation3dDlg::updateInterPolationDirectionByComboBox()
{
	QString name = m_ComboBox->currentText();

	if (name == QString("AXIAL"))
	{
		setInterPolDirection(1);
	}
	else if (name == QString("CORONAL"))
	{
		setInterPolDirection(2);
	}
	else if (name == QString("SAGITTAL"))
	{
		setInterPolDirection(3);
	}
}

void Interpolation3dDlg::OnComboChanged(int index)
{
	updateInterPolationDirectionByComboBox();
}

void Interpolation3dDlg::OnOk(void)
{
	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	updateInterPolationDirectionByComboBox();

	ACTION_MANAGER->action_MaskList_3D_Interpolation(m_n3dInterpolDir);

	done(QDialog::Accepted);
}

void Interpolation3dDlg::OnCancel(void)
{
	done(QDialog::Rejected);
}