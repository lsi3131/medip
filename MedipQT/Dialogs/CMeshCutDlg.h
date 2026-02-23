/*
@company		메디컬아이피
@autor			허 건 과장
@date			2021-12-17
@brief			CMeshCutDlg 클래스 헤더 파일
*/

#pragma once

#ifndef CMESHCUT_DLG_H
#define CMESHCUT_DLG_H

#include "MeshBaseDlg.h"
#include <QDialog>
#include "DataContext.h"

/*
@brief	메쉬 다이얼로그 클래스
*/
class	CMeshCutDlg : public MeshBaseDlg
{
public:
	CMeshCutDlg(DataContext* pDataContext, QWidget *parent = NULL);
	CMeshCutDlg(DataContext* pDataContext, QString _title, QWidget *parent = NULL);
	virtual ~CMeshCutDlg();

	virtual void reject(bool bForce = false);

	void clearSelectAreaALL();

	int getTypeFilling();

	void updateComboboxIndex(int index, bool bSaveUndoRedo = true);

public slots:
	void OnOK();
	void OnCancel();
	void OnClearSelection();
	void OnInverse();

	void OnComboChangeIndex(int);

private:
	void initQtUI();

	QComboBox* m_cb_CutMode;
	QComboBox* m_cb_FillingMode;

	QPushButton*	m_btnClearSelection;
	QPushButton*	m_btnInverse;

	QPushButton*	m_btnOk;
	QPushButton*	m_btnCancel;

	bool			m_bSaveUndoRedo;

	int				m_PrevIdx;
};
#endif
