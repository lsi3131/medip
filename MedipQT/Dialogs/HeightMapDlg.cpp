#include "stdafx.h"
#include "HeightMapDlg.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "Windows/windowManager.h"

HeightMapDlg::HeightMapDlg(float *p2DMap, int nRow, int nColumn, QImage colorTable, QWidget *parent) : QDialog(parent)
	, m_pMapInfo(nullptr)
{
	//m_strDlgName = strDlgName;
	///////////////////////////////////////////////

	m_pMapInfo = new CHeightMapInfo();
	// outputVolume 는 안쓰는건 -10, mask 데이터를 -1~1로 정규화
	// -4.5f = (-10.f + 1.f) * 0.5f;
	// -4.5f = (WIN_MANAGER->m_pRadiomicsResult[index3D]+1.f) * 0.5f;
	m_pMapInfo->setDrawData(p2DMap, nRow, nColumn, 0.f, 1.f, -4.5f);

	HeightMapView *pHMview_1 = new HeightMapView(this);
	pHMview_1->setDataObj(m_pMapInfo);
	pHMview_1->setColorTable(colorTable);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	QHBoxLayout *container = new QHBoxLayout;
	container->addWidget(pHMview_1);

	QWidget *w = new QWidget;
	w->setLayout(container);
	mainLayout->addWidget(w);
	setLayout(mainLayout);

	//setWindowTitle(m_strDlgName);
}

HeightMapDlg::~HeightMapDlg()
{
	SAFE_DELETE(m_pMapInfo);
}

void HeightMapDlg::setDepth(int nDepth)
{
	m_nDepth = nDepth;
}
