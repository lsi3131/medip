#include "stdafx.h"
#include "TableDialog.h"

#include "System\styleManager.h"

TableDialog::TableDialog(QWidget* parent, Qt::WindowFlags f)
	: QDialog(parent, f), m_tableFormat(), m_rowSpin(nullptr), m_colSpin(nullptr), m_borderColorBtn(nullptr), m_defaultTable()
{
	setFixedSize(250, 150);
	setStyleSheet(STYLE_MANAGER->mainFrame);
	setWindowTitle("Table Property");

	m_tableFormat.setCellSpacing(0);
	m_tableFormat.setCellPadding(nPadding);
	m_tableFormat.setBorderBrush(QBrush(nBorderColor));

	createUI();
}

TableDialog::~TableDialog()
{

}

void TableDialog::createUI()
{
	QVBoxLayout* vMainLayout = new QVBoxLayout;
	setLayout(vMainLayout);

	vMainLayout->addWidget(createTableProperty(), 1);
	//vMainLayout->addWidget(createMEDIPDefaultTableButton(), 1);
	vMainLayout->addWidget(createBtnGroup(), 0);
}

QWidget* TableDialog::createTableProperty()
{
	QGroupBox* group = new QGroupBox("Table Property", this);
	group->setStyleSheet(STYLE_MANAGER->imgDialogGroupBox);
	QVBoxLayout* vGroupLayout = new QVBoxLayout;
	group->setLayout(vGroupLayout);

	QHBoxLayout* hLayout = new QHBoxLayout;
	hLayout->setSpacing(5);

	QLabel* rowLabel = new QLabel("Row", group);
	hLayout->addWidget(rowLabel, 0);
	m_rowSpin = new QSpinBox(group);
	m_rowSpin->setStyleSheet(STYLE_MANAGER->spinbox);
	m_rowSpin->setValue(2);
	hLayout->addWidget(m_rowSpin, 1);

	QLabel* colLabel = new QLabel("Col", group);
	hLayout->addWidget(colLabel, 0);
	m_colSpin = new QSpinBox(group);
	m_colSpin->setStyleSheet(STYLE_MANAGER->spinbox);
	m_colSpin->setValue(2);
	hLayout->addWidget(m_colSpin, 1);
	vGroupLayout->addItem(hLayout);

	hLayout = new QHBoxLayout;
	hLayout->setSpacing(5);

	QLabel* paddingLabel = new QLabel("Padding", group);
	hLayout->addWidget(paddingLabel, 0);
	QSpinBox* paddingSpin = new QSpinBox(group);
	paddingSpin->setStyleSheet(STYLE_MANAGER->spinbox);
	paddingSpin->setValue(nPadding);
	hLayout->addWidget(paddingSpin, 1);

	connect(paddingSpin, SIGNAL(valueChanged(int)), this, SLOT(OnPaddingChange(int)));

	QPixmap pix(16, 16);
	pix.fill(nBorderColor);
	m_borderColorBtn = new QPushButton(QIcon(pix), "Border Color", group);
	m_borderColorBtn->setStyleSheet(STYLE_MANAGER->buttonBehind);
	hLayout->addWidget(m_borderColorBtn, 2);

	connect(m_borderColorBtn, &QPushButton::clicked, this, &TableDialog::OnColorChange);

	vGroupLayout->addItem(hLayout);
	
	return group;
}

QWidget* TableDialog::createMEDIPDefaultTableButton()
{
	QGroupBox* group = new QGroupBox("Default Table", this);
	group->setStyleSheet(STYLE_MANAGER->imgDialogGroupBox);
	QVBoxLayout* vGroupLayout = new QVBoxLayout;
	group->setLayout(vGroupLayout);

	QPushButton* btn = new QPushButton("Auto Patient Data", group);
	btn->setStyleSheet(STYLE_MANAGER->buttonBehind);
	vGroupLayout->addWidget(btn);

	btn = new QPushButton("Texture Feature Table", group);
	btn->setStyleSheet(STYLE_MANAGER->buttonBehind);
	vGroupLayout->addWidget(btn);

	return group;
}

QWidget* TableDialog::createBtnGroup()
{
	QWidget* btnArea = new QWidget(this);

	QHBoxLayout* hLayout = new QHBoxLayout;
	hLayout->setMargin(0);
	btnArea->setLayout(hLayout);

	QPushButton* btn = new QPushButton("OK", btnArea);
	btn->setStyleSheet(STYLE_MANAGER->buttonBehind);
	hLayout->addWidget(btn);

	connect(btn, &QPushButton::clicked, this, &TableDialog::OnFinished);

	btn = new QPushButton("Cancel", btnArea);
	btn->setStyleSheet(STYLE_MANAGER->buttonBehind);
	hLayout->addWidget(btn);

	connect(btn, &QPushButton::clicked, this, &TableDialog::OnCancel);

	return btnArea;
}

void TableDialog::OnFinished()
{
	emit accept();
}

void TableDialog::OnCancel()
{
	this->close();
}

void TableDialog::OnPaddingChange(int val)
{
	m_tableFormat.setCellPadding(val);
}

void TableDialog::OnColorChange()
{
	QColorDialog dlg(this);
	dlg.setStyleSheet("color : white");
	if (dlg.exec() == QDialog::Accepted)
	{
		QColor color = dlg.selectedColor();
		m_tableFormat.setBorderBrush(QBrush(color));
		QPixmap pix(16, 16);
		pix.fill(color);
		m_borderColorBtn->setIcon(pix);
	}
}

//========================================================================

TableCellSplitDialog::TableCellSplitDialog(QWidget* parent, Qt::WindowFlags f)
	: QDialog(parent, f), m_columEditBox(nullptr), m_rowEditBox(nullptr)
{
	setFixedSize(200, 150);
	setStyleSheet(STYLE_MANAGER->mainFrame);
	setWindowTitle("Split Cell");

	QWidget* widget = new QWidget(this);
	widget->setFixedSize(this->size());

	QVBoxLayout* vMainLayout = new QVBoxLayout;
	widget->setLayout(vMainLayout);

	QGroupBox* group = new QGroupBox("Number", this);
	group->setStyleSheet(STYLE_MANAGER->imgDialogGroupBox);

	QVBoxLayout* vLayout = new QVBoxLayout;
	group->setLayout(vLayout);

	vLayout->addWidget(createRowNumberGroup(group));
	vLayout->addWidget(createColumNumberGroup(group));

	vMainLayout->addWidget(group, 1);
	vMainLayout->addWidget(createBtnGroup(), 0);
}


TableCellSplitDialog::~TableCellSplitDialog()
{
}

QWidget* TableCellSplitDialog::createRowNumberGroup(QWidget* parent)
{
	QWidget* group = new QWidget(parent);

	QHBoxLayout* hLayout = new QHBoxLayout;
	hLayout->setSpacing(0);
	hLayout->setMargin(0);
	group->setLayout(hLayout);

	QLabel* label = new QLabel("Column", group);
	hLayout->addWidget(label, 1);

	m_columEditBox = new QSpinBox(group);
	m_columEditBox->setFixedWidth(this->size().width() / 2);
	m_columEditBox->setStyleSheet(STYLE_MANAGER->spinbox);
	m_columEditBox->setMaximum(50);
	m_columEditBox->setValue(2);
	hLayout->addWidget(m_columEditBox, 1, Qt::AlignRight);

	return group;
}

QWidget* TableCellSplitDialog::createColumNumberGroup(QWidget* parent)
{
	QWidget* group = new QWidget(parent);

	QHBoxLayout* hLayout = new QHBoxLayout;
	hLayout->setSpacing(0);
	hLayout->setMargin(0);
	group->setLayout(hLayout);

	QLabel* label = new QLabel("Row", group);
	hLayout->addWidget(label, 1);

	m_rowEditBox = new QSpinBox(group);
	m_rowEditBox->setFixedWidth(this->size().width() / 2);
	m_rowEditBox->setStyleSheet(STYLE_MANAGER->spinbox);
	m_rowEditBox->setMaximum(50);
	m_rowEditBox->setValue(2);
	hLayout->addWidget(m_rowEditBox, 1, Qt::AlignRight);

	return group;
}

QWidget* TableCellSplitDialog::createBtnGroup()
{
	QWidget* btnArea = new QWidget(this);

	QHBoxLayout* hLayout = new QHBoxLayout;
	hLayout->setMargin(0);
	btnArea->setLayout(hLayout);

	QPushButton* btn = new QPushButton("OK", btnArea);
	btn->setStyleSheet(STYLE_MANAGER->buttonBehind);
	hLayout->addWidget(btn);

	connect(btn, &QPushButton::clicked, this, &TableCellSplitDialog::OnFinished);

	btn = new QPushButton("Cancel", btnArea);
	btn->setStyleSheet(STYLE_MANAGER->buttonBehind);
	hLayout->addWidget(btn);

	connect(btn, &QPushButton::clicked, this, &TableCellSplitDialog::OnCancel);

	return btnArea;
}

std::pair<int, int> TableCellSplitDialog::getSplitNumber()
{
	std::pair<int, int> res(m_columEditBox->value(), m_rowEditBox->value());
	return res;
}

void TableCellSplitDialog::OnFinished()
{
	emit accept();
}

void TableCellSplitDialog::OnCancel()
{
	this->close();
}

//==========================================================

TableCellDialog::TableCellDialog(const QColor& cellColor, QWidget* parent, Qt::WindowFlags f)
	: QDialog(parent, f), m_tableCellFormat()
{
	setFixedSize(250, 165);
	setStyleSheet(STYLE_MANAGER->mainFrame);
	setWindowTitle("Cell Property");

	m_tableCellFormat.setBackground(QBrush(cellColor));

	QVBoxLayout* vMainLayout = new QVBoxLayout;
	setLayout(vMainLayout);

	QGroupBox* group = new QGroupBox("Padding", this);
	group->setStyleSheet(STYLE_MANAGER->imgDialogGroupBox);
	vMainLayout->addWidget(group, 1);

	QGridLayout* gridLay = new QGridLayout;
	gridLay->setSpacing(5);
	group->setLayout(gridLay);

	QLabel* leftLabel = new QLabel("Left", group);
	gridLay->addWidget(leftLabel, 0, 0);

	QSpinBox* leftSpin = new QSpinBox(group);
	leftSpin->setStyleSheet(STYLE_MANAGER->spinbox);
	leftSpin->setValue(TableDialog::getTablePadding());
	gridLay->addWidget(leftSpin, 0, 1);
	m_widgetList["leftSpin"] = leftSpin;

	QLabel* rightLabel = new QLabel("Right", group);
	gridLay->addWidget(rightLabel, 0, 2);

	QSpinBox* rightSpin = new QSpinBox(group);
	rightSpin->setStyleSheet(STYLE_MANAGER->spinbox);
	rightSpin->setValue(TableDialog::getTablePadding());
	gridLay->addWidget(rightSpin, 0, 3);
	m_widgetList["rightSpin"] = rightSpin;

	QLabel* topLabel = new QLabel("Top", group);
	gridLay->addWidget(topLabel, 1, 0);

	QSpinBox* topSpin = new QSpinBox(group);
	topSpin->setStyleSheet(STYLE_MANAGER->spinbox);
	topSpin->setValue(TableDialog::getTablePadding());
	gridLay->addWidget(topSpin, 1, 1);
	m_widgetList["topSpin"] = topSpin;

	QLabel* botLabel = new QLabel("Bottom", group);
	gridLay->addWidget(botLabel, 1, 2);

	QSpinBox* botSpin = new QSpinBox(group);
	botSpin->setStyleSheet(STYLE_MANAGER->spinbox);
	botSpin->setValue(TableDialog::getTablePadding());
	gridLay->addWidget(botSpin, 1, 3);
	m_widgetList["botSpin"] = botSpin;

	QPixmap pix(16, 16);
	pix.fill(cellColor);
	QPushButton* btnColor = new QPushButton(QIcon(pix), "Cell Color", this);
	btnColor->setStyleSheet(STYLE_MANAGER->buttonBehind);
	vMainLayout->addWidget(btnColor);
	m_widgetList["btnColor"] = btnColor;

	connect(btnColor, &QPushButton::clicked, this, &TableCellDialog::OnColorChange);

	QHBoxLayout* btnLayout = new QHBoxLayout;
	btnLayout->setSpacing(5);
	
	QPushButton* btnOK = new QPushButton("OK", this);
	btnOK->setStyleSheet(STYLE_MANAGER->buttonBehind);
	btnLayout->addWidget(btnOK);

	connect(btnOK, &QPushButton::clicked, this, &TableCellDialog::OnFinished);

	QPushButton* btnCancel = new QPushButton("Cancel", this);
	btnCancel->setStyleSheet(STYLE_MANAGER->buttonBehind);
	btnLayout->addWidget(btnCancel);

	connect(btnCancel, &QPushButton::clicked, this, &TableCellDialog::OnCancel);

	vMainLayout->addItem(btnLayout);
}

TableCellDialog::~TableCellDialog()
{

}

const QTextTableCellFormat& TableCellDialog::getTableCellFormat()
{
	m_tableCellFormat.setLeftPadding(dynamic_cast<QSpinBox*>(m_widgetList["leftSpin"])->value());
	m_tableCellFormat.setRightPadding(dynamic_cast<QSpinBox*>(m_widgetList["rightSpin"])->value());
	m_tableCellFormat.setTopPadding(dynamic_cast<QSpinBox*>(m_widgetList["topSpin"])->value());
	m_tableCellFormat.setBottomPadding(dynamic_cast<QSpinBox*>(m_widgetList["botSpin"])->value());

	return m_tableCellFormat;
}

void TableCellDialog::OnFinished()
{
	emit accept();
}

void TableCellDialog::OnCancel()
{
	this->close();
}

void TableCellDialog::OnColorChange()
{
	QColorDialog dlg(this);
	if (dlg.exec() == QDialog::Accepted)
	{
		QColor color = dlg.selectedColor();
		m_tableCellFormat.setBackground(QBrush(color));
		QPixmap pix(16, 16);
		pix.fill(color);
		dynamic_cast<QPushButton*>(m_widgetList["btnColor"])->setIcon(QIcon(pix));
	}
}