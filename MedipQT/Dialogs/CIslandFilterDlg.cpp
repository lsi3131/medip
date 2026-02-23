/*
@company		메디컬아이피
@autor			허 건 과장
@date			2021-10-18
@brief			CIslandFilterDlg 구현파일
*/

#include "stdafx.h"
#include "CIslandFilterDlg.h"
#include "stringManager.h"
#include "ActionManager.h"
#include "windowManager.h"
#include "styleManager.h"
#include "ActionMesh.h"
#include "MeshEdit\CMeshWorkManager.h"
#include "DataContext.h"

CIslandFilterDlg::CIslandFilterDlg(DataContext* pDataContext, QWidget *parent)
	: MeshBaseDlg(pDataContext, parent)
{
	setWindowTitle(QString("Island filter"));
	initQtUI();
}

/*
@brief
*/
CIslandFilterDlg::CIslandFilterDlg(DataContext* pDataContext, QString _title, QWidget *parent)
	: MeshBaseDlg(pDataContext, _title, parent)
{
	initQtUI();
}

/*
@brief
*/
CIslandFilterDlg::~CIslandFilterDlg()
{
	if (m_p_CheckBox_Remain_Large != NULL)
	{
		SAFE_DELETE(m_p_CheckBox_Remain_Large);
	}

	if (m_p_CheckBox_Remain_Inverse != NULL)
	{
		SAFE_DELETE(m_p_CheckBox_Remain_Inverse);
	}

	if (m_p_CheckBox_num_Face != NULL)
	{
		SAFE_DELETE(m_p_CheckBox_num_Face);
	}
}

/*
@brief
@return
*/
void CIslandFilterDlg::reject(bool bForce)
{
	if (!m_pDataContext)
	{
		return;
	}

	MESH_WORK_MODE mode = MESH_WORK_MANAGER->getWorkMode();

	setVisible(false);

	ACTION_MANAGER->action_UndoRedo_update(m_pDataContext, MESH_WORK_NONE, MESH_WORK_ISLANDFILTER);

	MESH_WORK_MANAGER->UpdateWorkMode(MESH_WORK_NONE, true);

	close();
}

/*
@brief
@return
*/
void	CIslandFilterDlg::initQtUI()
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
	QLabel *lb_H_0 = new QLabel(QString("Island Filter"), this);
	layoutH_0->addWidget(lb_H_0);
	lb_H_0->setStyleSheet("background-color: #414141;");

	m_p_CheckBox_Remain_Large = new QCheckBox(QString("Remain large parts"), this);
	m_p_CheckBox_Remain_Large->setChecked(true);
	m_p_CheckBox_Remain_Large->setEnabled(true);
	m_p_CheckBox_Remain_Large->setStyleSheet(STYLE_MANAGER->m_Checkbox);
	layoutH_1->addWidget(m_p_CheckBox_Remain_Large);

	m_p_CheckBox_Remain_Inverse = new QCheckBox(QString("Inverse"), this);
	m_p_CheckBox_Remain_Inverse->setChecked(false);
	m_p_CheckBox_Remain_Inverse->setEnabled(true);
	m_p_CheckBox_Remain_Inverse->setStyleSheet(STYLE_MANAGER->m_Checkbox);
	layoutH_2->addWidget(m_p_CheckBox_Remain_Inverse);

	m_p_SpinBox_Remain = new QSpinBox(this);
	m_p_SpinBox_Remain->setStyleSheet(STYLE_MANAGER->spinbox);
	m_p_SpinBox_Remain->setRange(1, INT_MAX);
	m_p_SpinBox_Remain->setFixedSize(125, 30);
	m_p_SpinBox_Remain->setAlignment(Qt::AlignLeft);
	m_p_SpinBox_Remain->setStyleSheet(STYLE_MANAGER->spinbox);
	layoutH_3->addWidget(m_p_SpinBox_Remain);

	m_p_CheckBox_num_Face = new QCheckBox(QString("Number of Triangles"), this);
	m_p_CheckBox_num_Face->setChecked(false);
	m_p_CheckBox_num_Face->setEnabled(true);
	m_p_CheckBox_num_Face->setStyleSheet(STYLE_MANAGER->m_Checkbox);
	layoutH_4->addWidget(m_p_CheckBox_num_Face);

	m_p_SpinBox_Triangle = new QSpinBox(this);
	m_p_SpinBox_Triangle->setStyleSheet(STYLE_MANAGER->spinbox);
	m_p_SpinBox_Triangle->setRange(1, INT_MAX);
	m_p_SpinBox_Triangle->setFixedSize(125, 30);
	m_p_SpinBox_Triangle->setAlignment(Qt::AlignLeft);
	m_p_SpinBox_Triangle->setStyleSheet(STYLE_MANAGER->spinbox);
	layoutH_5->addWidget(m_p_SpinBox_Triangle);

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

	connect(btnOK, &QPushButton::clicked, this, &CIslandFilterDlg::OnOK);
	connect(btnCancel, &QPushButton::clicked, this, &CIslandFilterDlg::OnCancel);

	connect(m_p_CheckBox_Remain_Large, &QCheckBox::clicked, this, &CIslandFilterDlg::OnCheckBoxLargeModeClicked);
	connect(m_p_CheckBox_Remain_Inverse, &QCheckBox::clicked, this, &CIslandFilterDlg::OnCheckBoxSmallClicked);
	connect(m_p_CheckBox_num_Face, &QCheckBox::clicked, this, &CIslandFilterDlg::OnCheckBoxFaceClicked);

	setLayout(layoutV);
}

/*
@brief
@return
*/
void CIslandFilterDlg::OnCheckBoxLargeModeClicked()
{
	if (m_p_CheckBox_Remain_Large->isChecked())
	{
		m_p_CheckBox_num_Face->setChecked(false);
		m_p_SpinBox_Triangle->setEnabled(false);

		m_p_SpinBox_Remain->setEnabled(true);
	}
	else
	{
		m_p_CheckBox_num_Face->setChecked(true);
		m_p_SpinBox_Triangle->setEnabled(true);

		m_p_CheckBox_Remain_Inverse->setChecked(false);
		m_p_SpinBox_Remain->setEnabled(false);
	}
}

/*
@brief
@return
*/
void CIslandFilterDlg::OnCheckBoxSmallClicked()
{
	if (m_p_CheckBox_Remain_Inverse->isChecked())
	{
		m_p_CheckBox_Remain_Large->setChecked(true);
		m_p_SpinBox_Remain->setEnabled(true);

		m_p_CheckBox_num_Face->setChecked(false);
		m_p_SpinBox_Triangle->setEnabled(false);
	}
}

/*
@brief
@return
*/
void CIslandFilterDlg::OnCheckBoxFaceClicked()
{
	if (m_p_CheckBox_num_Face->isChecked())
	{
		m_p_CheckBox_Remain_Large->setChecked(false);
		m_p_CheckBox_Remain_Inverse->setChecked(false);

		m_p_SpinBox_Remain->setEnabled(false);

		m_p_SpinBox_Triangle->setEnabled(true);
	}
	else
	{
		m_p_CheckBox_Remain_Large->setChecked(true);
		m_p_SpinBox_Remain->setEnabled(true);

		m_p_SpinBox_Remain->setEnabled(true);
		m_p_SpinBox_Triangle->setEnabled(false);
	}
}

/*
@brief
@return
*/
void CIslandFilterDlg::OnOK()
{
	MEVolumeView* pViewer = (MEVolumeView*)(this->parent());

	if (!m_pDataContext || !pViewer)
	{
		return;
	}

	if (ACTION_MANAGER->isActionFinished())
	{
		int MeshIdx = m_pDataContext->m_MeshData.GetCurrentMeshIndex();

		WorkMeshRemesh* pWMR = ACTION_MANAGER->action_Mesh_Init(MESH_ISLAND_FILTER, MeshIdx, m_pDataContext, pViewer);

		if (pWMR == nullptr)
		{
			return;
		}

		if (m_p_CheckBox_Remain_Large->isChecked())
		{
			pWMR->m_Params.n_remain_if = m_p_SpinBox_Remain->value();

			pWMR->m_Params.b_inverse_if = m_p_CheckBox_Remain_Inverse->isChecked() ? true : false;

			pWMR->m_Params.n_trinagles_if = -1;
		}
		else if (m_p_CheckBox_num_Face->isChecked())
		{
			pWMR->m_Params.n_trinagles_if = m_p_SpinBox_Triangle->value();

			pWMR->m_Params.n_remain_if = -1;

			pWMR->m_Params.b_inverse_if = false;
		}

		ACTION_MANAGER->action_Mesh(pWMR);

		//close();
	}
}

/*
@brief
@return
*/
void CIslandFilterDlg::OnCancel()
{
	if (ACTION_MANAGER->isActionFinished())
	{
		MESH_WORK_MANAGER->UpdateWorkMode(MESH_WORK_NONE, true);

		close();
	}
}