#pragma once

#ifndef SMOOTH_DLG_H
#define SMOOTH_DLG_H

#include "define.h"
#include "defineMEDIP.h"
#include "MeshBaseDlg.h"
#include <QDialog>

#include "DataContext.h"

class QLineEdit;
class QSlider;

class SmoothDlg : public MeshBaseDlg
{
	Q_OBJECT

private:
	void		FinishProcess(); //220121 허 건 과장

	int			m_nSmoothMode;
	double		m_nOffset;
	double		m_nStrength;
	int			m_nMeshIndex;

	QComboBox * m_pComboBox;

public:
	SmoothDlg(DataContext* pDataContext, QWidget *parent = NULL);
	SmoothDlg(DataContext* pDataContext, QString _title, QWidget *parent = NULL);
	virtual ~SmoothDlg();	
	virtual void reject(bool bForce = false);

	int		getSmoothMode() { return m_nSmoothMode; }
	int		getStrength() { return m_nStrength; }
	float	getOffset() { return m_nOffset; }
	
	void	setSmoothMode(int val) { m_nSmoothMode = val; }
	void	setStrength(int val) { m_nStrength = val; }
	void	setOffset(float val) { 
		m_nOffset = val; 
	}
	void	setMesh(mip::MeshTopology * pMT) { 
		m_pMT = pMT;
	}
	void	setMeshIdx(int Meshidx) {
		m_nMeshIndex = Meshidx;
	}

	void	updateDialog();

private:
	void	initQtUI();	

private slots:
	void OnOK();
	void OnCancel();
	void OnTextChanged();
	void OnDepthSlideReleased(int);
	void OnComboChanged(int);
};
#endif