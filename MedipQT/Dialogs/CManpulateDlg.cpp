#include "stdafx.h"
#include "CManpulateDlg.h"
#include "MeshEdit/CMeshModelViewManager.h"
#include "MeshEdit/CMeshWorkManager.h"
#include "MeshEdit/CMeshManipulator.h"
#include "MeshControl.h"

#include "windowManager.h"
#include "Main/MainMeshWidget.h"
#include "MEVolumeView.h"

#include "System/stringManager.h"
#include "System/styleManager.h"

#include "ActionManager.h"
#include "DataContext.h"

/*
@brief
*/
CManpulateDlg::CManpulateDlg(DataContext* pDataContext, QWidget *parent)
	: MeshBaseDlg(pDataContext, parent)
	, m_bPatientCoordinate(false)
	, m_bFinish(true)
{
	setWindowTitle(QString("Manipulate"));
	initQtUI();
}

/*
@brief
*/
CManpulateDlg::CManpulateDlg(DataContext* pDataContext, QString _title, QWidget *parent)
	: MeshBaseDlg(pDataContext, parent)
	, m_bPatientCoordinate(false)
	, m_bFinish(true)
{
	initQtUI();
}

/*
@brief
*/
CManpulateDlg::~CManpulateDlg()
{

}

/*
@brief
@return
*/
void		CManpulateDlg::initQtUI()
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

	QHBoxLayout *layoutH_7 = new QHBoxLayout;
	QHBoxLayout *layoutH_8 = new QHBoxLayout;
	QHBoxLayout *layoutH_9 = new QHBoxLayout;
	QHBoxLayout *layoutH_10 = new QHBoxLayout;

	QHBoxLayout *layoutH_11 = new QHBoxLayout;
	QHBoxLayout *layoutH_12 = new QHBoxLayout;
	QHBoxLayout *layoutH_13 = new QHBoxLayout;
	QHBoxLayout *layoutH_14 = new QHBoxLayout;
	QHBoxLayout *layoutH_15 = new QHBoxLayout;

	QHBoxLayout *layoutH_bottom = new QHBoxLayout;
	QVBoxLayout *layoutV_bottom_0 = new QVBoxLayout;
	QVBoxLayout *layoutV_bottom_1 = new QVBoxLayout;

	QVBoxLayout *layoutV_bottom_4_0 = new QVBoxLayout;
	QVBoxLayout *layoutV_bottom_4_1 = new QVBoxLayout;
	QVBoxLayout *layoutV_bottom_5_0 = new QVBoxLayout;
	QVBoxLayout *layoutV_bottom_5_1 = new QVBoxLayout;
	QVBoxLayout *layoutV_bottom_6_0 = new QVBoxLayout;
	QVBoxLayout *layoutV_bottom_6_1 = new QVBoxLayout;

	QVBoxLayout *layoutV_bottom_8_0 = new QVBoxLayout;
	QVBoxLayout *layoutV_bottom_8_1 = new QVBoxLayout;
	QVBoxLayout *layoutV_bottom_9_0 = new QVBoxLayout;
	QVBoxLayout *layoutV_bottom_9_1 = new QVBoxLayout;
	QVBoxLayout *layoutV_bottom_10_0 = new QVBoxLayout;
	QVBoxLayout *layoutV_bottom_10_1 = new QVBoxLayout;

	QVBoxLayout *layoutV_bottom_13_0 = new QVBoxLayout;
	QVBoxLayout *layoutV_bottom_13_1 = new QVBoxLayout;
	QVBoxLayout *layoutV_bottom_14_0 = new QVBoxLayout;
	QVBoxLayout *layoutV_bottom_14_1 = new QVBoxLayout;
	QVBoxLayout *layoutV_bottom_15_0 = new QVBoxLayout;
	QVBoxLayout *layoutV_bottom_15_1 = new QVBoxLayout;
	

	layoutV->addLayout(layoutH_Top);
	layoutV->addLayout(layoutH_0);
	layoutV->addLayout(layoutH_1);
	layoutV->addLayout(layoutH_2);

	layoutV->addLayout(layoutH_3);
	layoutV->addLayout(layoutH_4);
	layoutV->addLayout(layoutH_5);
	layoutV->addLayout(layoutH_6);

	layoutV->addLayout(layoutH_7);
	layoutV->addLayout(layoutH_8);
	layoutV->addLayout(layoutH_9);
	layoutV->addLayout(layoutH_10);

	layoutV->addLayout(layoutH_11);
	layoutV->addLayout(layoutH_12);
	layoutV->addLayout(layoutH_13);
	layoutV->addLayout(layoutH_14);
	layoutV->addLayout(layoutH_15);
	layoutV->addLayout(layoutH_bottom);

	container->setStyleSheet("background-color: #414141;");
	QRect rect = container->geometry();
	rect.setWidth(200);
	container->setGeometry(rect);
	this->setGeometry(rect);

	QLabel *lb_H_Top = new QLabel(QString("Manipulator"), this);
	lb_H_Top->setStyleSheet("background-color: #414141;");
	layoutH_Top->addWidget(lb_H_Top);

	//
	//QLabel *lb_H_0 = new QLabel(QString("Manipulator"), this);
	//layoutH_0->addWidget(lb_H_0);	
	//lb_H_0->setStyleSheet("background-color: #414141;");

	QLabel *lb_mode = new QLabel(QString("Space"), this);
	lb_mode->setAlignment(Qt::AlignLeft);
	layoutH_1->addWidget(lb_mode);

	m_p_cbSpace = new QComboBox(this);
	m_p_cbSpace->addItem("Local Coordinate");
	m_p_cbSpace->addItem("Patient Coordinate");
	m_p_cbSpace->setCurrentIndex(0);
	m_p_cbSpace->setFixedHeight(25);
	m_p_cbSpace->setStyleSheet(STYLE_MANAGER->comboBoxNormal);
	m_p_cbSpace->setEnabled(false);

	layoutH_2->addWidget(m_p_cbSpace);

	//connect(m_p_cbSpace, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CManpulateDlg::OnComboBoxIndexChanged);
	
	lb_mode = new QLabel(QString(" "), this);
	lb_mode->setAlignment(Qt::AlignLeft);
	layoutH_3->addWidget(lb_mode);

	lb_mode = new QLabel(QString("Rotate X(deg)"), this);
	lb_mode->setAlignment(Qt::AlignLeft);
	layoutV_bottom_4_0->addWidget(lb_mode, Qt::AlignLeft);	
	layoutH_4->addLayout(layoutV_bottom_4_0);	

	m_p_lbRotate[0] = new QLabel(QString("0.0"), this);
	m_p_lbRotate[0]->setAlignment(Qt::AlignRight);
	layoutV_bottom_4_1->addWidget(m_p_lbRotate[0], Qt::AlignRight);	
	layoutH_4->addLayout(layoutV_bottom_4_1);
	

	lb_mode = new QLabel(QString("Rotate Y(deg)"), this);
	lb_mode->setAlignment(Qt::AlignLeft);
	layoutV_bottom_5_0->addWidget(lb_mode, Qt::AlignLeft);
	layoutH_5->addLayout(layoutV_bottom_5_0);

	m_p_lbRotate[1] = new QLabel(QString("0.0"), this);
	m_p_lbRotate[1]->setAlignment(Qt::AlignRight);
	layoutV_bottom_5_1->addWidget(m_p_lbRotate[1], Qt::AlignRight);
	layoutH_5->addLayout(layoutV_bottom_5_1);

	lb_mode = new QLabel(QString("Rotate Z(deg)"), this);
	lb_mode->setAlignment(Qt::AlignLeft);
	layoutV_bottom_6_0->addWidget(lb_mode, Qt::AlignLeft);
	layoutH_6->addLayout(layoutV_bottom_6_0);

	m_p_lbRotate[2] = new QLabel(QString("0.0"), this);
	m_p_lbRotate[2]->setAlignment(Qt::AlignRight);
	layoutV_bottom_6_1->addWidget(m_p_lbRotate[2], Qt::AlignRight);
	layoutH_6->addLayout(layoutV_bottom_6_1);

	lb_mode = new QLabel(QString(" "), this);
	lb_mode->setAlignment(Qt::AlignLeft);
	layoutH_7->addWidget(lb_mode);

	lb_mode = new QLabel(QString("Translate X(mm)"), this);
	lb_mode->setAlignment(Qt::AlignLeft);
	layoutV_bottom_8_0->addWidget(lb_mode, Qt::AlignLeft);
	layoutH_8->addLayout(layoutV_bottom_8_0);

	m_p_lbTranslate[0] = new QLabel(QString("0.0 mm "), this);
	m_p_lbTranslate[0]->setAlignment(Qt::AlignRight);
	layoutV_bottom_8_1->addWidget(m_p_lbTranslate[0], Qt::AlignRight);
	layoutH_8->addLayout(layoutV_bottom_8_1);

	lb_mode = new QLabel(QString("Translate Y(mm)"), this);
	lb_mode->setAlignment(Qt::AlignLeft);
	layoutV_bottom_9_0->addWidget(lb_mode, Qt::AlignLeft);
	layoutH_9->addLayout(layoutV_bottom_9_0);

	m_p_lbTranslate[1] = new QLabel(QString("0.0 mm "), this);
	m_p_lbTranslate[1]->setAlignment(Qt::AlignRight);
	layoutV_bottom_9_1->addWidget(m_p_lbTranslate[1], Qt::AlignRight);
	layoutH_9->addLayout(layoutV_bottom_9_1);

	lb_mode = new QLabel(QString("Translate Z(mm)"), this);
	lb_mode->setAlignment(Qt::AlignLeft);
	layoutV_bottom_10_0->addWidget(lb_mode, Qt::AlignLeft);
	layoutH_10->addLayout(layoutV_bottom_10_0);

	m_p_lbTranslate[2] = new QLabel(QString("0.0 mm "), this);
	m_p_lbTranslate[2]->setAlignment(Qt::AlignRight);
	layoutV_bottom_10_1->addWidget(m_p_lbTranslate[2], Qt::AlignRight);
	layoutH_10->addLayout(layoutV_bottom_10_1);

	QLabel *lb_interval = new QLabel(QString("Interval : "), this);
	lb_interval->setAlignment(Qt::AlignLeft);
	layoutH_11->addWidget(lb_interval);

	m_p_spinboxInterval = new QDoubleSpinBox(this);
	m_p_spinboxInterval->setStyleSheet(STYLE_MANAGER->spinbox);
	m_p_spinboxInterval->setRange(DBL_MIN, DBL_MAX);
	m_p_spinboxInterval->setSingleStep(0.1);
	m_p_spinboxInterval->setFixedSize(50, 30);
	m_p_spinboxInterval->setAlignment(Qt::AlignLeft);
	m_p_spinboxInterval->setStyleSheet(STYLE_MANAGER->spinbox);
	m_p_spinboxInterval->setValue(INTERVAL_TRANSLATE * 10.f);
	m_p_spinboxInterval->setKeyboardTracking(false);
	layoutH_11->addWidget(m_p_spinboxInterval);

	//connect(m_p_spinboxInterval, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, CManpulateDlg::OnSpinBoxValueChanged);
	connect(m_p_spinboxInterval, SIGNAL(valueChanged(double)), this, SLOT(OnSpinBoxValueChanged(double)));
	connect(m_p_spinboxInterval, &QDoubleSpinBox::editingFinished, this, &CManpulateDlg::OnSpinBoxEditingFinished);
	

	lb_mode = new QLabel(QString(" "), this);
	lb_mode->setAlignment(Qt::AlignLeft);
	layoutH_12->addWidget(lb_mode);

	lb_mode = new QLabel(QString("Scale X         "), this);
	lb_mode->setAlignment(Qt::AlignLeft);
	layoutV_bottom_13_0->addWidget(lb_mode, Qt::AlignLeft);
	layoutH_13->addLayout(layoutV_bottom_13_0);

	m_p_lbScale[0] = new QLabel(QString("1.0"), this);
	m_p_lbScale[0]->setAlignment(Qt::AlignRight);
	layoutV_bottom_13_1->addWidget(m_p_lbScale[0], Qt::AlignRight);
	layoutH_13->addLayout(layoutV_bottom_13_1);

	lb_mode = new QLabel(QString("Scale Y         "), this);
	lb_mode->setAlignment(Qt::AlignLeft);
	layoutV_bottom_14_0->addWidget(lb_mode, Qt::AlignLeft);
	layoutH_14->addLayout(layoutV_bottom_14_0);

	m_p_lbScale[1] = new QLabel(QString("1.0"), this);
	m_p_lbScale[1]->setAlignment(Qt::AlignRight);
	layoutV_bottom_14_1->addWidget(m_p_lbScale[1], Qt::AlignRight);
	layoutH_14->addLayout(layoutV_bottom_14_1);

	lb_mode = new QLabel(QString("Scale Z         "), this);
	lb_mode->setAlignment(Qt::AlignLeft);
	layoutV_bottom_15_0->addWidget(lb_mode, Qt::AlignLeft);
	layoutH_15->addLayout(layoutV_bottom_15_0);

	m_p_lbScale[2] = new QLabel(QString("1.0"), this);
	m_p_lbScale[2]->setAlignment(Qt::AlignRight);
	layoutV_bottom_15_1->addWidget(m_p_lbScale[2], Qt::AlignRight);
	layoutH_15->addLayout(layoutV_bottom_15_1);

	QPushButton *btnOK = new QPushButton(this);
	btnOK->setText(QString("Apply"));
	btnOK->setFixedSize(80, 30);
	btnOK->setVisible(true);
	btnOK->setAutoDefault(false);
	btnOK->setDefault(false);
	btnOK->setStyleSheet(STYLE_MANAGER->buttonNormal);
	layoutV_bottom_0->addWidget(btnOK, Qt::AlignLeft);
	layoutV_bottom_0->setAlignment(Qt::AlignLeft);
	layoutH_bottom->addLayout(layoutV_bottom_0);

	QPushButton *btnCancel = new QPushButton(this);
	btnCancel->setText(STRING_MANAGER->getString(STR_CANCEL));
	btnCancel->setFixedSize(80, 30);
	btnCancel->setVisible(true);
	btnCancel->setAutoDefault(false);
	btnCancel->setDefault(false);
	btnCancel->setStyleSheet(STYLE_MANAGER->buttonNormal);
	layoutV_bottom_1->addWidget(btnCancel, Qt::AlignRight);
	layoutV_bottom_1->setAlignment(Qt::AlignRight);
	layoutH_bottom->addLayout(layoutV_bottom_1);

	connect(btnOK, &QPushButton::clicked, this, &CManpulateDlg::OnOK);
	connect(btnCancel, &QPushButton::clicked, this, &CManpulateDlg::OnCancel);

	setLayout(layoutV);
}

/*
@brief
@return
*/
void		CManpulateDlg::reject(bool bForce)
{
	if (!bForce)
	{
		OnOK();
	}
	else
	{
		int n_mesh = m_pDataContext->m_MeshData.GetMeshCount();
		for (int i = 0; i < n_mesh; ++i)
		{
			MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(i);

			//if (WIN_MANAGER->vt_pckID[i])
			if (pMeshInfo && pMeshInfo->selected)
			{
				auto mesh = m_pDataContext->m_MeshData.GetMesh(i);

				if (mesh)
				{
					MESH_MANIPULATOR->CancelProcess(i);
				}
			}
		}

		MESH_MODELVIEW_MANAGER->UpdatePivotPoint();

		auto view = WIN_MANAGER->mainMeshWidget->getMainView();
		view->renderLater();

		setVisible(false);
	}
}

/*
@brief
@return
*/
void		CManpulateDlg::OnSpinBoxValueChanged(double _val)
{
	qDebug() << "_val : " << _val;
}

/*
@brief
@return
*/
void		CManpulateDlg::OnSpinBoxEditingFinished()
{

}

/*
@brief
@return
*/
void		CManpulateDlg::OnComboBoxIndexChanged(int index)
{
	if (index == 0) 
	{
		m_bPatientCoordinate = false;
		onPatientCoordinate(true);
	}
	else // index == 1 -> patient coordinate
	{
		m_bPatientCoordinate = true;
		onPatientCoordinate(false);
	}
}

/*
@brief
@return
*/
void		CManpulateDlg::OnOK()
{
	if (!m_pDataContext)
	{
		return;
	}

	int n_mesh = m_pDataContext->m_MeshData.GetMeshCount();
	for (int i = 0; i < n_mesh; ++i)
	{
		MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(i);

		//if (WIN_MANAGER->vt_pckID[i])
		if (pMeshInfo && pMeshInfo->selected)
		{
			mip::MeshTopology* mesh = m_pDataContext->m_MeshData.GetMesh(i);

			if (mesh)
			{
				MESH_MANIPULATOR->OkProcess(i);
			}
		}
	}

	CompleteProcess();

	ACTION_MANAGER->action_UndoRedo_update(m_pDataContext, MESH_WORK_NONE, MESH_WORK_MANIFULATE);
}

/*
@brief
@return
*/
void		CManpulateDlg::OnCancel()
{
	if (!m_pDataContext)
	{
		return;
	}

	ACTION_MANAGER->action_UndoRedo_update(m_pDataContext, MESH_WORK_NONE, MESH_WORK_MANIFULATE);

	int n_mesh = m_pDataContext->m_MeshData.GetMeshCount();
	for (int i = 0; i < n_mesh; ++i)
	{
		MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(i);

		//if (WIN_MANAGER->vt_pckID[i])
		if (pMeshInfo && pMeshInfo->selected)
		{
			auto mesh = m_pDataContext->m_MeshData.GetMesh(i);

			if (mesh)
			{
				MESH_MANIPULATOR->CancelProcess(i);
			}
		}
	}

	CompleteProcess();
}

/*
@brief
@return
*/
void		CManpulateDlg::CompleteProcess()
{
	MESH_MODELVIEW_MANAGER->UpdatePivotPoint();

	auto view = WIN_MANAGER->mainMeshWidget->getMainView();
	view->renderLater();

	setVisible(false);

	MESH_WORK_MANAGER->setWorkMode(MESH_WORK_NONE);

	m_bFinish = true;

	m_bPatientCoordinate = false;
	m_p_cbSpace->setCurrentIndex(0);
}

/**
 * \brief 
 * 
*/
void CManpulateDlg::onPatientCoordinate(bool bInverse)
{
	if (m_bFinish || !m_pDataContext)
	{
		return;
	}

	mip::MeshTopology* pMesh = m_pDataContext->m_MeshData.GetCurrentMesh();
	int currentMeshIdx = m_pDataContext->m_MeshData.GetCurrentMeshIndex();

	if (pMesh)
	{
		ACTION_MANAGER->action_UndoRedo_update(m_pDataContext, MESH_WORK_MANIFULATE, MESH_WORK_MANIFULATE);

		double spaceZ = m_pDataContext->volume_data.getSpaceZ();

		const int nSlice = m_pDataContext->volume_data.getCZ();

		mip::MATRIX44 patientMat;
		patientMat.identity();

		float xdir[3];
		float ydir[3];

		m_pDataContext->volume_data.getImgOrientation(true, xdir);
		m_pDataContext->volume_data.getImgOrientation(false, ydir);

		float zdir[3];
		vtkMath::Cross(xdir, ydir, zdir);

		double position[3];

		position[0] = (double)m_pDataContext->volume_data.getImgPosX() * 0.1f;
		position[1] = (double)m_pDataContext->volume_data.getImgPosY() * 0.1f;
		position[2] = (double)m_pDataContext->volume_data.getImgPosZ() * 0.1f;

		for (int i = 0; i < 3; i++)
		{
			patientMat.m[i][0] = (double)xdir[i];
			patientMat.m[i][1] = (double)ydir[i];
			patientMat.m[i][2] = (double)zdir[i];
			patientMat.m[i][3] = position[i];
		}

		patientMat.m[3][3] = 1.0;

		//patientMat.m[0][3] -= zdir[0] * space[2] * (nSlice - 1);
		//patientMat.m[1][3] -= zdir[1] * space[2] * (nSlice - 1);
		patientMat.m[2][3] -= zdir[2] * spaceZ * (nSlice - 1);

		patientMat.transpose();

		if (bInverse)
		{
			patientMat.inverse();
		}

		pMesh->addRotate(patientMat.getQuaternion());
		pMesh->addTranslate(patientMat.getOrigin());

		UpdateMeshInfo(pMesh, MANIPULATOR_DIR_TYPE::TRANS_X_AXIS);
		UpdateMeshInfo(pMesh, MANIPULATOR_DIR_TYPE::ROTATE_X_AXIS);

		MESH_MANIPULATOR->AddTransformAxis(currentMeshIdx, patientMat);

		MESH_MODELVIEW_MANAGER->UpdatePivotPoint();

		m_bPatientCoordinate = true;
	}
}

/*
@brief
@return
*/
void CManpulateDlg::Reset()
{
	m_bFinish = false;
}

/*
@brief
@return
*/
void		CManpulateDlg::UpdateMeshInfo(
											mip::MeshTopology*		_p_mesh,
											MANIPULATOR_DIR_TYPE	_type
)
{
	mip::MATRIX44 mat	= _p_mesh->getMatrix();
	mip::VECTOR3 center = _p_mesh->m_boundingBox.getCenter() * _p_mesh->scale;

	mip::QUATERNION q = mat.getQuaternion();

	//center = q.rotateVector(center);
	//center += mat.getOrigin();
	center = mat.getOrigin();

	float roll, pitch, yaw;
	q.getYawPitchRoll(roll, pitch, yaw);

	auto scale_x = _p_mesh->scale.x;
	auto scale_y = _p_mesh->scale.y;
	auto scale_z = _p_mesh->scale.z;

	switch (_type)
	{
	case TRANS_X_AXIS:
	case TRANS_Y_AXIS:
	case TRANS_Z_AXIS:
	case TRANS_XY_AXIS:
	case TRANS_YZ_AXIS:
	case TRANS_XZ_AXIS:
		{
			m_p_lbTranslate[0]->setText(QString().sprintf("%.3f", center.x * 10.f));
			m_p_lbTranslate[1]->setText(QString().sprintf("%.3f", center.y * 10.f));
			m_p_lbTranslate[2]->setText(QString().sprintf("%.3f", center.z * 10.f));
		}
		break;
	case ROTATE_X_AXIS:
	case ROTATE_Y_AXIS:
	case ROTATE_Z_AXIS:
		{
			m_p_lbRotate[0]->setText(QString().sprintf("%.3f", yaw));
			m_p_lbRotate[1]->setText(QString().sprintf("%.3f", pitch));
			m_p_lbRotate[2]->setText(QString().sprintf("%.3f", roll));
		}
		break;
	case SCALE_X_AXIS:
	case SCALE_Y_AXIS:
	case SCALE_Z_AXIS:
	case SCALE_ALL_AXIS:
		{
			m_p_lbScale[0]->setText(QString().sprintf("%.3f", scale_x));
			m_p_lbScale[1]->setText(QString().sprintf("%.3f", scale_y));
			m_p_lbScale[2]->setText(QString().sprintf("%.3f", scale_z));
		}
		break;
	case NONE_AXIS:
		{
			m_p_lbTranslate[0]->setText(QString().sprintf("%.3f", center.x * 10.f));
			m_p_lbTranslate[1]->setText(QString().sprintf("%.3f", center.y * 10.f));
			m_p_lbTranslate[2]->setText(QString().sprintf("%.3f", center.z * 10.f));

			m_p_lbRotate[0]->setText(QString().sprintf("%.3f", yaw));
			m_p_lbRotate[1]->setText(QString().sprintf("%.3f", pitch));
			m_p_lbRotate[2]->setText(QString().sprintf("%.3f", roll));

			m_p_lbScale[0]->setText(QString().sprintf("%.3f", scale_x));
			m_p_lbScale[1]->setText(QString().sprintf("%.3f", scale_y));
			m_p_lbScale[2]->setText(QString().sprintf("%.3f", scale_z));
		}
	}
}