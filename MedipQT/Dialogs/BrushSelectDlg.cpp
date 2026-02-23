#include "stdafx.h"
#include "BrushSelectDlg.h"
#include "windowManager.h"
#include "Main/MainMeshWidget.h"
#include "MEVolumeView.h"
#include "MedipQT.h"

#include "System/styleManager.h"
#include "System/stringManager.h"
#include "System/resourceManager.h"
#include "Renderer\Renderer.h"

#include "MeshEdit\CMeshViewBtn3DScene.h"
#include "MeshEdit\CMeshWorkManager.h"
#include "MeshEdit\CMeshDlgManager.h"
#include "DataContext.h"


//#define SELECT_REMESH_MODE //> 211104 Select Remesh 구현 완료전까지 비활성화


BrushSelectDlg::BrushSelectDlg(DataContext* pDataContext, QWidget *parent /*= NULL*/)
	: MeshBaseDlg(pDataContext, parent),
	m_nSculptMode(SM_SMOOTH),
	m_nSelectMode(SM_VOLUME),
	m_nSize(50),
	m_nStrength(50),
	m_pSurfaceBtn(nullptr),
	m_pVolumeBtn(nullptr),
	m_pSmoothBtn(nullptr),
	m_pInflateBtn(nullptr),
	m_pReduceBtn(nullptr),
	m_pRemeshBtn(nullptr),
	m_TTriCnt(0),
	m_TVertCnt(0),
	m_pCurMesh(nullptr),
	m_old_pick_idx(-1),
	m_bMoveDlg(false)
{
	setWindowTitle(QString("Select"));
	initQtUI();
}

void BrushSelectDlg::initQtUI()
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
	QHBoxLayout *layoutH_4 = new QHBoxLayout;
	QHBoxLayout *layoutH_5 = new QHBoxLayout;
	QHBoxLayout *layoutH_6 = new QHBoxLayout;
	//QHBoxLayout *layoutH_7 = new QHBoxLayout;
	QHBoxLayout *layoutH_bottom = new QHBoxLayout;
	int LE_Width = 50;

	layoutV->addLayout(layoutH_Top);
	layoutV->addLayout(layoutH_Top2);
	layoutV->addLayout(layoutH_0);
	layoutV->addLayout(layoutH_1);
	layoutV->addLayout(layoutH_2);
	layoutV->addLayout(layoutH_3);
	layoutV->addLayout(layoutH_4);
	layoutV->addLayout(layoutH_5);
	//layoutV->addLayout(layoutH_6);
	//layoutV->addLayout(layoutH_7);
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


	QLabel *lb_H_0 = new QLabel(QString(" "), this);
	layoutH_0->addWidget(lb_H_0);

	int left = 11;
	QLabel *lb_Mode = new QLabel(QString("Mode"), this);
	lb_Mode->setGeometry(left, 31, 100, 30);
	lb_Mode->raise();

	QLabel *lb_Select = new QLabel(QString("Select"), this);
	lb_Select->setGeometry(left - 4, 5, 100, 20);
	lb_Select->setStyleSheet("background-color: #414141;");
	lb_Select->raise();

	// "MODE"
	layoutH_1->setDirection(QBoxLayout::RightToLeft);
	layoutH_1->addStretch();
	layoutH_1->addStretch();
	layoutH_1->addStretch();

	m_pVolumeBtn = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_BRUSH_OP_VOLUME, 0, 0), "", this);
	m_pVolumeBtn->setObjectName(QString("Volume"));
	m_pVolumeBtn->setIconSize(QSize(72, 34));
	m_pVolumeBtn->setFixedSize(QSize(72, 34));
	m_pVolumeBtn->setCheckable(true);
	m_pVolumeBtn->setMouseTracking(true);
	m_pVolumeBtn->installEventFilter(this);
	m_pVolumeBtn->setStyleSheet("border: none;");
	layoutH_1->addWidget(m_pVolumeBtn);
	_Button btn2(m_pVolumeBtn, ICON_NON_BRUSH_OP_VOLUME, ICON_BRUSH_OP_VOLUME_HOVER, ICON_BRUSH_OP_VOLUME);

	m_pSurfaceBtn = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_BRUSH_OP_SURFACE, 0, 0), "", this);
	m_pSurfaceBtn->setObjectName(QString("Surface"));
	m_pSurfaceBtn->setIconSize(QSize(72, 34));
	m_pSurfaceBtn->setFixedSize(QSize(72, 34));
	m_pSurfaceBtn->setCheckable(true);
	m_pSurfaceBtn->setMouseTracking(true);
	m_pSurfaceBtn->installEventFilter(this);
	m_pSurfaceBtn->setStyleSheet("border: none;");
	layoutH_1->addWidget(m_pSurfaceBtn);
	_Button btn(m_pSurfaceBtn, ICON_NON_BRUSH_OP_SURFACE, ICON_BRUSH_OP_SURFACE_HOVER, ICON_BRUSH_OP_SURFACE);

	
	lb_H_2 = new QLabel(QString("Function"), this);
	layoutH_2->addWidget(lb_H_2);

	 //"Function"
	layoutH_3->setDirection(QBoxLayout::RightToLeft);
	layoutH_3->addStretch();
	layoutH_3->addStretch();
	layoutH_3->addStretch();

#ifdef SELECT_REMESH_MODE 
	m_pRemeshBtn = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_SCULPT_OP_REDUCE, 0, 0), "", this);
	m_pRemeshBtn->setObjectName(QString("Brush_select_remesh"));
	m_pRemeshBtn->setIconSize(QSize(32, 32));
	m_pRemeshBtn->setFixedSize(QSize(32, 32));
	m_pRemeshBtn->setCheckable(true);
	m_pRemeshBtn->setMouseTracking(true);
	m_pRemeshBtn->installEventFilter(this);
	m_pRemeshBtn->setStyleSheet("border: none;");
	m_pRemeshBtn->setToolTip(QString("Remesh"));
	layoutH_3->addWidget(m_pRemeshBtn);
	_Button btn6(m_pRemeshBtn, ICON_NON_SCULPT_OP_REDUCE, ICON_SCULPT_OP_REDUCE_HOVER, ICON_SCULPT_OP_REDUCE);
#endif

	m_pReduceBtn = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_SCULPT_OP_REDUCE, 0, 0), "", this);
	m_pReduceBtn->setObjectName(QString("Surface"));
	m_pReduceBtn->setIconSize(QSize(32, 32));
	m_pReduceBtn->setFixedSize(QSize(32, 32));
	m_pReduceBtn->setCheckable(true);
	m_pReduceBtn->setMouseTracking(true);
	m_pReduceBtn->installEventFilter(this);
	m_pReduceBtn->setStyleSheet("border: none;");
	m_pReduceBtn->setToolTip(QString("Reduce"));
	layoutH_3->addWidget(m_pReduceBtn);
	_Button btn5(m_pReduceBtn, ICON_NON_SCULPT_OP_REDUCE, ICON_SCULPT_OP_REDUCE_HOVER, ICON_SCULPT_OP_REDUCE);

	m_pSmoothBtn = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_SCULPT_OP_SMOOTH, 0, 0), "", this);
	m_pSmoothBtn->setObjectName(QString("Smooth"));
	m_pSmoothBtn->setIconSize(QSize(32, 32));
	m_pSmoothBtn->setFixedSize(QSize(32, 32));
	m_pSmoothBtn->setCheckable(true);
	m_pSmoothBtn->setMouseTracking(true);
	m_pSmoothBtn->installEventFilter(this);
	m_pSmoothBtn->setStyleSheet("border: none;");
	m_pSmoothBtn->setToolTip(QString("Smooth"));
	layoutH_3->addWidget(m_pSmoothBtn);
	_Button btn3(m_pSmoothBtn, ICON_NON_SCULPT_OP_SMOOTH, ICON_SCULPT_OP_SMOOTH_HOVER, ICON_SCULPT_OP_SMOOTH);

	//m_pInflateBtn = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_SCULPT_OP_INFLATE, 0, 0), "", this);
	//m_pInflateBtn->setObjectName(QString("Surface"));
	//m_pInflateBtn->setIconSize(QSize(32, 32));
	//m_pInflateBtn->setFixedSize(QSize(32, 32));
	//m_pInflateBtn->setCheckable(true);
	//m_pInflateBtn->setMouseTracking(true);
	//m_pInflateBtn->installEventFilter(this);
	//m_pInflateBtn->setStyleSheet("border: none;");
	//m_pInflateBtn->setToolTip(QString("Inflate/Deflate"));
	//layoutH_3->addWidget(m_pInflateBtn);
	//_Button btn4(m_pInflateBtn, ICON_NON_SCULPT_OP_INFLATE, ICON_SCULPT_OP_INFLATE_HOVER, ICON_SCULPT_OP_INFLATE);

	// "SIZE"
	QLabel *lb_H_4 = new QLabel(QString("Size"), this);
	layoutH_4->addWidget(lb_H_4);

	double min = 1.0; double max = 100.0; int demical = 2; double initVal = 50.0;
	QLineEdit *LE_Size_H_4 = initLineEdit(this, QString("Size"), emLE_INT, min, max, (int)initVal, demical, LE_Width);
	layoutH_4->addWidget(LE_Size_H_4);

	_Slider sSlider0;
	initSlider(this, QString("Size"), sSlider0, Qt::Horizontal, emLE_INT, min, max, initVal);
	layoutH_5->addWidget(sSlider0.pSlidier);

	// "Strength"
	//QLabel *lb_H_6 = new QLabel(QString("Strength"), this);
	//layoutH_6->addWidget(lb_H_6);

	//QLineEdit *LE_Strength_H_6 = initLineEdit(this, QString("Strength"), emLE_INT, min, max, (int)initVal, demical, LE_Width);
	//layoutH_6->addWidget(LE_Strength_H_6);

	//_Slider sSlider1;
	//initSlider(this, QString("Strength"), sSlider1, Qt::Horizontal, emLE_INT, min, max, initVal);
	//layoutH_7->addWidget(sSlider1.pSlidier);

	//
	QPushButton *btnClear = new QPushButton(this);
	btnClear->setText("Clear Select");
	btnClear->setFixedSize(180, 30);
	btnClear->setVisible(true);
	btnClear->setStyleSheet(STYLE_MANAGER->buttonNormal);
	layoutH_bottom->addWidget(btnClear);

	QPushButton *btnOK = new QPushButton(this);
	btnOK->setText(QString("Apply"));
	btnOK->setFixedSize(30, 30);
	btnOK->setVisible(false);
	btnOK->setStyleSheet(STYLE_MANAGER->buttonNormal);
	//layoutH_bottom->addWidget(btnOK);

	QPushButton *btnCancel = new QPushButton(this);
	btnCancel->setText(STRING_MANAGER->getString(STR_CANCEL));
	btnCancel->setFixedSize(60, 30);
	btnCancel->setVisible(false);
	btnCancel->setStyleSheet(STYLE_MANAGER->buttonNormal);
	//layoutH_bottom->addWidget(btnCancel);

	connect(btnOK, &QPushButton::clicked, this, &BrushSelectDlg::OnOK);
	connect(btnCancel, &QPushButton::clicked, this, &BrushSelectDlg::OnCancel);
	connect(btnClear, &QPushButton::clicked, this, &BrushSelectDlg::OnClearFlag);
	connect(LE_Size_H_4, &QLineEdit::textChanged, this, &BrushSelectDlg::OnTextChanged);
	//connect(LE_Strength_H_6, &QLineEdit::textChanged, this, &BrushSelectDlg::OnTextChanged);

	connect(sSlider0.pSlidier, &QSlider::valueChanged, this, &BrushSelectDlg::OnDepthSlideReleased);
	//connect(sSlider1.pSlidier, &QSlider::valueChanged, this, &BrushSelectDlg::OnDepthSlideReleased);

	connect(m_pSurfaceBtn, &QPushButton::pressed, this, &BrushSelectDlg::OnPressed);
	connect(m_pSurfaceBtn, &QPushButton::released, this, &BrushSelectDlg::OnReleased);
	connect(m_pVolumeBtn, &QPushButton::pressed, this, &BrushSelectDlg::OnPressed);
	connect(m_pVolumeBtn, &QPushButton::released, this, &BrushSelectDlg::OnReleased);
	connect(m_pSmoothBtn, &QPushButton::pressed, this, &BrushSelectDlg::OnPressed);
	connect(m_pSmoothBtn, &QPushButton::released, this, &BrushSelectDlg::OnReleased);
	connect(m_pInflateBtn, &QPushButton::pressed, this, &BrushSelectDlg::OnPressed);
	connect(m_pInflateBtn, &QPushButton::released, this, &BrushSelectDlg::OnReleased);
	connect(m_pReduceBtn, &QPushButton::pressed, this, &BrushSelectDlg::OnPressed);
	connect(m_pReduceBtn, &QPushButton::released, this, &BrushSelectDlg::OnReleased);

#ifdef SELECT_REMESH_MODE 
	connect(m_pRemeshBtn, &QPushButton::pressed, this, &BrushSelectDlg::OnPressed);
	connect(m_pRemeshBtn, &QPushButton::released, this, &BrushSelectDlg::OnReleased);
#endif

	listLE.push_back(LE_Size_H_4);
	//listLE.push_back(LE_Strength_H_6);
	listSd.push_back(sSlider0);
	//listSd.push_back(sSlider1);
	listBtn.push_back(btn);
	listBtn.push_back(btn2);
	listBtn.push_back(btn3);	//smooth
	//listBtn.push_back(btn4);	//Inflate
	listBtn.push_back(btn5);	//reduce
#ifdef SELECT_REMESH_MODE 
	listBtn.push_back(btn6);	//remesh
#endif


	setBtnIcon(m_pVolumeBtn, emBTN_M_DOWN, true);
	setBtnIcon(m_pSmoothBtn, emBTN_NORMAL, false);
	setBtnIcon(m_pReduceBtn, emBTN_NORMAL, false);	
#ifdef SELECT_REMESH_MODE 
	setBtnIcon(m_pRemeshBtn, emBTN_NORMAL, false);
#endif
	m_pSmoothBtn->setVisible(false);
	m_pReduceBtn->setVisible(false);
#ifdef SELECT_REMESH_MODE 
	m_pRemeshBtn->setVisible(false);
#endif
	lb_H_2->setVisible(false);

	setLayout(layoutV);

	hideFuncBtn();
}


BrushSelectDlg::~BrushSelectDlg()
{
}

void BrushSelectDlg::reject(bool bForce)
{
	if (!m_pDataContext)
	{
		return;
	}

	// 220121 허 건 과장
	ACTION_MANAGER->action_UndoRedo_update(m_pDataContext, MESH_WORK_NONE, MESH_WORK_MANAGER->getWorkMode());

	printf_s("\n BrushSelectDlg::reject()");
	
	FinishProcess();
}

//220121 허 건 과장
void	BrushSelectDlg::FinishProcess()
{
#if 1
	mip::MeshTopology *p_mesh = m_pCurMesh;

	int nPickMesh = m_pDataContext->m_MeshData.GetCurrentMeshIndex();
	mip::MeshTopology *mesh = m_pDataContext->m_MeshData.GetMesh(nPickMesh);

	if (p_mesh != nullptr)
		mesh = p_mesh;

	int		n_mesh = m_pDataContext->m_MeshData.GetMeshCount();
	for (int i = 0; i < n_mesh; ++i)
	{
		mip::MeshTopology* p_mesh = m_pDataContext->m_MeshData.GetMesh(i);

		if (!p_mesh || m_bMoveDlg)
		{
			continue;
		}

		mip::mesh_control::MeshCutCancel(p_mesh);

		p_mesh->updateVertex();
		clearSelection(p_mesh);
		p_mesh->updateColor(p_mesh->m_baseColor);

		g_Renderer->makeCurrent();
		p_mesh->buildRenderBufferTopology();
		g_Renderer->doneCurrent();
	}
#endif
	//setWindowOpacity(0.1);
	//setVisible(false);

	MESH_WORK_MANAGER->UpdateWorkMode(MESH_WORK_NONE, true);
	//WIN_MANAGER->mainMeshWidget->getMainView()->getBtnSculpt()->setChecked(false);

	auto btn_Select = MESH_BTN_SCENE_MANAGER->m_BtnSelect;
	btn_Select->setChecked(false);
	m_pCurMesh = nullptr;
	m_old_pick_idx = -1;
	m_bMoveDlg = false;

	close();
}

void BrushSelectDlg::updateColor()
{
	mip::MeshTopology*			p_mesh = nullptr;
	int							meshCnt = m_pDataContext->m_MeshData.GetMeshCount();

	for (int i = 0; i < meshCnt; ++i)
	{
		//auto  pck_id = WIN_MANAGER->vt_pckID[i];
		//if ((-1 == pck_id) || !WIN_MANAGER->vt_pckID[i]) continue;

		MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(i);
		if (!pMeshInfo || !pMeshInfo->selected) continue;

		p_mesh = m_pDataContext->m_MeshData.GetMesh(i);
		//////////////p_mesh->updateColor();
		p_mesh->updateVertex();
		MeshInfo* mInfo = m_pDataContext->m_MeshData.GetMeshInfo(i);
		mip::VECTOR4 color(mip::VECTOR4(mInfo->color.r / 255.0f, mInfo->color.g / 255.0f, mInfo->color.b / 255.0f, p_mesh->getAlphaVal() / 255.0f));
		p_mesh->updateColor(color);

		WIN_MANAGER->makeCurrent();
		p_mesh->buildRenderBufferTopology();
		WIN_MANAGER->doneCurrent();
	}
}

void BrushSelectDlg::updateSizeSlider(float scale)
{
	QString str = QString("Size");
	float Val = scale * 100;

	_Slider sSlider;
	for (int ii = 0; ii < listSd.size(); ii++)
	{
		sSlider = listSd[ii];
		if (sSlider.pSlidier == nullptr) continue;
		if (str == sSlider.pSlidier->objectName()) break;
	}

	sSlider.pSlidier->blockSignals(true);
	sSlider.pSlidier->setValue((Val));
	sSlider.pSlidier->blockSignals(false);


	QLineEdit* pLineEdit = nullptr;
	
	for (int ii = 0; ii < listLE.size(); ii++)
	{
		pLineEdit = listLE[ii];
		if (pLineEdit->objectName() == str)
		{
			pLineEdit->blockSignals(true);
			pLineEdit->setText(QString::number(Val));
			pLineEdit->blockSignals(false);
		}
	}

	setSize(Val);
}


void BrushSelectDlg::sliderUpDown(QString str, int option) // 1 - up, 0 down
{
	float Val = 0;
	if (str == QString("Strength"))
		Val = getStrength();
	else if (str == QString("Size"))
		Val = getSize();

	_Slider sSlider;
	for (int ii = 0; ii < listSd.size(); ii++)
	{
		sSlider = listSd[ii];
		if (sSlider.pSlidier == nullptr) continue;
		if (str == sSlider.pSlidier->objectName()) break;
	}

	float offset = (sSlider.max - sSlider.min) * 0.01;
	offset = std::ceil(offset);
	if (option == 1)
	{
		Val += offset;
		(Val > sSlider.max) ? Val = sSlider.max : Val;
	}
	else
	{
		Val -= offset;
		(Val < sSlider.min) ? Val = sSlider.min : Val;
	}

	QLineEdit* pLineEdit = nullptr;
	for (int ii = 0; ii < listLE.size(); ii++)
	{
		pLineEdit = listLE[ii];
		if (pLineEdit->objectName() == str)
		{
			pLineEdit->blockSignals(true);
			pLineEdit->setText(QString::number(Val));
			pLineEdit->blockSignals(false);
		}
	}

	sSlider.pSlidier->blockSignals(true);
	sSlider.pSlidier->setValue((Val));
	sSlider.pSlidier->blockSignals(false);

	if (str == QString("Strength")) setStrength(Val);
	else if (str == QString("Size")) setSize(Val);

	auto view = WIN_MANAGER->mainMeshWidget->getMainView();
	view->setSphereScale(Val * 0.01);
	view->setUpdateFrame(false);
}

void BrushSelectDlg::clearSelection(mip::MeshTopology * pMT)
{
	if (pMT == nullptr) return;

	WIN_MANAGER->mainMeshWidget->getMainView()->clearSelectFlag(pMT);

	pMT->updateColor(pMT->m_baseColor);

	WIN_MANAGER->makeCurrent();
	pMT->buildRenderBufferTopology();
	WIN_MANAGER->doneCurrent();
}

void BrushSelectDlg::initUI()
{
	setBtnIcon(m_pSmoothBtn, emBTN_NORMAL, false);
	setBtnIcon(m_pReduceBtn, emBTN_NORMAL, false);
#ifdef SELECT_REMESH_MODE 
	setBtnIcon(m_pRemeshBtn, emBTN_NORMAL, false);
#endif
	//auto view = WIN_MANAGER->mainMeshWidget->getMainView();
	//view->updateSelectionUI();
	hideFuncBtn();
}


void BrushSelectDlg::OnTextChanged()
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
		if (name == QString("Strength")) setStrength(tmpVal);
		else if (name == QString("Size")) setSize(tmpVal);
	}

	sSlider.pSlidier->blockSignals(true);
	sSlider.pSlidier->setValue((tmpVal));
	sSlider.pSlidier->blockSignals(false);

	auto view = WIN_MANAGER->mainMeshWidget->getMainView();
	view->setSphereScale(tmpVal * 0.01);
	view->setUpdateFrame(false);
}

void BrushSelectDlg::OnDepthSlideReleased(int val)
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
		pLineEdit->setText(QString::number(tmpVal, 'f', 2));
	else
		pLineEdit->setText(QString::number((int)tmpVal));
	pLineEdit->blockSignals(false);

	if (name == QString("Strength")) setStrength(val);
	else if (name == QString("Size")) setSize(val);

	auto view = WIN_MANAGER->mainMeshWidget->getMainView();
	view->setSphereScale(tmpVal * 0.01);
	view->setUpdateFrame(false);
}

void BrushSelectDlg::OnComboChanged(int index)
{
	QComboBox* pCombo = nullptr;
	for (int ii = 0; ii < listCB.size(); ii++)
	{
		pCombo = listCB[ii];
		bool active = pCombo->hasFocus();

		if (pCombo->hasFocus()) break;
	}

	QString name = pCombo->objectName();

	if (name == QString("Sculpt"))
	{
		setSculptMode(index);
		WIN_MANAGER->mainMeshWidget->getMainView()->updateMeshVertex();
		//if (!WIN_MANAGER->mainMeshWidget->getMainView()->readyBrush(MESH_WORK_BRUSH_SCULPT)) reject();
	}
	else if (name == QString("Select")) setSelectMode(index);
}

void BrushSelectDlg::OnOK()
{
	m_Accept = true;
	close();
}

void BrushSelectDlg::OnClearFlag()
{
	WIN_MANAGER->mainMeshWidget->getMainView()->clearSelectFlag();
	hideFuncBtn();
}

void BrushSelectDlg::OnCancel()
{
	reject();
	close();
}

void BrushSelectDlg::OnPressed()
{
	if (!m_pDataContext)
	{
		return;
	}

	changePressedBtnIcon(); //20201216_byPHS_OnPressed에 선언해야 클릭 시 아이콘이 잘 변경됨

	QPushButton* focusBtn = nullptr;
	for (int ii = 0; ii < listBtn.size(); ii++)
	{
		if (listBtn[ii].pButton->hasFocus())
		{
			focusBtn = listBtn[ii].pButton;
			break;
		}
	}

	if (focusBtn == nullptr) return;

	if (focusBtn == m_pSurfaceBtn)
	{
		if (m_pSurfaceBtn->isChecked())
		{
			setBtnIcon(m_pVolumeBtn, emBTN_NORMAL, false);
			m_nSelectMode = SM_SURFACE;
		}
	}

	if (focusBtn == m_pVolumeBtn)
	{
		if (m_pVolumeBtn->isChecked())
		{
			setBtnIcon(m_pSurfaceBtn, emBTN_NORMAL, false);
			m_nSelectMode = SM_VOLUME;
		}
	}

	if (focusBtn == m_pSmoothBtn)
	{
		if (m_pSmoothBtn->isChecked())
		{
			m_bMoveDlg = true;
			//reject();

			FinishProcess();

			// 220120 허 건 과장
			ACTION_MANAGER->action_UndoRedo_update(m_pDataContext, MESH_WORK_SMOOTH, MESH_WORK_BRUSH_SELECTION);

			MESH_DIALOG_MANAGER->makeMeshDialog(MESH_WORK_SMOOTH);
			setBtnIcon(m_pInflateBtn, emBTN_NORMAL, false);
			setBtnIcon(m_pReduceBtn, emBTN_NORMAL, false);
#ifdef SELECT_REMESH_MODE 
			setBtnIcon(m_pRemeshBtn, emBTN_NORMAL, false);
#endif
			m_nSculptMode = SM_SMOOTH;
			auto dlg = static_cast<SmoothDlg*>(MESH_DIALOG_MANAGER->getMeshDialog(MESH_DIALOG_SMOOTH));
			if (dlg != nullptr)
			{
				dlg->setMesh(m_pCurMesh);
				dlg->setMeshIdx(m_pDataContext->m_MeshData.GetCurrentMeshIndex());
			}			
		}
	}

	if (focusBtn == m_pInflateBtn)
	{
		if (m_pInflateBtn->isChecked())
		{
			setBtnIcon(m_pSmoothBtn, emBTN_NORMAL, false);
			setBtnIcon(m_pReduceBtn, emBTN_NORMAL, false);
#ifdef SELECT_REMESH_MODE 
			setBtnIcon(m_pRemeshBtn, emBTN_NORMAL, false);
#endif
			m_nSculptMode = SM_INFLATE;
		}
	}

	if (focusBtn == m_pReduceBtn)
	{
		if (m_pReduceBtn->isChecked())
		{
			m_bMoveDlg = true;
			//reject();
			FinishProcess();

			// 220120 허 건 과장
			ACTION_MANAGER->action_UndoRedo_update(m_pDataContext, MESH_WORK_REDUCTION, MESH_WORK_BRUSH_SELECTION);

			MESH_DIALOG_MANAGER->makeMeshDialog(MESH_WORK_REDUCTION);
			setBtnIcon(m_pInflateBtn, emBTN_NORMAL, false);
			setBtnIcon(m_pReduceBtn, emBTN_NORMAL, false);
#ifdef SELECT_REMESH_MODE 
			setBtnIcon(m_pRemeshBtn, emBTN_NORMAL, false);
#endif
			m_nSculptMode = SM_REDUCE;
			auto dlg = static_cast<ReduceDlg*>(MESH_DIALOG_MANAGER->getMeshDialog(MESH_DIALOG_REDUCE));
			if (dlg != nullptr)
			{
				int mode = dlg->getReduceMode();
				dlg->initOptionValue(mode);
				//dlg->setMesh(m_pCurMesh);
				dlg->setMeshIdx(m_pDataContext->m_MeshData.GetCurrentMeshIndex());
			}
		}
	}

#ifdef SELECT_REMESH_MODE 
	if (focusBtn == m_pRemeshBtn)
	{
		if (m_pRemeshBtn->isChecked())
		{
			m_bMoveDlg = true;
			reject();
			MESH_DIALOG_MANAGER->makeMeshDialog(MESH_WORK_REMESH);
			setBtnIcon(m_pInflateBtn, emBTN_NORMAL, false);
			setBtnIcon(m_pReduceBtn, emBTN_NORMAL, false);
			setBtnIcon(m_pSmoothBtn, emBTN_NORMAL, false);
			m_nSelectMode = SM_REMESH;

			CRemeshDlg* dlg = static_cast<CRemeshDlg*>(MESH_DIALOG_MANAGER->getMeshDialog(MESH_DIALOG_REMESH));
			if (dlg != nullptr)
			{
				dlg->setSelectMode(true);
			}
		}
	}
#endif
}

void BrushSelectDlg::keyReleaseEvent(QKeyEvent * e)
{
	const int _modifier = e->key();

	if ((_modifier == Qt::Key_BracketLeft))
	{
		sliderUpDown(QString("Size"), 0);
	}

	if ((_modifier == Qt::Key_BracketRight))
	{
		sliderUpDown(QString("Size"), 1);
	}

}

void BrushSelectDlg::showFuncBtn()
{
	if (!m_pSmoothBtn->isVisible())
	{
		this->hide();

		this->setWindowModified(true);

		m_pSmoothBtn->setVisible(true);
		m_pReduceBtn->setVisible(true);
#ifdef SELECT_REMESH_MODE 
		m_pRemeshBtn->setVisible(true);
#endif
		lb_H_2->setVisible(true);

		int btnHeight, labelHeight;
		QRect btnRect = m_pSmoothBtn->geometry();
		QRect lbRect = lb_H_2->geometry();

		//QRect DialogRect = this->geometry();
		//DialogRect.setX(DialogRect.x());
		//DialogRect.setY(DialogRect.y());
		//DialogRect.setWidth(DialogRect.width());
		//DialogRect.setHeight(DialogRect.height() + (btnRect.height() + lbRect.height()));
		//this->setGeometry(DialogRect);

		this->show();
	}

	WIN_MANAGER->mainWindow->activateWindow();
	WIN_MANAGER->mainWindow->setFocus();
}

void BrushSelectDlg::hideFuncBtn()
{
	if (m_pSmoothBtn->isVisible())
	{
		//for (int i = 0; i < 2; ++i)
		//{
			this->hide();

			this->setWindowModified(true);

			m_pSmoothBtn->setVisible(false);
			m_pReduceBtn->setVisible(false);

#ifdef SELECT_REMESH_MODE 
			m_pRemeshBtn->setVisible(false);
#endif
			lb_H_2->setVisible(false);

			int btnHeight, labelHeight;
			QRect btnRect = m_pSmoothBtn->geometry();
			QRect lbRect = lb_H_2->geometry();

			QRect DialogRect = this->geometry();
			DialogRect.setX(DialogRect.x());
			DialogRect.setY(DialogRect.y());
			DialogRect.setWidth(DialogRect.width());
			DialogRect.setHeight(DialogRect.height() - (btnRect.height() + lbRect.height()));
			this->setGeometry(DialogRect);

			this->show();

			this->resize(DialogRect.size());
		//}
	}

	WIN_MANAGER->mainWindow->activateWindow();
	WIN_MANAGER->mainWindow->setFocus();
}