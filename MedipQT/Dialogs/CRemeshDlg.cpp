#include "stdafx.h"
#include "CRemeshDlg.h"
#include "windowManager.h"
#include "Main/MainMeshWidget.h"
#include "MEVolumeView.h"

#include "System/stringManager.h"
#include "System/styleManager.h"

#include "MeshEdit/CMeshWorkManager.h"
#include "MeshEdit/CMeshDlgManager.h"
#include "Actions/ActionManager.h"
#include "ActionMesh.h"

#include <ppl.h>

#include "DataContext.h"


#define USE_PPL

CRemeshDlg::CRemeshDlg(DataContext* pDataContext, QWidget *parent)
	: MeshBaseDlg(pDataContext, parent),
	m_bSeletMode(false)
{
	setWindowTitle(QString("Remesh"));
	initQtUI();
}

/*
@brief
*/
CRemeshDlg::CRemeshDlg(DataContext* pDataContext, QString _title, QWidget *parent)
	: MeshBaseDlg(pDataContext, _title, parent),
	m_bSeletMode(false)
{
	initQtUI();
}

/*
@brief
*/
CRemeshDlg::~CRemeshDlg()
{

}

/*
@brief
@return
*/
void CRemeshDlg::reject(bool bForce)
{
	MESH_WORK_MODE mode = MESH_WORK_MANAGER->getWorkMode();

	setVisible(false);

	ACTION_MANAGER->action_UndoRedo_update(m_pDataContext, MESH_WORK_NONE, MESH_WORK_REMESH);

	MESH_WORK_MANAGER->UpdateWorkMode(MESH_WORK_NONE, true);

	close();
}

/*
@brief
@return 
*/
void	CRemeshDlg::initQtUI()
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
	QHBoxLayout *layoutH_6 = new QHBoxLayout;
	
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
	layoutV->addLayout(layoutH_6);
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
	QLabel *lb_H_0 = new QLabel(QString("Remesh"), this);
	layoutH_0->addWidget(lb_H_0);
	lb_H_0->setStyleSheet("background-color: #414141;");

	QLabel *lb_mode = new QLabel(QString("Mode"), this);
	lb_mode->setAlignment(Qt::AlignLeft);
	layoutH_1->addWidget(lb_mode);

	m_pComboBoxMode = new QComboBox(this);
	m_pComboBoxMode->addItem("Edge-Length");
	m_pComboBoxMode->setCurrentIndex(0);
	m_pComboBoxMode->setFixedHeight(25);
	m_pComboBoxMode->setStyleSheet(STYLE_MANAGER->comboBoxNormal);

	layoutH_2->addWidget(m_pComboBoxMode);

	QLabel *lb_edge_len = new QLabel(QString("Edge Length"), this);
	layoutH_3->addWidget(lb_edge_len);

	double min = 0.;
	double max = 1.;
	double initVal = 0.5;

	m_pLineEdit = initLineEdit(this, QString("Offset"), emLE_DOUBLE, min, max, initVal, 2, le_wdth);
	m_pLineEdit->setEnabled(false);
	layoutH_3->addWidget(m_pLineEdit);

	initSlider(this, QString("Edge-Length"), m_Slider, Qt::Horizontal, emLE_INT, 1, 100, initVal);
	m_Slider.pSlidier->setEnabled(false);
	m_Slider.pSlidier->setStyleSheet(STYLE_MANAGER->sliderBarTab);
	layoutH_4->addWidget(m_Slider.pSlidier);

	m_p_CheckBoxAuto = new QCheckBox(QString("Auto-Mode"),this);
	m_p_CheckBoxAuto->setChecked(true);
	m_p_CheckBoxAuto->setEnabled(false);
	m_p_CheckBoxAuto->setStyleSheet(STYLE_MANAGER->m_Checkbox);
	layoutH_5->addWidget(m_p_CheckBoxAuto);

	m_p_CheckBoxPreserve = new QCheckBox(QString("Preserve Boundary"), this);
	m_p_CheckBoxPreserve->setChecked(false);
	m_p_CheckBoxPreserve->setStyleSheet(STYLE_MANAGER->m_Checkbox);
	layoutH_6->addWidget(m_p_CheckBoxPreserve);

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

	connect(btnOK, &QPushButton::clicked, this, &CRemeshDlg::OnOK);
	connect(btnCancel, &QPushButton::clicked, this, &CRemeshDlg::OnCancel);
	connect(m_p_CheckBoxAuto, &QCheckBox::clicked, this, &CRemeshDlg::OnCheckBoxModeClicked);
	connect(m_Slider.pSlidier, &QSlider::valueChanged, this, &CRemeshDlg::OnDepthSlideReleased);

	setLayout(layoutV);
}

void CRemeshDlg::OnOK()
{
	MEVolumeView* pViewer = (MEVolumeView*)(this->parent());

	if (pViewer && m_pDataContext && ACTION_MANAGER->isActionFinished())
	{
		int MeshIdx = m_pDataContext->m_MeshData.GetCurrentMeshIndex();

		WorkMeshRemesh* pWMR = ACTION_MANAGER->action_Mesh_Init(MESH_REMESH, MeshIdx, m_pDataContext, pViewer);

		//pWMR->setEdgeLength(m_EdgeLenthTemp);
		pWMR->m_Params.edge_len_remesh = (double)m_EdgeLenthTemp;

		if (pWMR == nullptr)
		{
			return;
		}

		if (m_bSeletMode)
		{
			pWMR->m_Params.b_sculpt_mode = false;
			pWMR->m_Params.b_select_mode = true;
		}
		else
		{
			pWMR->m_Params.b_sculpt_mode = false;
			pWMR->m_Params.b_select_mode = false;
		}

		ACTION_MANAGER->action_Mesh(pWMR);

		//close();	
	}
}

void CRemeshDlg::OnCancel()
{
	if (ACTION_MANAGER->isActionFinished())
	{
		MESH_WORK_MANAGER->UpdateWorkMode(MESH_WORK_NONE, true);

		close();
	}
}

void CRemeshDlg::OnCheckBoxModeClicked()
{
	bool b_checked = m_p_CheckBoxAuto->isChecked();

	if (b_checked)
	{
		//m_pLineEdit->setEnabled(false);
		//m_Slider.pSlidier->setEnabled(false);

		m_EdgeLenthTemp = FLT_MIN;
	}
	else
	{
		m_pLineEdit->setEnabled(true);
		m_Slider.pSlidier->setEnabled(true);

		if (m_PosSlider < 50)
		{
			m_EdgeLenthTemp = m_EdgeLenth - (m_Interval * (float)(50 - m_PosSlider));
		}
		else if (m_PosSlider > 50)
		{
			m_EdgeLenthTemp = m_EdgeLenth + (m_Interval * (float)(m_PosSlider - 50));
		}
		else
		{
			m_EdgeLenthTemp = m_EdgeLenth;
		}
	}
}

void CRemeshDlg::OnDepthSlideReleased(int _val)
{
	m_PosSlider = _val;

	if (m_PosSlider < 50)
	{
		m_EdgeLenthTemp = m_EdgeLenth - (m_Interval * (float)(50 - m_PosSlider));
	}
	else if (m_PosSlider > 50)
	{
		m_EdgeLenthTemp = m_EdgeLenth + (m_Interval * (float)(m_PosSlider - 50));
	}
	else
	{
		m_EdgeLenthTemp = m_EdgeLenth;
	}

	m_pLineEdit->blockSignals(true);
	m_pLineEdit->setText(QString().sprintf("%.3f", m_EdgeLenthTemp));
	m_pLineEdit->blockSignals(false);

	m_p_CheckBoxAuto->setChecked(false);
}

/*
@brief
@return
*/
void CRemeshDlg::Update()
{
	if (!m_pDataContext)
	{
		return;
	}

	int count = 0;
	int selected = 0;
	int nMesh = 0;
	//for (int i = 0; i < _vt_pick_model.size(); ++i)
	for (int i = 0; i < nMesh; ++i)
	{
		MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(i);
		//if (_vt_pick_model[i])
		if(pMeshInfo && pMeshInfo->selected)
		{
			++count;
			selected = i;
		}
	}

	bool b_enable = true;
	bool b_auto   = true;

	if (count == 1)
	{
		m_p_CheckBoxAuto->setEnabled(b_enable);
		m_Slider.pSlidier->setEnabled(b_enable);
		m_pLineEdit->setEnabled(b_enable);
	}
	else
	{
		b_enable = false;

		m_p_CheckBoxAuto->setEnabled(b_enable);
		m_Slider.pSlidier->setEnabled(b_enable);
		m_pLineEdit->setEnabled(b_enable);
	}

	if (m_Slider.pSlidier)
	{
		if (count == 1)
		{
			// Edge Length °¡Á®¿È
			auto mesh = m_pDataContext->m_MeshData.GetMesh(selected);

			auto & bb = mesh->m_boundingBox;

			float  diag = (bb.min - bb.max).length();

			// calc mean edge
			auto & ttris = mesh->m_ttris;
			auto & tverts = mesh->m_tverts;

			int n_tris = ttris.size();

#ifdef USE_PPL
			const auto				    num_core = std::thread::hardware_concurrency();

			std::vector<float> vt_mean(num_core, 0);
			std::vector<int> vt_cnt(num_core, 0);

			int interval = n_tris / (int)num_core;
			int remain = n_tris % (int)num_core;

			concurrency::parallel_for(0, (int)num_core, [&](int i)
			{
				int ti = interval * i;
				int finish = ti + interval;

				if (i == (num_core - 1))
				{
					finish += remain;
				}

				for (; ti < finish; ++ti)
				{
					if (ttris[ti].isD())
					{
						continue;
					}

					int & idx0 = ttris[ti].vi[0];
					int & idx1 = ttris[ti].vi[1];
					int & idx2 = ttris[ti].vi[2];

					auto & pos0 = tverts[idx0].pos;
					auto & pos1 = tverts[idx1].pos;
					auto & pos2 = tverts[idx2].pos;

					float dist0 = (pos0 - pos1).length();
					float dist1 = (pos0 - pos2).length();
					float dist2 = (pos1 - pos2).length();

					vt_mean[i] += (dist0 + dist1 + dist2) / 3.f;
					vt_cnt[i]++;
				}
			});

			m_EdgeLenth = std::accumulate(vt_mean.begin(), vt_mean.end(), static_cast<float>(0));

			int tot_cnt = std::accumulate(vt_cnt.begin(), vt_cnt.end(), static_cast<int>(0));

			m_EdgeLenth /= (float)tot_cnt;

#else
			float mean_dist = 0.f;
			for (int ti = 0; ti < n_tris; ++ti)
			{
				int & idx0 = ttris[ti].vi[0];
				int & idx1 = ttris[ti].vi[1];
				int & idx2 = ttris[ti].vi[2];

				auto & pos0 = tverts[idx0].pos;
				auto & pos1 = tverts[idx1].pos;
				auto & pos2 = tverts[idx2].pos;

				float dist0 = (pos0 - pos1).length();
				float dist1 = (pos0 - pos2).length();
				float dist2 = (pos1 - pos2).length();

				mean_dist += (dist0 + dist1 + dist2) / 3.f;
			}

			mean_dist /= (float)n_tris;
#endif

			m_EdgeLenthTemp  = m_EdgeLenth;

			m_Interval = float((double)m_EdgeLenth / 100.);

			m_Slider.pSlidier->setValue(50);

			m_PosSlider = 50;
		}
		else
		{
			m_EdgeLenthTemp = FLT_MIN;
		}

		if (m_pLineEdit)
		{
			if (count == 1)
			{
				m_pLineEdit->setText(QString().sprintf("%.3f", m_EdgeLenthTemp));
			}	
		}

		if (b_auto)
		{
			m_EdgeLenthTemp = FLT_MIN;
		}
	}

	m_p_CheckBoxAuto->setChecked(true);
}