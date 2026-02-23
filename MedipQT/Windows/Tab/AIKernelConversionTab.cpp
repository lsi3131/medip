#include "stdafx.h"
#include "AIKernelConversionTab.h"
#include "System/styleManager.h"
#include "Windows\windowManager.h"

#include "ActionManager.h"
#include "DataContext.h"


AIKernelConversionTab::AIKernelConversionTab(QWidget* parent) : CollapseWidget(QString(), parent)
{
	int row = 0;
	QGroupBox* groupMode = new QGroupBox("Mode Selection", this);
	QLayout* layoutH = new QHBoxLayout(groupMode);
	groupMode->setLayout(layoutH);

	m_rdoContinousConversion = new QRadioButton("Continous Conversion");
	m_rdoKernelNeutralization = new QRadioButton("Kernel Neutralization");
	
	layoutH->addWidget(m_rdoContinousConversion);
	layoutH->addWidget(m_rdoKernelNeutralization);

	m_rdoContinousConversion->setChecked(true);

	m_rdoCTQuantization = new QRadioButton("CT quantization");
	layoutH->addWidget(m_rdoCTQuantization);

	m_btnPredict = new QPushButton("Predict", this);
	m_btnPredict->setStyleSheet(STYLE_MANAGER->buttonBehind);
	connect(m_btnPredict, &QPushButton::clicked, this, &AIKernelConversionTab::slot_Predict);

	addWidget(groupMode, row++, 1);
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

	m_groupInterpolation = new QGroupBox("Kernel Interpolation", this);
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
		connect(m_sliderInterpolation, &QSlider::sliderReleased, this, &AIKernelConversionTab::slot_SliderRelease);
		connect(m_sliderInterpolation, &QSlider::valueChanged, this, &AIKernelConversionTab::slot_SliderValueChanged);
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
		connect(m_textInterpolation, &QLineEdit::returnPressed, this, &AIKernelConversionTab::slot_textChanged);
		connect(m_textInterpolation, &QLineEdit::editingFinished, this, &AIKernelConversionTab::slot_textChanged);
	}

	layout2->addWidget(m_textInterpolation);
	layout2->addWidget(m_sliderInterpolation);
	
	addWidget(m_groupInterpolation, row++, 1);

	SetVisibleInterpolation(false);

	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

	QString title = QString("AI Kernel Conversion");
	setTitle(title);

	setOpenWidget(false);
}

std::vector<mint16> AIKernelConversionTab::GetImageData_Origin() const
{
	return m_image_Origin;
}

void AIKernelConversionTab::SetImageData_Origin(const std::vector<mint16>& data)
{
	m_image_Origin = data;
}

std::vector<mint16> AIKernelConversionTab::GetImageData_Output() const
{
	return m_image_Output;
}

void AIKernelConversionTab::SetImageData_Output(const std::vector<mint16>& data)
{
	m_image_Output = data;
}

void AIKernelConversionTab::SetVisibleInterpolation(bool bVisible)
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

int AIKernelConversionTab::GetSliderValue()
{
	return m_sliderInterpolation->value();
}

void AIKernelConversionTab::slot_Predict()
{
	if (m_rdoContinousConversion->isChecked())
	{
		ACTION_MANAGER->action_AI_Kernel_Continous_Conversion();
	}
	else if (m_rdoKernelNeutralization->isChecked())
	{
		ACTION_MANAGER->action_AI_Kernel_Neutralization();
	}	
	else if (m_rdoCTQuantization->isChecked())
	{
		ACTION_MANAGER->action_AI_CT_Quantization();
	}


	// target data save
	//mint16* pHUdata = DATA_CONTEXT->volume_data.getHUDataPoint();

	//int width = DATA_CONTEXT->volume_data.getCX();
	//int height = DATA_CONTEXT->volume_data.getCY();		
	//int depth = DATA_CONTEXT->volume_data.getCZ();

	//mint16* pRaw = new mint16[width*height];

	//int d = depth / 2;
	//for (int h = 0; h < height; ++h)
	//{
	//	for (int w = 0; w < width; ++w)
	//	{
	//		int index3D = width * height * d + height * h + w;
	//		int index2D = height * h + w;
	//		pRaw[index2D] = pHUdata[index3D];
	//	}
	//}
	//
	//QFile file("D:\\target.dat");
	//		

	//if (!file.open(QIODevice::WriteOnly))
	//{
	//	file.remove();		
	//	return;
	//}

	//file.write((const char*)pRaw, (width * height) * sizeof(short));
	//file.close();
	//

	//delete[] pRaw;

}

void AIKernelConversionTab::slot_SliderRelease()
{
	if (!m_groupInterpolation->isVisible())
		return;

	applyInterpolationImage();

}

void AIKernelConversionTab::slot_SliderValueChanged()
{
	QString strValue = QString::number((float)m_sliderInterpolation->value());
	m_textInterpolation->setText(strValue);
}

void AIKernelConversionTab::slot_textChanged()
{

}

void AIKernelConversionTab::applyInterpolationImage(void)
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

	//const auto num_core = std::thread::hardware_concurrency();
	//int n_core = 1;//
	//if (num_core >= 3)
	//	n_core = 2;

	//printf_s("\n USE PPL Core - %d", n_core);

	//int size = allszie;
	//int interval = size / n_core;
	//int remain = size % n_core;

	//concurrency::parallel_for(0, n_core, [&](int i)
	//{
	//	int start = i * interval;
	//	int finish = (i + 1) * interval;

	//	if (i == n_core - 1)
	//		finish += remain;

	//	for (int ii = start; ii < finish; ii++)
	//	{
	//	}
	//	printf_s("\n [%d] Core finished", i);
	//});
}

