/*
@company		메디컬아이피
@autor			허 건 과장
@date			2021-12-17
@brief			CMeshCutDlg 클래스 구현 파일
*/

#include "stdafx.h"
#include "CMeshCutDlg.h"

#include "System/stringManager.h"
#include "System/styleManager.h"

#include "MeshEdit/CMeshWorkManager.h"
#include "MeshEdit/CMeshModelViewManager.h"
#include "MeshEdit/CManipulator.h"
#include "MeshEdit/CMeshCutManager.h"
#include "MeshControl.h"
#include "Renderer\Renderer.h"

#include "windowManager.h"
#include "ActionManager.h"
#include "MEVolumeView.h"
#include "Main/MainMeshWidget.h"
#include "DataContext.h"

CMeshCutDlg::CMeshCutDlg(DataContext* pDataContext, QWidget *parent)
	: MeshBaseDlg(pDataContext, parent),
	m_cb_CutMode(false),
	m_cb_FillingMode(false),
	m_bSaveUndoRedo(true),
	m_PrevIdx(-1)
{
	setWindowTitle(QString("Mesh Cut"));

	initQtUI();
}

/*
@brief
*/
CMeshCutDlg::CMeshCutDlg(DataContext* pDataContext, QString _title, QWidget *parent)
	: MeshBaseDlg(pDataContext, _title, parent),
	m_cb_CutMode(false),
	m_cb_FillingMode(false),
	m_bSaveUndoRedo(true),
	m_PrevIdx(-1)
{
	setWindowTitle(_title);

	initQtUI();
}

/*
@brief
*/
CMeshCutDlg::~CMeshCutDlg()
{

}

/*
@brief
@return
*/
void		CMeshCutDlg::reject(bool bForce)
{
	if (!m_pDataContext)
	{
		return;
	}

	if (!bForce)
	{
		OnCancel();
	}
	else
	{
		clearSelectAreaALL();

		MESH_WORK_MANAGER->UpdateWorkMode(MESH_WORK_NONE, true);

		WIN_MANAGER->renderLater_All();

		close();
	}
}

/*
@brief
@return
*/
void		CMeshCutDlg::initQtUI()
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

	QLabel *lb_H_Top = new QLabel(QString("Mesh Cut"), this);
	lb_H_Top->setStyleSheet("background-color: #414141;");
	layoutH_Top->addWidget(lb_H_Top);

	QLabel *lb_mode = new QLabel(QString("Mode"), this);
	lb_mode->setAlignment(Qt::AlignLeft);
	layoutH_1->addWidget(lb_mode);

	m_cb_CutMode = new QComboBox(this);
	m_cb_CutMode->addItem("Plane");
	m_cb_CutMode->addItem("Inner");
	m_cb_CutMode->addItem("Outer");
	m_cb_CutMode->addItem("Free-Poly");
	m_cb_CutMode->setFixedHeight(25);
	m_cb_CutMode->setStyleSheet(STYLE_MANAGER->comboBoxNormal);
	m_cb_CutMode->setCurrentIndex(0);
	layoutH_2->addWidget(m_cb_CutMode);

	m_PrevIdx = 0;

	QLabel *lb_filling_mode = new QLabel(QString("Filling Mode"), this);
	lb_filling_mode->setAlignment(Qt::AlignLeft);
	layoutH_3->addWidget(lb_filling_mode);

	m_cb_FillingMode = new QComboBox(this);
	m_cb_FillingMode->addItem("Minimal Fill");
	m_cb_FillingMode->addItem("Remeshed Fill");
	m_cb_FillingMode->addItem("No Fill");
	m_cb_FillingMode->setFixedHeight(25);
	m_cb_FillingMode->setCurrentIndex(1);
	m_cb_FillingMode->setStyleSheet(STYLE_MANAGER->comboBoxNormal);
	layoutH_4->addWidget(m_cb_FillingMode);

	m_btnClearSelection = new QPushButton(this);
	m_btnClearSelection->setText("Clear Selection");
	m_btnClearSelection->setFixedHeight(30);
	m_btnClearSelection->setVisible(true);
	m_btnClearSelection->setStyleSheet(STYLE_MANAGER->buttonNormal);
	layoutH_5->addWidget(m_btnClearSelection);

	m_btnInverse = new QPushButton(this);
	m_btnInverse->setText("Inverse Selection");
	m_btnInverse->setFixedHeight(30);
	m_btnInverse->setVisible(true);
	m_btnInverse->setStyleSheet(STYLE_MANAGER->buttonNormal);
	layoutH_6->addWidget(m_btnInverse);

	m_btnOk = new QPushButton(this);
	m_btnOk->setText("Cut");
	m_btnOk->setFixedSize(80, 30);
	m_btnOk->setVisible(true);
	m_btnOk->setStyleSheet(STYLE_MANAGER->buttonNormal);
	layoutV_bottom_0->addWidget(m_btnOk, Qt::AlignLeft);
	layoutV_bottom_0->setAlignment(Qt::AlignLeft);
	layoutH_bottom->addLayout(layoutV_bottom_0);

	m_btnCancel = new QPushButton(this);
	m_btnCancel->setText(STRING_MANAGER->getString(STR_CANCEL));
	m_btnCancel->setFixedSize(80, 30);
	m_btnCancel->setVisible(true);
	m_btnCancel->setStyleSheet(STYLE_MANAGER->buttonNormal);
	layoutV_bottom_1->addWidget(m_btnCancel, Qt::AlignRight);
	layoutV_bottom_1->setAlignment(Qt::AlignRight);
	layoutH_bottom->addLayout(layoutV_bottom_1);

	connect(m_btnOk, &QPushButton::clicked, this, &CMeshCutDlg::OnOK);
	connect(m_btnCancel, &QPushButton::clicked, this, &CMeshCutDlg::OnCancel);
	connect(m_btnClearSelection, &QPushButton::clicked, this, &CMeshCutDlg::OnClearSelection);
	connect(m_btnInverse, &QPushButton::clicked, this, &CMeshCutDlg::OnInverse);
	connect(m_cb_CutMode, QOverload<int>::of(&QComboBox::currentIndexChanged), this , &CMeshCutDlg::OnComboChangeIndex);

	setLayout(layoutV);
}

/*
@brief
@return
*/
void		CMeshCutDlg::clearSelectAreaALL()
{
	if (!m_pDataContext)
	{
		return;

	}
	int		n_mesh = m_pDataContext->m_MeshData.GetMeshCount();

	for (int i = 0; i < n_mesh; ++i)
	{
		mip::MeshTopology* p_mesh = m_pDataContext->m_MeshData.GetMesh(i);

		if (!p_mesh)
		{
			continue;
		}

		mip::mesh_control::MeshCutCancel(p_mesh);

		p_mesh->updateVertex();
		p_mesh->updateColor(p_mesh->m_baseColor);

		g_Renderer->makeCurrent();
		p_mesh->buildRenderBufferTopology();
		g_Renderer->doneCurrent();
	}

	WIN_MANAGER->renderLater_All();
}

/*
@brief
@return
*/
void		CMeshCutDlg::OnComboChangeIndex(int _index)
{
	if (!m_pDataContext)
	{
		return;
	}

	if (_index == 0)
	{
		mip::MeshTopology * m = m_pDataContext->m_MeshData.GetCurrentMesh();

		if (m)
		{
			PLANE_MANIPULATOR->UpdatePosition(m);
		}

		if (m_bSaveUndoRedo)
		{
			ACTION_MANAGER->action_UndoRedo_update(m_pDataContext, MESH_WORK_MODE(MESH_WORK_PLANE_CUT + _index), MESH_WORK_MODE(MESH_WORK_PLANE_CUT + m_PrevIdx));
		}
		
		MESH_CUT_MANAGER->process_CancelCut();
	}
	else
	{
		if (m_bSaveUndoRedo)
		{
			ACTION_MANAGER->action_UndoRedo_update(m_pDataContext, MESH_WORK_MODE(MESH_WORK_PLANE_CUT + _index), MESH_WORK_MODE(MESH_WORK_PLANE_CUT + m_PrevIdx));
		}
	}

	MESH_WORK_MANAGER->UpdateWorkMode(MESH_WORK_MODE(MESH_WORK_PLANE_CUT + _index), true);

	m_PrevIdx = _index;

	m_bSaveUndoRedo = true;

	WIN_MANAGER->renderLater_All();
}

/*
@brief
@return
*/
void		CMeshCutDlg::OnOK()
{
	if (!m_pDataContext)
	{
		return;
	}

	MESH_WORK_MODE mode = MESH_WORK_MODE(MESH_WORK_PLANE_CUT + m_cb_CutMode->currentIndex());

	// Plane Cut 수행
	switch (mode)
	{
	case MESH_WORK_PLANE_CUT :
		 MESH_CUT_MANAGER->Process(MESH_WORK_PLANE_CUT);
		 break;
	case MESH_WORK_POLYGON_CUT:
	case MESH_WORK_POLYLINE_CUT:
	case MESH_WORK_FREEPOLYLINE_CUT:
		 ACTION_MANAGER->action_UndoRedo_update(m_pDataContext, mode, mode);
		 MESH_CUT_MANAGER->process_FinishCut();
		 break;
	}

	MEVolumeView* view = WIN_MANAGER->mainMeshWidget->getMainView();
	view->updateGeometryCount();

	WIN_MANAGER->renderLater_All();
}

/*
@brief
@return
*/
void		CMeshCutDlg::OnClearSelection()
{
	if (!m_pDataContext)
	{
		return;
	}

	int n_mesh = m_pDataContext->m_MeshData.GetMeshCount();

	for (int iter = 0; iter < n_mesh; ++iter)
	{
		MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(iter);

		//if (WIN_MANAGER->vt_pckID[iter])
		if (pMeshInfo && pMeshInfo->selected)
		{
			mip::MeshTopology* p_mesh = m_pDataContext->m_MeshData.GetMesh(iter);

			if (p_mesh)
			{
				std::vector<mip::TVert> & tverts = p_mesh->m_tverts;

				int n_tverts = tverts.size();
				for (int vi = 0; vi < n_tverts; ++vi)
				{
					if (tverts[vi].isD())
					{
						continue;
					}

					if (tverts[vi].isS())
					{
						tverts[vi].clearS();
					}
				}

				std::vector<mip::TTri> & ttris = p_mesh->m_ttris;

				int n_ttris = ttris.size();
				for (int ti = 0; ti < n_ttris; ++ti)
				{
					if (ttris[ti].isD())
					{
						continue;
					}

					if (ttris[ti].isS())
					{
						ttris[ti].clearS();
					}
				}

				std::vector<mip::THEdge> & thedge = p_mesh->m_tVHedges;

				int n_hedge = thedge.size();
				for (int hi = 0; hi < n_hedge; ++hi)
				{
					if (thedge[hi].isD())
					{
						continue;
					}

					if (thedge[hi].isS())
					{
						thedge[hi].clearS();
					}
				}

				p_mesh->updateColor(p_mesh->m_baseColor);

				mip::SRenderBufferParams params;

				params.b_update_release = false;
				params.b_update_vertex = false;
				params.b_update_normal = false;
				params.b_update_color = false;
				params.b_update_texture = false;
				params.b_update_index = false;

				params.b_use_color_sub = true;

				WIN_MANAGER->buildRenderBufferTopology(p_mesh, &params);
			}
		}
	}

	WIN_MANAGER->renderLater_All();
}

/*
@brief
@return
*/
void		CMeshCutDlg::OnInverse()
{
	if (!m_pDataContext)
	{
		return;
	}

	MESH_WORK_MODE mode = MESH_WORK_MODE(MESH_WORK_PLANE_CUT + m_cb_CutMode->currentIndex());

	if (mode == MESH_WORK_PLANE_CUT)
	{
		PLANE_MANIPULATOR->setInverseZAxis();
	}
	else
	{
		int n_mesh = m_pDataContext->m_MeshData.GetMeshCount();

		for (int iter = 0; iter < n_mesh; ++iter)
		{
			MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(iter);

			//if (WIN_MANAGER->vt_pckID[iter])
			if (pMeshInfo && pMeshInfo->selected)
			{
				mip::MeshTopology* p_mesh = m_pDataContext->m_MeshData.GetMesh(iter);

				if (p_mesh)
				{
					std::vector<mip::TVert> & tverts = p_mesh->m_tverts;

					int n_tverts = tverts.size();
					for (int vi = 0; vi < n_tverts; ++vi)
					{
						if (tverts[vi].isD())
						{
							continue;
						}

						if (tverts[vi].isS())
						{
							tverts[vi].clearS();
						}
						else
						{
							tverts[vi].setS();
						}
					}

					std::vector<mip::TTri> & ttris = p_mesh->m_ttris;

					int n_ttris = ttris.size();
					for (int ti = 0; ti < n_ttris; ++ti)
					{
						if (ttris[ti].isD())
						{
							continue;
						}

						if (ttris[ti].isS())
						{
							ttris[ti].clearS();
						}
						else
						{
							ttris[ti].setS();
						}
					}

					p_mesh->updateColor(p_mesh->m_baseColor);

					mip::SRenderBufferParams params;

					params.b_update_release = false;
					params.b_update_vertex = false;
					params.b_update_normal = false;
					params.b_update_color = false;
					params.b_update_texture = false;
					params.b_update_index = false;

					params.b_use_color_sub = true;

					WIN_MANAGER->buildRenderBufferTopology(p_mesh, &params);
				}
			}
		}
	}

	WIN_MANAGER->renderLater_All();
}

/*
@brief
@return
*/
void		CMeshCutDlg::OnCancel()
{
	if (!m_pDataContext)
	{
		return;
	}

	ACTION_MANAGER->action_UndoRedo_update(m_pDataContext, MESH_WORK_NONE, MESH_WORK_MANAGER->getWorkMode());

	//MESH_CUT_MANAGER->process_CancelCut();
	clearSelectAreaALL();

	MESH_WORK_MANAGER->UpdateWorkMode(MESH_WORK_NONE, true);

	WIN_MANAGER->renderLater_All();

	close();
}

/*
@brief
@return
*/
void		CMeshCutDlg::updateComboboxIndex(int index, bool bSaveUndoRedo)
{
	if (index < 0 || index >= m_cb_CutMode->count())
	{
		return;
	}

	m_bSaveUndoRedo = bSaveUndoRedo;

	m_cb_CutMode->setCurrentIndex(index);
}

/*
@brief
@return
*/
int			CMeshCutDlg::getTypeFilling()
{
	if (!m_cb_FillingMode)
	{
		return -1;
	}

	return m_cb_FillingMode->currentIndex();
}