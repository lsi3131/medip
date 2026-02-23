#include "stdafx.h"
#include "CSolidDlg.h"
#include "windowManager.h"
#include "Main/MainMeshWidget.h"
#include "MEVolumeView.h"

#include "System/stringManager.h"
#include "System/styleManager.h"

#include "Actions/ActionManager.h"
#include "ActionMesh.h"

#include "MeshEdit\CMeshWorkManager.h"

#include "DataContext.h"

CSolidDlg::CSolidDlg(DataContext* pDataContext, QWidget *parent)
	: MeshBaseDlg(pDataContext, parent)
{
	setWindowTitle(QString("Solid"));
	initQtUI();
}

/*
@brief
*/
CSolidDlg::CSolidDlg(DataContext* pDataContext, QString _title, QWidget *parent)
	: MeshBaseDlg(pDataContext, _title, parent)
{
	initQtUI();
}

/*
@brief
*/
CSolidDlg::~CSolidDlg()
{

}

/*
@brief
@return
*/
void CSolidDlg::reject(bool bForce)
{
	if (!m_pDataContext)
	{
		return;
	}

	MESH_WORK_MODE mode = MESH_WORK_MANAGER->getWorkMode();

	setVisible(false);

	ACTION_MANAGER->action_UndoRedo_update(m_pDataContext, MESH_WORK_NONE, MESH_WORK_SOLID);

	MESH_WORK_MANAGER->UpdateWorkMode(MESH_WORK_NONE, true);

	close();
}

void CSolidDlg::initQtUI()
{
	QWidget* container = new QWidget(this);

	QVBoxLayout *layoutV = new QVBoxLayout;
	QHBoxLayout *layoutH_Top = new QHBoxLayout(container);
	QHBoxLayout *layoutH_0 = new QHBoxLayout;
	QHBoxLayout *layoutH_1 = new QHBoxLayout;
	QHBoxLayout *layoutH_2 = new QHBoxLayout;
	QHBoxLayout *layoutH_3 = new QHBoxLayout;
	QHBoxLayout *layoutH_bottom = new QHBoxLayout;
	int le_wdth = 40;

	layoutV->addLayout(layoutH_Top);
	layoutV->addLayout(layoutH_0);
	layoutV->addLayout(layoutH_1);
	layoutV->addLayout(layoutH_2);
	layoutV->addLayout(layoutH_3);
	layoutV->addLayout(layoutH_bottom);
	QVBoxLayout *layoutV_bottom_0 = new QVBoxLayout;
	QVBoxLayout *layoutV_bottom_1 = new QVBoxLayout;

	container->setStyleSheet("background-color: #414141;");
	QRect rect = container->geometry();
	rect.setWidth(200);
	container->setGeometry(rect);
	this->setGeometry(rect);

	QLabel *lb_H_Top = new QLabel(QString(""), this);
	lb_H_Top->setStyleSheet("background-color: #414141;");
	layoutH_Top->addWidget(lb_H_Top);

	//
	QLabel *lb_H_0 = new QLabel(QString("Solid"), this);
	layoutH_0->addWidget(lb_H_0);
	lb_H_0->setStyleSheet("background-color: #414141;");

	m_p_cb_auto = new QCheckBox(QString("Auto-Mode"), this);
	m_p_cb_auto->setChecked(true);
	//cb_auto->setEnabled(false);
	m_p_cb_auto->setStyleSheet(STYLE_MANAGER->m_Checkbox);
	layoutH_1->addWidget(m_p_cb_auto);

	QCheckBox* chck_preserve = new QCheckBox(QString("Preserve Boundary"), this);
	chck_preserve->setChecked(false);
	chck_preserve->setStyleSheet(STYLE_MANAGER->m_Checkbox);
	layoutH_2->addWidget(chck_preserve);

	QCheckBox* chck_repair = new QCheckBox(QString("Auto-Repair"), this);
	chck_repair->setChecked(false);
	chck_repair->setStyleSheet(STYLE_MANAGER->m_Checkbox);
	layoutH_3->addWidget(chck_repair);

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

	connect(btnOK, &QPushButton::clicked, this, &CSolidDlg::OnOK);
	connect(btnCancel, &QPushButton::clicked, this, &CSolidDlg::OnCancel);
	connect(m_p_cb_auto, &QPushButton::clicked, this, &CSolidDlg::OnCheckBoxClickedAutoMode);

	setLayout(layoutV);
}

void CSolidDlg::OnCheckBoxClickedAutoMode()
{
	m_p_cb_auto->setChecked(true);
}

void CSolidDlg::OnOK()
{
	MEVolumeView* pViewer = (MEVolumeView*)(this->parent());

	if (pViewer && m_pDataContext && ACTION_MANAGER->isActionFinished())
	{
		int MeshIdx = m_pDataContext->m_MeshData.GetCurrentMeshIndex();

		WorkMeshRemesh* pWMR = ACTION_MANAGER->action_Mesh_Init(MESH_SOLID, MeshIdx, m_pDataContext, pViewer);

		if (pWMR == nullptr)
		{
			return;
		}

		ACTION_MANAGER->action_Mesh(pWMR);

		//close();
	}
}

void CSolidDlg::OnCancel()
{
	if (m_pDataContext && ACTION_MANAGER->isActionFinished())
	{
		MESH_WORK_MANAGER->UpdateWorkMode(MESH_WORK_NONE, true);

		close();
	}
}