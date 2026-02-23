#pragma once

#ifndef HOLLOW_DLG_H
#define HOLLOW_DLG_H

#include "define.h"
#include "defineMEDIP.h"
#include "MeshBaseDlg.h"
#include "DataContext.h"

//Offset
//EdgeLength
class HollowDlg : public MeshBaseDlg
{
	Q_OBJECT

private:
	double		m_dOffsetDistance;
	double		m_nEdgeLength;
	int			m_nState; // 1 Apply

	enum enHollowState
	{
		HOLLOW_NONE,
		HOLLOW_APPLY
	};


public:
	HollowDlg(DataContext* pDataContext, QWidget *parent = NULL);
	HollowDlg(DataContext* pDataContext, QString _title, QWidget *parent = NULL);
	virtual ~HollowDlg();
	virtual void reject(bool bForce = false);

	double	getOffsetDist() { return m_dOffsetDistance; }
	double	getEdgeLength() { return m_nEdgeLength; }
	int		getState() { return m_nState; }
	
	void	setOffsetDist(double val) { m_dOffsetDistance = val; }
	void	setEdgeLength(double val) { m_nEdgeLength = val; }
	void	setState(int val) { m_nState = val; }

protected:
	void closeEvent(QCloseEvent *) override;

private:
	void	initQtUI();	
	double	calcEdgeLength(mip::MeshTopology* pMT);	

	void exitDialog();

private slots:
	void OnOK();
	void OnCancel();
	void OnTextChanged();
	void OnDepthSlideReleased(int);
	void OnComboChanged(int);
	void OnUpdateHollow();
};
#endif