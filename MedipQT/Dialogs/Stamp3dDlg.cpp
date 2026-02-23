#include "stdafx.h"
#include "Stamp3dDlg.h"
#include "windowManager.h"
#include "Main/MainMeshWidget.h"
#include "MEVolumeView.h"

#include "MeshControl.h"
#include "System/styleManager.h"
#include "System/stringManager.h"

#include "MeshEdit/CMeshViewRenderManager.h"
#include "MeshEdit/CMeshWorkManager.h"
#include "MeshEdit/CMeshViewBtn3DScene.h"
#include "MeshEdit/CMeshFontManager.h"

#include "Actions/ActionManager.h"
#include "ActionMesh.h"

Stamp3dDlg::Stamp3dDlg(DataContext* pDataContext, QWidget* parent /*= NULL*/)
	: MeshBaseDlg(pDataContext, parent),
	m_dOffsetDistance(-2.0),
	m_nEdgeLength(0.0),
	m_Font("Bold", 5, QFont::Bold),
	m_pTextLineEdit(nullptr),
	m_targetMesh(nullptr),
	m_fontMesh(nullptr),
	m_nPickFaceIdx(-1),
	m_fOffsetMin(-2.00f),
	m_fOffsetMax(2.00f),
	m_bEnable(false),
	m_FontSize(6),
	m_PickingPos(mip::VECTOR3(0, 0, 0)),
	m_OldPickingPos(mip::VECTOR3(0, 0, 0)),
	m_accumulatedQuater(mip::QUATERNION(0, 0, 0, 0))
{
	setWindowTitle(QString("Stamp3D"));
	initQtUI();
}

Stamp3dDlg::Stamp3dDlg(DataContext* pDataContext, QString _title, QWidget* parent /*= NULL*/)
	: MeshBaseDlg(pDataContext, _title, parent),
	m_dOffsetDistance(-2.0),
	m_nEdgeLength(0.0),
	m_Font("Bold", 5, QFont::Bold),
	m_pTextLineEdit(nullptr),
	m_targetMesh(nullptr),
	m_fontMesh(nullptr),
	m_nPickFaceIdx(-1),
	m_fOffsetMin(-2.00f),
	m_fOffsetMax(2.00f),
	m_bEnable(false),
	m_FontSize(6),
	m_PickingPos(mip::VECTOR3(0, 0, 0)),
	m_OldPickingPos(mip::VECTOR3(0, 0, 0)),
	m_accumulatedQuater(mip::QUATERNION(0, 0, 0, 0))
{
	initQtUI();
}

Stamp3dDlg::~Stamp3dDlg()
{
	initStampData();
}

void Stamp3dDlg::closeEvent(QCloseEvent*)
{
	if (!ACTION_MANAGER->isActionFinished()) return;

	exitDialog();
}


void Stamp3dDlg::reject(bool bForce)
{
	if (!ACTION_MANAGER->isActionFinished() && !bForce) return; //20210928_byPHS_기능수행중 Reject 안되게 수정

	QString _title = windowTitle();
	printf_s("\n %s ::reject()", _title.toStdString().c_str());
	setWindowOpacity(0.1);
	setVisible(false);

	//MESH_RENDER_MANAGER->getFontControl()->InitOrgData();
	MESH_WORK_MANAGER->UpdateWorkMode(MESH_WORK_NONE, true);
	close();
}


void Stamp3dDlg::initQtUI()
{
	initTopology();
	QWidget* container = new QWidget(this);
	//
	QVBoxLayout* layoutV = new QVBoxLayout;
	QHBoxLayout* layoutH_Top = new QHBoxLayout(container);
	QHBoxLayout* layoutH_0 = new QHBoxLayout;
	QHBoxLayout* layoutH_1 = new QHBoxLayout;
	QHBoxLayout* layoutH_2 = new QHBoxLayout;
	QHBoxLayout* layoutH_3 = new QHBoxLayout;
	QHBoxLayout* layoutH_4 = new QHBoxLayout;
	QHBoxLayout* layoutH_5 = new QHBoxLayout;
	QHBoxLayout* layoutH_6 = new QHBoxLayout;
	QHBoxLayout* layoutH_7 = new QHBoxLayout;
	QHBoxLayout* layoutH_8 = new QHBoxLayout;
	QHBoxLayout* layoutH_bottom = new QHBoxLayout;
	QVBoxLayout* layoutV_bottom_0 = new QVBoxLayout;
	QVBoxLayout* layoutV_bottom_1 = new QVBoxLayout;
	int LE_Width = 40;

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
	layoutV->addLayout(layoutH_bottom);

	container->setStyleSheet("background-color: #414141;");
	QRect rect = container->geometry();
	rect.setWidth(200);
	container->setGeometry(rect);
	this->setGeometry(rect);

	QLabel* lb_H_Top = new QLabel(QString(""), this);
	lb_H_Top->setStyleSheet("background-color: #414141;");
	layoutH_Top->addWidget(lb_H_Top);

	//
	QLabel* lb_H_0 = new QLabel(QString("Stamp3D"), this);
	layoutH_0->addWidget(lb_H_0);
	layoutH_0->setAlignment(Qt::AlignLeft | Qt::AlignTop);
	lb_H_0->setStyleSheet("background-color: #414141;");

	//Offset
	double min = m_fOffsetMin; double max = m_fOffsetMax; int demical = 2; double initVal = -2.0;
	QLabel* lb_H_2_0 = new QLabel(QString("Offset Distance (mm)"), this);
	lb_H_2_0->setObjectName(QString("Offset"));
	lb_H_2_0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	layoutH_1->addWidget(lb_H_2_0);

	QLineEdit* LE_Size_H_2_0 = initLineEdit(this, QString("Offset"), emLE_DOUBLE, min, max, initVal, demical, LE_Width);
	layoutH_1->addWidget(LE_Size_H_2_0);

	_Slider sSlider0;
	initSlider(this, QString("Offset"), sSlider0, Qt::Horizontal, emLE_DOUBLE, min, max, initVal);
	layoutH_2->addWidget(sSlider0.pSlidier);

	//EdgeLength
	min = 0.0; max = 100.0; demical = 2; initVal = 6;
	QLabel* lb_H_2_1 = new QLabel(QString("Font Size (mm)"), this);
	lb_H_2_1->setObjectName(QString("Font Size"));
	lb_H_2_0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	//lb_H_2_1->hide();
	layoutH_3->addWidget(lb_H_2_1);

	QLineEdit* LE_Size_H_2_1 = initLineEdit(this, QString("Font Size"), emLE_INT, min, max, initVal, demical, LE_Width);
	//LE_Size_H_2_1->hide();
	layoutH_3->addWidget(LE_Size_H_2_1);

	_Slider sSlider1;
	initSlider(this, QString("Font Size"), sSlider1, Qt::Horizontal, emLE_INT, min, max, initVal);
	//sSlider1.pSlidier->hide();
	layoutH_4->addWidget(sSlider1.pSlidier);

	QLabel* lb_H_5_0 = new QLabel(QString("Font TEXT"), this);
	lb_H_5_0->setObjectName(QString("Offset"));
	lb_H_5_0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	layoutH_5->addWidget(lb_H_5_0);

	LE_Width = 145;
	m_pTextLineEdit = initLineEdit(this, QString("StampText"), emLE_TEXT, min, max, initVal, demical, LE_Width);
	layoutH_6->addWidget(m_pTextLineEdit);

	QPushButton* btnChangeFontStyle = new QPushButton(this);
	btnChangeFontStyle->setText("...");
	btnChangeFontStyle->setFixedHeight(30);
	//btnUpdate->setFixedSize(30, 30);
	btnChangeFontStyle->setVisible(true);
	btnChangeFontStyle->setStyleSheet(STYLE_MANAGER->buttonNormal);
	layoutH_6->addWidget(btnChangeFontStyle);

	QPushButton* btnMakeFont3D = new QPushButton(this);
	btnMakeFont3D->setText("Make Font 3D");
	btnMakeFont3D->setFixedHeight(30);
	//btnUpdate->setFixedSize(30, 30);
	btnMakeFont3D->setVisible(true);
	btnMakeFont3D->setStyleSheet(STYLE_MANAGER->buttonNormal);
	layoutH_7->addWidget(btnMakeFont3D);

	QPushButton* btnUpdateStamp = new QPushButton(this);
	btnUpdateStamp->setText("Stamp3D");
	btnUpdateStamp->setFixedHeight(30);
	//btnUpdate->setFixedSize(30, 30);
	btnUpdateStamp->setVisible(true);
	btnUpdateStamp->setStyleSheet(STYLE_MANAGER->buttonNormal);
	layoutH_8->addWidget(btnUpdateStamp);

	QPushButton* btnOK = new QPushButton(this);
	btnOK->setText(QString("Apply"));
	btnOK->setFixedSize(80, 30);
	btnOK->setVisible(true);
	btnOK->setStyleSheet(STYLE_MANAGER->buttonNormal);
	layoutV_bottom_0->addWidget(btnOK, Qt::AlignLeft);
	layoutV_bottom_0->setAlignment(Qt::AlignLeft);
	layoutH_bottom->addLayout(layoutV_bottom_0);

	QPushButton* btnCancel = new QPushButton(this);
	btnCancel->setText(STRING_MANAGER->getString(STR_CANCEL));
	btnCancel->setFixedSize(80, 30);
	btnCancel->setVisible(true);
	btnCancel->setStyleSheet(STYLE_MANAGER->buttonNormal);
	layoutV_bottom_1->addWidget(btnCancel, Qt::AlignRight);
	layoutV_bottom_1->setAlignment(Qt::AlignRight);
	layoutH_bottom->addLayout(layoutV_bottom_1);

	connect(btnOK, &QPushButton::clicked, this, &Stamp3dDlg::OnOK);
	connect(btnCancel, &QPushButton::clicked, this, &Stamp3dDlg::OnCancel);
	//connect(LE_Size_H_2_0, &QLineEdit::editingFinished, this, &Stamp3dDlg::OnTextChanged);
	//connect(LE_Size_H_2_0, &QLineEdit::returnPressed, this, &Stamp3dDlg::OnTextChanged);
	connect(LE_Size_H_2_0, &QLineEdit::textChanged, this, &Stamp3dDlg::OnTextChanged);

	//connect(LE_Size_H_2_1, &QLineEdit::editingFinished, this, &Stamp3dDlg::OnTextChanged);
	//connect(LE_Size_H_2_1, &QLineEdit::returnPressed, this, &Stamp3dDlg::OnTextChanged);
	connect(LE_Size_H_2_1, &QLineEdit::textChanged, this, &Stamp3dDlg::OnTextChanged);

	connect(sSlider0.pSlidier, &QSlider::valueChanged, this, &Stamp3dDlg::OnDepthSlideReleased);
	connect(sSlider1.pSlidier, &QSlider::valueChanged, this, &Stamp3dDlg::OnDepthSlideReleased);
	connect(btnChangeFontStyle, &QPushButton::clicked, this, &Stamp3dDlg::OnChangeFontStyle);
	connect(btnMakeFont3D, &QPushButton::clicked, this, &Stamp3dDlg::OnMakefont3D);
	connect(btnUpdateStamp, &QPushButton::clicked, this, &Stamp3dDlg::OnUpdateStamp3D);

	listLE.push_back(LE_Size_H_2_0);
	listLE.push_back(LE_Size_H_2_1);
	listLE.push_back(m_pTextLineEdit);
	listSd.push_back(sSlider0);
	listSd.push_back(sSlider1);

	setLayout(layoutV);

	setSliderValue(QString("Font Size"), 6);
	setLineEditValue(QString("Font Size"), 6);

	MESH_RENDER_MANAGER->getFontControl()->setVisible(false);
}

void Stamp3dDlg::initStampData()
{
#if STAMP3D_TEST_FONTMESH_VISIBLE == 0
	if (m_fontMesh != nullptr)
	{
		MESH_RENDER_MANAGER->setTempMesh(nullptr);
		SAFE_DELETE(m_fontMesh);
	}
#endif
	float fOffset = -2.0f;
	int  size = 6;
	SetOffsetDist(fOffset);
	SetPickFaceIdx(-1);
	SetPickPos(mip::VECTOR3(0, 0, 0));
	SetTargetMesh(nullptr);
	SetFontMesh(nullptr);
	std::vector<mip::TVert> trash;
	m_orgFontTverts.swap(trash);

	setSliderValue(QString("Offset"), fOffset);
	setLineEditValue(QString("Offset"), fOffset);

	setSliderValue(QString("Font Size"), size);
	setLineEditValue(QString("Font Size"), size);
	MESH_RENDER_MANAGER->getFontControl()->setVisible(false);
}



double Stamp3dDlg::calcEdgeLength(mip::MeshTopology* pMT)
{
	return 0.0f;
}

void Stamp3dDlg::exitDialog()
{
	mip::MeshTopology* pTempMesh = MESH_RENDER_MANAGER->getTempMesh();
	mip::MeshTopology* pTempParents = MESH_RENDER_MANAGER->getTempParents();
	if (pTempParents != nullptr && pTempParents->m_ttris.size() > 0)
	{
		pTempParents->initShader(mip::SHADERTYPE::SHADER_PONG);
		WIN_MANAGER->buildRenderBufferTopology(pTempParents);
	}
	MESH_WORK_MANAGER->setWorkMode(MESH_WORK_NONE);
	MESH_RENDER_MANAGER->setTempMesh(nullptr);

	auto btn_Stamp = MESH_BTN_SCENE_MANAGER->m_BtnStamp3d;
	btn_Stamp->setChecked(false);

	initStampData();
}

void Stamp3dDlg::OnTextChanged()
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
		if (name == QString("Offset"))
			SetOffsetDist(tmpVal);
		else if (name == QString("Font Size"))
			SetFontSize(tmpVal);
	}

	sSlider.pSlidier->blockSignals(true);
	sSlider.pSlidier->setValue((tmpVal));
	sSlider.pSlidier->blockSignals(false);
}

void Stamp3dDlg::OnDepthSlideReleased(int val)
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
		SetOffsetDist(tmpVal);
	else if (name == QString("Font Size"))
		SetFontSize(tmpVal);
}

void Stamp3dDlg::OnComboChanged(int index)
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

void Stamp3dDlg::OnChangeFontStyle()
{
#if 1
	QFontDialog dlgFont(nullptr);

	if (QDialog::Accepted == dlgFont.exec())
	{
		m_Font = dlgFont.currentFont();
		qreal size = m_Font.pointSizeF();
		m_FontSize = size;

		setSliderValue(QString("Font Size"), m_FontSize);
		setLineEditValue(QString("Font Size"), m_FontSize);
		OnMakefont3D();
	}
#endif

#if 1
#endif
}

void Stamp3dDlg::OnUpdateStamp3D()
{
	if (m_fontMesh == nullptr || GetPickFaceIdx() < 0 || GetOffsetDist() == 0 || !GetStampEnable() || m_FontSize < 1) return;

	//ACTION_MANAGER->action_UndoRedo_update(MESH_WORK_STAMP3D);
	// ----------------------------------
	// offset, Boolean
	// ----------------------------------
#if 1
	float fOffset = 0.0f;
	if (GetOffsetDist() > 0)
	{
		mip::VECTOR3 normalAvg(0, 0, 0);
		for (int ii = 0; ii < GetFontMesh()->m_ttris.size(); ii++)
		{
			mip::VECTOR3	Normal;
			mip::VECTOR3	Xyz[3];

			int v0 = GetFontMesh()->m_ttris[ii].vi[0];
			int v1 = GetFontMesh()->m_ttris[ii].vi[1];
			int v2 = GetFontMesh()->m_ttris[ii].vi[2];

			Xyz[0] = GetFontMesh()->m_tverts[v0].pos;
			Xyz[1] = GetFontMesh()->m_tverts[v1].pos;
			Xyz[2] = GetFontMesh()->m_tverts[v2].pos;

			Normal = (Xyz[1] - Xyz[0]).cross((Xyz[2] - Xyz[0]));
			Normal.normalize();

			normalAvg = normalAvg + Normal;
		}
		normalAvg.normalize();

		fOffset = STAMP3D_OFFSET_ADJUSTMENT * 2;
		for (int ii = 0; ii < m_fontMesh->m_tverts.size(); ii++)
		{
			m_fontMesh->m_tverts[ii].pos = m_fontMesh->m_tverts[ii].pos + (normalAvg * fOffset);
		}
	}
#endif

	GetFontMesh()->updateVertex();
#if 0	
	GetFontMesh()->initShader(mip::SHADER_PONG);

	WIN_MANAGER->makeCurrent();
	GetFontMesh()->buildRenderBufferTopology();
	WIN_MANAGER->doneCurrent();
#endif

	bool bIntaglio = true; //음각
	float Medip_Millimeter = 0.1f;
	fOffset = (GetOffsetDist() * Medip_Millimeter) + (GetOffsetDist() < 0 ? -STAMP3D_OFFSET_ADJUSTMENT : STAMP3D_OFFSET_ADJUSTMENT);

#if SUPPORT_STAMP3D == 1
	ACTION_MANAGER->action_Mesh_Stamp3D(GetTargetMesh(), GetFontMesh(), GetPickFaceIdx(), fOffset, bIntaglio);
#endif

	MESH_RENDER_MANAGER->setTempMesh(nullptr);
	WIN_MANAGER->updateMeshUI();
}

void Stamp3dDlg::OnMakefont3D()
{
	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	QImage temp;
	QString str = "";
	str = m_pTextLineEdit->text();
	if (str.length() < 1 || GetPickFaceIdx() < 0 || GetTargetMesh() == nullptr || m_FontSize < 1)
	{
		printf_s("\n [OnUpdateStamp3D] Exception ..!");
		//QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

#if STAMP3D_TEST_FONTMESH_VISIBLE == 0
	if (m_fontMesh != nullptr)
	{
		MESH_RENDER_MANAGER->setTempMesh(nullptr);
		SAFE_DELETE(m_fontMesh);
		m_fontMesh = nullptr;
	}
#endif

	// ----------------------------------
	// create fontMesh 
	// ----------------------------------

	CMesh3DFontManager::Create2DImage(temp, str, m_Font, STAMP3D_DIMENSION);

	Model3D m3d;
	CMesh3DFontManager FM(temp, m3d);

	std::vector<mip::VECTOR3> _PointXYZ;	//중복안된 정점 리스트
	std::vector<float> _Point;
	std::vector<int> _Index;
	FM.Create2DFontPlane(_PointXYZ, _Point, _Index, m_FontSize);

	m_fontMesh = new mip::MeshTopology(g_Renderer);
	MESH_RENDER_MANAGER->setTempMesh(m_fontMesh);

	m_fontMesh->m_verts.reserve(_Index.size());
	m_fontMesh->m_normals.reserve(_Index.size());
	for (int ii = 0; ii < _Index.size(); ii++)
	{
		int idx = _Index[ii];

		m_fontMesh->m_verts.push_back(_PointXYZ[idx]);
	}

	for (int ii = 0; ii < m_fontMesh->m_verts.size(); ii += 3)
	{
		mip::VECTOR3	Normal;
		mip::VECTOR3	Xyz[3];

		Xyz[0] = m_fontMesh->m_verts[ii];
		Xyz[1] = m_fontMesh->m_verts[ii + 1];
		Xyz[2] = m_fontMesh->m_verts[ii + 2];

		Normal = (Xyz[1] - Xyz[0]).cross((Xyz[2] - Xyz[0]));

		m_fontMesh->m_normals.push_back(Normal);
		m_fontMesh->m_normals.push_back(Normal);
		m_fontMesh->m_normals.push_back(Normal);
	}

	for (int ii = 0; ii < m_fontMesh->m_verts.size(); ii++)
	{
		m_fontMesh->m_tris.push_back(ii);
	}

	m_fontMesh->setRotate(GetTargetMesh()->rotation);
	m_fontMesh->setTranslate(GetTargetMesh()->translation);
	m_fontMesh->setScale(GetTargetMesh()->scale);

	//fontMat *= targetMat;

	m_fontMesh->mergingVertex();
	m_fontMesh->buildTopologyHEdge();
	SetFontMesh(m_fontMesh);
	printf_s("\n [OnUpdateStamp3D] create fontMesh..!");

	// ----------------------------------
	// Move fontMesh 
	// ----------------------------------

	mip::VECTOR3 center = m_fontMesh->m_boundingBox.getCenter();

	for (int ii = 0; ii < m_fontMesh->m_tverts.size(); ii++)
	{
		m_fontMesh->m_tverts[ii].pos = m_fontMesh->m_tverts[ii].pos - center;
	}

	m_orgFontTverts.assign(m_fontMesh->m_tverts.size(), mip::VECTOR3(0, 0, 0));
	std::copy(m_fontMesh->m_tverts.begin(), m_fontMesh->m_tverts.end(), m_orgFontTverts.begin());
	SetOrgFontBox(m_fontMesh->m_boundingBox);

	float fOffset = 0.0f;
	if (GetFontMesh() != nullptr)
	{
		std::vector<mip::VECTOR3> pList;
		bool bAttached = mip::mesh_control::MoveFontPolygon(GetTargetMesh(), GetFontMesh(), GetOrgFontTVerts(),
			GetPickPos(), GetPickFaceIdx(), fOffset, pList);

		MESH_RENDER_MANAGER->getFontControl()->MoveCtrlToMeshSurface(GetTargetMesh(), GetPickPos(), GetPickFaceIdx());

		SetStampEnable(bAttached);
		if (!bAttached)
		{
			for (int ii = 0; ii < GetFontMesh()->m_tverts.size(); ii++)
			{
				GetFontMesh()->m_tverts[ii].pos = mip::VECTOR3(0, 0, 0);
			}
		}
		else
		{

			mip::VECTOR3 normalAvg(0, 0, 0);
			for (int ii = 0; ii < GetFontMesh()->m_ttris.size(); ii++)
			{
				mip::VECTOR3	Normal;
				mip::VECTOR3	Xyz[3];

				int v0 = GetFontMesh()->m_ttris[ii].vi[0];
				int v1 = GetFontMesh()->m_ttris[ii].vi[1];
				int v2 = GetFontMesh()->m_ttris[ii].vi[2];

				Xyz[0] = GetFontMesh()->m_tverts[v0].pos;
				Xyz[1] = GetFontMesh()->m_tverts[v1].pos;
				Xyz[2] = GetFontMesh()->m_tverts[v2].pos;

				Normal = (Xyz[1] - Xyz[0]).cross((Xyz[2] - Xyz[0]));
				Normal.normalize();


				normalAvg = normalAvg + Normal;
			}
			normalAvg.normalize();

			for (int ii = 0; ii < GetFontMesh()->m_tverts.size(); ii++)
			{
				GetFontMesh()->m_tverts[ii].pos = GetFontMesh()->m_tverts[ii].pos + (normalAvg * -STAMP3D_OFFSET_ADJUSTMENT);
			}
		}

#if 1
		if (bAttached)
		{
			GetFontMesh()->updateVertex();
			GetFontMesh()->initShader(mip::SHADER_PONG);

			WIN_MANAGER->makeCurrent();
			GetFontMesh()->buildRenderBufferTopology();
			WIN_MANAGER->doneCurrent();
		}
#endif
	}

	MESH_RENDER_MANAGER->getFontControl()->setVisible(true);
	WIN_MANAGER->mainMeshWidget->getMainView()->update();

	// ----------------------------------
	// Add Layer into MeshList 
	// ----------------------------------
#if 0
	// create mesh info
	WIN_MANAGER->volume_data.CreateMeshInfo();
	muint32 uid_font = WIN_MANAGER->volume_data.GetCurrentMeshIndex();
	WIN_MANAGER->volume_data.SetMeshName(QString("Tempfont"), uid_font);
	MeshInfo* info_font = WIN_MANAGER->volume_data.GetMeshInfo(uid_font);

	if (info_font)
	{
		info_font->upScale = false;
		info_font->uid = uid_font;
	}

	// push pck true 
	WIN_MANAGER->vt_pckID.push_back(false);

	WIN_MANAGER->volume_data.InsertMesh(uid_font, m_fontMesh);
	WIN_MANAGER->volume_data.MeshRenderUpdate(uid_font);
	WIN_MANAGER->updateMeshUI();
#endif

	return;
}


void Stamp3dDlg::OnOK()
{
	if (!ACTION_MANAGER->isActionFinished()) return;

	mip::MeshTopology* pTempMesh = MESH_RENDER_MANAGER->getTempMesh();
	mip::MeshTopology* pTempParents = MESH_RENDER_MANAGER->getTempParents();

	if (pTempMesh == nullptr || pTempParents == nullptr)
	{
		close();
		return;
	}

	mip::MATRIX44 mat_trans = pTempMesh->getMatrix() * pTempParents->getMatrix().inverse();

	auto zero = mip::VECTOR3::Zero;

	for (int i = 0; i < pTempMesh->m_tverts.size(); ++i)
	{
		if (pTempMesh->m_tverts[i] == zero)
		{
			continue;
		}

		pTempMesh->m_tverts[i].pos = mip::math::Vector4Transform(pTempMesh->m_tverts[i].pos, mat_trans);
	}

	mip::mesh_control::hollowOk(pTempParents, pTempMesh);

	pTempParents->buildTree();
	pTempParents->updateVertex();
	pTempParents->updateColor(pTempParents->m_baseColor);
	pTempParents->initShader(mip::SHADERTYPE::SHADER_PONG);
	WIN_MANAGER->buildRenderBufferTopology(pTempParents);

	auto view = WIN_MANAGER->mainMeshWidget->getMainView();
	view->updateGeometryCount();
	MESH_WORK_MANAGER->setWorkMode(MESH_WORK_NONE);
	MESH_RENDER_MANAGER->delTempMesh();
	close();
}

void Stamp3dDlg::OnCancel()
{
	if (!ACTION_MANAGER->isActionFinished()) return;

	//this->OnReleased();
	exitDialog();
	close();
}
