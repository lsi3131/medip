/*
@company		메디컬아이피
@autor			허 건 과장
@date			2021-10-18
@brief			CIslandFilterDlg 헤더파일
*/

#pragma once

#ifndef CISLANDFILTER_DLG_H
#define CISLANDFILTER_DLG_H

#include "MeshBaseDlg.h"
#include <QDialog>
#include "DataContext.h"

class MEVolumeView;

class CIslandFilterDlg : public MeshBaseDlg
{
	Q_OBJECT

public:
	CIslandFilterDlg(DataContext* pDataContext, QWidget *parent = NULL);
	CIslandFilterDlg(DataContext* pDataContext, QString _title, QWidget *parent = NULL);
	virtual ~CIslandFilterDlg();
	virtual void reject(bool bForce = false);

private:
	void initQtUI();

	QCheckBox*	 m_p_CheckBox_Remain_Large;
	QCheckBox*	 m_p_CheckBox_Remain_Inverse;
	QCheckBox*	 m_p_CheckBox_num_Face;
	QSpinBox*	 m_p_SpinBox_Remain;
	QSpinBox*	 m_p_SpinBox_Triangle;

private slots:
	void OnOK();
	void OnCancel();

	void OnCheckBoxLargeModeClicked();
	void OnCheckBoxSmallClicked();
	void OnCheckBoxFaceClicked();
};
#endif