#include "stdafx.h"
#include "MeshOffsetDlg.h"
#include "windowManager.h"
#include "Main/MainMeshWidget.h"
#include "MEVolumeView.h"

#include "MeshControl.h"
#include "System/styleManager.h"
#include "System/stringManager.h"
#include "MeshEdit/CMeshViewRenderManager.h"
#include "MeshEdit/CMeshWorkManager.h"

#include "Actions/ActionManager.h"
#include "ActionMesh.h"

#include "DataContext.h"

MeshOffsetDlg::MeshOffsetDlg(DataContext* pDataContext, QWidget *parent /*= NULL*/)
	: MeshBaseDlg(pDataContext, parent),
	m_dOffsetDistance(-2.0),
	m_nEdgeLength(0.0)
{
	setWindowTitle(QString("Mesh Offset"));
	initQtUI();
}

MeshOffsetDlg::MeshOffsetDlg(DataContext* pDataContext, QString _title, QWidget *parent /*= NULL*/)
	: MeshBaseDlg(pDataContext, _title, parent),
	m_dOffsetDistance(-2.0),
	m_nEdgeLength(0.0)
{
	initQtUI();
}

MeshOffsetDlg::~MeshOffsetDlg()
{

}

void MeshOffsetDlg::closeEvent(QCloseEvent *)
{
	if (!ACTION_MANAGER->isActionFinished()) return;

	exitDialog();
}


void MeshOffsetDlg::reject(bool bForce)
{
	if (!ACTION_MANAGER->isActionFinished() && !bForce) return; //20210928_byPHS_기능수행중 Reject 안되게 수정

	QString _title = windowTitle();
	printf_s("\n %s ::reject()", _title.toStdString().c_str());
	setWindowOpacity(0.1);
	setVisible(false);

	MESH_WORK_MANAGER->UpdateWorkMode(MESH_WORK_NONE, true);
	close();
}


void MeshOffsetDlg::initQtUI()
{
	initTopology();
	QWidget* container = new QWidget(this);
	//
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
	int LE_Width = 40;

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

	QLabel *lb_H_Top = new QLabel(QString(""), this);
	lb_H_Top->setStyleSheet("background-color: #414141;");
	layoutH_Top->addWidget(lb_H_Top);

	//
	QLabel *lb_H_0 = new QLabel(QString("Mesh Offset"), this);
	layoutH_0->addWidget(lb_H_0);
	lb_H_0->setStyleSheet("background-color: #414141;");

	//Offset
	double min = -4.0; double max = 4.0; int demical = 2; double initVal = 0.0;
	QLabel *lb_H_2_0 = new QLabel(QString("Offset Distance"), this);
	lb_H_2_0->setObjectName(QString("Offset"));
	lb_H_2_0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	layoutH_1->addWidget(lb_H_2_0);

	QLineEdit *LE_Size_H_2_0 = initLineEdit(this, QString("Offset"), emLE_DOUBLE, min, max, initVal, demical, LE_Width);
	layoutH_1->addWidget(LE_Size_H_2_0);

	_Slider sSlider0;
	initSlider(this, QString("Offset"), sSlider0, Qt::Horizontal, emLE_DOUBLE, min, max, initVal);
	layoutH_2->addWidget(sSlider0.pSlidier);

	//EdgeLength
	min = 0.0; max = 1.0; demical = 2; initVal = 0.0;
	QLabel *lb_H_2_1 = new QLabel(QString("Mesh edgeLength"), this);
	lb_H_2_1->setObjectName(QString("EdgeLength"));
	lb_H_2_1->setAlignment(Qt::AlignLeft);
	lb_H_2_1->hide();
	layoutH_3->addWidget(lb_H_2_1);

	//QLineEdit *LE_Size_H_2_1 = initLineEdit(this, QString("EdgeLength"), emLE_DOUBLE, min, max, initVal, demical, LE_Width);
	//LE_Size_H_2_1->hide();
	//layoutH_3->addWidget(LE_Size_H_2_1);

	//_Slider sSlider1;
	//initSlider(this, QString("EdgeLength"), sSlider1, Qt::Horizontal, emLE_DOUBLE, min, max, initVal);
	//sSlider1.pSlidier->hide();
	//layoutH_4->addWidget(sSlider1.pSlidier);

	//QPushButton *btnUpdate = new QPushButton(this);
	//btnUpdate->setText("Update Hollow");
	//btnUpdate->setFixedHeight(30);
	////btnUpdate->setFixedSize(30, 30);
	//btnUpdate->setVisible(true);
	//btnUpdate->setStyleSheet(STYLE_MANAGER->buttonNormal);
	//layoutH_6->addWidget(btnUpdate);

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

	connect(btnOK, &QPushButton::clicked, this, &MeshOffsetDlg::OnOK);
	connect(btnCancel, &QPushButton::clicked, this, &MeshOffsetDlg::OnCancel);
	//connect(LE_Size_H_2_0, &QLineEdit::editingFinished, this, &MeshOffsetDlg::OnTextChanged);
	//connect(LE_Size_H_2_0, &QLineEdit::returnPressed, this, &MeshOffsetDlg::OnTextChanged);
	connect(LE_Size_H_2_0, &QLineEdit::textChanged, this, &MeshOffsetDlg::OnTextChanged);
	connect(sSlider0.pSlidier, &QSlider::valueChanged, this, &MeshOffsetDlg::OnDepthSlideReleased);
	//connect(LE_Size_H_2_1, &QLineEdit::editingFinished, this, &MeshOffsetDlg::OnTextChanged);
	//connect(LE_Size_H_2_1, &QLineEdit::returnPressed, this, &MeshOffsetDlg::OnTextChanged);
	//connect(sSlider1.pSlidier, &QSlider::valueChanged, this, &MeshOffsetDlg::OnDepthSlideReleased);
	//connect(btnUpdate, &QPushButton::clicked, this, &MeshOffsetDlg::OnUpdateHollow);

	listLE.push_back(LE_Size_H_2_0);
	//listLE.push_back(LE_Size_H_2_1);
	listSd.push_back(sSlider0);
	//listSd.push_back(sSlider1);

	setLayout(layoutV);
}



double MeshOffsetDlg::calcEdgeLength(mip::MeshTopology* pMT)
{
	return 0.0f;
}

void MeshOffsetDlg::exitDialog()
{
	mip::MeshTopology* pTempMesh = MESH_RENDER_MANAGER->getTempMesh();
	mip::MeshTopology* pTempParents = MESH_RENDER_MANAGER->getTempParents();
	if (pTempParents != nullptr && pTempParents->m_ttris.size() > 0)
	{
		pTempParents->initShader(mip::SHADERTYPE::SHADER_PONG);
		WIN_MANAGER->buildRenderBufferTopology(pTempParents);
	}
	MESH_WORK_MANAGER->setWorkMode(MESH_WORK_NONE);
	MESH_RENDER_MANAGER->delTempMesh();

	float fOffset = 0.0f;
	int  size = 6;
	setOffsetDist(fOffset);

	setSliderValue(QString("Offset"), fOffset);
	setLineEditValue(QString("Offset"), fOffset);
}

void MeshOffsetDlg::OnTextChanged()
{
	QLineEdit* pLineEdit = nullptr;
	for (int ii = 0; ii < listLE.size(); ii++)
	{
		pLineEdit = listLE[ii];
		QString str = pLineEdit->objectName();
		if (pLineEdit->isModified()) break;
	}

	double tmpVal;
	QString text = pLineEdit->text();
	QString name = pLineEdit->objectName();
	if (dynamic_cast<const QDoubleValidator*>(pLineEdit->validator()) != nullptr)
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
		if (name == QString("Offset")) setOffsetDist(tmpVal);
		else if (name == QString("EdgeLength")) setEdgeLength(tmpVal);
	}

	sSlider.pSlidier->blockSignals(true);
	sSlider.pSlidier->setValue((tmpVal));
	sSlider.pSlidier->blockSignals(false);
}

void MeshOffsetDlg::OnDepthSlideReleased(int val)
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
	bool bDouble = dynamic_cast<const QDoubleValidator*>(pLineEdit->validator()) != nullptr;
	if (bDouble)
	{
		tmpVal /= 100;
		tmpVal *= (pSlider.max - pSlider.min);
		tmpVal += pSlider.min;
	}

	pLineEdit->blockSignals(true);
	if (bDouble)
		pLineEdit->setText(QString::number(tmpVal));
	else
		pLineEdit->setText(QString::number((int)tmpVal));
	pLineEdit->blockSignals(false);

	if (name == QString("Offset"))
	{
		setOffsetDist(tmpVal);
	}
	else if (name == QString("EdgeLength")) setEdgeLength(tmpVal);
}

void MeshOffsetDlg::OnComboChanged(int index)
{
	QComboBox* pCombo = nullptr;
	for (int ii = 0; ii < listCB.size(); ii++)
	{
		pCombo = listCB[ii];
		bool active = pCombo->hasFocus();

		if (pCombo->hasFocus()) break;
	}

	QString name = pCombo->objectName();
	if (index == RM_PERCENTAGE) name = QString("Offset");
	else if (index == RM_TRICOUNT) name = QString("EdgeLength");
}

void MeshOffsetDlg::OnUpdateHollow()
{
	int MeshIdx = m_pDataContext->m_MeshData.GetCurrentMeshIndex();

	//WorkMeshRemesh* pWMR = ACTION_MANAGER->action_Mesh_Init(MESH_HOLLOW, MeshIdx);
	//if (pWMR == nullptr) return;

	//float offset = getOffsetDist();
	//if (offset < -4.0f) offset = -4.0f;
	//else if (offset > 4.0f)	offset = 4.0f;

	//if (offset == 0.f)
	//{
	//	return;
	//}

	//pWMR->setOffset(offset);
	//pWMR->setEdgeLength(getEdgeLength());
	//ACTION_MANAGER->action_Mesh(pWMR);
}


void MeshOffsetDlg::OnOK()
{
	if (!ACTION_MANAGER->isActionFinished()) return;



	close();
}

void MeshOffsetDlg::OnCancel()
{
	if (!ACTION_MANAGER->isActionFinished()) return;

	//this->OnReleased();
	exitDialog();
	close();
}
