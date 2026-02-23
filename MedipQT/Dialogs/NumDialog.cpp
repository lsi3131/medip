#include "stdafx.h"
#include "NumDialog.h"
#include "styleManager.h"


NumDialog::NumDialog(QWidget* parent /*= NULL*/, int based /*= 1000*/)
	:QDialog(parent)
{
	cavityMode = true;

	setWindowTitle(QString("Generate Random point"));//default title
	resize(180, 50);
	setMaximumSize(QSize(320, 400));
	setMinimumSize(QSize(320, 400));

	QVBoxLayout* layMain = new QVBoxLayout(this);

	QWidget *widgetArea = new QWidget(this);
	widgetArea->setFixedSize(320, 400);
	layMain->addWidget(widgetArea);

	QLabel *num_label = new QLabel(widgetArea);
	num_label->setGeometry(QRect(5, 7, 100, 20));
	num_label->setText("Number of point");


	m_editNum = new QLineEdit(widgetArea);
	m_editNum->setGeometry(QRect(120, 5, 100, 25));
	m_editNum->setText(QString::number(based));
	m_editNum->setValidator(new QIntValidator(this));


	///////////

	QLabel *diameter_x_label = new QLabel(widgetArea);
	diameter_x_label->setGeometry(QRect(5, 57, 100, 20));
	diameter_x_label->setText("X Radius");

	diameter_x_min_edit = new QLineEdit(widgetArea);
	diameter_x_min_edit->setGeometry(QRect(120, 55, 50, 25));
	diameter_x_min_edit->setText(QString::number(1));
	diameter_x_min_edit->setValidator(new QIntValidator(this));
	
	QLabel *range_label1 = new QLabel(widgetArea);
	range_label1->setGeometry(QRect(175, 57, 10, 20));
	range_label1->setText("~");

	diameter_x_max_edit = new QLineEdit(widgetArea);
	diameter_x_max_edit->setGeometry(QRect(190, 55, 50, 25));
	diameter_x_max_edit->setText(QString::number(1));
	diameter_x_max_edit->setValidator(new QIntValidator(this));

	QLabel *mm_label1 = new QLabel(widgetArea);
	mm_label1->setGeometry(QRect(245, 57, 25, 20));
	mm_label1->setText("mm");


	///////////

	QLabel *diameter_y_label = new QLabel(widgetArea);
	diameter_y_label->setGeometry(QRect(5, 87, 100, 20));
	diameter_y_label->setText("Y Radius");

	diameter_y_min_edit = new QLineEdit(widgetArea);
	diameter_y_min_edit->setGeometry(QRect(120, 85, 50, 25));
	diameter_y_min_edit->setText(QString::number(1));
	diameter_y_min_edit->setValidator(new QIntValidator(this));

	QLabel *range_label2 = new QLabel(widgetArea);
	range_label2->setGeometry(QRect(175, 87, 10, 20));
	range_label2->setText("~");

	diameter_y_max_edit = new QLineEdit(widgetArea);
	diameter_y_max_edit->setGeometry(QRect(190, 85, 50, 25));
	diameter_y_max_edit->setText(QString::number(1));
	diameter_y_max_edit->setValidator(new QIntValidator(this));

	QLabel *mm_label2 = new QLabel(widgetArea);
	mm_label2->setGeometry(QRect(245, 87, 25, 20));
	mm_label2->setText("mm");

	///////////

	QLabel *diameter_z_label = new QLabel(widgetArea);
	diameter_z_label->setGeometry(QRect(5, 117, 100, 20));
	diameter_z_label->setText("Z Radius");

	diameter_z_min_edit = new QLineEdit(widgetArea);
	diameter_z_min_edit->setGeometry(QRect(120, 115, 50, 25));
	diameter_z_min_edit->setText(QString::number(1));
	diameter_z_min_edit->setValidator(new QIntValidator(this));

	QLabel *range_label3 = new QLabel(widgetArea);
	range_label3->setGeometry(QRect(175, 117, 10, 20));
	range_label3->setText("~");

	diameter_z_max_edit = new QLineEdit(widgetArea);
	diameter_z_max_edit->setGeometry(QRect(190, 115, 50, 25));
	diameter_z_max_edit->setText(QString::number(1));
	diameter_z_max_edit->setValidator(new QIntValidator(this));

	QLabel *mm_label3 = new QLabel(widgetArea);
	mm_label3->setGeometry(QRect(245, 117, 25, 20));
	mm_label3->setText("mm");

	///////////

		
	QLabel *mean_HU_label = new QLabel(widgetArea);
	mean_HU_label->setGeometry(QRect(5, 167, 100, 20));
	mean_HU_label->setText("Mean HU");


	mean_HU_min_edit = new QLineEdit(widgetArea);
	mean_HU_min_edit->setGeometry(QRect(120, 165, 50, 25));
	mean_HU_min_edit->setText(QString::number(based));
	mean_HU_min_edit->setValidator(new QIntValidator(this));

	QLabel *range_label4 = new QLabel(widgetArea);
	range_label4->setGeometry(QRect(175, 165, 10, 20));
	range_label4->setText("~");

	mean_HU_max_edit = new QLineEdit(widgetArea);
	mean_HU_max_edit->setGeometry(QRect(190, 165, 50, 25));
	mean_HU_max_edit->setText(QString::number(based));
	mean_HU_max_edit->setValidator(new QIntValidator(this));

	//////////////////
	QLabel *boundary_HU_label = new QLabel(widgetArea);
	boundary_HU_label->setGeometry(QRect(5, 197, 100, 20));
	boundary_HU_label->setText("Boundary HU");


	boundary_HU_min_edit = new QLineEdit(widgetArea);
	boundary_HU_min_edit->setGeometry(QRect(120, 195, 50, 25));
	boundary_HU_min_edit->setText(QString::number(based));
	boundary_HU_min_edit->setValidator(new QIntValidator(this));

	QLabel *range_label5 = new QLabel(widgetArea);
	range_label5->setGeometry(QRect(175, 195, 10, 20));
	range_label5->setText("~");

	boundary_HU_max_edit = new QLineEdit(widgetArea);
	boundary_HU_max_edit->setGeometry(QRect(190, 195, 50, 25));
	boundary_HU_max_edit->setText(QString::number(based));
	boundary_HU_max_edit->setValidator(new QIntValidator(this));

	QLabel *cavity_boundary_label = new QLabel(widgetArea);
	cavity_boundary_label->setGeometry(QRect(5, 227, 100, 20));
	cavity_boundary_label->setText("Cavity Boundary");


	cavity_boundary_edit = new QLineEdit(widgetArea);
	cavity_boundary_edit->setGeometry(QRect(120, 225, 50, 25));
	cavity_boundary_edit->setText(QString::number(based));
	cavity_boundary_edit->setValidator(new QIntValidator(this));

	QLabel *range_label6 = new QLabel(widgetArea);
	range_label6->setGeometry(QRect(175, 225, 50, 20));
	range_label6->setText("pixel(s)");

	///////////////////////
	QGroupBox *boxGroup = new QGroupBox(this);
	boxGroup->setTitle("Boundary mode");
	QVBoxLayout *LayGroup = new QVBoxLayout;
	boxGroup->setLayout(LayGroup);

	m_maskGroup = new QButtonGroup(this);

	QRadioButton *radioCavity = new QRadioButton(this);
	radioCavity->setText("Cavity");
	radioCavity->setCheckable(true);
	radioCavity->setChecked(true);
	radioCavity->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

	QRadioButton *radioNodule = new QRadioButton(this);
	radioNodule->setText("Nodule");
	radioNodule->setCheckable(true);
	radioNodule->setChecked(false);
	radioNodule->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

	m_maskGroup->addButton(radioCavity, 0);
	m_maskGroup->addButton(radioNodule, 1);

	
	connect(m_maskGroup, SIGNAL(buttonClicked(int)), this, SLOT(OnChangeMode(int)));

	LayGroup->addWidget(radioCavity);
	LayGroup->addWidget(radioNodule);

	layMain->addWidget(boxGroup);
	

	////////////

	chkbox = new QCheckBox(this);
	chkbox->setText("Point Unit Only");
	chkbox->setCheckable(true);
	chkbox->setChecked(false);
	chkbox->setStyleSheet(STYLE_MANAGER->m_Checkbox);
	layMain->addWidget(chkbox);
	connect(chkbox, &QCheckBox::clicked, this, &NumDialog::OnPointOnlyCheck);


	connect(m_editNum, &QLineEdit::editingFinished, this, &NumDialog::OnNumChanged);


//	layMain->setContentsMargins(0, 10, 0, 10);
//	layMain->addWidget(m_editNum, 0, Qt::AlignVCenter);

	QPushButton *btn = new QPushButton("OK", this);
	btn->setGeometry(QRect(this->width() - 10, this->height() - 10, 30, 30));
	connect(btn, &QPushButton::clicked, this, &NumDialog::OnAccept);

	layMain->addWidget(btn);
//	layMain->addWidget(btn, 0, Qt::AlignVCenter);

	enablePointOnly = true;

	OnNumChanged();
}

void NumDialog::OnNumChanged()
{
	QString strVal = m_editNum->text();
	int val = strVal.toInt();

// 	if (val < m_bottom)
// 		val = m_bottom;
// 	if (val > m_top)
// 		val = m_top;

	if (QString::number(val) != strVal)
	{
		m_editNum->blockSignals(true);
		m_editNum->setText(QString::number(val));
		m_editNum->blockSignals(false);
	}
}

void NumDialog::OnChangeMode(int type)
{
	if (0 == type)//All
	{
		if (m_maskGroup)
			m_maskGroup->button(1)->setChecked(false);
	
		cavityMode = true;
		boundary_HU_min_edit->setVisible(true);
		boundary_HU_max_edit->setVisible(true);
		cavity_boundary_edit->setVisible(true);
		
	}
	else
	{
		if (m_maskGroup)
			m_maskGroup->button(0)->setChecked(false);
	
		cavityMode = false;
		boundary_HU_min_edit->setVisible(false);
		boundary_HU_max_edit->setVisible(false);
		cavity_boundary_edit->setVisible(false);
	}
}

void NumDialog::OnAccept()
{
	getNum();
	QDialog::accept();
}

void NumDialog::setTitle(QString & title)
{
	setWindowTitle(title);
}

void NumDialog::getNum()
{
	QString strVal = m_editNum->text();
	m_number_of_point = strVal.toInt();
	strVal = diameter_x_min_edit->text();
	m_x_diameter_min = strVal.toInt();
	strVal = diameter_x_max_edit->text();
	m_x_diameter_max = strVal.toInt();

	strVal = diameter_y_min_edit->text();
	m_y_diameter_min = strVal.toInt();
	strVal = diameter_y_max_edit->text();
	m_y_diameter_max = strVal.toInt();

	strVal = diameter_z_min_edit->text();
	m_z_diameter_min = strVal.toInt();
	strVal = diameter_z_max_edit->text();
	m_z_diameter_max = strVal.toInt();

	strVal = mean_HU_min_edit->text();
	m_mean_min_HU = strVal.toInt();
	strVal = mean_HU_max_edit->text();
	m_mean_max_HU = strVal.toInt();

	strVal = boundary_HU_min_edit->text();
	m_boundary_min_HU = strVal.toInt();
	strVal = boundary_HU_max_edit->text();
	m_boundary_max_HU = strVal.toInt();

	strVal = cavity_boundary_edit->text();
	m_cavity_boundary = strVal.toInt();

}



void NumDialog::OnPointOnlyCheck(bool val)
{
	if (chkbox->isChecked())
	{
		diameter_x_min_edit->setDisabled(false);
		diameter_x_max_edit->setDisabled(false);
		diameter_y_min_edit->setDisabled(false);
		diameter_y_max_edit->setDisabled(false);
		diameter_z_min_edit->setDisabled(false);
		diameter_z_max_edit->setDisabled(false);
		diameter_x_min_edit->setVisible(false);
		diameter_x_max_edit->setVisible(false);
		diameter_y_min_edit->setVisible(false);
		diameter_y_max_edit->setVisible(false);
		diameter_z_min_edit->setVisible(false);
		diameter_z_max_edit->setVisible(false);
		mean_HU_min_edit->setDisabled(false);
		mean_HU_max_edit->setDisabled(false);
		
		mean_HU_min_edit->setVisible(false);
		mean_HU_max_edit->setVisible(false);
		
		boundary_HU_max_edit->setVisible(false);
		boundary_HU_min_edit->setVisible(false);

		cavity_boundary_edit->setVisible(false);

		enablePointOnly = true;
	}
	else
	{

		diameter_x_min_edit->setDisabled(true);
		diameter_x_max_edit->setDisabled(true);
		diameter_y_min_edit->setDisabled(true);
		diameter_y_max_edit->setDisabled(true);
		diameter_z_min_edit->setDisabled(true);
		diameter_z_max_edit->setDisabled(true);
		diameter_x_min_edit->setVisible(true);
		diameter_x_max_edit->setVisible(true);
		diameter_y_min_edit->setVisible(true);
		diameter_y_max_edit->setVisible(true);
		diameter_z_min_edit->setVisible(true);
		diameter_z_max_edit->setVisible(true);
		mean_HU_min_edit->setDisabled(true);
		mean_HU_max_edit->setDisabled(true);
		
		mean_HU_min_edit->setVisible(true);
		mean_HU_max_edit->setVisible(true);
		
		boundary_HU_max_edit->setVisible(true);
		boundary_HU_min_edit->setVisible(true);

		cavity_boundary_edit->setVisible(true);

		enablePointOnly = false;
	}

}