/*
@company		메디컬아이피
@autor			허 건 과장
@date			2021-03-08
@brief			CHoleFillingDlg 헤더파일
*/

#pragma once

#ifndef CPLANECUT_DLG_H
#define CPLANECUT_DLG_H

#include "MeshBaseDlg.h"
#include <QDialog>

/*
@brief
*/
class CPlaneCutDlg : public MeshBaseDlg
{
	Q_OBJECT

public:
	CPlaneCutDlg(DataContext* pDataContext, QWidget *parent = NULL);
	CPlaneCutDlg(DataContext* pDataContext, QString _title, QWidget *parent = NULL);
	virtual ~CPlaneCutDlg();

	virtual void reject(bool bForce = false);

	int getTypeFilling();

private:
	void initQtUI();

	QComboBox*		m_p_ComboTypeFilling;

	QPushButton*	m_p_BtnOk;
	QPushButton*	m_p_BtnCancel;

private slots:
	void OnOK();
	void OnCancel();
};
#endif