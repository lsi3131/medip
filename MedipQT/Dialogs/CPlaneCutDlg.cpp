#include "stdafx.h"
#include "CPlaneCutDlg.h"
#include "windowManager.h"
#include "Main/MainMeshWidget.h"
#include "MEVolumeView.h"

#include "System/stringManager.h"
#include "System/styleManager.h"

#include "Actions/ActionManager.h"
#include "ActionMesh.h"

#include "MeshEdit/CMeshWorkManager.h"
#include "MeshEdit/CMeshCutManager.h"


CPlaneCutDlg::CPlaneCutDlg(DataContext* pDataContext, QWidget *parent)
	: MeshBaseDlg(pDataContext, parent)
{
	setWindowTitle(QString("Plane-Cut"));
	initQtUI();
}

/*
@brief
*/
CPlaneCutDlg::CPlaneCutDlg(DataContext* pDataContext, QString _title, QWidget *parent)
	: MeshBaseDlg(pDataContext, _title, parent)
{
	initQtUI();
}

/*
@brief
*/
CPlaneCutDlg::~CPlaneCutDlg()
{

}


/*
@brief
@return
*/
void	CPlaneCutDlg::initQtUI()
{
	QWidget* container = new QWidget(this);

	QVBoxLayout *layoutV = new QVBoxLayout;
	QHBoxLayout *layoutH_Top = new QHBoxLayout;
	QHBoxLayout *layoutH_0 = new QHBoxLayout;
	QHBoxLayout *layoutH_1 = new QHBoxLayout;
	QHBoxLayout *layoutH_2 = new QHBoxLayout;
	QHBoxLayout *layoutH_3 = new QHBoxLayout;
	QHBoxLayout *layoutH_4 = new QHBoxLayout;
	QHBoxLayout *layoutH_5 = new QHBoxLayout;
	QHBoxLayout *layoutH_6 = new QHBoxLayout;

	QHBoxLayout *layoutH_bottom = new QHBoxLayout;
	int le_wdth = 70;

	layoutV->addLayout(layoutH_Top);
	layoutV->addLayout(layoutH_0);
	layoutV->addLayout(layoutH_1);
	layoutV->addLayout(layoutH_2);
	layoutV->addLayout(layoutH_3);
	layoutV->addLayout(layoutH_4);
	layoutV->addLayout(layoutH_bottom);

	container->setStyleSheet("background-color: #414141;");
	QRect rect = container->geometry();
	rect.setWidth(200);
	container->setGeometry(rect);

	// label
	QLabel *label = new QLabel(QString("Plane-Cut"), this);
	label->setStyleSheet("font-weight: bold;");
	label->setAlignment(Qt::AlignLeft);
	layoutH_0->addWidget(label);

	m_p_ComboTypeFilling = new QComboBox(this);
	m_p_ComboTypeFilling->setStyleSheet(STYLE_MANAGER->comboBoxNormal);
	
	m_p_ComboTypeFilling->addItem("Minimal Fill");
	m_p_ComboTypeFilling->addItem("Remeshed Fill");
	m_p_ComboTypeFilling->addItem("No Fill");

	m_p_ComboTypeFilling->setCurrentIndex(1);
	m_p_ComboTypeFilling->setStyleSheet(STYLE_MANAGER->comboBoxNormal);
	layoutH_1->addWidget(m_p_ComboTypeFilling);

	m_p_BtnOk = new QPushButton(this);
	m_p_BtnOk->setText(STRING_MANAGER->getString(STR_OK));
	m_p_BtnOk->setFixedSize(50, 30);
	m_p_BtnOk->setVisible(true);
	m_p_BtnOk->setStyleSheet(STYLE_MANAGER->buttonNormal);
	layoutH_bottom->addWidget(m_p_BtnOk);

	m_p_BtnCancel = new QPushButton(this);
	m_p_BtnCancel->setText(STRING_MANAGER->getString(STR_CANCEL));
	m_p_BtnCancel->setFixedSize(50, 30);
	m_p_BtnCancel->setVisible(true);
	m_p_BtnCancel->setStyleSheet(STYLE_MANAGER->buttonNormal);
	layoutH_bottom->addWidget(m_p_BtnCancel);

	connect(m_p_BtnOk, &QPushButton::clicked, this, &CPlaneCutDlg::OnOK);
	connect(m_p_BtnCancel, &QPushButton::clicked, this, &CPlaneCutDlg::OnCancel);

	setLayout(layoutV);
}

/*
@brief
@return
*/
void CPlaneCutDlg::reject(bool bForce)
{
	auto mode = MESH_WORK_MANAGER->getWorkMode();

	QString _title = windowTitle();
	setWindowOpacity(0.1);
	setVisible(false);

	OnCancel();
}

/*
@brief
@return
*/
void		CPlaneCutDlg::OnOK()
{
	// Plane Cut ผ๖วเ
	MESH_CUT_MANAGER->Process(MESH_WORK_PLANE_CUT);

	// view update

	MESH_WORK_MANAGER->UpdateWorkMode(MESH_WORK_NONE, true);
	close();
}

/*
@brief
@return
*/
void		CPlaneCutDlg::OnCancel()
{
	MESH_WORK_MANAGER->UpdateWorkMode(MESH_WORK_NONE, true);
	close();
}

/*
@brief
@return
*/
int			CPlaneCutDlg::getTypeFilling()
{
	if (!m_p_ComboTypeFilling)
	{
		return -1;
	}

	return m_p_ComboTypeFilling->currentIndex();
}