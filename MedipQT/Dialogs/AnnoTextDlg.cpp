#include "stdafx.h"
#include "AnnoTextDlg.h"
#include "System/resourceManager.h"
#include "System/stringManager.h"


AnnoTextDlg::AnnoTextDlg(QWidget* parent, QString & text, int size, COLOR & color)
	: QDialog(parent)
{
	this->ok = false;
	this->Text = text;
	this->FontSize = size;
	this->Color = QColor(color.r, color.g, color.b);

	this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
	resize(500, 300);

	QVBoxLayout * layout = new QVBoxLayout(this);
	QHBoxLayout * sizeLayout = new QHBoxLayout();
	layout->addLayout(sizeLayout);

	QLabel * labelText = new QLabel(this);
	labelText->setText(STRING_MANAGER->getString(STR_TEXT_SIZE));
	labelText->setMaximumWidth(65);
	sizeLayout->addWidget(labelText);


	QSpinBox *sizeBox = new QSpinBox;
	sizeBox->setValue(size);
	connect(sizeBox, SIGNAL(valueChanged(int)), this, SLOT(OnFontSizeChanged(int)) );
	sizeLayout->addWidget(sizeBox);

	textEdit = new QTextEdit(text, this);
	connect(textEdit, &QTextEdit::textChanged, this, &AnnoTextDlg::OnTextChanged);
	layout->addWidget(textEdit);

	labelColor = new QLabel(this);
	QPushButton * buttonColor = new QPushButton(STRING_MANAGER->getString(STR_COLOR_CHANGE), this);
	QPushButton * button = new QPushButton(STRING_MANAGER->getString(STR_OK), this);
	QPushButton * button2 = new QPushButton(STRING_MANAGER->getString(STR_CANCEL), this);
	connect(buttonColor, &QPushButton::released, this, &AnnoTextDlg::OnColorChangeClick);
	connect(button, &QPushButton::released, this, &AnnoTextDlg::OnOK);
	connect(button2, &QPushButton::released, this, &AnnoTextDlg::OnCANCEL);
	layout->addWidget(labelColor);
	layout->addWidget(buttonColor);
	layout->addWidget(button);
	layout->addWidget(button2);
	setLayout(layout);

	setWindowTitle(tr("Anno Text Dialog"));

	OnFontSizeChanged(size);
}

AnnoTextDlg::~AnnoTextDlg()
{
}

void AnnoTextDlg::OnFontSizeChanged(int i)
{
	textEdit->setFontPointSize(i);
	this->FontSize = i;
}

void AnnoTextDlg::OnColorChangeClick() 
{
	QColorDialog dlg(this);
	dlg.setStyleSheet("color : white");
	if (dlg.exec() == QDialog::Accepted)
	{
		this->Color = dlg.selectedColor();
		textEdit->setTextColor(this->Color);
	}
}

void AnnoTextDlg::OnOK()
{
	this->ok = true;
	accept();
}

void AnnoTextDlg::OnCANCEL()
{
	this->ok = false;
	reject();
}

void AnnoTextDlg::OnTextChanged()
{
	Text = textEdit->toPlainText();
}
