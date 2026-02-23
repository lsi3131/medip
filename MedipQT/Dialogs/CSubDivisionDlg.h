/*
@company		메디컬아이피
@autor			허 건 과장
@date			2021-10-18
@brief			CSubDivision 헤더파일
*/

#pragma once

#ifndef CSUDDIVISION_DLG_H
#define CSUDDIVISION_DLG_H

#include "MeshBaseDlg.h"
#include <QDialog>

#include "DataContext.h"

class MEVolumeView;

class CSubDivisionDlg : public MeshBaseDlg
{
	Q_OBJECT

public:
	CSubDivisionDlg(DataContext* pDataContext, QWidget *parent = NULL);
	CSubDivisionDlg(DataContext* pDataContext, QString _title, QWidget *parent = NULL);
	virtual ~CSubDivisionDlg();
	virtual void reject(bool bForce = false);

private:
	void initQtUI();

	QComboBox* m_p_Combo_Method;
private slots:
	void OnOK();
	void OnCancel();
};
#endif