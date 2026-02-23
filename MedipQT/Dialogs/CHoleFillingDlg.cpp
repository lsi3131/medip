#include "stdafx.h"
#include "CHoleFillingDlg.h"
#include "windowManager.h"
#include "Main/MainMeshWidget.h"
#include "MEVolumeView.h"

#include "System/stringManager.h"
#include "System/styleManager.h"

#include "Actions/ActionManager.h"
#include "ActionMesh.h"

#include "MeshEdit/CMeshWorkManager.h"
#include "MeshEdit/CMeshViewBtn3DScene.h"
#include "MeshEdit/CMeshHoleFillManager.h"

#include "DataContext.h"
#include <qpushbutton.h>

CHoleFillingDlg::CHoleFillingDlg(DataContext* pDataContext, QWidget *parent)
	: MeshBaseDlg(pDataContext, parent)
{
	setWindowTitle(QString("Hole Filling"));
	initQtUI();

	if (m_pDataContext)
	{
		int pck_idx = m_pDataContext->m_MeshData.GetCurrentMeshIndex();
		MESH_HOLE_MANAGER->setMeshIdx(pck_idx);
	}
}

/*
@brief
*/
CHoleFillingDlg::CHoleFillingDlg(DataContext* pDataContext, QString _title, QWidget *parent)
	: MeshBaseDlg(pDataContext, _title, parent)
{
	initQtUI();

	int pck_idx = m_pDataContext->m_MeshData.GetCurrentMeshIndex();
	MESH_HOLE_MANAGER->setMeshIdx(pck_idx);
}

/*
@brief
*/
CHoleFillingDlg::~CHoleFillingDlg()
{
	MESH_HOLE_MANAGER->setMeshIdx(-1);
}

/*
@brief
@return
*/
void CHoleFillingDlg::reject(bool bForce)
{
	if (!m_pDataContext)
	{
		return;
	}

	auto mode = MESH_WORK_MANAGER->getWorkMode();

	if (WIN_MANAGER->mainTabType != MAINTAB_MESH_EDITING || mode != MESH_WORK_PLANE_CUT || mode != MESH_WORK_HOLE_FILL_SELECTED)
	{
		setVisible(false);

		ACTION_MANAGER->action_UndoRedo_update(m_pDataContext, MESH_WORK_NONE, MESH_WORK_HOLE_FILL_SELECTED);

		MESH_WORK_MANAGER->UpdateWorkMode(MESH_WORK_NONE, true);

		auto hole_btn = MESH_BTN_SCENE_MANAGER->m_BtnFillHole;
		hole_btn->setChecked(false);

		MESH_HOLE_MANAGER->setMeshIdx(-1);

		close();
	}
}

/*
@brief
@return
*/
void	CHoleFillingDlg::initQtUI()
{
	QWidget* container = new QWidget(this);

	QVBoxLayout *layoutV = new QVBoxLayout;
	QHBoxLayout *layoutH_Top = new QHBoxLayout(container);
	//QHBoxLayout *layoutH_Top2 = new QHBoxLayout;
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
	//layoutV->addLayout(layoutH_Top2);
	layoutV->addLayout(layoutH_0);
	layoutV->addLayout(layoutH_1);
	layoutV->addLayout(layoutH_2);
	layoutV->addLayout(layoutH_3);
	layoutV->addLayout(layoutH_4);
	layoutV->addLayout(layoutH_5);
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
	QLabel *lb_H_0 = new QLabel(QString("Hole-Filling"), this);
	layoutH_0->addWidget(lb_H_0);
	lb_H_0->setStyleSheet("background-color: #414141;");

	m_p_CheckBoxUse = new QCheckBox(QString("use-fill"), this);
	m_p_CheckBoxUse->setChecked(true);
	m_p_CheckBoxUse->setStyleSheet(STYLE_MANAGER->m_Checkbox);
	layoutH_1->addWidget(m_p_CheckBoxUse);

	m_p_CheckBoxBasic = new QCheckBox(QString("basic-fill"), this);
	m_p_CheckBoxBasic->setChecked(true);
	m_p_CheckBoxBasic->setEnabled(false);
	m_p_CheckBoxBasic->setStyleSheet(STYLE_MANAGER->m_Checkbox);
	layoutH_2->addWidget(m_p_CheckBoxBasic);

	m_p_CheckBoxRemesh = new QCheckBox(QString("remesh-fill"), this);
	m_p_CheckBoxRemesh->setChecked(true);
	m_p_CheckBoxRemesh->setStyleSheet(STYLE_MANAGER->m_Checkbox);
	layoutH_3->addWidget(m_p_CheckBoxRemesh);

	m_p_CheckBoxSmooth = new QCheckBox(QString("smooth-fill"), this);
	m_p_CheckBoxSmooth->setChecked(true);
	m_p_CheckBoxSmooth->setStyleSheet(STYLE_MANAGER->m_Checkbox);
	layoutH_4->addWidget(m_p_CheckBoxSmooth);

	m_p_BtnFillHoleAll = new QPushButton(this);
	m_p_BtnFillHoleAll->setText("Fill all hole ");
	m_p_BtnFillHoleAll->setFixedSize(180, 30);
	m_p_BtnFillHoleAll->setVisible(true);
	m_p_BtnFillHoleAll->setStyleSheet(STYLE_MANAGER->buttonNormal);
	layoutH_5->addWidget(m_p_BtnFillHoleAll);

	QPushButton *btnCancel = new QPushButton(this);
	btnCancel->setText(STRING_MANAGER->getString(STR_CANCEL));
	btnCancel->setFixedSize(180, 30);
	btnCancel->setVisible(true);
	btnCancel->setStyleSheet(STYLE_MANAGER->buttonNormal);
	layoutH_bottom->addWidget(btnCancel);

	connect(btnCancel, &QPushButton::clicked, this, &CHoleFillingDlg::OnCancel);
	connect(m_p_BtnFillHoleAll, &QPushButton::clicked, this, &CHoleFillingDlg::OnFillHoleAll);

	setLayout(layoutV);
}

void CHoleFillingDlg::OnCancel()
{
	if (ACTION_MANAGER->isActionFinished())
	{
		MESH_WORK_MANAGER->UpdateWorkMode(MESH_WORK_NONE, true);

		auto hole_btn = MESH_BTN_SCENE_MANAGER->m_BtnFillHole;
		hole_btn->setChecked(false);

		close();
	}
}

void CHoleFillingDlg::OnFillHoleAll()
{
	if (ACTION_MANAGER->isActionFinished())
	{
		MESH_HOLE_MANAGER->FillHoleAll();
	}
}

bool CHoleFillingDlg::isCheckedUse()
{
	return m_p_CheckBoxUse->isChecked();
}

bool CHoleFillingDlg::isCheckedBasic()
{
	return m_p_CheckBoxBasic->isChecked();
}

bool CHoleFillingDlg::isCheckedRemesh()
{
	return m_p_CheckBoxRemesh->isChecked();
}

bool CHoleFillingDlg::isCheckedSmooth()
{
	return m_p_CheckBoxSmooth->isChecked();
}