#include "stdafx.h"
#include "Dialogs/ThickDialog.h"
#include "Windows/AnalMPRPlaneView.h"

#include "System/styleManager.h"

ThickDialog::ThickDialog(float _thick, AnalMPRPlaneView* view)
	:QDialog(view)
{
	setWindowFlags(this->windowFlags() & ~(Qt::WindowContextHelpButtonHint| Qt::WindowMinMaxButtonsHint));

	QString title;

	switch (view->getType())
	{
	case WT_AXIAL:
		title = "Axial";
		break;
	case WT_CORONAL:
		title = "Coronal";
		break;
	case WT_SAGITTAL:
		title = "Sagittal";
		break;
	default:
		break;
	}

	title.append(" view thickness");

	setWindowTitle(title);

	QVBoxLayout* main = new QVBoxLayout(this);
	QHBoxLayout* editLayout = new QHBoxLayout;
	QHBoxLayout* applyLayout = new QHBoxLayout;
	
	editLayout->setContentsMargins(0, 5, 5, 5);

	QWidget *emptyBox = new QWidget(this);
	emptyBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);


	main->addLayout(editLayout);
	main->addLayout(applyLayout);

	m_edit = new QLineEdit(this);
	if (_thick != 0)
	{
		m_edit->setText(QString::number(_thick*10));
	}
	m_edit->setValidator(new QDoubleValidator(this));

	connect(m_edit, &QLineEdit::editingFinished, this, &ThickDialog::OnEditFinished);
	editLayout->addWidget(emptyBox,1);
	editLayout->addWidget(m_edit, 1,Qt::AlignCenter);

	m_comboUnit = new QComboBox(this);
	m_comboUnit->addItem("mm");
	m_comboUnit->addItem("cm");
	m_comboUnit->setMinimumContentsLength(5);
	m_comboUnit->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);
	m_comboUnit->setStyleSheet(STYLE_MANAGER->comboBoxNormal);
	m_comboUnit->setCurrentIndex(0);
	
	connect(m_comboUnit, SIGNAL(currentIndexChanged(int)), this, SLOT(OnComboChanged(int)));

	emptyBox = new QWidget(this);
	emptyBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);


	editLayout->addWidget(m_comboUnit, 1);
	editLayout->addWidget(emptyBox, 1);

	QPushButton* btn = new QPushButton("Apply", this);

	connect(btn, &QPushButton::clicked, this, &ThickDialog::OnApply);
	emptyBox = new QWidget(this);
	emptyBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	applyLayout->addWidget(emptyBox, 1);
	applyLayout->addWidget(btn,1);
	emptyBox = new QWidget(this);
	emptyBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	applyLayout->addWidget(emptyBox, 1);

}

void ThickDialog::OnEditFinished()
{
	float textVal = m_edit->text().toFloat();

	if (textVal < 0)
		textVal = 0;

	if (m_comboUnit->currentIndex() != 0)
		textVal *= 10;

	m_thick = textVal;

}

void ThickDialog::OnComboChanged(int index)
{
	float textVal = m_edit->text().toFloat();

	if (0 == index)
		textVal *= 10;
	else
		textVal /= 10;

	m_edit->setText(QString::number(textVal));
	
	OnEditFinished();
}

void ThickDialog::OnApply()
{
	accept();
}
