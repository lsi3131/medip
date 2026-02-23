#ifndef LAYER_HISTOGRAM_DLG_H
#define LAYER_HISTOGRAM_DLG_H

#include "define.h"
#include <qdialog.h>

#include "volumedata.h"

class QVBoxLayout;
class QPushButton;
class LayerHistogram;

class LayerHistogramDlg : public QDialog
{
	Q_OBJECT

public:
	LayerHistogramDlg(VOLUME_DATA* pVolumeData, int layerUID, QString _str, QWidget *parent = NULL);
	~LayerHistogramDlg();


private slots:
	void slot_OnLogHistogram();
	void OnCopy();
	void OnSave();
private:
//	QLabel * MaxMinHU;
//	QLabel * AvgHU;
//	QLabel * HUInterval;
//	QLabel * TotalCount;
	int uid;
	QString str;
	QVBoxLayout * layout;
	LayerHistogram *histogram;
	QPushButton *btnLog;

	VOLUME_DATA* m_pVolumeData;
};
#endif
