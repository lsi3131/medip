#include "stdafx.h"
#include "IMGProcessTab.h"
#include "StringManager.h"
#include "StyleManager.h"
#include "WindowManager.h"
#include "ProductManager.h"
#include "Main/MainSegmentWidget.h"
#include "DataContext.h"

IMGProcessTab::IMGProcessTab(QWidget* parent /*= NULL*/)
	:CollapseWidget(QString(), parent)
{
	int nRow = 0;

	//QGroupBox* grpBox = new QGroupBox(this);
	QGroupBox* grpBox = PRODUCT_FACTORY->createWidget<QGroupBox>(MFL_Common_VolumeOperation_Filtration, this);
	QHBoxLayout* grpLay = nullptr;
	if (grpBox)
	{
		grpBox->setTitle("Spatial Variability Minimization");
		grpLay = new QHBoxLayout;
		grpBox->setLayout(grpLay);
		addWidget(grpBox, nRow++);
	}

	for (int i = 0; i < 5; i++)
	{
		//m_btnEnhance[i] = new QPushButton(QString(" x%1 ").arg(i + 1), this);
		m_btnEnhance[i] = PRODUCT_FACTORY->createWidget<QPushButton>(MFL_Common_VolumeOperation_Filtration, this);
		if (m_btnEnhance[i])
		{
			m_btnEnhance[i]->setText(QString(" x%1 ").arg(i + 1));
			m_btnEnhance[i]->setCheckable(true);
			m_btnEnhance[i]->setChecked(false);
			m_btnEnhance[i]->setObjectName(QString::number(5 - i));
			m_btnEnhance[i]->setStyleSheet(STYLE_MANAGER->buttonBehind);
			connect(m_btnEnhance[i], &QPushButton::clicked, this, &IMGProcessTab::slot_OnEnhance);
			if (grpLay) grpLay->addWidget(m_btnEnhance[i]);
		}
		//	addWidget(m_btnEnhance[i], nRow, 0, QMargins(), Qt::AlignLeft);
	}
	//QPushButton *btnReset = new QPushButton(STRING_MANAGER->getString(STR_RESET), this);
	QPushButton* btnReset = PRODUCT_FACTORY->createWidget<QPushButton>(MFL_Common_VolumeOperation_Filtration, this);
	if (btnReset)
	{
		btnReset->setText(STRING_MANAGER->getString(STR_RESET));
		btnReset->setStyleSheet(STYLE_MANAGER->buttonBehind);
		connect(btnReset, &QPushButton::clicked, this, &IMGProcessTab::slot_OnReset);
	}

#ifdef DEV_VER
	/////////////////////// Gaussian Smoothing
	grpBox = nullptr;
	grpLay = nullptr;
	//grpBox = new QGroupBox(this);
	grpBox = PRODUCT_FACTORY->createWidget<QGroupBox>(MFL_Common_VolumeOperation_Filtration, this);
	if (grpBox)
	{
		grpBox->setTitle("Gaussian Smoothing");
		grpLay = new QHBoxLayout;
		grpBox->setLayout(grpLay);
		addWidget(grpBox, nRow, 1);
	}

	for (int i = 0; i < 2; i++)
	{
		//m_btnGaussian[i] = new QPushButton(QString("%1x%1x%1").arg(3 + i * 2), this);
		m_btnGaussian[i] = PRODUCT_FACTORY->createWidget<QPushButton>(MFL_Common_VolumeOperation_Filtration, this);
		if (m_btnGaussian[i])
		{
			m_btnGaussian[i]->setText(QString("%1x%1x%1").arg(3 + i * 2));
			m_btnGaussian[i]->setCheckable(true);
			m_btnGaussian[i]->setChecked(false);
			m_btnGaussian[i]->setObjectName(QString::number(i));
			m_btnGaussian[i]->setStyleSheet(STYLE_MANAGER->buttonBehind);
			connect(m_btnGaussian[i], &QPushButton::clicked, this, &IMGProcessTab::slot_OnGaussian);
			if (grpLay) grpLay->addWidget(m_btnGaussian[i], 1);
		}
	}

	//QWidget *emptyBox1 = new QWidget(this);
	QWidget* emptyBox1 = PRODUCT_FACTORY->createWidget<QWidget>(MFL_Common_VolumeOperation_Filtration, this);
	if (emptyBox1)
	{
		emptyBox1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		addWidget(emptyBox1, nRow++, 1);
	}

	/////////////////////// Laplacian of Gaussian
	//grpBox = new QGroupBox(this);
	grpBox = nullptr;
	grpBox = PRODUCT_FACTORY->createWidget<QGroupBox>(MFL_Common_VolumeOperation_Filtration, this);
	if (grpBox)
	{
		grpBox->setTitle("Laplacian of Gaussian");
		grpLay = new QHBoxLayout;
		grpBox->setLayout(grpLay);
		addWidget(grpBox, nRow, 1);
	}

	for (int i = 0; i < 2; i++)
	{
		//m_btnLaplacian[i] = new QPushButton(QString("%1x%1x%1").arg(3 + i * 2), this);
		m_btnLaplacian[i] = PRODUCT_FACTORY->createWidget<QPushButton>(MFL_Common_VolumeOperation_Filtration, this);
		if (m_btnLaplacian[i])
		{
			m_btnLaplacian[i]->setText(QString("%1x%1x%1").arg(3 + i * 2));
			m_btnLaplacian[i]->setCheckable(true);
			m_btnLaplacian[i]->setChecked(false);
			m_btnLaplacian[i]->setObjectName(QString::number(i));
			m_btnLaplacian[i]->setStyleSheet(STYLE_MANAGER->buttonBehind);
			connect(m_btnLaplacian[i], &QPushButton::clicked, this, &IMGProcessTab::slot_OnLaplacian);
			grpLay->addWidget(m_btnLaplacian[i], 1);
		}
	}

	//emptyBox1 = new QWidget(this);
	emptyBox1 = nullptr;
	emptyBox1 = PRODUCT_FACTORY->createWidget<QWidget>(MFL_Common_VolumeOperation_Filtration, this);
	if (emptyBox1)
	{
		emptyBox1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		addWidget(emptyBox1, nRow++, 1);
	}

#endif
	if (btnReset) addWidget(btnReset, nRow, 1, QMargins(0, 15, 10, 0));

	//emptyBox1 = new QWidget(this);
	QWidget* emptyBox2 = nullptr;
	emptyBox2 = PRODUCT_FACTORY->createWidget<QWidget>(MFL_Common_VolumeOperation_Filtration, this);
	if (emptyBox2)
	{
		emptyBox2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		addWidget(emptyBox2, nRow++, 1);
	}

	//QWidget *emptyBox0 = new QWidget(this);
	// 	QWidget* emptyBox0 = PRODUCT_FACTORY->createWidget<QWidget>(MS_COLLAPS_FILTRATION_TAB, this);
	// 	if (emptyBox0)
	// 	{
	// 		emptyBox0->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
	// 		addWidget(emptyBox0, nRow++);
	// 	}
	//TODO
	//1. Enhanced menu right show
	QString title = "Filtration";
	setTitle(title);

	setOpenWidget(false);
}

void IMGProcessTab::slot_OnEnhance()
{
	if (!DATA_CONTEXT->volume_data.isValidate())
	{
		for (int i = 0; i < 5; i++)
			m_btnEnhance[i]->setChecked(false);
		return;
	}

	QPushButton* btnCheck = NULL;

	for (int i = 0; i < 5; i++)
	{
		if (m_btnEnhance[i]->isChecked())
		{
			btnCheck = m_btnEnhance[i];
			m_btnEnhance[i]->setChecked(false);
			break;
		}
	}

	int reduce = btnCheck->objectName().toInt();

	ACTION_MANAGER->action_ImageEnhance(reduce);
}

void IMGProcessTab::slot_OnReset()
{
	if (!DATA_CONTEXT->volume_data.isValidate()) return;

	ACTION_MANAGER->action_ImageEnhance(0);
}

void IMGProcessTab::slot_OnGaussian()
{
	if (!DATA_CONTEXT->volume_data.isValidate())
	{
		for (int i = 0; i < 5; i++)
			m_btnGaussian[i]->setChecked(false);
		return;
	}

	QPushButton* btnCheck = NULL;

	int index = 0;
	for (; index < 2; index++)
	{
		if (m_btnGaussian[index]->isChecked())
		{
			btnCheck = m_btnGaussian[index];
			m_btnGaussian[index]->setChecked(false);
			break;
		}
	}

	ACTION_MANAGER->action_ImageGaussian(index);


}

void IMGProcessTab::slot_OnLaplacian()
{
	if (!DATA_CONTEXT->volume_data.isValidate())
	{
		for (int i = 0; i < 5; i++)
			m_btnLaplacian[i]->setChecked(false);
		return;
	}

	QPushButton* btnCheck = NULL;

	int index = 0;
	for (; index < 2; index++)
	{
		if (m_btnLaplacian[index]->isChecked())
		{
			btnCheck = m_btnLaplacian[index];
			m_btnLaplacian[index]->setChecked(false);
			break;
		}
	}

	ACTION_MANAGER->action_ImageLaplacian(index);


}
