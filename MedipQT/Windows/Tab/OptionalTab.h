#pragma once

#ifndef OPTIONTAB_H
#define OPTIONTAB_H

#include "CollapseWidget.h"

class ProductManager;
class Factory;

class OptionalTab : public CollapseWidget
{
	Q_OBJECT

public:
	OptionalTab(ProductManager* pProductManager, QWidget* parent = NULL);

	void setGammaMode(bool val);
	bool isGammaMode();

public slots:
	void slot_OnReset();

protected:
	bool eventFilter(QObject* watched, QEvent* e) override;

private:
	void InAreaGamma(float&);

private:
	ProductManager* m_pProductManager;
	Factory* m_pProductFactory;

	QSlider* m_slideGamma;
	QLabel* m_labelGamma;
	QLineEdit* m_textGamma;
	QCheckBox* m_chkGamma;
	QPushButton* m_btnGamma;
	QLineEdit* m_textLimit;

	QLineEdit* m_textVisualPrintURL;
	QLineEdit* m_textVisualPrintPort;

	QPushButton* m_btnTmp;
	QPushButton* m_btnTmp_report;
	QLineEdit* m_lineTmp;
	QLineEdit* m_lineTmp_report;
	QButtonGroup* m_FuncGroup;

	// auto save
	QCheckBox* m_chkAutoSave;
	QLabel* m_labelInterval;
	QLineEdit* m_textAutoSave;

	QCheckBox* m_chkCubeRender;

	QSpinBox* m_spinBrushThickness;
	QSpinBox* m_spinBrushOpacity;

	QCheckBox* m_export3DMeshMarchingCube;
	QCheckBox* m_export3DMeshFitSize;

private slots:
	void slot_OnUndoEditFinish();
	void slot_OnGammaEditFinish();
	void slot_OnGammaChanged(int);
	void slot_OnChkGamma();
	void slot_OnTempDirOpen();
	void slot_OnTempDirOpen_report();
	void slot_OnExpertReportFileOpen();
	void slot_OnChangeMode(int type);
	void slot_OnChkAutoSave();
	void slot_OnChkCubeRender();
	void slot_OnAutoSaveEditFinish();
	void slot_OnVisualPrintURLEditFinish();
	void slot_OnVisualPrintPortEditFinish();

	// Brush (not mesh)
	void slot_OnSpinBoxBrushThicknessChanged(int);
	void slot_OnSpinBoxBrushOpacityChanged(int);

	// Export 3D Mesh Method
	void slot_OnExport3DMeshFitSizeClicked();
	void slot_OnExport3DMeshMarchingCubeClicked();
};
#endif