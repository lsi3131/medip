#include "stdafx.h"
#include "AlphaWidget.h"
#include "System/styleManager.h"
#include "windowManager.h"

/* AlphaWidget*/
AlphaWidget::AlphaWidget(VOLUME_DATA* pVolumeData, muint32 uid, QWidget* parent /*= NULL*/)
	: QWidget(parent),
	m_pVolumeData(pVolumeData)
{
	setAutoFillBackground(true);
	setStyleSheet(STYLE_MANAGER->sliderBarTab);

	MaskInfo* info = m_pVolumeData->getMaskInfo(uid, true);
	m_alpha = info->layerAlpha;
	m_uid = uid;

	QHBoxLayout* layMain = new QHBoxLayout;
	QVBoxLayout* btnLayout = new QVBoxLayout;
	QVBoxLayout* sliLayout = new QVBoxLayout;

	layMain->addLayout(btnLayout);
	layMain->addLayout(sliLayout);

	QLabel* label = new QLabel(this);
	label->setText("Opacity");

	m_spinbox = new QSpinBox(this);
	m_spinbox->setRange(0, 255);
	m_spinbox->setValue(m_alpha);
	m_spinbox->setStyleSheet(STYLE_MANAGER->spinbox);
	m_spinbox->setObjectName(QString("SpinAlpha"));
	m_spinbox->setContextMenuPolicy(Qt::NoContextMenu);
	m_spinbox->setMouseTracking(true);
	m_spinbox->installEventFilter(this);

	connect(m_spinbox, SIGNAL(valueChanged(int)), this, SLOT(slot_OnSpinChanged(int)));

	btnLayout->addWidget(label);
	btnLayout->addWidget(m_spinbox);

	m_slider = new QSlider(Qt::Horizontal, this);
	m_slider->setObjectName("SliderAlpha");
	m_slider->setRange(0, 255);
	//	m_slider->setPageStep(1);
	//	m_slider->setSingleStep(1);
	m_slider->setValue(m_alpha);
	m_slider->setMouseTracking(true);
	m_slider->installEventFilter(this);
	m_press = false;
	connect(m_slider, &QSlider::sliderPressed, this, &AlphaWidget::slot_OnSliderPress);
	connect(m_slider, &QSlider::valueChanged, this, &AlphaWidget::slot_OnSliderMoved);
	connect(m_slider, &QSlider::sliderReleased, this, &AlphaWidget::slot_OnSliderRelease);

	QWidget* emptyBox0 = new QWidget(this);
	emptyBox0->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	sliLayout->addWidget(m_slider);
	sliLayout->addWidget(emptyBox0);

	setLayout(layMain);
}


bool AlphaWidget::eventFilter(QObject* watched, QEvent* evt)
{
	if (NULL == watched) return false;

	if (watched->objectName().contains("Slider"))
	{
		const QEvent::Type type = evt->type();
		if (type == QEvent::Scroll ||
			type == QEvent::Wheel)
			return true;
	}

	return QWidget::eventFilter(watched, evt);
}

void AlphaWidget::slot_OnSliderPress()
{
	if (!m_press)
	{
		m_press = true;

		MaskInfo* info = m_pVolumeData->getMaskInfo(m_uid, true);
		if (info->show)
			WIN_MANAGER->useSkipRender(3);
	}
}

void AlphaWidget::slot_OnSliderRelease()
{
	MaskInfo* info = m_pVolumeData->getMaskInfo(m_uid, true);
	if (info->show)
	{
		WIN_MANAGER->useSkipRender(3, false);
		WIN_MANAGER->renderLater_3DView();
	}

	if (m_press)
		m_press = false;
}

void AlphaWidget::slot_OnSpinChanged(int val)
{
	if (m_alpha == val)
		return;

	MaskInfo* info = m_pVolumeData->getMaskInfo(m_uid, true);

	info->layerAlpha = m_alpha = val;

	m_slider->blockSignals(true);
	m_slider->setValue(m_alpha);
	m_slider->blockSignals(false);

	if (info->show)
	{
		WIN_MANAGER->forceUpdate2DViewData(false, true);
		WIN_MANAGER->renderLater_GridView(true);
	}
}

void AlphaWidget::slot_OnSliderMoved(int pos)
{
	if (m_alpha == pos)
		return;

	MaskInfo* info = m_pVolumeData->getMaskInfo(m_uid, true);
	info->layerAlpha = m_alpha = pos;

	m_spinbox->blockSignals(true);
	m_spinbox->setValue(m_alpha);
	m_spinbox->blockSignals(false);

	WIN_MANAGER->forceUpdate2DViewData(false, true);
	WIN_MANAGER->renderLater_GridView(true);
}
