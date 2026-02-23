#pragma once

#ifndef VISUALPRINTTAB_H
#define VISUALPRINTTAB_H

#include "CollapseWidget.h"
#include "graphics/MaskInfo.h"
#include "DataContext.h"

class VisualPrintDlg;

class VisualPrintTab : public CollapseWidget
{
	Q_OBJECT

public:
	explicit VisualPrintTab(QWidget* parent = NULL);

	//	QSlider		*m_slideSmooth;
	//	QLineEdit	*m_textSmooth;
	//	QCheckBox	*m_chkbox;

	void Init(DataContext* pDataContext, QWidget* pMainWindow);

public:
	//	bool		isExtract();
	void		slot_OnExportSTL();
	void		slot_OnExportSTL(QWidget* pParentWidget);
	void		Upload();
	void		OnVisualPrintWebLink();
	void		CreateVisualPrintDlg(DataContext* pDataContext, QWidget* parent);
	void		DestroyVisualPrintDlg();
	VisualPrintDlg* GetVisualPrintDlg();
	bool		CheckVisualPrintDlgCreated();	// VisualPrintDlg Create 여부 체크	
//	bool		VisualPrintDecreaseUsableCount(QString url);	// Visual Print 완료 시 Visual Print Count 차감 API (서버 연동)
	void		SetVisualPrintUsableCntLabel(int nUsableCnt);

	bool		getMeshlistCheckState(); //201110 허 건 대리

private:
	VisualPrintDlg* m_pVisualPrintDlg;
	//	VisualPrintDlg m_VisualPrintDlg;
//	QLabel		*m_pLabelVisualPrintUsableCnt;
	QLineEdit* m_pAvailableCnt;

	QRadioButton* m_RadioBtn_ROIList;
	QRadioButton* m_RadioBtn_MeshList;

	DataContext* m_pDataContext;

	QWidget*	 m_pMainWindow;
protected:
	bool		eventFilter(QObject* watched, QEvent* event) override;

private slots:
	void		slot_OnTextChanged(const QString& txt);
	void		slot_OnTextEditFinished();
	void		slot_OnSliderMoved(int val);

	//	void		OnCreatesurface();
	void		slot_OnVisualPrint();
	//	void		OnVisualPrint(QWidget *pParentWidget);
		//	void		OnVisualPrintWebLink();
		//	void		OnExportSTL();
	void		slot_OnExportOBJ();
	void		slot_OnExportVTK();
	//	bool        VisualPrintAvailableChk(QString url);			// Visual Print 가능 여부 체크 (가능 Count 체크)
																	//	bool		VisualPrintDecreaseUsableCount(QString url);	// Visual Print 완료 시 Visual Print Count 차감 API (서버 연동)
	void		slot_Single_ROI_InfoExport(QString* pFileName, MaskInfo* pInfo);					// Single ROIO Information File Export
	void		slot_Single_ROI_InfoExportObj(QString* pFileName, MaskInfo* pInfo);
	void		slot_Multi_ROI_InfoExport(QList<QTreeWidgetItem*>* pExplist, MaskInfo* pInfo);	// Multiple ROI Information File Export (STL File format)
	void        slot_Multi_ROI_InfoExportOBJ(QList<QTreeWidgetItem*>* pExplist, MaskInfo* pInfo);

	// 201005 허 건대리
	void		slot_UseMeshListChecked(bool _b_check); //< Mesh list 선택유무 체크박스 이벤트
	void		slot_UseROIListChecked(bool _b_check);  //< ROI list 선택유무 체크박스 이벤트

protected slots:
	virtual void slot_OnCollapse();

};
#endif

