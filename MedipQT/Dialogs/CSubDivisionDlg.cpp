/*
@company		메디컬아이피
@autor			허 건 과장
@date			2021-10-18
@brief			CSubDivisionDlg 구현파일
*/

#include "stdafx.h"
#include "CSubDivisionDlg.h"
#include "ActionManager.h"
#include "stringManager.h"
#include "styleManager.h"
#include "windowManager.h"
#include "ActionMesh.h"

#include "MeshEdit\CMeshWorkManager.h"

#include "DataContext.h"

/*
@brief
*/
CSubDivisionDlg::CSubDivisionDlg(DataContext* pDataContext, QWidget *parent)
	: MeshBaseDlg(pDataContext, parent)
{
	setWindowTitle(QString("Island filter"));
	initQtUI();
}

/*
@brief
*/
CSubDivisionDlg::CSubDivisionDlg(DataContext* pDataContext, QString _title, QWidget *parent)
	: MeshBaseDlg(pDataContext, _title, parent)
{
	initQtUI();
}

/*
@brief
*/
CSubDivisionDlg::~CSubDivisionDlg()
{

}

/*
@brief
@return
*/
void CSubDivisionDlg::reject(bool bForce)
{
	if (!m_pDataContext)
	{
		return;
	}

	MESH_WORK_MODE mode = MESH_WORK_MANAGER->getWorkMode();

	setVisible(false);

	ACTION_MANAGER->action_UndoRedo_update(m_pDataContext, MESH_WORK_NONE, MESH_WORK_SUBDIVISION);

	MESH_WORK_MANAGER->UpdateWorkMode(MESH_WORK_NONE, true);

	close();
}

/*
@brief
@return
*/
void	CSubDivisionDlg::initQtUI()
{
	QWidget* container = new QWidget(this);

	QVBoxLayout *layoutV = new QVBoxLayout;
	QHBoxLayout *layoutH_Top = new QHBoxLayout(container);
	QHBoxLayout *layoutH_0 = new QHBoxLayout;
	QHBoxLayout *layoutH_1 = new QHBoxLayout;
	QHBoxLayout *layoutH_2 = new QHBoxLayout;
	QHBoxLayout *layoutH_3 = new QHBoxLayout;
	QHBoxLayout *layoutH_4 = new QHBoxLayout;
	QHBoxLayout *layoutH_5 = new QHBoxLayout;
	QHBoxLayout *layoutH_bottom = new QHBoxLayout;
	QVBoxLayout *layoutV_bottom_0 = new QVBoxLayout;
	QVBoxLayout *layoutV_bottom_1 = new QVBoxLayout;
	int le_wdth = 70;

	layoutV->addLayout(layoutH_Top);
	layoutV->addLayout(layoutH_0);
	layoutV->addLayout(layoutH_1);
	layoutV->addLayout(layoutH_2);
	layoutV->addLayout(layoutH_bottom);

	container->setStyleSheet("background-color: #414141;");
	QRect rect = container->geometry();
	rect.setWidth(200);
	container->setGeometry(rect);
	this->setGeometry(rect);

	QLabel *lb_H_Top = new QLabel(QString(""), this);
	lb_H_Top->setStyleSheet("background-color: #414141;");
	layoutH_Top->addWidget(lb_H_Top);

	//
	QLabel *lb_H_0 = new QLabel(QString("SubDivision"), this);
	layoutH_0->addWidget(lb_H_0);
	lb_H_0->setStyleSheet("background-color: #414141;");

	QLabel *label2 = new QLabel(QString("Method"), this);
	label2->setStyleSheet("font-weight: bold;");
	label2->setAlignment(Qt::AlignLeft);
	layoutH_1->addWidget(label2);

	m_p_Combo_Method = new QComboBox(this);
	m_p_Combo_Method->addItem("mid-point");
	m_p_Combo_Method->addItem("L3-Loop");
	m_p_Combo_Method->setCurrentIndex(0);
	m_p_Combo_Method->setFixedHeight(25);	
	m_p_Combo_Method->setStyleSheet(STYLE_MANAGER->comboBoxNormal);
	layoutH_2->addWidget(m_p_Combo_Method);

	QPushButton *btnOK = new QPushButton(this);
	btnOK->setText(QString("Apply"));
	btnOK->setFixedSize(80, 30);
	btnOK->setVisible(true);
	btnOK->setStyleSheet(STYLE_MANAGER->buttonNormal);
	layoutV_bottom_0->addWidget(btnOK, Qt::AlignLeft);
	layoutV_bottom_0->setAlignment(Qt::AlignLeft);
	layoutH_bottom->addLayout(layoutV_bottom_0);

	QPushButton *btnCancel = new QPushButton(this);
	btnCancel->setText(STRING_MANAGER->getString(STR_CANCEL));
	btnCancel->setFixedSize(80, 30);
	btnCancel->setVisible(true);
	btnCancel->setStyleSheet(STYLE_MANAGER->buttonNormal);
	layoutV_bottom_1->addWidget(btnCancel, Qt::AlignRight);
	layoutV_bottom_1->setAlignment(Qt::AlignRight);
	layoutH_bottom->addLayout(layoutV_bottom_1);

	connect(btnOK, &QPushButton::clicked, this, &CSubDivisionDlg::OnOK);
	connect(btnCancel, &QPushButton::clicked, this, &CSubDivisionDlg::OnCancel);

	setLayout(layoutV);
}

/*
@brief
@return
*/
void CSubDivisionDlg::OnOK()
{
	MEVolumeView* pViewer = (MEVolumeView*)(this->parent());

	if (ACTION_MANAGER->isActionFinished() && m_pDataContext && pViewer)
	{
		int MeshIdx = m_pDataContext->m_MeshData.GetCurrentMeshIndex();

		WorkMeshRemesh* pWMR = ACTION_MANAGER->action_Mesh_Init(MESH_SUBDIVISION, MeshIdx, m_pDataContext, pViewer);

		if (pWMR == nullptr)
		{
			return;
		}

		int index = m_p_Combo_Method->currentIndex();

		pWMR->m_Params.method_sd = index;

		ACTION_MANAGER->action_Mesh(pWMR);

		//close();
	}
}

/*
@brief
@return
*/
void CSubDivisionDlg::OnCancel()
{
	if (ACTION_MANAGER->isActionFinished())
	{
		MESH_WORK_MANAGER->UpdateWorkMode(MESH_WORK_NONE, true);

		close();
	}
}