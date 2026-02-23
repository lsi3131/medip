#include "stdafx.h"
#include "ImageDialog.h"

#include "System\styleManager.h"

ImageDialog::ImageDialog(int w, int h, QWidget* parent, Qt::WindowFlags f)
	: QDialog(parent, f), m_imgSize(w, h), m_origin(w, h)
{
	w = (w <= 0) ? 1 : w;
	h = (h <= 0) ? 1 : h;

	setFixedSize(300, 250);
	setStyleSheet(STYLE_MANAGER->mainFrame);
	setWindowTitle("Image Dialog");

	QWidget* widget = new QWidget(this);
	widget->setFixedSize(this->size());

	QVBoxLayout* vMainLayout = new QVBoxLayout;
	widget->setLayout(vMainLayout);
	
	vMainLayout->addWidget(createPixelSizeGroup(parent), 1);
	
	vMainLayout->addWidget(createPercentageSizeGroup(), 1);
	
	vMainLayout->addWidget(createCheckBoxGroup(), 1);

	vMainLayout->addWidget(createCmLabelGroup(), 1);

	vMainLayout->addWidget(createBtnGroup());
}


ImageDialog::~ImageDialog()
{

}

QWidget* ImageDialog::createPixelSizeGroup(QWidget* parent)
{
	QGroupBox* group = new QGroupBox("Pixel", this);
	group->setStyleSheet(STYLE_MANAGER->imgDialogGroupBox);

	QHBoxLayout* hLayout = new QHBoxLayout;
	hLayout->setSpacing(5);
	group->setLayout(hLayout);

	int margin = 0;
	QSize imgMax(500, 500);
	QTextEdit* p = dynamic_cast<QTextEdit*>(parent);
	if (p)
	{
		margin = static_cast<int>(p->document()->documentMargin());
		imgMax = p->document()->pageSize().toSize();
	}

	QLabel* label = new QLabel("Width", group);
	hLayout->addWidget(label);

	QSpinBox* wValBox = new QSpinBox(group);
	wValBox->setFixedWidth(this->size().width() / 4);
	wValBox->setStyleSheet(STYLE_MANAGER->spinbox);
	wValBox->setMaximum(imgMax.width());
	wValBox->setValue(m_imgSize.width());
	wValBox->setSuffix(" px");//1px == 0.026458333cm
	hLayout->addWidget(wValBox, 1, Qt::AlignLeft);

	connect(wValBox, SIGNAL(valueChanged(int)), this, SLOT(OnWValueChange(int)));

	label = new QLabel("Height", group);
	hLayout->addWidget(label);

	QSpinBox* hValBox = new QSpinBox(group);
	hValBox->setFixedWidth(this->size().width() / 4);
	hValBox->setStyleSheet(STYLE_MANAGER->spinbox);
	hValBox->setMaximum(imgMax.height());
	hValBox->setValue(m_imgSize.height());
	hValBox->setSuffix(" px");
	hLayout->addWidget(hValBox, 1, Qt::AlignLeft);

	connect(hValBox, SIGNAL(valueChanged(int)), this, SLOT(OnHValueChange(int)));
	
	m_widgetList["wValBox"] = wValBox;
	m_widgetList["hValBox"] = hValBox;

	return group;
}

QWidget* ImageDialog::createPercentageSizeGroup()
{
	QGroupBox* group = new QGroupBox("Percentage", this);
	group->setStyleSheet(STYLE_MANAGER->imgDialogGroupBox);

	QHBoxLayout* hLayout = new QHBoxLayout;
	hLayout->setSpacing(5);
	group->setLayout(hLayout);

	QLabel* label = new QLabel("Width", group);
	hLayout->addWidget(label);

	QSpinBox* wPerBox = new QSpinBox(group);
	wPerBox->setFixedWidth(this->size().width() / 4);
	wPerBox->setStyleSheet(STYLE_MANAGER->spinbox);
	wPerBox->setMaximum(900);
	wPerBox->setValue(100);
	wPerBox->setSuffix(" %");
	hLayout->addWidget(wPerBox, 1, Qt::AlignLeft);
	
	connect(wPerBox, SIGNAL(valueChanged(int)), this, SLOT(OnWPercentageChange(int)));

	label = new QLabel("Height", group);
	hLayout->addWidget(label);

	QSpinBox* hPerBox = new QSpinBox(group);
	hPerBox->setFixedWidth(this->size().width() / 4);
	hPerBox->setStyleSheet(STYLE_MANAGER->spinbox);
	hPerBox->setMaximum(900);
	hPerBox->setValue(100);
	hPerBox->setSuffix(" %");
	hLayout->addWidget(hPerBox, 1, Qt::AlignLeft);
	
	connect(hPerBox, SIGNAL(valueChanged(int)), this, SLOT(OnHPercentageChange(int)));

	m_widgetList["wPerBox"] = wPerBox;
	m_widgetList["hPerBox"] = hPerBox;

	return group;
}

QWidget*ImageDialog::createCheckBoxGroup()
{
	QCheckBox* checkFixedRate = new QCheckBox(this);
	checkFixedRate->setText("Fixed Rate");
	checkFixedRate->setCheckable(true);
	checkFixedRate->setChecked(true);
	checkFixedRate->setStyleSheet(STYLE_MANAGER->m_Checkbox);
	m_widgetList["checkFixedRate"] = checkFixedRate;

	return checkFixedRate;
}

QWidget* ImageDialog::createCmLabelGroup()
{
	QGroupBox* group = new QGroupBox("cm", this);
	group->setStyleSheet(STYLE_MANAGER->imgDialogGroupBox);

	QHBoxLayout* hLayout = new QHBoxLayout;
	group->setLayout(hLayout);

	qreal qW = m_imgSize.width() * 0.02645833; //1px == 0.026458333cm
	qreal qH = m_imgSize.height() * 0.02645833;

	QLabel* wCmLabel = new QLabel("Width : " + QString::number(qW, 'g', 3) + "cm", group);
	hLayout->addWidget(wCmLabel);

	QLabel* hCmLabel = new QLabel("Height : " + QString::number(qH, 'g', 3) + "cm", group);
	hLayout->addWidget(hCmLabel);
	
	m_widgetList["wCmLabel"] = wCmLabel;
	m_widgetList["hCmLabel"] = hCmLabel;

	return group;
}

QWidget* ImageDialog::createBtnGroup()
{
	QWidget* btnArea = new QWidget(this);

	QHBoxLayout* hLayout = new QHBoxLayout;
	hLayout->setMargin(0);
	btnArea->setLayout(hLayout);

	QPushButton* btn = new QPushButton("OK", btnArea);
	btn->setStyleSheet(STYLE_MANAGER->buttonBehind);
	hLayout->addWidget(btn);
	connect(btn, &QPushButton::clicked, this, &ImageDialog::OnFinished);

	btn = new QPushButton("Cancel", btnArea);
	btn->setStyleSheet(STYLE_MANAGER->buttonBehind);
	connect(btn, &QPushButton::clicked, this, &ImageDialog::OnCancel);
	hLayout->addWidget(btn);

	return btnArea;
}

void ImageDialog::updateSize(bool isWidth)
{
	QCheckBox* checkBox = dynamic_cast<QCheckBox*>(m_widgetList["checkFixedRate"]);
	QSpinBox* wValBox = dynamic_cast<QSpinBox*>(m_widgetList["wValBox"]);
	QSpinBox* hValBox = dynamic_cast<QSpinBox*>(m_widgetList["hValBox"]);
	QSpinBox* wPerBox = dynamic_cast<QSpinBox*>(m_widgetList["wPerBox"]);
	QSpinBox* hPerBox = dynamic_cast<QSpinBox*>(m_widgetList["hPerBox"]);
	QLabel* wCmLabel = dynamic_cast<QLabel*>(m_widgetList["wCmLabel"]);
	QLabel* hCmLabel = dynamic_cast<QLabel*>(m_widgetList["hCmLabel"]);

	bool nullCheck = checkBox && wValBox && hValBox && wPerBox && hPerBox;
	if (nullCheck == false) return;

	if (checkBox->isChecked())
	{
		int tempW = m_imgSize.width();
		int tempH = m_imgSize.height();

		if (isWidth)
		{
			m_imgSize.rheight() = tempW * m_origin.height() / m_origin.width();
		}
		else
		{
			m_imgSize.rwidth() = tempH * m_origin.width() / m_origin.height();

		}
	}

	int imgWidth = m_imgSize.rwidth() = std::max(1, m_imgSize.width());
	int imgHeight = m_imgSize.rheight() = std::max(1, m_imgSize.height());

	wValBox->blockSignals(true);
	wValBox->setValue(imgWidth);
	wValBox->blockSignals(false);

	hValBox->blockSignals(true);
	hValBox->setValue(imgHeight);
	hValBox->blockSignals(false);
	
	int wPercentage = floorf((static_cast<float>(imgWidth) / static_cast<float>(m_origin.width()) + 0.005f) * 100);
	wPerBox->blockSignals(true);
	wPerBox->setValue(wPercentage);
	wPerBox->blockSignals(false);
	
	int hPercentage = floorf((static_cast<float>(imgHeight) / static_cast<float>(m_origin.height()) + 0.005f) * 100);
	hPerBox->blockSignals(true);
	hPerBox->setValue(hPercentage);
	hPerBox->blockSignals(false);

	qreal qW = imgWidth * 0.02645833; //1px == 0.026458333cm
	qreal qH = imgHeight * 0.02645833;
	wCmLabel->setText("Width : " + QString::number(qW, 'g', 3) + "cm");
	hCmLabel->setText("Height : " + QString::number(qH, 'g', 3) + "cm");
}

QSize ImageDialog::getImageSize()
{
	QCheckBox* checkBox = dynamic_cast<QCheckBox*>(m_widgetList["checkFixedRate"]);
	if (nullptr == checkBox) return QSize(1, 1);

	const int MIN_IMAGE_SIZE = 30;
	if (checkBox->isChecked())
	{
		if (m_imgSize.height() < MIN_IMAGE_SIZE)
		{
			m_imgSize.rheight() = MIN_IMAGE_SIZE;
			m_imgSize.rwidth() = m_imgSize.height() * m_origin.width() / m_origin.height();
		}
		else if (m_imgSize.width() < MIN_IMAGE_SIZE)
		{
			m_imgSize.rwidth() = MIN_IMAGE_SIZE;
			m_imgSize.rheight() = m_imgSize.width() * m_origin.height() / m_origin.width();
		}
	}
	m_imgSize.rwidth() = std::max(MIN_IMAGE_SIZE, m_imgSize.width());
	m_imgSize.rheight() = std::max(MIN_IMAGE_SIZE, m_imgSize.height());
	return m_imgSize;
}

void ImageDialog::OnFinished()
{
	emit accept();
}

void ImageDialog::OnCancel()
{
	this->close();
}

void ImageDialog::OnWValueChange(int v)
{
	m_imgSize.rwidth() = v;
	updateSize(true);
}

void ImageDialog::OnHValueChange(int v)
{
	m_imgSize.rheight() = v;
	updateSize(false);
}

void ImageDialog::OnWPercentageChange(int v)
{
	if (m_imgSize.width() < (float)m_origin.width() * (float)v / 100.0f)
		m_imgSize.rwidth() = std::ceil((float)m_origin.width() * (float)v / 100.0f);
	else
		m_imgSize.rwidth() = std::floor((float)m_origin.width() * (float)v / 100.0f);
	updateSize(true);
}

void ImageDialog::OnHPercentageChange(int v)
{
	if(m_imgSize.height() < (float)m_origin.height() * (float)v / 100.0f)
		m_imgSize.rheight() = std::ceil((float)m_origin.height() * (float)v / 100.0f);
	else
		m_imgSize.rheight() = std::floor((float)m_origin.height() * (float)v / 100.0f);
	updateSize(false);
}