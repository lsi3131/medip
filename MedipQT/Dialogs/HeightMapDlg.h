#ifndef HEIGHTMAPDLG_H
#define HEIGHTMAPDLG_H

#include <QDialog>
#include <QOpenGLFunctions>
#include "Windows/HeightMapView.h"
#include "Windows/HeightMapInfo.h"

class HeightMapDlg : public QDialog
{
	Q_OBJECT

public:
	HeightMapDlg(float *p2DMap, int nRow, int nColumn, QImage colorTable, QWidget *parent =nullptr);
	~HeightMapDlg();

	void setDepth(int nDepth);

private:
	int		m_nDepth;
	//QString	m_strDlgName;
	CHeightMapInfo *m_pMapInfo;
	
};

#endif // HEIGHTMAPDLG_H
