#pragma once

#ifndef IMAGE_CALCULATOR_DLG_H
#define IMAGE_CALCULATOR_DLG_H

#include <qdialog>
#include "ui_ImageCalculatorDlg.h"

struct MaskInfo;
class VOLUME_DATA;
class QAction;
class MaskListWidget;

class ImageCalculatorDlg : public QDialog
	, public Ui::ImageCalculatorDlg
{
	Q_OBJECT

public:
	ImageCalculatorDlg(MaskListWidget* pMaskListWidget, QWidget* parent = nullptr);
	~ImageCalculatorDlg();

protected:
	virtual void keyPressEvent(QKeyEvent* e) override;
	virtual void showEvent(QShowEvent *e) override;
	virtual void hideEvent(QHideEvent *e) override;

private slots :
	void onPreview();
	void onClear();
	void onOK();
	void onCancel();
	void onCalcBtnClick();
	void onSpinValueChanged(double value);
	void onScrollValueChanged(int value);


private:
	void UpdateVolumeCalculatorParameter();
	void UpdateViewer();

private:
	MaskListWidget* m_pMaskListWidget;
};
#endif