/*
@company		메디컬아이피
@autor			허 건 대리
@date			2021-01-07
@brief			CHoleFillingDlg 헤더파일
*/

#pragma once

#ifndef CHOLEFILLING_DLG_H
#define CHOLEFILLING_DLG_H

#include "MeshBaseDlg.h"
#include <QDialog>

#include "DataContext.h"

class CHoleFillingDlg : public MeshBaseDlg
{
	Q_OBJECT

public	:
	CHoleFillingDlg(DataContext* pDataContext, QWidget *parent = NULL);
	CHoleFillingDlg(DataContext* pDataContext, QString _title, QWidget *parent = NULL);
	virtual ~CHoleFillingDlg();

	virtual void reject(bool bForce = false);

	bool isCheckedUse();
	bool isCheckedBasic();
	bool isCheckedRemesh();
	bool isCheckedSmooth();

private :
	void initQtUI();

	QCheckBox*	 m_p_CheckBoxUse;
	QCheckBox*	 m_p_CheckBoxBasic;
	QCheckBox*	 m_p_CheckBoxRemesh;
	QCheckBox*	 m_p_CheckBoxSmooth;

	QPushButton* m_p_BtnFillHoleAll;

private slots:
	void OnCancel();
	void OnFillHoleAll();
};
#endif