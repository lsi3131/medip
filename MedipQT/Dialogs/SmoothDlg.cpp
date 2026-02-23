#include "stdafx.h"
#include "SmoothDlg.h"
#include "windowManager.h"
#include "Main/MainMeshWidget.h"
#include "MEVolumeView.h"

#include "System/styleManager.h"
#include "System/stringManager.h"

#include "Actions/ActionManager.h"
#include "ActionMesh.h"

#include "MeshEdit/CMeshWorkManager.h"

#include "DataContext.h"

class MainMeshWidget;

SmoothDlg::SmoothDlg(DataContext* pDataContext, QWidget *parent /*= NULL*/)	
	: MeshBaseDlg(pDataContext, parent),
	 m_nSmoothMode(0),
	 m_nOffset(1),
	 m_nStrength(50),
	 m_nMeshIndex(-1),
	 m_pComboBox(nullptr)
{
	setWindowTitle(QString("Smooth"));
	initQtUI();
}

SmoothDlg::SmoothDlg(DataContext* pDataContext, QString _title, QWidget *parent /*= NULL*/)
	: MeshBaseDlg(pDataContext, _title, parent),
	m_nSmoothMode(0),
	m_nOffset(50),
	m_nStrength(50),
	m_pComboBox(nullptr)
{	
	initQtUI();
}

SmoothDlg::~SmoothDlg()
{

}

void SmoothDlg::updateDialog()
{
	if (m_pComboBox != nullptr && m_pMT != nullptr)
	{
		if(mip::mesh_control::isTriFlag(m_pMT, mip::SELECTED))
		{ 
			QListView* view = qobject_cast<QListView *>(m_pComboBox->view());
			view->setRowHidden(1, true);	//hide HC-Laplacian
		}
		else
		{
			QListView* view = qobject_cast<QListView *>(m_pComboBox->view());
			view->setRowHidden(1, false);	//hide HC-Laplacian
		}
	}	
}

void SmoothDlg::initQtUI()
{
	QWidget* container = new QWidget(this);
	QWidget* container2 = new QWidget(this);

	QVBoxLayout *layoutV = new QVBoxLayout;
	QHBoxLayout *layoutH_Top = new QHBoxLayout(container);
	QHBoxLayout *layoutH_Top2 = new QHBoxLayout;
	QHBoxLayout *layoutH_0 = new QHBoxLayout;
	QHBoxLayout *layoutH_1 = new QHBoxLayout;
	QHBoxLayout *layoutH_2 = new QHBoxLayout;
	QHBoxLayout *layoutH_3 = new QHBoxLayout;
	QHBoxLayout *layoutH_bottom = new QHBoxLayout;
	QVBoxLayout *layoutV_bottom_0 = new QVBoxLayout;
	QVBoxLayout *layoutV_bottom_1 = new QVBoxLayout;
	int LE_Width = 40;

	layoutV->addLayout(layoutH_Top);
	layoutV->addLayout(layoutH_Top2);
	layoutV->addLayout(layoutH_0);
	layoutV->addLayout(layoutH_1);
	layoutV->addLayout(layoutH_2);
	layoutV->addLayout(layoutH_3);
	layoutV->addLayout(layoutH_bottom);

	QLabel *lb_H_Top = new QLabel(QString(""), this);
	QLabel *lb_H_Top2 = new QLabel(QString(""), this);
	lb_H_Top2->setStyleSheet("background-color: #414141;");

	layoutH_Top2->setAlignment(Qt::AlignLeft | Qt::AlignTop);
	container->setStyleSheet("background-color: #414141;");
	QRect rect2 = container->geometry();
	rect2.setWidth(200);
	container->setGeometry(rect2);
	container2->setStyleSheet("background-color: #414141;");
	layoutH_Top->addWidget(lb_H_Top);
	layoutH_Top2->addWidget(lb_H_Top2);

	//QLabel *lb_H_Top = new QLabel(QString("Smooth"), this);
	//layoutH_Top->addWidget(lb_H_Top);

	//
	QLabel *lb_H_0 = new QLabel(QString("Mode"), this);
	layoutH_0->addWidget(lb_H_0);

	int left = 11;
	QLabel *lb_Select = new QLabel(QString("Smooth"), this);
	lb_Select->setGeometry(left - 4, 5, 100, 20);
	lb_Select->setStyleSheet("background-color: #414141;");
	lb_Select->raise();

	//
	QComboBox * CB_Select_H_1 = new QComboBox(this);
	CB_Select_H_1->setObjectName(QString("Smooth"));
	CB_Select_H_1->addItem(QString("Laplacian"));
	CB_Select_H_1->addItem(QString("HC-Laplacian"));
	CB_Select_H_1->addItem(QString("Taubin-Laplacian"));
	CB_Select_H_1->setStyleSheet(STYLE_MANAGER->comboBoxNormal);
	CB_Select_H_1->setFixedHeight(25);
	m_pComboBox = CB_Select_H_1;


	
	layoutH_1->addWidget(CB_Select_H_1);

	//
	double min = 0.0; double max = 1.0; int demical = 2; int initVal = 1.0;
	QLabel *lb_H_2 = new QLabel(QString("Offset"), this);
	layoutH_2->addWidget(lb_H_2);
	QLineEdit *LE_Size_H_2 = initLineEdit(this, QString("Offset"), emLE_DOUBLE, min, max, 1.0, demical, LE_Width);
	layoutH_2->addWidget(LE_Size_H_2);

	_Slider sSlider; 
	initSlider(this, QString("Offset"), sSlider, Qt::Horizontal, emLE_DOUBLE, min, max, initVal);

	layoutH_3->addWidget(sSlider.pSlidier);

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

	connect(btnOK, &QPushButton::clicked, this, &SmoothDlg::OnOK);
	connect(btnCancel, &QPushButton::clicked, this, &SmoothDlg::OnCancel);
	connect(CB_Select_H_1, SIGNAL(currentIndexChanged(int)), this, SLOT(OnComboChanged(int)));
	connect(LE_Size_H_2, &QLineEdit::editingFinished, this, &SmoothDlg::OnTextChanged);
	connect(LE_Size_H_2, &QLineEdit::returnPressed, this, &SmoothDlg::OnTextChanged);
	connect(sSlider.pSlidier, &QSlider::valueChanged, this, &SmoothDlg::OnDepthSlideReleased);

	listLE.push_back(LE_Size_H_2);
	listSd.push_back(sSlider);
	listCB.push_back(CB_Select_H_1);

	setLayout(layoutV);
}


void SmoothDlg::reject(bool bForce)
{
	if (!m_pDataContext || !ACTION_MANAGER->isActionFinished() && !bForce) return;

	// 220121 허 건 과장
	ACTION_MANAGER->action_UndoRedo_update(m_pDataContext, MESH_WORK_NONE, MESH_WORK_MANAGER->getWorkMode());

	FinishProcess();
}

void		SmoothDlg::FinishProcess()
{
	mip::MeshTopology *p_mesh = m_pDataContext->m_MeshData.GetMesh(m_nMeshIndex);

	int nPickMesh = m_pDataContext->m_MeshData.GetCurrentMeshIndex();
	mip::MeshTopology *mesh = m_pDataContext->m_MeshData.GetMesh(nPickMesh);

	if (p_mesh != nullptr)
		mesh = p_mesh;

	if (mesh && mesh->m_tverts.size() > 0)
	{
		printf_s("\n SmoothDlg::reject()");
		WIN_MANAGER->mainMeshWidget->getMainView()->clearSelectFlag(mesh);
		mesh->updateColor(mesh->m_baseColor);
		WIN_MANAGER->buildRenderBufferTopology(mesh);
		//mesh
		auto view = WIN_MANAGER->mainMeshWidget->getMainView();
		view->update();
	}

	setWindowOpacity(0.1);
	setVisible(false);

	MESH_WORK_MANAGER->UpdateWorkMode(MESH_WORK_NONE, true);

	m_pMT = nullptr;
	m_nMeshIndex = -1;
	close();
}

void SmoothDlg::OnTextChanged()
{	
	QLineEdit* pLineEdit = nullptr;
	for (int ii = 0; ii < listLE.size(); ii++)
	{
		pLineEdit = listLE[ii];
		if (pLineEdit->isModified()) break;
	}

	double tmpVal;	
	QString text = pLineEdit->text();
	QString name = pLineEdit->objectName();	
	if ( dynamic_cast<const QDoubleValidator*>(pLineEdit->validator()) != nullptr )
		tmpVal = text.toDouble();
	else
		tmpVal = text.toInt();


	//find Slider
	_Slider sSlider;
	for (int ii = 0; ii < listSd.size(); ii++)
	{
		sSlider = listSd[ii];
		if (sSlider.pSlidier == nullptr) continue;
		if (name == sSlider.pSlidier->objectName()) break;
	}

	pLineEdit->blockSignals(true);

	if (tmpVal < sSlider.min)
		tmpVal = sSlider.min;
	else if (tmpVal > sSlider.max)
		tmpVal = sSlider.max;	
	
	if (QString::number(tmpVal) != text)
		pLineEdit->setText(QString::number(tmpVal));

	pLineEdit->blockSignals(false);

	if (dynamic_cast<const QDoubleValidator*>(pLineEdit->validator()) != nullptr)
	{
		tmpVal -= sSlider.min;
		tmpVal /= (sSlider.max - sSlider.min);
		tmpVal *= 100;
	}

	{
		if (name == QString("Strength")) setStrength(tmpVal);
		else if (name == QString("Offset")) setOffset(tmpVal);
	}

	sSlider.pSlidier->blockSignals(true);
	sSlider.pSlidier->setValue((tmpVal));
	sSlider.pSlidier->blockSignals(false);
}

void SmoothDlg::OnDepthSlideReleased(int val)
{
	double tmpVal = val;

	_Slider pSlider;
	for (int ii = 0; ii < listSd.size(); ii++)
	{
		pSlider = listSd[ii];
		if (pSlider.pSlidier->hasFocus()) break;
	}

	QString name = pSlider.pSlidier->objectName();

	//QLineEdit
	QLineEdit* pLineEdit = nullptr;
	for (int ii = 0; ii < listLE.size(); ii++)
	{
		pLineEdit = listLE[ii];
		if (name == pLineEdit->objectName()) break;
	}	

	if (dynamic_cast<const QDoubleValidator*>(pLineEdit->validator()) != nullptr)
	{
		tmpVal /= 100;
		tmpVal *= (pSlider.max - pSlider.min);
		tmpVal += pSlider.min;
	}

	pLineEdit->blockSignals(true);
	pLineEdit->setText(QString::number(tmpVal));
	pLineEdit->blockSignals(false);

	if (name == QString("Strength")) setStrength(tmpVal);
	else if (name == QString("Offset")) setOffset(tmpVal);
}

void SmoothDlg::OnComboChanged(int index)
{
	QComboBox* pCombo = nullptr;
	for (int ii = 0; ii < listCB.size(); ii++)
	{
		pCombo = listCB[ii];
		bool active = pCombo->hasFocus();

		if (pCombo->hasFocus()) break;
	}

	QString name = pCombo->objectName();

	setSmoothMode(index);
	if (name == QString("Laplacian")){}
	else if (name == QString("HC-Laplacian")){}
	else if (name == QString("Taubin-Laplacian")){}
}

void SmoothDlg::OnOK()
{
	MEVolumeView* pViewer = (MEVolumeView*)(this->parent());

	if (!ACTION_MANAGER->isActionFinished() || !m_pDataContext || !pViewer) return;
	int MeshIdx = m_pDataContext->m_MeshData.GetCurrentMeshIndex();
	
	WorkMeshRemesh* pWMR = ACTION_MANAGER->action_Mesh_Init(MESH_SMOOTH, MeshIdx, m_pDataContext, pViewer, getOffset());
	if (pWMR == nullptr) return;

	pWMR->setSmoothMethod(getSmoothMode());
	pWMR->setOffset(getOffset());
	ACTION_MANAGER->action_Mesh(pWMR);
}

void SmoothDlg::OnCancel()
{
	if (!ACTION_MANAGER->isActionFinished()) return;
	reject();
	close();
}