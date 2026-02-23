#include "stdafx.h"
#include "ReduceDlg.h"
#include "windowManager.h"
#include "Main/MainMeshWidget.h"
#include "MEVolumeView.h"

#include "MeshControl.h"
#include "System/styleManager.h"
#include "System/stringManager.h"

#include "Actions/ActionManager.h"
#include "ActionMesh.h"

#include "MeshEdit\CMeshWorkManager.h"
#include "DataContext.h"

class MainMeshWidget;

#define LE_WIDTH 65

ReduceDlg::ReduceDlg(DataContext* pDataContext, QWidget *parent /*= NULL*/)
	: MeshBaseDlg(pDataContext, parent),
	m_nReduceMode(0),
	m_nPercentage(50),
	m_nTriCount(50),
	m_fMaxDeviation(50),
	m_cPreserveBoundary(true),
	m_pCheck(nullptr),
	m_nMeshIndex(-1)
{
	setWindowTitle(QString("Reduce"));
	initQtUI();
}

ReduceDlg::ReduceDlg(DataContext* pDataContext, QString _title, QWidget *parent /*= NULL*/)
	: MeshBaseDlg(pDataContext, _title, parent),
	m_nReduceMode(0),
	m_nPercentage(50),
	m_nTriCount(50),
	m_fMaxDeviation(50),
	m_cPreserveBoundary(true),
	m_pCheck(nullptr),
	m_nMeshIndex(-1)
{	
	initQtUI();	
}

ReduceDlg::~ReduceDlg()
{

}

void ReduceDlg::initQtUI()
{
	initTopology();
	QWidget* container = new QWidget(this);
	QWidget* container2 = new QWidget(this);
	//
	QVBoxLayout *layoutV = new QVBoxLayout;
	QHBoxLayout *layoutH_Top = new QHBoxLayout(container);
	QHBoxLayout *layoutH_Top2 = new QHBoxLayout;
	QHBoxLayout *layoutH_0 = new QHBoxLayout;
	QHBoxLayout *layoutH_1 = new QHBoxLayout;
	QHBoxLayout *layoutH_2 = new QHBoxLayout;
	QHBoxLayout *layoutH_3 = new QHBoxLayout;
	QHBoxLayout *layoutH_4 = new QHBoxLayout;
	QHBoxLayout *layoutH_bottom = new QHBoxLayout;
	QVBoxLayout *layoutV_bottom_0 = new QVBoxLayout;
	QVBoxLayout *layoutV_bottom_1 = new QVBoxLayout;
	int LE_Width = LE_WIDTH;

	layoutV->addLayout(layoutH_Top);
	layoutV->addLayout(layoutH_Top2);
	layoutV->addLayout(layoutH_0);
	layoutV->addLayout(layoutH_1);
	layoutV->addLayout(layoutH_2);
	layoutV->addLayout(layoutH_3);
	layoutV->addLayout(layoutH_4);
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

	//
	QLabel *lb_H_0 = new QLabel(QString("Mode"), this);
	layoutH_0->addWidget(lb_H_0);

	int left = 11;
	QLabel *lb_Select = new QLabel(QString("Reduction"), this);
	lb_Select->setGeometry(left - 4, 5, 100, 20);
	lb_Select->setStyleSheet("background-color: #414141;");
	lb_Select->raise();

	//
	QComboBox * CB_Select_H_1 = new QComboBox(this);
	CB_Select_H_1->setObjectName(QString("ReduceCombo"));
	CB_Select_H_1->addItem(QString("Percentage"));
	CB_Select_H_1->addItem(QString("TriCount"));
	CB_Select_H_1->addItem(QString("MaxDeviation"));
	CB_Select_H_1->setStyleSheet(STYLE_MANAGER->comboBoxNormal);
	CB_Select_H_1->setFixedHeight(25);
	layoutH_1->addWidget(CB_Select_H_1);

	//Percentage
	double min = 1.0; double max = 100.0; int demical = 2; double initVal = 50.0;
	QLabel *lb_H_2_0 = new QLabel(QString("Percentage"), this);
	lb_H_2_0->setObjectName(QString("Percentage"));	
	lb_H_2_0->setAlignment(Qt::AlignLeft);
	layoutH_2->addWidget(lb_H_2_0);

	QLineEdit *LE_Size_H_2_0 = initLineEdit(this, QString("Percentage"), emLE_INT, min, max, (int)initVal, demical, LE_Width);
	layoutH_2->addWidget(LE_Size_H_2_0);

	_Slider sSlider0; 
	initSlider(this, QString("Percentage"), sSlider0, Qt::Horizontal, emLE_INT, min, max, initVal);
	layoutH_3->addWidget(sSlider0.pSlidier);	

	//TriCount	
	int TriCnt = mip::mesh_control::getTopologyTriCnt(m_pMT);
	min = 1.0; max = TriCnt; demical = 3; initVal = (int)(TriCnt * 0.5);
	QLabel *lb_H_2_1 = new QLabel(QString("TriCount"), this);
	lb_H_2_1->setObjectName(QString("TriCount"));
	lb_H_2_1->setAlignment(Qt::AlignLeft);
	layoutH_2->addWidget(lb_H_2_1);	

	QLineEdit *LE_Size_H_2_1 = initLineEdit(this, QString("TriCount"), emLE_INT, min, max, (int)initVal, demical, LE_Width);	
	layoutH_2->addWidget(LE_Size_H_2_1);

	_Slider sSlider1; 
	initSlider(this, QString("TriCount"), sSlider1, Qt::Horizontal, emLE_INT, min, max, initVal);
	layoutH_3->addWidget(sSlider1.pSlidier);	

	//MaxDeviation
	//double MaxDeviation = calcMaxDeviation(m_pMT);
	double MaxDeviation = 0.0;
	min = MaxDeviation; max = MaxDeviation + 1.0; demical = 3; initVal = MaxDeviation;
	QLabel *lb_H_2_2 = new QLabel(QString("MaxDeviation"), this);
	lb_H_2_2->setObjectName(QString("MaxDeviation"));
	lb_H_2_2->setAlignment(Qt::AlignLeft);
	layoutH_2->addWidget(lb_H_2_2);

	QLineEdit *LE_Size_H_2_2 = initLineEdit(this, QString("MaxDeviation"), emLE_DOUBLE, min, max, initVal, demical, LE_Width);
	layoutH_2->addWidget(LE_Size_H_2_2);

	_Slider sSlider2; 
	initSlider(this, QString("MaxDeviation"), sSlider2, Qt::Horizontal, emLE_DOUBLE, min, max, initVal);
	layoutH_3->addWidget(sSlider2.pSlidier);

	//CheckBox
	m_pCheck = new QCheckBox("Preserve Boundaries");
	m_pCheck->setChecked(true);
	m_pCheck->setStyleSheet(STYLE_MANAGER->m_Checkbox);
	layoutH_4->addWidget(m_pCheck);
	

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

	connect(btnOK, &QPushButton::clicked, this, &ReduceDlg::OnOK);
	connect(btnCancel, &QPushButton::clicked, this, &ReduceDlg::OnCancel);
	connect(CB_Select_H_1, SIGNAL(currentIndexChanged(int)), this, SLOT(OnComboChanged(int)));
	connect(LE_Size_H_2_0, &QLineEdit::editingFinished, this, &ReduceDlg::OnTextChanged);
	connect(LE_Size_H_2_0, &QLineEdit::returnPressed, this, &ReduceDlg::OnTextChanged);
	connect(sSlider0.pSlidier, &QSlider::valueChanged, this, &ReduceDlg::OnDepthSlideReleased);
	connect(LE_Size_H_2_1, &QLineEdit::editingFinished, this, &ReduceDlg::OnTextChanged);
	connect(LE_Size_H_2_1, &QLineEdit::returnPressed, this, &ReduceDlg::OnTextChanged);
	connect(sSlider1.pSlidier, &QSlider::valueChanged, this, &ReduceDlg::OnDepthSlideReleased);
	connect(LE_Size_H_2_2, &QLineEdit::editingFinished, this, &ReduceDlg::OnTextChanged);
	connect(LE_Size_H_2_2, &QLineEdit::returnPressed, this, &ReduceDlg::OnTextChanged);
	connect(sSlider2.pSlidier, &QSlider::valueChanged, this, &ReduceDlg::OnDepthSlideReleased);
	connect(m_pCheck, SIGNAL(clicked()), this, SLOT(OnClickCheckBox()));


	listLE.push_back(LE_Size_H_2_0);
	listLE.push_back(LE_Size_H_2_1);
	listLE.push_back(LE_Size_H_2_2);
	listSd.push_back(sSlider0);
	listSd.push_back(sSlider1);
	listSd.push_back(sSlider2);
	listCB.push_back(CB_Select_H_1);
	listLB.push_back(lb_H_2_0);
	listLB.push_back(lb_H_2_1);
	listLB.push_back(lb_H_2_2);

	setLayout(layoutV);
	hideFuncUI(QString("Percentage"));
	hideFuncUI(QString("TriCount"));
	hideFuncUI(QString("MaxDeviation"));
	showFuncUI(QString("Percentage"));
}

void ReduceDlg::initOptionValue(int Reduce_Method)
{
	if (m_pMT == nullptr) return;
	int triCnt = 0; 
	double Deviation = 0.0f;

	if (Reduce_Method == RM_MAXDEVIATION || Reduce_Method == 99)
	{
		Deviation = calcMaxDeviation(m_pMT);
	}	
	else if(Reduce_Method == RM_TRICOUNT || Reduce_Method == 99)
	{
		triCnt = mip::mesh_control::getTopologyTriCnt(m_pMT);
	}

	double min = 0.0; double max = 1.0; int initVal = 50.0;
	for (int ii = 0; ii < listSd.size(); ii++)
	{
		QString name = listSd[ii].pSlidier->objectName();
		if (name == QString("Percentage"))
		{
			listSd[ii].pSlidier->setMinimum(1);
			listSd[ii].pSlidier->setMaximum(100);
			listSd[ii].pSlidier->setValue(initVal);
			setPercentage(initVal);
		}
			
		if (name == QString("TriCount") )
		{
			listSd[ii].pSlidier->setMinimum(1);
			listSd[ii].pSlidier->setMaximum(triCnt);
			if (listSd[ii].type == emLE_INT)
			{
				listSd[ii].pSlidier->setValue(triCnt * 0.5);
				setTriCount(triCnt * 0.5);
			}

		}
			
		if (name == QString("MaxDeviation"))
		{
			listSd[ii].pSlidier->setValue(Deviation);
			setMaxDeviation(Deviation);
		}
			
	}
	
	for (int ii = 0; ii < listLE.size(); ii++)
	{
		QString name = listLE[ii]->objectName();
		if (name == QString("Percentage"))
			listLE[ii]->setText(QString::number(initVal)); 
		if (name == QString("TriCount"))		
			listLE[ii]->setText(QString::number((int)(triCnt * 0.5)));
		if (name == QString("MaxDeviation"))
			listLE[ii]->setText(QString::number(Deviation, 'f', 2));
	}
}


void ReduceDlg::reject(bool bForce)
{
	if (!m_pDataContext || !ACTION_MANAGER->isActionFinished() && !bForce) return;

	// 220121 허 건 과장
	ACTION_MANAGER->action_UndoRedo_update(m_pDataContext, MESH_WORK_NONE, MESH_WORK_MANAGER->getWorkMode());

	mip::MeshTopology *p_mesh = m_pDataContext->m_MeshData.GetMesh(m_nMeshIndex);

	int nPickMesh = m_pDataContext->m_MeshData.GetCurrentMeshIndex();
	mip::MeshTopology *mesh = m_pDataContext->m_MeshData.GetMesh(nPickMesh);

	if (p_mesh != nullptr)
		mesh = p_mesh;

	if (mesh && mesh->m_tverts.size() > 0)
	{
		printf_s("\n ReduceDlg::reject()");
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

double ReduceDlg::calcMaxDeviation(mip::MeshTopology* pMT)
{
	if (pMT == nullptr) return -1.0;

	double maxdeviation = mip::mesh_control::getMaxDeviation(pMT);
	return maxdeviation;
}

void ReduceDlg::OnTextChanged()
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
		if (name == QString("Percentage")) setPercentage(tmpVal);
		else if (name == QString("TriCount")) setTriCount(tmpVal);
		else if (name == QString("MaxDeviation")) setMaxDeviation(tmpVal);
	}

	sSlider.pSlidier->blockSignals(true);
	sSlider.pSlidier->setValue((tmpVal));
	sSlider.pSlidier->blockSignals(false);
}

void ReduceDlg::OnDepthSlideReleased(int val)
{
	double tmpVal = val;

	_Slider pSlider;
	for (int ii = 0; ii < listSd.size(); ii++)
	{
		_Slider _pSlider = listSd[ii];
		if (_pSlider.pSlidier->hasFocus())
		{
			pSlider = _pSlider;
			break;
		}
	}

	if (pSlider.pSlidier == nullptr) return;

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
	if(bDouble)
		pLineEdit->setText(QString::number(tmpVal, 'f', 2));
	else
		pLineEdit->setText(QString::number((int)tmpVal));
	pLineEdit->blockSignals(false);

	if (name == QString("Percentage")) setPercentage(tmpVal);
	else if (name == QString("TriCount")) setTriCount(tmpVal);
	else if (name == QString("MaxDeviation")) setMaxDeviation(tmpVal);
}

void ReduceDlg::OnComboChanged(int index)
{
	QComboBox* pCombo = nullptr;
	for (int ii = 0; ii < listCB.size(); ii++)
	{
		pCombo = listCB[ii];
		bool active = pCombo->hasFocus();

		if (pCombo->hasFocus()) break;
	}

	QString name = pCombo->objectName();
	if (index == RM_PERCENTAGE)
	{
		name = QString("Percentage");
		initOptionValue();
	}
	else if (index == RM_TRICOUNT)
	{
		name = QString("TriCount");
		initOptionValue(RM_TRICOUNT);
	}
	else if (index == RM_MAXDEVIATION)
	{
		name = QString("MaxDeviation");		
		initOptionValue(RM_MAXDEVIATION);
	}

	setReduceMode(index);

	hideFuncUI();
	showFuncUI(name);
}

void ReduceDlg::OnClickCheckBox()
{
	int aa = 0;
	if (m_pCheck->isChecked())
		m_cPreserveBoundary = true;
	else
		m_cPreserveBoundary = false;
}

void ReduceDlg::OnOK()
{
	MEVolumeView* pViewer = (MEVolumeView*)(this->parent());

	if (!pViewer || !m_pDataContext || !ACTION_MANAGER->isActionFinished()) return;
	int MeshIdx = m_pDataContext->m_MeshData.GetCurrentMeshIndex();
	
	WorkMeshRemesh* pWMR = ACTION_MANAGER->action_Mesh_Init(MESH_REDUCTION, MeshIdx, m_pDataContext, pViewer);
	if (pWMR == nullptr) return;

	double value = 0.0f;
	if (getReduceMode() == RM_PERCENTAGE) value = getPercentage();
	else if (getReduceMode() == RM_TRICOUNT) value = getTriCount();
	else if (getReduceMode() == RM_MAXDEVIATION) value = getMaxDeviation();

	pWMR->setOffset(value);
	pWMR->setReducePreserveBound(m_cPreserveBoundary);	
	pWMR->setReduceMethod(getReduceMode());
	ACTION_MANAGER->action_Mesh(pWMR);
}

void ReduceDlg::OnCancel()
{
	if (!ACTION_MANAGER->isActionFinished()) return;
	reject();
	close();
}

void ReduceDlg::hideFuncUI()
{
	for (int ii = 0; ii < listLB.size(); ii++)
		listLB[ii]->hide();

	for (int ii = 0; ii < listLE.size(); ii++)
		listLE[ii]->hide();

	for (int ii = 0; ii < listSd.size(); ii++)
		listSd[ii].pSlidier->hide();
}

void ReduceDlg::hideFuncUI(QString option)
{
	for (int ii = 0; ii < listLB.size(); ii++)
	{
		QString name = listLB[ii]->objectName();
		if (name == option)
			listLB[ii]->hide();
	}
		

	for (int ii = 0; ii < listLE.size(); ii++)
	{
		QString name = listLE[ii]->objectName();
		if (name == option)
			listLE[ii]->hide();
	}

	for (int ii = 0; ii < listSd.size(); ii++)
	{
		QString name = listLE[ii]->objectName();
		if (name == option)
			listSd[ii].pSlidier->hide();
	}
}

void ReduceDlg::showFuncUI(QString option)
{
	double min = 0.0; double max = 1.0; int initVal = 50.0;
	for (int ii = 0; ii < listSd.size(); ii++)
	{
		QString name = listSd[ii].pSlidier->objectName();
		if (name == option)
		{
			listSd[ii].pSlidier->show();
			max = listSd[ii].max;
			min = listSd[ii].min;
			initVal = listSd[ii].initVal;
		}
	}

	int LE_Width = LE_WIDTH;
	for (int ii = 0; ii < listLB.size(); ii++)
	{
		QString name = listLB[ii]->objectName();
		if (name == option)
			listLB[ii]->show();

		listLB[ii]->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	}

	for (int ii = 0; ii < listLE.size(); ii++)
	{
		QString name = listLE[ii]->objectName();
		if (name == option)
			listLE[ii]->show();
			
		listLE[ii]->setFixedWidth(LE_Width);
		listLE[ii]->setAlignment(Qt::AlignRight);
	}
}