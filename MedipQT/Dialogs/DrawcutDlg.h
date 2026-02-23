#pragma once

#ifndef DRAWCUT_DLG_H
#define DRAWCUT_DLG_H

#include "define.h"
#include <qdialog.h>
#include <qstring.h>
class QLineEdit;
class QSlider;

class OffsetDlg : public QDialog
{
	Q_OBJECT

public:
	OffsetDlg(QWidget *parent = NULL);
	OffsetDlg(QString _title,
		QString _label, double _base, double _step, double _min, double _max, QWidget *parent = NULL);
	virtual ~OffsetDlg();

	double	getDoubleVal() { return m_res; }
	int		getIntVal() { return m_res; }
	bool isAccept() { return m_Accept; }

private:
	double m_default;
	double m_minVal;
	double m_maxVal;
	double m_step;
private:
	double m_res;
	bool m_Accept;
	QLineEdit *m_lineEdit;
	QSlider *m_slider;

private slots:
	void OnDepthSlideReleased(int);
	void OnTextChanged();
	void OnOK();
	void OnCancel();
};
#endif