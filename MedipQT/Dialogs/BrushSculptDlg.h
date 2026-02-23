#pragma once

#ifndef BRUSH_SCULPT_DLG_H
#define BRUSH_SCULPT_DLG_H

#include "define.h"
#include "defineMEDIP.h"
#include "MeshBaseDlg.h"
#include <QDialog>

#include "DataContext.h"

class QLineEdit;
class QSlider;
class MEVolumeView;

class BrushSculptDlg : public MeshBaseDlg
{
	Q_OBJECT

public:
	BrushSculptDlg(DataContext* pDataContext, QWidget* parent = NULL);
	virtual ~BrushSculptDlg();

public:
	virtual void reject(bool bForce = false) override;
	virtual void keyReleaseEvent(QKeyEvent* e) override;

public:
	SCULPT_MODE getSculptMode();
	int getSelectMode();
	int getStrength();
	int getSize();
	bool isAccept();

	void setSculptMode(SCULPT_MODE val);
	void setSelectMode(int val);
	void setStrength(int val);
	void setSize(int val);

	void setVertTriCnt(int _vCnt, int _tCnt);

	void setBoundingBox(mip::AABB _box);
	mip::AABB getBoundingBox();
	void getVertTriCnt(int& _vCnt, int& _tCnt);

	void sliderUpDown(QString str, int option = 1);	// 1 - up, 0 down
	void updateSizeSlider(float scale);

private:
	void initQtUI();
	void tryBtnRoutine(SCULPT_MODE nSCULPT_MODE);

private slots:
	void OnOK();
	void OnCancel();
	void OnTextChanged();
	void OnDepthSlideReleased(int);
	void OnComboChanged(int);
	void OnPressed();
	void OnSculptReleased();

public:
	std::vector<QLineEdit*> listLE;

private:
	SCULPT_MODE m_nSculptMode;
	int m_nSelectMode;
	int m_nSize;
	int m_nStrength;
	int m_TTriCnt;
	int m_TVertCnt;
	mip::AABB m_box;

	bool m_Accept;
	bool m_bTaskFinished;

	//Mode
	QPushButton* m_pSurfaceBtn;
	QPushButton* m_pVolumeBtn;

	//Brush Type
	QPushButton* m_pSmoothBtn;
	QPushButton* m_pInflateBtn;
	QPushButton* m_pDeflateBtn;
	QPushButton* m_pReduceBtn;
	QPushButton* m_pMoveBtn;
	QPushButton* m_pDragBtn;
	QPushButton* m_pRefineBtn;
};
#endif
