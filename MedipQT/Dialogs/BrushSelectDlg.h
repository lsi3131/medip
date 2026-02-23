#pragma once

#ifndef BRUSH_SELECT_DLG_H
#define BRUSH_SELECT_DLG_H

#include "define.h"
#include "defineMEDIP.h"
#include "MeshBaseDlg.h"
#include <QDialog>

#include "DataContext.h"

class QLineEdit;
class QSlider;


class BrushSelectDlg : public MeshBaseDlg
{
	Q_OBJECT

public:
	BrushSelectDlg(DataContext* pDataContext, QWidget *parent = NULL);
	BrushSelectDlg(DataContext* pDataContext, QString _title, QWidget *parent = NULL);
	virtual ~BrushSelectDlg();
	virtual void reject(bool bForce = false);

	int		getSculptMode() { return m_nSculptMode; }
	int		getSelectMode() { return m_nSelectMode; }
	int		getStrength() { return m_nStrength; }
	int		getSize() { return m_nSize; }
	bool isAccept() { return m_Accept; }

	void	setSculptMode(int val) { m_nSculptMode = val; }
	void	setSelectMode(int val) { m_nSelectMode = val; }
	void	setStrength(int val) { m_nStrength = val; }
	void	setSize(int val) { m_nSize = val; }

	void	sliderUpDown(QString str, int option = 1);	// 1 - up, 0 down

	void	setMesh(mip::MeshTopology * pMT) { m_pCurMesh = pMT; }
	void	setOldIdx(int old_idx) { m_old_pick_idx = old_idx; }
	void	clearSelection(mip::MeshTopology * pMT);
	void	setVertTriCnt(int _vCnt, int _tCnt) {
		m_TVertCnt = _vCnt;
		m_TTriCnt = _tCnt;
	};

	void	initUI();
	

	void	setBoundingBox(mip::AABB _box) { m_box = _box; }
	mip::AABB	getBoundingBox() { return m_box; }
	void	getVertTriCnt(int & _vCnt, int & _tCnt) {
		_vCnt = m_TVertCnt;
		_tCnt = m_TTriCnt;
	};

	std::vector<QLineEdit*> listLE;
	void updateColor();
	void updateSizeSlider(float scale);

	void showFuncBtn();
	void hideFuncBtn();

private:
	void		initQtUI();

	private slots:
	void OnOK();
	void OnCancel();
	void OnTextChanged();
	void OnClearFlag();
	void OnDepthSlideReleased(int);
	void OnComboChanged(int);
	void OnPressed();
	void keyReleaseEvent(QKeyEvent *e) override;

private:
	void	FinishProcess(); // 220121 허 건 과장

	int		m_nSculptMode;
	int		m_nSelectMode;
	int		m_nSize;
	int		m_nStrength;
	int		m_TTriCnt;
	int		m_TVertCnt;
	bool	m_bMoveDlg;
	mip::AABB	m_box;

	bool m_Accept;
	mip::MeshTopology * m_pCurMesh;
	int					m_old_pick_idx;

	//Mode
	QPushButton* m_pSurfaceBtn;
	QPushButton* m_pVolumeBtn;

	//Brush Type
	QPushButton* m_pSmoothBtn;
	QPushButton* m_pInflateBtn;
	QPushButton* m_pReduceBtn;
	QPushButton* m_pRemeshBtn;

	QLabel *lb_H_2;
};


#if USE_OLDDLG
class BrushSelectDlg : public QDialog
{
	Q_OBJECT
	
public:
	BrushSelectDlg(QWidget *parent = NULL);
	BrushSelectDlg(QString _title, QWidget *parent = NULL);
	virtual ~BrushSelectDlg();

	virtual void reject();

	int		getSculptMode() { return m_nSculptMode; }
	int		getSelectMode() { return m_nSelectMode; }
	int		getStrength() { return m_nStrength; }
	int		getSize() { return m_nSize; }
	bool isAccept() { return m_Accept; }
	
	void	setSculptMode(int val) { m_nSculptMode = val; }
	void	setSelectMode(int val) { m_nSelectMode = val; }
	void	setStrength(int val) { m_nStrength = val; }
	void	setSize(int val) { m_nSize = val; }

private:
	void		initQtUI();
	QLineEdit*	initLineEdit(QWidget *parent, QString objectName, QString initVal = QString("50"), int fixWidth = 0, int fixHeight = 0);
	QSlider*	initSlider(QWidget *parent, int Orientation = Qt::Horizontal,  int min = 0, int max = 100, int initVal = 50, int fixWidth = 0, int fixHeight = 0);


private slots:	
	void OnOK();
	void OnCancel();
	void OnTextChanged();
	void OnDepthSlideReleased(int);
	void OnComboChanged(int);


private:
	int		m_nSculptMode;
	int		m_nSelectMode;
	int		m_nSize;
	int		m_nStrength;

	bool m_Accept;

	std::vector<QLineEdit*> listLE;
	std::vector<QSlider*>	listSd;
	std::vector<QComboBox*>	listCB;

	enum
	{
		emLE_INT,
		emLE_DOUBLE
	};

};

#endif
#endif
