#include "stdafx.h"
#include "TrWidget.h"
#include "windowManager.h"
#include "resourceManager.h"
#include "styleManager.h"
#include "stringManager.h"
#include "ActionManager.h"
#include "MeshControl.h"
#include "DataContext.h"

TrWidget::TrWidget(muint8 UID, DataContext* pDataContext, QWidget* parent /*= NULL*/)
	:QWidget(parent),
	UID(UID),
	m_pDataContext(pDataContext)
{
	setAutoFillBackground(true);

	MeshInfo* info = DATA_CONTEXT->m_MeshData.GetMeshInfo(UID);

	LayerUID = info->uid;
	meshScale = info->upScale ? 1.f : 10.f;
	QString connLayer = QString("NOT CONNECTED");

	QColor col(info->color.r, info->color.g, info->color.b);
	if (LayerUID != -1)
	{
		col = DATA_CONTEXT->volume_data.getMaskColor(LayerUID, true);

		connLayer = QString("Connected with [%1]").
			arg(DATA_CONTEXT->volume_data.getMaskName(LayerUID, true));
	}

	QVBoxLayout* layMain = new QVBoxLayout; //ownership to this
	QHBoxLayout* hLayout = new QHBoxLayout; //ownership to layMain

	setLayout(layMain);
	layMain->addLayout(hLayout);

	labelConn = new QLabel(this);
	labelConn->setText(connLayer);
	labelConn->setStyleSheet(QString("color : rgba(%1,%2,%3,255);").arg(col.red()).arg(col.green()).arg(col.blue()));
	hLayout->addWidget(labelConn);

	//////////////////////////////////////////////////////////////////////////
	hLayout = new QHBoxLayout;
	layMain->addLayout(hLayout);

	QLabel* label = new QLabel(this);
	label->setText("Transform");

	btnUpdate = new QPushButton(this);
	btnUpdate->setIcon(RESOURCE_MANAGER->getIcon(ICON_NON_RESET));
	btnUpdate->setIconSize(QSize(20, 20));
	btnUpdate->setFixedSize(QSize(20, 20));
	btnUpdate->setMouseTracking(true);
	btnUpdate->installEventFilter(this);
	btnUpdate->setToolTip("Refresh latest info.");
	btnUpdate->setStyleSheet("color : black;");
	btnUpdate->setObjectName("ButtonUpdate");

	connect(btnUpdate, &QPushButton::clicked, this, &TrWidget::Update);

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

	connect(btnEdit, &QPushButton::clicked, this, &TrWidget::Edit);

	btnApply = new QPushButton(this);
	btnApply->setIcon(RESOURCE_MANAGER->getIcon(ICON_FILE_NON_SAVE));//TODO CHANGE EDIT ICON
	btnApply->setIconSize(QSize(20, 20));
	btnApply->setFixedSize(QSize(20, 20));
	btnApply->setMouseTracking(true);
	btnApply->installEventFilter(this);
	btnApply->setToolTip("Apply");
	btnApply->setStyleSheet("color : black;");
	btnApply->setObjectName("ButtonApply");

	connect(btnApply, &QPushButton::clicked, this, &TrWidget::Apply);

	QWidget* emptyBox = new QWidget(this);
	emptyBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	hLayout->addWidget(label);
	hLayout->addWidget(emptyBox);
	hLayout->addWidget(btnUpdate);
	hLayout->addWidget(btnEdit);
	hLayout->addWidget(btnApply);

	mip::MeshTopology* m = DATA_CONTEXT->m_MeshData.GetMesh(UID);
	if (m == NULL)
	{
		return;
	}
	m->createMinMax();
	mip::AABB box = m->m_boundingBox;

	hLayout = new QHBoxLayout;
	layMain->addLayout(hLayout);

	label = new QLabel(this);
	label->setText("Translate");
	label->setFixedSize(70, 20);

	hLayout->addWidget(label);

	for (int i = 0; i < 3; i++)
	{
		//lineTrans[i] = new QDoubleSpinBox(this);
		//lineTrans[i]->setButtonSymbols(QDoubleSpinBox::NoButtons);
		//lineTrans[i]->setRange(VTK_DOUBLE_MIN, VTK_DOUBLE_MAX);
		//lineTrans[i]->setValue(0);
		lineTrans[i] = new QLineEdit("0.0", this);
		lineTrans[i]->setValidator(new QDoubleValidator(VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 2, this));
		lineTrans[i]->setStyleSheet(STYLE_MANAGER->editBoxTab);
		lineTrans[i]->setObjectName(QString("SpinTrans%1").arg(i));
		lineTrans[i]->setContextMenuPolicy(Qt::NoContextMenu);
		lineTrans[i]->setMouseTracking(true);
		lineTrans[i]->installEventFilter(this);
		lineTrans[i]->setContentsMargins(0, 0, 0, 0);
		//lineTrans[i]->setEnabled(false);
		//connect(lineTrans[i], &QDoubleSpinBox::editingFinished, this, &TrWidget::OnTransChanged);
		lineTrans[i]->setFixedSize(60, 20);
		connect(lineTrans[i], &QLineEdit::editingFinished, this, &TrWidget::OnTransChanged);
		hLayout->addWidget(lineTrans[i]);

		label = new QLabel(this);
		if (i < 2)
			label->setText(",");
		else
			label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		label->setContentsMargins(0, 0, 0, 0);
		hLayout->addWidget(label);
	}

	hLayout = new QHBoxLayout;
	layMain->addLayout(hLayout);

	label = new QLabel(this);
	label->setText("Rotate");
	label->setFixedSize(70, 20);

	hLayout->addWidget(label);

	for (int i = 0; i < 3; i++)
	{
		//lineRotate[i] = new QDoubleSpinBox(this);

		//lineRotate[i]->setButtonSymbols(QDoubleSpinBox::NoButtons);
		//lineRotate[i]->setRange(0, 180);
		//lineRotate[i]->setValue(0);
		lineRotate[i] = new QLineEdit("0.0", this);
		QDoubleValidator* doubleValidator = new QDoubleValidator(0.00, 360.00, 2);
		doubleValidator->setNotation(QDoubleValidator::StandardNotation);
		lineRotate[i]->setValidator(doubleValidator);
		lineRotate[i]->setStyleSheet(STYLE_MANAGER->editBoxTab);
		lineRotate[i]->setObjectName(QString("SpinRotate%1").arg(i));
		lineRotate[i]->setContextMenuPolicy(Qt::NoContextMenu);
		lineRotate[i]->setMouseTracking(true);
		lineRotate[i]->installEventFilter(this);
		lineRotate[i]->setContentsMargins(0, 0, 0, 0);
		lineRotate[i]->setEnabled(true);
		//connect(lineRotate[i], &QDoubleSpinBox::editingFinished, this, &TrWidget::OnRotationChanged);
		lineRotate[i]->setFixedSize(60, 20);
		connect(lineRotate[i], &QLineEdit::editingFinished, this, &TrWidget::OnRotationChanged);
		hLayout->addWidget(lineRotate[i]);

		label = new QLabel(this);
		if (i < 2)
			label->setText(",");
		else
			label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		label->setContentsMargins(0, 0, 0, 0);
		hLayout->addWidget(label);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////SCALE & DIMENSIONS/////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	hLayout = new QHBoxLayout;
	layMain->addLayout(hLayout);

	label = new QLabel(this);
	label->setText("Scale");
	label->setFixedSize(70, 20);

	hLayout->addWidget(label);

	mip::VECTOR3 scales = m->scale;

	float sc[3];
	sc[0] = scales.x;
	sc[1] = scales.y;
	sc[2] = scales.z;

	for (int i = 0; i < 3; i++)
	{
		//lineScale[i] = new QDoubleSpinBox(this);

		//lineScale[i]->setButtonSymbols(QDoubleSpinBox::NoButtons);
		//lineScale[i]->setRange(0.01f, VTK_DOUBLE_MAX);
		//lineScale[i]->setValue(sc[i]);
		lineScale[i] = new QLineEdit(QString().sprintf("%.4lf", (double)sc[i]), this);
		lineScale[i]->setValidator(new QDoubleValidator(0.01, VTK_DOUBLE_MAX, 2, this));
		lineScale[i]->setStyleSheet(STYLE_MANAGER->editBoxTab);
		lineScale[i]->setObjectName(QString("SpinScale%1").arg(i));
		lineScale[i]->setContextMenuPolicy(Qt::NoContextMenu);
		lineScale[i]->setMouseTracking(true);
		lineScale[i]->installEventFilter(this);
		lineScale[i]->setContentsMargins(0, 0, 0, 0);
		lineScale[i]->setEnabled(true);
		//connect(lineScale[i], &QDoubleSpinBox::editingFinished, this, &TrWidget::OnScaleChanged);
		lineScale[i]->setFixedSize(60, 20);
		connect(lineScale[i], &QLineEdit::editingFinished, this, &TrWidget::OnScaleChanged);
		hLayout->addWidget(lineScale[i]);

		label = new QLabel(this);
		if (i < 2)
			label->setText("*");
		else
			label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		label->setContentsMargins(0, 0, 0, 0);
		hLayout->addWidget(label);

	}

	hLayout = new QHBoxLayout;
	layMain->addLayout(hLayout);

	label = new QLabel(this);
	label->setText("Dimension");
	label->setFixedSize(70, 20);

	hLayout->addWidget(label);

	mip::VECTOR3 dims = box.getSize();
	dims *= meshScale;

	float di[3];
	di[0] = dims.x;
	di[1] = dims.y;
	di[2] = dims.z;

	//mip::MeshTopology* m = DATA_CONTEXT->m_MeshData.GetMesh(UID);
	//
	for (int i = 0; i < 3; i++)
	{
		//lineDim[i] = new QDoubleSpinBox(this);

		//lineDim[i]->setButtonSymbols(QDoubleSpinBox::NoButtons);
		//lineDim[i]->setRange(0.1f, VTK_DOUBLE_MAX);
		//lineDim[i]->setValue(di[i]);
		lineDim[i] = new QLineEdit(QString().sprintf("%.2f", di[i]), this);
		lineDim[i]->setValidator(new QDoubleValidator(0.1f, VTK_DOUBLE_MAX, 2, this));
		lineDim[i]->setStyleSheet(STYLE_MANAGER->editBoxTab);
		lineDim[i]->setObjectName(QString("SpinDim%1").arg(i));
		lineDim[i]->setContextMenuPolicy(Qt::NoContextMenu);
		lineDim[i]->setMouseTracking(true);
		lineDim[i]->installEventFilter(this);
		lineDim[i]->setContentsMargins(0, 0, 0, 0);
		lineDim[i]->setEnabled(true);
		//connect(lineDim[i], &QDoubleSpinBox::editingFinished, this, &TrWidget::OnDimChanged);
		lineDim[i]->setFixedSize(60, 20);
		connect(lineDim[i], &QLineEdit::editingFinished, this, &TrWidget::OnDimChanged);
		hLayout->addWidget(lineDim[i]);

		label = new QLabel(this);
		if (i < 2)
			label->setText("*");
		else
			label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		label->setContentsMargins(0, 0, 0, 0);
		hLayout->addWidget(label);
	}

	HoverWidget = NULL;

}

void TrWidget::ChangeUID(mint8 UID)
{
	this->UID = UID;
	//	Update();
}

void TrWidget::EditCancel()
{
	Update();

	if (btnEdit->isChecked())
	{
		btnEdit->setChecked(false);
		Edit();
	}

	//	Apply();
}

void TrWidget::UpdateLayerConnection()
{
	Update();
}

//void TrWidget::SetDisconnection()
//{
//	LayerUID = -1;
//	labelConn->setText(QString("NOT CONNECTED"));
//	labelConn->update();
//}
//
//void TrWidget::SetConnection(mint32 LayerUID)
//{
//
//}

bool TrWidget::eventFilter(QObject* watched, QEvent* e)
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
			// 201007 허 건대리 주석처리
			// Preview Surface 수행 중, Mesh list 계속 클릭시 프로그램 종료 발생
			//if (!HoverWidget->isChecked())
			{
				if (!LeaveIcon.isNull())
				{
					HoverWidget->setIcon(LeaveIcon);
				}
			}
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


void TrWidget::Update()
{
	printf_s("Pass\n");
	MeshInfo* info = DATA_CONTEXT->m_MeshData.GetMeshInfo(UID);
	QColor col(info->color.r, info->color.g, info->color.b);

	LayerUID = info->uid;
	meshScale = info->upScale ? 1.f : 10.f;
	QString connLayer = QString("NOT CONNECTED");

	if (LayerUID != -1)
	{
		col = DATA_CONTEXT->volume_data.getMaskColor(LayerUID, true);
		connLayer = QString("Connected with [%1]").
			arg(DATA_CONTEXT->volume_data.getMaskName(LayerUID, true));
	}

	labelConn->setText(connLayer);
	labelConn->setStyleSheet(QString("color : rgba(%1,%2,%3,255);").arg(col.red()).arg(col.green()).arg(col.blue()));

	mip::MeshTopology* m = DATA_CONTEXT->m_MeshData.GetMesh(UID);
	mip::VECTOR3 scales = m->scale;

	float sc[3];
	sc[0] = scales.x;
	sc[1] = scales.y;
	sc[2] = scales.z;

	for (int i = 0; i < 3; i++)
	{
		//if (sc[i] == lineScale[i]->value())
		if (sc[i] == lineScale[i]->text().toDouble())
			continue;

		lineScale[i]->blockSignals(true);
		//lineScale[i]->setValue(sc[i]);
		lineScale[i]->setText(QString().sprintf("%.2f", sc[i]));
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
		//if (di[i] == lineDim[i]->value())
		if (di[i] == lineDim[i]->text().toDouble())
			continue;

		lineDim[i]->blockSignals(true);
		//lineDim[i]->setValue(di[i]);
		lineDim[i]->setText(QString().sprintf("%.2f", di[i]));
		lineDim[i]->blockSignals(false);
	}

	for (int i = 0; i < 3; i++)
	{
		lineTrans[i]->blockSignals(true);
		//lineTrans[i]->setValue(0);
		lineTrans[i]->setText("0.0");
		lineTrans[i]->blockSignals(false);

		lineRotate[i]->blockSignals(true);
		//lineRotate[i]->setValue(0);
		lineRotate[i]->setText("0.0");
		lineRotate[i]->blockSignals(false);
	}

}

void TrWidget::Edit()
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

void TrWidget::Apply()
{
	static QIcon saveIcons[] = { RESOURCE_MANAGER->getIcon(ICON_FILE_NON_SAVE), RESOURCE_MANAGER->getIcon(ICON_FILE_SAVE) };

	WIN_MANAGER->setMoveFocus(true);

	if (!WIN_MANAGER->IsLicensePass())
	{
		EditCancel();
		QMessageBox::warning(this, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(STR_LICENSE_WARN));
		return;
	}

	//mip::VECTOR3 Scale = mip::VECTOR3(lineScale[0]->value(), lineScale[1]->value(), lineScale[2]->value());
	//mip::VECTOR3 Trans = mip::VECTOR3(lineTrans[0]->value(), lineTrans[1]->value(), lineTrans[2]->value());
	mip::VECTOR3 Scale = mip::VECTOR3(lineScale[0]->text().toDouble(), lineScale[1]->text().toDouble(), lineScale[2]->text().toDouble());
	mip::VECTOR3 Trans = mip::VECTOR3(lineTrans[0]->text().toDouble(), lineTrans[1]->text().toDouble(), lineTrans[2]->text().toDouble());
	mip::MeshTopology* m = DATA_CONTEXT->m_MeshData.GetMesh(UID);

	auto& tverts = m->m_tverts;
	int n_verts = (int)tverts.size();

	muint32 cx, cy, cz;
	DATA_CONTEXT->volume_data.getLengthForScreen(WT_AXIAL, cx, cy, cz);

	float spX, spY, spZ;
	DATA_CONTEXT->volume_data.getSpacingForScreen(WT_AXIAL, spX, spY, spZ);


	if (nullptr == m)
		return;

	if (m_OrgCenter == m_OrgCenter.Zero)
	{
		mip::mesh_control::getMinMax(m->m_verts, m->m_boundingBox.min, m->m_boundingBox.max);
		m_OrgCenter = m->m_boundingBox.getCenter();
	}

	bool chk = false;
	float fRot[3];

	for (int i = 0; i < 3; i++)
	{
		//fRot[i] = lineRotate[i]->value();
		auto text = lineRotate[i]->text();
		fRot[i] = lineRotate[i]->text().toFloat();

		if (!chk)
		{
			chk = fRot[i] != 0.f;
		}
	}

	if ((Trans == Trans.Zero) && (m->scale == Scale) && !chk)
	{
		return;
	}

	ACTION_MANAGER->action_UndoRedo_update(m_pDataContext);

	btnApply->setChecked(true);
	btnApply->setIcon(saveIcons[1]);

	float scal[3];
	if (m->scale != Scale)
	{
		for (int i = 0; i < 3; i++)
		{
			//scal[i] = lineScale[i]->value();
			scal[i] = lineScale[i]->text().toFloat();
		}

		for (int i = 0; i < n_verts; ++i)
		{
			if (tverts[i].isD())
			{
				continue;
			}

			tverts[i].pos.x *= scal[0];
			tverts[i].pos.y *= scal[1];
			tverts[i].pos.z *= scal[2];
		}

		m->updateVertex();
		mip::mesh_control::getMinMax(m->m_verts, m->m_boundingBox.min, m->m_boundingBox.max);

		auto center = m->m_boundingBox.getCenter();

		auto diff_center = m_OrgCenter - center;

		if (diff_center != diff_center.Zero)
		{
			for (int i = 0; i < n_verts; ++i)
			{
				if (tverts[i].isD())
				{
					continue;
				}

				tverts[i].pos += diff_center;
			}

			m->updateVertex();
			mip::mesh_control::getMinMax(m->m_verts, m->m_boundingBox.min, m->m_boundingBox.max);
		}
	}

	if (chk)
	{
		mip::TRANSFORM		transform;
		mip::TRANSFORM		transform2;

		mip::VECTOR3		euler;

		transform.zero();
		transform2.zero();

		euler.x = float((double)fRot[0] / 360. * (2.0 * M_PI));
		euler.y = float((double)fRot[1] / 360. * (2.0 * M_PI));
		euler.z = float((double)fRot[2] / 360. * (2.0 * M_PI));

		transform.addRotateZ(euler.z);
		transform.addRotateY(euler.y);
		transform.addRotateX(euler.x);

		auto mat_rot = mip::math::MatrixAffineTransformation(1.f, m_OrgCenter, transform.getQuaternion(), mip::VECTOR3::Zero);

		mip::MATRIX44 rot;
		mip::MATRIX44 mat_mesh = transform2.getMatrix();
		mip::math::MatrixMultiply(&rot, &mat_mesh, &mat_rot);

		transform2.addRotate(mat_rot.getQuaternion());

		mat_mesh = transform2.getMatrix();

		auto offset = rot.getOrigin();

		for (int i = 0; i < n_verts; ++i)
		{
			if (tverts[i].isD())
			{
				continue;
			}

			tverts[i].pos = mat_mesh * tverts[i].pos;
			tverts[i].pos += offset;
		}

		m->updateVertex();
		mip::mesh_control::getMinMax(m->m_verts, m->m_boundingBox.min, m->m_boundingBox.max);
	}

	Trans /= meshScale;

	if (Trans != Trans.Zero)
	{
		for (int i = 0; i < n_verts; ++i)
		{
			if (tverts[i].isD())
			{
				continue;
			}

			tverts[i].pos += Trans;
		}
	}

	btnApply->setIcon(saveIcons[0]);
	btnApply->setChecked(false);

	m->updateVertex();
	mip::mesh_control::getMinMax(m->m_verts, m->m_boundingBox.min, m->m_boundingBox.max);

	WIN_MANAGER->buildRenderBufferTopology(m);

	Update();
}

void TrWidget::OnDimChanged()
{
	WIN_MANAGER->setMoveFocus(true);

	double tmpVal;

	mip::MeshTopology* m = DATA_CONTEXT->m_MeshData.GetMesh(UID);

	//m->createMinMax();
	mip::AABB box = m->m_boundingBox;

	mip::VECTOR3 dims = box.getSize();
	float di[3];
	di[0] = dims.x;
	di[1] = dims.y;
	di[2] = dims.z;

	for (int i = 0; i < 3; i++)
	{
		//tmpVal = lineDim[i]->value();
		tmpVal = lineDim[i]->text().toDouble();

		if (tmpVal < 0.01f)
			tmpVal = 0.01f;

		if (tmpVal != lineDim[i]->text().toDouble())
		{
			lineDim[i]->blockSignals(true);
			//lineDim[i]->setValue(tmpVal);
			lineDim[i]->setText(QString().sprintf("%.4lf", tmpVal));
			lineDim[i]->blockSignals(false);
		}

		tmpVal /= meshScale;
		tmpVal /= di[i];

		//if (tmpVal != lineScale[i]->value())
		if (tmpVal != lineScale[i]->text().toDouble())
		{
			lineScale[i]->blockSignals(true);
			//lineScale[i]->setValue(tmpVal);
			lineScale[i]->setText(QString().sprintf("%.4lf", tmpVal));
			lineScale[i]->blockSignals(false);
		}
	}
}

void TrWidget::OnTransChanged()
{
	WIN_MANAGER->setMoveFocus(true);
}

void TrWidget::OnRotationChanged()
{
	WIN_MANAGER->setMoveFocus(true);

	double tmpVal;

	for (int i = 0; i < 3; i++)
	{
		//const double curVal = tmpVal = lineRotate[i]->value();
		const double curVal = tmpVal = lineRotate[i]->text().toDouble();

		if (0. > tmpVal)
			tmpVal = 0.;
		else if (360. < tmpVal)
			tmpVal = 360.;

		if (curVal != tmpVal)
		{
			lineRotate[i]->blockSignals(true);
			//lineRotate[i]->setValue(tmpVal);
			lineRotate[i]->setText(QString().sprintf("%.4lf", tmpVal));
			lineRotate[i]->blockSignals(false);
		}
	}
}


//void TrWidget::OnAngleChanged()
//{
//	WIN_MANAGER->setMoveFocus(true);
//}

//void TrWidget::OnRotationChanged()
//{
//	WIN_MANAGER->setMoveFocus(true);
//}

void TrWidget::OnScaleChanged()
{
	WIN_MANAGER->setMoveFocus(true);

	double tmpVal;

	mip::MeshTopology* m = DATA_CONTEXT->m_MeshData.GetMesh(UID);

	//m->createMinMax();
	mip::AABB box = m->m_boundingBox;

	mip::VECTOR3 dims = box.getSize();
	float oldDim[3];
	oldDim[0] = dims.x;
	oldDim[1] = dims.y;
	oldDim[2] = dims.z;

	double newDim;
	for (int i = 0; i < 3; i++)
	{
		//tmpVal = lineScale[i]->value();
		tmpVal = lineScale[i]->text().toDouble();
		oldDim[i] *= meshScale;

		if (tmpVal < 0.01f)
			tmpVal = 0.01f;

		//if (tmpVal != lineScale[i]->value())
		if (tmpVal != lineScale[i]->text().toDouble())
		{
			lineScale[i]->blockSignals(true);
			//lineScale[i]->setValue(tmpVal);
			lineScale[i]->setText(QString().sprintf("%.4lf", tmpVal));
			lineScale[i]->blockSignals(false);
		}
		newDim = tmpVal * oldDim[i];

		if (newDim != oldDim[i])
		{
			lineDim[i]->blockSignals(true);
			//lineDim[i]->setValue(newDim);
			lineDim[i]->setText(QString().sprintf("%.4lf", newDim));
			lineDim[i]->blockSignals(false);
		}
	}
}

