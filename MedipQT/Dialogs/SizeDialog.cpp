#include "stdafx.h"
#include "SizeDialog.h"

int			SizeDialog::mResol[] = { 25,25,25 };
//x*y*z
float		SizeDialog::mSize[][3] = { {20,20,20 },{ 20,20,20 },{ 20,20,20 } };


SizeDialog::SizeDialog(QString title, int type, QWidget*parent /*= NULL*/)
	:QDialog(parent),
	m_type(type)
{
	setWindowFlags(this->windowFlags() & ~(Qt::WindowContextHelpButtonHint | Qt::WindowMinMaxButtonsHint));
	setWindowTitle(title);

	QVBoxLayout* layout = new QVBoxLayout;
	setLayout(layout);

	QHBoxLayout* wLayout = new QHBoxLayout;
	QHBoxLayout* hLayout = new QHBoxLayout;
	QHBoxLayout* dLayout = new QHBoxLayout;
	QHBoxLayout* rLayout = (m_type != 0) ? new QHBoxLayout : NULL;
	QHBoxLayout* nLayout = new QHBoxLayout;
	QVBoxLayout* bLayout = new QVBoxLayout;

	layout->addLayout(wLayout);
	layout->addLayout(hLayout);
	layout->addLayout(dLayout);
	if (rLayout) layout->addLayout(rLayout);
	layout->addLayout(nLayout);
	layout->addLayout(bLayout);

	QLabel* label = new QLabel(this);
	label->setText("Size X (mm)");

	m_lineWidth = new QLineEdit(this);
	m_lineWidth->setText(QString::number(mSize[m_type][0]));
	m_lineWidth->setValidator(new QDoubleValidator(this));

	connect(m_lineWidth, &QLineEdit::editingFinished, this, &SizeDialog::OnWidthEditFinish);

	wLayout->addWidget(label);
	wLayout->addWidget(m_lineWidth);

	label = new QLabel(this);
	label->setText("Size Y (mm)");

	m_lineHeight = new QLineEdit(this);
	m_lineHeight->setText(QString::number(mSize[m_type][1]));
	m_lineHeight->setValidator(new QDoubleValidator(this));

	connect(m_lineHeight, &QLineEdit::editingFinished, this, &SizeDialog::OnHeightEditFinish);

	hLayout->addWidget(label);
	hLayout->addWidget(m_lineHeight);

	label = new QLabel(this);
	label->setText("Size Z (mm)");

	m_lineDepth = new QLineEdit(this);
	m_lineDepth->setText(QString::number(mSize[m_type][2]));
	m_lineDepth->setValidator(new QDoubleValidator(this));

	connect(m_lineDepth, &QLineEdit::editingFinished, this, &SizeDialog::OnDepthEditFinish);

	dLayout->addWidget(label);
	dLayout->addWidget(m_lineDepth);

	

	for (int i = 0; i < 2; i++)
		m_lineResol[i] = NULL;

	if (m_type == 1)//cylinder
	{
		label = new QLabel(this);
		label->setText("Tri count");

		m_lineResol[0] = new QLineEdit(this);
		m_lineResol[0]->setText(QString::number(mResol[0]));
		m_lineResol[0]->setValidator(new QIntValidator(this));

		connect(m_lineResol[0], &QLineEdit::editingFinished, this, &SizeDialog::OnResolEditFinish);

		rLayout->addWidget(label);
		rLayout->addWidget(m_lineResol[0]);
	}
	else if (m_type == 2)//sphere
	{
		label = new QLabel(this);
		label->setText("Resolution");

		m_lineResol[0] = new QLineEdit(this);
		m_lineResol[0]->setText(QString::number(mResol[1]));
		m_lineResol[0]->setValidator(new QIntValidator(this));

		connect(m_lineResol[0], &QLineEdit::editingFinished, this, &SizeDialog::OnResolEditFinish);

		rLayout->addWidget(label);
		rLayout->addWidget(m_lineResol[0]);

		label = new QLabel(this);
		label->setText("x");

		m_lineResol[1] = new QLineEdit(this);
		m_lineResol[1]->setText(QString::number(mResol[2]));
		m_lineResol[1]->setValidator(new QIntValidator(this));

		connect(m_lineResol[1], &QLineEdit::editingFinished, this, &SizeDialog::OnResol2EditFinish);

		rLayout->addWidget(label);
		rLayout->addWidget(m_lineResol[1]);
	}

	label = new QLabel(this);
	label->setText("Name");

	m_lineName = new QLineEdit(this);

	nLayout->addWidget(label);
	nLayout->addWidget(m_lineName);

	QPushButton* btn = new QPushButton(QString("Create"), this);

	connect(btn, &QPushButton::clicked, this, &SizeDialog::OnCreate);

	bLayout->addWidget(btn);
}

float SizeDialog::getWidth()
{
	mSize[m_type][0] = m_lineWidth->text().toFloat();
	return mSize[m_type][0];
}

float SizeDialog::getHeight()
{
	mSize[m_type][1] = m_lineHeight->text().toFloat();
	return mSize[m_type][1];
}

float SizeDialog::getDepth()
{
	mSize[m_type][2] = m_lineDepth->text().toFloat();
	return mSize[m_type][2];
}

int SizeDialog::getResolution(int _index)
{
	if (_index < 0 ||
		_index >= 2)
		return -1;

	if (NULL == m_lineResol[_index])
		return -1;

	int resol = m_lineResol[_index]->text().toInt();

	if (m_type == 1)
	{
		mResol[0] = resol;
	}
	else
	{
		_index -= m_type;

		mResol[m_type + _index] = resol;
	}

	return resol;
}

QString SizeDialog::getName()
{
	return m_lineName->text();
}

void SizeDialog::OnWidthEditFinish()
{
	m_lineWidth->blockSignals(true);

	bool ok = false;
	float val = m_lineWidth->text().toFloat(&ok);

	if (!ok || (val <= 0))
	{
		val = 1.f;

		m_lineWidth->setText(QString::number(val));
	}
	m_lineWidth->blockSignals(false);
}

void SizeDialog::OnHeightEditFinish()
{
	m_lineHeight->blockSignals(true);

	bool ok = false;
	float val = m_lineHeight->text().toFloat(&ok);

	if (!ok || (val <= 0))
	{
		val = 1.f;

		m_lineHeight->setText(QString::number(val));
	}
	m_lineHeight->blockSignals(false);
}

void SizeDialog::OnDepthEditFinish()
{
	m_lineDepth->blockSignals(true);

	bool ok = false;
	float val = m_lineDepth->text().toFloat(&ok);

	if (!ok || (val <= 0))
	{
		val = 1.f;

		m_lineDepth->setText(QString::number(val));
	}
	m_lineDepth->blockSignals(false);
}

void SizeDialog::OnResolEditFinish()
{
	m_lineResol[0]->blockSignals(true);

	bool ok = false;
	int val = m_lineResol[0]->text().toInt(&ok);

	if (!ok || (val < 6))
	{
		val = 6;

		m_lineDepth->setText(QString::number(val));
	}

	m_lineResol[0]->blockSignals(false);
}

void SizeDialog::OnResol2EditFinish()
{
	m_lineResol[1]->blockSignals(true);

	bool ok = false;
	int val = m_lineResol[1]->text().toInt(&ok);

	if (!ok || (val < 6))
	{
		val = 6;

		m_lineDepth->setText(QString::number(val));
	}

	m_lineResol[1]->blockSignals(false);
}

void SizeDialog::OnCreate()
{
	bool chkCreate = true;

	if ((m_lineDepth->text().length() == 0) ||
		(m_lineHeight->text().length() == 0) ||
		(m_lineWidth->text().length() == 0))
		chkCreate = false;

	if (!chkCreate)
	{
		QMessageBox::warning(this, QString("Size specification"),
			QString("Fill in the information about Width / Height / Depth."));
		m_lineWidth->setFocus();
		return;
	}

	OnWidthEditFinish();
	OnHeightEditFinish();
	OnDepthEditFinish();
	
	QString name = m_lineName->text();

	bool res = name.length() == 0;

	if (res)
	{
		//warning msg
		QMessageBox::warning(this, QString("mesh name rule"),
			QString("The mesh name must be <b>at least one character</b>."));
		return;
	}

	if (name.contains(QRegularExpression(FILE_NAME_RULE)))
	{
		QMessageBox::warning(this, QString("mesh name rule"),
			QString("The file name can not contain any of the following characters:<br><b>\\ \" / : * ? &lt; &gt; | </b> "));
		return;
	}

	QDialog::accept();
}
