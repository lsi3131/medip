#include "stdafx.h"
#include "VisualPrintMeshInfoWidget.h"
#include "WindowManager.h"
#include "ResourceManager.h"
#include "StringManager.h"
#include "ActionManager.h"
#include "DataContext.h"
#include "Renderer/MeshTopology.h"

////////////////////////////////////////////////////////////////////////////////////
// Visual Print Mesh Info Widget Class Member Functions - Start
/////////////////////////////////////////////////////////////////////////////////
VisualPrintMeshInfoWidget::VisualPrintMeshInfoWidget(DataContext* pDataContext, muint8 UID, QWidget* parent /*= NULL*/)
	:QWidget(parent),
	UID(UID),
	m_pDataContext(pDataContext)
{
	setAutoFillBackground(true);

	MeshInfo* info = m_pDataContext->m_VisualPrinting_MeshData.GetMeshInfo(UID);

	LayerUID = info->uid;
	meshScale = info->upScale ? 1.f : 10.f;
	//	QString connLayer = QString("NOT CONNECTED");

	QColor col(info->color.r, info->color.g, info->color.b);
	if (LayerUID != -1)
	{
		col = m_pDataContext->volume_data.getMaskColor(LayerUID, true);

		//		connLayer = QString("Connected with [%1]").
		//			arg(m_pDataContext->m_MeshData.getMaskName(LayerUID, true));
	}

	QVBoxLayout* layMain = new QVBoxLayout; //ownership to this
	QHBoxLayout* hLayout = new QHBoxLayout; //ownership to layMain

	setLayout(layMain);
	layMain->addLayout(hLayout);

	labelConn = new QLabel(this);
	//	labelConn->setText(connLayer);
	labelConn->setStyleSheet(QString("color : rgba(%1,%2,%3,255);").arg(col.red()).arg(col.green()).arg(col.blue()));
	hLayout->addWidget(labelConn);

	//////////////////////////////////////////////////////////////////////////
	hLayout = new QHBoxLayout;
	layMain->addLayout(hLayout);

	QLabel* label = new QLabel(this);
	//	label->setText("Transform");

	btnUpdate = new QPushButton(this);
	btnUpdate->setIcon(RESOURCE_MANAGER->getIcon(ICON_NON_RESET));
	btnUpdate->setIconSize(QSize(20, 20));
	btnUpdate->setFixedSize(QSize(20, 20));
	btnUpdate->setMouseTracking(true);
	btnUpdate->installEventFilter(this);
	btnUpdate->setToolTip("Refresh latest info.");
	btnUpdate->setStyleSheet("color : black;");
	btnUpdate->setObjectName("ButtonUpdate");

	connect(btnUpdate, &QPushButton::clicked, this, &VisualPrintMeshInfoWidget::Update);

	btnEdit = new QPushButton(this);
	btnEdit->setIcon(RESOURCE_MANAGER->getIcon(ICON_NON_ANNO_TEXT));//TODO CHANGE EDIT ICON
	btnEdit->setIconSize(QSize(20, 20));
	btnEdit->setFixedSize(QSize(20, 20));
	btnEdit->setMouseTracking(true);
	btnEdit->installEventFilter(this);
	btnEdit->setCheckable(true);
	btnEdit->setChecked(false);
	btnEdit->setToolTip("Edit scale/angle/rotate value");
	btnEdit->setStyleSheet("color : black;");
	btnEdit->setObjectName("ButtonEdit");

	connect(btnEdit, &QPushButton::clicked, this, &VisualPrintMeshInfoWidget::Edit);

	btnApply = new QPushButton(this);
	btnApply->setIcon(RESOURCE_MANAGER->getIcon(ICON_FILE_NON_SAVE));//TODO CHANGE EDIT ICON
	btnApply->setIconSize(QSize(20, 20));
	btnApply->setFixedSize(QSize(20, 20));
	btnApply->setMouseTracking(true);
	btnApply->installEventFilter(this);
	btnApply->setToolTip("Apply");
	btnApply->setStyleSheet("color : black;");
	btnApply->setObjectName("ButtonApply");

	connect(btnApply, &QPushButton::clicked, this, &VisualPrintMeshInfoWidget::Apply);

	QWidget* emptyBox = new QWidget(this);
	emptyBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	//	hLayout->addWidget(label);
	hLayout->addWidget(emptyBox);
	hLayout->addWidget(btnUpdate);
	hLayout->addWidget(btnEdit);
	hLayout->addWidget(btnApply);

	//	mip::MeshCore* m = m_pDataContext->m_MeshData.GetMesh(UID);
	mip::MeshCore* m = m_pDataContext->m_VisualPrinting_MeshData.GetMesh(UID);
	if (m)
	{
		m->createMinMax();
		mip::AABB box = m->m_boundingBox;
	}

	hLayout = new QHBoxLayout;
	layMain->addLayout(hLayout);

	//	label = new QLabel(this);
	//	label->setText("Translate");

	//	hLayout->addWidget(label);

	/*for (int i = 0; i < 3; i++)
	{
	lineTrans[i] = new QDoubleSpinBox(this);

	lineTrans[i]->setButtonSymbols(QDoubleSpinBox::NoButtons);
	lineTrans[i]->setRange(VTK_DOUBLE_MIN, VTK_DOUBLE_MAX);
	lineTrans[i]->setValue(0);
	lineTrans[i]->setStyleSheet(STYLE_MANAGER->editBoxTab);
	lineTrans[i]->setObjectName(QString("SpinTrans%1").arg(i));
	lineTrans[i]->setContextMenuPolicy(Qt::NoContextMenu);
	lineTrans[i]->setMouseTracking(true);
	lineTrans[i]->installEventFilter(this);
	lineTrans[i]->setContentsMargins(0, 0, 0, 0);
	lineTrans[i]->setEnabled(false);
	connect(lineTrans[i], &QDoubleSpinBox::editingFinished, this, &VisualPrintMeshInfoWidget::OnTransChanged);
	hLayout->addWidget(lineTrans[i]);

	label = new QLabel(this);
	if (i < 2)
	label->setText(",");
	else
	label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	label->setContentsMargins(0, 0, 0, 0);
	hLayout->addWidget(label);
	}*/

	hLayout = new QHBoxLayout;
	layMain->addLayout(hLayout);

	//	label = new QLabel(this);
	//	label->setText("Rotate");

	//	hLayout->addWidget(label);

	/*for (int i = 0; i < 3; i++)
	{
	lineRotate[i] = new QDoubleSpinBox(this);

	lineRotate[i]->setButtonSymbols(QDoubleSpinBox::NoButtons);
	lineRotate[i]->setRange(0, 180);
	lineRotate[i]->setValue(0);
	lineRotate[i]->setStyleSheet(STYLE_MANAGER->editBoxTab);
	lineRotate[i]->setObjectName(QString("SpinRotate%1").arg(i));
	lineRotate[i]->setContextMenuPolicy(Qt::NoContextMenu);
	lineRotate[i]->setMouseTracking(true);
	lineRotate[i]->installEventFilter(this);
	lineRotate[i]->setContentsMargins(0, 0, 0, 0);
	lineRotate[i]->setEnabled(false);
	connect(lineRotate[i], &QDoubleSpinBox::editingFinished, this, &VisualPrintMeshInfoWidget::OnRotationChanged);
	hLayout->addWidget(lineRotate[i]);

	label = new QLabel(this);
	if (i < 2)
	label->setText(",");
	else
	label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	label->setContentsMargins(0, 0, 0, 0);
	hLayout->addWidget(label);
	}*/

	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////SCALE & DIMENSIONS/////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	hLayout = new QHBoxLayout;
	layMain->addLayout(hLayout);

	//	label = new QLabel(this);
	//	label->setText("Scale");

	//	hLayout->addWidget(label);

	/*mip::VECTOR3 scales = m->scale;

	float sc[3];
	sc[0] = scales.x;
	sc[1] = scales.y;
	sc[2] = scales.z;

	for (int i = 0; i < 3; i++)
	{
	lineScale[i] = new QDoubleSpinBox(this);

	lineScale[i]->setButtonSymbols(QDoubleSpinBox::NoButtons);
	lineScale[i]->setRange(0.01f, VTK_DOUBLE_MAX);
	lineScale[i]->setValue(sc[i]);
	lineScale[i]->setStyleSheet(STYLE_MANAGER->editBoxTab);
	lineScale[i]->setObjectName(QString("SpinScale%1").arg(i));
	lineScale[i]->setContextMenuPolicy(Qt::NoContextMenu);
	lineScale[i]->setMouseTracking(true);
	lineScale[i]->installEventFilter(this);
	lineScale[i]->setContentsMargins(0, 0, 0, 0);
	lineScale[i]->setEnabled(false);
	connect(lineScale[i], &QDoubleSpinBox::editingFinished, this, &VisualPrintMeshInfoWidget::OnScaleChanged);
	hLayout->addWidget(lineScale[i]);

	label = new QLabel(this);
	if (i < 2)
	label->setText("*");
	else
	label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	label->setContentsMargins(0, 0, 0, 0);
	hLayout->addWidget(label);

	}*/

	hLayout = new QHBoxLayout;
	layMain->addLayout(hLayout);

	//	label = new QLabel(this);
	//	label->setText("Dimension");

	//	hLayout->addWidget(label);

	//mip::VECTOR3 dims = box.getSize();
	//dims *= meshScale;

	//float di[3];
	//di[0] = dims.x;
	//di[1] = dims.y;
	//di[2] = dims.z;

	////mip::MeshCore* m = m_pDataContext->m_MeshData.GetMesh(UID);
	////
	//for (int i = 0; i < 3; i++)
	//{
	//	lineDim[i] = new QDoubleSpinBox(this);

	//	lineDim[i]->setButtonSymbols(QDoubleSpinBox::NoButtons);
	//	lineDim[i]->setRange(0.1f, VTK_DOUBLE_MAX);
	//	lineDim[i]->setValue(di[i]);
	//	lineDim[i]->setStyleSheet(STYLE_MANAGER->editBoxTab);
	//	lineDim[i]->setObjectName(QString("SpinDim%1").arg(i));
	//	lineDim[i]->setContextMenuPolicy(Qt::NoContextMenu);
	//	lineDim[i]->setMouseTracking(true);
	//	lineDim[i]->installEventFilter(this);
	//	lineDim[i]->setContentsMargins(0, 0, 0, 0);
	//	lineDim[i]->setEnabled(false);
	//	connect(lineDim[i], &QDoubleSpinBox::editingFinished, this, &VisualPrintMeshInfoWidget::OnDimChanged);
	//	hLayout->addWidget(lineDim[i]);

	//	label = new QLabel(this);
	//	if (i < 2)
	//		label->setText("*");
	//	else
	//		label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	//	label->setContentsMargins(0, 0, 0, 0);
	//	hLayout->addWidget(label);
	//}

	HoverWidget = NULL;
}

void VisualPrintMeshInfoWidget::ChangeUID(mint8 UID)
{
	this->UID = UID;
	//	Update();
}

void VisualPrintMeshInfoWidget::EditCancel()
{
	Update();

	if (btnEdit->isChecked())
	{
		btnEdit->setChecked(false);
		Edit();
	}

	//	Apply();
}

void VisualPrintMeshInfoWidget::UpdateLayerConnection()
{
	Update();
}

//void VisualPrintMeshInfoWidget::SetDisconnection()
//{
//	LayerUID = -1;
//	labelConn->setText(QString("NOT CONNECTED"));
//	labelConn->update();
//}
//
//void VisualPrintMeshInfoWidget::SetConnection(mint32 LayerUID)
//{
//
//}

bool VisualPrintMeshInfoWidget::eventFilter(QObject* watched, QEvent* e)
{
	static QIcon editIcons[] = { RESOURCE_MANAGER->getIcon(ICON_NON_ANNO_TEXT), RESOURCE_MANAGER->getIcon(ICON_ANNO_TEXT_HOVER) };
	static QIcon saveIcons[] = { RESOURCE_MANAGER->getIcon(ICON_FILE_NON_SAVE), RESOURCE_MANAGER->getIcon(ICON_FILE_SAVE_HOVER) };

	//static QPushButton *HoverWidget = NULL;

	if (watched == nullptr) return QWidget::eventFilter(watched, e);

	if (watched->objectName().contains("Spin"))
	{
		if (e->type() == QEvent::FocusIn)
			WIN_MANAGER->setMoveFocus(false);
		else if (e->type() == QEvent::Scroll ||
			e->type() == QEvent::Wheel)
			return true;
	}

	if (!watched->inherits("QAbstractButton"))
		return QWidget::eventFilter(watched, e);

	QEvent::Type _type = e->type();

	int res = 0;

	if (_type == QEvent::HoverLeave ||
		_type == QEvent::Leave || _type == QEvent::Show)
	{
		if (HoverWidget)
		{
			if (!HoverWidget->isChecked())
				HoverWidget->setIcon(LeaveIcon);
		}
	}

	QWidget* w = dynamic_cast<QWidget*>(watched);
	{
		QMouseEvent* evt = dynamic_cast<QMouseEvent*>(e);
		if (evt)
		{
			if (w)
			{
				if (w->rect().contains(evt->pos()))
					res = 1;
			}
		}
	}
	{
		QHoverEvent* evt = dynamic_cast<QHoverEvent*>(e);
		if (evt)
		{
			if (w)
			{
				if (w->rect().contains(evt->pos()))
					res = 1;
			}
		}
	}
	{
		QWheelEvent* evt = dynamic_cast<QWheelEvent*>(e);
		if (evt)
		{
			if (w)
			{
				if (w->rect().contains(evt->pos()))
					res = 1;
			}
		}
	}

	if (res == 1)
	{
		if (watched == btnEdit && !btnEdit->isChecked())
		{
			btnEdit->setIcon(editIcons[res]);
			HoverWidget = btnEdit;
			LeaveIcon = editIcons[0];
		}
		else if (watched == btnApply && !btnApply->isChecked())
		{
			btnApply->setIcon(saveIcons[res]);
			HoverWidget = btnApply;
			LeaveIcon = saveIcons[0];
		}

	}

	return QWidget::eventFilter(watched, e);
}


void VisualPrintMeshInfoWidget::Update()
{
	MeshInfo* info = m_pDataContext->m_MeshData.GetMeshInfo(UID);
	QColor col(info->color.r, info->color.g, info->color.b);

	LayerUID = info->uid;
	meshScale = info->upScale ? 1.f : 10.f;
	//	QString connLayer = QString("NOT CONNECTED");

	if (LayerUID != -1)
	{
		col = m_pDataContext->volume_data.getMaskColor(LayerUID, true);
		//		connLayer = QString("Connected with [%1]").
		//			arg(m_pDataContext->m_MeshData.getMaskName(LayerUID, true));
	}

	//	labelConn->setText(connLayer);
	labelConn->setStyleSheet(QString("color : rgba(%1,%2,%3,255);").arg(col.red()).arg(col.green()).arg(col.blue()));

	auto m = m_pDataContext->m_MeshData.GetMesh(UID);
	mip::VECTOR3 scales = m->scale;

	float sc[3];
	sc[0] = scales.x;
	sc[1] = scales.y;
	sc[2] = scales.z;

	for (int i = 0; i < 3; i++)
	{
		if (sc[i] == lineScale[i]->value())
			continue;

		lineScale[i]->blockSignals(true);
		lineScale[i]->setValue(sc[i]);
		lineScale[i]->blockSignals(false);
	}

	m->createMinMax();

	mip::AABB box = m->m_boundingBox;

	mip::VECTOR3 dims = box.getSize();

	dims *= meshScale;

	float di[3];
	di[0] = dims.x;
	di[1] = dims.y;
	di[2] = dims.z;

	for (int i = 0; i < 3; i++)
	{
		if (di[i] == lineDim[i]->value())
			continue;

		lineDim[i]->blockSignals(true);
		lineDim[i]->setValue(di[i]);
		lineDim[i]->blockSignals(false);
	}

	for (int i = 0; i < 3; i++)
	{
		lineTrans[i]->blockSignals(true);
		lineTrans[i]->setValue(0);
		lineTrans[i]->blockSignals(false);

		lineRotate[i]->blockSignals(true);
		lineRotate[i]->setValue(0);
		lineRotate[i]->blockSignals(false);
	}
}

void VisualPrintMeshInfoWidget::Edit()
{
	static QIcon editIcons[] = { RESOURCE_MANAGER->getIcon(ICON_NON_ANNO_TEXT), RESOURCE_MANAGER->getIcon(ICON_ANNO_TEXT) };

	WIN_MANAGER->setMoveFocus(true);

	bool chk = btnEdit->isChecked();
	btnEdit->setIcon(editIcons[chk]);

	for (int i = 0; i < 3; i++)
	{
		lineScale[i]->setEnabled(chk);
		lineDim[i]->setEnabled(chk);
		lineTrans[i]->setEnabled(chk);
		lineRotate[i]->setEnabled(chk);
	}

	if (!chk)
		Update();
}

void VisualPrintMeshInfoWidget::Apply()
{
	//static QIcon saveIcons[] = { RESOURCE_MANAGER->getIcon(ICON_FILE_NON_SAVE), RESOURCE_MANAGER->getIcon(ICON_FILE_SAVE) };

	//WIN_MANAGER->setMoveFocus(true);

	//if (!WIN_MANAGER->IsLicensePass())
	//{
	//	EditCancel();
	//	QMessageBox::warning(this, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(STR_LICENSE_WARN));
	//	return;
	//}

	//mip::VECTOR3 Scale = mip::VECTOR3(lineScale[0]->value(), lineScale[1]->value(), lineScale[2]->value());
	//mip::VECTOR3 Trans = mip::VECTOR3(lineTrans[0]->value(), lineTrans[1]->value(), lineTrans[2]->value());
	//mip::MeshCore* m = m_pDataContext->m_MeshData.GetMesh(UID);

	//muint32 cx, cy, cz;
	//m_pDataContext->volume_data.getLengthForScreen(WT_AXIAL, cx, cy, cz);

	//float spX, spY, spZ;
	//m_pDataContext->volume_data.getSpacingForScreen(WT_AXIAL, spX, spY, spZ);


	//if (nullptr == m)
	//	return;

	//btnApply->setChecked(true);
	//btnApply->setIcon(saveIcons[1]);

	//if (m->scale != Scale)
	//{
	//	float scal[3];

	//	for (int i = 0; i < 3; i++)
	//		scal[i] = lineScale[i]->value();

	//	ACTION_MANAGER->action_MeshList_Scale_change(UID, scal);
	//}

	//Trans /= meshScale;

	//if (Trans != Trans.Zero)
	//	ACTION_MANAGER->action_MeshList_Translation_change(UID, Trans);

	//bool chk = false;
	//float fRot[3];

	//for (int i = 0; i < 3; i++)
	//{
	//	fRot[i] = lineRotate[i]->value();

	//	if (!chk)
	//		chk = fRot[i] != 0;
	//}

	//if (chk)
	//	ACTION_MANAGER->action_MeshList_Rotation_change(UID, fRot);//todo


	//btnApply->setIcon(saveIcons[0]);
	//btnApply->setChecked(false);
}

void VisualPrintMeshInfoWidget::OnDimChanged()
{
	//WIN_MANAGER->setMoveFocus(true);

	//double tmpVal;

	//mip::MeshCore* m = m_pDataContext->m_MeshData.GetMesh(UID);

	////m->createMinMax();
	//mip::AABB box = m->m_boundingBox;

	//mip::VECTOR3 dims = box.getSize();
	//float di[3];
	//di[0] = dims.x;
	//di[1] = dims.y;
	//di[2] = dims.z;

	//for (int i = 0; i < 3; i++)
	//{
	//	tmpVal = lineDim[i]->value();

	//	if (tmpVal < 0.01f)
	//		tmpVal = 0.01f;

	//	if (tmpVal != lineDim[i]->value())
	//	{
	//		lineDim[i]->blockSignals(true);
	//		lineDim[i]->setValue(tmpVal);
	//		lineDim[i]->blockSignals(false);
	//	}

	//	tmpVal /= meshScale;
	//	tmpVal /= di[i];

	//	if (tmpVal != lineScale[i]->value())
	//	{
	//		lineScale[i]->blockSignals(true);
	//		lineScale[i]->setValue(tmpVal);
	//		lineScale[i]->blockSignals(false);
	//	}
	//}
}

void VisualPrintMeshInfoWidget::OnTransChanged()
{
	WIN_MANAGER->setMoveFocus(true);
}

void VisualPrintMeshInfoWidget::OnRotationChanged()
{
	WIN_MANAGER->setMoveFocus(true);

	double tmpVal;

	for (int i = 0; i < 3; i++)
	{
		const double curVal = tmpVal = lineRotate[i]->value();

		if (0 > tmpVal)
			tmpVal = 0;
		else if (180 < tmpVal)
			tmpVal = 180;

		if (curVal != tmpVal)
		{
			lineRotate[i]->blockSignals(true);
			lineRotate[i]->setValue(tmpVal);
			lineRotate[i]->blockSignals(false);
		}
	}
}


//void VisualPrintMeshInfoWidget::OnAngleChanged()
//{
//	WIN_MANAGER->setMoveFocus(true);
//}

//void VisualPrintMeshInfoWidget::OnRotationChanged()
//{
//	WIN_MANAGER->setMoveFocus(true);
//}

void VisualPrintMeshInfoWidget::OnScaleChanged()
{
	//WIN_MANAGER->setMoveFocus(true);

	//double tmpVal;

	//mip::MeshCore* m = m_pDataContext->m_MeshData.GetMesh(UID);

	////m->createMinMax();
	//mip::AABB box = m->m_boundingBox;

	//mip::VECTOR3 dims = box.getSize();
	//float oldDim[3];
	//oldDim[0] = dims.x;
	//oldDim[1] = dims.y;
	//oldDim[2] = dims.z;

	//double newDim;
	//for (int i = 0; i < 3; i++)
	//{
	//	tmpVal = lineScale[i]->value();
	//	oldDim[i] *= meshScale;

	//	if (tmpVal < 0.01f)
	//		tmpVal = 0.01f;

	//	if (tmpVal != lineScale[i]->value())
	//	{
	//		lineScale[i]->blockSignals(true);
	//		lineScale[i]->setValue(tmpVal);
	//		lineScale[i]->blockSignals(false);
	//	}
	//	newDim = tmpVal * oldDim[i];

	//	if (newDim != oldDim[i])
	//	{
	//		lineDim[i]->blockSignals(true);
	//		lineDim[i]->setValue(newDim);
	//		lineDim[i]->blockSignals(false);
	//	}
	//}
}
/////////////////////////////////////////////////////////////////////////
// Visual Print Mesh Info Widget Class Member Functions - End
//////////////////////////////////////////////////////////////////////

