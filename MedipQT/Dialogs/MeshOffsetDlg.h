#pragma once

#ifndef MESHOFFSET_DLG_H
#define MESHOFFSET_DLG_H

#include "define.h"
#include "defineMEDIP.h"
#include "MeshBaseDlg.h"
#include <QDialog>
class QLineEdit;
class QSlider;


class MeshOffsetDlg : public MeshBaseDlg
{	
private:
	double		m_dOffsetDistance;
	double		m_nEdgeLength;

public:
	MeshOffsetDlg(DataContext* pDataContext, QWidget *parent = NULL);
	MeshOffsetDlg(DataContext* pDataContext, QString _title, QWidget *parent = NULL);
	virtual ~MeshOffsetDlg();
	virtual void reject(bool bForce = false);

	double	getOffsetDist() { return m_dOffsetDistance; }
	double	getEdgeLength() { return m_nEdgeLength; }

	void	setOffsetDist(double val) { m_dOffsetDistance = val; }
	void	setEdgeLength(double val) { m_nEdgeLength = val; }

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
