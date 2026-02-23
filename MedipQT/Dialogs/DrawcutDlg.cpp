#include "stdafx.h"
#include "DrawcutDlg.h"

#include "System/styleManager.h"
#include "System/stringManager.h"

OffsetDlg::OffsetDlg(QWidget *parent /*= NULL*/)
{
	setWindowTitle(STRING_MANAGER->getString(STR_DRAW_CUT));
	//	resize(180, 150);
	//	setMaximumSize(QSize(180, 150));
	//	setMinimumSize(QSize(180, 150));
	m_default = 50.0;
	m_minVal = 10.0;
	m_maxVal = 1000.0;
	m_step = 1.0;

	m_Accept = false;
	m_res = m_default;
	QVBoxLayout *layout = new QVBoxLayout;
	QHBoxLayout *layout2 = new QHBoxLayout;

	layout->addLayout(layout2);

	QLabel *label1 = new QLabel(QString("Lambda : "), this);
	layout2->addWidget(label1);

	m_lineEdit = new QLineEdit(this);
	//	m_lineEdit->setStyleSheet(STYLE_MANAGER->editBoxNormal);
	m_lineEdit->setValidator(new QDoubleValidator(this));
	m_lineEdit->setText(QString::number(m_res));

	connect(m_lineEdit, &QLineEdit::textChanged, this, &OffsetDlg::OnTextChanged);
	layout2->addWidget(m_lineEdit);

	QHBoxLayout *layout3 = new QHBoxLayout;

	layout->addLayout(layout3);

	m_slider = new QSlider(Qt::Horizontal, this);
	if (m_step == 1.0)
	{
		m_slider->setMinimum(m_minVal);
		m_slider->setMaximum(m_maxVal);
		m_slider->setValue(m_res);
	}
	else
	{
		m_slider->setMinimum(0);
		m_slider->setMaximum(100);
		m_slider->setValue((float(m_res - m_minVal) / (m_maxVal - m_minVal)) * 100);
	}

	connect(m_slider, &QSlider::valueChanged, this, &OffsetDlg::OnDepthSlideReleased);

	layout3->addWidget(m_slider);

	QHBoxLayout *layout4 = new QHBoxLayout;

	layout->addLayout(layout4);

	QPushButton *btnOK = new QPushButton(this);
	//	btnOK->setStyleSheet(STYLE_MANAGER->buttonNormal);
	btnOK->setText(STRING_MANAGER->getString(STR_OK));
	btnOK->setFixedSize(30, 30);
	connect(btnOK, &QPushButton::clicked, this, &OffsetDlg::OnOK);
	layout4->addWidget(btnOK);

	QPushButton *btnCancel = new QPushButton(this);
	//	btnCancel->setStyleSheet(STYLE_MANAGER->buttonNormal);
	btnCancel->setText(STRING_MANAGER->getString(STR_CANCEL));
	btnCancel->setFixedSize(60, 30);
	connect(btnCancel, &QPushButton::clicked, this, &OffsetDlg::OnCancel);
	layout4->addWidget(btnCancel);


	setLayout(layout);
}

OffsetDlg::OffsetDlg(QString _title, QString _label, double _base, double _step, double _min, double _max, QWidget *parent /*= NULL*/)
	:QDialog(parent),
	m_res(_base),
	m_minVal(_min),
	m_maxVal(_max),
	m_step(_step),
	m_Accept(false)
{
	setWindowTitle(_title);

	QVBoxLayout *layout = new QVBoxLayout;
	QHBoxLayout *layout2 = new QHBoxLayout;

	layout->addLayout(layout2);

	QLabel *label1 = new QLabel(_label, this);
	layout2->addWidget(label1);

	m_lineEdit = new QLineEdit(this);
	if (_step == 1.0)
		m_lineEdit->setValidator(new QIntValidator(m_minVal, m_maxVal, this));
	else
		m_lineEdit->setValidator(new QDoubleValidator(m_minVal, m_maxVal, 2, this));

	m_lineEdit->setText(QString::number(m_res));

	connect(m_lineEdit, &QLineEdit::editingFinished, this, &OffsetDlg::OnTextChanged);
	connect(m_lineEdit, &QLineEdit::returnPressed, this, &OffsetDlg::OnTextChanged);
	layout2->addWidget(m_lineEdit);

	QHBoxLayout *layout3 = new QHBoxLayout;

	layout->addLayout(layout3);

	m_slider = new QSlider(Qt::Horizontal, this);
	if (m_step == 1.0)
	{
		m_slider->setMinimum(m_minVal);
		m_slider->setMaximum(m_maxVal);
		m_slider->setValue(m_res);
	}
	else
	{
		m_slider->setMinimum(0);
		m_slider->setMaximum(100);
		m_slider->setValue((float(m_res - m_minVal) / (m_maxVal - m_minVal)) * 100);
	}

	connect(m_slider, &QSlider::valueChanged, this, &OffsetDlg::OnDepthSlideReleased);

	layout3->addWidget(m_slider);

	QHBoxLayout *layout4 = new QHBoxLayout;

	layout->addLayout(layout4);

	QPushButton *btnOK = new QPushButton(this);
	//	btnOK->setStyleSheet(STYLE_MANAGER->buttonNormal);
	btnOK->setText(STRING_MANAGER->getString(STR_OK));
	btnOK->setFixedSize(30, 30);
	connect(btnOK, &QPushButton::clicked, this, &OffsetDlg::OnOK);
	layout4->addWidget(btnOK);

	QPushButton *btnCancel = new QPushButton(this);
	//	btnCancel->setStyleSheet(STYLE_MANAGER->buttonNormal);
	btnCancel->setText(STRING_MANAGER->getString(STR_CANCEL));
	btnCancel->setFixedSize(60, 30);
	connect(btnCancel, &QPushButton::clicked, this, &OffsetDlg::OnCancel);
	layout4->addWidget(btnCancel);


	setLayout(layout);
}

OffsetDlg::~OffsetDlg()
{
}

void OffsetDlg::OnDepthSlideReleased(int val)
{
	double tmpVal = val;

	if (m_step != 1.0)
	{
		tmpVal /= 100;
		tmpVal *= (m_maxVal - m_minVal);
		tmpVal += m_minVal;
	}

	if (m_lineEdit->text() != QString::number(tmpVal))
	{
		m_lineEdit->blockSignals(true);
		m_lineEdit->setText(QString::number(tmpVal));
		m_lineEdit->blockSignals(false);
	}

}

void OffsetDlg::OnTextChanged()
{
	QString txt = m_lineEdit->text();
	double tmpVal = (m_step == 1.0) ? txt.toInt() : txt.toDouble();

	m_lineEdit->blockSignals(true);

	if (tmpVal < m_minVal)
		tmpVal = m_minVal;
	else if (tmpVal > m_maxVal)
		tmpVal = m_maxVal;

	if (QString::number(tmpVal) != txt)
		m_lineEdit->setText(QString::number(tmpVal));

	m_lineEdit->blockSignals(false);

	if(m_step != 1.0)
	{
		tmpVal -= m_minVal;
		tmpVal /= (m_maxVal - m_minVal);
		tmpVal *= 100;
	}

	if (tmpVal != m_slider->value())
	{
		m_slider->blockSignals(true);
		m_slider->setValue(tmpVal);
		m_slider->blockSignals(false);
	}

}

void OffsetDlg::OnOK()
{
	m_Accept = true;
	m_res = (m_step == 1.0) ? m_lineEdit->text().toInt() : m_lineEdit->text().toDouble();
	close();
}

void OffsetDlg::OnCancel()
{
	close();
}
