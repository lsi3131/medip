#include "stdafx.h"
#include "BrushSculptDlg.h"
#include "windowManager.h"
#include "Windows/Main/MainMeshWidget.h"
#include "MEVolumeView.h"

#include "System/styleManager.h"
#include "System/stringManager.h"
#include "System/resourceManager.h"

#include "MeshEdit/CMeshViewBtn3DScene.h"
#include "MeshEdit/CMeshWorkManager.h"

#include "Actions/ActionManager.h"

#include <ppltasks.h>

//#define SUPPORT_SCULPT_SURFACE 1

BrushSculptDlg::BrushSculptDlg(DataContext* pDataContext, QWidget* parent /*= NULL*/)
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
	m_pDeflateBtn(nullptr),
	m_pMoveBtn(nullptr),
	m_pDragBtn(nullptr),
	m_pRefineBtn(nullptr),
	m_TTriCnt(0),
	m_TVertCnt(0),
	m_bTaskFinished(true)
{
	setWindowTitle(QString("Brush Sculpt"));
	initQtUI();
}

SCULPT_MODE BrushSculptDlg::getSculptMode()
{
	return m_nSculptMode;
}

int BrushSculptDlg::getSelectMode()
{
	return m_nSelectMode;
}

int BrushSculptDlg::getStrength()
{
	return m_nStrength;
}

int BrushSculptDlg::getSize()
{
	return m_nSize;
}

bool BrushSculptDlg::isAccept()
{
	return m_Accept;
}

void BrushSculptDlg::setSculptMode(SCULPT_MODE val)
{
	m_nSculptMode = val;
}

void BrushSculptDlg::setSelectMode(int val)
{
	m_nSelectMode = val;
}

void BrushSculptDlg::setStrength(int val)
{
	m_nStrength = val;
}

void BrushSculptDlg::setSize(int val)
{
	m_nSize = val;
}

void BrushSculptDlg::setVertTriCnt(int _vCnt, int _tCnt)
{
	m_TVertCnt = _vCnt;
	m_TTriCnt = _tCnt;
};

void BrushSculptDlg::setBoundingBox(mip::AABB _box)
{
	m_box = _box;
}

mip::AABB BrushSculptDlg::getBoundingBox()
{
	return m_box;
}

void BrushSculptDlg::getVertTriCnt(int& _vCnt, int& _tCnt) 
{
	_vCnt = m_TVertCnt;
	_tCnt = m_TTriCnt;
};

void BrushSculptDlg::updateSizeSlider(float scale)
{
	QString str = QString("Size");
	float Val = scale * 100;

	_Slider sSlider;
	for (int ii = 0; ii < listSd.size(); ii++)
	{
		sSlider = listSd[ii];
		if (sSlider.pSlidier == nullptr)
		{
			continue;
		}

		if (str == sSlider.pSlidier->objectName())
		{
			break;
		}
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

void BrushSculptDlg::initQtUI()
{
	QWidget* container = new QWidget(this);
	QWidget* container2 = new QWidget(this);

	QVBoxLayout* layoutV = new QVBoxLayout;
	QHBoxLayout* layoutH_Top = new QHBoxLayout(container);
	QHBoxLayout* layoutH_Top2 = new QHBoxLayout;
	QHBoxLayout* layoutH_0 = new QHBoxLayout;
	QHBoxLayout* layoutH_1 = new QHBoxLayout;
	QHBoxLayout* layoutH_2 = new QHBoxLayout;
	QHBoxLayout* layoutH_3 = new QHBoxLayout;
	QHBoxLayout* layoutH_3_1 = new QHBoxLayout;
	//QHBoxLayout *layoutH_3_2 = new QHBoxLayout;
	QHBoxLayout* layoutH_4 = new QHBoxLayout;
	QHBoxLayout* layoutH_5 = new QHBoxLayout;
	QHBoxLayout* layoutH_6 = new QHBoxLayout;
	QHBoxLayout* layoutH_7 = new QHBoxLayout;
	QHBoxLayout* layoutH_bottom = new QHBoxLayout;
	int LE_Width = 50;

	layoutV->addLayout(layoutH_Top);
	layoutV->addLayout(layoutH_Top2);
	layoutV->addLayout(layoutH_0);
	layoutV->addLayout(layoutH_1);
	layoutV->addLayout(layoutH_2);
	layoutV->addLayout(layoutH_3);
	layoutV->addLayout(layoutH_3_1);
	//layoutV->addLayout(layoutH_3_2);
	layoutV->addLayout(layoutH_4);
	layoutV->addLayout(layoutH_5);
	layoutV->addLayout(layoutH_6);
	layoutV->addLayout(layoutH_7);
	layoutV->addLayout(layoutH_bottom);

	QLabel* lb_H_Top = new QLabel(QString(""), this);
	QLabel* lb_H_Top2 = new QLabel(QString(""), this);
	lb_H_Top2->setStyleSheet("background-color: #414141;");

	layoutH_Top2->setAlignment(Qt::AlignLeft | Qt::AlignTop);
	container->setStyleSheet("background-color: #414141;");
	QRect rect2 = container->geometry();
	rect2.setWidth(200);
	container->setGeometry(rect2);
	container2->setStyleSheet("background-color: #414141;");
	layoutH_Top->addWidget(lb_H_Top);
	layoutH_Top2->addWidget(lb_H_Top2);


	QLabel* lb_H_0 = new QLabel(QString(" "), this);
	layoutH_0->addWidget(lb_H_0);

	int left = 11;
	QLabel* lb_Mode = new QLabel(QString("Mode"), this);
	lb_Mode->setGeometry(left, 31, 100, 30);
	lb_Mode->raise();

	QLabel* lb_Sculpt = new QLabel(QString("Sculpt"), this);
	lb_Sculpt->setGeometry(left - 4, 5, 100, 20);
	lb_Sculpt->setStyleSheet("background-color: #414141;");
	lb_Sculpt->raise();

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

	//
	QLabel* lb_H_2 = new QLabel(QString("Brush Type"), this);
	layoutH_2->addWidget(lb_H_2);

	// "Brush Type"
	layoutH_3->setDirection(QBoxLayout::RightToLeft);
	layoutH_3->addStretch();
	layoutH_3->addStretch();
	layoutH_3->addStretch();
	layoutH_3->addStretch();



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

	m_pRefineBtn = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_SCULPT_OP_REFINE, 0, 0), "", this);
	m_pRefineBtn->setObjectName(QString("Refine"));
	m_pRefineBtn->setIconSize(QSize(32, 32));
	m_pRefineBtn->setFixedSize(QSize(32, 32));
	m_pRefineBtn->setCheckable(true);
	m_pRefineBtn->setMouseTracking(true);
	m_pRefineBtn->installEventFilter(this);
	m_pRefineBtn->setStyleSheet("border: none;");
	m_pRefineBtn->setToolTip(QString("Refine"));
	layoutH_3->addWidget(m_pRefineBtn);
	_Button btn9(m_pRefineBtn, ICON_NON_SCULPT_OP_REFINE, ICON_SCULPT_OP_REFINE_HOVER, ICON_SCULPT_OP_REFINE);

	m_pDeflateBtn = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_SCULPT_OP_DEFLATE, 0, 0), "", this);
	m_pDeflateBtn->setObjectName(QString("Surface"));
	m_pDeflateBtn->setIconSize(QSize(32, 32));
	m_pDeflateBtn->setFixedSize(QSize(32, 32));
	m_pDeflateBtn->setCheckable(true);
	m_pDeflateBtn->setMouseTracking(true);
	m_pDeflateBtn->installEventFilter(this);
	m_pDeflateBtn->setStyleSheet("border: none;");
	m_pDeflateBtn->setToolTip(QString("Deflate"));
	layoutH_3->addWidget(m_pDeflateBtn);
	_Button btn4(m_pDeflateBtn, ICON_NON_SCULPT_OP_DEFLATE, ICON_SCULPT_OP_DEFLATE_HOVER, ICON_SCULPT_OP_DEFLATE);

	m_pInflateBtn = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_SCULPT_OP_INFLATE, 0, 0), "", this);
	m_pInflateBtn->setObjectName(QString("Surface"));
	m_pInflateBtn->setIconSize(QSize(32, 32));
	m_pInflateBtn->setFixedSize(QSize(32, 32));
	m_pInflateBtn->setCheckable(true);
	m_pInflateBtn->setMouseTracking(true);
	m_pInflateBtn->installEventFilter(this);
	m_pInflateBtn->setStyleSheet("border: none;");
	m_pInflateBtn->setToolTip(QString("Inflate"));
	layoutH_3->addWidget(m_pInflateBtn);
	_Button btn6(m_pInflateBtn, ICON_NON_SCULPT_OP_INFLATE, ICON_SCULPT_OP_INFLATE_HOVER, ICON_SCULPT_OP_INFLATE);



	// "Brush Type"
	layoutH_3_1->setDirection(QBoxLayout::RightToLeft);
	layoutH_3_1->addStretch();
	layoutH_3_1->addStretch();

	m_pDragBtn = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_SCULPT_OP_DRAG, 0, 0), "", this);
	m_pDragBtn->setObjectName(QString("Drag"));
	m_pDragBtn->setIconSize(QSize(32, 32));
	m_pDragBtn->setFixedSize(QSize(32, 32));
	m_pDragBtn->setCheckable(true);
	m_pDragBtn->setMouseTracking(true);
	m_pDragBtn->installEventFilter(this);
	m_pDragBtn->setStyleSheet("border: none;");
	m_pDragBtn->setToolTip(QString("Drag"));
	layoutH_3_1->addWidget(m_pDragBtn);
	_Button btn7(m_pDragBtn, ICON_NON_SCULPT_OP_DRAG, ICON_SCULPT_OP_DRAG_HOVER, ICON_SCULPT_OP_DRAG);

	m_pMoveBtn = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_SCULPT_OP_MOVE, 0, 0), "", this);
	m_pMoveBtn->setObjectName(QString("Move"));
	m_pMoveBtn->setIconSize(QSize(32, 32));
	m_pMoveBtn->setFixedSize(QSize(32, 32));
	m_pMoveBtn->setCheckable(true);
	m_pMoveBtn->setMouseTracking(true);
	m_pMoveBtn->installEventFilter(this);
	m_pMoveBtn->setStyleSheet("border: none;");
	m_pMoveBtn->setToolTip(QString("Move"));
	layoutH_3_1->addWidget(m_pMoveBtn);
	_Button btn8(m_pMoveBtn, ICON_NON_SCULPT_OP_MOVE, ICON_SCULPT_OP_MOVE_HOVER, ICON_SCULPT_OP_MOVE);

	m_pSmoothBtn = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_SCULPT_OP_SMOOTH, 0, 0), "", this);
	m_pSmoothBtn->setObjectName(QString("Smooth"));
	m_pSmoothBtn->setIconSize(QSize(32, 32));
	m_pSmoothBtn->setFixedSize(QSize(32, 32));
	m_pSmoothBtn->setCheckable(true);
	m_pSmoothBtn->setMouseTracking(true);
	m_pSmoothBtn->installEventFilter(this);
	m_pSmoothBtn->setStyleSheet("border: none;");
	m_pSmoothBtn->setToolTip(QString("Smooth"));
	layoutH_3_1->addWidget(m_pSmoothBtn);
	_Button btn3(m_pSmoothBtn, ICON_NON_SCULPT_OP_SMOOTH, ICON_SCULPT_OP_SMOOTH_HOVER, ICON_SCULPT_OP_SMOOTH);


	// "SIZE"
	QLabel* lb_H_4 = new QLabel(QString("Size"), this);
	layoutH_4->addWidget(lb_H_4);

	double min = 1.0; double max = 100.0; int demical = 2; double initVal = 50.0;
	QLineEdit* LE_Size_H_4 = initLineEdit(this, QString("Size"), emLE_INT, min, max, (int)initVal, demical, LE_Width);
	layoutH_4->addWidget(LE_Size_H_4);

	_Slider sSlider0;
	initSlider(this, QString("Size"), sSlider0, Qt::Horizontal, emLE_INT, min, max, initVal);
	layoutH_5->addWidget(sSlider0.pSlidier);

	// "Strength"
	QLabel* lb_H_6 = new QLabel(QString("Strength"), this);
	layoutH_6->addWidget(lb_H_6);

	QLineEdit* LE_Strength_H_6 = initLineEdit(this, QString("Strength"), emLE_INT, min, max, (int)initVal, demical, LE_Width);
	layoutH_6->addWidget(LE_Strength_H_6);

	_Slider sSlider1;
	initSlider(this, QString("Strength"), sSlider1, Qt::Horizontal, emLE_INT, min, max, initVal);
	layoutH_7->addWidget(sSlider1.pSlidier);

	//
	QPushButton* btnOK = new QPushButton(this);
	btnOK->setText(STRING_MANAGER->getString(STR_OK));
	btnOK->setFixedSize(30, 30);
	btnOK->setVisible(false);
	layoutH_bottom->addWidget(btnOK);

	QPushButton* btnCancel = new QPushButton(this);
	btnCancel->setText(STRING_MANAGER->getString(STR_CANCEL));
	btnCancel->setFixedSize(60, 30);
	btnCancel->setVisible(false);
	layoutH_bottom->addWidget(btnCancel);

	connect(btnOK, &QPushButton::clicked, this, &BrushSculptDlg::OnOK);
	connect(btnCancel, &QPushButton::clicked, this, &BrushSculptDlg::OnCancel);
	connect(LE_Size_H_4, &QLineEdit::textChanged, this, &BrushSculptDlg::OnTextChanged);
	connect(LE_Strength_H_6, &QLineEdit::textChanged, this, &BrushSculptDlg::OnTextChanged);

	connect(sSlider0.pSlidier, &QSlider::valueChanged, this, &BrushSculptDlg::OnDepthSlideReleased);
	connect(sSlider1.pSlidier, &QSlider::valueChanged, this, &BrushSculptDlg::OnDepthSlideReleased);

	connect(m_pSurfaceBtn, &QPushButton::pressed, this, &BrushSculptDlg::OnPressed);
	connect(m_pSurfaceBtn, &QPushButton::released, this, &BrushSculptDlg::OnSculptReleased);
	connect(m_pVolumeBtn, &QPushButton::pressed, this, &BrushSculptDlg::OnPressed);
	connect(m_pVolumeBtn, &QPushButton::released, this, &BrushSculptDlg::OnSculptReleased);
	connect(m_pSmoothBtn, &QPushButton::pressed, this, &BrushSculptDlg::OnPressed);
	connect(m_pSmoothBtn, &QPushButton::released, this, &BrushSculptDlg::OnReleased);
	connect(m_pInflateBtn, &QPushButton::pressed, this, &BrushSculptDlg::OnPressed);
	connect(m_pInflateBtn, &QPushButton::released, this, &BrushSculptDlg::OnReleased);
	connect(m_pReduceBtn, &QPushButton::pressed, this, &BrushSculptDlg::OnPressed);
	connect(m_pReduceBtn, &QPushButton::released, this, &BrushSculptDlg::OnReleased);
	connect(m_pDeflateBtn, &QPushButton::pressed, this, &BrushSculptDlg::OnPressed);
	connect(m_pDeflateBtn, &QPushButton::released, this, &BrushSculptDlg::OnReleased);
	connect(m_pMoveBtn, &QPushButton::pressed, this, &BrushSculptDlg::OnPressed);
	connect(m_pMoveBtn, &QPushButton::released, this, &BrushSculptDlg::OnReleased);
	connect(m_pDragBtn, &QPushButton::pressed, this, &BrushSculptDlg::OnPressed);
	connect(m_pDragBtn, &QPushButton::released, this, &BrushSculptDlg::OnReleased);
	connect(m_pRefineBtn, &QPushButton::pressed, this, &BrushSculptDlg::OnPressed);
	connect(m_pRefineBtn, &QPushButton::released, this, &BrushSculptDlg::OnReleased);

	listLE.push_back(LE_Size_H_4);
	listLE.push_back(LE_Strength_H_6);
	listSd.push_back(sSlider0);
	listSd.push_back(sSlider1);
	listBtn.push_back(btn);
	listBtn.push_back(btn2);
	listBtn.push_back(btn3);
	listBtn.push_back(btn4);
	listBtn.push_back(btn5);
	listBtn.push_back(btn6);
	listBtn.push_back(btn7);
	listBtn.push_back(btn8);
	listBtn.push_back(btn9);

	setBtnIcon(m_pVolumeBtn, emBTN_M_DOWN, true);
	setBtnIcon(m_pSmoothBtn, emBTN_M_DOWN, true);

	setLayout(layoutV);
}

void BrushSculptDlg::tryBtnRoutine(SCULPT_MODE nSCULPT_MODE)
{
	m_nSculptMode = nSCULPT_MODE;

	if (nSCULPT_MODE != SM_SMOOTH)
	{
		setBtnIcon(m_pSmoothBtn, emBTN_NORMAL, false);
	}

	if (nSCULPT_MODE != SM_INFLATE)
	{
		setBtnIcon(m_pInflateBtn, emBTN_NORMAL, false);
	}

	if (nSCULPT_MODE != SM_DEFLATE)
	{
		setBtnIcon(m_pDeflateBtn, emBTN_NORMAL, false);
	}

	if (nSCULPT_MODE != SM_REDUCE)
	{
		setBtnIcon(m_pReduceBtn, emBTN_NORMAL, false);
	}

	if (nSCULPT_MODE != SM_MOVE)
	{
		setBtnIcon(m_pMoveBtn, emBTN_NORMAL, false);
	}

	if (nSCULPT_MODE != SM_DRAG)
	{
		setBtnIcon(m_pDragBtn, emBTN_NORMAL, false);
	}

	if (nSCULPT_MODE != SM_REFINE)
	{
		setBtnIcon(m_pRefineBtn, emBTN_NORMAL, false);
	}
}

BrushSculptDlg::~BrushSculptDlg()
{
}

void BrushSculptDlg::reject(bool bForce)
{
	printf_s("\n BrushSculptDlg::reject()");
	setWindowOpacity(0.1);
	setVisible(false);


	ACTION_MANAGER->action_UndoRedo_update(m_pDataContext, MESH_WORK_NONE, MESH_WORK_MANAGER->getWorkMode());
	//MESH_WORK_MANAGER->UpdateWorkMode(MESH_WORK_NONE, true);

	QPushButton* btn_sculpt = MESH_BTN_SCENE_MANAGER->m_BtnSculpt;
	btn_sculpt->setChecked(false);

	//WIN_MANAGER->mainMeshWidget->getMainView()->createMatchingData();
	close();
}

void BrushSculptDlg::keyReleaseEvent(QKeyEvent* e)
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

void BrushSculptDlg::sliderUpDown(QString str, int option) // 1 - up, 0 down
{
	float Val = 0;
	if (str == QString("Strength"))
	{
		Val = getStrength();
	}
	else if (str == QString("Size"))
	{
		Val = getSize();
	}

	_Slider sSlider;
	for (int ii = 0; ii < listSd.size(); ii++)
	{
		sSlider = listSd[ii];
		if (sSlider.pSlidier == nullptr)
		{
			continue;
		}

		if (str == sSlider.pSlidier->objectName())
		{
			break;
		}
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

	if (str == QString("Strength"))
	{
		setStrength(Val);
	}
	else if (str == QString("Size"))
	{
		//auto view = WIN_MANAGER->mainMeshWidget->getMainView();
		MEVolumeView* pViewer = (MEVolumeView*)(this->parent());

		if (pViewer)
		{
			pViewer->setSphereScale(Val * 0.01f);
			pViewer->setUpdateFrame(false);
		}

		setSize(Val);
	}
}


void BrushSculptDlg::OnTextChanged()
{
	QLineEdit* pLineEdit = nullptr;
	for (int ii = 0; ii < listLE.size(); ii++)
	{
		pLineEdit = listLE[ii];
		if (pLineEdit->isModified())
		{
			break;
		}
	}

	double tmpVal;
	QString text = pLineEdit->text();
	QString name = pLineEdit->objectName();
	if (dynamic_cast<const QDoubleValidator*>(pLineEdit->validator()) != nullptr)
	{
		tmpVal = text.toDouble();
	}
	else
	{
		tmpVal = text.toInt();
	}


	//find Slider
	_Slider sSlider;
	for (int ii = 0; ii < listSd.size(); ii++)
	{
		sSlider = listSd[ii];
		if (sSlider.pSlidier == nullptr)
		{
			continue;
		}

		if (name == sSlider.pSlidier->objectName())
		{
			break;
		}
	}

	pLineEdit->blockSignals(true);

	if (tmpVal < sSlider.min)
	{
		tmpVal = sSlider.min;
	}
	else if (tmpVal > sSlider.max)
	{
		tmpVal = sSlider.max;
	}

	if (QString::number(tmpVal) != text)
	{
		pLineEdit->setText(QString::number(tmpVal));
	}

	pLineEdit->blockSignals(false);

	if (dynamic_cast<const QDoubleValidator*>(pLineEdit->validator()) != nullptr)
	{
		tmpVal -= sSlider.min;
		tmpVal /= (sSlider.max - sSlider.min);
		tmpVal *= 100;
	}

	{
		if (name == QString("Strength"))
		{
			setStrength(tmpVal);
		}

		else if (name == QString("Size"))
		{
			setSize(tmpVal);
		}
	}

	sSlider.pSlidier->blockSignals(true);
	sSlider.pSlidier->setValue((tmpVal));
	sSlider.pSlidier->blockSignals(false);

	if (name == QString("Size"))
	{
		//auto view = WIN_MANAGER->mainMeshWidget->getMainView();
		MEVolumeView* pViewer = (MEVolumeView*)(this->parent());
		if (pViewer)
		{
			pViewer->setSphereScale(tmpVal * 0.01f);
			pViewer->setUpdateFrame(false);
		}
	}

}

void BrushSculptDlg::OnDepthSlideReleased(int val)
{
	double tmpVal = val;

	_Slider pSlider;
	for (int ii = 0; ii < listSd.size(); ii++)
	{
		pSlider = listSd[ii];
		if (pSlider.pSlidier->hasFocus())
		{
			break;
		}
	}

	QString name = pSlider.pSlidier->objectName();

	//QLineEdit
	QLineEdit* pLineEdit = nullptr;
	for (int ii = 0; ii < listLE.size(); ii++)
	{
		pLineEdit = listLE[ii];
		if (name == pLineEdit->objectName())
		{
			break;
		}
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
	{
		pLineEdit->setText(QString::number(tmpVal, 'f', 2));
	}
	else
	{
		pLineEdit->setText(QString::number((int)tmpVal));
	}
	pLineEdit->blockSignals(false);

	if (name == QString("Strength"))
	{
		setStrength(val);
	}
	else if (name == QString("Size"))
	{
		setSize(val);
	}

	if (name == QString("Size"))
	{
		//auto view = WIN_MANAGER->mainMeshWidget->getMainView();
		MEVolumeView* pViewer = (MEVolumeView*)(this->parent());
		if (pViewer)
		{
			pViewer->setSphereScale(tmpVal * 0.01f);
			pViewer->setUpdateFrame(false);
		}
	}
}

void BrushSculptDlg::OnComboChanged(int index)
{
	QComboBox* pCombo = nullptr;
	for (int ii = 0; ii < listCB.size(); ii++)
	{
		pCombo = listCB[ii];
		bool active = pCombo->hasFocus();

		if (pCombo->hasFocus())
		{
			break;
		}
	}

	QString name = pCombo->objectName();

	if (name == QString("Sculpt"))
	{
		setSculptMode((SCULPT_MODE)index);
		MEVolumeView* pViewer = (MEVolumeView*)(this->parent());
		if (pViewer)
		{
			pViewer->updateMeshVertex();
		}
		//WIN_MANAGER->mainMeshWidget->getMainView()->updateMeshVertex();
		//if (!WIN_MANAGER->mainMeshWidget->getMainView()->readyBrush(MESH_WORK_BRUSH_SCULPT)) reject();
	}
	else if (name == QString("Select"))
	{
		setSelectMode(index);
	}
}

void BrushSculptDlg::OnOK()
{
	m_Accept = true;
	close();
}

void BrushSculptDlg::OnCancel()
{
	reject();
	close();
}

void BrushSculptDlg::OnPressed()
{
	changePressedBtnIcon(); //20201216_byPHS_OnPressed에 선언해야 클릭 시 아이콘이 잘 변경됨
	m_pSurfaceBtn->setDisabled(false);
	QPushButton* focusBtn = nullptr;
	for (int ii = 0; ii < listBtn.size(); ii++)
	{
		if (listBtn[ii].pButton->hasFocus())
		{
			focusBtn = listBtn[ii].pButton;
			break;
		}
	}

	if (focusBtn == nullptr)
	{
		return;
	}

	if (focusBtn == m_pSurfaceBtn)
	{
		if (m_pSurfaceBtn->isChecked())
		{
			setBtnIcon(m_pVolumeBtn, emBTN_NORMAL, false);
			m_nSelectMode = SM_SURFACE;

#ifndef SUPPORT_SCULPT_SURFACE
			if (m_nSculptMode == SM_MOVE || m_nSculptMode == SM_DRAG)
			{
				setSelectMode(0);
				//m_pSurfaceBtn->setDisabled(true);
				setBtnIcon(m_pSurfaceBtn, emBTN_NORMAL, false);
				setBtnIcon(m_pVolumeBtn, emBTN_NORMAL, true);
				setBtnIcon(m_pVolumeBtn, emBTN_M_DOWN, true);
				m_nSelectMode = SM_VOLUME;
			}
#endif
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
			tryBtnRoutine(SM_SMOOTH);
		}
	}

	if (focusBtn == m_pInflateBtn)
	{
		if (m_pInflateBtn->isChecked())
		{
			tryBtnRoutine(SM_INFLATE);
		}
	}

	if (focusBtn == m_pDeflateBtn)
	{
		if (m_pDeflateBtn->isChecked())
		{
			tryBtnRoutine(SM_DEFLATE);
		}
	}

	if (focusBtn == m_pReduceBtn)
	{
		if (m_pReduceBtn->isChecked())
		{
			tryBtnRoutine(SM_REDUCE);
		}
	}

	if (focusBtn == m_pMoveBtn)
	{
		if (m_pMoveBtn->isChecked())
		{
			tryBtnRoutine(SM_MOVE);
			//202010607_Drag_Move시 Surface 안되는걸로 변경
#ifndef SUPPORT_SCULPT_SURFACE
			setSelectMode(0);
			//m_pSurfaceBtn->setDisabled(true);
			setBtnIcon(m_pSurfaceBtn, emBTN_NORMAL, false);
			setBtnIcon(m_pVolumeBtn, emBTN_NORMAL, true);
			setBtnIcon(m_pVolumeBtn, emBTN_M_DOWN, true);
			m_nSelectMode = SM_VOLUME;
#endif
		}
	}

	if (focusBtn == m_pDragBtn)
	{
		if (m_pDragBtn->isChecked())
		{
			tryBtnRoutine(SM_DRAG);
			//202010607_Drag_Move시 Surface 안되는걸로 변경

#ifndef SUPPORT_SCULPT_SURFACE
			setSelectMode(0);
			//m_pSurfaceBtn->setDisabled(true);
			setBtnIcon(m_pSurfaceBtn, emBTN_NORMAL, false);
			setBtnIcon(m_pVolumeBtn, emBTN_NORMAL, true);
			setBtnIcon(m_pVolumeBtn, emBTN_M_DOWN, true);
			m_nSelectMode = SM_VOLUME;
#endif
		}
	}

	if (focusBtn == m_pRefineBtn)
	{
		if (m_pRefineBtn->isChecked())
		{
			tryBtnRoutine(SM_REFINE);

			//#ifndef SUPPORT_SCULPT_SURFACE
			//			setSelectMode(0);
			//			//m_pSurfaceBtn->setDisabled(true);
			//			setBtnIcon(m_pSurfaceBtn, emBTN_NORMAL, false);
			//			setBtnIcon(m_pVolumeBtn, emBTN_NORMAL, true);
			//			setBtnIcon(m_pVolumeBtn, emBTN_M_DOWN, true);
			//			m_nSelectMode = SM_VOLUME;
			//#endif
		}
	}

}

void BrushSculptDlg::OnSculptReleased()
{
	for (int ii = 0; ii < listBtn.size(); ii++)
	{
		if (listBtn[ii].pButton == HoverWidget)
		{
			if (!listBtn[ii].pButton->isChecked())
			{
				listBtn[ii].pButton->setChecked(true);
			}

#ifndef SUPPORT_SCULPT_SURFACE
			if (m_nSculptMode == SM_MOVE || m_nSculptMode == SM_DRAG)
			{
				setSelectMode(0);
				//m_pSurfaceBtn->setDisabled(true);
				setBtnIcon(m_pSurfaceBtn, emBTN_NORMAL, false);
				setBtnIcon(m_pVolumeBtn, emBTN_NORMAL, true);
				setBtnIcon(m_pVolumeBtn, emBTN_M_DOWN, true);
				m_nSelectMode = SM_VOLUME;
			}
#endif

		}
	}
}
