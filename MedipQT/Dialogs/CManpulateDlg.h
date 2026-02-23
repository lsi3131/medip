/*
@company		메디컬아이피
@autor			허 건 과장
@date			2021-03-11
@brief			CManpulateDlg 헤더파일
*/

#pragma once

#ifndef CMANIPULATOR_DLG_H
#define CMANIPULATOR_DLG_H

#include "MeshBaseDlg.h"
#include <QDialog>
#include "MeshEdit\CManipulator.h"
#include "DataContext.h"

class CManpulateDlg : public MeshBaseDlg
{
	Q_OBJECT

public:
	CManpulateDlg(DataContext* pDataContext, QWidget *parent = NULL);
	CManpulateDlg(DataContext* pDataContext, QString _title, QWidget *parent = NULL);
	virtual ~CManpulateDlg();

	virtual void reject(bool bForce = false);

	void UpdateMeshInfo(mip::MeshTopology* _p_mesh, MANIPULATOR_DIR_TYPE _type);

	double getInterval() { return m_p_spinboxInterval->value() * 0.1f; }

	void Reset();

private:
	void initQtUI();

	void CompleteProcess();

	void onPatientCoordinate(bool bInverse);

	QComboBox*	 m_p_cbSpace;

	QLabel*		 m_p_lbTranslate[3];
	QLabel*		 m_p_lbRotate[3];
	QLabel*		 m_p_lbScale[3];

	QDoubleSpinBox*	 m_p_spinboxInterval;

	bool		 m_bPatientCoordinate;
	bool		 m_bFinish;

private slots:
	void OnOK();
	void OnCancel();
	void OnSpinBoxValueChanged(double _val);
	void OnSpinBoxEditingFinished();
	void OnComboBoxIndexChanged(int);
};
#endif