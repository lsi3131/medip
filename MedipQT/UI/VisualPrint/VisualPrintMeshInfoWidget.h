#pragma once

#ifndef VISUALPRINT_MESHINFO_WIDGET_H
#define VISUALPRINT_MESHINFO_WIDGET_H

#include "defineMEDIP.h"
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QDoubleSpinBox>
#include "DataContext.h"

class VisualPrintMeshInfoWidget : public QWidget // Visual Print Mesh info widget
{
	Q_OBJECT

public:
	VisualPrintMeshInfoWidget(DataContext* pDataContext, muint8 UID, QWidget* parent = NULL);

	void ChangeUID(mint8 UID);
	void EditCancel();
	void UpdateLayerConnection();
protected:
	bool eventFilter(QObject* watched, QEvent* e) override;

public slots:
	void Update();	//최신 정보로 '새로고침'
	void Edit();
	void Apply();

private slots:
	void OnScaleChanged();
	void OnDimChanged();
	void OnTransChanged();
	//	void OnAngleChanged();
	void OnRotationChanged();

private:
	muint8		UID;
	mint32		LayerUID;	//-1 : UNCONNECTED
	float		meshScale;

	QPushButton* btnUpdate;
	QPushButton* btnEdit;
	QPushButton* btnApply;

	QDoubleSpinBox* lineScale[3]; //scale (중점 기준 증/감)
	QDoubleSpinBox* lineDim[3]; //X,Y,Z (mm 기준)

	QDoubleSpinBox* lineTrans[3]; //locale(translation)
	QDoubleSpinBox* lineRotate[3];	//rotate

									//	QDoubleSpinBox	*lineAngle;	//원점에 대한 angle 값 표시
									//	QDoubleSpinBox	*lineRotate;	//원점에 대한 rotate 값 표시

	QLabel* labelConn;	//Connection label info

	QPushButton* HoverWidget;
	QIcon		LeaveIcon;

	DataContext* m_pDataContext;
};

#endif