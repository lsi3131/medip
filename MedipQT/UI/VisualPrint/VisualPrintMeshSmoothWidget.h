#pragma once

#ifndef VISUALPRINT_MESHSMOOTH_WIDGET_H
#define VISUALPRINT_MESHSMOOTH_WIDGET_H

#include "defineMEDIP.h"
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QDoubleSpinBox>

#include "DataContext.h"

class VisualPrintMeshSmoothWidget : public QWidget
{
	Q_OBJECT
public:
	VisualPrintMeshSmoothWidget(DataContext* pDataContext, muint32 uid, bool bCreate, QWidget* parent);
	muint8 GetSmoothParam();
	muint8 GetRemeshParam();

	void setParams(int _val_smooth, int _val_reduce);
	void setSmoothVal(int _val_smooth);
	void setReduceVal(int _val_reduce);

protected:
	bool eventFilter(QObject* watched, QEvent* evt) override;

private slots:
	// For Smooth Control
	void slot_OnSliderMoved(int pos);
	void slot_OnSliderPress();
	void slot_OnSliderRelease();
	void slot_OnSpinChanged(int val);

	// For Reduce Control
	void slot_OnSliderMovedReduce(int pos);
	void slot_OnSliderPressReduce();
	void slot_OnSliderReleaseReduce();
	void slot_OnSpinChangedReduce(int val);

	// For Remesh Control
	void slot_OnSliderMovedRemesh(int pos);
	void slot_OnSliderPressRemesh();
	void slot_OnSliderReleaseRemesh();
	void slot_OnSpinChangedRemesh(int val);

private:
	// For Smooth Control - Start
	QSlider* m_slider;//	QSpinBox	*m_spinbox;
	QLabel* m_labelMeshSmooth;

	bool m_press;
	int m_nSmoothLevel;	// Smooth Parameter (0 ~ 100)

	// Mesh Reduce
	QSlider* m_sliderReduction;
	QLabel* m_labelMeshReduce;
	bool m_bPressReduce;
	int m_nReductionLevel;	// Reduction Parameter (0 ~ 100)

	muint8 m_muint32MeshInfoUID;
	// For Smooth Control - End	

	// For Remesh Control - Start	
	QSlider* m_sliderRemesh;
	QSpinBox* m_pSpinBoxRemesh;
	bool m_bPressRemesh;
	int m_nRemeshLevel;	// Remesh Parameter (0 ~ 100)

	DataContext* m_pDataContext;
};
#endif