#include "stdafx.h"
#include "SpacingDialog.h"

#include "System/styleManager.h"
#include "System/stringManager.h"


SpacingDialog::SpacingDialog(double m_x, double m_y, double m_z, QWidget *parent)
	:QDialog(parent)
{
	setWindowTitle(STRING_MANAGER->getString(STR_PASSWORD));
	resize(180, 50);
	setMaximumSize(QSize(180, 200));
	setMinimumSize(QSize(180, 200));

	QVBoxLayout *layout = new QVBoxLayout(this);

	QWidget *widgetArea = new QWidget(this);
	widgetArea->setFixedSize(200, 200);
	layout->addWidget(widgetArea);

	QLabel *x_labelIter = new QLabel(widgetArea);
	x_labelIter->setGeometry(QRect(5, 7, 70, 20));
	x_labelIter->setText("x spacing");

	x_edit = new QLineEdit(widgetArea);
	x_edit->setGeometry(QRect(80, 5, 70, 25));
	x_edit->setStyleSheet(STYLE_MANAGER->editBoxNormal);
	x_edit->setValidator(new QDoubleValidator(this));

	  
	QLabel *y_labelIter = new QLabel(widgetArea);
	y_labelIter->setGeometry(QRect(5, 57, 70, 20));
	y_labelIter->setText("y spacing");

	y_edit = new QLineEdit(widgetArea);
	y_edit->setGeometry(QRect(80, 55, 70, 25));
	y_edit->setStyleSheet(STYLE_MANAGER->editBoxNormal);
	y_edit->setValidator(new QDoubleValidator(this));


	QLabel *z_labelIter = new QLabel(widgetArea);
	z_labelIter->setGeometry(QRect(5, 107, 70, 20));
	z_labelIter->setText("z spacing");

	z_edit = new QLineEdit(widgetArea);
	z_edit->setGeometry(QRect(80, 105, 70, 25));
	z_edit->setStyleSheet(STYLE_MANAGER->editBoxNormal);
	z_edit->setValidator(new QDoubleValidator(this));
		
	chkbox = new QCheckBox(this);
	chkbox->setText("Z-Isotropification");
	chkbox->setCheckable(true);
	chkbox->setChecked(false);
	chkbox->setStyleSheet(STYLE_MANAGER->m_Checkbox);
	layout->addWidget(chkbox);
	connect(chkbox, &QCheckBox::clicked, this, &SpacingDialog::OnIsotropicCheck);

	QPushButton *OKbtn = new QPushButton(this);
	OKbtn->setGeometry(QRect(this->width() - 10, this->height() - 10, 30, 30));
	OKbtn->setStyleSheet(STYLE_MANAGER->buttonNormal);
	OKbtn->setText(STRING_MANAGER->getString(STR_OK));
	connect(OKbtn, &QPushButton::clicked, this, &SpacingDialog::OnOK);
	layout->addWidget(OKbtn);

	isOK = false;
	enableIsotro = false;
	m_x_original_spacing = m_x;
	m_y_original_spacing = m_y;
	m_z_original_spacing = m_z;
		
}

void SpacingDialog::OnIsotropicCheck(bool val)
{
	if (chkbox->isChecked())
	{
		x_edit->setDisabled(true);		
		x_edit->setText(QString::number(m_x_original_spacing));
		y_edit->setDisabled(true);
		y_edit->setText(QString::number(m_y_original_spacing));
		z_edit->setDisabled(true);
		z_edit->setText(QString::number(m_z_original_spacing));
		enableIsotro = true;
	}
	else
	{
		x_edit->setDisabled(false);
		y_edit->setDisabled(false);
		z_edit->setDisabled(false);
		enableIsotro = false;
	}

}

void SpacingDialog::getSpacing(double spacing[3])
{
	m_x_spacing = x_edit->text().toDouble();
	m_y_spacing = y_edit->text().toDouble();
	m_z_spacing = z_edit->text().toDouble();

	
	spacing[0] = m_x_spacing;
	spacing[1] = m_y_spacing;
	spacing[2] = m_z_spacing;

}

void SpacingDialog::OnOK()
{
	isOK = true;


	close();
}

SpacingDialog::~SpacingDialog()
{
}

