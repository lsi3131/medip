#include "stdafx.h"
#include "CroppingViewControlWidget.h"
#include "CroppingView.h"
#include <qlayout>

#define ROTATE_ANGLE_DIAL_MULTIPLIER 100.0

CroppingViewControlWidget::CroppingViewControlWidget(QWidget *parent) :
	QWidget(parent),
	m_winType(WT_AXIAL)
{

}

CroppingViewControlWidget::~CroppingViewControlWidget()
{
}

void CroppingViewControlWidget::InitLayout(WINDOW_TYPE winType)
{
	m_winType = winType;

	QVBoxLayout* mainVBoxLayout = new QVBoxLayout();
	mainVBoxLayout->setMargin(0);
	setLayout(mainVBoxLayout);

	//======== Viewer =========
	QWidget* viewerLayoutWidget = new QWidget();
	QHBoxLayout* viewerHBoxLayout = new QHBoxLayout();
	viewerHBoxLayout->setMargin(0);
	viewerLayoutWidget->setLayout(viewerHBoxLayout);
	viewerLayoutWidget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

	m_view = new CroppingView(this, m_winType);
	m_view->setMinimumSize(QSize(400, 400));

	m_viewScrollBar = new QScrollBar(Qt::Orientation::Vertical);
	connect(m_viewScrollBar, &QScrollBar::valueChanged, this, &CroppingViewControlWidget::slot_OnScrollView);

	viewerHBoxLayout->addWidget(m_view);
	viewerHBoxLayout->addWidget(m_viewScrollBar);

	//======== Rotate Angle =========
	QWidget* rotateAngleLayoutWidget = new QWidget();
	QHBoxLayout* rotateAngleHBoxLayout = new QHBoxLayout();
	rotateAngleHBoxLayout->setMargin(0);
	rotateAngleLayoutWidget->setLayout(rotateAngleHBoxLayout);
	rotateAngleLayoutWidget->setMinimumHeight(100);

	m_dialRotateAngle = new QDial(this);
	m_dialRotateAngle->setRange(-180 * ROTATE_ANGLE_DIAL_MULTIPLIER, 180 * ROTATE_ANGLE_DIAL_MULTIPLIER);
	m_dialRotateAngle->setValue(0);
	m_dialRotateAngle->setSingleStep(1);
	m_dialRotateAngle->setPageStep(5);
	m_dialRotateAngle->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	connect(m_dialRotateAngle, &QDial::valueChanged, this, &CroppingViewControlWidget::slot_OnDialRotateAnglelView);

	m_spinBoxRotateAngle = new QDoubleSpinBox(this);
	m_spinBoxRotateAngle->setRange(-180.0, 180.0);
	m_spinBoxRotateAngle->setValue(0);
	m_spinBoxRotateAngle->setSingleStep(0.1);
	m_spinBoxRotateAngle->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	connect(m_spinBoxRotateAngle, SIGNAL(valueChanged(double)), this, SLOT(slot_OnSpinBoxRotateAnglelView(double)));

	rotateAngleHBoxLayout->addWidget(m_dialRotateAngle);
	rotateAngleHBoxLayout->addWidget(m_spinBoxRotateAngle);

	mainVBoxLayout->addWidget(viewerLayoutWidget);
	mainVBoxLayout->addWidget(rotateAngleLayoutWidget);
}

void CroppingViewControlWidget::Init()
{
	m_view->initTexture();
	m_viewScrollBar->setRange(0, m_view->getMaxDepth());
	m_viewScrollBar->setValue(m_view->getDepth());

	Reset();
}

void CroppingViewControlWidget::Reset()
{
	m_view->reset();
	float angle = m_view->getRotationAngle();
	m_dialRotateAngle->setValue(ToDialWidgetValue(angle));
	m_spinBoxRotateAngle->setValue(angle);
}


CroppingView* CroppingViewControlWidget::View()
{
	return m_view;
}

int CroppingViewControlWidget::ToDialWidgetValue(float angle)
{
	int value = angle * ROTATE_ANGLE_DIAL_MULTIPLIER;
	return value;
}

float CroppingViewControlWidget::ToAngleFromDialWidget(int value)
{
	float fValue = value / ROTATE_ANGLE_DIAL_MULTIPLIER;
	return fValue;
}

void CroppingViewControlWidget::slot_OnDialRotateAnglelView(int value)
{
	float angle = ToAngleFromDialWidget(value);

	m_spinBoxRotateAngle->blockSignals(true);
	m_spinBoxRotateAngle->setValue(angle);
	m_spinBoxRotateAngle->blockSignals(false);

	m_view->setRotationAngle(angle);
	m_view->update();
}

void CroppingViewControlWidget::slot_OnSpinBoxRotateAnglelView(double value)
{
	double angle = value;
	int dialgValue = ToDialWidgetValue(angle);

	m_dialRotateAngle->blockSignals(true);
	m_dialRotateAngle->setValue(dialgValue);
	m_dialRotateAngle->blockSignals(false);

	m_view->setRotationAngle(value);
	m_view->update();
}

void CroppingViewControlWidget::slot_OnScrollView(int value)
{
	m_view->setDepth(value);
}

