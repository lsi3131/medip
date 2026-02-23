#pragma once

#ifndef VISUALPRINT_DLG_H
#define VISUALPRINT_DLG_H

#include <ppl.h>
#include <qdialog.h>
#include "windowManager.h"
#include "stringManager.h"
#include "Tabwindow.h"

#define	LINK_CLICKED	120

class VisualPrintTab;
class VisualPrintMeshTab;

class VisualPrintDlg : public QDialog
{
	Q_OBJECT

public:	
	VisualPrintDlg(DataContext* pDataContext, QWidget* parent = NULL);
	void Init();
	void InitCTPlaneAxial(QSlider *pSlideBar, WINDOW_TYPE windowType);
	void InitCTPlaneCoronal(QSlider *pSlideBar, WINDOW_TYPE windowType);
	void InitCTPlaneSagittal(QSlider *pSlideBar, WINDOW_TYPE windowType);
	void OnDepthSlideMoved(int act);
//	void OnDepthSlideChanged(int value);
	void OnDepthSlideChangedAxial(int value);
	void OnDepthSlideChangedCoronal(int value);
	void OnDepthSlideChangedSagittal(int value);
//	void processSlideZ(float dt, bool isSlider, bool isRotate);
	void processSlideZ(float dt, bool isSlider, bool isRotate, WINDOW_TYPE windowType);
	void OnDepthSlideReleased();
	~VisualPrintDlg();

//	VisualPrintMeshTab* GetVisualPrintMeshTab();
	void Update();
	bool GetVisualPrintMeshTabCreate();

	// 201019 허 건 대리
	// Sync Parameters(Smooth, Reduce)
	void SyncSmoothSliderMoved(int _act);
	void SyncSmoothSliderChanged(int _value);
	void SyncSmoothSliderReleased();
	void SyncReduceSliderMoved(int _act);
	void SyncReduceSliderChanged(int _value);
	void SyncReduceSliderReleased();

	// 201019 허 건 대리
	// 프로그레스 
	void BeginProgress();
	void EndProgress();
	void CancelProgress();
	void initProgress();
	void UpdateProgress(int _val);

	// 201102 허 건 대리
	// Undo / Redo
	void InitUndoRedo();

	void UpdateRender();

	static void UpdateProgress(float, void*);

	concurrency::combinable<float>	m_ProgressInterval;
	concurrency::combinable<float>	m_ProgressTotal;
	concurrency::combinable<float>	m_countMesh;
	concurrency::combinable<float>	m_pckMesh;

private:
	void VisualPrint();	//< Mesh Update event(smooth, reduce, ....)
	void VisualPrintWebLink();

	void UpdateParallel();
	void UpdateSingle();

	void EnableControl(bool _b_enable);

	void closeEvent(QCloseEvent *event);
	void resizeEvent(QResizeEvent *event); // 201021 허 건 대리
	bool eventFilter(QObject*, QEvent*) override;	

	QTabBar*			m_pTabBar;
	QHBoxLayout*		m_pHBoxLayout;

	VisualPrintTab*		m_pVisualPrintTab;
//	VisualPrintMeshTab *m_pVisualPrintMeshTab;

	bool				m_bVisualPrintMeshTabCreate;

	int					m_rValue;
	int					m_nAddWidgetRowCount; // Widget 추가 위치 Row Count;

	QSlider*			m_pSliderAxial;
	QSlider*			m_pSliderCoronal;
	QSlider*			m_pSliderSagital;
	float				Winsize;

	// 201019 허 건대리
	// Sync value
	QSlider*			m_pSliderSyncSmooth;
	QSlider*			m_pSliderSyncReduce;
	QLabel *			m_p_LabelSyncSmooth;
	QLabel *			m_p_LabelSyncReduce;

	QPushButton*		m_p_BtnWebLink;
	QPushButton*		m_p_BtnPreview;

	// 프로그레스바
	QProgressDialog*	m_progressDlg;

	MAINTAB_TYPE		m_prevType;

	// 201021 허 건 대리
	QSplitter*			m_Splliter;

	// 201027 허 건 대리
	// Undo Redo
	QAction*			m_pActUndo;
	QAction*			m_pActRedo;

	DataContext*		m_pDataContext;

private slots:	
	void		OnLinkClick();
	void		Onfinish();

	void		slot_updateFinished();
	void		slot_updateProgress(int);

	void		slot_updateRender(mip::MeshTopology*);

signals:
	void progress(int);
	void finished();

	void sig_updateFinished();
	void sig_updateProgress(int);

	void		sig_updateRender(mip::MeshTopology*);
};
#endif
