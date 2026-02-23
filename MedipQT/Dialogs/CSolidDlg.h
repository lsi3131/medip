/*
@company		메디컬아이피
@autor			허 건 대리
@date			2021-01-05
@brief			CSolidDlg 헤더파일
*/

#pragma once

#ifndef CSOLID_DLG_H
#define CSOLID_DLG_H

#include "MeshBaseDlg.h"
#include <QDialog>

#include "DataContext.h"

class CSolidDlg : public MeshBaseDlg
{
	Q_OBJECT

public:
	CSolidDlg(DataContext* pDataContext, QWidget *parent = NULL);
	CSolidDlg(DataContext* pDataContext, QString _title, QWidget *parent = NULL);
	virtual ~CSolidDlg();
	virtual void reject(bool bForce = false);

private:
	void initQtUI();

	float m_EdgeLenth;

	QCheckBox* m_p_cb_auto;

private slots:
	void OnOK();
	void OnCancel();

	void OnCheckBoxClickedAutoMode();
	//void OnDepthSlideReleased(int);
};
#endif
