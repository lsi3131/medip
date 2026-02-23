/*
@company		메디컬아이피
@autor			허 건 과장
@date			2021-12-09
@brief			CVoxelCountRatioDlg 헤더파일
*/

#pragma once

#ifndef CVOXELCOUNTRATION_DLG_H
#define CVOXELCOUNTRATION_DLG_H

#include "MaskList.h"
#include "MedipQT.h"
#include "Tabwindow.h"
#include "volumedata.h"
#include "CListWidget.h"

#include <QDialog>

/*
@brief  레이어 부피 비교 클래스
*/
class	CVoxelCountRatioDlg : public QDialog
{
public : 
	CVoxelCountRatioDlg(
							MedipQT* pParent = nullptr, 
							ROITab2* pRoiTab = nullptr,
							VOLUME_DATA* p_volume_data = nullptr
						);
	CVoxelCountRatioDlg(
							QString title, 
							MedipQT* pParent = nullptr, 
							ROITab2* pRoiTab = nullptr,
							VOLUME_DATA* p_volume_data = nullptr
							);
	~CVoxelCountRatioDlg();

protected :
	bool	eventFilter(QObject *watched, QEvent *event) override;

private slots:
	void OnCalculate();
	void OnCancel();

	void OnInsertList1();
	void OnRemoveList1();
	void OnInsertList2();
	void OnRemoveList2();

private :
	void init();
	void UpdateCtrlPosition();
	
	muint32 voxelcount(QList<muint32> & list_uid);

	MedipQT*		m_p_ParentWidget;
	ROITab2*		m_p_RoiTab;
	VOLUME_DATA*	m_p_VolumeData;

	QPushButton*	m_btnArrow1;
	//QPushButton*	m_btnArrow11;
	//QPushButton*	m_btnArrow21;
	QPushButton*	m_btnArrow2;

	QPushButton*	m_btnCalculate;
	QPushButton*	m_btnCancel;

	CListWidget*	m_ListOrigin;
	CListWidget*	m_ListSelection1;
	CListWidget*	m_ListSelection2;

	QLabel*			m_lbVoxelCountOrg;
	QLabel*			m_lbVoxelCount1;
	QLabel*			m_lbVoxelCount2;
	QLabel*			m_lbVoxelRatio;

	muint32			m_nVoxelCount[2];
};
#endif