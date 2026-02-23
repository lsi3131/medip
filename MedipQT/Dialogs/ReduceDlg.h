#pragma once

#ifndef REDUCE_DLG_H
#define REDUCE_DLG_H

#include "define.h"
#include "defineMEDIP.h"
#include "MeshBaseDlg.h"

#include "DataContext.h"

class ReduceDlg : public MeshBaseDlg
{
	Q_OBJECT

private:
	int			m_nReduceMode;
	int			m_nPercentage;
	int			m_nTriCount;
	double		m_fMaxDeviation;
	bool		m_cPreserveBoundary;
	int			m_nMeshIndex;

	QCheckBox*	m_pCheck;
	std::vector<QLabel*>		listLB;

public:
	ReduceDlg(DataContext* pDataContext, QWidget *parent = NULL);
	ReduceDlg(DataContext* pDataContext, QString _title, QWidget *parent = NULL);
	virtual ~ReduceDlg();
	virtual void reject(bool bForce = false);

	int		getReduceMode() { return m_nReduceMode; }
	int		getPercentage() { return m_nPercentage; }
	int		getTriCount() { return m_nTriCount; }
	double	getMaxDeviation() { return m_fMaxDeviation; }
	
	void	setReduceMode(int val) { m_nReduceMode = val; }
	void	setPercentage(int val) { m_nPercentage = val; }
	void	setTriCount(int val) { m_nTriCount = val; }
	void	setMaxDeviation(double val) { m_fMaxDeviation = val; }
	void	setMesh(mip::MeshTopology * pMT) {
		m_pMT = pMT;
	}
	void	setMeshIdx(int Meshidx) {
		m_nMeshIndex = Meshidx;
	}

	void	hideFuncUI(); 
	void	hideFuncUI(QString option);
	void	showFuncUI(QString option);

	void	initOptionValue(int Reduce_Method = 0);
private:
	void	initQtUI();	
	double	calcMaxDeviation(mip::MeshTopology* pMT);	

private slots:
	void OnOK();
	void OnCancel();
	void OnTextChanged();
	void OnDepthSlideReleased(int);
	void OnComboChanged(int);
	void OnClickCheckBox();
};
#endif