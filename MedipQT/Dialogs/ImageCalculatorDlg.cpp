#include "stdafx.h"
#include "ImageCalculatorDlg.h"
#include "ActionManager.h"
#include "ActionMaskListsImageCalculate.h"
#include "ShortcutManager.h"
#include "windowManager.h"
#include "StyleManager.h"
#include "UI/MaskList.h"
#include "Windows/Tabwindow.h"
#include "Windows/Main/MainSegmentWidget.h"
#include "System/VolumeCalculator.h"
#include "DataContext.h"

#include <QLayout>
#include <QRadioButton>

#define SCROLL_VALUE_MULTIPLIER 100.0

//=======================================
//		ImageCalculatorDlg
//=======================================
ImageCalculatorDlg::ImageCalculatorDlg(MaskListWidget* pMaskListWidget, QWidget* parent) :
	QDialog(parent),
	m_pMaskListWidget(pMaskListWidget)
{
	/* TODO : Qt StyleSheet 대신 Code에서 인터페이스 생성할 것*/
	setupUi(this);

	setWindowTitle("Image Calculator");
	setStyleSheet(QString(
		"QWidget {"
		"color: white;"
		"font-family:Arial;"
		"background: rgba(82, 82, 82, 255); "
		"}"
	));

	connect(m_chkPreview, &QRadioButton::clicked, this, &ImageCalculatorDlg::onPreview);
	connect(m_btnOK, &QRadioButton::clicked, this, &ImageCalculatorDlg::onOK);
	connect(m_btnCancel, &QRadioButton::clicked, this, &ImageCalculatorDlg::onCancel);

	m_btnCancel->setText("Close");

	connect(m_rdoPlus, &QRadioButton::clicked, this, &ImageCalculatorDlg::onCalcBtnClick);
	connect(m_rdoMinus, &QRadioButton::clicked, this, &ImageCalculatorDlg::onCalcBtnClick);
	connect(m_rdoMultiply, &QRadioButton::clicked, this, &ImageCalculatorDlg::onCalcBtnClick);
	connect(m_rdoDivide, &QRadioButton::clicked, this, &ImageCalculatorDlg::onCalcBtnClick);
	connect(m_rdoReplace, &QRadioButton::clicked, this, &ImageCalculatorDlg::onCalcBtnClick);

	connect(m_spinValue, SIGNAL(valueChanged(double)), this, SLOT(onSpinValueChanged(double)));
	connect(m_scrollValue, SIGNAL(valueChanged(int)), this, SLOT(onScrollValueChanged(int)));

	m_rdoPlus->setStyleSheet(STYLE_MANAGER->m_Radiobtn);
	m_rdoMinus->setStyleSheet(STYLE_MANAGER->m_Radiobtn);
	m_rdoMultiply->setStyleSheet(STYLE_MANAGER->m_Radiobtn);
	m_rdoDivide->setStyleSheet(STYLE_MANAGER->m_Radiobtn);
	m_rdoReplace->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

	QFont font;
	font.setFamily("Gulim");

	font.setPixelSize(20);
	m_rdoPlus->setFont(font);
	m_rdoMinus->setFont(font);
	m_rdoReplace->setFont(font);

	font.setPixelSize(24);
	m_rdoMultiply->setFont(font);
	m_rdoDivide->setFont(font);

	m_chkPreview->setStyleSheet(STYLE_MANAGER->buttonBehind);
	m_btnOK->setStyleSheet(STYLE_MANAGER->buttonBehind);
	m_btnCancel->setStyleSheet(STYLE_MANAGER->buttonBehind);

	m_spinValue->setStyleSheet(STYLE_MANAGER->spinbox);

	m_rdoPlus->setChecked(true);
	m_spinValue->setValue(1.0);

	//m_scrollValue->setVisible(false);
	m_chkPreview->setVisible(false);
}


ImageCalculatorDlg::~ImageCalculatorDlg()
{
}

void ImageCalculatorDlg::keyPressEvent(QKeyEvent* e)
{
	QDialog::keyPressEvent(e);
}

void ImageCalculatorDlg::showEvent(QShowEvent *e)
{
	/* Image Calculator 중에는 Volume 편집을 할 수 없도록 한다. */
	WIN_MANAGER->GetTab()->getVolTab()->setEnabled(false);
}

void ImageCalculatorDlg::hideEvent(QHideEvent *e)
{
	WIN_MANAGER->GetTab()->getVolTab()->setEnabled(true);
	onClear();
	QDialog::hideEvent(e);
}

void ImageCalculatorDlg::onOK()
{
	UpdateVolumeCalculatorParameter();

	/* 전체 Volume Calculate*/
	VOLUME_CALCULATOR->CalcHU_Volume(
		&DATA_CONTEXT->volume_data
	);

	/* 화면 업데이트 */
	UpdateViewer();

	/* Volume 정보 undo,redo stack에 업데이트*/
	mint16* pOriginHUVolume = VOLUME_CALCULATOR->OriginHUVolumeData();
	if (pOriginHUVolume != nullptr)
	{
		ACTION_MANAGER->action_MaskList_ImageCalculate(pOriginHUVolume);
	}

	/* 계산된 Volume 정보 Update */
	VOLUME_CALCULATOR->Initialize(&DATA_CONTEXT->volume_data);

	WIN_MANAGER->GetTab()->getVolTab()->setEnabled(true);
	onClear();

	WIN_MANAGER->mainSegmentWidget->updateSharedInfo();
	//hide();
}

void ImageCalculatorDlg::onCancel()
{
	hide();
}

void ImageCalculatorDlg::onCalcBtnClick()
{
	UpdateVolumeCalculatorParameter();
	UpdateViewer();
}

void ImageCalculatorDlg::onSpinValueChanged(double value)
{
	int scrollValue = value * SCROLL_VALUE_MULTIPLIER;
	m_scrollValue->blockSignals(true);
	m_scrollValue->setValue(scrollValue);
	m_scrollValue->blockSignals(false);

	UpdateVolumeCalculatorParameter();

	//bool updateImage = false;
	//bool updateMask = true;
	//WIN_MANAGER->forceUpdate2DViewData(updateImage, updateMask);
	//WIN_MANAGER->renderLater_GridView();
}

void ImageCalculatorDlg::onScrollValueChanged(int value)
{
	double spinValue = value / SCROLL_VALUE_MULTIPLIER;
	m_spinValue->blockSignals(true);
	m_spinValue->setValue(spinValue);
	m_spinValue->blockSignals(false);

	UpdateVolumeCalculatorParameter();

	//bool updateImage = false;
	//bool updateMask = true;
	//WIN_MANAGER->forceUpdate2DViewData(updateImage, updateMask);
	//WIN_MANAGER->renderLater_GridView();
}

void ImageCalculatorDlg::onPreview()
{
	UpdateVolumeCalculatorParameter();

	if (VOLUME_CALCULATOR->IsPreviewON() == false)
	{
		VOLUME_CALCULATOR->Reset(&DATA_CONTEXT->volume_data);
	}

	UpdateViewer();
}

void ImageCalculatorDlg::UpdateVolumeCalculatorParameter()
{
	IMAGE_CALCULATOR_OPERATOR op = OP_PLUS;
	if (m_rdoPlus->isChecked())
	{
		op = OP_PLUS;
	}
	else if (m_rdoMinus->isChecked())
	{
		op = OP_MINUS;
	}
	else if (m_rdoMultiply->isChecked())
	{
		op = OP_MULTIPLY;
	}
	else if (m_rdoDivide->isChecked())
	{
		op = OP_DIVIDE;
	}
	else if (m_rdoReplace->isChecked())
	{
		op = OP_REPLACE;
	}
	double value = m_spinValue->value();

	std::vector<muint32> maskUIDs = m_pMaskListWidget->GetSelectedItemIndices();
	std::vector<MaskInfo*> maskInfos;
	MaskUtil::GetMaskInfos(maskInfos, &DATA_CONTEXT->volume_data, maskUIDs);

	bool previewOn = m_chkPreview->isChecked();
	VOLUME_CALCULATOR->SetCalcParameter(op, value, maskInfos);
	VOLUME_CALCULATOR->SetPreivewON(previewOn);
}

void ImageCalculatorDlg::UpdateViewer()
{
	bool updateImage = false;
	bool updateMask = true;
	WIN_MANAGER->forceUpdate2DViewData(updateImage, updateMask);
	WIN_MANAGER->renderLater_GridView();
}

void ImageCalculatorDlg::onClear()
{
	m_chkPreview->setChecked(false);
	UpdateVolumeCalculatorParameter();
	if (VOLUME_CALCULATOR->Reset(&DATA_CONTEXT->volume_data) == false)
	{
		return;
	}

	UpdateViewer();
}

