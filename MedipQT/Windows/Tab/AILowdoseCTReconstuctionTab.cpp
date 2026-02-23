#include "stdafx.h"
#include "AILowdoseCTReconstuctionTab.h"
#include "ActionManager.h"
#include "windowManager.h"
#include "styleManager.h"


AILowdoseCTReconstuctionTab::AILowdoseCTReconstuctionTab(QWidget* parent) : CollapseWidget(QString(), parent)
{
	int row = 0;
	
	m_btnPredict = new QPushButton("Predict", this);
	m_btnPredict->setStyleSheet(STYLE_MANAGER->buttonBehind);
	connect(m_btnPredict, &QPushButton::clicked, this, &AILowdoseCTReconstuctionTab::slot_Predict);

	addWidget(m_btnPredict, row++, 1);

	QWidget* emptyBox0 = new QWidget(this);
	emptyBox0->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
	addWidget(emptyBox0, row++, 1);

	QFrame* frLine = new QFrame(this);
	frLine->setStyleSheet("background-color:#414141;");
	frLine->setFrameShape(QFrame::HLine);
	frLine->setFrameShadow(QFrame::Sunken);
	frLine->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
	addWidget(frLine, row++, 0, QMargins(0, 5, 0, 5));

	m_groupInterpolation = new QGroupBox("Dose Interpolation", this);
	QLayout* layout2 = new QHBoxLayout();
	m_groupInterpolation->setLayout(layout2);

	m_sliderInterpolation = new QSlider(this);

	if (m_sliderInterpolation)
	{
		m_sliderInterpolation->setObjectName("SliderInterpolation");
		m_sliderInterpolation->setOrientation(Qt::Horizontal);
		m_sliderInterpolation->setStyleSheet(STYLE_MANAGER->sliderBarTab);
		m_sliderInterpolation->installEventFilter(this);
		m_sliderInterpolation->setMinimum(0);
		m_sliderInterpolation->setMaximum(100);
		m_sliderInterpolation->setValue(50);
		connect(m_sliderInterpolation, &QSlider::sliderReleased, this, &AILowdoseCTReconstuctionTab::slot_SliderRelease);
		connect(m_sliderInterpolation, &QSlider::valueChanged, this, &AILowdoseCTReconstuctionTab::slot_SliderValueChanged);
	}

	m_textInterpolation = new QLineEdit(this);
	if (m_textInterpolation)
	{
		m_textInterpolation->setStyleSheet(STYLE_MANAGER->editBoxTab);
		m_textInterpolation->setText(tr("0"));
		m_textInterpolation->setAlignment(Qt::AlignCenter);
		m_textInterpolation->setFixedWidth(60);
		m_textInterpolation->setObjectName("EditInterpolation");
		m_textInterpolation->setMouseTracking(true);
		m_textInterpolation->installEventFilter(this);
		m_textInterpolation->setText("50");
		connect(m_textInterpolation, &QLineEdit::returnPressed, this, &AILowdoseCTReconstuctionTab::slot_textChanged);
		connect(m_textInterpolation, &QLineEdit::editingFinished, this, &AILowdoseCTReconstuctionTab::slot_textChanged);
	}

	layout2->addWidget(m_textInterpolation);
	layout2->addWidget(m_sliderInterpolation);

	addWidget(m_groupInterpolation, row++, 1);

	SetVisibleInterpolation(false);

	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

	QString title = QString("AI Low-dose CT Reconstruction");
	setTitle(title);

	setOpenWidget(false);
}


AILowdoseCTReconstuctionTab::~AILowdoseCTReconstuctionTab()
{
}

void AILowdoseCTReconstuctionTab::SetVisibleInterpolation(bool bVisible)
{
	if (bVisible)
	{
		m_groupInterpolation->setEnabled(true);
	}
	else
	{
		m_sliderInterpolation->setValue(50);
		m_textInterpolation->setText("50");
		m_groupInterpolation->setEnabled(false);
	}
}

int AILowdoseCTReconstuctionTab::GetSliderValue()
{
	return m_sliderInterpolation->value();
}

std::vector<mint16> AILowdoseCTReconstuctionTab::GetImageData_Origin() const
{
	return m_image_Origin;
}

void AILowdoseCTReconstuctionTab::SetImageData_Origin(const std::vector<mint16>& data)
{
	m_image_Origin = data;
}

std::vector<mint16> AILowdoseCTReconstuctionTab::GetImageData_Output() const
{
	return m_image_Output;
}

void AILowdoseCTReconstuctionTab::SetImageData_Output(const std::vector<mint16>& data)
{
	m_image_Output = data;
}

void AILowdoseCTReconstuctionTab::slot_Predict()
{
	ACTION_MANAGER->action_AI_Lowdose_Reconstruction();
}

void AILowdoseCTReconstuctionTab::slot_SliderRelease()
{
	if (!m_groupInterpolation->isVisible())
		return;

	applyInterpolationImage();
}

void AILowdoseCTReconstuctionTab::slot_SliderValueChanged()
{
	QString strValue = QString::number((float)m_sliderInterpolation->value());
	m_textInterpolation->setText(strValue);
}

void AILowdoseCTReconstuctionTab::slot_textChanged()
{

}

void AILowdoseCTReconstuctionTab::applyInterpolationImage(void)
{
	VOLUME_DATA* pVolumeData = &DATA_CONTEXT->volume_data;
	int dataSize = pVolumeData->getVolumeDataLength();
	std::vector<mint16> result(dataSize, 0);

	float alpha = (float)m_sliderInterpolation->value() / 100.f;

	float fMinusAlpha = (1.f - alpha);
	for (unsigned int i = 0; i < dataSize; ++i)
	{
		result[i] = fMinusAlpha * m_image_Origin[i] + alpha * m_image_Output[i];
	}

	memcpy_s(pVolumeData->getHUDataPoint(), dataSize * sizeof(mint16), result.data(), dataSize * sizeof(mint16));

	WIN_MANAGER->forceUpdate2DViewData(true, false);
	WIN_MANAGER->renderLater_GridView(false);

	std::cout << "size" << dataSize << std::endl << "kernel continous conversion!!" << std::endl;
}
