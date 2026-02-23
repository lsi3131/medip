/*
@company		메디컬아이피
@autor			허 건 대리
@date			2021-01-05
@brief			CRemeshDlg 헤더파일
*/

#pragma once

#ifndef CREMESH_DLG_H
#define CREMESH_DLG_H

#include "MeshBaseDlg.h"
#include <QDialog>

#include "DataContext.h"

class CRemeshDlg : public MeshBaseDlg
{
	Q_OBJECT

public : 
	CRemeshDlg(DataContext* pDataContext, QWidget *parent = NULL);
	CRemeshDlg(DataContext* pDataContext, QString _title, QWidget *parent = NULL);
	virtual ~CRemeshDlg();
	virtual void reject(bool bForce = false);

	//void Update(std::vector<bool> & _vt_pick_model);
	void Update();

	void setSelectMode(bool _flag) { m_bSeletMode = _flag; }
private :
	void initQtUI();

	QComboBox*	 m_pComboBoxMode;
	QLineEdit*	 m_pLineEdit;
	QCheckBox*	 m_p_CheckBoxAuto;
	QCheckBox*	 m_p_CheckBoxPreserve;

	_Slider		m_Slider;

	float m_EdgeLenthTemp;
	float m_EdgeLenth;
	float m_Interval;
	int	  m_PosSlider;

	bool  m_bSeletMode;

private slots:
	void OnOK();
	void OnCancel();

	void OnDepthSlideReleased(int);
	void OnCheckBoxModeClicked();
};
#endif
